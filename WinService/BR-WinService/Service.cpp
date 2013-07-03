//  Service.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Winsvc.h"
#include "EnumPort.h"
#include "Tlhelp32.h"
#include <initguid.h>
#include <dbt.h>
#include <WtsApi32.h>
#include <UserEnv.h>
#include <devguid.h>
#include <setupapi.h>
#pragma comment(lib,"WtsApi32.lib")
#pragma comment(lib,"UserEnv.lib")
#pragma comment(lib,"Setupapi.lib")   
//#pragma comment(lib, "EnumDll.lib")

#define		MAX_NUM_OF_PROCESS		1
#define		SERVICE_CONTROL_RUNAPP	128
#define		SERVICE_CONTROL_WRITE_LOG	129
#define		SERVICE_CONTROL_RESTART_APP	130
#define		SERVICE_CONTROL_RESTART_DRIVER	131

/** Window Service **/
VOID ServiceMainProc();
VOID Install(char* pPath, char* pName);
VOID UnInstall(char* pName);
VOID WriteLog(char* pFile, char* pMsg);
BOOL KillService(char* pName);
BOOL RunService(char* pName);
BOOL StartServiceFromStop(char* pName);
VOID ExecuteSubProcess();
VOID ProcMonitorThread(VOID *);
BOOL GetTokenByName(HANDLE &hToken,LPSTR lpName);
BOOL StartProcess(int ProcessIndex);
VOID EndProcess(int ProcessIndex);
VOID MiniHideProcess(int ProcessIndex);
BOOL CtrlService(char* pName,DWORD _ctrCode);
VOID AttachProcessNames();
VOID DeviceEventNotify(DWORD evtype, PVOID evdata);
VOID ModemDriverReset(BOOL Reset);
VOID CallAppWithoutUAC();

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
//VOID WINAPI ServiceHandler(DWORD fdwControl);
VOID WINAPI ServiceHandler(DWORD dwOpcode,DWORD evtype, PVOID evdata, PVOID Context);


/** Window Service **/
const int nBufferSize = 500;
CHAR pServiceName[nBufferSize+1];
CHAR pExeFile[nBufferSize+1];
CHAR lpCmdLineData[nBufferSize+1];
CHAR pLogFile[nBufferSize+1];
BOOL ProcessStarted = TRUE;
BOOL bisDevExist = FALSE;
BOOL bNeedStartAP = TRUE;
BOOL bStartAPwNoDevice = FALSE;
BOOL bWriteLog = FALSE;
LPTSTR lpszpath;
HDEVNOTIFY hDevNotify[2];
BOOL bEjectCD = FALSE;
OSVERSIONINFOEX osvi;

CRITICAL_SECTION		myCS;
SERVICE_TABLE_ENTRY		lpServiceStartTable[] = 
{
	{pServiceName, ServiceMain},
	{NULL, NULL}
};

/*LPTSTR ProcessNames[] = { "C:\\n.exe",
						 "C:\\a.exe",
						 "C:\\b.exe",
						 "C:\\c.exe" };*/
LPTSTR ProcessNames[MAX_NUM_OF_PROCESS];

SERVICE_STATUS_HANDLE   hServiceStatusHandle; 
SERVICE_STATUS          ServiceStatus; 
PROCESS_INFORMATION	pProcInfo[MAX_NUM_OF_PROCESS];

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc >= 2)
		strcpy_s(lpCmdLineData, nBufferSize+1 ,argv[1]);
	ServiceMainProc();
	return 0;
}

