// LogTestDlg.h : header file
//

#if !defined(AFX_LOGTESTDLG_H__875D4572_D12A_4BD2_9F47_87A0F1DFBEE8__INCLUDED_)
#define AFX_LOGTESTDLG_H__875D4572_D12A_4BD2_9F47_87A0F1DFBEE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLog;

/////////////////////////////////////////////////////////////////////////////
// CLogTestDlg dialog

class CLogTestDlg : public CDialog
{
  public:
	  CLogTestDlg(CWnd* pParent = NULL);	// standard constructor
    ~CLogTestDlg( void );

	  //{{AFX_DATA(CLogTestDlg)
  	enum { IDD = IDD_LOGTEST_DIALOG };
	  CString	m_strPath;
	  long	m_lMaxLevel;
	  BOOL	m_bChecked;
  	//}}AFX_DATA

	  //{{AFX_VIRTUAL(CLogTestDlg)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	  //}}AFX_VIRTUAL

  protected:
	  HICON m_hIcon;

  private:
    CLog *m_pLog; // pointer to instance of LOG Class

  protected:
	  //{{AFX_MSG(CLogTestDlg)
	  virtual BOOL OnInitDialog();
	  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	  afx_msg void OnPaint();
	  afx_msg HCURSOR OnQueryDragIcon();
	  afx_msg void OnCheck1();
	  afx_msg void OnCheck2();
	  afx_msg void OnCheck3();
	  afx_msg void OnButton1();
	  afx_msg void OnWindowlog();
	  afx_msg void OnFilelog();
	afx_msg void OnCheck4();
	afx_msg void OnChangeEdit2();
	//}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_LOGTESTDLG_H__875D4572_D12A_4BD2_9F47_87A0F1DFBEE8__INCLUDED_)
