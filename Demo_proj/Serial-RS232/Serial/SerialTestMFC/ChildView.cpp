#include "StdAfx.h"
#include "SerialTestMFC.h"
#include "ChildView.h"
#include "ComSelectDlg.h"
#include "SheetSettings.h"

#ifdef _UNICODE
#include <malloc.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CChildView

BEGIN_MESSAGE_MAP(CChildView,CWnd)
	//{{AFX_MSG_MAP(CChildView)
	ON_COMMAND(ID_COM_SETTINGS, OnComSettings)
	ON_UPDATE_COMMAND_UI(ID_COM_SETTINGS, OnUpdateComSettings)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_COMMANDS_SENDFILE, OnCommandSendFile)
	ON_COMMAND(ID_COMMANDS_SENDBREAK, OnCommandSendBreak)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_WM_SERIAL(OnSerialMsg)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView methods

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
	// Close the serial port
	m_serial.Close();
}

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// Check if the window can be created
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	// Change the styles
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,::LoadCursor(0, IDC_ARROW), HBRUSH(COLOR_WINDOW), 0);

	return TRUE;
}

void CChildView::OnComSettings() 
{
	// Display the COM settings dialog box
	CSheetSettings sheetSettings(&m_serial,this);
	sheetSettings.DoModal();
}

void CChildView::OnUpdateComSettings(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_serial.GetCommHandle() != INVALID_HANDLE_VALUE);
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// Call base class
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the rich edit control
	VERIFY(m_wndEdit.Create(WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_LEFT|ES_MULTILINE,CRect(0,0,0,0),this,0));

	// Obtain the requested COM port
	CComSelectDlg dlgComSelect(this);
	dlgComSelect.DoModal();

	// Open the serial port
	if (m_serial.Open(dlgComSelect.m_strComPort,this) != ERROR_SUCCESS)
	{
		AfxMessageBox(_T("Unable to open COM-port"),MB_ICONSTOP|MB_OK);
		GetParent()->PostMessage(WM_CLOSE);
		return 0;
	}

	// Set the serial port in the RichCommEdit control
	m_wndEdit.m_pSerial = &m_serial;
	return 0;
}

void CChildView::OnSize(UINT nType, int cx, int cy) 
{
	// Call base class
	CWnd::OnSize(nType, cx, cy);
	
	// Size the controls as well
	m_wndEdit.SetWindowPos(0,0,0,cx,cy,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER);
}

LRESULT CChildView::OnSerialMsg (WPARAM wParam, LPARAM /*lParam*/)
{
	CSerial::EEvent eEvent = CSerial::EEvent(LOWORD(wParam));
	CSerial::EError eError = CSerial::EError(HIWORD(wParam));

	if (eError)
		DisplayEvent(_T("An internal error occurred."));

	if (eEvent & CSerial::EEventBreak)
		DisplayEvent(_T("Break detected on input."));

	if (eEvent & CSerial::EEventError)
		DisplayEvent(_T("A line-status error occurred."));
	
	if (eEvent & CSerial::EEventRcvEv)
		DisplayEvent(_T("Event character has been received."));

	if (eEvent & CSerial::EEventRing)
		DisplayEvent(_T("Ring detected"));
	
	if (eEvent & CSerial::EEventSend)
		DisplayEvent(_T("All data is send"));
	
	if (eEvent & CSerial::EEventCTS)
		DisplayEventSetting(_T("CTS signal change"), _T("CTS"), m_serial.GetCTS());
	
	if (eEvent & CSerial::EEventDSR)
		DisplayEventSetting(_T("DSR signal change"), _T("DSR"), m_serial.GetDSR());
	
	if (eEvent & CSerial::EEventRLSD)
		DisplayEventSetting(_T("RLSD signal change"), _T("RLSD"), m_serial.GetRLSD());
	
	if (eEvent & CSerial::EEventRecv)
	{
		// Create a clean buffer
		DWORD dwRead;
		char szData[101];
		const int nBuflen = sizeof(szData)-1;

		// Obtain the data from the serial port
		do
		{
			m_serial.Read(szData,nBuflen,&dwRead);
			szData[dwRead] = '\0';

			// Scan the string for unwanted characters
			for (DWORD dwChar=0; dwChar<dwRead; dwChar++)
			{
				if (!isprint(szData[dwChar]) && !isspace(szData[dwChar]))
				{
					szData[dwChar] = '.';
				}
			}

#ifdef _UNICODE
			// Convert the ANSI data to Unicode
			LPTSTR lpszData = LPTSTR(_alloca((dwRead+1)*sizeof(TCHAR)));
			if (!::MultiByteToWideChar(CP_ACP, 0, szData, -1, lpszData, dwRead+1))
				return 0;

			// Display the fetched string
			DisplayData(lpszData);
#else
			// Display the fetched string
			DisplayData(szData);
#endif
		} while (dwRead == nBuflen);
	}

	return 0;
}