VOID ServiceMainProc()
{
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	BOOL isOsVerInfoEx = GetVersionEx((OSVERSIONINFO *) &osvi);

	::InitializeCriticalSection(&myCS);
	// initialize variables for .exe and .log file names
	char pModuleFile[nBufferSize+1];
	DWORD dwSize = GetModuleFileName(NULL, pModuleFile, nBufferSize);
	pModuleFile[dwSize] = 0;
	if(dwSize>4 && pModuleFile[dwSize-4] == '.')
	{
		sprintf_s(pExeFile,nBufferSize+1,"%s",pModuleFile);
		pModuleFile[dwSize-4] = 0;
		sprintf_s(pLogFile,nBufferSize+1,"%s.log",pModuleFile);
	}
	strcpy_s(pServiceName,nBufferSize+1,"BandLuxe_Service");

	if(_stricmp("-i",lpCmdLineData) == 0 || _stricmp("-I",lpCmdLineData) == 0)
		Install(pExeFile, pServiceName);
	else if(_stricmp("-k",lpCmdLineData) == 0 || _stricmp("-K",lpCmdLineData) == 0)
		KillService(pServiceName);
	else if(_stricmp("-u",lpCmdLineData) == 0 || _stricmp("-U",lpCmdLineData) == 0)
		UnInstall(pServiceName);
	else if(_stricmp("-s",lpCmdLineData) == 0 || _stricmp("-S",lpCmdLineData) == 0)		
	{		
		RunService(pServiceName);		
	}
	else if(_stricmp("-r",lpCmdLineData) == 0 || _stricmp("-R",lpCmdLineData) == 0)		
	{		
		//KillService(pServiceName);
		//RunService(pServiceName);
		CtrlService(pServiceName,SERVICE_CONTROL_RESTART_APP);//restart app
	}
	else if(_stricmp("-g",lpCmdLineData) == 0 || _stricmp("-G",lpCmdLineData) == 0)		
	{		
		StartServiceFromStop(pServiceName);
		Sleep(1000);
		CtrlService(pServiceName,SERVICE_CONTROL_RUNAPP);
	}
	else if(_stricmp("-l",lpCmdLineData) == 0 || _stricmp("-L",lpCmdLineData) == 0)		
	{
		CtrlService(pServiceName,SERVICE_CONTROL_WRITE_LOG);//enable or disable write log
	}
	else if(_stricmp("-e",lpCmdLineData) == 0 || _stricmp("-E",lpCmdLineData) == 0)		
		ExecuteSubProcess();
	else if(_stricmp("-rm",lpCmdLineData) == 0 || _stricmp("-RM",lpCmdLineData) == 0)	 
	{
		ModemDriverReset(TRUE);
	}
}

VOID Install(char* pPath, char* pName)
{  
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS);//SC_MANAGER_CREATE_SERVICE); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"OpenSCManager failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
	}
	else
	{		
		strcat_s(pPath,nBufferSize+1," -e");

		SC_HANDLE schService = CreateService
		( 
			schSCManager,	/* SCManager database      */ 
			pName,			/* name of service         */ 
			pName,			/* service name to display */ 
			SERVICE_ALL_ACCESS,        /* desired access          */ 
			SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS , /* service type            */ 
			SERVICE_AUTO_START,      /* start type              */ 
			SERVICE_ERROR_NORMAL,      /* error control type      */ 
			pPath,			/* service's binary        */ 
			NULL,                      /* no load ordering group  */ 
			NULL,                      /* no tag identifier       */ 
			NULL,                      /* no dependencies         */ 
			NULL,                      /* LocalSystem account     */ 
			NULL
		);                     /* no password             */ 
		if (schService==0) 
		{
			long nError =  GetLastError();
			char pTemp[121];
			sprintf_s(pTemp, 121,"Failed to create service %s, error code = %d\n", pName, nError);
			WriteLog(pLogFile, pTemp);
		}
		else
		{
			SERVICE_DESCRIPTION sdBuf;
			
			sdBuf.lpDescription = "BandRich HSPA Service.";
			
			char pTemp[121];

			SC_LOCK sclLock;
			sclLock = LockServiceDatabase(schSCManager);
			
			if ( sclLock != NULL )
			{
				if ( !ChangeServiceConfig2(schService,SERVICE_CONFIG_DESCRIPTION,&sdBuf) )
				{
					sprintf_s(pTemp,121, "ChangeServiceConfig2 fail.\n");	
					WriteLog(pLogFile, pTemp);
				}
				else
				{					
					sprintf_s(pTemp,121, "ChangeServiceConfig2 success.\n");
					WriteLog(pLogFile, pTemp);
				}
				UnlockServiceDatabase(sclLock);
			}
			else
			{
				if (GetLastError() != ERROR_SERVICE_DATABASE_LOCKED) 
				{					
					sprintf_s(pTemp,121, "Database lock failed (%d)\n", GetLastError());
					WriteLog(pLogFile, pTemp);
				}

			}

			sprintf_s(pTemp,121, "Service %s installed\n", sdBuf.lpDescription);
			WriteLog(pLogFile, pTemp);
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);
	}	
}

VOID UnInstall(char* pName)
{
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"OpenSCManager failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
	}
	else
	{
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf_s(pTemp, 121, "OpenService failed, error code = %d\n", nError);
			WriteLog(pLogFile, pTemp);
		}
		else
		{
			if(!DeleteService(schService)) 
			{
				char pTemp[121];
				sprintf_s(pTemp, 121,"Failed to delete service %s\n", pName);
				WriteLog(pLogFile, pTemp);
			}
			else 
			{
				char pTemp[121];
				sprintf_s(pTemp, 121,"Service %s removed\n",pName);
				WriteLog(pLogFile, pTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);	
	}
	//DeleteFile(pLogFile);
}

VOID WriteLog(char* pFile, char* pMsg)
{
	if ( bWriteLog == FALSE )
		return;

	// write error or other information into log file
	::EnterCriticalSection(&myCS);
	try
	{
		SYSTEMTIME oT;
		::GetLocalTime(&oT);
		FILE* pLog;
		fopen_s(&pLog,pFile,"a");
		fprintf(pLog,"%02d/%02d/%04d, %02d:%02d:%02d    %s\n",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,pMsg); 
		fclose(pLog);
	} catch(...) {}
	::LeaveCriticalSection(&myCS);
}

