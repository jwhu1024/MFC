#define ADB_PROC_NAME			_T("\\adb.exe")
#define CMD_CHG_FASTBOOT_MODE	_T("adb shell sys_reboot bootloader")
#define CMD_ADB_DEVICES			_T("adb devices")

void CTestCreateProcessDlg::OnBnClickedButton3()
{
	TCHAR cCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, cCurrentDir);
	_tcscat(cCurrentDir, ADB_PROC_NAME);

	CString cmdline = CMD_ADB_DEVICES;//(cCurrentDir);// = _T("")
	DWORD dExit=0;
	CString strRet;
	ExeCommand(cmdline, dExit, strRet);
	CString tet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Executes the given command using CreateProcess() and WaitForSingleObject().
// Returns FALSE if the command could not be executed or if the exit code could not be determined.
// Extend : Redirect the stdout to get the result code///////////////////////////////////////////
BOOL CTestCreateProcessDlg::ExeCommand(CString cmdLine, DWORD & exitCode, CString& strRet)
{
	//Create pipes to write and read data
	HANDLE rPipe, wPipe;
	SECURITY_ATTRIBUTES secattr; 
	ZeroMemory(&secattr,sizeof(secattr));
	secattr.nLength = sizeof(secattr);
	secattr.bInheritHandle = TRUE;

	CreatePipe( &rPipe, &wPipe, &secattr, 0 );

	// Get current file directory
	TCHAR cCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, cCurrentDir);
	_tcscat(cCurrentDir, ADB_PROC_NAME);

	PROCESS_INFORMATION processInformation = {0};
	STARTUPINFO startupInfo                = {0};
	startupInfo.cb                         = sizeof(startupInfo);
	startupInfo.dwFlags					   = STARTF_USESTDHANDLES;
	startupInfo.hStdInput				   = rPipe;
	startupInfo.hStdOutput				   = wPipe;
	startupInfo.hStdError				   = wPipe;
	int nStrBuffer                         = cmdLine.GetLength() + 50;

	// Create the process
	BOOL result = CreateProcess(cCurrentDir, cmdLine.GetBuffer(nStrBuffer), 
		NULL, NULL, TRUE, 
		NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 
		NULL, NULL, &startupInfo, &processInformation);

	cmdLine.ReleaseBuffer();

	// Close the pipe write handle
	CloseHandle(wPipe); 

	//now read the output pipe here.
	char buf[100]={0};
	DWORD reDword;
	string m_csOutput, csTemp;
	BOOL res=TRUE;
	
	while(res)
	{
		if(res = ::ReadFile( rPipe, buf, 100, &reDword, 0 ))
		{
			csTemp = buf;
			m_csOutput += csTemp;
		}
	}

	// Get the output string here
	strRet = m_csOutput.c_str();

	// Close the pipe read handle
	CloseHandle(rPipe);

	if (!result)
	{
		// CreateProcess() failed
		// Get the error from the system
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);

		// Display the error
		CString strError = (LPTSTR) lpMsgBuf;
		TRACE(_T("::executeCommandLine() failed at CreateProcess()\nCommand=%s\nMessage=%s\n\n"), cmdLine, strError);

		// Free resources created by the system
		LocalFree(lpMsgBuf);

		// We failed.
		return FALSE;
	}
	else
	{
		// Successfully created the process.  Wait for it to finish.
		WaitForSingleObject( processInformation.hProcess, INFINITE );

		// Get the exit code.
		result = GetExitCodeProcess(processInformation.hProcess, &exitCode);

		// Close the handles.
		CloseHandle( processInformation.hProcess );
		CloseHandle( processInformation.hThread );

		if (!result)
		{
			// Could not get exit code.
			TRACE(_T("Executed command but couldn't get exit code.\nCommand=%s\n"), cmdLine);
			return FALSE;
		}

		// We succeeded.
		return TRUE;
	}
}

