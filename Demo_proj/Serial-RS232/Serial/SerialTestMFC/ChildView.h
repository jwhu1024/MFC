#ifndef __CHILDVIEW_H
#define __CHILDVIEW_H


/////////////////////////////////////////////////////////////////////////////
// Include headerfiles

#include "SerialMFC.h"
#include "RichCommEdit.h"


/////////////////////////////////////////////////////////////////////////////
// CChildView window

class CChildView : public CWnd
{
public:
	// Construction
	CChildView();
	virtual ~CChildView();

public:
	// Attributes
	CRichCommEdit	m_wndEdit;
	CSerialMFC		m_serial;

public:
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions

	//{{AFX_MSG(CChildView)
	afx_msg void OnComSettings();
	afx_msg void OnUpdateComSettings(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCommandSendFile();
	afx_msg void OnCommandSendBreak();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnSerialMsg (WPARAM wParam, LPARAM lParam);

protected:
	// Implementation
	void DisplayEvent (LPCTSTR lpszMsg);
	void DisplayEventSetting (LPCTSTR lpszMsg, LPCTSTR lpszSetting, bool fOn);
	void DisplayData (LPCTSTR pszData);

	DECLARE_MESSAGE_MAP()
};


#endif // __CHILDVIEW_H
