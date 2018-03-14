#if !defined(AFX_STDAFX_H__16AA33FA_B59A_4910_9F60_CAA3FF9A3EF1__INCLUDED_)
#define AFX_STDAFX_H__16AA33FA_B59A_4910_9F60_CAA3FF9A3EF1__INCLUDED_

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

#include <atlbase.h>

#define _ATL_APARTMENT_THREADED
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;

// Imports
#import "CoreLib.tlb" no_implementation no_namespace raw_method_prefix("") high_method_prefix("__") no_registry
#import "MetaLib.tlb" no_implementation no_namespace raw_method_prefix("") high_method_prefix("__") no_registry
#import "MgaLib.tlb" no_implementation no_namespace raw_method_prefix("") high_method_prefix("__") no_registry
#import "MgaUtilLib.tlb" no_implementation no_namespace raw_method_prefix("") high_method_prefix("__") no_registry
#import "ParserLib.tlb" no_implementation no_namespace raw_method_prefix("") high_method_prefix("__") no_registry
#import "GMELib.tlb" no_implementation no_namespace raw_method_prefix("") high_method_prefix("__") no_registry

#include "CommonImport.h"

#define cpp_quote(x) 
#include "Interfaceversion.h"
#undef cpp_quote
typedef GMEInterfaceVersion_enum GMEInterfaceVersion;

// Don't load the MIDL-generated headers
#define __Core_h__
#define __Meta_h__
#define __Mga_h__
#define __MgaUtil_h__
#define __MgaUtilLib_h__
#define __Parser_h__
#define __Gme_h__
#define __GmeLib_h__
#define IID_IMgaDataSource __uuidof(IMgaDataSource)
// End Imports

#include <atlcom.h>
#include "CommonError.h"
#include "CommonMfc.h"
#include "CommonMgaTrukk.h"

// Comment this out if you do not want to see the messages in the debug window
#define DEBUG_EVENTS


#ifdef DEBUG_EVENTS
 #define EVENT_TRACE(x) TRACE(x)
#else
 #define EVENT_TRACE(x)
#endif 



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__16AA33FA_B59A_4910_9F60_CAA3FF9A3EF1__INCLUDED_)
