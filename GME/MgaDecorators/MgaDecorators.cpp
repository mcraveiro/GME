// MgaDecorators.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f MgaDecoratorsps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "Gme.h"
#include "MgaDecoratorLib.h"
#include "MgaDecoratorLib_i.c"
#include "BoxDecoratorImpl.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_BoxDecorator, CBoxDecoratorImpl)
END_OBJECT_MAP()

class CMgaDecoratorsApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMgaDecoratorsApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMgaDecoratorsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CMgaDecoratorsApp, CWinApp)
	//{{AFX_MSG_MAP(CMgaDecoratorsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMgaDecoratorsApp theApp;

BOOL CMgaDecoratorsApp::InitInstance()
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

    _Module.Init(ObjectMap, m_hInstance, &LIBID_MGADECORATORLib);
    return CWinApp::InitInstance();
}

int CMgaDecoratorsApp::ExitInstance()
{
    _Module.Term();
    return CWinApp::ExitInstance();
}

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
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}


