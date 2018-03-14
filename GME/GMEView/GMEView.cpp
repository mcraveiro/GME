// GMEView.cpp : Implementation of CGMEViewApp and DLL registration.

#include "stdafx.h"
#include "GMEView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CGMEViewApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xE8F51618, 0x624F, 0x41D6, { 0xB3, 0x8F, 0x1, 0xC8, 0x1, 0x82, 0xAE, 0xDC } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CGMEViewApp::InitInstance - DLL initialization

BOOL CGMEViewApp::InitInstance()
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

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}



// CGMEViewApp::ExitInstance - DLL termination

int CGMEViewApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
