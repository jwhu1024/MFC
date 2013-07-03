/************************************************************************* 
* Serial port enumeration routines
*
* The EnumSerialPort function will populate an array of SSerInfo structs,
* each of which contains information about one serial port present in
* the system. Note that this code must be linked with setupapi.lib,
* which is included with the Win32 SDK.
*
************************************************************************/
#include <atlstr.h>
#define BR_DIAG_PORT_NAME     "BandLuxe Wideband Control Interface"

BOOL detect_diagnostic_port();    // return 0:Die, 1:Exist 

int  process_bandrich_device_change(UINT wParam, LPCWSTR dbcc_name);  // return -1:Not BR, 0:Removed , 1:BR CD-ROM ,2:BR Diag ,3:BR Mess
CString m_TargetDeviceMode;
// Eject Bandrich CD ROM (support W2K)
void eject_bandrich_cdrom(LPCSTR lpszTargetDeviceMode);



