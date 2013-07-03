// WebAccess.cpp : implementation file
//

#include "stdafx.h"
#include "InternetAccess.h"
#include "WebAccess.h"
#include "Registry.h"


// CWebAccess

CWebAccess::CWebAccess(LPCTSTR pstrAgent /*= NULL*/,
		DWORD dwContext /*= 1*/,
		DWORD dwAccessType /*= PRE_CONFIG_INTERNET_ACCESS*/,
		LPCTSTR pstrProxyName /*= NULL*/,
		LPCTSTR pstrProxyBypass /*= NULL*/,
		DWORD dwFlags /*= 0*/)
		: CInternetSession (pstrAgent, dwContext, dwAccessType, pstrProxyName, 
							pstrProxyBypass, dwFlags)
{
	CRegistry registry;
	registry.SetRootKey(HKEY_CURRENT_USER);
	registry.CreateKey("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings");
	
	DWORD proxyEnabled;
	proxyEnabled = registry.ReadDword("ProxyEnable", 0);
	if( proxyEnabled )
	{
		m_bProxyEnable = TRUE;
		registry.ReadString("ProxyOverride", m_szProxyOverride);
		registry.ReadString("ProxyServer", m_szProxyServer);
	}
	else
	{
		m_bProxyEnable = FALSE;	

		m_szProxyOverride = "";
		m_szProxyServer = "";
	}
}

CWebAccess::~CWebAccess()
{
	this->Close();
}


// CWebAccess member functions
void CWebAccess::OnStatusCallback(DWORD  dwContext , DWORD dwInternetStatus,
	LPVOID  lpvStatusInformation , DWORD  dwStatusInformationLength )
{
    // Status callbacks need thread-state protection. 
    AFX_MANAGE_STATE(AfxGetAppModuleState( ));

    CString strStatus;

	switch (dwInternetStatus)
	{
	case INTERNET_STATUS_RESOLVING_NAME:
		strStatus.Format("Resolving name %s", lpvStatusInformation);
		break;

	case INTERNET_STATUS_NAME_RESOLVED:
		strStatus.Format("Name resolved %s", lpvStatusInformation);
		break;

	case INTERNET_STATUS_HANDLE_CREATED:
		strStatus.Format("Handle created");
		break;

	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		strStatus.Format("Connecting to socket address");
		break;

	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		strStatus.Format("Connected to socket address");
		break;

	case INTERNET_STATUS_SENDING_REQUEST:
		strStatus.Format("Sending request");
		break;

	case INTERNET_STATUS_REQUEST_SENT:
		strStatus.Format("Request sent");
		break;

	case INTERNET_STATUS_RECEIVING_RESPONSE:
        return;
		strStatus.Format("Receiving response");
		break;

	case INTERNET_STATUS_RESPONSE_RECEIVED:
		strStatus.Format("Response received");
		break;

	case INTERNET_STATUS_CLOSING_CONNECTION:
		strStatus.Format("Closing the connection to the server");
		break;

	case INTERNET_STATUS_CONNECTION_CLOSED:
		strStatus.Format("Connection to the server closed");
		break;

	case INTERNET_STATUS_HANDLE_CLOSING:
        return;
		strStatus.Format("Handle closed");
		break;
	
	//Check MSDN information about CInternetSession dwFlag INTERNET_FLAG_ASYNC
	//I have never been able to get this to work to my liking
	case INTERNET_STATUS_REQUEST_COMPLETE:
  		strStatus.Format("Request complete");
		break;

	case INTERNET_STATUS_REDIRECT:
		strStatus.Format("Being redirected");
		break;

	default:
		strStatus.Format("Unknown status: %d", dwInternetStatus);
		break;
	}  
}

//use this function to update any status, edit or what ever control
//that needs updating
void CWebAccess::ShowStatus(LPCTSTR strStatus)
{
	
}


