// MutexDemo1.cpp : Defines the entry point for the console application.
//
// 與 Event 物件比較: 測試 Mutex 是否可以重新進入

#include "stdafx.h"
#include "MutexDemo1.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

#include <iostream>
using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{

		HANDLE hMutex=CreateMutex(NULL,FALSE,NULL);
	    WaitForSingleObject(hMutex,INFINITE);

	    // 再次進入
	    WaitForSingleObject(hMutex,INFINITE);
	    cout << "Mutex 使用次數遞增" << endl;
     
		ReleaseMutex(hMutex);
		ReleaseMutex(hMutex);
/*
	// =========================  比較一下 Event =========================
	    HANDLE hEvent=CreateEvent(NULL,FALSE,TRUE,NULL);
	    WaitForSingleObject(hMutex,INFINITE);

	    // 無法進入
	    WaitForSingleObject(hEvent,INFINITE);
	    cout << "無法進入" << endl;
	// =========================  End =========================
*/
	return 0;
}
