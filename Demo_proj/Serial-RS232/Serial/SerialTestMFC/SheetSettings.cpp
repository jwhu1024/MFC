#include "StdAfx.h"
#include "SerialTestMFC.h"
#include "SheetSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSheetSettings

BEGIN_MESSAGE_MAP(CSheetSettings, CPropertySheet)
	//{{AFX_MSG_MAP(CSheetSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSheetSettings methods

CSheetSettings::CSheetSettings(CSerial* pSerial, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(_T("COM Settings"), pParentWnd, iSelectPage)
	, m_pageSettings(pSerial)
	, m_pageEvents(pSerial)
{
	// Remove apply button
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// Add the property pages
	AddPage(&m_pageSettings);
	AddPage(&m_pageEvents);
}

CSheetSettings::~CSheetSettings()
{
}
