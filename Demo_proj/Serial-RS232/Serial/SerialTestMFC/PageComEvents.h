#ifndef __PAGECOMEVENTS_H
#define __PAGECOMEVENTS_H


/////////////////////////////////////////////////////////////////////////////
// Include headerfiles

#include "Resource.h"
#include "Serial.h"


/////////////////////////////////////////////////////////////////////////////
// CPageComEvents dialog

class CPageComEvents : public CPropertyPage
{
public:
	// Construction
	CPageComEvents(CSerial* pSerial);

public:
	// Attributes
	CSerial* m_pSerial;

public:
	// Dialog Data
	//{{AFX_DATA(CPageComEvents)
	enum { IDD = IDD_COM_EVENTS };
	BOOL	m_fBreak;
	BYTE	m_bEventChar;
	BOOL	m_fCTS;
	BOOL	m_fDSR;
	BOOL	m_fError;
	BOOL	m_fEventChar;
	BOOL	m_fRecv;
	BOOL	m_fRing;
	BOOL	m_fRLSD;
	BOOL	m_fSend;
	//}}AFX_DATA

public:
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageComEvents)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions

	//{{AFX_MSG(CPageComEvents)
	virtual BOOL OnInitDialog();
	afx_msg void OnEventRcvEv();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif // __PAGECOMEVENTS_H
