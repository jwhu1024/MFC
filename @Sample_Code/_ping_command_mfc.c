// Execute a command and get the results in a CString.
// Synchronously launches a child process and waits up to 2 seconds for completion.
// Uses a pipe to get the output of the child process.
// Does not pipe to stdin of child process.
// Example usage:
//   CString str;
//   str = ExecCmd( "ping 127.0.0.1 -n 99 " );  // This ping command will be terminated early before the -n 99 completes.
//   str.Replace( "\x0d\x0d\x0a", "\x0d\x0a" ); // fixes some ugly non-standard line terminators created by ping.
//
//   str = ExecCmd( "java -version" ); // A more practical usage.
CString ExecCmd( LPCSTR pCmdArg)
{
   //Handle Inheritance - to pipe child's stdout via pipes to parent, handles must be inherited.
   //SECURITY_ATTRIBUTES.bInheritHandle must be TRUE
   //CreateProcess parameter bInheritHandles must be TRUE;
   //STARTUPINFO.dwFlags must have STARTF_USESTDHANDLES set.
   
   CString strResult; // Contains result of cmdArg.
   HANDLE hChildStdoutRd; // Read-side, used in calls to ReadFile() to get child's stdout output.
   HANDLE hChildStdoutWr; // Write-side, given to child process using si struct.
   BOOL fSuccess;
   
   // Create security attributes to create pipe.   
   SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES)} ;
   saAttr.bInheritHandle  = TRUE; // Set the bInheritHandle flag so pipe handles are inherited by child process. Required.
   saAttr.lpSecurityDescriptor = NULL;
   
   // Create a pipe to get results from child's stdout.
   // I'll create only 1 because I don't need to pipe to the child's stdin.
   if ( !CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0) )
   {
       return strResult;
   }
   STARTUPINFO si = { sizeof(STARTUPINFO) }; // specifies startup parameters for child process.
   si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; // STARTF_USESTDHANDLES is Required.
   si.hStdOutput = hChildStdoutWr; // Requires STARTF_USESTDHANDLES in dwFlags.
   si.hStdError   = hChildStdoutWr; // Requires STARTF_USESTDHANDLES in dwFlags.
   // si.hStdInput remains null.
   si.wShowWindow = SW_HIDE; // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.
   
   PROCESS_INFORMATION pi  = { 0 };
   
   // Create the child process.
   fSuccess = CreateProcess(
   NULL,
   (LPSTR)pCmdArg,     // command line
   NULL,               // process security attributes
   NULL,               // primary thread security attributes
   TRUE,               // TRUE=handles are inherited. Required.
   CREATE_NEW_CONSOLE, // creation flags
   NULL,               // use parent's environment
   NULL,               // use parent's current directory
   &si,                // __in, STARTUPINFO pointer
   &pi);               // __out, receives PROCESS_INFORMATION
   
   if (! fSuccess)
   {
	   return strResult;
   }
   
   // Wait until child processes exit. Don't wait forever.
   WaitForSingleObject( pi.hProcess, 2000 );
   TerminateProcess( pi.hProcess, 0 ); // Kill process if it is still running. Tested using cmd "ping blah -n 99"
   
   // Close the write end of the pipe before reading from the read end of the pipe.
   if (!CloseHandle(hChildStdoutWr))
   {
		return strResult;
	}
	
	//Read output from the child process.
	for (;;)
	{
		DWORD dwRead;
		CHAR chBuf[4096];
	
		// Read from pipe that is the standard output for child process.
		bool done = !ReadFile( hChildStdoutRd, chBuf, 4096, &dwRead, NULL) || dwRead == 0;
		if( done )
		{
			break;
		}
	
		// Append result to string.
		strResult += CString( chBuf, dwRead) ;
	}
	
	// Close process and thread handles.
	CloseHandle( hChildStdoutRd );
	
	// CreateProcess docs specify that these must be closed.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return strResult;
}

Unicode use

CString CtestDlg::ExecCmd( LPWSTR pCmdArg)
{
	wchar_t test[64] = L"ping tw.yahoo.com";
	CString strResult; // Contains result of cmdArg.
	HANDLE hChildStdoutRd; // Read-side, used in calls to ReadFile() to get child's stdout output.
	HANDLE hChildStdoutWr; // Write-side, given to child process using si struct.
	BOOL fSuccess;

	SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES)} ;
	saAttr.bInheritHandle  = TRUE; // Set the bInheritHandle flag so pipe handles are inherited by child process. Required.
	saAttr.lpSecurityDescriptor = NULL;

	if ( !CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0) )
	{
		return strResult;
	}
	STARTUPINFO si = { sizeof(STARTUPINFO) }; // specifies startup parameters for child process.
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; // STARTF_USESTDHANDLES is Required.
	si.hStdOutput = hChildStdoutWr; // Requires STARTF_USESTDHANDLES in dwFlags.
	si.hStdError   = hChildStdoutWr; // Requires STARTF_USESTDHANDLES in dwFlags.

	si.wShowWindow = SW_HIDE; // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.

	PROCESS_INFORMATION pi  = { 0 };

	// Create the child process.
	fSuccess = CreateProcess(
		NULL,
		test,     // command line
		NULL,               // process security attributes
		NULL,               // primary thread security attributes
		TRUE,               // TRUE=handles are inherited. Required.
		CREATE_NEW_CONSOLE, // creation flags
		NULL,               // use parent's environment
		NULL,               // use parent's current directory
		&si,                // __in, STARTUPINFO pointer
		&pi);               // __out, receives PROCESS_INFORMATION

	if (! fSuccess)
	{
		return strResult;
	}

	// Wait until child processes exit. Don't wait forever.
	WaitForSingleObject( pi.hProcess, 2000 );
	TerminateProcess( pi.hProcess, 0 ); // Kill process if it is still running. Tested using cmd "ping blah -n 99"

	// Close the write end of the pipe before reading from the read end of the pipe.
	if (!CloseHandle(hChildStdoutWr))
	{
		return strResult;
	}

	//Read output from the child process.
	for (;;)
	{
		DWORD dwRead;
		CHAR chBuf[4096];

		// Read from pipe that is the standard output for child process.
		bool done = !ReadFile( hChildStdoutRd, chBuf, 4096, &dwRead, NULL) || dwRead == 0;
		if( done )
		{
			break;
		}

		// Append result to string.
		strResult += CString( chBuf, dwRead) ;
	}

	// Close process and thread handles.
	CloseHandle( hChildStdoutRd );

	// CreateProcess docs specify that these must be closed.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return strResult;
}