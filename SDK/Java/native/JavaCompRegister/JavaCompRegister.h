// JavaCompRegister.h : main header file for the JAVACOMPREGISTER application
//

#if !defined(AFX_JAVACOMPREGISTER_H__54B52AF1_42E0_4C6C_A821_D159565BD3D3__INCLUDED_)
#define AFX_JAVACOMPREGISTER_H__54B52AF1_42E0_4C6C_A821_D159565BD3D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CJavaCompRegisterApp:
// See JavaCompRegister.cpp for the implementation of this class
//

class CJavaCompRegisterApp : public CWinApp
{
public:
	CJavaCompRegisterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJavaCompRegisterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CJavaCompRegisterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JAVACOMPREGISTER_H__54B52AF1_42E0_4C6C_A821_D159565BD3D3__INCLUDED_)
