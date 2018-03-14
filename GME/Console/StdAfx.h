#if !defined(AFX_STDAFX_H__237E732C_5B4E_4D36_94D1_25CD9EEC33AB__INCLUDED_)
#define AFX_STDAFX_H__237E732C_5B4E_4D36_94D1_25CD9EEC33AB__INCLUDED_

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _CRT_SECURE_NO_WARNINGS

//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxctl.h>         // MFC support for ActiveX Controls
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Comon Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Delete the two includes below if you do not wish to use the MFC
//  database classes
#include <afxdb.h>			// MFC database classes

#include "CommonError.h"
#include "CommonSmart.h"
#include "CommonMfc.h"
#include "CommonMgaTrukk.h"

#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to  override 
//something, but do not change the name of _Module 
extern CComModule _Module; 
#include <atlcom.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__237E732C_5B4E_4D36_94D1_25CD9EEC33AB__INCLUDED_)