//Lets get the file via http
DWORD CWebAccess::GetWebFile(LPCTSTR pstrAgent, LPCTSTR lpstrServer, int nPort, CString strFile)
{
	//Check what file types we will allow to be requested
	CString extension = strFile.Right(3);

	
	DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS;
	DWORD dwHttpRequestFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE;

	/*string containing the application name that is used to refer
	  client making the request. If this NULL the frame work will
	  call  the global function AfxGetAppName which returns the application
	  name.*/

	//LPCTSTR pstrAgent = NULL;

	//the verb we will be using for this connection
	//if NULL then GET is assumed
	/*LPCTSTR pstrVerb = "GET";
	
	//the address of the url in the request was obtained from
	LPCTSTR pstrReferer = NULL;

	//Http version we are using; NULL = HTTP/1.0
	LPCTSTR pstrVersion = NULL;

	//For the Accept request headers if we need them later on
	//LPCTSTR pstrAcceptTypes = "Accept: audio/x-aiff, audio/basic, audio/midi, audio/mpeg, audio/wav, image/jpeg, image/gif, image/jpg, image/png, image/mng, image/bmp, text/plain, text/html, text/htm\r\n";
	LPCTSTR pstrAcceptTypes = NULL;
	CString szHeaders = "Accept: audio/x-aiff, audio/basic, audio/midi, audio/mpeg, audio/wav, image/jpeg, image/gif, image/jpg, image/png, image/mng, image/bmp, text/plain, text/html, text/htm\r\n";

	//the server port we need changed
	//nPort = INTERNET_INVALID_PORT_NUMBER
	unsigned short usPort = nPort;
	
	//Username we will use if a secure site comes into play
	LPCTSTR pstrUserName = NULL; 
	
	//The password we will use
	LPCTSTR pstrPassword = NULL;

	//CInternetSession flags if we need them
	//DWORD dwFlags = INTERNET_FLAG_ASYNC;
	DWORD dwFlags = NULL;

	//Proxy setting if we need them

	CString pstrProxyName = NULL;
	LPCTSTR pstrProxyBypass = NULL;

	char *proxy = g_szProxy.GetBuffer(g_szProxy.GetLength());

	if(proxy!= NULL )
	{
		CString proxyName = g_szProxy;
		proxyName+=":";
		proxyName += g_szPort;

		pstrProxyName = proxyName;
	}

	CWebAccess	session(pstrAgent, dwAccessType, (LPCSTR)pstrProxyName, pstrProxyBypass, dwFlags);

	//Set any CInternetSession options we  may need
	int ntimeOut = 30;
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,1000* ntimeOut);
	session.SetOption(INTERNET_OPTION_CONNECT_BACKOFF,1000);
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES,1);

	//Enable or disable status callbacks
	session.EnableStatusCallback(TRUE);

	CHttpConnection*	pServer = NULL;   
	CHttpFile*			pFile = NULL;

	DWORD				dwRet;	
	try 
	{		

		pServer = session.GetHttpConnection(lpstrServer, usPort, 
			pstrUserName, pstrPassword);
		pFile = pServer->OpenRequest(pstrVerb, strFile, pstrReferer, 
			1, &pstrAcceptTypes, pstrVersion, dwHttpRequestFlags);

		pFile->AddRequestHeaders(szHeaders);
		pFile->AddRequestHeaders("User-Agent: GetWebFile/1.0\r\n", HTTP_ADDREQ_FLAG_ADD_IF_NEW);
		pFile->SendRequest();

		pFile->QueryInfoStatusCode(dwRet);//Check wininet.h for info
										  //about the status codes


		if (dwRet == HTTP_STATUS_DENIED)
		{
			return dwRet;
		}

		if (dwRet == HTTP_STATUS_MOVED ||
			dwRet == HTTP_STATUS_REDIRECT ||
			dwRet == HTTP_STATUS_REDIRECT_METHOD)
		{
			CString strNewAddress;

			//again check wininet.h for info on the query info codes
			//there is alot one can do and re-act to based on these codes
			pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewAddress);
			
			int nPos = strNewAddress.Find(_T("Location: "));
			if (nPos == -1)
			{
				return 0;
			}

			strNewAddress = strNewAddress.Mid(nPos + 10);
			nPos = strNewAddress.Find('\n');
			if (nPos > 0)
				strNewAddress = strNewAddress.Left(nPos);

			pFile->Close();      
			delete pFile;

			pServer->Close();  
			delete pServer;

			CString strServerName;
			CString strObject;

			INTERNET_PORT nNewPort;
			DWORD dwServiceType;

			if (!AfxParseURL(strNewAddress, dwServiceType, strServerName, strObject, nNewPort))
			{
				return 0;
			}

			pServer = session.GetHttpConnection(strServerName, nNewPort, 
				pstrUserName, pstrPassword);
			pFile = pServer->OpenRequest(pstrVerb, strObject, 
				pstrReferer, 1, &pstrAcceptTypes, pstrVersion, dwHttpRequestFlags);
			pFile->AddRequestHeaders(szHeaders);
			pFile->SendRequest();

			pFile->QueryInfoStatusCode(dwRet);
			if (dwRet != HTTP_STATUS_OK)
			{
				return dwRet;
			}
		}

		if(dwRet == HTTP_STATUS_OK)
		{
			int len = pFile->GetLength();
			char buf[2000];
			int numread;
			CString filepath;

			//extract file name
			int index = strFile.Find("/");
			int stringLength = strFile.GetLength();

			CString originalFileName = "VideoStreamer.exe";
			CString fileName = strFile.Right(originalFileName.GetLength());

			filepath.Format(".\\Files\\%s", strFile);

			CFile myfile(fileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
			DWORD total = pFile->GetLength();
			DWORD readSoFar = 0;
			*/
			//while ((numread = pFile->Read(buf,/*sizeof(buf)-1)*/1) )> 0)
			/*{
				readSoFar += 1;

				buf[numread] = '\0';
				strFile += buf;
				myfile.Write(buf, numread);

				double percentDone= ((double)readSoFar/(double)390000) * (double)100;

				percentDone = (percentDone>100)?100:percentDone;

				pDialog->SetProgress(percentDone);		
			}

			myfile.Close();			

		}
		pFile->Close();      
		delete pFile;

		pServer->Close();  
		delete pServer;
		session.Close();
	}
	catch (CInternetException* pEx) 
	{
      // catch any exceptions from WinINet      
		TCHAR szErr[1024];
		szErr[0] = '\0';
        if(!pEx->GetErrorMessage(szErr, 1024))
			strcpy(szErr,"Some crazy unknown error");
		TRACE("File transfer failed!! - %s",szErr);      
		pEx->Delete();
		if(pFile)
			delete pFile;
		if(pServer)
			delete pServer;
		session.Close(); 
		return 0;
	}
	*/
	return 0;//dwRet;
}

