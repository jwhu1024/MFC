/********************************************************************\
	created:		2001/03/22
	created:		22:3:2001   20:23
	filename: 	c:\my projects\dualmagic\controlappl\include\cfilelog.cpp
	file path:	c:\my projects\dualmagic\controlappl\include
	file base:	cfilelog
	file ext:		cpp
	author:			Alex Kucherenko
	
	purpose:	
\********************************************************************/
#pragma warning( disable : 4786 4146 4100 4663 4018 4244 )
#include "stdafx.h"
#include "CFileLog.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
// Constructor 

CFileLog::CFileLog( const std::string &FileName, long lSize ) :
  m_lBufSize( lSize ),
  m_lBufPos( 0 )
{
  m_file = CreateFile( FileName.c_str(), 
    GENERIC_WRITE | SYNCHRONIZE,
    FILE_SHARE_WRITE | FILE_SHARE_READ, 
    NULL, 
    OPEN_ALWAYS, 
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
    NULL );
  
  if( m_file == INVALID_HANDLE_VALUE ) 
    throw std::string( "Error Opening file" );
  
  m_pBuffer = new UCHAR[m_lBufSize];
}

//////////////////////////////////////////////////////////////////////////
// Destructor 

CFileLog::~CFileLog(  )
{
  if( m_file != INVALID_HANDLE_VALUE )
    CloseHandle( m_file );

  delete []m_pBuffer;
}

//////////////////////////////////////////////////////////////////////////
// Store data from buffer to store class

int CFileLog::FlushData()
{
  DWORD tmpWrite = 0;

  if( m_lBufPos > 0 )
    WriteFile( m_file, m_pBuffer, m_lBufPos, &tmpWrite, NULL );

  FlushFileBuffers( m_file );

  m_lBufPos = 0;

  return 0;
}

//////////////////////////////////////////////////////////////////////////
// Write string to store class 

int CFileLog::WriteString( const std::string &Message )
{
  DWORD tmpWrite = 0;
  long strLen = Message.length();

  if( strLen >= m_lBufSize ) 
    return WriteFile( m_file, Message.c_str(), strLen, &tmpWrite, NULL );

  if( strLen+m_lBufPos >= m_lBufSize )
  {
    FlushData();
    return WriteFile( m_file, Message.c_str(), strLen, &tmpWrite, NULL );
  }

  memcpy( m_pBuffer + m_lBufPos, Message.c_str(), strLen );
  m_lBufPos += strLen;

  return 0;
}

//////////////////////////////////////////////////////////////////////////
// Set maximum buffer length 

int CFileLog::SetBufferLimit( long lSize )
{
  FlushData(  );

  if( lSize == m_lBufSize ) return 0;
  
  delete []m_pBuffer;

  m_pBuffer = new UCHAR[ m_lBufSize = lSize ];
  
  return 0;
}

// end of file