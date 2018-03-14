// Component.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
//#include <afxctl.h>
#include <jni.h>
#include "ComponentDll.h"
#include "ComponentObj.h"
#include "ComponentConfig.h"
#include "MgaUtil.h"

extern bool const g_REGISTER_SYSTEMWIDE;
extern const char* g_COMPONENT_NAME;


#define COMRETURN(hr) { HRESULT res; if((res = (hr)) != S_OK) return res; }
#define VERIFYTHROW(FUNC) \
do { \
	if( !(bool)(FUNC) ) { \
		COMTHROW(E_FAIL); \
	} \
} while(false)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


class CJavaCompFactory: public COleObjectFactory
{
public:
    CJavaCompFactory(REFCLSID clsid, CRuntimeClass* pRuntimeClass,
		BOOL bMultiInstance, LPCTSTR lpszProgID): COleObjectFactory(clsid, pRuntimeClass, bMultiInstance, lpszProgID)

    {
    }

protected:
    CCmdTarget* OnCreateObject()
    {
        CComponentObj* p = (CComponentObj*)m_pRuntimeClass->CreateObject();

        p->rawcomp.m_javaClassPath = m_javaClassPath;
        p->rawcomp.m_javaClass     = m_javaClass;

	    return p;
    }

public:
    std::string  m_javaClassPath;
    std::string  m_javaClass;
};

/////////////////////////////////////////////////////////////////////////////
// CComponentApp

BEGIN_MESSAGE_MAP(CComponentApp, CWinApp)
	//{{AFX_MSG_MAP(CComponentApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComponentApp construction

CComponentApp::CComponentApp() : CWinApp(g_COMPONENT_NAME)
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CComponentApp object

CComponentApp theApp;

void CComponentApp::addComponents( HKEY root )
{
    char  buf[2000];
    char  buf2[2000];
    char  componentName[2000];        
    int   ind = 0;

    HKEY folder;
    VERIFYTHROW(RegOpenKeyEx(root, "SOFTWARE\\GME\\Components", 0, KEY_READ, &folder )==ERROR_SUCCESS);

    LONG  ret = RegEnumKey(folder, ind, componentName, 2000);
    while(ret == ERROR_SUCCESS)
    {
        char  classPath[2000];
        char  jclass[2000];
        DWORD len = 2000;
        HKEY  component;

        sprintf( buf, "SOFTWARE\\GME\\Components\\%s", componentName );
        VERIFYTHROW(RegOpenKeyEx(root, buf, 0, KEY_READ, &component )==ERROR_SUCCESS);
        if(RegQueryValueEx(component, "JavaClassPath", NULL, NULL, (unsigned char*)classPath, &len ) == ERROR_SUCCESS)
        {
            classPath[len] = '\0';
            // Java component!

            // query clsid
            HKEY  clsidkey;
            CLSID clsid;
            len = 2000;
            sprintf( buf, "%s\\CLSID", componentName );
            VERIFYTHROW(RegOpenKeyEx(HKEY_CLASSES_ROOT, buf, 0, KEY_READ, &clsidkey)==ERROR_SUCCESS);
            VERIFYTHROW(RegQueryValueEx(clsidkey, "", NULL, NULL, (unsigned char*)buf2, &len) == ERROR_SUCCESS);
            buf2[len] = '\0';
            CComBSTR clsidBstr( buf2 );             
            COMTHROW(CLSIDFromString(clsidBstr,&clsid));

            // query class
            len = 2000;
            jclass[0] = 0;
            if (RegQueryValueEx(component, "JavaClass", NULL, NULL, (unsigned char*)jclass, &len ) == ERROR_SUCCESS)
                jclass[len] = '\0';

            // create factory
            CJavaCompFactory * fac = new CJavaCompFactory(clsid, RUNTIME_CLASS(CComponentObj), FALSE, componentName);
            fac->m_javaClassPath = classPath;
            fac->m_javaClass     = jclass;
            m_factories.push_back(fac);
        }
        ind++;
        ret = RegEnumKey(folder, ind, componentName, 2000);
    }

    RegCloseKey(folder);
}

