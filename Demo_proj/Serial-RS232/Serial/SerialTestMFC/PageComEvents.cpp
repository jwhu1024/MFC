#include "StdAfx.h"
#include "SerialTestMFC.h"
#include "PageComEvents.h"
#include "Serial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPageComEvents property page

BEGIN_MESSAGE_MAP(CPageComEvents, CPropertyPage)
	//{{AFX_MSG_MAP(CPageComEvents)
	ON_BN_CLICKED(IDC_EVENT_RCV_EV, OnEventRcvEv)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPageComEvents methods

CPageComEvents::CPageComEvents(CSerial* pSerial)
	: CPropertyPage(CPageComEvents::IDD)
	, m_pSerial(pSerial)
{
	// Obtain all settings from the serial port
	DWORD dwEvents = m_pSerial->GetEventMask();
	m_fBreak     = ((dwEvents & CSerial::EEventBreak) != 0);
	m_fCTS       = ((dwEvents & CSerial::EEventCTS  ) != 0);
	m_fDSR       = ((dwEvents & CSerial::EEventDSR  ) != 0);
	m_fError     = ((dwEvents & CSerial::EEventError) != 0);
	m_fEventChar = ((dwEvents & CSerial::EEventRcvEv) != 0);
	m_fRecv      = ((dwEvents & CSerial::EEventRecv ) != 0);
	m_fRing      = ((dwEvents & CSerial::EEventRing ) != 0);
	m_fRLSD      = ((dwEvents & CSerial::EEventRLSD ) != 0);
	m_fSend      = ((dwEvents & CSerial::EEventSend ) != 0);

	// Obtain the event character
	m_bEventChar = m_pSerial->GetEventChar();

}

void CPageComEvents::DoDataExchange(CDataExchange* pDX)
{
	// Call base class
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CPageComEvents)
	DDX_Check(pDX, IDC_EVENT_BREAK, m_fBreak);
	DDX_Check(pDX, IDC_EVENT_CTS, m_fCTS);
	DDX_Check(pDX, IDC_EVENT_DSR, m_fDSR);
	DDX_Check(pDX, IDC_EVENT_ERROR, m_fError);
	DDX_Check(pDX, IDC_EVENT_RCV_EV, m_fEventChar);
	DDX_Check(pDX, IDC_EVENT_RECV, m_fRecv);
	DDX_Check(pDX, IDC_EVENT_RING, m_fRing);
	DDX_Check(pDX, IDC_EVENT_RLSD, m_fRLSD);
	DDX_Check(pDX, IDC_EVENT_SEND, m_fSend);
	DDX_Text(pDX, IDC_EVENT_CHAR_VALUE, m_bEventChar);
	DDV_MinMaxByte(pDX, m_bEventChar, 0, 255);
	//}}AFX_DATA_MAP
}

BOOL CPageComEvents::OnInitDialog() 
{
	// Call base class
	CPropertyPage::OnInitDialog();

	// Update event character settings
	OnEventRcvEv();
	return TRUE;
}

BOOL CPageComEvents::OnApply() 
{
	// Retrieve the settings from the page
	UpdateData(true);

	// Determine the events
	DWORD dwEvents = 0;
	if (m_fBreak)     dwEvents |= CSerial::EEventBreak;
	if (m_fCTS)       dwEvents |= CSerial::EEventCTS;
	if (m_fDSR)       dwEvents |= CSerial::EEventDSR;
	if (m_fError)     dwEvents |= CSerial::EEventError;
	if (m_fEventChar) dwEvents |= CSerial::EEventRcvEv;
	if (m_fRecv)      dwEvents |= CSerial::EEventRecv;
	if (m_fRing)      dwEvents |= CSerial::EEventRing;
	if (m_fRLSD)      dwEvents |= CSerial::EEventRLSD;
	if (m_fSend)      dwEvents |= CSerial::EEventSend;
	
	// Setup handshaking
	if (m_pSerial->SetMask(dwEvents))
		return false;

	// Setup the event character
	if (m_pSerial->SetEventChar(m_bEventChar,false))
		return false;

	// Call base class
	return CPropertyPage::OnApply();
}

void CPageComEvents::OnEventRcvEv() 
{
	// Determine whether the event character is being used
	BOOL fUseEventChar = (SendDlgItemMessage(IDC_EVENT_RCV_EV,BM_GETCHECK) == BST_CHECKED);

	// Enable/disable event character windows
	GetDlgItem(IDC_EVENT_CHAR)->EnableWindow(fUseEventChar);
	GetDlgItem(IDC_EVENT_CHAR_TEXT)->EnableWindow(fUseEventChar);
	GetDlgItem(IDC_EVENT_CHAR_VALUE)->EnableWindow(fUseEventChar);
}
