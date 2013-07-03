/*****************************************************************************
 * Filename : Socket.cpp
 *
 * Project : R505A
 *
 * Description:
 * ------------
 *   The file contains definition of interface to transfer between client AP and 
 *   server which in router.
 *
 * Author: Lester (lester_hu@bandrich.com)
 ****************************************************************************/
#include "stdafx.h"
#include "Socket.h"

CSocketCtrl::CSocketCtrl(void)
{
	
}

CSocketCtrl::~CSocketCtrl(void)
{
	
}

void CSocketCtrl::Char2CString(char* source, CString* dest) 
{ 
	dest->SetString(CA2CT(source)); 
}

void CSocketCtrl::DbgPrint(char* chString, CString strData)  // Add this method to output debug string with CString type.
{
	CString strTemp;
	strTemp.SetString(L"");

	Char2CString(chString, &strTemp);

	if(!strData.IsEmpty())
		strTemp.Append(strData);

	OutputDebugString(strTemp);
	OutputDebugString(L"\n");
}

#if 0 // Not use
UINT CSocketCtrl::ConnectOnly(char* IpAddress)  // Initial and connect to server
{
	char cIP2soc[35];

	sprintf_s(cIP2soc, IpAddress);
	address.sin_addr.s_addr = inet_addr(cIP2soc);
	address.sin_family = AF_INET;
	address.sin_port = htons(PRE_AGREED_PORT);	

	//----------------------
	// Initialize Win sock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		OutputDebugString(_T("WSAStartup function failed!!\n"));
		iResult = -1;
	}
	else
		OutputDebugString(_T("WSAStartup function Success!!\n"));
	//----------------------
	// Create a SOCKET for connecting to server
	clisock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clisock == INVALID_SOCKET) {
		OutputDebugString(_T("Create socket error\n"));
		WSACleanup();
		iResult = -1;
	}
	else
		OutputDebugString(_T("Create socket Success!!\n"));

	//set the socket I/O mode
	//if iMode = 0, blocking is enabled
	//if iMode != 0, non-blocking mode is enabled
	u_long iMode = 1;
	ioctlsocket(clisock, FIONBIO, &iMode);

	/*Blocking, if fail return -1 directly*/	
	result = connect(clisock, (sockaddr*)&(address), sizeof(address));
	if (result == SOCKET_ERROR) {
		OutputDebugString(_T("Connect to server Fail!!\n"));
	}
	else
		OutputDebugString(_T("Connect to server Success!!\n"));

	return result;
}
#endif

UINT CSocketCtrl::Connect2Server(char* IpAddress)  // Initial and connect to server
{	
	char cIP2soc[35];

	sprintf_s(cIP2soc, IpAddress);
	address.sin_addr.s_addr = inet_addr(cIP2soc);
	address.sin_family = AF_INET;
	address.sin_port = htons(PRE_AGREED_PORT);	

	//----------------------
	// Initialize Win sock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		OutputDebugString(_T("WSAStartup function failed!!\n"));
		iResult = -1;
	}
	else
		OutputDebugString(_T("WSAStartup function Success!!\n"));
	//----------------------
	// Create a SOCKET for connecting to server
	clisock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clisock == INVALID_SOCKET) {
		OutputDebugString(_T("Create socket error\n"));
		WSACleanup();
		iResult = -1;
	}
	else
		OutputDebugString(_T("Create socket Success!!\n"));
#if 0
	// Set timeout
	int TimeOut=5000; //³]¸m¶W®É3¬í

	if(::setsockopt(clisock, SOL_SOCKET, SO_SNDTIMEO, (char *)&TimeOut, sizeof(TimeOut))==SOCKET_ERROR)  // Send timeout
	{
		OutputDebugString(_T("Set Send timeout Fail!!\n"));
		iResult = -1;
	}

	OutputDebugString(_T("Set Send timeout Success!!\n"));

	if(::setsockopt(clisock, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut))==SOCKET_ERROR)  // Receive timeout
	{
		OutputDebugString(_T("Set Receive timeout Fail!!\n"));
		iResult = -1;
	}

	OutputDebugString(_T("Set Receive timeout Success!!\n"));
