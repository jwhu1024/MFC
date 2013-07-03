#include "StdAfx.h"
#include "SerialTestMFC.h"
#include "RichCommEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRichCommEdit

BEGIN_MESSAGE_MAP(CRichCommEdit, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRichCommEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRichCommEdit methods

CRichCommEdit::CRichCommEdit()
	: m_pSerial(0)
	, m_fEcho(false)
{
}

CRichCommEdit::~CRichCommEdit()
{
}

void CRichCommEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Send the data through the serial connection (if it's open)
	if (m_pSerial && m_pSerial->IsOpen())
	{
		char ch = static_cast<char>(nChar);
		m_pSerial->Write(&ch,1);
	}
	
	// Use default handling when echo is enabled
	if (m_fEcho)
	{
		CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);
	}
}