//************************************
// Method:    AdbExeCommand
// FullName:  CAdbCtrl::AdbExeCommand
// Access:    public 
// Returns:   Returns FALSE if the command could not be executed or if the exit code could not be determined.
// Parameter: CString cmdLine - For example : adb.exe devices
// Parameter: DWORD & exitCode
// Parameter: CString & strRet - stdout log
// Parameter: CMD_TYPE iCMD - Determine the path of console file
// Remark: Executes the given command using CreateProcess() and WaitForSingleObject().
//		   Redirect the stdout to get the result code
//************************************
BOOL CAdbCtrl::AdbExeCommand(CString cmdLine, DWORD& exitCode, CString& strRet, CMD_TYPE iCMD)
{
	//Create pipes to write and read data
	HANDLE rPipe, wPipe;
	SECURITY_ATTRIBUTES secattr; 
	ZeroMemory( &secattr,sizeof(secattr) );
	secattr.nLength = sizeof(secattr);
	secattr.bInheritHandle = TRUE;

	// Create pipe for redirect stdout
	CreatePipe( &rPipe, &wPipe, &secattr, 0 );

	// Get current file directory
	TCHAR cCurrentDir[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, cCurrentDir );
	//(iCMD == CMD_ADB)? _tcscat(cCurrentDir, ADB_PROC_NAME) : _tcscat(cCurrentDir, FASTBOOT_PROC_NAME);	

	PROCESS_INFORMATION processInfo		=   {0};
	STARTUPINFO startupInfo             =   {0};
	ZeroMemory( &startupInfo, sizeof(startupInfo) );
	ZeroMemory( &processInfo, sizeof(processInfo) );
	startupInfo.cb                      =   sizeof(startupInfo);
	startupInfo.dwFlags					=   STARTF_USESTDHANDLES;
	startupInfo.hStdInput				=   rPipe;
	startupInfo.hStdOutput				=   wPipe;
	startupInfo.hStdError				=   wPipe;
	int nStrBuffer                      =   cmdLine.GetLength() + 1;
		
	DbgPrint("[ADB-EXE]Execute command  : ", cmdLine);
	Sleep(100);

	// Create the process
	BOOL result = CreateProcess( NULL, cmdLine.GetBuffer(nStrBuffer), 
								 NULL, NULL, TRUE, 
								 HIGH_PRIORITY_CLASS | CREATE_NO_WINDOW, 
								 NULL, cCurrentDir, &startupInfo, &processInfo );

	cmdLine.ReleaseBuffer();

	if ( !result )
	{
		DbgPrint("CreateProcess retrun zero!!!!!!!!!!!!!");

		// CreateProcess() failed - Get the error from the system
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );

		// Display the error
		CString strError = (LPTSTR) lpMsgBuf;
		TRACE(_T("::AdbExeCommand() failed at CreateProcess()\nCommand=%s\nMessage=%s\n\n"), cmdLine, strError);

		// Free resources created by the system
		LocalFree( lpMsgBuf );

		// We failed.
		return FALSE;
	}
	else
	{
		//WaitForSingleObject( processInfo.hProcess, 2000 );
		// Successfully created the process.  Wait for it to finish.
		if( WaitForSingleObject( processInfo.hProcess, ADB_QUERY_TIMEOUT ) == WAIT_TIMEOUT )
		{
			DbgPrint("CreateProcess Timeout...");
			CloseHandle( wPipe );
			CloseHandle( rPipe );
			CloseHandle( processInfo.hProcess );
			CloseHandle( processInfo.hThread );
			AfxMessageBox(L"CreateProcess Timeout...");
			return FALSE;
		}

		// After process finished, now read the output pipe here.
		char buf[BUF_SIZE]={0};
		DWORD reDword;
		string m_csOutput, csTemp;
		BOOL res=TRUE;

		// Close the pipe write handle
		CloseHandle( wPipe );

		int iCount=0;
		while( (res) && (iCount < 10 ) )
		{
			//DbgPrint("HI~Got Some message from rPipe-1");
			if( res = ::ReadFile( rPipe, buf, BUF_SIZE, &reDword, 0 ) )
			{
				csTemp = buf;
				m_csOutput += csTemp;
			}
			iCount++;
			DbgPrint("HI~");
		}

		// Save the output string from console
		strRet = m_csOutput.c_str();

		// Print the log to debugview for debug
		DbgPrint("[ADB-EXE]Command Response : ", strRet);

		// Close the pipe read handle
		CloseHandle( rPipe );

		// Get the exit code.
		//result = GetExitCodeProcess( processInfo.hProcess, &exitCode );

		// Close the handles.
		DbgPrint( "Close the process & thread..." );
		CloseHandle( processInfo.hProcess );
		CloseHandle( processInfo.hThread );

		//if ( 0 ) //!result )
		//{
			// Could not get exit code.
		//	TRACE(_T("Executed command but couldn't get exit code.Command=%s\n"), cmdLine);
		//	DbgPrint("Executed command but couldn't get exit code.Command=%s");
		//	return FALSE;
		//}

		// We succeeded.
		return TRUE;
	}
	return TRUE;
}