DWORD CWebAccess::Get(CString url, CString & resultString)
{
	// http connection
	CHttpConnection *pHttpConnection;
	// http file pointer
	CHttpFile		*pHttpFile;
	// server url and object.
	CString			szServerUrl, szObject;

	// extract server and objects
	szServerUrl = url;
	ExtractObject(szServerUrl,szObject);

	try 
	{
		pHttpConnection=GetHttpConnection(szServerUrl);	

		if( NULL == pHttpConnection)
		{
			// no exception raised but there is an error. 
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch (CInternetException *pException)
	{
		char buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		resultString = buffer;
		return pException->m_dwError;		
	}

	// open request
	try {
		pHttpFile = pHttpConnection->OpenRequest(NULL, szObject);

		if(NULL == pHttpFile)
		{
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch(CInternetException *pException)
	{
		char buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		resultString = buffer;
		return pException->m_dwError;
    }

	// send the request
	try {
		BOOL ret = pHttpFile->SendRequest();

		if( FALSE == ret )
		{
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch(CInternetException *pException)
	{
		char buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		resultString = buffer;
		return pException->m_dwError;
	}

	// query status code
	DWORD retCode;
	BOOL ret = pHttpFile->QueryInfoStatusCode(retCode);
	if( FALSE == ret )
	{
		return WEB_ACCESS_QUERY_INFO_ERROR;
	}
	else if( HTTP_STATUS_OK != retCode )
	{
		return retCode;
	}

	char buf[2];
	int bytesRead;
	resultString = "";
	
	while( (bytesRead = pHttpFile->Read(buf, 1)) > 0 )
	{
		resultString += buf[0];
	}

	return WEB_ACCESS_DONE;
}

DWORD CWebAccess::Post(CString url, CString szFormData, CString & resultString)
{
	// http connection
	CHttpConnection *pHttpConnection;
	// http file pointer
	CHttpFile		*pHttpFile;
	// server url and object.
	CString			szServerUrl, szObject;
	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");


	// extract server and objects
	szServerUrl = url;
	ExtractObject(szServerUrl,szObject);

	try 
	{
		pHttpConnection=GetHttpConnection(szServerUrl);	

		if( NULL == pHttpConnection)
		{
			// no exception raised but there is an error. 
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch (CInternetException *pException)
	{
		char buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		resultString = buffer;
		return pException->m_dwError;		
	}

	// open request
	try {
		pHttpFile = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, szObject);

		if(NULL == pHttpFile)
		{
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch(CInternetException *pException)
	{
		char buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		resultString = buffer;
		return pException->m_dwError;
    }

	// send the request
	try {

		BOOL ret = pHttpFile->SendRequest(strHeaders, (LPVOID)(LPCSTR)szFormData, szFormData.GetLength());

		if( FALSE == ret )
		{
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch(CInternetException *pException)
	{
		char buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		resultString = buffer;
		return pException->m_dwError;
	}

	// query status code
	DWORD retCode;
	BOOL ret = pHttpFile->QueryInfoStatusCode(retCode);
	if( FALSE == ret )
	{
		return WEB_ACCESS_QUERY_INFO_ERROR;
	}
	else if( HTTP_STATUS_OK != retCode )
	{
		return retCode;
	}

	char buf[2];
	int bytesRead;
	resultString = "";
	
	while( (bytesRead = pHttpFile->Read(buf, 1)) > 0 )
	{
		resultString += buf[0];
	}
	return WEB_ACCESS_DONE;
}

void CWebAccess::AddPostArgument(CString name, CString value)
{
}

void CWebAccess::ClearPostArguments()
{
	postArguments.RemoveAll();
}

DWORD CWebAccess::DownloadFile(CString szCompleteUrl, CString szLocalFile)
{
	return 0;
}

// Extracts server and object from a complete url
// TODO:
// a. Check for malformed url. 
void CWebAccess::ExtractObject(CString & szUrl, CString &szObject)
{
	CString tempUrl = szUrl;

	// find "http" and remove it from the string if it's present.
	int index = tempUrl.Find("http", 0);
	if( index!= -1)
	{
		tempUrl.Delete(0, strlen("http://"));
	}

	// extract server
	BOOL ret = AfxExtractSubString(szUrl, tempUrl, 0, '/');

	index = tempUrl.Find("/", 0);	
	if( index != -1 )
	{
		tempUrl.Delete(0, index+1);

		// temp url now contains object. 
		szObject = tempUrl;
	}
}