#endif	

	/*Blocking, if fail return -1 directly*/	
	result = connect(clisock, (sockaddr*)&(address), sizeof(address));
	if (result == SOCKET_ERROR) {
		OutputDebugString(_T("Connect to server Fail!!\n"));
	}
	else
		OutputDebugString(_T("Connect to server Success!!\n"));

	return result;
}

BOOL CSocketCtrl::DisconnectSocket()
{
	int ret = closesocket(clisock);
	WSACleanup(); // Clean memory

	if(ret == SOCKET_ERROR)
		return FALSE;

	return TRUE;
}

UINT CSocketCtrl::SendCmd2Server(CString& strResp)
{
	int ret=0;

	ret = CmdTransfer();         // Socket send and receive

	CString str_recv;   // Get response from server and convert it from char array to cstring

	Char2CString(ch_recv, &strResp);

	return ret;
}

// Need to verify - lester
UINT CSocketCtrl::SendItemCount2Server(UINT count)
{
	int result = -1;

	if(count <= 0)
		return SOCKET_ERROR;
	else
		sprintf_s(ch_send, "%d", count);  // test item count modify here

	result = send(clisock, ch_send, sizeof(ch_send), 0);

	return result;
}

void CSocketCtrl::InitBuffer()
{
	memset(ch_send, 0, sizeof(ch_send)); 
	memset(ch_recv, 0, sizeof(ch_recv));
	bConnected = FALSE;
}

#if 0 // Not use
UINT CSocketCtrl::CmdTransferWithRetry(char* nIp)
{
	int ret=0, n=0, nCount=0;
	fd_set readfd, confd;
	struct timeval timeout, timeout1;
	timeout.tv_sec=10;
	timeout.tv_usec=0;
	timeout1.tv_sec=20;
	timeout1.tv_usec=0;
	CString szData;

	// 1. Send Request
	result = send(clisock, ch_send, sizeof(ch_send), 0);
	if(result == SOCKET_ERROR)
	{
		int nErrorCode = WSAGetLastError();
		SocErrorHandler(nErrorCode);		
		return SOC_ERROR_WRITE;
	}
	else
		OutputDebugString(_T("Send to server Success!!\n"));

	Sleep (100);    /* Delay */

	// 2. Disconnect Socket
	DisconnectSocket();

	Sleep(5000);

	// 3.  Wait until MAC write done
#if 1
	//while((bConnected != TRUE) && (nCount < 10))
	//{
	if(Connect2Server(nIp) == SOCKET_ERROR)				
	{		
		nCount++;
		Sleep(1500);
		OutputDebugString(_T("Phone Connect Fail\n"));
		//return SOCKET_ERROR;
		//if(nCount == 9)
		return SOCKET_ERROR;
		//continue;
	}
	//bConnected = TRUE;
	OutputDebugString(_T("Phone Connect\t\t\t\tSuccess\n"));		
	//}
#endif
	while(1)
	{
		FD_ZERO(&readfd);
		FD_SET(clisock, &readfd);
		ret = select(clisock, &readfd, NULL, NULL, &timeout);
		if((ret == SOCKET_ERROR) || (ret == SOCKET_TIMEOUT))  //Data error or timeout - 20111103 - lester_hu@bandrich
		{
			int nErrorCode = WSAGetLastError();
			SocErrorHandler(nErrorCode);
			OutputDebugString(_T("socket error in select\n"));
			closesocket(clisock);
			return SOC_ERROR_READ;
		}
		else					       //Data ready to read - 20111103 - lester_hu@bandrich
		{ 	
			if(FD_ISSET(clisock, &readfd))
			{
				n = recv(clisock, ch_recv, sizeof(ch_recv), 0);  // Read command from Client
				Char2CString(ch_recv, &szData);
				if(ch_recv[0] == NULL){
					return SOC_ERROR_READ;
				}
				OutputDebugString(szData);
				OutputDebugString(_T("\n"));
				return SOC_ERROR_SUCCESS;
			}
		}				
	}
	return SOC_ERROR_SUCCESS;
}
#endif

