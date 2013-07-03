/************************************************************************* 
* Serial port enumeration routines
*
* The EnumSerialPort function will populate an array of SSerInfo structs,
* each of which contains information about one serial port present in
* the system. Note that this code must be linked with setupapi.lib,
* which is included with the Win32 SDK.
*
************************************************************************/

// For MFC
#include "StdAfx.h"
#include "EnumPort.h"

// The next 3 includes are needed for serial port enumeration



#define MAX_DISK_EJECT_TIMES            60     // The max number of times to eject CD-ROM

CString   m_strDiagPort;
CString cd_dbcc_name = L"";
CString cd_szDevId = L"";

BOOL m_bCdromExist = FALSE;

const std::wstring VID_LIST[] = {

#ifdef FEATURE_FOR_BANDLUXE
	L"VID_1A8D", // BandRich's USB VID
#endif

#ifdef FEATURE_FOR_DLINK
	L"VID_1186", // D-Link's USB VID
#endif

#ifdef FEATURE_FOR_TELEWELL
	L"VID_1C15", // TeleWell's USB VID
#endif

#ifdef FEATURE_FOR_NOBRAND
	L"VID_05C6", // Qualcomm's USB VID
#endif

	L"VID_NULL"  // Null VID
};

const std::wstring VEN_LIST[][2] = {

#ifdef FEATURE_FOR_BANDLUXE
	{ L"VEN_BANDLUXE", L"REV_1.01"},// BandRich's USBSTOR VEN
	{ L"VEN_BANDLUXE", L"REV_1.10"},
	{ L"VEN_BANDRICH", L"REV_2.01"},
#endif

#ifdef FEATURE_FOR_DLINK
	{ L"VEN_D-LINK", L"REV_1.01"},// D-Link's USBSTOR VEN
#endif

#ifdef FEATURE_FOR_TELEWELL
	{ L"VEN_TELEWELL", L"REV_1.01"},// TeleWell's USBSTOR VEN
#endif

#ifdef FEATURE_FOR_NOBRAND 
	{ L"VEN_HSPA", L"REV_1.01"},// Nobrand USBSTOR VEN
#endif

	{ L"VEN_NULL", L"REV_NULL" } // Null VID
};

typedef struct {
	SCSI_PASS_THROUGH Spt;
	BYTE Sense[18];
	BYTE Data[36];
} SCSI_CMD_INFO;


// Struct used when enumerating the available serial ports
// Holds information about an individual serial port.
struct SSerInfo {
    SSerInfo() : bUsbDevice(FALSE) {}
    CString strDevPath;           // Device path for use with CreateFile()
    CString strPortName;          // Simple name (i.e. COM1)
    CString strFriendlyName;      // Full name to be displayed to a user
    BOOL bUsbDevice;              // Provided through a USB connection?
    CString strPortDesc;          // friendly name without the COMx
};

//---------------------------------------------------------------
// Helpers for enumerating the available serial ports.
// These throw a CString on failure, describing the nature of
// the error that occurred.

void EnumPortsWdm(CArray<SSerInfo,SSerInfo&> &asi);

// Routine for enumerating the available serial ports. Throws a CString on
// failure, describing the error that occurred. If bIgnoreBusyPorts is TRUE,
// ports that can't be opened for read/write access are not included.
void EnumSerialPorts(CArray<SSerInfo,SSerInfo&> &asi, BOOL bIgnoreBusyPorts=TRUE);


/** Find a given device info by enumeration device info set
* @param hDevInfo [IN] A handle open by SetupDiGetClassDevs
* @param szDevId [IN] the device instance name, eg. USB\Vid_04e8&Pid_503b\0002F9A9828E0F06
* @param spDevInfoData [OUT] the device info
* @return TRUE if found, FALSE if not found or error
*/
BOOL FindDevice(HDEVINFO& hDevInfo, CString& szDevId, SP_DEVINFO_DATA& spDevInfoData);

// Eject Bandrich CD ROM (not support W2K)
void EjectCDROM(CString dbcc_name);

