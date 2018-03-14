// Mga.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f Mgaps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>

#include "Core_i.c"
#include "CoreLib_i.c"
#include "Meta_i.c"
#include "Mga_i.c"
#include "MgaLib_i.c"
#include "Gme_i.c"

#include "MgaProject.h"
#include "MgaFCO.h"

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Core' version='1.0.0.0' language='*'\"")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Meta' version='1.0.0.0' language='*'\"")


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_MgaProject, CMgaProject)
OBJECT_ENTRY(CLSID_MgaO, FCO)
OBJECT_ENTRY(CLSID_MgaFCOs, CMgaFCOs)
OBJECT_ENTRY(CLSID_MgaFolders, CMgaFolders)
OBJECT_ENTRY(CLSID_MgaObjects, CMgaObjects)
OBJECT_ENTRY(CLSID_MgaRegNodes, CMgaRegNodes)
//OBJECT_ENTRY(CLSID_MgaFolder, CMgaFolder)
//OBJECT_ENTRY(CLSID_MgaModel, CMgaModel)
//OBJECT_ENTRY(CLSID_MgaAtom, CMgaAtom)
//OBJECT_ENTRY(CLSID_MgaConnection, CMgaSimpleConnection)
//OBJECT_ENTRY(CLSID_MgaReference, CMgaReference)
//OBJECT_ENTRY(CLSID_MgaSet, CMgaSet)
//OBJECT_ENTRY(CLSID_MgaSimpleConnection, CMgaSimpleConnection)
//OBJECT_ENTRY(CLSID_MgaFilter, CMgaFilter)
//OBJECT_ENTRY(CLSID_MgaConstraint, CMgaConstraint)
//OBJECT_ENTRY(CLSID_MgaClient, CMgaClient)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_MGALib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT res = _Module.RegisterServer(TRUE);
	return res;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    HRESULT res = _Module.UnregisterServer(TRUE);
	return res;
}

/////////////////////////////////////////////////////////////////////////////

const CoreObj NULLCOREOBJ;
const CComVariant NULLVARIANT;

/*
HRESULT check_location_compatibility(IMgaFCO *newobj, IMgaFCO *oldobj) {
	CComPtr<IMgaProject> p;
	HRESULT hr;
	if((hr = newobj->get_Project(&p)) != S_OK) return hr;
	if(!oldobj) {
		if(p->Open != CMgaProject::Open) return E_SAMEPROJECT;
	}
	else {
		return oldobj->CheckProject(p);
	}
	return S_OK;
}
*/

/////////////////////////////////////////////////////////////////////////////


#include <time.h>
CComBSTR Now(void) {
	time_t t;
	time(&t);
	char *p = ctime(&t);
	*strchr(p,'\n') = '\0';
	return p;

}


#ifdef OWN_META
#include "MgaMetaProject.cpp"
#include "MgaMetaModel.cpp"
#endif
#include "MgaO.h"

