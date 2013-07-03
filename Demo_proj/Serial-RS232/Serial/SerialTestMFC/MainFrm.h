#ifndef __MAINFRM_H
#define __MAINFRM_H


/////////////////////////////////////////////////////////////////////////////
// CMainFrame window

class CMainFrame : public CMDIFrameWnd
{
public:
	// Construction
	CMainFrame();
	virtual ~CMainFrame();

public:
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	//}}AFX_VIRTUAL

public:
	// Generated message map functions

	//{{AFX_MSG(CMainFrame)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CMainFrame)
};


#endif // __MAINFRM_H
