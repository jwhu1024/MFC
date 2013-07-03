#ifndef __STDAFX_H
#define __STDAFX_H


// Windows version definition
#ifndef WINVER
#define WINVER	0x0400
#endif


// Exclude rarely-used stuff from Windows headers
#define VC_EXTRALEAN


// Include MFC header-files
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>			// MFC support for Windows Common Controls


//////////////////////////////////////////////////////////////////////
// Disable warning C4711: function selected for automatic inline
// expansion, which is good and not worth reporting.

#pragma warning(disable: 4711)


#endif // __STDAFX_H
