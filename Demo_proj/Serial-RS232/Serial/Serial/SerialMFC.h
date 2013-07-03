//	SerialMFC.h - Definition of the CSerialMFC class
//
//	Copyright (C) 1999-2003 Ramon de Klein (Ramon.de.Klein@ict.nl)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifndef __SERIAL_MFC_H
#define __SERIAL_MFC_H


//////////////////////////////////////////////////////////////////////
// Include CSerialWnd base class

#include "SerialWnd.h"


//////////////////////////////////////////////////////////////////////
// Make sure that MFC is already loaded
//
// We don't include it here, because MFC based applications typically
// use a precompiled header file.

#ifndef __AFXWIN_H__
#error "CWnd class isn't included (include AFXWIN.H)"
#endif


//////////////////////////////////////////////////////////////////////
//
// CSerialMFC - MFC wrapper for serial communication handling
//
// CSerialMFC is a very thing wrapper around the CSerialWnd class,
// which makes it more suitable for use in MFC-based applications.
// Unfortenately, the MFC framework isn't very flexible when it comes
// to adding custom message handlers. We cannot add our own types to
// the message handlers, but we're stuck to use 32-bit integers.
//
// Add the following entry to the window's message map to make sure
// that MFC calls your handler, when a serial message comes in.
//
//      DECLARE_MESSAGE_MAP(CMyClass,CWnd)
//          ...
//          ON_WM_SERIAL(OnSerialMsg)
//          ...
//     END_MESSAGE_MAP()
//
// A typical handler could look like this:
//
//     afx_msg LRESULT CMyClass::OnSerialMsg (WPARAM wParam, LPARAM lParam)
//     {
//         CSerial::EEvent eEvent = CSerial::EEvent(LOWORD(wParam));
//         CSerial::EError eError = CSerial::EError(HIWORD(wParam));
//
//         // lParam: User-defined 32-bit integer (type LPARAM)
//         switch (eEvent)
//         {
//         case CSerialMFC::EEventRecv:
//             // TODO: Read data from the port
//             break;
//         ...
//         }
//         
//         // Return successful
//         return 0;
//     }
//
//
// Pros:
// -----
//	- Easy to use
//	- Fully ANSI and Unicode aware
//  - Integrates seamless in MFC applications
//
// Cons:
// -----
//  - No more then CSerialWnd, except that you need MFC
//
// Copyright (C) 1999-2003 Ramon de Klein
//                         (Ramon.de.Klein@ict.nl)

class CSerialMFC : public CSerialWnd
{
// Operations
public:
	// Open the serial communications for a particular COM port. You
	// need to use the full devicename (i.e. "COM1") to open the port.
	virtual LONG Open (LPCTSTR lpszDevice, CWnd* pwndDest, UINT nComMsg = WM_NULL, LPARAM lParam = 0, DWORD dwInQueue = 0, DWORD dwOutQueue = 0)
	{ 
		return CSerialWnd::Open(lpszDevice,pwndDest->GetSafeHwnd(),nComMsg,lParam,dwInQueue,dwOutQueue); 
	}
};


//////////////////////////////////////////////////////////////////////
// Message map macro for the default COM message

#define ON_WM_SERIAL(memberFxn)	\
	ON_REGISTERED_MESSAGE(CSerialMFC::mg_nDefaultComMsg,memberFxn)


//////////////////////////////////////////////////////////////////////
// Don't link to the default runtime libraries, because this will
// give a warning during link-time.

#ifdef _DEBUG
#pragma comment(linker,"/nodefaultlib:LIBCD")
#else
#pragma comment(linker,"/nodefaultlib:LIBC")
#endif


#endif	// __SERIAL_MFC_H