UINT CSocketCtrl::CmdTransfer()
{
	int ret=0, n=0;
	fd_set readfd;
	struct timeval timeout;
	timeout.tv_sec=10;
	timeout.tv_usec=0;
	CString szData;

	result = send(clisock, ch_send, sizeof(ch_send), 0);
	if(result == SOCKET_ERROR)
	{
		int nErrorCode = WSAGetLastError();
		SocErrorHandler(nErrorCode);		
		return SOC_ERROR_WRITE;
	}
	else
		OutputDebugString(_T("Send to server Success!!\n"));

	Sleep (100);    /* Delay */

	while(1)
	{
		FD_ZERO(&readfd);
		FD_SET(clisock, &readfd);
		ret = select(clisock, &readfd, NULL, NULL, &timeout);
		if((ret == SOCKET_ERROR) || (ret == SOCKET_TIMEOUT))  //Data error or timeout - 20111103 - lester_hu@bandrich
		{
			int nErrorCode = WSAGetLastError();
			SocErrorHandler(nErrorCode);
			OutputDebugString(_T("socket error in select\n"));
			closesocket(clisock);
			return SOC_ERROR_READ;
		}
		else					       //Data ready to read - 20111103 - lester_hu@bandrich
		{ 	
			if(FD_ISSET(clisock, &readfd))
			{
				n = recv(clisock, ch_recv, sizeof(ch_recv), 0);  // Read command from Client
				Char2CString(ch_recv, &szData);
				if(ch_recv[0] == NULL){
					return SOC_ERROR_READ;
				}

				OutputDebugString(szData);
				OutputDebugString(_T("\n"));
				return SOC_ERROR_SUCCESS;
			}
		}				
	}
	return SOC_ERROR_SUCCESS;
}

UINT CSocketCtrl::WriteToServer(char* DateBuffer, UINT length)
{
	result = send(clisock, DateBuffer, length, 0);
	if(result == SOCKET_ERROR)
	{
		int nErrorCode = WSAGetLastError();
		SocErrorHandler(nErrorCode);		
		return SOC_ERROR_WRITE;
	}
	else
		OutputDebugString(_T("Send to server Success!!\n"));
	
	return result;
}

UINT CSocketCtrl::ReadFromServer(char* DateBuffer, UINT length)
{
	int ret=0, n=0;
	fd_set readfd;
	struct timeval timeout;
	timeout.tv_sec=45;
	timeout.tv_usec=0;
	CString szData;

	while(1)
	{
		FD_ZERO(&readfd);
		FD_SET(clisock, &readfd);
		ret = select(clisock, &readfd, NULL, NULL, &timeout);
		if((ret == SOCKET_ERROR) || (ret == SOCKET_TIMEOUT))  //Data error or timeout - 20111103 - lester_hu@bandrich
		{
			int nErrorCode = WSAGetLastError();
			SocErrorHandler(nErrorCode);
			OutputDebugString(_T("socket error in select\n"));
			closesocket(clisock);
			return SOC_ERROR_READ;;
		}
		else					       //Data ready to read - 20111103 - lester_hu@bandrich
		{ 	
			if(FD_ISSET(clisock, &readfd))
			{
				n = recv(clisock, DateBuffer, length, 0);  // Read command from Client
				Char2CString(DateBuffer, &szData);
				OutputDebugString(szData);
				OutputDebugString(_T("\n"));	
				return SOC_ERROR_SUCCESS;
			}
		}				
	}
	return SOC_ERROR_SUCCESS;
}