void RemoveCompositMastorage(LPCTSTR szDevIdString);



BOOL detect_diagnostic_port()
{
	CArray<SSerInfo,SSerInfo&> asi;

	// Populate the list of serial ports.
	EnumSerialPorts(asi,FALSE);

	for (int ii=0; ii<asi.GetSize(); ii++) 
	{
		if ( (asi[ii].strFriendlyName.Find(_T(BR_DIAG_PORT_NAME)) > -1) ) /* For Diagnostic first resolve */
		{
			return TRUE;
		}
	}

	return FALSE;
}


// Routine for enumerating the available serial ports.
// Throws a CString on failure, describing the error that
// occurred. If bIgnoreBusyPorts is TRUE, ports that can't
// be opened for read/write access are not included.

void EnumSerialPorts(CArray<SSerInfo,SSerInfo&> &asi, BOOL bIgnoreBusyPorts)
{
	// Clear the output array
	asi.RemoveAll();

	// Win2k and later support a standard API for
	// enumerating hardware devices.
	EnumPortsWdm(asi);

	for (int ii=asi.GetSize()-1; ii >= 0; ii--)
	{
		int iStart = 0, iEnd = 0;
		CString tcom = _T("");

		if (asi[ii].strPortName.IsEmpty())
		{
			iStart = asi[ii].strFriendlyName.Find(_T("("));
			iEnd   = asi[ii].strFriendlyName.Find(_T(")"));

			if ( (iStart > 0) &&  (iEnd > iStart) )
			{
				tcom  = asi[ii].strFriendlyName.Mid(iStart+1,(iEnd-iStart-1));
			}
		}
		else
		{
			tcom = asi[ii].strPortName;
		}

		if (tcom.IsEmpty())
		{
			asi.RemoveAt(ii);
			continue;
		}

		if (bIgnoreBusyPorts) 
		{
			// Only display ports that can be opened for read/write
			HANDLE hCom = CreateFile(_T("\\\\.\\") + tcom,
									GENERIC_READ | GENERIC_WRITE,
									0,    /* comm devices must be opened w/exclusive-access */
									NULL, /* no security attrs */
									OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
									0,    /* not overlapped I/O */
									NULL  /* hTemplate must be NULL for comm devices */
									);
			if (hCom == INVALID_HANDLE_VALUE) 
			{
				// It can't be opened; remove it.
				asi.RemoveAt(ii);
				ii--;
				continue;
			}
			else 
			{
				// It can be opened! Close it and add it to the list
				::CloseHandle(hCom);
			}
		}

		if (asi[ii].strPortName.IsEmpty())
		{
			asi[ii].strPortName = tcom;
		}
	}

}

void EnumPortsWdm(CArray<SSerInfo,SSerInfo&> &asi)
{
	// get the set of network adapters on the system
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_CLASS_COMPORT,
											NULL,
											NULL,
											DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);

	// Enumerate through all devices in the set
	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo,i,&DeviceInfoData); i++)
	{
		DWORD regDataType = 0;
		TCHAR MyBuf[256] = {0};
		DWORD MyBufSize = 256;

		// get the device capabilities
		SetupDiGetDeviceRegistryProperty(hDevInfo,
										&DeviceInfoData,
										SPDRP_ENUMERATOR_NAME,
										&regDataType,
										(PBYTE)&MyBuf,
										MyBufSize,
										&MyBufSize);

		// we are only interested in removable devices
		if (wcsncmp(MyBuf,_T("USB"), 3) == 0)
		{
			TCHAR fname[256] = {0};
			DWORD bufSize = 256;

			// get the device friendly name
			SetupDiGetDeviceRegistryProperty(hDevInfo,
											&DeviceInfoData,
											SPDRP_FRIENDLYNAME,
											&regDataType,
											(PBYTE)fname,
											bufSize,
											&bufSize);

			if (_tcsstr(fname,_T(BR_DIAG_PORT_NAME)))
			{
				// Add an entry to the array
				TCHAR desc[256] = {0};
				SSerInfo si;
				si.strFriendlyName = fname;

				// if no device friendly name, get the device description
				// get the device friendly name
				SetupDiGetDeviceRegistryProperty(
				hDevInfo,
				&DeviceInfoData,
				SPDRP_DEVICEDESC,
				&regDataType,
				(PBYTE)desc,
				bufSize,
				&bufSize);

				si.strPortDesc = desc;

				HKEY hKey = SetupDiOpenDevRegKey(
				hDevInfo, 
				&DeviceInfoData, 
				DICS_FLAG_GLOBAL,
				NULL,
				DIREG_DEV,KEY_READ);

				TCHAR szPortName[MAX_PATH];
				if (hKey)
				{
					DWORD dwType = REG_SZ;
					DWORD dwReqSize = sizeof(szPortName);

					// Query for portname
					if (RegQueryValueEx(hKey,_T("PortName"), 0, &dwType, (LPBYTE)&szPortName, &dwReqSize) == ERROR_SUCCESS)
					{
						si.strPortName = szPortName;
					}
				}

				asi.Add(si);				
			}
		}
	}
	//  Cleanup
	SetupDiDestroyDeviceInfoList(hDevInfo);
}

