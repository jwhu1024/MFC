http://www.codeguru.com/cpp/w-p/system/misc/print.php/c15963

#include "stdafx.h"
#include "USB_Drive_Property.h"
#include <windows.h>
#include <SetupApi.h> // Included from Windows SDKs

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Defined in winioctl.h
DEFINE_GUID(GUID_DEVINTERFACE_VOLUME, 0x53f5630dL, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);

// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
   int nRetCode = 0;
   
   // initialize MFC and print and error on failure
   if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
   {
      // TODO: change error code to suit your needs
      _tprintf(_T("Fatal Error: MFC initialization failed\n"));
      nRetCode = 1;
   }
   else
   {
      // TODO: code your application's behavior here.

      HDEVINFO hDevHandle;
      SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
      DWORD required = 0;
      deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

      int nBufferSize = 0;
      
      SP_DEVINFO_DATA devInfoData;
      devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

      DWORD MemberIndex = 0;
      BOOL  Result;

      hDevHandle = SetupDiGetClassDevs(&GUID_DEVINTERFACE_VOLUME, NULL, NULL, 
         DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

      if(hDevHandle == INVALID_HANDLE_VALUE)
      {
         return 1;
      }
      else
      {
         BOOL bStart = false;
         TCHAR *buffer = NULL;
         PSP_DEVICE_INTERFACE_DETAIL_DATA devicedetailData;
			do
			{
				Result = SetupDiEnumDeviceInfo(hDevHandle, MemberIndex, &devInfoData);

				if(Result)
				{
				   Result = SetupDiEnumDeviceInterfaces(hDevHandle, 0, &GUID_DEVINTERFACE_VOLUME, 
                  MemberIndex, &deviceInterfaceData);
            }
            
            if(!Result)
            {
               SetupDiDestroyDeviceInfoList(hDevHandle);
               delete []buffer;
               buffer = NULL;
               return 1;
            }
            MemberIndex++;

            BOOL detailResult = FALSE;
      
            if(!bStart)
            {
               // As per MSDN, Get the required buffer size. Call SetupDiGetDeviceInterfaceDetail with a 
               // NULL DeviceInterfaceDetailData pointer, a DeviceInterfaceDetailDataSize of zero, 
               // and a valid RequiredSize variable. In response to such a call, this function returns 
               // the required buffer size at RequiredSize and fails with GetLastError returning 
               // ERROR_INSUFFICIENT_BUFFER. 
               // Allocate an appropriately sized buffer and call the function again to get the interface details. 

               SetupDiGetDeviceInterfaceDetail(hDevHandle, &deviceInterfaceData, NULL, 0, &required, NULL);

               buffer = new TCHAR[required];
               devicedetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) buffer;
               devicedetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
               nBufferSize = required;
               bStart = true;
            }

            detailResult = SetupDiGetDeviceInterfaceDetail(hDevHandle, &deviceInterfaceData, 
               devicedetailData, nBufferSize , &required, NULL);

            _tprintf(L"Device Path = %s\n\n", devicedetailData->DevicePath);

            if(detailResult)
            {
               TCHAR szDescription[MAX_PATH];
               memset(szDescription, 0, MAX_PATH);

               SetupDiGetClassDescription(&devInfoData.ClassGuid, szDescription, MAX_PATH, &required);
               _tprintf(L"Class Description = %s\n\n", szDescription);

               memset(szDescription, 0, MAX_PATH);
               SetupDiGetDeviceInstanceId(hDevHandle, &devInfoData, szDescription, MAX_PATH, 0);
               _tprintf(L"Device Instance Id = %s\n\n\n", szDescription);
            }

            if(!detailResult)
            {
               continue;
            }
         }while(Result);
      }
   }

   return nRetCode;
}
