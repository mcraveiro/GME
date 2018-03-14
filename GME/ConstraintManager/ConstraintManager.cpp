// ConstraintManager.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL,
//      run nmake -f ConstraintManagerps.mk in the project directory.
#include "Solve4786.h"
#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "ConstraintManager.h"

#include "ConstraintManager_i.c"
#include "ConstraintMgr.h"
#include "Core_i.c"
#include "Mga_i.c"
#include "MgaUtil.h"
#include "ExpressionChecker.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_ConstraintManager, CConstraintMgr)
OBJECT_ENTRY(CLSID_ExpressionChecker, CExpressionChecker)
END_OBJECT_MAP()

class CConstraintManagerApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConstraintManagerApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CConstraintManagerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CConstraintManagerApp, CWinApp)
	//{{AFX_MSG_MAP(CConstraintManagerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CConstraintManagerApp theApp;

BOOL CConstraintManagerApp::InitInstance()
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

    _Module.Init(ObjectMap, m_hInstance, &LIBID_CONSTRAINTMANAGERLib);
    return CWinApp::InitInstance();
}

int CConstraintManagerApp::ExitInstance()
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
    AFX_MANAGE_STATE(AfxGetStaticModuleState());//z1
	return _Module.GetClassObject(rclsid, riid, ppv);
}

class CComponentReg
{
public:
	static HRESULT RegisterComponent();
	static HRESULT UnregisterComponent();
};


HRESULT CComponentReg::UnregisterComponent() {
  COMTRY {
	CComPtr<IMgaRegistrar> registrar;
	COMTHROW(registrar.CoCreateInstance(OLESTR("Mga.MgaRegistrar")));
	COMTHROW(registrar->UnregisterComponent(CComBSTR(MGR_COMPONENT_PROGID), REGACCESS_SYSTEM));
	COMTHROW(registrar->UnregisterComponent(CComBSTR(EXP_COMPONENT_PROGID), REGACCESS_SYSTEM));
  } COMCATCH(;)
}

HRESULT CComponentReg::RegisterComponent() {
  COMTRY {
	CComPtr<IMgaRegistrar> registrar;
	COMTHROW(registrar.CoCreateInstance(OLESTR("Mga.MgaRegistrar")));

	COMTHROW(registrar->RegisterComponent(CComBSTR(MGR_COMPONENT_PROGID), MGR_CETYPE, CComBSTR(MGR_COMPONENT_NAME), REGACCESS_SYSTEM));
	COMTHROW(registrar->Associate(CComBSTR(MGR_COMPONENT_PROGID), CComBSTR(MGR_PARADIGM), REGACCESS_SYSTEM));

	COMTHROW(registrar->RegisterComponent(CComBSTR(EXP_COMPONENT_PROGID), EXP_CETYPE, CComBSTR(EXP_COMPONENT_NAME), REGACCESS_SYSTEM));
	COMTHROW(registrar->Associate(CComBSTR(EXP_COMPONENT_PROGID), CComBSTR(EXP_PARADIGM), REGACCESS_SYSTEM));

	#ifdef ICON_SUPPORT
	COMTHROW(registrar->put_ComponentExtraInfo( REGACCESS_SYSTEM, CComBSTR( EXP_COMPONENT_PROGID ), CComBSTR( "Icon" ), CComBSTR( ",IDI_EC_CHECKALL" ) ) );
	#endif

  } COMCATCH(;)
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());//z1
	// registers object, typelib and all interfaces in typelib
	HRESULT res = _Module.RegisterServer(TRUE);
	if(res == S_OK) {
		COMRETURN( CComponentReg::RegisterComponent());
	}
    return res;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());//z1
	HRESULT res = _Module.UnregisterServer(TRUE);
	if(res == S_OK) {
		COMRETURN( CComponentReg::UnregisterComponent());
	}
	return res;
}


