#if !defined(AFX_GMEACTIVEBROWSER_H__9D546378_72C2_494C_846F_256F257F85B0__INCLUDED_)
#define AFX_GMEACTIVEBROWSER_H__9D546378_72C2_494C_846F_256F257F85B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// GMEActiveBrowser.h : main header file for GMEACTIVEBROWSER.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols
#include "CurrentProject.h"	// Added by ClassView
#include "GMEActiveBrowser_i.h"

enum eDragOperation{DRAGOP_COPY,DRAGOP_MOVE,DRAGOP_REFERENCE,DRAGOP_INSTANCE,DRAGOP_SUBTYPE, DRAGOP_CLOSURE, DRAGOP_CLOSURE_MERGE};
/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserApp : See GMEActiveBrowser.cpp for implementation.

class CGMEActiveBrowserCtrl;

class CGMEActiveBrowserApp : public COleControlModule
{
public:
	void SetCtrl(CGMEActiveBrowserCtrl* pCtrl);
	CCurrentProject m_CurrentProject;
	BOOL InitInstance();
	int ExitInstance();
	CGMEActiveBrowserCtrl* GetCtrl();
private:
	CGMEActiveBrowserCtrl* m_pCtrl;
	BOOL InitATL();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMEACTIVEBROWSER_H__9D546378_72C2_494C_846F_256F257F85B0__INCLUDED)
