In header

// DloadDll.h : DloadDll DLL 的主要標頭檔

#pragma once

#ifndef __AFXWIN_H__
#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號

// CDloadDllApp
#ifdef _BR_EXPORTS
#define DLOAD_API extern "C" void __declspec(dllexport) __stdcall
#else
#define DLOAD_API extern "C" void __declspec(dllimport) __stdcall
#endif

DLOAD_API DoProcess(LPCTSTR fp=NULL, 
					LPCTSTR lpszPortNum=NULL,
					CB_PROG_FUNC NandPrg=NULL,
					E_JOB_TYPE e_Job_Type=JOB_UNKNOWN);

In Cpp

DLOAD_API DoProcess(LPCTSTR fp,			/* =NULL */
						 LPCTSTR lpszPortNum,	/* =NULL */
						 CB_PROG_FUNC NandPrg,	/* =NULL */
						 E_JOB_TYPE e_Job_Type) /* =JOB_UNKNOWN */
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	// Calling Static Method
	CDloadDllApp::DloadFirmwareUpgrade(fp, 
									   lpszPortNum,
									   NandPrg, 
									   e_Job_Type);
}