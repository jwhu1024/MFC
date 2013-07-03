// InternetAccessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InternetAccess.h"
#include "InternetAccessDlg.h"
#include "WebAccess.h"
#include ".\internetaccessdlg.h"
#include "About.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CInternetAccessDlg dialog
#define URL			"www.wedcam.co.uk"
#define GET_DATA	"application/showall.php?vid=30"
#define POST_OBJECT "application/get_sched.php"
#define POST_DATA	"venue_id=25&date=2005-3-12"


CInternetAccessDlg::CInternetAccessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInternetAccessDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInternetAccessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_POST_DATA, m_PostData);
}

BEGIN_MESSAGE_MAP(CInternetAccessDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TEST_GETTEST, OnTestGettest)
	ON_COMMAND(ID_TEST_POSTTEST, OnTestPosttest)
	ON_COMMAND(ID_TEST_EXIT, OnTestExit)
	ON_COMMAND(ID_HELP_ABOUTCWEBACCESS, OnHelpAboutcwebaccess)
END_MESSAGE_MAP()


// CInternetAccessDlg message handlers

BOOL CInternetAccessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	// Read information
	CWebAccess	webAccess;
	

	if( webAccess.m_bProxyEnable )
	{
		SetDlgItemText(IDC_CONNECTION_TYPE, "Proxy");
		SetDlgItemText(IDC_PROXY, webAccess.m_szProxyServer);
		SetDlgItemText(IDC_BYPASS, webAccess.m_szProxyOverride);
	}
	else
	{
		SetDlgItemText(IDC_CONNECTION_TYPE, "Direct");
		SetDlgItemText(IDC_PROXY, "N/A");
		SetDlgItemText(IDC_BYPASS, "N/A");
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CInternetAccessDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CInternetAccessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CInternetAccessDlg::OnTestGettest()
{
	m_PostData.SetCheck(FALSE);
	CString url, urlData;
	url = URL;
	SetDlgItemText(IDC_URL, url);

	urlData = GET_DATA;
	SetDlgItemText(IDC_URL_DATA, urlData);

	SetDlgItemText(IDC_STATUS, "Sending data");
	CWebAccess webAccess;

	url += "/";
	url += urlData;


	CString result;
	webAccess.Get(url, result);

	SetDlgItemText(IDC_STATUS, "Data Sent");
	SetDlgItemText(IDC_DATA_RECEIVED, result);
}

void CInternetAccessDlg::OnTestPosttest()
{
	CString url, urlData;
	url = URL;
	url += "/";
	url	+=POST_OBJECT;
	urlData = POST_DATA;
	SetDlgItemText(IDC_URL, url);

	SetDlgItemText(IDC_URL_DATA, urlData);
	m_PostData.SetCheck(TRUE);
	SetDlgItemText(IDC_STATUS, "Sending data");
	CWebAccess webAccess;
	CString result;
	webAccess.Post(url, urlData, result);
	SetDlgItemText(IDC_STATUS, "Data Sent");
	SetDlgItemText(IDC_DATA_RECEIVED, result);
}

void CInternetAccessDlg::OnTestExit()
{
	EndDialog(1);
}

void CInternetAccessDlg::OnHelpAboutcwebaccess()
{
	CAbout about;
	about.DoModal();
}
