// CriticalSection 使用範例
// 別忘了, 設定 multi-thread 版本的 C/C++ run-time library
//                                                                       井民全

#include <windows.h>
#include <process.h>        // for _beginthreadex(...)

#include <tchar.h>
#include <iostream>
using namespace std;


// ============ 設定 Critical Section 變數 =============
const int MAX_TIMES = 100;
CRITICAL_SECTION g_cs; 
// ============ End ===============================

int g_nIndex = 0;
DWORD g_dwTimes[MAX_TIMES];


unsigned int __stdcall FirstThread(void *p);
unsigned int __stdcall SecondThread(void *p);


int _tmain(int argc, _TCHAR* argv[])
{
	// Step 1: 對 Critical Section 進行初始化的動作
	InitializeCriticalSection( &g_cs);

	HANDLE hThread[2];

	// Step 2: 建立 FristThread 
	unsigned int thread1_Id;
	hThread[0]=(HANDLE)_beginthreadex(NULL,0,FirstThread,NULL,0,&thread1_Id);

	// Step 3: 建立 SecondThread 
	// hThread[1]=(HANDLE) _beginthread(SecondThread,0,NULL);
	unsigned int thread2_Id;
	hThread[1]=(HANDLE)_beginthreadex(NULL,0,SecondThread,NULL,0,&thread2_Id);
	
	WaitForMultipleObjects(2,hThread,TRUE,INFINITE);

	printf("請按任意鍵離開");
	getchar();
	// Step 4: 刪除 CriticalSection
	DeleteCriticalSection (&g_cs); 

	// Step 5: 刪除 thread 的 Handle
	BOOL bOk=CloseHandle(hThread[0]);
	bOk=CloseHandle(hThread[1]);
	return 0;
}

unsigned int __stdcall FirstThread(void *p){
	bool bQuit=false;
	while (!bQuit) {
		EnterCriticalSection( &g_cs);               // 進入 Critical Section
		if(g_nIndex < MAX_TIMES){	
			g_dwTimes[g_nIndex] = GetTickCount();
			printf("\n=========== FirstThread ===========\n");
			printf("目前 g_nIndex= %d, 寫入值為= %d\n",g_nIndex,g_dwTimes[g_nIndex]);
			g_nIndex++;
		}else{
			bQuit=TRUE;
		}
		LeaveCriticalSection( &g_cs);               // 離開 Critical Section	
	}
	return 0;
}

unsigned int __stdcall SecondThread(void *p){
	bool bQuit=false;
	while (!bQuit) {
		EnterCriticalSection( &g_cs);                 // 進入 Critical Section
		if(g_nIndex < MAX_TIMES){
			g_nIndex++;
			g_dwTimes[g_nIndex - 1] = GetTickCount(); 
			printf("\n---- > SecondThread \n");
			printf("目前 g_nIndex= %d, 寫入值為= %d\n",g_nIndex,g_dwTimes[g_nIndex - 1]);
		}else{
			bQuit=TRUE;
		}
		LeaveCriticalSection( &g_cs);                 // 離開 Critical Section
	}
	return 0;
}

