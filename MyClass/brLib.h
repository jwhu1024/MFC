// brLib.h : brLib DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號

#define BRLIB_SUCCESS 1
#define BRLIB_ERROR 0

// CbrLibApp
// 這個類別的實作請參閱 brLib.cpp
//

class CbrLibApp : public CWinApp
{
public:
	CbrLibApp();

// 覆寫
public:
	virtual BOOL InitInstance();	
	DECLARE_MESSAGE_MAP()

public:
	/* Convert Use */
	__declspec(dllexport) int UnicodeToAnsi(TCHAR *unicode, char* ansi);
	__declspec(dllexport) int AnsiToUnicode(char* ansi, TCHAR* unicode);
	__declspec(dllexport) int CStringToChar(CString strsrc, char* dest);
	__declspec(dllexport) int CharToCString(char* source, CString* dest);
	/* Debug Use */
	__declspec(dllexport) int DbgPrint(char* chString, CString strData=NULL);
	/* General Use */
	__declspec(dllexport) BOOL CheckFileExist(char* path);
};