BOOL KillService(char* pName) 
{ 
	// kill service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"OpenSCManager failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf_s(pTemp, "OpenService failed, error code = %d\n", nError);
			WriteLog(pLogFile, pTemp);
		}
		else
		{
			// call ControlService to kill the given service
			SERVICE_STATUS status;
			if(ControlService(schService,SERVICE_CONTROL_STOP,&status))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				long nError = GetLastError();
				char pTemp[121];
				sprintf_s(pTemp, 121,"ControlService failed, error code = %d\n", nError);
				WriteLog(pLogFile, pTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}


BOOL CtrlService(char* pName , DWORD _ctrCode)
{
	// kill service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"OpenSCManager failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf_s(pTemp, "OpenService failed, error code = %d\n", nError);
			WriteLog(pLogFile, pTemp);
		}
		else
		{			
			SERVICE_STATUS status;
			if(ControlService(schService,_ctrCode,&status))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				long nError = GetLastError();
				char pTemp[121];
				sprintf_s(pTemp, 121,"ControlService failed, error code = %d\n", nError);
				WriteLog(pLogFile, pTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;

}

BOOL RunService(char* pName) 
{ 
	// run service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"OpenSCManager failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf_s(pTemp, 121,"OpenService failed, error code = %d\n", nError);
			WriteLog(pLogFile, pTemp);
		}
		else
		{
			// call StartService to run the service
			if(StartService(schService, 0, (const char**)NULL))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				long nError = GetLastError();

				if ( nError != ERROR_SERVICE_ALREADY_RUNNING )
				{
					char pTemp[121];
					sprintf_s(pTemp, 121,"StartService failed, error code = %d\n", nError);
					WriteLog(pLogFile, pTemp);
				}
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

BOOL StartServiceFromStop(char* pName) 
{ 
	// run service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"OpenSCManager failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTemp[121];
			sprintf_s(pTemp, 121,"OpenService failed, error code = %d\n", nError);
			WriteLog(pLogFile, pTemp);
		}
		else
		{
			SERVICE_STATUS _service_status;			
			
			if(QueryServiceStatus(schService,&_service_status))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager);
				if ( _service_status.dwCurrentState == SERVICE_RUNNING )
				{
					char pTemp[121];
					sprintf_s(pTemp, 121,"Service Running.\n");
					WriteLog(pLogFile, pTemp);
					return TRUE;
				}
				else
					RunService(pName);

			}
			else
			{
				long nError = GetLastError();

				char pTemp[121];
				sprintf_s(pTemp, 121,"StartServiceFromStop failed, error code = %d\n", nError);
				WriteLog(pLogFile, pTemp);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}


VOID ExecuteSubProcess()
{	
	if(_beginthread(ProcMonitorThread, 0, NULL) == -1)
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"StartService failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
	}

	if(!StartServiceCtrlDispatcher(lpServiceStartTable))
	{
		ProcessStarted = FALSE;
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"StartServiceCtrlDispatcher failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
	}	
	::DeleteCriticalSection(&myCS);
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	DWORD   status = 0; 
    DWORD   specificError = 0xfffffff; 
 
    ServiceStatus.dwServiceType        = SERVICE_WIN32; 
    ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;// | SERVICE_ACCEPT_PAUSE_CONTINUE; 
    ServiceStatus.dwWin32ExitCode      = 0; 
    ServiceStatus.dwServiceSpecificExitCode = 0; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0; 
 
    hServiceStatusHandle = RegisterServiceCtrlHandlerEx(pServiceName, (LPHANDLER_FUNCTION_EX)ServiceHandler,0); 
    if (hServiceStatusHandle==0) 
    {
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"RegisterServiceCtrlHandler failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
        return; 
    } 


	//Register Device Notification	
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = 
        sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;	

	//USB	
	memcpy( &(NotificationFilter.dbcc_classguid),
   			&(GUID_DEVINTERFACE_CDROM),			
   			sizeof(struct _GUID));
	hDevNotify[0] = RegisterDeviceNotification(hServiceStatusHandle, 
    &NotificationFilter,
    DEVICE_NOTIFY_SERVICE_HANDLE);

	//End USB
	
	//COM PORT
	memcpy( &(NotificationFilter.dbcc_classguid),
   			&(GUID_DEVINTERFACE_COMPORT),
   			sizeof(struct _GUID));
	hDevNotify[1] = RegisterDeviceNotification(hServiceStatusHandle, 
    &NotificationFilter,
    DEVICE_NOTIFY_SERVICE_HANDLE);
	//End COM PORT	
	//
 
    // Initialization complete - report running status 
    ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0;  
    if(!SetServiceStatus(hServiceStatusHandle, &ServiceStatus)) 
    { 
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"SetServiceStatus failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
    } 

	AttachProcessNames();
	for(int iLoop = 0; iLoop < MAX_NUM_OF_PROCESS; iLoop++)
	{
		pProcInfo[iLoop].hProcess = 0;
		//StartProcess(iLoop);
	}
}

VOID AttachProcessNames()
{
	//LPTSTR lpszpath;
	lpszpath = new char[nBufferSize];
	memset(lpszpath,0x00,sizeof(lpszpath));
	DWORD dwSize = GetModuleFileName(NULL, lpszpath, nBufferSize);
	lpszpath[dwSize] = 0;
	while(lpszpath[dwSize] != '\\' && dwSize != 0)
	{
		lpszpath[dwSize] = 0; dwSize--;
	}
	for(int iLoop = 0; iLoop < MAX_NUM_OF_PROCESS; iLoop++)
	{
		ProcessNames[iLoop] = (char*) malloc(nBufferSize);
		memset(ProcessNames[iLoop], 0x00, nBufferSize);
		strcpy_s(ProcessNames[iLoop],nBufferSize,lpszpath); 
	}
	strcat_s(ProcessNames[0],nBufferSize, "CManager.exe -h");
	/*strcat(ProcessNames[1], "2.exe");
	strcat(ProcessNames[2], "3.exe");
	strcat(ProcessNames[3], "4.exe");*/
}

//VOID WINAPI ServiceHandler(DWORD fdwControl)
VOID WINAPI ServiceHandler(DWORD dwOpcode,DWORD evtype, PVOID evdata, PVOID Context)
{
	switch(dwOpcode) 
	{
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			UnregisterDeviceNotification(hDevNotify[0]);
			UnregisterDeviceNotification(hDevNotify[1]);
			ProcessStarted = FALSE;
			ServiceStatus.dwWin32ExitCode = 0; 
			ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
			ServiceStatus.dwCheckPoint    = 0; 
			ServiceStatus.dwWaitHint      = 0;
			// terminate all processes started by this service before shutdown
			{
				for(int i = MAX_NUM_OF_PROCESS - 1 ; i >= 0; i--)
				{
					EndProcess(i);
					delete ProcessNames[i];
				}			
				delete lpszpath;
			}
			break; 
		case SERVICE_CONTROL_PAUSE:
			ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
			break;
		case SERVICE_CONTROL_CONTINUE:
			ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
			break;
		case SERVICE_CONTROL_INTERROGATE:
			break;
		case SERVICE_CONTROL_DEVICEEVENT:			
			WriteLog(pLogFile, "==> SERVICE_CONTROL_DEVICEEVENT <==");
			DeviceEventNotify(evtype,evdata);
			break;
		case SERVICE_CONTROL_RUNAPP:
			bNeedStartAP = TRUE;
			bStartAPwNoDevice = TRUE;
			break;
		case SERVICE_CONTROL_WRITE_LOG:
			bWriteLog = !bWriteLog;
			break;
		case SERVICE_CONTROL_RESTART_APP:
			EndProcess(0);
			pProcInfo[0].hProcess = 0;		
			StartProcess(0);
			break;
		case SERVICE_CONTROL_RESTART_DRIVER:
			//CallAppWithoutUAC();
			ModemDriverReset(TRUE);
			break;
	};
    if (!SetServiceStatus(hServiceStatusHandle,  &ServiceStatus)) 
	{ 
		long nError = GetLastError();
		char pTemp[121];
		sprintf_s(pTemp, 121,"SetServiceStatus failed, error code = %d\n", nError);
		WriteLog(pLogFile, pTemp);
    } 
}


BOOL GetTokenByName(HANDLE &hToken,LPSTR lpName)
{
	if(!lpName)
	{
	  return FALSE;
	}
	HANDLE         hProcessSnap = NULL; 
    BOOL           bRet      = FALSE; 
    PROCESSENTRY32 pe32      = {0}; 

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (hProcessSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 

    pe32.dwSize = sizeof(PROCESSENTRY32); 

    if (Process32First(hProcessSnap, &pe32)) 
    {  
        do 
        {
		   _strupr_s(pe32.szExeFile,MAX_PATH);
		   _strupr_s(lpName,MAX_PATH);	   

		   if( !strcmp(pe32.szExeFile,lpName) )
		   {
				//萬一同時有多個USER LOGIN，要確定目前ACTIVE的桌面是哪個
				DWORD sessionID = WTSGetActiveConsoleSessionId();
				DWORD currentID = pe32.th32ProcessID;
				DWORD SID;
				if(!ProcessIdToSessionId(currentID, &SID))				
					return FALSE;				
				if(SID != sessionID)
					continue;

				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,pe32.th32ProcessID);

				bRet = OpenProcessToken(hProcess,TOKEN_ALL_ACCESS,&hToken);

				CloseHandle (hProcessSnap); 

				return (bRet);
		   }
        } 
        while (Process32Next(hProcessSnap, &pe32)); 
        bRet = FALSE; 
    } 
    else 
        bRet = FALSE;

    CloseHandle (hProcessSnap); 
    return (bRet);
}


BOOL StartProcess(int ProcessIndex)
{
	if (TRUE || osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) //Win2000
	{
		//if no device return

		HANDLE hToken;
		CHAR szShellName[MAX_PATH];
		sprintf_s(szShellName,MAX_PATH,"EXPLORER.EXE");
		//sprintf_s(szShellName,MAX_PATH,"BRService.EXE");

		if(!GetTokenByName(hToken,szShellName))
		{
		  Sleep(1000);
		  return FALSE;
		}


		STARTUPINFO startUpInfo = { sizeof(STARTUPINFO),NULL,"",NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0};  
		startUpInfo.wShowWindow = SW_SHOW;
		startUpInfo.lpDesktop = NULL;
		//if(((int)ShellExecute(NULL, _T("open"), _T("CManager.exe") , _T("-h"), NULL, SW_SHOW)) < 32)
		if(CreateProcessAsUser(hToken,NULL, ProcessNames[ProcessIndex],NULL,NULL,TRUE,NORMAL_PRIORITY_CLASS,\
					 NULL,lpszpath,&startUpInfo,&pProcInfo[ProcessIndex]))
		//if(CreateProcess(NULL, ProcessNames[ProcessIndex],NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,\
		//			 NULL,lpszpath,&startUpInfo,&pProcInfo[ProcessIndex]))
		{
			Sleep(1000);
			return TRUE;
		}
		else
		{
			long nError = GetLastError();
			char pTemp[256];
			sprintf_s(pTemp,256,"Failed to start program '%s', error code = %d\n", ProcessNames[ProcessIndex], nError); 
			WriteLog(pLogFile, pTemp);
			Sleep(1000);
			return FALSE;
		}
	}
	//如果把CM權限改成SYSTEM，會不能讀取OUTLOOK連絡人
	else //WinXP 以上
	{
		CallAppWithoutUAC();
	}
}

VOID CallAppWithoutUAC()
{
	
	PROCESS_INFORMATION pi;
	STARTUPINFO si  = { sizeof(STARTUPINFO),NULL,"",NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0};
	si.wShowWindow = SW_HIDE;
	si.lpDesktop = NULL;
	BOOL bResult = FALSE;
	DWORD dwSessionId,winlogonPid;
	HANDLE hUserToken,hUserTokenDup,hPToken,hProcess;
	DWORD dwCreationFlags;

	// Log the client on to the local computer.

	dwSessionId = WTSGetActiveConsoleSessionId();

	//////////////////////////////////////////
	// Find the winlogon process
	////////////////////////////////////////

	PROCESSENTRY32 procEntry;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		return;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnap, &procEntry))
	{
		return;
	}

	do
	{
		if (_stricmp(procEntry.szExeFile, "winlogon.exe") == 0)
		{
			// We found a winlogon process...make sure it's running in the console session
			DWORD winlogonSessId = 0;
			if (ProcessIdToSessionId(procEntry.th32ProcessID, &winlogonSessId) && winlogonSessId == dwSessionId)
			{
				winlogonPid = procEntry.th32ProcessID;
				break;
			}
		}

	} while (Process32Next(hSnap, &procEntry));

	////////////////////////////////////////////////////////////////////////

	WTSQueryUserToken(dwSessionId,&hUserToken);
	dwCreationFlags = NORMAL_PRIORITY_CLASS/*|CREATE_NEW_CONSOLE*/;
	//ZeroMemory(&si, sizeof(STARTUPINFO));
	//si.cb= sizeof(STARTUPINFO);
	//si.lpDesktop = "winsta0\\default";
	ZeroMemory(&pi, sizeof(pi));
	TOKEN_PRIVILEGES tp;
	LUID luid;
	hProcess = OpenProcess(MAXIMUM_ALLOWED,FALSE,winlogonPid);

	if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY
									|TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY|TOKEN_ADJUST_SESSIONID
									|TOKEN_READ|TOKEN_WRITE,&hPToken))
	{
	   int abcd = GetLastError();
	   printf("Process token open Error: %u\n",GetLastError()); 			   
	}

	if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid))
	{
	   printf("Lookup Privilege value Error: %u\n",GetLastError());
	}
	tp.PrivilegeCount =1;
	tp.Privileges[0].Luid =luid;
	tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;

	DuplicateTokenEx(hPToken,MAXIMUM_ALLOWED,NULL,SecurityIdentification,TokenPrimary,&hUserTokenDup);
	int dup = GetLastError();

	//Adjust Token privilege
	SetTokenInformation(hUserTokenDup,TokenSessionId,(void*)dwSessionId,sizeof(DWORD));

	if (!AdjustTokenPrivileges(hUserTokenDup,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),(PTOKEN_PRIVILEGES)NULL,NULL))
	{
	   int abc =GetLastError();
	   printf("Adjust Privilege value Error: %u\n",GetLastError());
	}

	if (GetLastError()== ERROR_NOT_ALL_ASSIGNED)
	{
		
		printf("Token does not have the provilege\n");
	}

	LPVOID pEnv =NULL;

	if(CreateEnvironmentBlock(&pEnv,hUserTokenDup,TRUE))
	{
	   dwCreationFlags|=CREATE_UNICODE_ENVIRONMENT;
	}
	else
	  pEnv=NULL;

	bResult = CreateProcessAsUser(
	  hUserTokenDup,            // client's access token
	  NULL,              // file to execute
	  "BRService.exe -RM",     // command line
	  NULL,              // pointer to process SECURITY_ATTRIBUTES
	  NULL,              // pointer to thread SECURITY_ATTRIBUTES
	  FALSE,             // handles are not inheritable
	  dwCreationFlags,  // creation flags
	  pEnv,              // pointer to new environment block 
	  lpszpath,              // name of current directory 
	  &si,               // pointer to STARTUPINFO structure
	  &pi                // receives information about new process
	);

	
	CloseHandle(hProcess);
	CloseHandle(hUserToken);
	CloseHandle(hUserTokenDup);
	CloseHandle(hPToken);

	return;
}

