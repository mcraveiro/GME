// MgaUtil.h : main header file for the MGAUTIL DLL
//

#if !defined(AFX_MGAUTIL_H__461F30A7_3BF0_11D4_B3F0_005004D38590__INCLUDED_)
#define AFX_MGAUTIL_H__461F30A7_3BF0_11D4_B3F0_005004D38590__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "MgaUtilLib.h"

/////////////////////////////////////////////////////////////////////////////
// CMgaUtilApp
// See MgaUtil.cpp for the implementation of this class
//

class CMgaUtilApp : public CWinApp
{
public:
	CMgaUtilApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMgaUtilApp)
		virtual BOOL InitInstance();
		virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMgaUtilApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL InitATL();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MGAUTIL_H__461F30A7_3BF0_11D4_B3F0_005004D38590__INCLUDED_)
