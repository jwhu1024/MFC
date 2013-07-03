/*
Module : CMAPI.H
Purpose: Defines the interface for an MFC wrapper class for sending an email using simple MAPI
Created: PJN / 11-05-1999
History: None

Copyright (c) 1999 by PJ Naughter.  
All rights reserved.

*/

#ifndef __CMAPI_H__
#define __CMAPI_H__


#ifndef MAPI_H
#pragma message("To avoid this message, put mapi.h in your PCH")
#include <mapi.h>
#endif


//Class which encapsulates a MAPI mail message
class CMapiMessage
{
public:
  CStringArray  m_To;               //The To: Recipients
  CStringArray  m_CC;               //The CC: Recipients
  CStringArray  m_BCC;              //The BCC Recipients
  CString       m_sSubject;         //The Subject of the message
  CString       m_sBody;            //The Body of the message
  CStringArray  m_Attachments;      //Files to attach to the email
  CStringArray  m_AttachmentTitles; //Titles to use for the email file attachments
};


//The class which encapsulates the MAPI connection
class CMapiSession
{
public:
//Constructors / Destructors
  CMapiSession();
  ~CMapiSession();

//Logon / Logoff Methods
  BOOL Logon(const CString& sProfileName, const CString& sPassword = CString(), CWnd* pParentWnd = NULL);
  BOOL LoggedOn() const;
  BOOL Logoff();

//Send a message
  BOOL Send(CMapiMessage& mesage);

//General MAPI support
  BOOL MapiInstalled() const;

//Error Handling
  ULONG GetLastError() const;

protected:
//Methods
  void Initialise();
  void Deinitialise(); 
  BOOL Resolve(const CString& sName, lpMapiRecipDesc* lppRecip);

//Data
  LHANDLE m_hSession; //Mapi Session handle
  ULONG m_nLastError; //Last Mapi error value
  HINSTANCE m_hMapi; //Instance handle of the MAPI dll
  LPMAPILOGON m_lpfnMAPILogon; //MAPILogon function pointer
  LPMAPILOGOFF m_lpfnMAPILogoff; //MAPILogoff function pointer
  LPMAPISENDMAIL m_lpfnMAPISendMail; //MAPISendMail function pointer
  LPMAPIRESOLVENAME m_lpfnMAPIResolveName; //MAPIResolveName function pointer
  LPMAPIFREEBUFFER m_lpfnMAPIFreeBuffer; //MAPIFreeBuffer function pointer
};


#endif //__CMAPI_H__