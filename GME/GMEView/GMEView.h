#pragma once

// GMEView.h : main header file for GMEView.DLL

#if !defined( __AFXCTL_H__ )
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

enum eDragOperation { DRAGOP_COPY, DRAGOP_MOVE, DRAGOP_REFERENCE, DRAGOP_INSTANCE, DRAGOP_SUBTYPE, DRAGOP_CLOSURE, DRAGOP_CLOSURE_MERGE };

// CGMEViewApp : See GMEView.cpp for implementation.

class CGMEViewApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