//************************************
// Method:    SocErrorHandler
// Use: This method use to switch socket error and print out the log to dbgView
// Author : lester_hu@bandrich.com
//************************************
void CSocketCtrl::SocErrorHandler(int nError)
{
	if( WSAENOTCONN == nError )
		OutputDebugString(_T("The Socket is not connected!\n"));    //Error Handle
	else if( WSAESHUTDOWN == nError )
		OutputDebugString(_T("The socket has been shut down!\n"));  //Error Handle
	else if( WSAETIMEDOUT == nError )
		OutputDebugString(_T("The Connection has been dropped!\n"));			//Error Handle
	else if( WSAECONNRESET == nError )
		OutputDebugString(_T("The Socket will reset by remote side!\n"));			//Error Handle
	else
		OutputDebugString(_T("Fail!!\n"));
}

/*************************************************************
Cate : WiFi						
Item : Write MAC Address						
Request  : BR$FTM_MAC_WRITE=00:26:FA:00:00:01						
Response : $FTM_MAC_WRITE:0 (0-Success, 1-Fail, -1-Others Fail)
*************************************************************/
long CSocketCtrl::BR_FTM_MAC_WRITE(CStringA strMAC, CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_MAC_WRITE=%s", strMAC);

	int ret = SendCmd2Server(strResp);	

	Char2CString(ch_recv, &strResp);

	return ret;
}

/*************************************************************
Cate : WiFi						
Item : Read MAC Address						
Request  : BR$FTM_MAC_READ						
Response : $FTM_MAC_READ:00:26:FA:00:00:01
*************************************************************/
long CSocketCtrl::BR_FTM_MAC_READ(CString& strResp)
{
	InitBuffer();
	
	sprintf_s(ch_send, "BR$FTM_MAC_READ");

	int ret = SendCmd2Server(strResp);

	return ret;
}

/*************************************************************
Cate : WiFi						
Item : Write SSID					
Request  : BR$FTM_SSID_WRITE=R505A-0123456789					
Response : $FTM_SSID_WRITE:0 (0-Success, 1-Fail, -1-Others Fail)
*************************************************************/
long CSocketCtrl::BR_FTM_SSID_WRITE(CStringA strSSID, CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_SSID_WRITE=%s", strSSID);

	int ret = SendCmd2Server(strResp);	

	return ret;
}

/*************************************************************
Cate : WiFi						
Item : Read SSID					
Request  : BR$FTM_SSID_READ		
Response : $FTM_SSID_READ:"R505A-0123456789"
*************************************************************/
long CSocketCtrl::BR_FTM_SSID_READ(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_SSID_READ");

	int ret = SendCmd2Server(strResp);	

	return ret;
}

/*************************************************************
Cate : WiFi						
Item : Write WPA PassPhrase					
Request  : BR$FTM_WPA_PASSPHRASE_WRITE=LTE012345					
Response : $FTM_WPA_PASSPHRASE_WRITE:0 (0-Success, 1-Fail, -1-Others Fail)	
*************************************************************/				
long CSocketCtrl::BR_FTM_WPA_PASSPHRASE_WRITE(CStringA strPASSPHRASE, CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_WPA_PASSPHRASE_WRITE=%s", strPASSPHRASE);

	int ret = SendCmd2Server(strResp);	

	return ret;
}					

/*************************************************************
Cate : WiFi						
Item : Read WPA PassPhrase						
Request  : BR$FTM_WPA_PASSPHRASE_READ		
Response : $FTM_WPA_PASSPHRASE_READ:"LTE012345"
*************************************************************/
long CSocketCtrl::BR_FTM_WPA_PASSPHRASE_READ(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_WPA_PASSPHRASE_READ");

	int ret = SendCmd2Server(strResp);	

	return ret;
}

/*************************************************************
Cate : WiFi						
Item : Set WPA Enable Flag
Request  : BR$FTM_WPA_ENABLE_WRITE=1			
Response : $FTM_WPA_ENABLE_WRITE:0 (0-Success, 1-Fail, -1-Others Fail)
*************************************************************/				
long CSocketCtrl::BR_FTM_WPA_ENABLE_WRITE(CStringA strWPA, CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_WPA_ENABLE_WRITE=%s", strWPA);

	int ret = SendCmd2Server(strResp);	

	return ret;
}	

