//	SerialEx.h - Definition of the CSerialEx class
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


#ifndef __SERIAL_EX_H
#define __SERIAL_EX_H


//////////////////////////////////////////////////////////////////////
// Include CSerial base class

#include "Serial.h"


//////////////////////////////////////////////////////////////////////
//
// CSerialEx - Win32 message-based wrapper for serial communications
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

class CSerialEx : public CSerial
{
// Construction
public:
	CSerialEx();
	virtual ~CSerialEx();

// Operations
public:
	// Open the serial communications for a particular COM port. You
	// need to use the full devicename (i.e. "COM1") to open the port.
	virtual LONG Open (LPCTSTR lpszDevice, DWORD dwInQueue = 0, DWORD dwOutQueue = 0, bool fStartListener = false);

	// Close the serial port.
	virtual LONG Close (void);

	// Start the listener thread
	virtual LONG StartListener (void);

	// Stop the listener thread. Because the other thread might be
	// busy processing data it might take a while, so you can specify
	// a time-out.
	virtual LONG StopListener (DWORD dwTimeout = INFINITE);

protected:
	// Each opened COM-port uses its own specific thread, which will
	// wait for one of the events to happen. When an event happens,
	// then the client window is send a message informing about the
	// event.
	static DWORD WINAPI ThreadProc (LPVOID lpArg);
	DWORD ThreadProc (void);

	// Event handler
	virtual void OnEvent (EEvent eEvent, EError eError) = 0;

protected:
	// The WaitEvent method is being used by this class internally
	// and shouldn't be used by client applications. Client
	// application should monior the messages.
	using CSerial::WaitEvent;

	// The event-type is send in the WPARAM of the message and
	// the GetEventType method returns the wrong data, so we'll
	// hide this method for client applications to avoid problems.
	using CSerial::GetEventType;
	using CSerial::GetError;

protected:
	// Internal attributes
	bool	m_fStopping;
	HANDLE	m_hThread;

#ifndef SERIAL_NO_OVERLAPPED
	// Handle for overlapped operations in worker-thread
	HANDLE	m_hevtOverlappedWorkerThread;
#endif
};

#endif	// __SERIAL_EX_H
