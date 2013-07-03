#ifndef __COMSELECTDLG_H
#define __COMSELECTDLG_H


/////////////////////////////////////////////////////////////////////////////
// Include headerfiles

#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CComSelectDlg dialog

class CComSelectDlg : public CDialog
{
public:
	// Construction
	CComSelectDlg(CWnd* pParent = 0);

public:
	// Attributes
	CString	m_strComPort;

public:
	// Dialog Data
	//{{AFX_DATA(CComSelectDlg)
	enum { IDD = IDD_COM_SELECT };
	int		m_nComIndex;
	//}}AFX_DATA

public:
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK (void);
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions

	//{{AFX_MSG(CComSelectDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif // __COMSELECTDLG_H
