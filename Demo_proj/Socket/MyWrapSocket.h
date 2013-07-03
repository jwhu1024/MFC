/*****************************************************************************
 * Filename : Socket.h
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

#define PRE_AGREED_PORT		7568
#define SOCKET_TIMEOUT		0
#define FW_UPGRADE          99
#define PACKET_SIZE 64

typedef enum
{
	SOC_ERROR_SUCCESS=0,
	SOC_ERROR_READ,
	SOC_ERROR_WRITE,
} E_SOCKET_STATUS;

class CSocketCtrl
{

public:
	CSocketCtrl(void);
	virtual ~CSocketCtrl(void);

private:
	
public:
	SOCKET clisock;

public:
	UINT	Connect2Server(char* IpAddress);
	UINT	ConnectOnly(char* IpAddress);
	UINT	SendCmd2Server(CString& strResp);
	UINT    SendItemCount2Server(UINT count);
	UINT    CmdTransferWithRetry(char* nIp);
	UINT	CmdTransfer();
	UINT    WriteToServer(char* DateBuffer, UINT length=0);
	UINT	ReadFromServer(char* DateBuffer, UINT length);
	BOOL	DisconnectSocket();
	void	InitBuffer();
	void	Char2CString(char* source, CString* dest);
	void    SocErrorHandler(int nError);
	
public:
	char    ch_recv[PACKET_SIZE];	// Receive buffer
	char    ch_send[PACKET_SIZE];	// Send	buffer
	BOOL	bConnected;
	//CString g_recvdata;
	UINT    result;
	struct sockaddr_in address;

	// Useful method
public:
	void DbgPrint(char* chString, CString strData = "");

public:
	long	BR_FTM_MAC_READ(CString& strResp);
	long	BR_FTM_MAC_WRITE(CStringA strMAC, CString& strResp);
	
	long	BR_FTM_SSID_READ(CString& strResp);
	long	BR_FTM_SSID_WRITE(CStringA strSSID, CString& strResp);

	long	BR_FTM_WPA_PASSPHRASE_READ(CString& strResp);
	long	BR_FTM_WPA_PASSPHRASE_WRITE(CStringA strPASSPHRASE, CString& strResp);

	long	BR_FTM_WPA_ENABLE_READ(CString& strResp);
	long	BR_FTM_WPA_ENABLE_WRITE(CStringA strWPA, CString& strResp);

	long	BR_FTM_AUTH_TYPE_READ(CString& strResp);
	long	BR_FTM_AUTH_TYPE_WRITE(CStringA strAUTHTYPE, CString& strResp);

	long	BR_FTM_BACKUP_DEFAULT(CString& strResp);
	long	BR_FTM_DAEMON_VERSION(CString& strResp);
	long	BR_FTM_PR_VERSION(CString& strResp);
	long	BR_FTM_REBOOT(CString& strResp);

	long	BR_FTM_GET_OPMODE(CString& strResp);
	long	BR_FTM_SET_OPMODE(CStringA m_OPMode, CString& strResp);

	long	BR_FTM_SN_READ(CString& strResp);
	long	BR_FTM_SN_WRITE(CStringA strSN, CString& strResp);

	long	BR_FTM_4G_IMEI(CString& strResp);
	long	BR_FTM_4G_IMSI(CString& strResp);
	long	BR_FTM_4G_VERSION(CString& strResp);

	long	BR_FTM_LCD_TEST(CStringA strLCD, CString& strResp);
	long	BR_FTM_LED_TEST(CStringA strLED, CString& strResp);
};