/*************************************************************
Cate : WiFi						
Item : Get WPA Enable Flag					
Request  : BR$FTM_WPA_ENABLE_READ
Response : $FTM_WPA_ENABLE_READ:1
*************************************************************/
long CSocketCtrl::BR_FTM_WPA_ENABLE_READ(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_WPA_ENABLE_READ");

	int ret = SendCmd2Server(strResp);	

	return ret;
}		

/*************************************************************
Cate : WiFi						
Item : Write Authentication Type
Request  : BR$FTM_AUTH_TYPE_WRITE=wpa		
Response : $FTM_AUTH_TYPE_WRITE:0 (0-Success, 1-Fail, -1-Others Fail)
*************************************************************/				
long CSocketCtrl::BR_FTM_AUTH_TYPE_WRITE(CStringA strAUTHTYPE, CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_AUTH_TYPE_WRITE=%s", strAUTHTYPE);

	int ret = SendCmd2Server(strResp);	
	return ret;
}

/*************************************************************
Cate : WiFi						
Item : Read Authentication Type				
Request  : BR$FTM_AUTH_TYPE_READ	
Response : $FTM_AUTH_TYPE_READ:"wpa"
*************************************************************/
long CSocketCtrl::BR_FTM_AUTH_TYPE_READ(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_AUTH_TYPE_READ");

	int ret = SendCmd2Server(strResp);	

	return ret;
}

/*************************************************************
Cate : WiFi						
Item : Backup Configuration File		
Request  : BR$FTM_BACKUP_DEFAULT	
Response : $FTM_BACKUP_DEFAULT:0 (0-Success, 1-Fail, -1-Others Fail)
*************************************************************/
long CSocketCtrl::BR_FTM_BACKUP_DEFAULT(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_BACKUP_DEFAULT");

	int ret = SendCmd2Server(strResp);

	return ret;
}
						
/*************************************************************
Cate : General						
Item : Get FTM Daemon Version		
Request  : BR$FTM_DAEMON_VERSION	
Response : $FTM_DAEMON_VERSION:Rev 1.1.1
*************************************************************/
long CSocketCtrl::BR_FTM_DAEMON_VERSION(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_DAEMON_VERSION");

	int ret = SendCmd2Server(strResp);
	
	DbgPrint("[E5XX-PCBA] BR_FTM_DAEMON_VERSION return ", strResp);

	return ret;
}
						
/*************************************************************
Cate : General						
Item : Get Router FW Version	
Request  : BR$FTM_PR_VERSION	
Response : $FTM_PR_VERSION:20100722_16_41_50
*************************************************************/
long CSocketCtrl::BR_FTM_PR_VERSION(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_PR_VERSION");

	int ret = SendCmd2Server(strResp);

	DbgPrint("[E5XX-PCBA] BR_FTM_PR_VERSION return ", strResp);

	return ret;
}
	
/*************************************************************
Cate : General						
Item : Set Operation Mode		
Request  : BR$FTM_SET_OPMODE=1(0-Normal mode, 1-FTM mode, 2-WiFi mode, 3-VOIP mode)
Response : $FTM_SET_OPMODE:0 (0-Success, 1-Fail, -1-Others Fail)
*************************************************************/
long CSocketCtrl::BR_FTM_SET_OPMODE(CStringA m_OPMode, CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_SET_OPMODE=%s", m_OPMode);

	int ret = SendCmd2Server(strResp);

	return ret;
}

/*************************************************************
Cate : General						
Item : Get Operation Mode
Request  : BR$FTM_GET_OPMODE	
Response : $FTM_GET_OPMODE:1 (0-Normal mode, 1-FTM mode, 2-WiFi mode, 3-VOIP mode)
*************************************************************/
long CSocketCtrl::BR_FTM_GET_OPMODE(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_GET_OPMODE");

	int ret = SendCmd2Server(strResp);

	return ret;
}
	
