/********************************************************************\
	created:		2001/03/22
	created:		22:3:2001   20:23
	filename: 	c:\my projects\dualmagic\controlappl\include\clog.cpp
	file path:	c:\my projects\dualmagic\controlappl\include
	file base:	clog
	file ext:		cpp
	author:			Alex Kucherenko
	
	purpose:	
\********************************************************************/

#include "stdafx.h"
#include "CLog.h"

//////////////////////////////////////////////////////////////////////////
// Format string using sprintf function 

std::string CLog::LogFormatString( std::string Format, ... )
{
  va_list args;
  va_start( args, Format );
  m_tmpBuffer[0] = 0;
  _vsnprintf( m_tmpBuffer, sizeof( m_tmpBuffer ), Format.c_str(), args );
  va_end( args );

  return std::string( m_tmpBuffer );
}

//////////////////////////////////////////////////////////////////////////
// redirector of simple strings

int CLog::LogString( long Level, const char *szMessage )
{
  return LogString( Level, std::string( szMessage ) );
};

//////////////////////////////////////////////////////////////////////////
// Log String according to level  

int CLog::LogString( long Level, const std::string &Message )
{
  if( Level > m_lLevel ) return 0;

  std::string msgHeader;
  std::string retString;

  switch( Level )
  {
    case 0 : msgHeader = "LOG ERROR";   break;
    case 1 : msgHeader = "LOG WARNING"; break;
    case 2 : msgHeader = "LOG INFO";    break;
    default: 
      msgHeader = LevelText( Level );
  };

  retString = LogFormatString( 
    m_strMessageFormat, 
    GetCurrentProcessId(), 
    GetCurrentThreadId(), 
    ( m_bLogTime ) ? CurrentTime().c_str() : "", 
    msgHeader.c_str(), Message.c_str() );

  m_pStore->WriteString( retString );
  
  if( m_bAutoFlush == true )
    m_pStore->FlushData();

  return 0;
}

//////////////////////////////////////////////////////////////////////////
// log string without any formating and testing

int CLog::LogRawString( const std::string &Message )
{
  m_pStore->WriteString( Message );

  if( m_bAutoFlush == true )
    m_pStore->FlushData();

  return 0;
}

//:> End of file