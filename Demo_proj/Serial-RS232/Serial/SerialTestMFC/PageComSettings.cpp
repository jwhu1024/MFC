#include "StdAfx.h"
#include "SerialTestMFC.h"
#include "PageComSettings.h"
#include "Serial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPageComSettings property page

BEGIN_MESSAGE_MAP(CPageComSettings, CPropertyPage)
	//{{AFX_MSG_MAP(CPageComSettings)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPageComSettings methods

CPageComSettings::CPageComSettings(CSerial* pSerial)
	: CPropertyPage(CPageComSettings::IDD)
	, m_pSerial(pSerial)
{
	// Obtain all settings from the serial port
	CSerial::EBaudrate  eBaudrate  = m_pSerial->GetBaudrate();
	CSerial::EDataBits  eDataBits  = m_pSerial->GetDataBits();
	CSerial::EParity    eParity    = m_pSerial->GetParity();
	CSerial::EStopBits  eStopBits  = m_pSerial->GetStopBits();
	CSerial::EHandshake eHandshake = m_pSerial->GetHandshaking();

	switch (eBaudrate)
	{
	case CSerial::EBaud1200:   m_nBaudrateIndex = 0; break;
	case CSerial::EBaud2400:   m_nBaudrateIndex = 1; break;
	case CSerial::EBaud9600:   m_nBaudrateIndex = 2; break;
	case CSerial::EBaud14400:  m_nBaudrateIndex = 3; break;
	case CSerial::EBaud19200:  m_nBaudrateIndex = 4; break;
	case CSerial::EBaud38400:  m_nBaudrateIndex = 5; break;
	case CSerial::EBaud56000:  m_nBaudrateIndex = 6; break;
	case CSerial::EBaud57600:  m_nBaudrateIndex = 7; break;
	case CSerial::EBaud115200: m_nBaudrateIndex = 8; break;
	default:                   m_nBaudrateIndex = 2; break;
	}

	switch (eDataBits)
	{
	case CSerial::EData5: m_nDataIndex = 0; break;
	case CSerial::EData6: m_nDataIndex = 1; break;
	case CSerial::EData7: m_nDataIndex = 2; break;
	case CSerial::EData8: m_nDataIndex = 3; break;
	default:              m_nDataIndex = 3; break;
	}

	switch (eParity)
	{
	case CSerial::EParNone:  m_nParityIndex = 0; break;
	case CSerial::EParOdd:   m_nParityIndex = 1; break;
	case CSerial::EParEven:  m_nParityIndex = 2; break;
	case CSerial::EParMark:  m_nParityIndex = 3; break;
	case CSerial::EParSpace: m_nParityIndex = 4; break;
	default:                 m_nParityIndex = 0; break;
	}

	switch (eStopBits)
	{
	case CSerial::EStop1:   m_nStopIndex = 0; break;
	case CSerial::EStop1_5: m_nStopIndex = 1; break;
	case CSerial::EStop2:   m_nStopIndex = 2; break;
	default:                m_nStopIndex = 0; break;
	}

	switch (eHandshake)
	{
	case CSerial::EHandshakeOff:      m_nHandshakingIndex = 0; break;
	case CSerial::EHandshakeSoftware: m_nHandshakingIndex = 1; break;
	case CSerial::EHandshakeHardware: m_nHandshakingIndex = 2; break;
	default:                          m_nHandshakingIndex = 2; break;
	}
}

void CPageComSettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageComSettings)
	DDX_Radio(pDX, IDC_BAUDRATE_1200, m_nBaudrateIndex);
	DDX_Radio(pDX, IDC_DATA_5, m_nDataIndex);
	DDX_Radio(pDX, IDC_HANDSHAKING_OFF, m_nHandshakingIndex);
	DDX_Radio(pDX, IDC_PARITY_NONE, m_nParityIndex);
	DDX_Radio(pDX, IDC_STOP_1, m_nStopIndex);
	//}}AFX_DATA_MAP
}

BOOL CPageComSettings::OnApply() 
{
	// Retrieve the settings from the page
	UpdateData(true);

	// Determine baudrate
	CSerial::EBaudrate eBaudrate = CSerial::EBaudUnknown;
	switch (m_nBaudrateIndex)
	{
	case 0:  eBaudrate = CSerial::EBaud1200;   break;
	case 1:  eBaudrate = CSerial::EBaud2400;   break;
	case 2:  eBaudrate = CSerial::EBaud9600;   break;
	case 3:  eBaudrate = CSerial::EBaud14400;  break;
	case 4:  eBaudrate = CSerial::EBaud19200;  break;
	case 5:  eBaudrate = CSerial::EBaud38400;  break;
	case 6:  eBaudrate = CSerial::EBaud56000;  break;
	case 7:  eBaudrate = CSerial::EBaud57600;  break;
	case 8:  eBaudrate = CSerial::EBaud115200; break;
	default: ASSERT(false); break;
	}

	CSerial::EDataBits eDataBits = CSerial::EDataUnknown;
	switch (m_nDataIndex)
	{
	case 0:  eDataBits = CSerial::EData5; break;
	case 1:  eDataBits = CSerial::EData6; break;
	case 2:  eDataBits = CSerial::EData7; break;
	case 3:  eDataBits = CSerial::EData8; break;
	}

	CSerial::EParity eParity = CSerial::EParUnknown;
	switch (m_nParityIndex)
	{
	case 0: eParity = CSerial::EParNone;  break;
	case 1: eParity = CSerial::EParOdd;   break;
	case 2: eParity = CSerial::EParEven;  break;
	case 3: eParity = CSerial::EParMark;  break;
	case 4: eParity = CSerial::EParSpace; break;
	default: ASSERT(false); break;
	}

	CSerial::EStopBits eStopBits = CSerial::EStopUnknown;
	switch (m_nStopIndex)
	{
	case 0: eStopBits = CSerial::EStop1;   break;
	case 1: eStopBits = CSerial::EStop1_5; break;
	case 2: eStopBits = CSerial::EStop2;   break;
	default: ASSERT(false); break;
	}

	CSerial::EHandshake eHandshake = CSerial::EHandshakeUnknown;
	switch (m_nHandshakingIndex)
	{
	case 0: eHandshake = CSerial::EHandshakeOff;      break;
	case 1: eHandshake = CSerial::EHandshakeSoftware; break;
	case 2: eHandshake = CSerial::EHandshakeHardware; break;
	default: ASSERT(false); break;
	}

	// Setup the COM port
	if (m_pSerial->Setup(eBaudrate,eDataBits,eParity,eStopBits))
		return false;

	// Setup handshaking
	if (m_pSerial->SetupHandshaking(eHandshake))
		return false;

	// Call base class
	return CPropertyPage::OnApply();
}
