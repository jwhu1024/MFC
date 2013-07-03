// 這段範例程式利用簡單的 named kernel object 達到
// 防止同時有兩個相同的 instalnces 執行的狀況

#include "stdafx.h"
#include <windows.h>
int _tmain(int argc, _TCHAR* argv[])
{
	// 防止剛好,你的系統中剛好有人使用 Baby 這個名字,
	// 最好的方法是使用 GUID(Globally Unique Identifier) 來當作名稱
	// 產生 GUID 的方法: 使用 VS 的工具 UUIDGEN.EXE
	HANDLE h = CreateEvent(NULL,FALSE,FALSE,"Baby");

   if (GetLastError( )==ERROR_ALREADY_EXISTS) {
      MessageBox(NULL,"系統中已經有重複的程式正在執行","Error",MB_OK);
      return(0);
   }
   getchar();
   // Before exiting, close the object.
   CloseHandle(h);
   return(0);
}

// ============ 另外的方法 ============  
/* http://www.codeguru.com/Cpp/W-P/system/misc/article.php/c5663/
This is a very simple way to detect the running instance of an application. It can be achieved by declaring a data variable in the compiler's own data section called shared.

#pragma data_seg("Shared")
LONG g_Counter = -1;
#pragma data_seg()

After this, the Linker will be noticed to make the Shared section readable, writable, and shared.


#pragma comment(linker, "/section:Shared, rws")
Now, whenever the application runs, the data variable declared in the compiler's data section will be incremented by 1 and until or unless the application gets closed, the data variable's value will not be decremented. Now, at this point, if we start the application again, it will check for the value of the variable declared in the data section of the compiler. If it's greater than 1, splash message of duplicate instance; else, start the application.

The value in the data variable of the compiler's data section is incremented or decremented by the functions declared in winbase.h; these are InterLockedIncrement and InterLockedDecrement. Just pass the address of the variable and the rest will be performed by the function.






*/