void CChildView::DisplayEvent (LPCTSTR lpszMsg)
{
	// Format the selection as red text
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR;
	m_wndEdit.GetSelectionCharFormat(cf);
	cf.crTextColor = 0x000000FF;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	m_wndEdit.SetSelectionCharFormat(cf);

	// Display the text
	TCHAR tszMsg[200];
	wsprintf(tszMsg,_T("[%s]"), lpszMsg);
	m_wndEdit.SetSel(-1,-1);
	m_wndEdit.ReplaceSel(tszMsg);
}

void CChildView::DisplayEventSetting (LPCTSTR lpszMsg, LPCTSTR lpszSetting, bool fOn)
{
	TCHAR tszMsg[200];
	wsprintf(tszMsg,_T("%s (%s=%s)"), lpszMsg, lpszSetting, (fOn?_T("on"):_T("off")));
	DisplayEvent(tszMsg);
}

void CChildView::DisplayData (LPCTSTR pszData)
{
	// Format the selection as default text
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR;
	m_wndEdit.GetSelectionCharFormat(cf);
	cf.dwEffects |= CFE_AUTOCOLOR;
	m_wndEdit.SetSelectionCharFormat(cf);

	// Add the event to the edit control
	m_wndEdit.SetSel(-1,-1);
	m_wndEdit.ReplaceSel(pszData);
}

void CChildView::OnCommandSendFile() 
{
	// Select the file
	CFileDialog dlgFileOpen(TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,_T("All files (*.*)|*.*||"),this);
	if (dlgFileOpen.DoModal() != IDOK)
		return;

	// Open the file
	HANDLE hFile = ::CreateFile(dlgFileOpen.GetPathName(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		// Obtain file length
		DWORD dwDataLen = ::GetFileSize(hFile,0);

		// Map the file into memory
		HANDLE hMap = ::CreateFileMapping(hFile,NULL,PAGE_READONLY,0,0,NULL);
		if (hMap != NULL)
		{
			LPVOID lpData = ::MapViewOfFile(hMap,FILE_MAP_READ,0,0,0);
			if (lpData)
			{
				// Write all data
				DWORD dwWritten = 0;
				m_serial.Write(lpData,dwDataLen,&dwWritten);

				// Display event
				CString strEvent;
				strEvent.Format(_T("Written %d bytes."), dwWritten);
				DisplayEvent(strEvent);

				// Unmap the view
				::UnmapViewOfFile(lpData);
			}

			// Close mapping
			::CloseHandle(hMap);
		}

		// Close file
		::CloseHandle(hFile);
	}
}

void CChildView::OnCommandSendBreak() 
{
	// Send the break
	m_serial.Break();
}

void CChildView::OnSetFocus(CWnd* pOldWnd) 
{
	// Call base class
	CWnd::OnSetFocus(pOldWnd);
	
	// Pass focus to edit-window
	m_wndEdit.SetFocus();
}
