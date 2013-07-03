#ifndef __SHEETSETTINGS_H
#define __SHEETSETTINGS_H


/////////////////////////////////////////////////////////////////////////////
// Include headerfiles

#include "Resource.h"
#include "Serial.h"
#include "PageComSettings.h"
#include "PageComEvents.h"


/////////////////////////////////////////////////////////////////////////////
// CSheetSettings

class CSheetSettings : public CPropertySheet
{
public:
	// Construction
	CSheetSettings(CSerial* pSerial, CWnd* pParentWnd = 0, UINT iSelectPage = 0);
	virtual ~CSheetSettings();

public:
	// Attributes
	CPageComSettings m_pageSettings;
	CPageComEvents   m_pageEvents;

public:
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheetSettings)
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions

	//{{AFX_MSG(CSheetSettings)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif // __SHEETSETTINGS_H