VOID EndProcess(int ProcessIndex)
{
	if(ProcessIndex >=0 && ProcessIndex <= MAX_NUM_OF_PROCESS)
	{
		if(pProcInfo[ProcessIndex].hProcess)
		{
			char pTemp[121];
			sprintf_s(pTemp, 121,"Stop dwThreadId %d\n", pProcInfo[ProcessIndex].dwThreadId);
			WriteLog(pLogFile, pTemp);

			// post a WM_CLOSE message first
			while ( PostThreadMessage(pProcInfo[ProcessIndex].dwThreadId, WM_CLOSE, 0, 0) != TRUE )
			{
				if ( GetLastError() == ERROR_INVALID_THREAD_ID )
				{
					sprintf_s(pTemp, 121,"Stop dwThreadId fail.\n");
					WriteLog(pLogFile, pTemp);
					return;
				}
				Sleep(1000);
			}

			DWORD dwCode;
			
			if(::GetExitCodeProcess(pProcInfo[ProcessIndex].hProcess, &dwCode) && pProcInfo[ProcessIndex].hProcess != NULL)
			{
				PostThreadMessage(pProcInfo[ProcessIndex].dwThreadId, WM_QUIT, 0, 0);//Clean Dialogs
				Sleep(1000);
				PostThreadMessage(pProcInfo[ProcessIndex].dwThreadId, WM_CLOSE, 0, 0);
				DWORD start_tick = GetTickCount();
				DWORD tick_delta = 0;

				while ( dwCode == STILL_ACTIVE && tick_delta < 10000)
				{
					::GetExitCodeProcess(pProcInfo[ProcessIndex].hProcess, &dwCode);
					tick_delta = GetTickCount() - start_tick;
					Sleep(100);
				}

				if(dwCode == STILL_ACTIVE)
				{
					PostThreadMessage(pProcInfo[ProcessIndex].dwThreadId, WM_QUIT, 0, 0);
					Sleep(5000);
					TerminateProcess(pProcInfo[ProcessIndex].hProcess, 0);
				}
			}
			
			// terminate the process by force
			//TerminateProcess(pProcInfo[ProcessIndex].hProcess, 0);
		}
	}
}


