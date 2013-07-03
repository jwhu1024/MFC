#define _WIN32_IE 0x500
#include <windows.h>
#include "msgballoon.h"

void main()
{
	{
		MessageBalloon mb("Initializing", "MsgBalloon Demo");
		Sleep(3000);
	}
	{
		MessageBalloon mb("A long time operation..", "MsgBalloon Demo");
		Sleep(3000);
	}
	{
		MessageBalloon mb("Quitting..", "MsgBalloon Demo");
		Sleep(3000);
	}
}