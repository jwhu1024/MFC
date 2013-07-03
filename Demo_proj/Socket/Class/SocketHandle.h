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
///////////////////////////////////////////////////////////////////////////////
#ifndef SOCKETHANDLE_H
#define SOCKETHANDLE_H

#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32")

#ifndef BUFFER_SIZE
#define BUFFER_SIZE     MAX_PATH
#endif
#ifndef SOCKHANDLE_TTL
#define SOCKHANDLE_TTL 5
#endif
#define HOSTNAME_SIZE   MAX_PATH
#define STRING_LENGTH   40

struct SockAddrIn : public SOCKADDR_IN {
public:
    SockAddrIn() { Clear(); }
    SockAddrIn(const SockAddrIn& sin) { Copy( sin ); }
    ~SockAddrIn() { }
    SockAddrIn& Copy(const SockAddrIn& sin);
    void    Clear() { memset(this, 0, sizeof(SOCKADDR_IN)); }
    bool    IsEqual(const SockAddrIn& sin) const;
    bool    IsNull() const { return ((sin_addr.s_addr==0L)&&(sin_port==0)); }
    ULONG   GetIPAddr() const { return sin_addr.s_addr; }
    short   GetPort() const { return sin_port; }
    bool    CreateFrom(LPCTSTR sAddr, LPCTSTR sService, int nFamily = AF_INET);
    SockAddrIn& operator=(const SockAddrIn& sin) { return Copy( sin ); }
    bool    operator==(const SockAddrIn& sin) const { return IsEqual( sin ); }
    bool    operator!=(const SockAddrIn& sin) const { return !IsEqual( sin ); }
    operator LPSOCKADDR() { return (LPSOCKADDR)(this); }
    size_t  Size() const { return sizeof(SOCKADDR_IN); }
    void    SetAddr(SOCKADDR_IN* psin) { memcpy(this, psin, Size()); }
};

typedef LPWSAOVERLAPPED_COMPLETION_ROUTINE LPWSACOMPLETIONROUTINE;
class CSocketHandle
{
public:
    CSocketHandle();
    ~CSocketHandle();

    /// Check if socket is opened
    bool IsOpen() const { return ( INVALID_SOCKET != m_hSocket ); }
    /// Get SOCKET handle
    SOCKET GetSocket() const { return m_hSocket; }
    /// Get HANDLE
    HANDLE GetHandle() const { return (HANDLE) m_hSocket; }
    /// Create a Socket - Server side
    bool CreateSocket(LPCTSTR pszHost, LPCTSTR pszServiceName, int nFamily, int nType, UINT uOptions = 0);
    /// Create a socket, connect to (Client side)
    bool ConnectTo(LPCTSTR pszHostName, LPCTSTR pszServiceName, int nFamily, int nType);
    /// Close Socket
    void Close();

    // Data function
    /// Read from socket
    DWORD Read(LPBYTE lpBuffer, DWORD dwSize, LPSOCKADDR lpAddrIn = NULL, DWORD dwTimeout = INFINITE);
    DWORD ReadEx(LPBYTE lpBuffer, DWORD dwSize, LPSOCKADDR lpAddrIn,
                LPWSAOVERLAPPED lpOverlapped, LPWSACOMPLETIONROUTINE lpCompletionRoutine);
    /// Write to a destination socket
    DWORD Write(const LPBYTE lpBuffer, DWORD dwCount, const LPSOCKADDR lpAddrIn = NULL, DWORD dwTimeout = INFINITE);
    DWORD WriteEx(const LPBYTE lpBuffer, DWORD dwCount, const LPSOCKADDR lpAddrIn,
                LPWSAOVERLAPPED lpOverlapped, LPWSACOMPLETIONROUTINE lpCompletionRoutine);

    /// Get Overlapped result
    bool GetTransferOverlappedResult(LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, bool bWait = true, LPDWORD lpdwFlags = 0);

    /// Get Socket name - address
    bool GetSockName(SockAddrIn& saddr_in) const;
    /// Get Peer Socket name - address
    bool GetPeerName(SockAddrIn& saddr_in) const;
    /// Register to multicast address
    bool AddMembership(LPCTSTR pszIPAddr, LPCTSTR pszNIC);
    /// Unregister from a multicast address
    bool DropMembership(LPCTSTR pszIPAddr, LPCTSTR pszNIC);

    // Utility functions
    /// Initialize Winsock library
    static bool InitLibrary(WORD wVersion);
    /// Release Winsock library
    static bool ReleaseLibrary();
    /// Wait for a new connection
    static SOCKET WaitForConnection(SOCKET sock);
    /// Shutdown a connection
    static bool ShutdownConnection(SOCKET sock);
    /// Check if IP address is unicast (network order)
    static bool IsUnicastIP( ULONG ulAddr );
    /// Check if IP address is multicast (network order)
    static bool IsMulticastIP( ULONG ulAddr );
    /// Format IP address to string
    static bool FormatIP(LPTSTR pszIPAddr, UINT nSize, ULONG ulAddr, bool bFmtHost);
    /// Get service port number
    static USHORT GetPortNumber( LPCTSTR pszServiceName );
    /// Get IP address of a host
    static ULONG GetIPAddress( LPCTSTR pszHostName );
    /// Get current localname for this machine
    static bool GetLocalName(LPTSTR pszName, UINT nSize);
    /// Get current (default) IP address for this machine
    static bool GetLocalAddress(LPTSTR pszAddress, UINT nSize);

// CSocketHandle - data
protected:
    SOCKET      m_hSocket;  // socket handle
};

#endif //SOCKETHANDLE_H