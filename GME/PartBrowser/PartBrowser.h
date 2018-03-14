#pragma once

// PartBrowser.h : main header file for PartBrowser.DLL

#if !defined( __AFXCTL_H__ )
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols


// CPartBrowserApp : See PartBrowser.cpp for implementation.

class CPartBrowserApp : public COleControlModule //CWinApp
{
public:
	BOOL InitInstance();
	int ExitInstance();
private:
	BOOL InitATL();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

