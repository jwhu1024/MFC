// About.cpp : implementation file
//

#include "stdafx.h"
#include "InternetAccess.h"
#include "About.h"
#include ".\about.h"


// CAbout dialog

IMPLEMENT_DYNAMIC(CAbout, CDialog)
CAbout::CAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CAbout::IDD, pParent)
{
}

CAbout::~CAbout()
{
}

void CAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAbout, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CAbout message handlers

void CAbout::OnBnClickedOk()
{
	EndDialog(0);
}

void CAbout::OnBnClickedCancel()
{
	EndDialog(1);
}
