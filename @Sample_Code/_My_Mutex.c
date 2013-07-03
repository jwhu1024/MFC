hMutex = CreateMutex( 
				 NULL,                       // default security attributes
				 FALSE,                      // initially not owned
				 NULL);                      // unnamed mutex

		if (hMutex == NULL)		DbgPrint("CreateMutex Error");



		BOOL bIsOk = FALSE;
		DWORD dwWaitResult = NULL;
		dwWaitResult = WaitForSingleObject(	theApp.hMutex,   // handle to mutex
											15000);			 // five-second time-out interval
		switch (dwWaitResult) 
		{
			case WAIT_OBJECT_0:
				Sleep(5000);
				bIsOk = m_AdbCtrl.AdbSetSerialNumber(lCurid);
				theApp.DbgPrint("thread got mutex ownership.");				
				break;
			case WAIT_ABANDONED: 
				theApp.DbgPrint("Got ownership of the abandoned mutex object.");
				break;
			default:
				theApp.DbgPrint("Unknown error with MUTEX.");
				break;
		}

		// Release ownership of the mutex object.
		if (! ReleaseMutex(theApp.hMutex)) 
		{ 
			// Deal with error.
		}
		else
			theApp.DbgPrint("Mutex have been released.");

		// fastboot.exe oem setserialnoXXX
		if( bIsOk )
		{
			theApp.DbgPrint( "AdbSetSerialNumber Success...");
			bisSnSet = TRUE;
			Sleep(500);
		}
		else
		{
			theApp.DbgPrint( "AdbSetSerialNumber Failed...");
			bReflashOK = FALSE;
			goto UPDATE_RESULT;
		}