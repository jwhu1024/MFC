// InternetAccessDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CInternetAccessDlg dialog
class CInternetAccessDlg : public CDialog
{
// Construction
public:
	CInternetAccessDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_INTERNETACCESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_PostData;
	afx_msg void OnTestGettest();
	afx_msg void OnTestPosttest();
	afx_msg void OnTestExit();
	afx_msg void OnHelpAboutcwebaccess();
};
