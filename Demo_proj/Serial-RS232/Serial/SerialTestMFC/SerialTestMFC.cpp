#include "StdAfx.h"
#include "SerialTestMFC.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// The one and only CTestApp object

CTestApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CTestApp

BEGIN_MESSAGE_MAP(CTestApp, CWinApp)
	//{{AFX_MSG_MAP(CTestApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_COM_OPEN, OnComOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTestApp methods

CTestApp::CTestApp()
{
}

CTestApp::~CTestApp()
{
}

BOOL CTestApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.
	CMDIFrameWnd* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// try to load shared MDI menus and accelerator table
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_SERIALTYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_SERIALTYPE));

	// The main window has been initialized, so show and update it.
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	return TRUE;
}

int CTestApp::ExitInstance() 
{
	if (m_hMDIMenu)
		FreeResource(m_hMDIMenu);

	if (m_hMDIAccel)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}

void CTestApp::OnComOpen() 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame,m_pMainWnd);

	// Create a new MDI child window
	pFrame->CreateNewChild(RUNTIME_CLASS(CChildFrame), IDR_SERIALTYPE, m_hMDIMenu, m_hMDIAccel);
}

void CTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
