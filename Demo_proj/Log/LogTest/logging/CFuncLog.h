/********************************************************************\
  created:    2001/03/22
  created:    22:3:2001   20:20
  filename:   c:\my projects\dualmagic\controlappl\include\cfunclog.h
  file path:  c:\my projects\dualmagic\controlappl\include
  file base:  cfunclog
  file ext:   h
  author:     Alex Kucherenko
  
  purpose:  
\********************************************************************/

#ifndef _LOG_SYSTEM_FUNCTION_LOG_H_
#define _LOG_SYSTEM_FUNCTION_LOG_H_

#pragma warning( push, 3 )
#pragma once

#include "..\stdafx.h"

#pragma warning( disable: 4786 ) // disable STL warnings

#include "CLog.h"


class CFuncLog
{
  public:
    CFuncLog( CLog &pLog, const std::string &FunctionName );
    CFuncLog( CLog *pLog, const std::string &FunctionName );
    CFuncLog( CLog *pLog, char *szFunctionName );
    ~CFuncLog(  );

    int LogString( long Level, const std::string &Message );
    std::string FormatString( const std::string Format, ... );

    CFuncLog &operator<< ( int iValue );
    CFuncLog &operator<< ( long lValue );
    CFuncLog &operator<< ( double dValue );
    CFuncLog &operator<< ( const std::string &strValue );
    CFuncLog &operator<< ( char *cszValue );
    
    char *endl( void )
    {
      return "\n";
    };
    
  private:
    std::string   m_strFuncName;
    CLog          *m_pLog;
    char          m_tmpBuf[ MAX_STRING_LENGTH ];
};

#pragma warning( pop )

#endif /* _LOG_SYSTEM_FUNCTION_LOG_H_ */