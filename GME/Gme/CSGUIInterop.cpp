#include "StdAfx.h"

#include <direct.h>

#import "C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727\mscorlib.tlb" no_implementation
#import "CSGUI.tlb" no_implementation

#import "CSGUI.tlb" implementation_only

#pragma comment(linker, "\"/manifestdependency:type='win32' processorArchitecture='msil' name='CSGUI' version='1.0.0.0' language='*'\"")

static HRESULT IsolationAwareCoCreateInstance_(const IID& clsid, const IID& iid, void** out)
{
	ULONG_PTR cookie;
	BOOL succ = IsolationAwareActivateActCtx(0, &cookie);
	ASSERT(succ);
	HRESULT hr = CoCreateInstance(clsid, 0, CLSCTX_ALL, iid, (void**)out);
	succ = IsolationAwareDeactivateActCtx(0, cookie);
	ASSERT(succ);
	return hr;
}

void MoveReferenceWithRefportConnectionsAndWriteToConsole(IMgaFCO* target, IMgaReference* ref)
{
	using namespace CSGUI;
	_ReferenceSwitcherPtr switcher;
	// n.b. RS is the coclass, _RS is the interface
	HRESULT hr = IsolationAwareCoCreateInstance_(__uuidof(ReferenceSwitcher), __uuidof(_ReferenceSwitcherPtr), (void**) &switcher);
	if (FAILED(hr))
		throw hresult_exception(E_MGA_REFPORTS_USED);
	switcher->SwitchReference(target, ref);
}