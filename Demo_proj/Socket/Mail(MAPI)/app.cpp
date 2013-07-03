#include "stdafx.h"
#include "app.h"
#include "cmapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	//{{AFX_MSG_MAP(CApp)
END_MESSAGE_MAP()

CApp::CApp()
{
}

CApp theApp;

BOOL CApp::InitInstance()
{
  //Setup a demo message with some parameters, Some of these recipients may not be reachable
  //depending on your profile settings.
  CMapiMessage message;
  message.m_To.Add(_T("pjn@indigo.ie"));
  message.m_CC.Add(_T("SMTP:pjn@indigo.ie"));
  message.m_BCC.Add(_T("SMTP:pj.naughter@softech-telecom.com"));
  message.m_sSubject = _T("Demo email using CMapiSession - Please ignore");
  message.m_sBody = _T("This is the body of the message\r\nRegards,\r\nCMapiMessage");
  message.m_Attachments.Add(_T("C:\\autoexec.bat"));
  message.m_AttachmentTitles.Add(_T("my autoexec.bat"));

  //Setup the mapi session
  CMapiSession session;
  BOOL bMapiInstalled = session.MapiInstalled();

  //Logon to MAPI
  BOOL bLoggedOn = session.Logon(_T("PJ Naughter")); //You will need to change this to use a MAPI profile
                                                     //setup on your computer. If you want CMapiSession to
                                                     //bring up a dialog to allow a MAPI profile to be 
                                                     //interactively chosen, then you will need to create
                                                     //a Window in your app using the normal MFC mechanims 
  if (bLoggedOn)
  {

    //Send the message
    BOOL bSent = session.Send(message);
  }


	return FALSE;
}

