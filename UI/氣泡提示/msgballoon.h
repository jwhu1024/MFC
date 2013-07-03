#pragma once
#include <tchar.h>

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

struct MessageBalloon : public NOTIFYICONDATA
{
	MessageBalloon(LPCTSTR _szInfo = _T("Prompt"), LPCTSTR _szInfoTitle = 0, LPCTSTR _szTip = 0, HICON _hIcon = 0)
	{
		ASSERT(_szInfo);
		static struct wnd_holder{
			HWND hwnd_;
			wnd_holder(){hwnd_ = CreateWindowW(L"STATIC", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);}
			~wnd_holder(){DestroyWindow(hwnd_);}
		} wh;
		
		memset(this, 0, sizeof(*this));
		cbSize = sizeof(*this);
		hWnd = wh.hwnd_;
		hIcon = _hIcon ? _hIcon : ::LoadIcon(::GetModuleHandle(NULL),MAKEINTRESOURCE(128/*IDR_MAINFRAME*/));
		if (! hIcon) hIcon = ::LoadIcon(NULL, IDI_INFORMATION);
		// if 'NIF_INFO' is an undeclared identifier, define _WIN32_IE 0x500 at the very beginning
		uFlags = NIF_INFO | NIF_TIP | NIF_ICON; 
		dwInfoFlags = NIIF_INFO;
		_tcscpy(szInfo, _szInfo);
		if (! _szInfoTitle) LoadString(::GetModuleHandle(NULL), 57344/*AFX_IDS_APP_TITLE*/, szInfoTitle, sizeof(szInfoTitle));
		else _tcscpy(szInfoTitle, _szInfoTitle);
		_tcscpy(szTip, _szTip ? _szTip : szInfoTitle);

		Shell_NotifyIcon(NIM_ADD, this);
	}
	
	~MessageBalloon(){Shell_NotifyIcon(NIM_DELETE, this);}
};