int process_bandrich_device_change(UINT wParam, LPCWSTR dbcc_name)
{
	ASSERT(lstrlen(dbcc_name) > 4);
	CString tmp;

	tmp.Format(L"%d",lstrlen(dbcc_name));
	//myCMlog.writeLog(L"",L"pDevInf->dbcc_name n = ",tmp);
	CString szDevId;
	szDevId= dbcc_name+4;
	//myCMlog.writeLog(L"",L"szDevId = ",szDevId);

	int idx = szDevId.ReverseFind(_T('#'));
	ASSERT( -1 != idx );
	szDevId.Truncate(idx);
	szDevId.Replace(_T('#'), _T('\\'));
	szDevId.MakeUpper();

	CString szClass;
	idx = szDevId.Find(_T('\\'));
	ASSERT(-1 != idx );
	szClass = szDevId.Left(idx);

	CString szVid;
	int jdx = szDevId.Find(_T('&'));
	ASSERT(-1 != jdx );
	szVid = szDevId.Mid(idx+1,jdx-idx-1);

	if ( (szClass == L"USBSTOR") && (szVid == L"CDROM") )  // check CD-ROM
	{  
		//CWaitCursor cursor;
		idx = szDevId.Find(_T('&'), jdx+1);
		ASSERT(-1 != idx );
		szVid = szDevId.Mid(jdx+1,idx-jdx-1);

		std::wstring _tmpCompare(L"VEN_NULL");

		for (int i=0; VEN_LIST[i][0].c_str() != _tmpCompare; i++)
		{
			if (VEN_LIST[i][0].c_str() == szVid)  // BR CD-ROM
			{
				// save related parameters for win2000
				cd_dbcc_name = dbcc_name;
				//Added by Shawn
				GUID  *guid = (GUID *)&GUID_DEVCLASS_CDROM;
				HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
				//szClass.Format(_T("USBSTOR\\CDROM&%s&PROD_CDROM&REV_1.01"),szVid);
				if (!szClass.IsEmpty())
				{
					hDevInfo = SetupDiGetClassDevs(guid, szClass, NULL, DIGCF_PRESENT);
				}
				else
				{
					hDevInfo = SetupDiGetClassDevs(guid, NULL, NULL, DIGCF_PRESENT);
				}

				if ( INVALID_HANDLE_VALUE == hDevInfo)
				{
					_RPTF0(_CRT_WARN, "CReflashToolDlg::OnUsbStorArrival - Wrong Device Class ID...\n"); 
					//break;
					return -1;
				}
				SP_DEVINFO_DATA spDevInfoData;
				ZeroMemory(&spDevInfoData, sizeof(SP_DEVINFO_DATA));
				spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
				for (DWORD j = 0; SetupDiEnumDeviceInfo(hDevInfo, j, &spDevInfoData); j++)
				{
					TCHAR szBuff[MAX_PATH];
					DWORD nSize = 0;
					if ( !SetupDiGetDeviceInstanceId(hDevInfo, &spDevInfoData, szBuff, sizeof(szBuff), &nSize) ) 
					{
						continue;
					} 

					CString tmp;
					tmp = szBuff;

					DEVINST DevInstParent = 0;
					CM_Get_Parent(&DevInstParent, spDevInfoData.DevInst, 0); 
					TCHAR szDeviceIdString[MAX_PATH];
					CM_Get_Device_ID(DevInstParent, szDeviceIdString, MAX_PATH, 0);
					// Check the device matches to the target Vender Name.
					CString str = szDeviceIdString;

					BOOL bMatched = FALSE;
					std::wstring _tmpCompare(L"VEN_NULL");

					//for (int i = 0; VEN_LIST[i] != L"VEN_NULL"; i ++)
					for (int i=0; VEN_LIST[i][0].c_str() != _tmpCompare; i++)
					{

						if ( _tcsstr( tmp, VEN_LIST[i][0].c_str())  && _tcsstr(tmp, VEN_LIST[i][1].c_str()) )
						{
							bMatched = TRUE;
							break;
						}
					}

					if (bMatched == FALSE)
					{
						continue;
					}

					//不是PID_1000就不做退光碟
					if( str.Find(_T("VID_1A8D")) > -1 && str.Find(_T("PID_1000") > 0) )
					{
						if(wParam == DBT_DEVICEARRIVAL)
						{
							cd_szDevId = szDevId;
							m_bCdromExist = TRUE;
						}
						else
						{
							cd_szDevId = _T("");
							m_bCdromExist = FALSE;
						}
						return 1;
					}
				}
			}
		}
		return -1;
	}
	else  // check Diag port
	{
		DWORD dwFlag = DBT_DEVICEARRIVAL != wParam ? DIGCF_ALLCLASSES : (DIGCF_ALLCLASSES | DIGCF_PRESENT);		
		HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL,szClass,NULL,dwFlag);
		if( INVALID_HANDLE_VALUE == hDevInfo ) 
		{
			return -2;
		}

		SP_DEVINFO_DATA spDevInfoData;
		if ( FindDevice(hDevInfo, szDevId, spDevInfoData) ) 
		{
			// OK, device found
			DWORD DataT ;
			TCHAR buf[MAX_PATH];
			DWORD nSize = 0;

			// get Friendly Name or Device Description
			if ( SetupDiGetDeviceRegistryProperty(  hDevInfo, 
													&spDevInfoData, 
													SPDRP_FRIENDLYNAME, 
													&DataT, (PBYTE)buf, 
													sizeof(buf), 
													&nSize) ) 
			{
				CString strBuff = buf;              

				// Release Device Handler
				SetupDiDestroyDeviceInfoList(hDevInfo);

				if( strBuff.Find(_T(BR_DIAG_PORT_NAME)) > -1 )
				{
					return 2;  // BR Diag Port
				}
				else
				{
					return -3; // Not BR Device
				}
			} 
			else if ( SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, 
				SPDRP_DEVICEDESC, &DataT, (PBYTE)buf, sizeof(buf), &nSize) ) 
			{

			}
			else 
			{
				// Release Device Handler
				SetupDiDestroyDeviceInfoList(hDevInfo);

				lstrcpy(buf, _T("Unknown"));
				return -4; // Not BR Device
			}
		} // if ( FindDevice(hDevInfo, szDevId, spDevInfoData) ) 

		// Release Device Handler
		SetupDiDestroyDeviceInfoList(hDevInfo);

	} // if ( (szClass == L"USBSTOR") && (szVid == L"CDROM") )


	return -5;  // Not BR Device
}

