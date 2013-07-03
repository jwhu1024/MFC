/*
Module : CMAPI.CPP
Purpose: Defines the implementation for an MFC wrapper class for sending an email using simple MAPI
Created: PJN / 11-05-1999
History: PJN / 05-12-1999 1. Fixed potential problem where CMapi is used in a console app which was giving an ASSERT
                          2. Fixed an assert which can occur if you dismiss the login dialog when doing an 
                          interactive MAPI logon

Copyright (c) 1999 by PJ Naughter.  
All rights reserved.


*/



/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#include "CMapi.h"
#include <afxpriv.h>




//////////////////////////////// Statics / Macros /////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif




////////////////////////////////// Implementation /////////////////////////////

CMapiSession::CMapiSession()
{
  m_hSession = 0;
  m_nLastError = 0;
  m_hMapi = NULL;
  m_lpfnMAPILogon = NULL;
  m_lpfnMAPILogoff = NULL;
  m_lpfnMAPISendMail = NULL;
  m_lpfnMAPIResolveName = NULL;
  m_lpfnMAPIFreeBuffer = NULL;
  Initialise();
}

CMapiSession::~CMapiSession()
{
  //Logoff if logged on
  Logoff();

  //Unload the MAPI dll
  Deinitialise();
}

void CMapiSession::Initialise() 
{
  //First make sure the "WIN.INI" entry for MAPI is present aswell 
  //as the MAPI32 dll being present on the system
	BOOL bMapiInstalled = (GetProfileInt(_T("MAIL"), _T("MAPI"), 0) != 0) && 
                        (SearchPath(NULL, _T("MAPI32.DLL"), NULL, 0, NULL, NULL) != 0);

  if (bMapiInstalled)
  {
    //Load up the MAPI dll and get the function pointers we are interested in
    m_hMapi = LoadLibrary(_T("MAPI32.DLL"));
    if (m_hMapi)
    {
      m_lpfnMAPILogon = (LPMAPILOGON) GetProcAddress(m_hMapi, "MAPILogon");
      m_lpfnMAPILogoff = (LPMAPILOGOFF) GetProcAddress(m_hMapi, "MAPILogoff");
      m_lpfnMAPISendMail = (LPMAPISENDMAIL) GetProcAddress(m_hMapi, "MAPISendMail");
      m_lpfnMAPIResolveName = (LPMAPIRESOLVENAME) GetProcAddress(m_hMapi, "MAPIResolveName");
      m_lpfnMAPIFreeBuffer = (LPMAPIFREEBUFFER) GetProcAddress(m_hMapi, "MAPIFreeBuffer");
  
      //If any of the functions are not installed then fail the load
      if (m_lpfnMAPILogon == NULL ||
          m_lpfnMAPILogoff == NULL ||
          m_lpfnMAPISendMail == NULL ||
          m_lpfnMAPIResolveName == NULL ||
          m_lpfnMAPIFreeBuffer == NULL)
      {
        TRACE(_T("Failed to get one of the functions pointer in MAPI32.DLL\n"));
        Deinitialise();
      }
    }
  }
  else
    TRACE(_T("Mapi is not installed on this computer\n"));
}

void CMapiSession::Deinitialise()
{
  if (m_hMapi)
  {
    //Unload the MAPI dll and reset the function pointers to NULL
    FreeLibrary(m_hMapi);
    m_hMapi = NULL;
    m_lpfnMAPILogon = NULL;
    m_lpfnMAPILogoff = NULL;
    m_lpfnMAPISendMail = NULL;
    m_lpfnMAPIResolveName = NULL;
    m_lpfnMAPIFreeBuffer = NULL;
  }
}

