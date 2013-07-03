#include "StdAfx.h"
#include "SerialTestMFC.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg methods

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

BOOL CAboutDlg::OnInitDialog() 
{
	// Call base class
	CDialog::OnInitDialog();
	
	LOGFONT logFont = {0};

	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = 24;
	logFont.lfWeight = FW_BOLD;
	lstrcpy(logFont.lfFaceName, _T("Arial"));
	
	HFONT hFont = ::CreateFontIndirect(&logFont);

	SendDlgItemMessage(IDC_ABOUT_TITLE, WM_SETFONT, LPARAM(hFont), FALSE);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
