/********************************************************************\
	created:		2001/03/22
	created:		22:3:2001   20:21
	filename: 	c:\my projects\dualmagic\controlappl\include\cwinlog.h
	file path:	c:\my projects\dualmagic\controlappl\include
	file base:	cwinlog
	file ext:		h
	author:			Alex Kucherenko
	
	purpose:	
\********************************************************************/

#ifndef _LOG_SYSTEM_WIN_LOG_H_
#define _LOG_SYSTEM_WIN_LOG_H_

#pragma warning( push, 3 )
#pragma warning( disable: 4786 4018 4100 4146 4663 4244 )
#pragma once

#include "CStoreLog.h"
#include "logsync.h"
#include <vector>

typedef std::vector<std::string> TStrArray;
typedef TStrArray::iterator TStrIter;

#define WINDOW_HEIGHT 400
#define WINDOW_LENGTH 600

#define WM_USERSENDMESSAGE WM_USER+1
#define DEF_WINDOW_CLASS_NAME "LoggerClass"

//////////////////////////////////////////////////////////////////////////
//  

class CWinLog : public IStoreLog
{
  public:
    CWinLog( long MaxLines = 1000 );
    virtual ~CWinLog(  );
  
    virtual int FlushData();
    virtual int WriteString( const std::string &Message );
    virtual int SetBufferLimit( long lSize );
  
  protected:
    inline  HWND    CreateLogWindow( void );
    static  LRESULT APIENTRY localWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
  
  private:
    typedef LOGGER::CAutoCritic     CSyncSection;
    typedef LOGGER::CLogSimpleLock  CSyncLock;

    bool              m_bFirstString;
  
    ULONG             m_lMaxLines;
    TStrArray         m_CurrMessage;
    long              m_lBufferLimit;
    HWND              m_hEdit;
    HWND              m_hWindow;
    CSyncSection      m_CriticalSection;
};

#pragma warning( pop )

#endif /* _LOG_SYSTEM_WIN_LOG_H_ */