BOOL CMapiSession::Logon(const CString& sProfileName, const CString& sPassword, CWnd* pParentWnd)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  ASSERT(MapiInstalled()); //MAPI must be installed
  ASSERT(m_lpfnMAPILogon); //Function pointer must be valid

  //Initialise the function return value
  BOOL bSuccess = FALSE;

  //Just in case we are already logged in
  Logoff();

  //Setup the ascii versions of the profile name and password
  int nProfileLength = sProfileName.GetLength();
  int nPasswordLength = sPassword.GetLength();
  LPSTR pszProfileName = NULL;
  LPSTR pszPassword = NULL;
  if (nProfileLength)
  {
    pszProfileName = T2A((LPTSTR) (LPCTSTR) sProfileName);
    pszPassword = T2A((LPTSTR) (LPCTSTR) sPassword);
  }

  //Setup the flags & UIParam parameters used in the MapiLogon call
  FLAGS flags = 0;
  ULONG nUIParam = 0;
  if (nProfileLength == 0)
  {
    //No profile name given, then we must interactively request a profile name
    if (pParentWnd)
    {
      nUIParam = (ULONG) pParentWnd->GetSafeHwnd();
      flags |= MAPI_LOGON_UI;
    }
    else
    {
      //No CWnd given, just use the main window of the app as the parent window
      if (AfxGetMainWnd())
      {
        nUIParam = (ULONG) AfxGetMainWnd()->GetSafeHwnd();
        flags |= MAPI_LOGON_UI;
      }
    }
  }
  
  //First try to acquire a new MAPI session using the supplied settings using the MAPILogon functio
  ULONG nError = m_lpfnMAPILogon(nUIParam, pszProfileName, pszPassword, flags | MAPI_NEW_SESSION, 0, &m_hSession);
  if (nError != SUCCESS_SUCCESS && nError != MAPI_E_USER_ABORT)
  {
    //Failed to create a create mapi session, try to acquire a shared mapi session
    TRACE(_T("Failed to logon to MAPI using a new session, trying to acquire a shared one\n"));
    nError = m_lpfnMAPILogon(nUIParam, NULL, NULL, 0, 0, &m_hSession);
    if (nError == SUCCESS_SUCCESS)
    {
      m_nLastError = SUCCESS_SUCCESS;
      bSuccess = TRUE;
    }
    else
    {
      TRACE(_T("Failed to logon to MAPI using a shared session, Error:%d\n"), nError);
      m_nLastError = nError;
    }
  }
  else if (nError == SUCCESS_SUCCESS)
  {
    m_nLastError = SUCCESS_SUCCESS;
    bSuccess = TRUE;
  }

  return bSuccess;
}

BOOL CMapiSession::LoggedOn() const
{
  return (m_hSession != 0);
}

BOOL CMapiSession::MapiInstalled() const
{
  return (m_hMapi != NULL);
}

BOOL CMapiSession::Logoff()
{
  ASSERT(MapiInstalled()); //MAPI must be installed
  ASSERT(m_lpfnMAPILogoff); //Function pointer must be valid

  //Initialise the function return value
  BOOL bSuccess = FALSE;

  if (m_hSession)
  {
    //Call the MAPILogoff function
    ULONG nError = m_lpfnMAPILogoff(m_hSession, 0, 0, 0); 
    if (nError != SUCCESS_SUCCESS)
    {
      TRACE(_T("Failed in call to MapiLogoff, Error:%d"), nError);
      m_nLastError = nError;
      bSuccess = TRUE;
    }
    else
    {
      m_nLastError = SUCCESS_SUCCESS;
      bSuccess = TRUE;
    }
    m_hSession = 0;
  }
    
  return bSuccess;
}

BOOL CMapiSession::Resolve(const CString& sName, lpMapiRecipDesc* lppRecip)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  ASSERT(MapiInstalled()); //MAPI must be installed
  ASSERT(m_lpfnMAPIResolveName); //Function pointer must be valid
  ASSERT(LoggedOn()); //Must be logged on to MAPI
  ASSERT(m_hSession); //MAPI session handle must be valid

  //Call the MAPIResolveName function
  LPSTR lpszAsciiName = T2A((LPTSTR) (LPCTSTR) sName);
  ULONG nError = m_lpfnMAPIResolveName(m_hSession, 0, lpszAsciiName, 0, 0, lppRecip);
  if (nError != SUCCESS_SUCCESS)
  {
    TRACE(_T("Failed to resolve the name: %s, Error:%d\n"), sName, nError);
    m_nLastError = nError;
  }

  return (nError == SUCCESS_SUCCESS);
}