BOOL FindDevice(HDEVINFO& hDevInfo, CString& szDevId, SP_DEVINFO_DATA& spDevInfoData)
{
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for(int i=0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++) 
	{
		DWORD nSize=0 ;
		TCHAR buf[MAX_PATH];

		if ( !SetupDiGetDeviceInstanceId(hDevInfo, &spDevInfoData, buf, sizeof(buf), &nSize) ) 
		{
//			TRACE(CString("SetupDiGetDeviceInstanceId(): ") 
//				+ _com_error(GetLastError()).ErrorMessage());
			return FALSE;
		}

		if ( szDevId == buf ) 
		{
			// OK, device found
			return TRUE;
		}
	}

	return FALSE;
}

void eject_bandrich_cdrom(LPCSTR lpszTargetDeviceMode)
{
	// check length of VEN_NAME
	m_TargetDeviceMode = lpszTargetDeviceMode;
	//AfxMessageBox(m_TargetDeviceMode);
	CString szClass = _T("");
	//if ( cd_dbcc_name.IsEmpty() || cd_szDevId.IsEmpty() )
	//{
	//	cd_szDevId.Format(_T("USBSTOR\\CDROM&%s&PROD_CDROM&REV_1.01"), VEN_LIST[0].c_str());
	//	//cd_szDevId = L"USBSTOR\\CDROM&VEN_BANDLUXE&PROD_CDROM&REV_1.01";
	//	szClass=cd_szDevId;
	//	m_bCdromExist = TRUE;
	//}
	//else
	if ( ! cd_szDevId.IsEmpty() )
	{
		int idx = cd_szDevId.ReverseFind(_T('\\'));
		if (idx != -1)
		{
			szClass = cd_szDevId.Left(idx);
			idx = szClass.ReverseFind(_T('\\'));
			if (idx != -1)
			{
				szClass.Truncate(idx);
				AfxMessageBox(L"szClass:");
				AfxMessageBox(szClass);
			}
		}
	}

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);

	GUID  *guid = (GUID *)&GUID_DEVCLASS_CDROM; // 0x36fc9e60L, 0xc465, 0x11cf, 0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );

	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	//szClass.Format(_T("USBSTOR\\CDROM&%s&PROD_CDROM&REV_1.01"),szVid);
	if (!szClass.IsEmpty())
	{
		hDevInfo = SetupDiGetClassDevs(guid, szClass, NULL, DIGCF_PRESENT);
	}
	else
	{
		hDevInfo = SetupDiGetClassDevs(guid, NULL, NULL, DIGCF_PRESENT);
	}

	if ( INVALID_HANDLE_VALUE == hDevInfo)
	{
		_RPTF0(_CRT_WARN, "CReflashToolDlg::OnUsbStorArrival - Wrong Device Class ID...\n"); 
		//break;
		return;
	}
	SP_DEVINFO_DATA spDevInfoData;
	ZeroMemory(&spDevInfoData, sizeof(SP_DEVINFO_DATA));
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (DWORD j = 0; SetupDiEnumDeviceInfo(hDevInfo, j, &spDevInfoData); j++)
	{
		TCHAR szBuff[MAX_PATH];
		DWORD nSize = 0;
		if ( !SetupDiGetDeviceInstanceId(hDevInfo, &spDevInfoData, szBuff, sizeof(szBuff), &nSize) ) 
		{
			continue;
		} 
		CString tmp;
		tmp = szBuff;

		if ( tmp.Find(cd_szDevId) < 0 ) // Skip non-active device.
		{
			continue;
		}

		DEVINST DevInstParent = 0;
		CM_Get_Parent(&DevInstParent, spDevInfoData.DevInst, 0); 
		TCHAR szDeviceIdString[MAX_PATH];
		CM_Get_Device_ID(DevInstParent, szDeviceIdString, MAX_PATH, 0);
		// Check the device matches to the target Vender Name.
		CString str = szDeviceIdString;
		BOOL bMatched = FALSE;
		std::wstring _tmpCompare(L"VEN_NULL");

		//for (int i = 0; VEN_LIST[i] != L"VEN_NULL"; i ++)
		for (int i=0; VEN_LIST[i][0].c_str() != _tmpCompare; i++)
		{

			if ( _tcsstr( tmp, VEN_LIST[i][0].c_str())  && _tcsstr(tmp, VEN_LIST[i][1].c_str()) )
			{
				bMatched = TRUE;
				break;
			}
		}

		if (bMatched == FALSE)
		{
			continue;
		}

		if (_tcsstr(szDeviceIdString, L"&MI_")) 
		{
			RemoveCompositMastorage(szDeviceIdString);
		}
		else
		{
			if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )//windows2000
			{
				ZeroMemory(szDeviceIdString, sizeof(szDeviceIdString));
				CM_Get_Device_ID(spDevInfoData.DevInst, szDeviceIdString, MAX_PATH, 0);
				cd_dbcc_name = szDeviceIdString;
				cd_dbcc_name.Replace(_T('\\'), _T('#'));
				cd_dbcc_name.AppendFormat(_T("#{53f56308-b6bf-11d0-94f2-00a0c91efb8b}"));
				cd_dbcc_name.Insert(0, _T("\\\\?\\"));

				OutputDebugString(cd_dbcc_name.GetBuffer(0));

				HANDLE hUsbDev = CreateFile(cd_dbcc_name, 
											GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
											NULL, 
											OPEN_EXISTING, 
											0, 
											NULL);
				if (hUsbDev)
				{

					SCSI_CMD_INFO Info = {0};
					DWORD ReturnLen;
					BOOL  bDevIO = FALSE;
					Info.Spt.Length = sizeof(Info.Spt);
					Info.Spt.SenseInfoLength = sizeof(Info.Sense);
					Info.Spt.DataTransferLength = sizeof(Info.Data);
					Info.Spt.SenseInfoOffset = offsetof(SCSI_CMD_INFO, Sense);
					Info.Spt.DataBufferOffset = offsetof(SCSI_CMD_INFO, Data);
					Info.Spt.TimeOutValue = 30;
					Info.Spt.DataIn = SCSI_IOCTL_DATA_IN;
					Info.Spt.CdbLength = 6;
					Info.Spt.Cdb[0] = 0x03; // sense unit command
					Info.Spt.Cdb[1] = 0x00;
					Info.Spt.Cdb[2] = 0x00; //0x00;
					Info.Spt.Cdb[3] = 0x00;
					Info.Spt.Cdb[4] = 0x12;
					Info.Spt.Cdb[5] = 0x00;

					for (int n = 0; n < 6; n ++)
					{
						DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);
					}

					Info.Spt.DataTransferLength = 0;
					Info.Spt.DataIn = SCSI_IOCTL_DATA_UNSPECIFIED;
					Info.Spt.Cdb[0] = 0x00; // Test Unit
					Info.Spt.Cdb[2] = 0x00;
					Info.Spt.Cdb[4] = 0x00;
					DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);

					Info.Spt.Cdb[0] = 0x1E; // Prevent Eject Media Allow
					Info.Spt.Cdb[4] = 0x00;
					DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);

					Info.Spt.Cdb[0] = 0x1B; // Test Unit
					Info.Spt.Cdb[4] = 0x02;
					DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);

					CloseHandle(hUsbDev);
				}

				// Do Driver Unload Process
				TCHAR VetoName[MAX_PATH] = {0};
				PNP_VETO_TYPE VetoType = PNP_VetoTypeUnknown;
				DEVINST DevInstChild = 0;
				// Do Removable Drive Eject Operation.
				if ( (CM_Get_Child(&DevInstChild, DevInstParent, 0) == CR_SUCCESS) && 
					(CM_Request_Device_Eject(DevInstParent, &VetoType, VetoName, MAX_PATH, 0) != CR_SUCCESS) )
				{
					int nTry = 20;
					while ( (nTry > 0) && (CM_Get_Child(&DevInstChild, DevInstParent, 0) == CR_SUCCESS) )
					{
						//if (CM_Query_And_Remove_SubTree(DevInstParent, NULL, NULL, 0, CM_REMOVE_NO_RESTART) == CR_SUCCESS)
						if (CM_Query_And_Remove_SubTree(DevInstParent, &VetoType, VetoName, MAX_PATH, CM_REMOVE_NO_RESTART) == CR_SUCCESS)
						{
							break;
						}
						nTry --;
						Sleep(500);
					}
				}

			}
			else
			{
				if (cd_dbcc_name.IsEmpty())
				{
					CM_Get_Device_ID(spDevInfoData.DevInst, szDeviceIdString, MAX_PATH, 0);
					//{53f56308-b6bf-11d0-94f2-00a0c91efb8b} is GUID_DEVINTERFACE_CDROM
					cd_dbcc_name.Format(_T("%s#{53f56308-b6bf-11d0-94f2-00a0c91efb8b}"),szDeviceIdString);
					cd_dbcc_name.Replace(_T('\\'), _T('#'));
					cd_dbcc_name.Insert(0, _T("\\\\?\\"));
				}				
				EjectCDROM(cd_dbcc_name);
				AfxMessageBox(L"cd_dbcc_name:");
				AfxMessageBox(cd_dbcc_name);
			}
		}
		break;
	}
	// Release Device Handler
	SetupDiDestroyDeviceInfoList(hDevInfo);
}




