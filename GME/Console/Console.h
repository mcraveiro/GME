#if !defined(AFX_CONSOLE_H__25F84967_DCC3_4EF7_9E42_402398CE7C40__INCLUDED_)
#define AFX_CONSOLE_H__25F84967_DCC3_4EF7_9E42_402398CE7C40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Console.h : main header file for CONSOLE.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols
#include "Console_i.h"

/////////////////////////////////////////////////////////////////////////////
// CConsoleApp : See Console.cpp for implementation.

class CConsoleApp : public COleControlModule
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

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONSOLE_H__25F84967_DCC3_4EF7_9E42_402398CE7C40__INCLUDED)
