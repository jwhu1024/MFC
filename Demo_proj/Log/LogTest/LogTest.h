// LogTest.h : main header file for the LOGTEST application
//

#if !defined(AFX_LOGTEST_H__3AEE0D79_776A_4B00_B104_27AF34947D60__INCLUDED_)
#define AFX_LOGTEST_H__3AEE0D79_776A_4B00_B104_27AF34947D60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLogTestApp:
// See LogTest.cpp for the implementation of this class
//

class CLogTestApp : public CWinApp
{
public:
	CLogTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLogTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGTEST_H__3AEE0D79_776A_4B00_B104_27AF34947D60__INCLUDED_)