static void EjectCDROM(CString dbcc_name)
{
	HANDLE hUsbDev;
	BOOL  bDevIO = FALSE;

	hUsbDev = CreateFile(dbcc_name, 
						GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL, OPEN_EXISTING, 
						0, 
						NULL);
	if (hUsbDev)
	{

		SCSI_CMD_INFO Info = {0x00};
		DWORD ReturnLen;

		if(TRUE/* || !m_TargetDeviceMode.IsEmpty()*/)
		{
			char mode = 0xFF;
			TCHAR* p;

			if(m_TargetDeviceMode.GetLength() > 0)
			{
				mode = (char)wcstol(m_TargetDeviceMode, &p, 10);
			}

			if(mode == 0x00)
			{				
				*p = '\0';
				mode = 0xFF;
			}
		
			Info.Spt.Length = sizeof(SCSI_PASS_THROUGH);
			Info.Spt.SenseInfoLength = sizeof(Info.Sense);
			Info.Spt.DataTransferLength = 0x00;
			Info.Spt.SenseInfoOffset = offsetof(SCSI_CMD_INFO, Sense);
			Info.Spt.DataBufferOffset = offsetof(SCSI_CMD_INFO, Data);
			Info.Spt.TimeOutValue = 30;
			Info.Spt.DataIn = SCSI_IOCTL_DATA_UNSPECIFIED;
			Info.Spt.CdbLength = 6;

			if( *p == '\0' )
			{	
				Info.Spt.Cdb[0] = 0x00;
				Info.Spt.Cdb[1] = 'B';
				Info.Spt.Cdb[2] = 'R';
				Info.Spt.Cdb[3] = ':';
				Info.Spt.Cdb[4] = mode;
				Info.Spt.Cdb[5] = 0x00;		
			}
			else
			{				
				Info.Spt.Cdb[0] = 0x00;
				for(int i=1 ; i<=m_TargetDeviceMode.GetLength() && i<=4 ; i++)
				{
					Info.Spt.Cdb[i] = m_TargetDeviceMode.GetAt(i);				
				}
				Info.Spt.Cdb[5] = 0x00;				
			}

			DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);		
			DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);

			Info.Spt.Cdb[0] = 0x00;
			Info.Spt.Cdb[1] = 0x00;
			Info.Spt.Cdb[2] = 0x00;
			Info.Spt.Cdb[3] = 0x00;
			Info.Spt.Cdb[4] = 0x00;
			// Prevent Media Remove Allow command
			Info.Spt.Cdb[0] = 0x1E;
			Info.Spt.Cdb[4] = 0x00;
			DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);


			Info.Spt.Cdb[0] = 0x1B; // Start/Stop Unit
			Info.Spt.Cdb[4] = 0x02;
			DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);
		}
		//else
		//{
		//	Info.Spt.Length = sizeof(Info.Spt);
		//	Info.Spt.SenseInfoLength = sizeof(Info.Sense);
		//	Info.Spt.DataTransferLength = sizeof(Info.Data);
		//	Info.Spt.SenseInfoOffset = offsetof(SCSI_CMD_INFO, Sense);
		//	Info.Spt.DataBufferOffset = offsetof(SCSI_CMD_INFO, Data);
		//	Info.Spt.TimeOutValue = 30;
		//	Info.Spt.DataIn = SCSI_IOCTL_DATA_IN;
		//	Info.Spt.CdbLength = 6;

		//	Info.Spt.Cdb[0] = 0x03; // sense unit command
		//	Info.Spt.Cdb[1] = 0x00;
		//	Info.Spt.Cdb[2] = 0x00;
		//	Info.Spt.Cdb[3] = 0x00;
		//	Info.Spt.Cdb[4] = 0x12;
		//	for (int i = 0; i < 6; i++)
		//	{
		//		DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);
		//	}

		//	// Test unit command
		//	Info.Spt.Cdb[0] = 0x00;
		//	Info.Spt.Cdb[2] = 0x00;
		//	Info.Spt.Cdb[4] = 0x00;
		//	Info.Spt.DataIn = SCSI_IOCTL_DATA_UNSPECIFIED;
		//	Info.Spt.DataTransferLength = 0;
		//	for (int i = 0; i < 2; i++)
		//	{
		//		DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);
		//	}

		//	// Prevent Media Remove Allow command
		//	Info.Spt.Cdb[0] = 0x1E;
		//	Info.Spt.Cdb[4] = 0x00;
		//	DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);

		//	Info.Spt.Cdb[0] = 0x1B; // Start/Stop Unit
		//	Info.Spt.Cdb[4] = 0x02;
		//	DeviceIoControl(hUsbDev, IOCTL_SCSI_PASS_THROUGH, &Info, sizeof(Info), &Info, sizeof(Info),&ReturnLen, FALSE);
		//}
		CloseHandle(hUsbDev);
	}
}


