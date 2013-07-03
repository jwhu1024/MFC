/********************************************************************\
	created:		2001/03/22
	created:		22:3:2001   20:20
	filename: 	c:\my projects\dualmagic\controlappl\include\cfilelog.h
	file path:	c:\my projects\dualmagic\controlappl\include
	file base:	cfilelog
	file ext:		h
	author:			Alex Kucherenko
	
	purpose:	
\********************************************************************/

#ifndef _LOG_SYSTEM_FILE_LOG_H_
#define _LOG_SYSTEM_FILE_LOG_H_

#pragma warning( push, 3 )
#pragma once

#pragma warning( disable: 4786 4290 ) // disable warning

#include "CStoreLog.h"

#ifndef MAX_STRING_LENGTH
# define MAX_STRING_LENGTH 8192
#endif

class CFileLog : public IStoreLog
{
  public:
    CFileLog( const std::string &FileName, long lSize = MAX_STRING_LENGTH ) throw ( std::string );
    virtual ~CFileLog(  );

    virtual int FlushData();
    virtual int WriteString( const std::string &Message );
    virtual int SetBufferLimit( long lSize );

  private:
    std::string    m_strFileName;
    HANDLE         m_file;
    long           m_lBufSize;
    PUCHAR         m_pBuffer;
    long           m_lBufPos;
};

#pragma warning( pop )

#endif /* _LOG_SYSTEM_FILE_LOG_H_ */