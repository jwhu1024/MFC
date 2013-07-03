Callback function 範例程式
TouchAPI.h

#ifdef  TOUCHAPI_EXPORTS  
 #define TOUCH_API __declspec(dllexport)  
#else  
 #define TOUCH_API __declspec(dllimport)  
#endif  
  
// 定義觸控點的相關資訊  
struct TouchPoint  
{  
 int X;   
 int Y;   
};  
  
// 定義callback function的prototype  
// CB_TOUCH_FUNC是一個pointer,指向具有一個TouchPoint類型,且回傳void型的函?  
// 前面有個typedef,所以現在CB_TOUCH_FUNC是這種類型的?名  
typedef void (_stdcall *CB_TOUCH_FUNC) (TouchPoint);  
  
// 定義了一個RegisterCBTouchFunc的函式,可傳入一個具有CB_TOUCH_FUNC型別的callback function  
TOUCH_API void RegisterCBTouchFunc(CB_TOUCH_FUNC callback);  


TouchAPI.cpp
#include "stdafx.h"  
#include "TouchAPI.h"  
  
void DetectTouchFromSystem(CB_TOUCH_FUNC callback)  
{  
    // 當系統取得touch資料時,  
    // 就會呼叫一次pCB_TOUCH_FUNC參數所指到function一次  
    TouchPoint tp;  
    tp.X = 5;  
    tp.Y = 10;  
    callback(tp);  
}  
  
void RegisterCallbackTouchFunc(CB_TOUCH_FUNC callback)  
{  
 DetectTouchFromSystem(callback);  
}  
TouchPaint.cpp
#include "stdafx.h"  
#include "conio.h"  
#include "TouchAPI.h"  
   
// 定義了一個名為GetTouch的函式, 也就是callback函式  
// 它的prototype必須與TouchAPI的CB_TOUCH_FUNC宣告一致  
void _stdcall GetTouch(TouchPoint point)  
{  
 printf("x=%d, y=%d", point.X, point.Y);  
}  
   
int main(void)  
{  
    //註冊一個callback function  
    RegisterCBTouchFunc(GetTouch);  
    getch();  
    return 0 ;  
}; 


My Example:

In header:

DLOAD_API DoProcess(LPCTSTR fp=NULL, 
					LPCTSTR lpszPortNum=NULL,
					CB_PROG_FUNC NandPrg=NULL,
					E_JOB_TYPE e_Job_Type=JOB_UNKNOWN);
					
struct _CbMsg  
{  
	E_CB_STATE _cb_State;
	CString    _cb_TaskName;
	DWORD	   _cb_Percent;
};

typedef void (_stdcall *CB_PROG_FUNC) (_CbMsg);


In Cpp:
void _stdcall GetProg(_CbMsg nProg)
{
	// Do something;
}