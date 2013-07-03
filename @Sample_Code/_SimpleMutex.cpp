// SimpleMutex.cpp : Defines the entry point for the console application.
// 簡單的主要工作
// 1. 建立 Mutex
// 2. 產生兩條 thread  A, B
//    A 產生一個 radom number 連續放到公用記憶體區中
//    B 連續讀取 10 筆資料,顯示在螢幕上
//    (寫入與讀取必須具有獨占性)

#include "stdafx.h"
#include "SimpleMutex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <iostream>
using namespace std;

void ShowError();
UINT  WriteData(LPVOID LParam);       // 不斷的寫入資料 (寫入與讀取的速度不同)
UINT  ReadData(LPVOID LParam);        // 不斷的從共用記憶體區讀取資料

struct ShareInfo{
	HANDLE hMutex;                    // 同步用的 kernel object
	int *ShareDataBuffer;             // 記錄共用記憶體的位置
	int DataLength;
}Info1;


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	// 共用記憶體區塊
	int Size=10;
	int *DataBuffer=(int*)HeapAlloc(GetProcessHeap(),0,Size*sizeof(int));
	if(DataBuffer==NULL){
		ShowError();
	}
	
	cout << "共用記憶體配置核准= "<<HeapSize(GetProcessHeap(),0,DataBuffer) <<" Bytes (請按任意鑑繼續)"<< endl;
    Info1.ShareDataBuffer=DataBuffer;
	Info1.DataLength=Size;
	getchar();

	// 1. 建立 Mutex: Initial 狀態為 non-signal
       HANDLE hMutex=CreateMutex(NULL,FALSE,NULL);
	   Info1.hMutex=hMutex;

    // 2. 建立 A, B threads
	   AfxBeginThread(WriteData,(LPVOID)&Info1);
	   AfxBeginThread(ReadData,(LPVOID)&Info1);


    HANDLE hevent=CreateEvent(NULL,FALSE,FALSE,NULL);
	WaitForSingleObject(hevent,INFINITE);

	// 下面的程式永遠不會被執行, 基本上,這是不會停止的程式
	HeapFree(GetProcessHeap(),NULL,DataBuffer);
	CloseHandle(hMutex);
	CloseHandle(hevent);
	return 0;
}

UINT  WriteData(LPVOID LParam){
	ShareInfo *pInfo1=(ShareInfo*) LParam;

	while(true){   // 不斷的寫入資料

		// Step 1: 得到存取許可
			WaitForSingleObject(pInfo1->hMutex,INFINITE);
			cout << "============================= 寫入程式得到許可 =============================" << endl;
		// Step 2: 寫入資料
			srand( (unsigned)time( NULL ) );
			
			for(int i=0;i<pInfo1->DataLength;i++){
				int data=rand();
				pInfo1->ShareDataBuffer[i]=data;
				cout<<"寫入: " << data << endl;
				Sleep(100);
			}

		// Step 3: 離開
			cout << "============================= 寫入程式離開 =============================" << endl;
			ReleaseMutex(pInfo1->hMutex);
	}

	return(0);	
}

UINT  ReadData(LPVOID LParam){
	ShareInfo *pInfo1=(ShareInfo*) LParam;

	while(true){   // 不斷的讀取資料

		// Step 1: 得到存取許可
			WaitForSingleObject(pInfo1->hMutex,INFINITE);
            cout << "============================= 讀取程式得到許可 =============================" << endl;
		// Step 2: 寫入資料
			for(int i=0;i<pInfo1->DataLength;i++){
				int data=pInfo1->ShareDataBuffer[i];
				cout<<"讀取: " << data << endl;
				Sleep(1000);
			}

		// Step 3: 離開
			cout << "============================= 讀取程式離開 =============================" << endl;
			ReleaseMutex(pInfo1->hMutex);
			
	}

	return(0);	
}

// 若有任何的錯誤, 你可以呼叫下面這個 functin
// 幫你組合出錯誤訊息
void ShowError(){
  LPVOID lpMsgBuf;

  /* 中文訊息
     FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_IGNORE_INSERTS,
		   NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		   (LPTSTR) &lpMsgBuf,0,NULL );
  */

   // 英文訊息 (比較方便到 Yahoo 查別人如何解決)
   FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_IGNORE_INSERTS,
                 NULL,GetLastError(),MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), // Default language
		 (LPTSTR) &lpMsgBuf,0,NULL );
   MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
   LocalFree( lpMsgBuf );
}