VOID MiniHideProcess(int ProcessIndex)
{
	if(ProcessIndex >=0 && ProcessIndex <= MAX_NUM_OF_PROCESS)
	{
		if(pProcInfo[ProcessIndex].hProcess)
		{
			DWORD dwCode;
			
			if(::GetExitCodeProcess(pProcInfo[ProcessIndex].hProcess, &dwCode) && pProcInfo[ProcessIndex].hProcess != NULL)
			{				
				if(dwCode == STILL_ACTIVE)
				{
					PostThreadMessage(pProcInfo[ProcessIndex].dwThreadId, SW_MINIMIZE, 0, 0);			
					PostThreadMessage(pProcInfo[ProcessIndex].dwThreadId, SW_HIDE, 0, 0);
				}
			}			
			
		}
	}
}


VOID ProcMonitorThread(VOID *)
{
	bisDevExist = detect_diagnostic_port();

	LPCSTR mode = new CHAR[260];
	HKEY hkOpened;
	DWORD dwType = REG_SZ;
	DWORD dwSize = 260;

	// Try to open the key
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\BandRich\\BandLuxeService"), 0, KEY_READ, &hkOpened) != ERROR_SUCCESS)
	{
		 // Clean up
		 RegCloseKey(hkOpened);
	}

	// If the key was opened, try to retrieve the value
	if (RegQueryValueEx(hkOpened, _T("TargetDeviceMode"), 0, &dwType, (LPBYTE)mode, &dwSize) != ERROR_SUCCESS)
	{		 
		RegCloseKey(hkOpened);
	}

	// Clean up
	RegCloseKey(hkOpened);

	Sleep(5000);
	eject_bandrich_cdrom(mode);

	while(ProcessStarted == TRUE)
	{	
		if ( bEjectCD )
		{
			bEjectCD = FALSE;
			Sleep(5000);
			eject_bandrich_cdrom(mode);			
		}

		if ( (bisDevExist == TRUE && ServiceStatus.dwCurrentState != SERVICE_PAUSED) || bStartAPwNoDevice == TRUE )//device is exist
		{	

			if ( bNeedStartAP == TRUE || bStartAPwNoDevice == TRUE )
			{				
				bStartAPwNoDevice = FALSE;

				if ( pProcInfo[0].hProcess == NULL)
				{					
					for(int iLoop = 0; iLoop < MAX_NUM_OF_PROCESS; iLoop++)
					{
						pProcInfo[iLoop].hProcess = 0;						
						StartProcess(iLoop);
					}
				}

				DWORD dwCode;
				for(int iLoop = 0; iLoop < MAX_NUM_OF_PROCESS; iLoop++)
				{
					if(::GetExitCodeProcess(pProcInfo[iLoop].hProcess, &dwCode) && pProcInfo[iLoop].hProcess != NULL)
					{
						if(dwCode != STILL_ACTIVE)
						{
							if(StartProcess(iLoop))
							{
								char pTemp[121];
								sprintf_s(pTemp, 121,"Restarted process %d\n", iLoop);
								WriteLog(pLogFile, pTemp);
								bNeedStartAP = FALSE;
							}
						} else if ( bNeedStartAP == TRUE )
						{
							bNeedStartAP = FALSE;
							PostThreadMessage(pProcInfo[iLoop].dwThreadId, SW_SHOW, 0, 0);
							Sleep(3000);
						}
					}
				}				
			}
		}
		else//device is not exist
		{			
			bNeedStartAP = TRUE;
			///////////*DWORD dwCode;
			//////////for(int iLoop = 0; iLoop < MAX_NUM_OF_PROCESS; iLoop++)
			//////////{
			//////////	if(::GetExitCodeProcess(pProcInfo[iLoop].hProcess, &dwCode) && pProcInfo[iLoop].hProcess != NULL)
			//////////	{
			//////////		if(dwCode == STILL_ACTIVE)
			//////////		{
			//////////			EndProcess(iLoop);
			//////////			
			//////////			char pTemp[121];
			//////////			sprintf_s(pTemp, 121,"Hide&Mini UI %d\n", iLoop);
			//////////			WriteLog(pLogFile, pTemp);						
			//////////			
			//////////		}
			//////////	}
			//////////}*/

		}

		Sleep(10);
	}
}


