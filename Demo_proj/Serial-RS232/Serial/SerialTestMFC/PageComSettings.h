#ifndef __PAGECOMSETTINGS_H
#define __PAGECOMSETTINGS_H


/////////////////////////////////////////////////////////////////////////////
// Include headerfiles

#include "Resource.h"
#include "Serial.h"


/////////////////////////////////////////////////////////////////////////////
// CPageComSettings dialog

class CPageComSettings : public CPropertyPage
{
public:
	// Construction
	CPageComSettings(CSerial* pSerial);

public:
	// Attributes
	CSerial* m_pSerial;

public:
	// Dialog Data
	//{{AFX_DATA(CPageComSettings)
	enum { IDD = IDD_COM_SETTINGS };
	int		m_nBaudrateIndex;
	int		m_nDataIndex;
	int		m_nHandshakingIndex;
	int		m_nParityIndex;
	int		m_nPortIndex;
	int		m_nStopIndex;
	//}}AFX_DATA

public:
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageComSettings)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions

	//{{AFX_MSG(CPageComSettings)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif // __PAGECOMSETTINGS_H
