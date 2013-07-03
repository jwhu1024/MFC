//------------------------------------------------------------------------------
// Redirect.cpp : implementation file
//
// Creates a child process that runs a user-specified command and redirects its
// standard output and standard error to a CEdit control.
//
// Written by Matt Brunk (brunk@gorge.net)
// Copyright (C) 1999 Matt Brunk
// All rights reserved.
//
// This code may be used in compiled form in any way. This file may be
// distributed by any means providing it is not sold for profit without
// the written consent of the author, and providing that this notice and the
// author's name is included in the distribution. If the compiled form of the
// source code in this file is used in a commercial application, an e-mail to
// the author would be appreciated.
//
// Thanks to Dima Shamroni (dima@abirnet.co.il) for providing the essential
// code for this class.
//
// Thanks to Chris Maunder (chrismaunder@codeguru.com) for the PeekAndPump()
// function (from his CProgressWnd class).
//
// Initial Release Feb 8, 1999
//------------------------------------------------------------------------------


#include "stdafx.h"
#include <string.h>
#include "Redirect.h"

const int BUF_SIZE = 8192;

CRedirect::CRedirect
(
	LPCTSTR		szCommand,
	CEdit		*pEdit,
	LPCTSTR		szCurrentDirectory
)
{
	m_bStopped				= false;
	m_dwSleepMilliseconds	= 100;
	m_pEdit					= pEdit;
	m_szCommand				= szCommand;
	m_szCurrentDirectory	= szCurrentDirectory;
}

CRedirect::~CRedirect()
{
}

