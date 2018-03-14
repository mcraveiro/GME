// GMEActiveBrowser.cpp : Implementation of CGMEActiveBrowserApp and DLL registration.

#include "stdafx.h"
#include "GMEActiveBrowser.h"
#include <initguid.h>
#include "GMEActiveBrowser_i.c"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGMEActiveBrowserApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xfda9edb8, 0xed6a, 0x468f, { 0xa3, 0xa9, 0xe9, 0x36, 0x1f, 0xee, 0xd0, 0x51 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserApp::InitInstance - DLL initialization

BOOL CGMEActiveBrowserApp::InitInstance()
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
		m_pCtrl=NULL;
		
		SetRegistryKey(_T("GME\\GUI"));

		free((void*)m_pszProfileName);
		//Change the name of the .INI file.
		//The CWinApp destructor will free the memory.
		m_pszProfileName=_tcsdup(_T("GMEActiveBrowser"));


	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserApp::ExitInstance - DLL termination

int CGMEActiveBrowserApp::ExitInstance()
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

	return NOERROR;
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

	_Module.UnregisterServer(TRUE); //TRUE indicates that typelib is unreg'd

	return NOERROR;
}

	
CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
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

BOOL CGMEActiveBrowserApp::InitATL()
{
	_Module.Init(ObjectMap, AfxGetInstanceHandle());
	return TRUE;

}


// Stupid MFC does not make the Control object reachable from the App.

CGMEActiveBrowserCtrl* CGMEActiveBrowserApp::GetCtrl()
{
	ASSERT(m_pCtrl);
	return m_pCtrl;
}

void CGMEActiveBrowserApp::SetCtrl(CGMEActiveBrowserCtrl *pCtrl)
{
	m_pCtrl=pCtrl;
}
