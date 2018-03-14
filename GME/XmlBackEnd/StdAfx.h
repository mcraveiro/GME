// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__6DE09E50_EB29_43FD_A37B_A41FA6758CA0__INCLUDED_)
#define AFX_STDAFX_H__6DE09E50_EB29_43FD_A37B_A41FA6758CA0__INCLUDED_

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _CRT_SECURE_NO_WARNINGS

#define _ATL_APARTMENT_THREADED

#include <afxwin.h>
#include <afxdisp.h>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <hash_map>

#define _NOTHREADS

#include "SvnConfig.h"

#include "XmlBackEnd.h"
#include "CommonSmart.h"
#include "CommonStl.h"
#include "CoreUtilities.h"

#include "Core.h"
#include "CoreLib.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6DE09E50_EB29_43FD_A37B_A41FA6758CA0__INCLUDED)