VOID DeviceEventNotify(DWORD evtype, PVOID evdata)
{
	switch (evtype)
	{
		PDEV_BROADCAST_HDR pdata;

		case DBT_DEVICEREMOVECOMPLETE:
			//OutputDebugString("");
			
			pdata=(PDEV_BROADCAST_HDR)evdata;
			if ( pdata->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE )
			{
				PDEV_BROADCAST_DEVICEINTERFACE_W pDevInf;
				pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE_W)pdata;

				int i = process_bandrich_device_change(evtype,pDevInf->dbcc_name);
				char pTemp[121];
				sprintf_s(pTemp, 121,"Stop process_bandrich_device_change = %d", i);
				OutputDebugString(pTemp);
				if ( i == 2 )
					bisDevExist = FALSE;
					//bisDevExist = detect_diagnostic_port();
			}

			break;
		case DBT_DEVICEARRIVAL:				
			pdata=(PDEV_BROADCAST_HDR)evdata;			
		
			if ( pdata->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE )
			{	
				PDEV_BROADCAST_DEVICEINTERFACE_W pDevInf;
				pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE_W)pdata;

				int ii ;
				ii = process_bandrich_device_change(evtype,pDevInf->dbcc_name);
				char pTemp[121];
				sprintf_s(pTemp, 121,"DBT_DEVICEARRIVAL process_bandrich_device_change = %d", ii);
				OutputDebugString(pTemp);
				WriteLog(pLogFile, pTemp);
				
				if ( ii == 2 )
					bisDevExist = TRUE;
					//bisDevExist = detect_diagnostic_port();
				else if ( ii==1 )
				{
					bEjectCD = TRUE;
				}
			}		
			break;
	}
}