static void RemoveCompositMastorage(LPCTSTR szDevIdString)
{
	CString szDevId = szDevIdString;
	int idx = szDevId.ReverseFind(_T('\\'));
	if (idx == -1)
	{
		return;
	}

	CString szClass = szDevId.Left(idx);

	GUID  *guid = (GUID *)&GUID_DEVCLASS_USB; // 0x36fc9e60L, 0xc465, 0x11cf, 0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 );

	HDEVINFO hDevInfo = SetupDiGetClassDevs(guid, szClass, NULL, DIGCF_PRESENT);
	if ( INVALID_HANDLE_VALUE == hDevInfo)
	{
		_RPTF0(_CRT_WARN, "CReflashToolDlg::OnUsbStorArrival - Wrong Device Class ID...\n"); 
		return;
	}

	SP_DEVINFO_DATA spDevInfoData;
	ZeroMemory(&spDevInfoData, sizeof(SP_DEVINFO_DATA));
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (DWORD j = 0; SetupDiEnumDeviceInfo(hDevInfo, j, &spDevInfoData); j++)
	{
		TCHAR szBuff[MAX_PATH];
		DWORD nSize = 0;
		DWORD regDataType = 0;

		// get the device friendly name to filter AT Command Port.
		if (SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_SERVICE  , &regDataType, (PBYTE)szBuff, sizeof(szBuff),	&nSize ))
		{
			SP_PROPCHANGE_PARAMS spPropChangeParams;
			spPropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
			spPropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
			spPropChangeParams.Scope = DICS_FLAG_CONFIGSPECIFIC;
			spPropChangeParams.StateChange = DICS_DISABLE;
			spPropChangeParams.HwProfile = 0;
			if (!SetupDiSetClassInstallParams(hDevInfo, &spDevInfoData, (SP_CLASSINSTALL_HEADER *)&spPropChangeParams, sizeof(spPropChangeParams)))
			{
				//throw _T("");
			}

			/* Call the ClassInstaller and perform the change. */
			if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo ,&spDevInfoData))
			{
				//throw _T("");
			}
			break;
		}

	}
	// Release Device Handler
	SetupDiDestroyDeviceInfoList(hDevInfo);

}
