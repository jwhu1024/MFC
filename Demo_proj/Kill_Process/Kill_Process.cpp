// Kill_Process.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Kill_Process.h"
#include "Tlhelp32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	///////////////////////////////////////////////////////////////
	// Finding Application which we have specified once that 
	// application finds it will give you message Application found
	// then first of all it opens that process and getting all rights 
	// of that application.Once we have all rights then we can kill 
	// that application with Terminate Process.
	///////////////////////////////////////////////////////////////
		HANDLE hndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		DWORD dwsma = GetLastError();

		DWORD dwExitCode = 0;

		PROCESSENTRY32  procEntry={0};
		procEntry.dwSize = sizeof( PROCESSENTRY32 );
		Process32First(hndl,&procEntry);
		do
		{
			if(!strcmpi(procEntry.szExeFile,"wweb32.exe"))
			{
				AfxMessageBox("Application found");
				break;
			}
			//cout<<"Running Process "<<"          "<<procEntry.szExeFile<<endl;
		}while(Process32Next(hndl,&procEntry));
		
	///////////////////////////////////////////////////////////////	
		HANDLE hHandle;
		hHandle = ::OpenProcess(PROCESS_ALL_ACCESS,0,procEntry.th32ProcessID);
		
		::GetExitCodeProcess(hHandle,&dwExitCode);
		::TerminateProcess(hHandle,dwExitCode);

	return nRetCode;
}