//************************************
BOOL CAdbCtrl::AdbExeCommand(CString cmdLine, DWORD& exitCode, CString& strRet, CMD_TYPE iCMD)
{
	int iCounter=0, SecondsToWait=30;
	
	//Create pipes to write and read data
	HANDLE rPipe, wPipe;
	SECURITY_ATTRIBUTES secattr; 
	ZeroMemory( &secattr,sizeof(secattr) );
	secattr.nLength = sizeof(secattr);
	secattr.bInheritHandle = TRUE;

	// Create pipe for redirect stdout
	CreatePipe( &rPipe, &wPipe, &secattr, 0 );

	// Get current file directory
	TCHAR cCurrentDir[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, cCurrentDir );
	(iCMD == CMD_ADB)? _tcscat(cCurrentDir, ADB_PROC_NAME) : _tcscat(cCurrentDir, FASTBOOT_PROC_NAME);	

	PROCESS_INFORMATION processInfo		=   {0};
	STARTUPINFO startupInfo             =   {0};
	ZeroMemory( &startupInfo, sizeof(startupInfo) );
	ZeroMemory( &processInfo, sizeof(processInfo) );
	startupInfo.cb                      =   sizeof(startupInfo);
	startupInfo.dwFlags					=   STARTF_USESTDHANDLES;
	startupInfo.hStdInput				=   rPipe;
	startupInfo.hStdOutput				=   wPipe;
	startupInfo.hStdError				=   wPipe;
	int nStrBuffer                      =   cmdLine.GetLength() + 1;
		
	DbgPrint("[ADB-EXE]Execute command  : ", cmdLine);
	Sleep(100);

	// Create the process
	if (CreateProcess(  cCurrentDir,					// No module name (use command line)
					    cmdLine.GetBuffer(nStrBuffer),  // Command line
						NULL,							// Process handle not inheritable
						NULL,							// Thread handle not inheritable
						TRUE,							// Set handle inheritance to FALSE
						CREATE_NEW_CONSOLE,  		    // use create new console so i can see what the child proc is actually doing - HIGH_PRIORITY_CLASS | CREATE_NO_WINDOW,
						NULL,							// Use parent's environment block
						NULL,							// Use parent's starting directory
						&startupInfo,					// Pointer to STARTUPINFO structure
                        &processInfo) == FALSE )		// Pointer to PROCESS_INFORMATION structure
    { 
		DbgPrint("CreateProcess retrun zero!!!!!!!!!!!!!");

		// CreateProcess() failed - Get the error from the system
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );

		// Display the error
		CString strError = (LPTSTR) lpMsgBuf;
		TRACE(_T("::AdbExeCommand() failed at CreateProcess()\nCommand=%s\nMessage=%s\n\n"), cmdLine, strError);

		// Free resources created by the system
		LocalFree( lpMsgBuf );

		// We failed.
		return FALSE;
	}
	else
	{
		// Successfully created the process.  Wait for it to finish.
		/* A loop to watch the process. Dismissed with SecondsToWait set to 0 */
        GetExitCodeProcess(piProcessInfo.hProcess, &exitCode);

        while (exitCode == STILL_ACTIVE && SecondsToWait != 0)
        {
            GetExitCodeProcess(piProcessInfo.hProcess, &exitCode);
            Sleep(500);
            iCounter += 500;

            if (iCounter > (SecondsToWait * 1000)) 
            { 
                exitCode = 0;
            } 
        }
		
		if (exitCode) // Not timeout
		{
			// After process finished, now read the output pipe here.
			char buf[BUF_SIZE]={0};
			DWORD reDword;
			string m_csOutput, csTemp;
			BOOL res=TRUE;

			while( res ))
			{
				DbgPrint("HI~1");
				if( res = ::ReadFile( rPipe, buf, BUF_SIZE, &reDword, 0 ) )
				{
					csTemp = buf;
					m_csOutput += csTemp;
				}
				DbgPrint("HI~2");
			}
			// Save the output string from console
			strRet = m_csOutput.c_str();
		}
		
		// Print the log to debugview for debug
		DbgPrint("[ADB-EXE]Command Response : ", strRet);

		// Close the pipe handle
		CloseHandle( rPipe );
		CloseHandle( wPipe );

		// Close the handles.
		CloseHandle( processInfo.hProcess );
		CloseHandle( processInfo.hThread );

		// Release buffer
		cmdLine.ReleaseBuffer();
		
		// We succeeded.
		return TRUE;
	}
	cmdLine.ReleaseBuffer();
	return TRUE;
}