BOOL CComponentApp::InitInstance()
{
    GUID defGUID = {COCLASS_UUID_EXPLODED1,
					COCLASS_UUID_EXPLODED2,
					COCLASS_UUID_EXPLODED3,
					{	COCLASS_UUID_EXPLODED4,
						COCLASS_UUID_EXPLODED5,
						COCLASS_UUID_EXPLODED6,
						COCLASS_UUID_EXPLODED7,
						COCLASS_UUID_EXPLODED8,
						COCLASS_UUID_EXPLODED9,
						COCLASS_UUID_EXPLODED10,
						COCLASS_UUID_EXPLODED11}
					};
    CJavaCompFactory * defFactory = new CJavaCompFactory(defGUID, RUNTIME_CLASS(CComponentObj), FALSE, COCLASS_PROGID);
    m_factories.push_back(defFactory);
    try
	{
        addComponents( HKEY_LOCAL_MACHINE );
        addComponents( HKEY_CURRENT_USER );        
    }
    catch(...)
    {
    }
  
	COleObjectFactory::RegisterAll();

	return TRUE;
}

int CComponentApp::ExitInstance()
{
    // free m_factories
    for( unsigned int i=0; i<m_factories.size(); ++i )
        delete m_factories[i];

    return CWinApp::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{    
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    /*if( theApp.m_javaVMDll == NULL )
        theApp.loadJavaVM();*/
    HRESULT res = AfxDllGetClassObject(rclsid, riid, ppv);
    return res;
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}


//#define COMRETURN(hr) { HRESULT res; if((res = (hr)) != S_OK) return res; }

// this flag supresses running Dll(Un)RegisterServer if DllInstall is also used
// bool called = false; 

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
/*	
	if(called) return S_OK;
	called = true;
*/
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!COleObjectFactory::UpdateRegistryAll(TRUE))
		return SELFREG_E_CLASS;

	// Note:
	// We can register the typelib either from the .tlb file
	// or from the resources. But the resource ID of the 
	// TYPELIB must be 1 !!!
/*
	if( !AfxOleRegisterTypeLib(AfxGetInstanceHandle(), LIBID_MgaComponentLib, NULL) )
		return E_FAIL;
*/
	CComponentReg reg;
	if (g_REGISTER_SYSTEMWIDE)
		COMRETURN(reg.RegisterParadigms(REGACCESS_SYSTEM))
	else
		COMRETURN(reg.RegisterParadigms())
	

	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
/*	
	if(called) return S_OK;
	called = true;
*/
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!COleObjectFactory::UpdateRegistryAll(FALSE))
		return SELFREG_E_CLASS;

	CComponentReg reg;
    if (g_REGISTER_SYSTEMWIDE)
		COMRETURN(reg.UnregisterParadigms(REGACCESS_SYSTEM))
	else
		COMRETURN(reg.UnregisterParadigms())
	
	return S_OK;
}

#define DIM(x) (sizeof(x)/sizeof((x)[0]))

STDAPI DllInstall(BOOL bInstall, LPCWSTR cmdl) {

	regaccessmode_enum code = REGACCESS_NONE;
	if(iswdigit(cmdl[0]) && cmdl[1] == 0) {
		code = regaccessmode_enum(cmdl[0] - L'0');
	}
	else {
		struct {
			LPCWSTR name;
			regaccessmode_enum cc;
		} mnemonics[] = { { L"USER" , REGACCESS_USER},
						  { L"REGACCESS_USER" , REGACCESS_USER},	
						  { L"SYSTEM" , REGACCESS_SYSTEM},	
						  { L"REGACCESS_SYSTEM" , REGACCESS_SYSTEM},	
						  { L"BOTH" , REGACCESS_BOTH},	
						  { L"REGACCESS_BOTH" , REGACCESS_BOTH} };
		for(int i = 0; i < DIM(mnemonics); i++) {
			if(_wcsicmp(cmdl, mnemonics[i].name) == 0) {
				code = mnemonics[i].cc;
				break;
			}
		}
	}
	
//	if(called || code == REGACCESS_NONE) return E_INVALIDARG;
//	called = true;

	CComponentReg reg;
	if(bInstall) { COMRETURN(reg.RegisterParadigms(code)); }
	else		 { COMRETURN(reg.UnregisterParadigms(code)); }
	return S_OK;
};



