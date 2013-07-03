// CWebAccess class (C) Uraan Inc. 2003-2005
// Dependencies:
// a. WinInet
// b. CRegistry -> registry.h
// 
// History:
// 5th March, 2005: Created. 
// 6th March, 2005: Added registry reading option to save internet settings from browser.
// 

#pragma once


#include <afx.h>
#include <afxwin.h>
#include <afxinet.h>
#include <stdlib.h>
#include <afxtempl.h>

typedef struct {
	CString name;				// name of the argument
	CString value;				// value of the argument
} PostArgument;

#define WEB_ACCESS_DONE				1
#define WEB_ACCESS_UNEXPECTED_ERROR -20
#define WEB_ACCESS_QUERY_INFO_ERROR -21
// CWebAccess command target

class CWebAccess : public CInternetSession
{
public:
	CString m_szProxyServer, m_szProxyOverride;
	BOOL	m_bProxyEnable;

	CWebAccess(LPCTSTR pstrAgent = NULL,
		DWORD dwContext = 1,
		DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS,
		LPCTSTR pstrProxyName = NULL,
		LPCTSTR pstrProxyBypass = NULL,
		DWORD dwFlags = 0);
	virtual ~CWebAccess();

	virtual void OnStatusCallback(DWORD dwContext, 
								  DWORD dwInternetStatus,
								  LPVOID lpvStatusInformation, 
								  DWORD dwStatusInformationLength);

	DWORD GetWebFile(LPCTSTR pszAppName, 
					 LPCTSTR lpstrServer,
					 int nPort, 
					 CString lpstrFile);

	// download a file 
	DWORD DownloadFile(CString szCompleteUrl, CString szLocalFile);

	// Gets an http request
	DWORD Get(CString url, CString & resultString);

	// Post an http request
	// assumes that the arguments have been added. 
	DWORD Post(CString url, CString szFormData, CString & resultString);

	void ShowStatus(LPCTSTR strStatus);

	// add a member to the post argument set
	void AddPostArgument(CString name, CString value);
private:
	CArray<PostArgument, PostArgument> postArguments;	// post arguments

	// clear the array of any arguments.
	void ClearPostArguments();

	// extract object from url
	void ExtractObject(CString & szUrl, CString & szObject);

};


