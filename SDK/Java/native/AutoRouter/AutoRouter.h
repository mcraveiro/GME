// AutoRouter.h : main header file for the AUTOROUTER DLL
//

#if !defined(AFX_AUTOROUTER_H__1BD63F99_F623_4A2F_A3A5_51813A017081__INCLUDED_)
#define AFX_AUTOROUTER_H__1BD63F99_F623_4A2F_A3A5_51813A017081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAutoRouterApp
// See AutoRouter.cpp for the implementation of this class
//

class CAutoRouterApp : public CWinApp
{
public:
	CAutoRouterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoRouterApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAutoRouterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOROUTER_H__1BD63F99_F623_4A2F_A3A5_51813A017081__INCLUDED_)