/*************************************************************
Cate : General						
Item : Force Reboot	
Request  : BR$FTM_REBOOT
Response : $FTM_REBOOT:0 (0-Success, 1-Fail, -1-Others Fail)
*************************************************************/
long CSocketCtrl::BR_FTM_REBOOT(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_REBOOT");

	int ret = SendCmd2Server(strResp);

	return ret;
}
			
/*************************************************************
Cate : NV Manager						
Item : Write Serial Number
Request  : BR$FTM_SN_WRITE=012345678912345
Response : $FTM_SN_WRITE:0 (0-Success, 1-Fail, -1-Others Fail)	
*************************************************************/
long CSocketCtrl::BR_FTM_SN_WRITE(CStringA strSN, CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_SN_WRITE=%s", strSN);

	int ret = SendCmd2Server(strResp);

	DbgPrint("[E5XX-PCBA] BR_FTM_SN_WRITE - ", (CString)strSN);

	return ret;
}

/*************************************************************
Cate : NV Manager						
Item : Serial Number Read	
Request  : BR$FTM_SN_READ	
Response : $FTM_SN_READ:012345678912345
*************************************************************/
long CSocketCtrl::BR_FTM_SN_READ(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_SN_READ");

	int ret = SendCmd2Server(strResp);

	DbgPrint("[E5XX-PCBA] BR_FTM_SN_READ return - ", strResp);

	return ret;
}

/*************************************************************
Cate : LTE Module				
Item : Read IMEI
Request  : BR$FTM_4G_IMEI	
Response : $FTM_4G_IMEI:357112020006195
*************************************************************/
long CSocketCtrl::BR_FTM_4G_IMEI(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_4G_IMEI");

	int ret = SendCmd2Server(strResp);

	DbgPrint("[E5XX-PCBA] BR_FTM_4G_IMEI return - ", strResp);

	return ret;
}

/*************************************************************
Cate : LTE Module				
Item : Read IMSI
Request  : BR$FTM_4G_IMSI
Response : $FTM_4G_IMSI:466014378600028
*************************************************************/
long CSocketCtrl::BR_FTM_4G_IMSI(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_4G_IMSI");

	int ret = SendCmd2Server(strResp);

	DbgPrint("[E5XX-PCBA] BR_FTM_4G_IMSI return - ", strResp);

	return ret;
}

/*************************************************************
Cate : LTE Module				
Item : Read Module FW Version
Request  : BR$FTM_4G_VERSION
Response : $FTM_4G_VERSION:R1.2.1
*************************************************************/
long CSocketCtrl::BR_FTM_4G_VERSION(CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_4G_VERSION");

	int ret = SendCmd2Server(strResp);

	DbgPrint("[E5XX-PCBA] BR_FTM_4G_VERSION return ", strResp);

	return ret;
}

/*************************************************************
Cate : LCD						
Item : BANDLUXE Display, All ON, All OFF, Grid ON, Grid OFF
Request  : BR$FTM_LCD_TEST=BANDLUXE
Response : $FTM_LCD_TEST:0 (0-Success, 1-Fail, -1-Others Fail)
*************************************************************/
long CSocketCtrl::BR_FTM_LCD_TEST(CStringA strLCD, CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_LCD_TEST=%s", strLCD);

	int ret = SendCmd2Server(strResp);

	return ret;
}					

/*************************************************************
Cate : LED						
Item : All ON, All OFF
Request  : BR$FTM_LED_TEST=ALLON
Response : $FTM_LCD_TEST:0 (0-Success, 1-Fail, -1-Others Fail)
*************************************************************/
long CSocketCtrl::BR_FTM_LED_TEST(CStringA strLED, CString& strResp)
{
	InitBuffer();

	sprintf_s(ch_send, "BR$FTM_LED_TEST=%s", strLED);

	int ret = SendCmd2Server(strResp);

	return ret;
}		