VOID ModemDriverReset(BOOL Reset)
{
	CString m_strPortName;
	CString strPortName = _T("\\\\.\\");
	//get modem com port
	HDEVINFO hDevInfo = 
		SetupDiGetClassDevs(&GUID_DEVCLASS_MODEM,NULL,NULL,DIGCF_PRESENT);

	if ( hDevInfo == INVALID_HANDLE_VALUE )
	{
		//AfxMessageBox(_T("hDevInfo == INVALID_HANDLE_VALUE"));
		return ;
	}

	SP_DEVINFO_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD i=0;SetupDiEnumDeviceInfo(hDevInfo,i,&DeviceInfoData);i++)
	{
		// get hardware id string
		DWORD regDataType = 0;
		TCHAR fname[256] = {0};
		DWORD bufSize = 256;

		// get the device capabilities
		SetupDiGetDeviceRegistryProperty(
				hDevInfo,
				&DeviceInfoData,
				SPDRP_FRIENDLYNAME,
				&regDataType,
				(PBYTE)fname,
				bufSize,
				&bufSize);

		if ( _tcsstr(fname,_T("BandLuxe")) )
		{
			TCHAR szPortName[MAX_PATH];
			HKEY hKey = SetupDiOpenDevRegKey(hDevInfo, 
											&DeviceInfoData, 
											DICS_FLAG_GLOBAL,
											NULL,
											DIREG_DRV,
											KEY_READ);
			if (hKey)
			{
				DWORD dwType = REG_SZ;
				DWORD dwReqSize = sizeof(szPortName);

				//AfxMessageBox(_T("hKey OK"));
				// Query for portname
				if (RegQueryValueEx(hKey,_T("AttachedTo"), 0, &dwType, (LPBYTE)&szPortName, &dwReqSize) == ERROR_SUCCESS)
				{
					m_strPortName = CString(szPortName);
					strPortName.Append(m_strPortName);
					HANDLE m_hFile = ::CreateFile( strPortName.GetString(),
												   GENERIC_READ|GENERIC_WRITE,
												   0,
												   0,
												   OPEN_EXISTING,
												   0,
												   0);
	
					//Can't open modem port-> occupied  -> restart modem
					if (Reset == TRUE ||  m_hFile == INVALID_HANDLE_VALUE)
					{
				
						if( Reset == TRUE )
							CloseHandle(m_hFile);

						TCHAR szBuff[MAX_PATH];
						DWORD nSize = 0;
						regDataType = 0;

						if (SetupDiGetDeviceRegistryProperty(hDevInfo, 
															&DeviceInfoData, 
															SPDRP_SERVICE  , 
															&regDataType, 
															(PBYTE)szBuff, 
															sizeof(szBuff),   
															&nSize ))
						{
							SP_PROPCHANGE_PARAMS spPropChangeParams;
							spPropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
							spPropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
							spPropChangeParams.Scope = DICS_FLAG_CONFIGSPECIFIC;
							spPropChangeParams.StateChange = DICS_DISABLE;
							spPropChangeParams.HwProfile = 0;

							//DISABLE
							if (!SetupDiSetClassInstallParams(hDevInfo, &DeviceInfoData, (SP_CLASSINSTALL_HEADER *)&spPropChangeParams, sizeof(spPropChangeParams)))
							{
							  //AfxMessageBox(_T("Modem Driver Reset Fail at Disable Phase 1"));
							}
							/* Call the ClassInstaller and perform the change. */
							if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo ,&DeviceInfoData))
							{
							  //AfxMessageBox(_T("Modem Driver Reset Fail at Disable Phase 2"));
							}

							::Sleep(2500);
							spPropChangeParams.StateChange = DICS_ENABLE;

							//ENABLE
							if (!SetupDiSetClassInstallParams(hDevInfo, &DeviceInfoData, (SP_CLASSINSTALL_HEADER *)&spPropChangeParams, sizeof(spPropChangeParams)))
							{
							  //AfxMessageBox(_T("Modem Driver Reset Fail at Enable Phase 1"));
							}
							/* Call the ClassInstaller and perform the change. */
							if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo ,&DeviceInfoData))
							{
							  //AfxMessageBox(_T("Modem Driver Reset Fail at Enable Phase 2"));
							}							
							::Sleep(2500);
							//AfxMessageBox(_T("Modem Driver Reset OK"));
						}
					}					
					else
						CloseHandle(m_hFile);
					}
				}
			}	
		}
	
	SetupDiDestroyDeviceInfoList(hDevInfo);
	
}
