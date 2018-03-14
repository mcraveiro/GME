// ComponentDll.h : main header file for the Component DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <vector>
#include "MgaUtil.h"

typedef jint (JNICALL *P_JNI_CreateJavaVM)        (JavaVM **pvm, void** penv, void *args);
typedef jint (JNICALL *P_JNI_GetCreatedJavaVMs)   (JavaVM **vmBuf,jsize bufLen, jsize *nVMs);

/////////////////////////////////////////////////////////////////////////////
// CComponentApp
// See Component.cpp for the implementation of this class
//

class CComponentApp : public CWinApp
{
public:
	CComponentApp();
    
    void addComponents( HKEY folder );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComponentApp)
	public:
	virtual BOOL InitInstance();    
	//}}AFX_VIRTUAL

    virtual int ExitInstance();

	//{{AFX_MSG(CComponentApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//protected:

public:
    std::vector<COleObjectFactory*> m_factories;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.



class CUACUtils
{
public:

	// Vista-dependent icon constants (copied from Commctrl.h)
#ifndef BCM_FIRST
	static const DWORD BCM_FIRST = 0x1600;
#endif
#ifndef BCM_SETSHIELD
	static const DWORD BCM_SETSHIELD = (BCM_FIRST + 0x000C);
#endif

	static bool isVistaOrLater() 
	{
		OSVERSIONINFO osvi;

		::ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		GetVersionEx(&osvi);

		return (osvi.dwMajorVersion >= 6);
	}

	static bool isElevated()
	{
		if ( !isVistaOrLater() ) {
			return true;
		}

		HANDLE hToken   = NULL;

		if ( !::OpenProcessToken( 
					::GetCurrentProcess(), 
					TOKEN_QUERY, 
					&hToken ) )
		{
			return false;
		}

		DWORD dwReturnLength = 0;
		TOKEN_ELEVATION te;
		::ZeroMemory(&te, sizeof(te));

		if ( ::GetTokenInformation(
					hToken,
					TokenElevation,
					&te,
					sizeof(te),
					&dwReturnLength ) )
		{
				ASSERT( dwReturnLength == sizeof( te ) );
				return (te.TokenIsElevated != 0);
		}

		::CloseHandle( hToken );

		return false;
	}

	template <typename T>
	static HRESULT CreateElevatedInstance(LPCOLESTR progId,
                               T** object, HWND window = 0)
	{
		CLSID clsId;
		HRESULT hr = ::CLSIDFromProgID(progId, &clsId);
		if (FAILED(hr)) {
			return hr;
		}

		return CreateElevatedInstance(clsId, object, window);
	}

	template <typename T>
	static HRESULT CreateElevatedInstance(REFCLSID classId,
                               T** object, HWND window = 0)
	{
		BIND_OPTS3 bindOptions;
		gettokeninformation
		::ZeroMemory(&bindOptions, sizeof (BIND_OPTS3));
		bindOptions.cbStruct = sizeof (BIND_OPTS3);
		bindOptions.hwnd = window;
		bindOptions.dwClassContext = CLSCTX_LOCAL_SERVER;

		WCHAR wszMonikerName[300];
		WCHAR wszCLSID[50];

		#define cntof(a) (sizeof(a)/sizeof(a[0]))

		::StringFromGUID2(classId, wszCLSID,
									 cntof(wszCLSID));

		HRESULT hr = ::StringCchPrintfW(wszMonikerName, cntof(wszMonikerName), L"Elevation:Administrator!new:%s", wszCLSID);
	
		if (FAILED(hr))
		{
			return hr;
		}

		return ::CoGetObject(wszMonikerName,
							 &bindOptions,
							 __uuidof(T),
							 reinterpret_cast<void**>(object));
	}

	static void SetShieldIcon(const CButton& button, bool on=true)
	{
		button.SendMessage(BCM_SETSHIELD, 0, on ? TRUE : FALSE);
	}
};


class CComponentReg
{
public:
	CComponentReg();

	CStringList paradigms;
	HRESULT RegisterParadigms(regaccessmode_enum loc = REGACCESS_USER);
	HRESULT UnregisterParadigms(regaccessmode_enum loc = REGACCESS_USER);
};
