/********************************************************************\
	created:		2001/03/22
	created:		22:3:2001   20:20
	filename: 	c:\my projects\dualmagic\controlappl\include\clog.h
	file path:	c:\my projects\dualmagic\controlappl\include
	file base:	clog
	file ext:		h
	author:			Alex Kucherenko
	
	purpose:	
\********************************************************************/

#ifndef _LOG_SYSTEM_LOG_MAIN_CLASS_H_
#define _LOG_SYSTEM_LOG_MAIN_CLASS_H_

#pragma warning( push, 3 )
#pragma once

#include "..\stdafx.h"

#pragma warning( disable: 4786 ) // disable STL warnings

#include "CStoreLog.h"

#ifndef MAX_STRING_LENGTH
# define MAX_STRING_LENGTH 8192
#endif

#ifndef LOG_INFO
# define  LOG_ERROR     0
# define  LOG_WARNING   1
# define  LOG_INFO      2
# define  LOG_MAX_LEVEL 2
#endif

#define DEF_LONG_FORMAT_STR "Process : %#06x : Thread : %#04x : Time : %s : Level : %12s : Message : %s\n"
#define DEF_SHORT_FORMAT_STR "%#06x : %#04x : %s : %12s : %s\n"
#define DEF_TIME_LONG_STR "%02u:%02u:%02u ms:%03u"
#define DEF_TIME_SHORT_STR "%02u:%02u:%02u"

class CLog
{
  public:
    // constructor 
    CLog( IStoreLog *pStoreLog, long maxLevel, bool bParentToStore = true ) : 
      m_strTimeFormat( DEF_TIME_LONG_STR ),
      m_strMessageFormat( DEF_LONG_FORMAT_STR ),
      m_bIsParent( bParentToStore ),    // if true - then we destroy StoreClass
      m_bAutoFlush( true ),             // Store Log to disk immediately
      m_lLevel( maxLevel ),
      m_pStore( pStoreLog ),            // our store class
      m_bLogTime( true )                // Add time string into log
    {
      m_tmpBuffer[0] = 0;
    }
    
    // destructor
    ~CLog(  )
    {
      m_pStore->FlushData();

      if( m_bIsParent == true )
        delete m_pStore, m_pStore = NULL;
    };
  
    // Log function ( most used )
    inline std::string LogFormatString( std::string Format, ... );
    int LogString( long Level, const std::string &Message );
    int LogString( long Level, const char *szMessage );
    int LogRawString( const std::string &Message );

    // AutoFlush
    inline bool GetAutoFlush( void ) const 
    {
      return m_bAutoFlush;
    }

    inline void SetAutoFlush( bool state )
    {
      m_bAutoFlush = state;
    }
    
    // TimeFormat
    inline std::string GetTimeFormat( void ) const
    {
      return m_strTimeFormat;
    }

    inline void SetTimeFormat( const std::string &format )
    {
      m_strTimeFormat = format.c_str();
    }

    // Message format
    inline const std::string &GetMessageFormat( void ) 
    {
      return m_strMessageFormat;
    }

    inline void SetMessageFormat( const std::string &format )
    {
      m_strMessageFormat = format.c_str();
    }

    // Write Time to Log
    inline bool GetLogTime( void ) const
    {
      return  m_bLogTime;
    }

    inline void SetLogTime( bool state )
    {
      m_bLogTime = state;
    }

  protected:

    virtual const std::string &LevelText( long /*Level*/ )
    {
      m_strTempBuf = " LOG NOT IMPLEMENTED CATEGORY : ";
      return m_strTempBuf;
    };
    
    inline const std::string &CurrentTime(  )
    {
      SYSTEMTIME	SystemTime;
      GetLocalTime( &SystemTime );
      
      m_tmpBuffer[0] = 0;

      _snprintf( m_tmpBuffer, 
        sizeof( m_tmpBuffer ),
        m_strTimeFormat.c_str(), 
        SystemTime.wHour, SystemTime.wMinute, 
        SystemTime.wSecond, SystemTime.wMilliseconds );

      m_strTempBuf = m_tmpBuffer;
      
      return m_strTempBuf;
    };


  private:
    IStoreLog     *m_pStore;
    long          m_lLevel;
    bool          m_bAutoFlush;
    std::string   m_strTimeFormat;
    std::string   m_strMessageFormat;
    bool          m_bIsParent;
    bool          m_bLogTime;

    char          m_tmpBuffer[ MAX_STRING_LENGTH ];
    std::string   m_strTempBuf;        
};

#pragma warning( pop )

#endif /* _LOG_SYSTEM_LOG_MAIN_CLASS_H_ */