///////////////////////////////////////////////////////////////////////////////
//  File:       SocketHandle.cpp
//  Version:    1.1
//
//  Author:     Ernest Laurentin
//  E-mail:     elaurentin@netzero.net
//
//  Implementation of the CSocketHandle and associated classes.
//
//  This code may be used in compiled form in any way you desire. This
//  file may be redistributed unmodified by any means PROVIDING it is
//  not sold for profit without the authors written consent, and
//  providing that this notice and the authors name and all copyright
//  notices remains intact.
//
//  This file is provided "AS IS" with no expressed or implied warranty.
//  The author accepts no liability for any damage/loss of business for the use of
//  class, indirect and consequential damages, even if the Author has been advised
//  of the possibility of such damages.
//
//  Version history
//
//  1.1 - Initial release.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <crtdbg.h>
#include "SocketHandle.h"

///////////////////////////////////////////////////////////////////////////////
// SockAddrIn Struct

///////////////////////////////////////////////////////////////////////////////
// Copy
SockAddrIn& SockAddrIn::Copy(const SockAddrIn& sin)
{
    memcpy(this, &sin, Size());
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// IsEqual
bool SockAddrIn::IsEqual(const SockAddrIn& sin) const
{
    // Is it Equal? - ignore 'sin_zero'
    return (memcmp(this, &sin, Size()-sizeof(sin_zero)) == 0);
}

///////////////////////////////////////////////////////////////////////////////
// CreateFrom
bool SockAddrIn::CreateFrom(LPCTSTR sAddr, LPCTSTR sService, int nFamily /*=AF_INET*/)
{
    Clear();
    sin_addr.s_addr = htonl( CSocketHandle::GetIPAddress(sAddr) );
    sin_port = htons( CSocketHandle::GetPortNumber( sService ) );
    sin_family = static_cast<short>(nFamily);

    return !IsNull();
}


///////////////////////////////////////////////////////////////////////////////
// CSocketHandle
CSocketHandle::CSocketHandle()
: m_hSocket(INVALID_SOCKET)
{
}

CSocketHandle::~CSocketHandle()
{
    Close();
}

///////////////////////////////////////////////////////////////////////////////
// GetSockName
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//              Get current address
// PARAMETERS:
//  SockAddrIn& saddr_in: address structure when connected
///////////////////////////////////////////////////////////////////////////////
bool CSocketHandle::GetSockName(SockAddrIn& saddr_in) const
{
    if (IsOpen())
    {
        int namelen = (int)saddr_in.Size();
        return (SOCKET_ERROR != getsockname(GetSocket(), (LPSOCKADDR)saddr_in, &namelen));
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// GetPeerName
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//              Get peer address (connected socket)
// PARAMETERS:
//  SockAddrIn& saddr_in: address structure when connected
///////////////////////////////////////////////////////////////////////////////
bool CSocketHandle::GetPeerName(SockAddrIn& saddr_in) const
{
    if (IsOpen())
    {
        int namelen = (int)saddr_in.Size();
        return (SOCKET_ERROR != getpeername(GetSocket(), (LPSOCKADDR)saddr_in, &namelen));  
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Close
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//              Close a socket connection
// PARAMETERS:
//  None
///////////////////////////////////////////////////////////////////////////////
void CSocketHandle::Close()
{
    if ( IsOpen() )
    {
        ShutdownConnection(m_hSocket);
        ::InterlockedExchange((LONG*)&m_hSocket, INVALID_SOCKET);
    }
}


///////////////////////////////////////////////////////////////////////////////
// AddMembership
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//              Add membership to a multicast address
// PARAMETERS:
//  LPCTSTR pszIPAddr: IP address for membership
//  LPCTSTR pszNIC: interface IP
///////////////////////////////////////////////////////////////////////////////
bool CSocketHandle::AddMembership(LPCTSTR pszIPAddr, LPCTSTR pszNIC)
{
    if ( IsOpen() )
    {
        int nType = 0;
        int nOptLen = sizeof(int);
        if ( SOCKET_ERROR != getsockopt(m_hSocket, SOL_SOCKET, SO_TYPE, (char*)&nType, &nOptLen))
        {
            if ( nType == SOCK_DGRAM )
            {
                // Setup interface for multicast traffic
                ULONG ulNIC = htonl( CSocketHandle::GetIPAddress( pszNIC ) );
                if ( SOCKET_ERROR != setsockopt(m_hSocket, IPPROTO_IP, IP_MULTICAST_IF,(char *) &ulNIC, sizeof(ulNIC)))
                {
                    int nTTL = SOCKHANDLE_TTL;
                    if ( SOCKET_ERROR != setsockopt(m_hSocket, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&nTTL, sizeof(nTTL)))
                    {
                        ip_mreq mreq;
                        mreq.imr_multiaddr.s_addr = htonl( CSocketHandle::GetIPAddress( pszIPAddr ) );
                        mreq.imr_interface.s_addr = ulNIC;
                        return ( SOCKET_ERROR != setsockopt(m_hSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)));
                    }
                }
            }
        }
        SetLastError( WSAGetLastError() );
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// DropMembership
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//              Remove membership from a multicast address
// PARAMETERS:
//  LPCTSTR pszIPAddr: IP address for membership
//  LPCTSTR pszNIC: interface IP
///////////////////////////////////////////////////////////////////////////////
bool CSocketHandle::DropMembership(LPCTSTR pszIPAddr, LPCTSTR pszNIC)
{
    if ( IsOpen() )
    {
        int nType = 0;
        int nOptLen = sizeof(int);
        if ( SOCKET_ERROR != getsockopt(m_hSocket, SOL_SOCKET, SO_TYPE, (char*)&nType, &nOptLen))
        {
            if ( nType == SOCK_DGRAM )
            {
                ip_mreq mreq;
                mreq.imr_multiaddr.s_addr = htonl( CSocketHandle::GetIPAddress( pszIPAddr ) );
                mreq.imr_interface.s_addr = htonl( CSocketHandle::GetIPAddress( pszNIC ) );
                return ( SOCKET_ERROR != setsockopt(m_hSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)));
            }
        }
        SetLastError( WSAGetLastError() );
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// CreateSocket
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//              This function creates a new socket for connection (SOCK_STREAM)
//              or an connectionless socket (SOCK_DGRAM).  A connectionless
//              socket should not call "accept()" since it cannot receive new
//              connection.  This is used as SERVER socket
// PARAMETERS:
//  LPCTSTR pszHostName: hostname (or NULL to select default adapter)
//  LPCTSTR pszServiceName: Service name or port number
//  int nFamily: address family to use (set to AF_INET)
//  int nType: type of socket to create (SOCK_STREAM, SOCK_DGRAM)
//  UINT uOptions: other options to use
///////////////////////////////////////////////////////////////////////////////
bool CSocketHandle::CreateSocket(LPCTSTR pszHostName, LPCTSTR pszServiceName,
                                int nFamily, int nType, UINT uOptions /* = 0 */)
{
    // Socket is already opened
    if ( IsOpen() )
        return false;

    // Create a Socket that is bound to a specific service provider
    // nFamily: (AF_INET)
    // nType: (SOCK_STREAM, SOCK_DGRAM)
#ifdef SOCKHANDLE_USE_OVERLAPPED
    SOCKET sock = WSASocket(nFamily, nType, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
    SOCKET sock = socket(nFamily, nType, IPPROTO_IP);
#endif
    if (INVALID_SOCKET != sock)
    {
        if (uOptions & SO_REUSEADDR)
        {
            // Inform Windows Sockets provider that a bind on a socket should not be disallowed
            // because the desired address is already in use by another socket
            BOOL optval = TRUE;
            if ( SOCKET_ERROR == setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof( BOOL ) ) )
            {
                SetLastError( WSAGetLastError() );
                closesocket( sock );
                return false;
            }
        }

        if (nType == SOCK_DGRAM)
        {
            if (uOptions & SO_BROADCAST)
            {
                // Inform Windows Sockets provider that broadcast messages are allowed
                BOOL optval = TRUE;
                if ( SOCKET_ERROR == setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char *) &optval, sizeof( BOOL ) ) )
                {
                    SetLastError( WSAGetLastError() );
                    closesocket( sock );
                    return false;
                }
            }
        }

        // Associate a local address with the socket
        SockAddrIn sockAddr;
        sockAddr.CreateFrom(pszHostName, pszServiceName, nFamily);

        if ( SOCKET_ERROR == bind(sock, (LPSOCKADDR)sockAddr, (int)sockAddr.Size()))
        {
            SetLastError( WSAGetLastError() );
            closesocket( sock );
            return false;
        }

        // Listen to the socket, only valid for connection socket (TCP)
        if (SOCK_STREAM == nType)
        {
            if ( SOCKET_ERROR == listen(sock, SOMAXCONN))
            {
                SetLastError( WSAGetLastError() );
                closesocket( sock );
                return false;
            }
        }

        // Success, now we may save this socket
        m_hSocket = sock;
    }

    return (INVALID_SOCKET != sock);
}


///////////////////////////////////////////////////////////////////////////////
// ConnectTo
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//              Establish connection with a server service or port
// PARAMETERS:
//  LPCTSTR pszHostName: hostname or address to connect (in .dot format)
//  LPCTSTR pszServiceName: Service name or port number
//  int nFamily: address family to use (set to AF_INET)
//  int nType: type of socket to create (SOCK_STREAM, SOCK_DGRAM)
///////////////////////////////////////////////////////////////////////////////
bool CSocketHandle::ConnectTo(LPCTSTR pszHostName, LPCTSTR pszServiceName,
                             int nFamily, int nType)
{
    // Socket is already opened
    if ( IsOpen() )
        return false;

    // Create a Socket that is bound to a specific service provider
    // nFamily: (AF_INET)
    // nType: (SOCK_STREAM, SOCK_DGRAM)
#ifdef SOCKHANDLE_USE_OVERLAPPED
    SOCKET sock = WSASocket(nFamily, nType, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
    SOCKET sock = socket(nFamily, nType, IPPROTO_IP);
#endif
    if (INVALID_SOCKET != sock)
    {
        // Associate a local address with the socket
        LPCTSTR pszHost = NULL;
        if ( pszHostName && _tcscmp(pszHostName, TEXT("localhost")) == 0 )
            pszHost = TEXT("localhost");
        SockAddrIn sockAddr;
        if (false == sockAddr.CreateFrom(pszHost, TEXT("0"), nFamily))
        {
            SetLastError( WSAGetLastError() );
            closesocket( sock );
            return false;
        }

        if ( SOCKET_ERROR == bind(sock, (LPSOCKADDR)sockAddr, sizeof(SOCKADDR_IN)))
        {
            SetLastError( WSAGetLastError() );
            closesocket( sock );
            return false;
        }

        // Now get destination address & port
        sockAddr.CreateFrom( pszHostName, pszServiceName );

        // try to connect - if fail, server not ready
        if (SOCKET_ERROR == connect( sock, (LPSOCKADDR)sockAddr, sizeof(SOCKADDR_IN)))
        {
            SetLastError( WSAGetLastError() );
            closesocket( sock );
            return false;
        }

        // Success, now we may save this socket
        m_hSocket = sock;
    }
    return (INVALID_SOCKET != sock);
}


///////////////////////////////////////////////////////////////////////////////
// Read
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//      Reads the Socket Communication
// PARAMETERS:
//      LPBYTE lpBuffer: buffer to place new data
//      DWORD dwSize: maximum size of buffer
//      DWORD dwTimeout: timeout to use in millisecond
///////////////////////////////////////////////////////////////////////////////
DWORD CSocketHandle::Read(LPBYTE lpBuffer, DWORD dwSize, LPSOCKADDR lpAddrIn,
                         DWORD dwTimeout)
{
    _ASSERTE( IsOpen() );
    _ASSERTE( lpBuffer != NULL );

    if (!IsOpen() || lpBuffer == NULL || dwSize < 1L)
        return (DWORD)-1L;

    fd_set  fdRead  = { 0 };
    TIMEVAL stTime;
    TIMEVAL *pstTime = NULL;

    if ( INFINITE != dwTimeout ) {
        stTime.tv_sec = dwTimeout/1000;
        stTime.tv_usec = (dwTimeout%1000)*1000;
        pstTime = &stTime;
    }

    SOCKET s = GetSocket();

    // Set Descriptor
    FD_SET( s, &fdRead );

    // Select function set read timeout
    DWORD dwBytesRead = 0L;
    int res = select((int)s, &fdRead, NULL, NULL, pstTime );
    if ( res > 0)
    {
        if (lpAddrIn)
        {
            // UDP
            int fromlen = sizeof(SOCKADDR_IN);
            res = recvfrom( s,(LPSTR) lpBuffer, dwSize, 0, lpAddrIn, &fromlen);
        }
        else
        {
            // TCP
            res = recv( s,(LPSTR) lpBuffer, dwSize, 0);
        }
    }
    if ( res == SOCKET_ERROR )
    {
        SetLastError( WSAGetLastError() );
    }
    dwBytesRead = (DWORD)((res >= 0)?(res) : (-1));

    return dwBytesRead;
}

///////////////////////////////////////////////////////////////////////////////
// ReadEx
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//      Asynchronous Read from the Socket Communication
// PARAMETERS:
//      LPBYTE lpBuffer: buffer to place new data
//      DWORD dwSize: maximum size of buffer
//      LPWSAOVERLAPPED lpOverlapped: Overlapped structure
//      LPWSACOMPLETIONROUTINE lpCompletionRoutine: Completion routine
///////////////////////////////////////////////////////////////////////////////
DWORD CSocketHandle::ReadEx(LPBYTE lpBuffer, DWORD dwSize, LPSOCKADDR lpAddrIn,
                         LPWSAOVERLAPPED lpOverlapped, LPWSACOMPLETIONROUTINE lpCompletionRoutine)
{
    _ASSERTE( IsOpen() );
    _ASSERTE( lpBuffer != NULL );

    if (!IsOpen() || lpBuffer == NULL || dwSize < 1L)
        return (DWORD)-1L;

    SOCKET s = GetSocket();
    // Send message to peer
    WSABUF wsabuf;
    wsabuf.buf = (char FAR*)lpBuffer;
    wsabuf.len = dwSize;

    // Select function set read timeout
    DWORD dwBytesRead = 0L;
    DWORD dwFlags = 0L;
    int res = 0;
    if (lpAddrIn)
    {
        // UDP
        int fromlen = sizeof(SOCKADDR_IN);
        res = WSARecvFrom( s, &wsabuf, 1, &dwBytesRead, &dwFlags, lpAddrIn, &fromlen, lpOverlapped, lpCompletionRoutine);
    }
    else
    {
        // TCP
        res = WSARecv( s, &wsabuf, 1, &dwBytesRead, &dwFlags, lpOverlapped, lpCompletionRoutine);
    }
    if ( res == SOCKET_ERROR )
    {
        res = WSAGetLastError();
        if ( res != WSA_IO_PENDING )
        {
            dwBytesRead = -1L;
            SetLastError( res );
        }
    }

    return dwBytesRead;
}


///////////////////////////////////////////////////////////////////////////////
// Write
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//      Writes data to the Socket Communication
// PARAMETERS:
//      const LPBYTE lpBuffer: data to write
//      DWORD dwCount: maximum characters to write
//      LPSOCKADDR* lpAddrIn: socket address (UDP mode)
//      DWORD dwTimeout: timeout to use in millisecond
///////////////////////////////////////////////////////////////////////////////
DWORD CSocketHandle::Write(const LPBYTE lpBuffer, DWORD dwCount,
                          const LPSOCKADDR lpAddrIn, DWORD dwTimeout)
{
    _ASSERTE( IsOpen() );
    _ASSERTE( NULL != lpBuffer );

    // Accept 0 bytes message
    if (!IsOpen() || NULL == lpBuffer)
        return -1L;

    fd_set  fdWrite  = { 0 };
    TIMEVAL stTime;
    TIMEVAL *pstTime = NULL;

    if ( INFINITE != dwTimeout ) {
        stTime.tv_sec = dwTimeout/1000;
        stTime.tv_usec = (dwTimeout%1000)*1000;
        pstTime = &stTime;
    }

    SOCKET s = GetSocket();

    // Set Descriptor
    FD_SET( s, &fdWrite );

    // Select function set write timeout
    DWORD dwBytesWritten = 0L;
    int res = select((int)s, NULL, &fdWrite, NULL, pstTime );
    if ( res > 0)
    {
        // Send message to peer
        if (lpAddrIn)
        {
            // UDP
            res = sendto( s, (LPCSTR)lpBuffer, dwCount, 0, lpAddrIn, sizeof(SOCKADDR_IN));
        }
        else
        {
            // TCP
            res = send( s, (LPCSTR)lpBuffer, dwCount, 0);
        }
    }
    if ( res == SOCKET_ERROR )
    {
        SetLastError( WSAGetLastError() );
    }
    dwBytesWritten = (DWORD)((res >= 0)?(res) : (-1));

    return dwBytesWritten;
}


///////////////////////////////////////////////////////////////////////////////
// WriteEx
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//      Asynchronous Write to the Socket Communication
// PARAMETERS:
//      const LPBYTE lpBuffer: data to write
//      DWORD dwCount: maximum characters to write
//      LPSOCKADDR lpAddrIn: socket address (UDP mode)
//      LPWSAOVERLAPPED lpOverlapped: Overlapped structure
//      LPWSACOMPLETIONROUTINE lpCompletionRoutine: Completion routine
///////////////////////////////////////////////////////////////////////////////
DWORD CSocketHandle::WriteEx(const LPBYTE lpBuffer, DWORD dwCount,
                            const LPSOCKADDR lpAddrIn,
                            LPWSAOVERLAPPED lpOverlapped, LPWSACOMPLETIONROUTINE lpCompletionRoutine)
{
    _ASSERTE( IsOpen() );
    _ASSERTE( NULL != lpBuffer );

    // Accept 0 bytes message
    if (!IsOpen() || NULL == lpBuffer)
        return -1L;

    SOCKET s = GetSocket();

    // Select function set write timeout
    DWORD dwBytesWritten = 0L;
    int res = 0;
    // Send message to peer
    WSABUF wsabuf;
    wsabuf.buf = (char FAR*) lpBuffer;
    wsabuf.len = dwCount;
    if (lpAddrIn)
    {
        // UDP
        res = WSASendTo( s, &wsabuf, 1, &dwBytesWritten, 0, lpAddrIn, sizeof(SOCKADDR_IN),
            lpOverlapped, lpCompletionRoutine);
    }
    else // TCP
        res = WSASend( s, &wsabuf, 1, &dwBytesWritten, 0, lpOverlapped, lpCompletionRoutine);

    if ( res == SOCKET_ERROR )
    {
        res = WSAGetLastError();
        if ( res != WSA_IO_PENDING )
        {
            dwBytesWritten = -1L;
            SetLastError( res );
        }
    }

    return dwBytesWritten;
}

///////////////////////////////////////////////////////////////////////////////
// GetTransferOverlappedResult
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//      Get overlapped result
// PARAMETERS:
//      LPWSAOVERLAPPED lpOverlapped: Overlapped structure
//      LPDWORD lpcbTransfer: Number of bytes tranferred
//      bool bWait: Force wait for the operation to complete
//      LPDWORD lpdwFlags: flags of the completion
///////////////////////////////////////////////////////////////////////////////
bool CSocketHandle::GetTransferOverlappedResult(LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer,
                                 bool bWait /*= true*/, LPDWORD lpdwFlags /*= NULL*/)
{
    _ASSERTE( IsOpen() );
    _ASSERTE( NULL != lpOverlapped );

    // Accept 0 bytes message
    if (!IsOpen() || NULL == lpOverlapped)
        return false;

    SOCKET s = GetSocket();
    DWORD dwFlags = 0;
    if ( lpdwFlags == NULL )
        lpdwFlags = &dwFlags;
    BOOL bRet = WSAGetOverlappedResult( s, lpOverlapped, lpcbTransfer, bWait, lpdwFlags );
    if ( !bRet )
    {
        SetLastError( WSAGetLastError() );
    }
    return (bRet != FALSE);
}


///////////////////////////////////////////////////////////////////////////////
// Utility functions

///////////////////////////////////////////////////////////////////////////////
// InitLibrary
bool CSocketHandle::InitLibrary(WORD wVersion)
{
    WSADATA WSAData = { 0 };
    return ( 0 == WSAStartup( wVersion, &WSAData ) );
}

///////////////////////////////////////////////////////////////////////////////
// ReleaseLibrary
bool CSocketHandle::ReleaseLibrary()
{
    return ( 0 == WSACleanup() ); 
}

///////////////////////////////////////////////////////////////////////////////
// WaitForConnection
SOCKET CSocketHandle::WaitForConnection(SOCKET sock)
{
    return accept(sock, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
// ShutdownConnection
bool CSocketHandle::ShutdownConnection(SOCKET sock)
{
    shutdown(sock, SD_BOTH);
    return ( 0 == closesocket( sock ));
}

static unsigned char chMinClassA_IP [] = { 1,   0,   0,   0   } ;
static unsigned char chMinClassD_IP [] = { 224, 0,   0,   0   } ;
static unsigned char chMaxClassD_IP [] = { 239, 255, 255, 255 } ;

///////////////////////////////////////////////////////////////////////////////
// IsUnicastIP
bool CSocketHandle::IsUnicastIP( ULONG ulAddr )
{
    return (((unsigned char *) & ulAddr) [0] >= chMinClassA_IP [0] &&
            ((unsigned char *) & ulAddr) [0] < chMinClassD_IP [0]) ;
}

///////////////////////////////////////////////////////////////////////////////
// IsMulticastIP
bool CSocketHandle::IsMulticastIP( ULONG ulAddr )
{
    return (((unsigned char *) & ulAddr) [0] >= chMinClassD_IP [0] &&
            ((unsigned char *) & ulAddr) [0] <= chMaxClassD_IP [0]) ;
}

///////////////////////////////////////////////////////////////////////////////
// FormatIP
bool CSocketHandle::FormatIP(LPTSTR pszIPAddr, UINT nSize, ULONG ulAddr, bool bFmtHost)
{
    if ( pszIPAddr )
    {
        if ( bFmtHost )
            ulAddr = htonl( ulAddr );
        // Create Address string
        return (SUCCEEDED(StringCchPrintf(pszIPAddr, nSize, _T("%u.%u.%u.%u"),
                            (UINT)(((PBYTE) &ulAddr)[0]),
                            (UINT)(((PBYTE) &ulAddr)[1]),
                            (UINT)(((PBYTE) &ulAddr)[2]),
                            (UINT)(((PBYTE) &ulAddr)[3]))));
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// GetPortNumber
USHORT CSocketHandle::GetPortNumber( LPCTSTR pszServiceName )
{
    LPSERVENT   lpservent;
    USHORT      nPort = 0;

    if ( _istdigit( pszServiceName[0] ) ) {
        nPort = (USHORT) _ttoi( pszServiceName );
    }
    else {
#ifdef _UNICODE
        char pstrService[HOSTNAME_SIZE] = { 0 };
        WideCharToMultiByte(CP_ACP, 0, pszServiceName, -1, pstrService, sizeof(pstrService), NULL, NULL );
#else
        LPCTSTR pstrService = pszServiceName;
#endif
        // Convert network byte order to host byte order
        if ( (lpservent = getservbyname( pstrService, NULL )) != NULL )
            nPort = ntohs( lpservent->s_port );
    }

    return nPort;
}

///////////////////////////////////////////////////////////////////////////////
// GetIPAddress
ULONG CSocketHandle::GetIPAddress( LPCTSTR pszHostName )
{
    LPHOSTENT   lphostent;
    ULONG       uAddr = INADDR_NONE;
    TCHAR       szLocal[HOSTNAME_SIZE] = { 0 };

    // if no name specified, get local
    if ( NULL == pszHostName || !pszHostName[0])
    {
        GetLocalName(szLocal, HOSTNAME_SIZE);
        pszHostName = szLocal;
    }

#ifdef _UNICODE
    char pszHost[HOSTNAME_SIZE] = { 0 };
    WideCharToMultiByte(CP_ACP, 0, pszHostName, -1, pszHost, sizeof(pszHost), NULL, NULL );
#else
    LPCTSTR pszHost = pszHostName;
#endif

    // Check for an Internet Protocol dotted address string
    uAddr = inet_addr( pszHost );

    if ( (INADDR_NONE == uAddr) && (strcmp( pszHost, "255.255.255.255" )) )
    {
        // It's not an address, then try to resolve it as a hostname
        if ( (lphostent = gethostbyname( pszHost )) != NULL )
            uAddr = *((ULONG *) lphostent->h_addr_list[0]);
    }
    
    return ntohl( uAddr );
}

///////////////////////////////////////////////////////////////////////////////
// GetLocalName
bool CSocketHandle::GetLocalName(LPTSTR pszName, UINT nSize)
{
    if (pszName != NULL && nSize > 0)
    {
        char szHost[HOSTNAME_SIZE] = { 0 };

        // get host name, if fail, SetLastError is set
        if (SOCKET_ERROR != gethostname(szHost, sizeof(szHost)))
        {
            struct hostent* hp;
            hp = gethostbyname(szHost);
            if (hp != NULL) {
                ::StringCbCopyA(szHost, HOSTNAME_SIZE, hp->h_name);
            }

            // check if user provide enough buffer
            size_t cbLength = 0;
            ::StringCbLengthA(szHost, HOSTNAME_SIZE, &cbLength);
            if ( cbLength > nSize )
            {
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return false;
            }

            // Unicode conversion
#ifdef _UNICODE
            return (0 != MultiByteToWideChar(CP_ACP, 0, szHost, -1, pszName, nSize ));
#else
            ::StringCbCopyA(pszName, nSize, szHost);
            return true;
#endif
        }
    }
    else
        SetLastError(ERROR_INVALID_PARAMETER);
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// GetLocalAddress
bool CSocketHandle::GetLocalAddress(LPTSTR pszAddress, UINT nSize)
{
    // Get computer local address
    if (pszAddress != NULL && nSize > 0)
    {
        char szHost[HOSTNAME_SIZE] = { 0 };

        // get host name, if fail, SetLastError is called
        if (SOCKET_ERROR != gethostname(szHost, sizeof(szHost)))
        {
            struct hostent* hp;
            hp = gethostbyname(szHost);
            if (hp != NULL && hp->h_addr_list[0] != NULL)
            {
                // IPv4: Address is four bytes (32-bit)
                if ( hp->h_length < 4)
                    return false;

                // Convert address to . format
                szHost[0] = 0;

                // Create Address string
                ::StringCbPrintfA(szHost, HOSTNAME_SIZE, "%u.%u.%u.%u",
                    (UINT)(((PBYTE) hp->h_addr_list[0])[0]),
                    (UINT)(((PBYTE) hp->h_addr_list[0])[1]),
                    (UINT)(((PBYTE) hp->h_addr_list[0])[2]),
                    (UINT)(((PBYTE) hp->h_addr_list[0])[3]));

                // check if user provide enough buffer
                size_t cbLength = 0;
                ::StringCbLengthA(szHost, HOSTNAME_SIZE, &cbLength);
                if ( cbLength > nSize)
                {
                    SetLastError(ERROR_INSUFFICIENT_BUFFER);
                    return false;
                }

                // Unicode conversion
#ifdef _UNICODE
                return (0 != MultiByteToWideChar(CP_ACP, 0, szHost, -1, pszAddress, nSize ));
#else
                ::StringCbCopyA(pszAddress, nSize, szHost);
                return true;
#endif
            }
        }
    }
    else
        SetLastError(ERROR_INVALID_PARAMETER);
    return false;
}
