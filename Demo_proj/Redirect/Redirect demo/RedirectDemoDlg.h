// RedirectDemoDlg.h : header file
//

#if !defined(AFX_REDIRECTDEMODLG_H__65B88E86_BC98_11D2_8498_00C0F026E611__INCLUDED_)
#define AFX_REDIRECTDEMODLG_H__65B88E86_BC98_11D2_8498_00C0F026E611__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CRedirectDemoDlg dialog

class CRedirect;

class CRedirectDemoDlg : public CDialog
{
// Construction
public:
	CRedirectDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRedirectDemoDlg)
	enum { IDD = IDD_REDIRECTDEMO_DIALOG };
	CButton	m_BtnOk;
	CEdit	m_EditRun;
	CEdit	m_EditOutput;
	CButton	m_BtnStop;
	CButton	m_BtnRun;
	CButton	m_BtnBrowse;
	BOOL	m_bClearOutput;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRedirectDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hIcon;
	CFont		m_Font;
	CRedirect	*m_pRedirect;

	void		UpdateControls();
	void		OnCancel();

	// Generated message map functions
	//{{AFX_MSG(CRedirectDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonRun();
	afx_msg void OnButtonStop();
	afx_msg void OnUpdateEditRun();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REDIRECTDEMODLG_H__65B88E86_BC98_11D2_8498_00C0F026E611__INCLUDED_)
