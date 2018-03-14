#if !defined(AFX_STDAFX_H__0A65F2CE_B54D_4572_853D_C7D495E70C5F__INCLUDED_)
#define AFX_STDAFX_H__0A65F2CE_B54D_4572_853D_C7D495E70C5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				
#define WINVER 0x0501		
#endif

#ifndef _WIN32_WINNT		
#define _WIN32_WINNT 0x0501	
#endif						

#ifndef _WIN32_WINDOWS		
#define _WIN32_WINDOWS 0x0510
#endif

#ifndef _WIN32_IE			
#define _WIN32_IE 0x0600	
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <afxctl.h>         // MFC support for ActiveX Controls
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Comon Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Delete the two includes below if you do not wish to use the MFC
//  database classes
#include <afxdb.h>			// MFC database classes
#include <afxdao.h>			// MFC DAO database classes

#include <atlbase.h>
#include <atlcom.h>

#include "..\Interfaces\Meta.h"
#include "..\Interfaces\MgaUtil.h"
#include "..\Interfaces\GME.h"
#include "..\Interfaces\MgaDecorator.h"
#include "..\Common\CommonError.h"
#include "..\Common\CommonSmart.h"
#include "..\Common\CommonMfc.h"
#include "..\Common\CommonMgaTrukk.h"

// Comment this out if you do not want to see the messages in the debug window
#define DEBUG_EVENTS


#ifdef DEBUG_EVENTS
 #define EVENT_TRACE(x) TRACE(x)
#else
 #define EVENT_TRACE(x)
#endif 


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0A65F2CE_B54D_4572_853D_C7D495E70C5F__INCLUDED_)
