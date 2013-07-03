#ifndef __SERIALTESTMFC_H
#define __SERIALTESTMFC_H


/////////////////////////////////////////////////////////////////////////////
// Include headerfiles

#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CTestApp application

class CTestApp : public CWinApp
{
public:
	// Construction
	CTestApp();
	virtual ~CTestApp();

public:
	// Attributes
	HMENU	m_hMDIMenu;
	HACCEL	m_hMDIAccel;

public:
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions

	//{{AFX_MSG(CTestApp)
	afx_msg void OnAppAbout();
	afx_msg void OnComOpen();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif // __SERIALTESTMFC_H
