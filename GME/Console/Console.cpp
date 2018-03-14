// Console.cpp : Implementation of CConsoleApp and DLL registration.

#include "stdafx.h"
#include "Console.h"
#include <initguid.h>
#include "Console_i.c"
#include "ScriptHost.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CConsoleApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x339fd307, 0x717b, 0x4698, { 0xac, 0x13, 0x4a, 0xcc, 0xcf, 0x5d, 0x4c, 0x91 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


////////////////////////////////////////////////////////////////////////////
// CConsoleApp::InitInstance - DLL initialization

BOOL CConsoleApp::InitInstance()
{
#if _MSC_VER < 1700
	// See MSDN example code for CWinApp::InitInstance: http://msdn.microsoft.com/en-us/library/ae6yx0z0.aspx
	// MFC module state handling code is changed with VC80.
	// We follow the Microsoft's suggested way, but in case of any trouble the set the
	// HKCU\Software\GME\AfxSetAmbientActCtxMod key to 0
	UINT uAfxSetAmbientActCtxMod = 1;
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\GME\\"),
					 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		TCHAR szData[128];
		DWORD dwKeyDataType;
		DWORD dwDataBufSize = sizeof(szData)/sizeof(TCHAR);

		if (RegQueryValueEx(hKey, _T("AfxSetAmbientActCtxMod"), NULL, &dwKeyDataType,
							(LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
		{
			uAfxSetAmbientActCtxMod = _tcstoul(szData, NULL, 10);
		}

		RegCloseKey(hKey);
	}
	if (uAfxSetAmbientActCtxMod != 0)
	{
		AfxSetAmbientActCtx(FALSE);
	}
#endif

	BOOL bInit = COleControlModule::InitInstance();
	if (!InitATL())
		return FALSE;


	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CConsoleApp::ExitInstance - DLL termination

int CConsoleApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	_Module.Term();


	return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return _Module.RegisterServer(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return _Module.UnregisterServer(TRUE); //TRUE indicates that typelib is unreg'd
}


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_ScriptHost, CScriptHost)
END_OBJECT_MAP()

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE; 
} 
/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(AfxDllGetClassObject(rclsid, riid, ppv) == S_OK)
		return S_OK;
	return _Module.GetClassObject(rclsid, riid, ppv);
}

BOOL CConsoleApp::InitATL()
{
	_Module.Init(ObjectMap, AfxGetInstanceHandle());
	return TRUE;

}
