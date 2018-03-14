// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__70C41B18_E3CE_11D3_B37A_005004D38590__INCLUDED_)
#define AFX_STDAFX_H__70C41B18_E3CE_11D3_B37A_005004D38590__INCLUDED_

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

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>


#define ASSERT ATLASSERT

// Imports
#define wireHWND HWND

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
#define ATTVAL_ENUM _attval_enum

// End Imports

#include "CommonSmart.h"
#include "CommonStl.h"
#define GLOBAL_ID_STR   "guid"
#define GLOBAL_ID_LEN   38
#define PREV_GLOBAL_ID_STR   "guid/prev"
#define PREV_ID_STR     "prev"
typedef IMgaConstraint IMgaMetaConstraint;
typedef IMgaConstraints IMgaMetaConstraints;

#include <string>
namespace std {
#ifdef UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
static void CopyTo(const BSTR from, std::wstring& to)
{
	if (from != NULL) {
		to = from;
	} else {
		to = L"";
	}
}
#include "comutil.h"
#include "comdef.h"
static _bstr_t PutInBstr(const std::wstring& str)
{
	return _bstr_t(str.c_str());
}
static _bstr_t PutInBstr(const std::string& str)
{
	return _bstr_t(str.c_str());
}

class XMLPlatformUtilsTerminate_RAII {
public:
	XMLPlatformUtilsTerminate_RAII();
	~XMLPlatformUtilsTerminate_RAII();
};

template <const IID* piid1, const IID* piid2>
class ATL_NO_VTABLE ISupportErrorInfoImpl2 : 
	public ISupportErrorInfo
{
public:
	STDMETHOD(InterfaceSupportsErrorInfo)(_In_ REFIID riid)
	{
		return (InlineIsEqualGUID(riid,*piid1) || InlineIsEqualGUID(riid,*piid2)) ? S_OK : S_FALSE;
	}
};

template <const IID* piid1, const IID* piid2, const IID* piid3>
class ATL_NO_VTABLE ISupportErrorInfoImpl3 : 
	public ISupportErrorInfo
{
public:
	STDMETHOD(InterfaceSupportsErrorInfo)(_In_ REFIID riid)
	{
		return (InlineIsEqualGUID(riid,*piid1) || InlineIsEqualGUID(riid,*piid2) || InlineIsEqualGUID(riid,*piid3)) ? S_OK : S_FALSE;
	}
};

extern _locale_t c_locale;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__70C41B18_E3CE_11D3_B37A_005004D38590__INCLUDED)
