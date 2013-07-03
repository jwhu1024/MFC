// RedirectDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RedirectDemo.h"
#include "RedirectDemoDlg.h"
#include "Redirect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int FILENAMES_BUF_SIZE	= 1024;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRedirectDemoDlg dialog

CRedirectDemoDlg::CRedirectDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRedirectDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRedirectDemoDlg)
	m_bClearOutput = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon		= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pRedirect	= 0;
	m_bClearOutput = TRUE;
}

void CRedirectDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRedirectDemoDlg)
	DDX_Control(pDX, IDOK, m_BtnOk);
	DDX_Control(pDX, IDC_EDIT_RUN, m_EditRun);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, m_EditOutput);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_BtnStop);
	DDX_Control(pDX, IDC_BUTTON_RUN, m_BtnRun);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_BtnBrowse);
	DDX_Check(pDX, IDC_CHECK_CLEAR, m_bClearOutput);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRedirectDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CRedirectDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_RUN, OnButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_EN_UPDATE(IDC_EDIT_RUN, OnUpdateEditRun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRedirectDemoDlg message handlers

BOOL CRedirectDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	//--------------------------------------------------------------------------
	//	Create a fix-spaced font to use in output edit control.
	//--------------------------------------------------------------------------
	CClientDC dc(this);
	int	nHeight = -((dc.GetDeviceCaps(LOGPIXELSY) * 10) / 72);

	m_Font.CreateFont
	(
		nHeight,
		0,
		0,
		0,
		FW_NORMAL,
		0,
		0,
		0,
		DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		_T("Courier New")
	);
	
	m_EditOutput.SetFont(&m_Font);
	m_BtnStop.EnableWindow(FALSE);
	m_BtnRun.EnableWindow(FALSE);	

	m_EditRun.LimitText(0x20000);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRedirectDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ( ((nID & 0x0FFF0) != SC_CLOSE) || (m_pRedirect == NULL) )
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRedirectDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRedirectDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRedirectDemoDlg::OnButtonBrowse() 
{
	static TCHAR	szFilter[] = _T("Executable Files (*.exe;*.bat)|*.exe;*.bat|||");
	TCHAR			filenamesBuf[FILENAMES_BUF_SIZE];

	CFileDialog	dlg
	(
		TRUE,	// TRUE for FileOpen, FALSE for FileSaveAs
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilter,
		NULL
	);

	filenamesBuf[0]      = 0;
	dlg.m_ofn.lpstrFile  = filenamesBuf;
	dlg.m_ofn.nMaxFile   = FILENAMES_BUF_SIZE;

	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		CString Command = dlg.GetPathName();
		m_EditRun.SetWindowText(Command);
	}
}

void CRedirectDemoDlg::OnButtonRun()
{
	CString Command;

	UpdateData();

	if ( m_bClearOutput )
	{
		m_EditOutput.SetWindowText(_T(""));
	}

	m_EditRun.GetWindowText(Command);

	m_pRedirect = new CRedirect((LPTSTR)(LPCTSTR)Command,  &m_EditOutput);

	UpdateControls();

	m_pRedirect->Run();

	delete m_pRedirect;

	m_pRedirect = 0;

	UpdateControls();

	m_EditRun.SetFocus();
	m_EditRun.SetSel(0, -1);
}

void CRedirectDemoDlg::OnButtonStop() 
{
	if ( m_pRedirect )
	{
		m_pRedirect->Stop();
	}	
}

void CRedirectDemoDlg::UpdateControls()
{
	CMenu* pSysMenu = GetSystemMenu(FALSE);

	if ( !pSysMenu )
	{
		return;
	}

	if ( m_pRedirect )
	{
		m_EditRun.EnableWindow(FALSE);
		m_BtnBrowse.EnableWindow(FALSE);
		m_BtnRun.EnableWindow(FALSE);
		m_BtnStop.EnableWindow(TRUE);
		m_BtnOk.EnableWindow(FALSE);

		//----------------------------------------------------------------------
		//	Disable Close in System Menu (this also disables close box).
		//----------------------------------------------------------------------
		if (pSysMenu != NULL)
		{
			pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
		}
	}
	else 
	{
		m_EditRun.EnableWindow(TRUE);
		m_BtnBrowse.EnableWindow(TRUE);
		m_BtnRun.EnableWindow(m_EditRun.GetWindowTextLength() > 0);
		m_BtnStop.EnableWindow(FALSE);
		m_BtnOk.EnableWindow(TRUE);

		//----------------------------------------------------------------------
		//	Enable Close in System Menu (this also enables close box).
		//----------------------------------------------------------------------
		if (pSysMenu != NULL)
		{
			pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
		}

	}
}

void CRedirectDemoDlg::OnUpdateEditRun() 
{
	UpdateControls();	
}

void CRedirectDemoDlg::OnCancel()
{
	if ( !m_pRedirect )
	{
		CDialog::OnCancel();
	}

}