BOOL CMapiSession::Send(CMapiMessage& message)
{
	//For correct operation of the T2A macro, see MFC Tech Note 59
	USES_CONVERSION;

  ASSERT(MapiInstalled()); //MAPI must be installed
  ASSERT(m_lpfnMAPISendMail); //Function pointer must be valid
  ASSERT(m_lpfnMAPIFreeBuffer); //Function pointer must be valid
  ASSERT(LoggedOn()); //Must be logged on to MAPI
  ASSERT(m_hSession); //MAPI session handle must be valid

  //Initialise the function return value
  BOOL bSuccess = FALSE;  

  //Create the MapiMessage structure to match the message parameter send into us
  MapiMessage mapiMessage;
  ZeroMemory(&mapiMessage, sizeof(mapiMessage));
  mapiMessage.lpszSubject = T2A((LPTSTR) (LPCTSTR) message.m_sSubject);
  mapiMessage.lpszNoteText = T2A((LPTSTR) (LPCTSTR) message.m_sBody);
  mapiMessage.nRecipCount = message.m_To.GetSize() + message.m_CC.GetSize() + message.m_BCC.GetSize(); 
  ASSERT(mapiMessage.nRecipCount); //Must have at least 1 recipient!

  //Allocate the recipients array
  mapiMessage.lpRecips = new MapiRecipDesc[mapiMessage.nRecipCount];

  //Setup the "To" recipients
  int nRecipIndex = 0;
  int nToSize = message.m_To.GetSize();
  for (int i=0; i<nToSize; i++)
  {
    MapiRecipDesc& recip = mapiMessage.lpRecips[nRecipIndex];
    ZeroMemory(&recip, sizeof(MapiRecipDesc));
    recip.ulRecipClass = MAPI_TO;
    CString& sName = message.m_To.ElementAt(i);

    //Try to resolve the name
    lpMapiRecipDesc lpTempRecip;  
    if (Resolve(sName, &lpTempRecip))
    {
      //Resolve worked, put the resolved name back into the sName
      sName = lpTempRecip->lpszName;

      //Don't forget to free up the memory MAPI allocated for us
      m_lpfnMAPIFreeBuffer(lpTempRecip);
    }
    recip.lpszName = T2A((LPTSTR) (LPCTSTR) sName);

    ++nRecipIndex;
  }

  //Setup the "CC" recipients
  int nCCSize = message.m_CC.GetSize();
  for (i=0; i<nCCSize; i++)
  {
    MapiRecipDesc& recip = mapiMessage.lpRecips[nRecipIndex];
    ZeroMemory(&recip, sizeof(MapiRecipDesc));
    recip.ulRecipClass = MAPI_CC;
    CString& sName = message.m_CC.ElementAt(i);

    //Try to resolve the name
    lpMapiRecipDesc lpTempRecip;  
    if (Resolve(sName, &lpTempRecip))
    {
      //Resolve worked, put the resolved name back into the sName
      sName = lpTempRecip->lpszName;

      //Don't forget to free up the memory MAPI allocated for us
      m_lpfnMAPIFreeBuffer(lpTempRecip);
    }
    recip.lpszName = T2A((LPTSTR) (LPCTSTR) sName);

    ++nRecipIndex;
  }

  //Setup the "BCC" recipients
  int nBCCSize = message.m_BCC.GetSize();
  for (i=0; i<nBCCSize; i++)
  {
    MapiRecipDesc& recip = mapiMessage.lpRecips[nRecipIndex];
    ZeroMemory(&recip, sizeof(MapiRecipDesc));
    recip.ulRecipClass = MAPI_BCC;
    CString& sName = message.m_BCC.ElementAt(i);

    //Try to resolve the name
    lpMapiRecipDesc lpTempRecip;  
    if (Resolve(sName, &lpTempRecip))
    {
      //Resolve worked, put the resolved name back into the sName
      sName = lpTempRecip->lpszName;

      //Don't forget to free up the memory MAPI allocated for us
      m_lpfnMAPIFreeBuffer(lpTempRecip);
    }
    recip.lpszName = T2A((LPTSTR) (LPCTSTR) sName);

    ++nRecipIndex;
  }

  //Setup the attachments 
  int nAttachmentSize = message.m_Attachments.GetSize();
  int nTitleSize = message.m_AttachmentTitles.GetSize();
  if (nTitleSize)
  { 
    ASSERT(nTitleSize == nAttachmentSize); //If you are going to set the attachment titles then you must set 
                                           //the attachment title for each attachment
  }
  if (nAttachmentSize)
  {
    mapiMessage.nFileCount = nAttachmentSize;
    mapiMessage.lpFiles = new MapiFileDesc[nAttachmentSize];
    for (i=0; i<nAttachmentSize; i++)
    {
      MapiFileDesc& file = mapiMessage.lpFiles[i];
      ZeroMemory(&file, sizeof(MapiFileDesc));
      file.nPosition = 0xFFFFFFFF;
      CString& sFilename = message.m_Attachments.ElementAt(i);
      file.lpszPathName = T2A((LPTSTR) (LPCTSTR) sFilename);
      file.lpszFileName = file.lpszPathName;
      if (nTitleSize)
      {
        CString& sTitle = message.m_AttachmentTitles.ElementAt(i);
        file.lpszFileName = T2A((LPTSTR) (LPCTSTR) sTitle);
      }
    }
  }

  //Do the actual send using MAPISendMail
  ULONG nError = m_lpfnMAPISendMail(m_hSession, 0, &mapiMessage, 0, 0);
  if (nError == SUCCESS_SUCCESS)
  {
    bSuccess = TRUE;
    m_nLastError = SUCCESS_SUCCESS;
  }
  else
  {
    TRACE(_T("Failed to send mail message, Error:%d\n"), nError);
    m_nLastError = nError;
  }

  //Tidy up the Attachements
  if (nAttachmentSize)
    delete [] mapiMessage.lpFiles;
  
  //Free up the Recipients memory
  delete [] mapiMessage.lpRecips;

  return bSuccess;
}

ULONG CMapiSession::GetLastError() const
{
  return m_nLastError;
}

