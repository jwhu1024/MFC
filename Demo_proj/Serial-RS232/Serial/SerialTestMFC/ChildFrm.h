#ifndef __CHILDFRM_H
#define __CHILDFRM_H


/////////////////////////////////////////////////////////////////////////////
// Include headerfiles

#include "ChildView.h"


/////////////////////////////////////////////////////////////////////////////
// CChildFrame window

class CChildFrame : public CMDIChildWnd
{
public:
	// Construction
	CChildFrame();
	virtual ~CChildFrame();

public:
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

public:
	// Attributes
	CChildView m_wndView;

protected:
	// Generated message map functions

	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnFileClose();
	afx_msg void OnComClose();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CChildFrame)
};


#endif // __CHILDFRM_H
