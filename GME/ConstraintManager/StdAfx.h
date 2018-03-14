// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__B08088DA_93F4_4F85_ACD8_CF02148B472A__INCLUDED_)
#define AFX_STDAFX_H__B08088DA_93F4_4F85_ACD8_CF02148B472A__INCLUDED_

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS 1

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

// Defined in project settings (because the ANTLR generated stuff does not include stdafx.h) 
//#define _CRT_SECURE_NO_WARNINGS

//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#define _ATL_APARTMENT_THREADED

#include <afxwin.h>
#include <afxdisp.h>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <afxole.h>
#include "AfxCmn.h"
#include "resource.h"

#include "commonsmart.h"
#include "CommonMFC.h"
#include "CommonMgaTrukk.h"


#define XASSERT(x) if(!(x)) COMTHROW( ((x), -1))

#include "string"
#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B08088DA_93F4_4F85_ACD8_CF02148B472A__INCLUDED)
