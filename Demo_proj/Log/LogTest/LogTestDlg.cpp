// LogTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogTest.h"
#include "LogTestDlg.h"

#pragma warning( disable: 4786 ) // disable STL warnings

#include "clog.h"
#include "cfunclog.h"
#include "cwinlog.h"
#include "cfilelog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
//  

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

//////////////////////////////////////////////////////////////////////////
//  

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

//////////////////////////////////////////////////////////////////////////
//  

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogTestDlg dialog

CLogTestDlg::CLogTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogTestDlg::IDD, pParent)
  , m_pLog( NULL )
{
	//{{AFX_DATA_INIT(CLogTestDlg)
	m_strPath = _T("c:\\logtest.log");
	m_lMaxLevel = LOG_MAX_LEVEL;
	m_bChecked = FALSE;
	//}}AFX_DATA_INIT

  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_pLog = new CLog( new CWinLog(), m_lMaxLevel );
}

//////////////////////////////////////////////////////////////////////////
//  

CLogTestDlg::~CLogTestDlg( void )
{
  if( m_pLog != NULL )
    delete m_pLog, m_pLog = NULL;
}

//////////////////////////////////////////////////////////////////////////
//  

void CLogTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogTestDlg)
	DDX_Text(pDX, IDC_EDIT1, m_strPath);
	DDX_Text(pDX, IDC_EDIT2, m_lMaxLevel);
	DDV_MinMaxLong(pDX, m_lMaxLevel, 0, 2);
	DDX_Check(pDX, IDC_CHECK4, m_bChecked);
	//}}AFX_DATA_MAP
}

//////////////////////////////////////////////////////////////////////////
//  

BEGIN_MESSAGE_MAP(CLogTestDlg, CDialog)
	//{{AFX_MSG_MAP(CLogTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_WINDOWLOG, OnWindowlog)
	ON_BN_CLICKED(IDC_FILELOG, OnFilelog)
	ON_BN_CLICKED(IDC_CHECK4, OnCheck4)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeEdit2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogTestDlg message handlers

BOOL CLogTestDlg::OnInitDialog()
{
  CFuncLog  log( m_pLog, "CLogTestDlg::OnInitDialog" );

	CDialog::OnInitDialog();

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

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
  log.LogString( LOG_WARNING, "Log something simple!!!" );

  ::SendMessage( ::GetDlgItem( m_hWnd, IDC_WINDOWLOG ), BM_SETCHECK, BST_CHECKED, NULL );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//////////////////////////////////////////////////////////////////////////
//  

void CLogTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CFuncLog log( m_pLog, "CLogTestDlg::OnSysCommand" );
  
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLogTestDlg::OnPaint() 
{
  CFuncLog log( m_pLog, "CLogTestDlg::OnPaint" );

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

//////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.

HCURSOR CLogTestDlg::OnQueryDragIcon()
{
  return (HCURSOR) m_hIcon;
}

//////////////////////////////////////////////////////////////////////////
//  

void CLogTestDlg::OnCheck1() 
{
  CFuncLog log( m_pLog, "CLogTestDlg::OnCheck1" );
  
  log.LogString( LOG_INFO, "Now we are in OnCheck1 function body!" );
  log.LogString( LOG_WARNING, "Warning Message From OnCheck1 function body!" );
  log.LogString( LOG_ERROR, "Error Message From OnCheck1 function body!" );
}

//////////////////////////////////////////////////////////////////////////
//  

void CLogTestDlg::OnCheck2() 
{
  CFuncLog log( m_pLog, "CLogTestDlg::OnCheck2" );

  UpdateData();

  log << "This is a raw data output: " << log.endl();
  log << "m_strPath == " << ( LPCSTR )m_strPath << log.endl();
  log << "m_lMaxLevel ==" << m_lMaxLevel << log.endl();

}

//////////////////////////////////////////////////////////////////////////
//  

void CLogTestDlg::OnCheck3() 
{
  CFuncLog log( m_pLog, "CLogTestDlg::OnCheck3" );
  
  UpdateData();

  log.LogString( LOG_INFO, 
    log.FormatString( " m_strPath == %s m_lMaxLevel == %d", 
      ( LPCSTR )m_strPath, m_lMaxLevel ) );

  log.LogString( LOG_WARNING, 
    log.FormatString( " m_strPath == %s m_lMaxLevel == %d",
      ( LPCSTR )m_strPath, m_lMaxLevel ) );

  log.LogString( LOG_ERROR, 
    log.FormatString( " m_strPath == %s m_lMaxLevel == %d", 
      ( LPCSTR )m_strPath, m_lMaxLevel ) );
}

//////////////////////////////////////////////////////////////////////////
//  

void CLogTestDlg::OnButton1() 
{
  UpdateData();

	CFileDialog dlg( FALSE, ".log" );

  int result = dlg.DoModal();
	
  if( result == IDOK )
  {
    m_strPath = dlg.GetFileName();
    UpdateData( FALSE );

    delete m_pLog, m_pLog = NULL;
    m_pLog = new CLog( new CFileLog( ( LPCSTR )m_strPath ), LOG_MAX_LEVEL );
  }
}

//////////////////////////////////////////////////////////////////////////
//  

void CLogTestDlg::OnWindowlog() 
{
  UpdateData();

  ::EnableWindow( ::GetDlgItem( m_hWnd, IDC_EDIT1 ), FALSE );
  ::EnableWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON1 ), FALSE );

  if( m_pLog )
    delete m_pLog, m_pLog = NULL;

  m_pLog = new CLog( new CWinLog(), m_lMaxLevel );
  m_pLog->LogRawString( "init Windows Output!!!\n" );

  UpdateData( FALSE );
}

//////////////////////////////////////////////////////////////////////////
//  

void CLogTestDlg::OnFilelog() 
{
  UpdateData( );

  ::EnableWindow( ::GetDlgItem( m_hWnd, IDC_EDIT1 ), TRUE );
  ::EnableWindow( ::GetDlgItem( m_hWnd, IDC_BUTTON1 ), TRUE );

  if( m_pLog )
    delete m_pLog, m_pLog = NULL;

  int result = ::MessageBox( m_hWnd, "Do you want to create file logging?\n"
    "If you want to select output name first press No.", "WARNING", MB_YESNO );

  if( result == IDYES )
  {
    m_pLog = new CLog( new CFileLog( ( LPCSTR )m_strPath ), m_lMaxLevel );
    m_pLog->LogRawString( "init File Output!!!\n" );
  }
  
  UpdateData( FALSE );
}

//////////////////////////////////////////////////////////////////////////
//  

void CLogTestDlg::OnCheck4() 
{
  UpdateData();

  if( m_pLog )
    m_pLog->SetAutoFlush( ( m_bChecked == TRUE ) );
}

//////////////////////////////////////////////////////////////////////////
//  

void CLogTestDlg::OnChangeEdit2() 
{
  if( IsWindowVisible() == FALSE ) return;
  UpdateData();

  if( m_lMaxLevel > 2 ) m_lMaxLevel = 2;
  if( m_lMaxLevel < 0 ) m_lMaxLevel = 0;

  UpdateData( FALSE );
}

//:> end of file
