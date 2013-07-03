/********************************************************************
  created:   2002/01/01
  created:   1:1:2002   1:57
  filename:  E:\My Projects\Repository\logging\CWinLog.cpp
  file path: E:\My Projects\Repository\logging
  file base: CWinLog
  file ext:  cpp
  author:    Alex Kucherenko

  purpose:   
*********************************************************************/

#include "stdafx.h"
#include "CStoreLog.h"
#include "CWinLog.h"

//////////////////////////////////////////////////////////////////////////
//  

CWinLog::CWinLog( long MaxLines /*= 1000*/ ) 
  : m_lMaxLines( MaxLines )
  , m_bFirstString( true )
{
  CreateLogWindow();
}

//////////////////////////////////////////////////////////////////////////
//  

CWinLog::~CWinLog(  )
{
  {
    CSyncLock lock( &m_CriticalSection );
    DestroyWindow( m_hWindow );
    m_hWindow = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////
//  

int CWinLog::FlushData()
{
  return 0;
}

//////////////////////////////////////////////////////////////////////////
//  

int CWinLog::WriteString( const std::string &Message )
{
  if( m_bFirstString )
  {
    ShowWindow( m_hWindow, SW_SHOWDEFAULT );
    UpdateWindow( m_hWindow );
    m_bFirstString = false;
  }
  
  CSyncLock lock( &m_CriticalSection );

  m_CurrMessage.push_back( Message.c_str( ) );

  PostMessage( m_hWindow, WM_USERSENDMESSAGE, 0, ( LPARAM ) 0 );

  return 0;
}

//////////////////////////////////////////////////////////////////////////
//  

int CWinLog::SetBufferLimit( long lSize )
{
  m_lMaxLines = lSize;
  return 0;
}

//////////////////////////////////////////////////////////////////////////
//  

HWND CWinLog::CreateLogWindow( void )
{
  WNDCLASS  wc; 
  
  wc.style            = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE; 
  wc.lpfnWndProc      = localWndProc; 
  wc.cbClsExtra       = 0; 
  wc.cbWndExtra       = 0; 
  wc.hInstance        = NULL; 
  wc.hIcon            = 0; 
  wc.hCursor          = LoadCursor( NULL, IDC_ARROW ); 
  wc.hbrBackground    = ( HBRUSH )( COLOR_BACKGROUND+5 ); 
  wc.lpszMenuName     = NULL; 
  wc.lpszClassName    = DEF_WINDOW_CLASS_NAME; 
  
  RegisterClass( &wc );
		
  m_hWindow = CreateWindow( 
      DEF_WINDOW_CLASS_NAME, 
      "Logger Window", 
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 
      CW_USEDEFAULT, CW_USEDEFAULT, 
      WINDOW_LENGTH, 
      WINDOW_HEIGHT, 
      NULL, NULL, NULL, 
      NULL );

  if( m_hWindow == NULL || m_hWindow == INVALID_HANDLE_VALUE )
  {
    ::MessageBox( NULL, "Error at window process creation!", "Error", MB_OK );
    return m_hWindow;
  }
  
  SetWindowLong( m_hWindow, GWL_USERDATA, ( long )this );
  
  // Init common controls if they are not init before by someone else
  static bool bFirst = false;
  if( bFirst == false )
  {
    INITCOMMONCONTROLSEX controls;
    controls.dwICC = ICC_TREEVIEW_CLASSES;
    controls.dwSize = sizeof( INITCOMMONCONTROLSEX );
    InitCommonControlsEx( &controls );
    bFirst = true;
  }
  
  m_hEdit = CreateWindow( WC_TREEVIEW, NULL, 
    WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | 
      ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
    0,0,0,0, 
    m_hWindow, 
    0, 
    ( HINSTANCE ) GetWindowLong( m_hWindow, GWL_HINSTANCE ), NULL );
  
  if( m_hEdit == NULL || m_hEdit == INVALID_HANDLE_VALUE )
  {
    ::MessageBox( NULL, "Error at window process creation! Cann't create Tree Control!!!",
      "Error!", MB_OK );
  }

  UpdateWindow( m_hWindow );
  return m_hWindow; 
}

//////////////////////////////////////////////////////////////////////////
// Static function which handle messages 

LRESULT APIENTRY CWinLog::localWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
  CWinLog* pThis = ( CWinLog * ) GetWindowLong( hwnd, GWL_USERDATA );  

  switch( message ) 
  { 
		case WM_USERSENDMESSAGE :
    {
      CSyncLock lock( &( pThis->m_CriticalSection ) );
      
      if( !pThis->m_CurrMessage.empty( ) )
      {
        std::string            tmpString;
        TVINSERTSTRUCT    MainItem;
        TVITEM            &item = MainItem.item;
        std::string::size_type k, i;
        
        for( i=0 ; i < pThis->m_CurrMessage.size() ; i++ )
        {
          tmpString = pThis->m_CurrMessage[ i ];

          // Remove control code from string
          while( 1 )
          {
            k = tmpString.find( '\n' );
            if( k == tmpString.npos ) break;

            tmpString.replace( k, 2, " " );
          }
          
          if( TreeView_GetCount( pThis->m_hEdit ) >= pThis->m_lMaxLines )
            TreeView_DeleteItem( pThis->m_hEdit, TreeView_GetRoot( pThis->m_hEdit ) );

	        item.mask = TVIF_TEXT;
	        item.hItem = NULL;
	        item.cChildren = item.lParam = item.iImage = item.state = 0;
	        item.pszText = ( LPTSTR )tmpString.c_str( );
          item.cchTextMax = tmpString.length( );
	        item.stateMask = ( UINT )-1;
          
          MainItem.hInsertAfter = TVI_LAST;
          MainItem.hParent = NULL;

          TreeView_InsertItem( pThis->m_hEdit, &MainItem );
        }
        
        pThis->m_CurrMessage.clear();
      }
      
      break; 
    }

    case WM_SETFOCUS: 
      if( pThis != NULL ) 
        SetFocus( ( HWND )pThis->m_hEdit ); 
      return 0; 
      
    case WM_SIZE: 
      if( pThis != NULL )
        MoveWindow( ( HWND )pThis->m_hEdit, 0, 0, LOWORD( lParam ), HIWORD( lParam ), TRUE);
      
      return 0; 
      
    case WM_DESTROY: 
      return 0; 
    
    case WM_CLOSE:
      ExitThread(0);
    
    default: 
      return DefWindowProc( hwnd, message, wParam, lParam ); 
  } 

  return NULL;
}

//:> end of file