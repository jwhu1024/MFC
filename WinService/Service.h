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
VOID BR_EJECT();
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


LPTSTR ProcessNames[MAX_NUM_OF_PROCESS];

SERVICE_STATUS_HANDLE   hServiceStatusHandle; 
SERVICE_STATUS          ServiceStatus; 
PROCESS_INFORMATION	pProcInfo[MAX_NUM_OF_PROCESS];