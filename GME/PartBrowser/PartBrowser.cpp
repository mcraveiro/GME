// PartBrowser.cpp : Implementation of CPartBrowserApp and DLL registration.

#include "stdafx.h"
#include "PartBrowser.h"
#include "PartBrowserCtrl.h"
#include "PartBrowser_i.c"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CPartBrowserApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xEA3F7431, 0x913A, 0x421F, { 0x9B, 0x9C, 0xBE, 0xB6, 0xA7, 0x35, 0x28, 0x5A } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()


// CPartBrowserApp::InitInstance - DLL initialization

BOOL CPartBrowserApp::InitInstance()
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

    _Module.Init(ObjectMap, m_hInstance, &LIBID_PartBrowserLib);

//	return CWinApp::InitInstance();
	return COleControlModule::InitInstance();
}



// CPartBrowserApp::ExitInstance - DLL termination

int CPartBrowserApp::ExitInstance()
{
	// TODO: Add your own module termination code here.
    _Module.Term();

//	return CWinApp::ExitInstance();
	return COleControlModule::ExitInstance();
}

/*
/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());//z
	return _Module.GetClassObject(rclsid, riid, ppv);
}
*/
/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
