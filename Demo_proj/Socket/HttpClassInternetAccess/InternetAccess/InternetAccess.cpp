// InternetAccess.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "InternetAccess.h"
#include "InternetAccessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CInternetAccessApp

BEGIN_MESSAGE_MAP(CInternetAccessApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CInternetAccessApp construction

CInternetAccessApp::CInternetAccessApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CInternetAccessApp object

CInternetAccessApp theApp;


// CInternetAccessApp initialization

BOOL CInternetAccessApp::InitInstance()
{
	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CInternetAccessDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
