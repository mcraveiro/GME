// stdafx.cpp : source file that includes just the standard includes
//	MgaUtil.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "resource.h"


#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#endif

#import "CoreLib.tlb" implementation_only no_namespace raw_method_prefix("") high_method_prefix("__") no_registry
#import "MgaLib.tlb" implementation_only no_namespace raw_method_prefix("") high_method_prefix("__") no_registry
#import "MetaLib.tlb" implementation_only no_namespace raw_method_prefix("") high_method_prefix("__") no_registry
#import "ParserLib.tlb" implementation_only no_namespace raw_method_prefix("") high_method_prefix("__") no_registry
#import "MgaUtilLib.tlb" implementation_only no_namespace raw_method_prefix("") high_method_prefix("__") no_registry

#if defined(_M_IX86)
#define PROCESSOR_ARCHITECTURE "x86"
#elif defined(_M_X64)
#define PROCESSOR_ARCHITECTURE "amd64"
#endif
#pragma comment(linker,"/manifestdependency:\"type='win32'  name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='" PROCESSOR_ARCHITECTURE "' publicKeyToken='6595b64144ccf1df' language='*'\"")

