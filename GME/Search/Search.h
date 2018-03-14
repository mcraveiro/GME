#if !defined(AFX_SEARCH_H__07C694D3_0C01_45CC_9B31_4A1012F75D34__INCLUDED_)
#define AFX_SEARCH_H__07C694D3_0C01_45CC_9B31_4A1012F75D34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Search.h : main header file for SEARCH.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSearchApp : See Search.cpp for implementation.

class CSearchApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCH_H__07C694D3_0C01_45CC_9B31_4A1012F75D34__INCLUDED)
