#ifndef __RICHCOMMEDIT_H
#define __RICHCOMMEDIT_H


/////////////////////////////////////////////////////////////////////////////
// Include headerfiles

#include "Serial.h"


/////////////////////////////////////////////////////////////////////////////
// CRichCommEdit window

class CRichCommEdit : public CRichEditCtrl
{
public:
	// Construction
	CRichCommEdit();
	virtual ~CRichCommEdit();

public:
	// Attributes
	CSerial*	m_pSerial;
	bool		m_fEcho;

public:
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichCommEdit)
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions

	//{{AFX_MSG(CRichCommEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif // __RICHCOMMEDIT_H