void CRedirect::Run()
{
	HANDLE					PipeReadHandle;
	HANDLE					PipeWriteHandle;
	PROCESS_INFORMATION		ProcessInfo;
	SECURITY_ATTRIBUTES		SecurityAttributes;
	STARTUPINFO				StartupInfo;
	BOOL					Success;

	//--------------------------------------------------------------------------
	//	Zero the structures.
	//--------------------------------------------------------------------------
	ZeroMemory( &StartupInfo,			sizeof( StartupInfo ));
	ZeroMemory( &ProcessInfo,			sizeof( ProcessInfo ));
	ZeroMemory( &SecurityAttributes,	sizeof( SecurityAttributes ));

	//--------------------------------------------------------------------------
	//	Create a pipe for the child's STDOUT.
	//--------------------------------------------------------------------------
	SecurityAttributes.nLength              = sizeof(SECURITY_ATTRIBUTES);
	SecurityAttributes.bInheritHandle       = TRUE;
	SecurityAttributes.lpSecurityDescriptor = NULL;

	Success = CreatePipe
	(
		&PipeReadHandle,		// address of variable for read handle
		&PipeWriteHandle,		// address of variable for write handle
		&SecurityAttributes,	// pointer to security attributes
		0						// number of bytes reserved for pipe (use default size)
	);

	if ( !Success )
	{
		ShowLastError(_T("Error creating pipe"));
		return;
	}	

	//--------------------------------------------------------------------------
	//	Set up members of STARTUPINFO structure.
	//--------------------------------------------------------------------------
	StartupInfo.cb           = sizeof(STARTUPINFO);
	StartupInfo.dwFlags      = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	StartupInfo.wShowWindow  = SW_HIDE;
	StartupInfo.hStdOutput   = PipeWriteHandle;
	StartupInfo.hStdError    = PipeWriteHandle;

	//----------------------------------------------------------------------------
	//	Create the child process.
	//----------------------------------------------------------------------------
	Success = CreateProcess
	( 
		NULL,					// pointer to name of executable module
		LPTSTR(m_szCommand),	// command line 
		NULL,					// pointer to process security attributes 
		NULL,					// pointer to thread security attributes (use primary thread security attributes)
		TRUE,					// inherit handles
		0,						// creation flags
		NULL,					// pointer to new environment block (use parent's)
		m_szCurrentDirectory,	// pointer to current directory name
		&StartupInfo,			// pointer to STARTUPINFO
		&ProcessInfo			// pointer to PROCESS_INFORMATION
	);                 

	if ( !Success )
	{
		ShowLastError(_T("Error creating process"));
		return;
	}

	DWORD	BytesLeftThisMessage = 0;
	DWORD	NumBytesRead;
	TCHAR	PipeData[BUF_SIZE]; 
	DWORD	TotalBytesAvailable = 0;

	for ( ; ; )
	{ 
		NumBytesRead = 0;

		Success = PeekNamedPipe
		( 
			PipeReadHandle,				// handle to pipe to copy from 
			PipeData,					// pointer to data buffer 
			1,							// size, in bytes, of data buffer 
			&NumBytesRead,				// pointer to number of bytes read 
			&TotalBytesAvailable,		// pointer to total number of bytes available
			&BytesLeftThisMessage		// pointer to unread bytes in this message 
		);

		if ( !Success )
		{
			ShowLastError(_T("PeekNamedPipe fialed"));
			break;
		}

		if ( NumBytesRead )
		{
			Success = ReadFile
			(
				PipeReadHandle,		// handle to pipe to copy from 
				PipeData,			// address of buffer that receives data
				BUF_SIZE - 1,		// number of bytes to read
				&NumBytesRead,		// address of number of bytes read
				NULL				// address of structure for data for overlapped I/O
			);

			if ( !Success )
			{
				ShowLastError(_T("ReadFile fialed"));
				break;
			}

			//------------------------------------------------------------------
			//	Zero-terminate the data.
			//------------------------------------------------------------------
			PipeData[NumBytesRead] = '\0';

			//------------------------------------------------------------------
			//	Replace backspaces with spaces.
			//------------------------------------------------------------------
			for ( DWORD ii = 0; ii < NumBytesRead; ii++ )
			{
				if ( PipeData[ii] == _T('\b') )
				{
					PipeData[ii] = ' ';
				}
			}
			
			//------------------------------------------------------------------
			//	If we're running a batch file that contains a pause command, 
			//	assume it is the last output from the batch file and remove it.
			//------------------------------------------------------------------
			TCHAR  *ptr = _tcsstr(PipeData, _T("Press any key to continue . . ."));
			if ( ptr )
			{
				*ptr = '\0';
			}

			//------------------------------------------------------------------
			//	Append the output to the CEdit control.
			//------------------------------------------------------------------
			AppendText(PipeData);

			//------------------------------------------------------------------
			//	Peek and pump messages.
			//------------------------------------------------------------------
			PeekAndPump();
		}
		else
		{
			//------------------------------------------------------------------
			//	If the child process has completed, break out.
			//------------------------------------------------------------------
			if ( WaitForSingleObject(ProcessInfo.hProcess, 0) == WAIT_OBJECT_0 )	//lint !e1924 (warning about C-style cast)
			{
				break;
			}

			//------------------------------------------------------------------
			//	Peek and pump messages.
			//------------------------------------------------------------------
			PeekAndPump();

			//------------------------------------------------------------------
			//	If the user cancelled the operation, terminate the process.
			//------------------------------------------------------------------
			if ( m_bStopped )
			{
				Success = TerminateProcess
				(
					ProcessInfo.hProcess,
					0
				);

				if ( Success )
				{
					AppendText(_T("\r\nCancelled.\r\n\r\nProcess terminated successfully.\r\n"));
				}
				else
				{
					ShowLastError(_T("Error terminating process."));
				}

				break;
			}

			//------------------------------------------------------------------
			//	Sleep.
			//------------------------------------------------------------------
			Sleep(m_dwSleepMilliseconds);
		}
		
	}

	//--------------------------------------------------------------------------
	//	Close handles.
	//--------------------------------------------------------------------------
	Success = CloseHandle(ProcessInfo.hThread);
	if ( !Success )
	{
		ShowLastError(_T("Error closing thread handle."));
	}

	Success = CloseHandle(ProcessInfo.hProcess);
	if ( !Success )
	{
		ShowLastError(_T("Error closing process handle."));
	}

	Success = CloseHandle(PipeReadHandle);
	if ( !Success )
	{
		ShowLastError(_T("Error closing pipe read handle."));
	}

	Success = CloseHandle(PipeWriteHandle);
	if ( !Success )
	{
		ShowLastError(_T("Error closing pipe write handle."));
	}

}


void CRedirect::ShowLastError(LPCTSTR szText)
{
	LPVOID		lpMsgBuf;
	DWORD		Success;

	//--------------------------------------------------------------------------
	//	Get the system error message.
	//--------------------------------------------------------------------------
	Success = FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	//lint !e1924 (warning about C-style cast)
		LPTSTR(&lpMsgBuf),
		0,
		NULL
	);

	CString	Msg;

	Msg = szText;
	Msg += _T("\r\n");
	if ( Success )
	{
		Msg += LPTSTR(lpMsgBuf);
	}
	else
	{
		Msg += _T("No status because FormatMessage failed.\r\n");
	}

	AppendText(Msg);

}

void CRedirect::PeekAndPump()
{
    MSG Msg;
    while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE)) 
    {
        (void)AfxGetApp()->PumpMessage(); //lint !e1924 (warning about C-style cast)
    }
}

void CRedirect::Stop()
{
	m_bStopped = true;
}

void CRedirect::AppendText(LPCTSTR Text)
{
	int Length = m_pEdit->GetWindowTextLength();

	m_pEdit->SetSel(Length, Length);
	m_pEdit->ReplaceSel(Text);
	m_pEdit->LineScroll( m_pEdit->GetLineCount() );
}

void CRedirect::SetSleepInterval(DWORD dwMilliseconds)
{
	m_dwSleepMilliseconds = dwMilliseconds;
}
