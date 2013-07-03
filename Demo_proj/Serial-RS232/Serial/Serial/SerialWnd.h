//	SerialWnd.h - Definition of the CSerialWnd class
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


#ifndef __SERIAL_WND_H
#define __SERIAL_WND_H


//////////////////////////////////////////////////////////////////////
// Include CSerialEx base class

#include "SerialEx.h"


//////////////////////////////////////////////////////////////////////
//
// CSerialWnd - Win32 message-based wrapper for serial communications
//
// A lot of MS-Windows GUI based programs use a central message
// loop, so the application cannot block to wait for objects. This
// make serial communication difficult, because it isn't event
// driven using a message queue. This class makes the CSerial based
// classes suitable for use with such a messagequeue. Whenever
// an event occurs on the serial port, a user-defined message will
// be sent to a user-defined window. It can then use the standard
// message dispatching to handle the event.
//
// Pros:
// -----
//	- Easy to use
//	- Fully ANSI and Unicode aware
//  - Integrates easily in GUI applications and is intuitive to
//    use for GUI application programmers
//
// Cons:
// -----
//  - Uses a thread for each COM-port, which has been opened.
//  - More overhead, due to thread switching and message queues.
//  - Requires a window, but that's probably why you're using
//    this class.
//
// Copyright (C) 1999-2003 Ramon de Klein
//                         (Ramon.de.Klein@ict.nl)

class CSerialWnd : public CSerialEx
{
// Construction
public:
	CSerialWnd();
	virtual ~CSerialWnd();

// Operations
public:
	// Open the serial communications for a particular COM port. You
	// need to use the full devicename (i.e. "COM1") to open the port.
	virtual LONG Open (LPCTSTR lpszDevice, HWND hwndDest, UINT nComMsg=WM_NULL, LPARAM lParam=0, DWORD dwInQueue = 0, DWORD dwOutQueue = 0);

	// Close the serial port.
	virtual LONG Close (void);

protected:
	// Event handler that is called when 
	virtual void OnEvent (EEvent eEvent, EError eError);

public:
	// Default Serial notification message
	static const UINT mg_nDefaultComMsg;

protected:
	// Internal attributes
	HWND	m_hwndDest;
	UINT	m_nComMsg;
	LONG	m_lParam;
};

#endif	// __SERIAL_WND_H
