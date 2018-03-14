
#include "stdafx.h"
#include "htmlhelp.h"
#include "MgaLauncher.h"

#include "MetaDlg.h"
#include "GmeDlg.h"
#include "CompDlg.h"
#include "RegistryBrowserDlg.h"
#include "AnnotationBrowserDlg.h"
#include "CommonComponent.h"

#include <comutil.h>
#include <comdef.h>

#include "CrashRpt.h"
#ifdef _DEBUG
#pragma comment(lib, "CrashRptd.lib")
#else
#pragma comment(lib, "CrashRpt.lib")
#endif


/* Deprecated Web based help
#define GME_UMAN_HOME				"http://www.isis.vanderbilt.edu/projects/GME/Doc/UsersManual/"
#define GME_UMAN_CONTENTS			"helpcontents1.htm"
#define GME_UMAN_MODEL				"models.htm"
#define GME_UMAN_ATOM				"atoms.htm"
#define GME_UMAN_REFERENCE			"references.htm"
#define GME_UMAN_SET				"sets.htm"
#define GME_UMAN_CONNECTION			"connectionsandlinks.htm"
#pragma bookmark ( change when folders.htm exists for documentation )
#define GME_UMAN_FOLDER				"themodelbrowser.htm"
*/

#define GME_UMAN_HOME				"Doc/GME Manual and User Guide.chm::/"
#define GME_UMAN_CONTENTS			"index.html"
#define GME_UMAN_MODEL				"models.html"
#define GME_UMAN_ATOM				"atoms.html"
#define GME_UMAN_REFERENCE			"references.html"
#define GME_UMAN_SET				"sets.html"
#define GME_UMAN_CONNECTION			"connectionsandlinks.html"
#pragma bookmark ( change when folders.htm exists for documentation )
#define GME_UMAN_FOLDER				"themodelbrowser.html"


// --------------------------- CMgaLauncher

STDMETHODIMP CMgaLauncher::put_ParadigmName(BSTR p)
{
	COMTRY
	{
		paradigmname = p;
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaLauncher::put_Parameter(VARIANT p)
{
	COMTRY
	{
		parameter = p;
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaLauncher::put_ComponentType(componenttype_enum type)
{
	comptype = type;

	return S_OK;
}

STDMETHODIMP CMgaLauncher::get_ParadigmName(BSTR *p)
{
	CHECK_OUT(p);

	COMTRY
	{
            CopyTo(paradigmname, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaLauncher::get_ConnStr(BSTR *p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CopyTo(connstr, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaLauncher::get_ProgID(BSTR *p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CopyTo(progid, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaLauncher::get_Paradigm(IMgaMetaProject **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		CopyTo(paradigm, p);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaLauncher::MetaDlg(metadlg_enum flags)
{
	COMTRY
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		connstr.Empty();
		paradigm.Release();

		CMetaDlg dlg;

		dlg.name = paradigmname;
		dlg.flags = flags;

		if( dlg.DoModal() != IDOK )
			COMRETURN(S_FALSE);

		paradigmname = dlg.name;
		if( paradigmname.IsEmpty() )
			COMRETURN(S_FALSE);

		connstr = dlg.connstr;
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaLauncher::GmeDlg()
{
	COMTRY
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CGmeDlg dlg;

		if( dlg.DoModal() != IDOK )
			COMRETURN(S_FALSE);
	} 
	COMCATCH(;)
}

STDMETHODIMP CMgaLauncher::ComponentDlg(componentdlg_enum flags)
{
	COMTRY
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		progid.Empty();

		CCompDlg dlg;

		dlg.paradigm = paradigmname;
		dlg.type = comptype;
		dlg.parameter = parameter;

		if( dlg.DoModal() != IDOK )
			COMRETURN(S_FALSE);

		progid = dlg.progid;
		if( progid.IsEmpty() )
			COMRETURN(S_FALSE);
	}
	COMCATCH(;)
}


STDMETHODIMP CMgaLauncher::PropDlg(IMgaObject * obj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return E_NOTIMPL;
}

STDMETHODIMP CMgaLauncher::AttrDlg(IMgaObject * obj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	COMTRY
	{

#ifdef _DEBUG
		AfxMessageBox(_T("dummy AttrDlg() implementation-- only in Debug version"));
#endif

	}
	COMCATCH(;)
}

LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hKey;
    LONG lResult = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hKey); 
  
    if( lResult == ERROR_SUCCESS)
	{
         long lDataSize = MAX_PATH;
		TCHAR data[MAX_PATH];
  
		RegQueryValue( hKey, NULL, data, &lDataSize);
		lstrcpy( retdata,data);
		RegCloseKey( hKey);
    }
  
    return lResult;
}
  
HINSTANCE GotoURL(LPCTSTR url, int showcmd)
{
//	HtmlHelp(NULL, url, HH_DISPLAY_TOPIC, 0);
//	return NULL;

	TCHAR key[MAX_PATH + MAX_PATH];
  
    // First try ShellExecute()
    HINSTANCE hResult = ShellExecute( NULL, _T("open"), url, NULL,NULL, showcmd);

    // If it failed, get the .htm regkey and lookup the program
	if( (UINT)hResult <= HINSTANCE_ERROR)
	{
		if( GetRegKey( HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS)
		{
			lstrcat( key, _T("\\shell\\open\\command")); 
  
            if( GetRegKey( HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS)
			{
				TCHAR *pos;
                pos = _tcsstr( key, _T("\"%1\"")); 
  
                if( pos == NULL)
				{
					// No quotes found
                    pos = _tcsstr( key, _T("%1")); // Check for % 1, without quotes
                    if( pos == NULL)    // No  parameter at all...
                        pos = key+lstrlen( key)-1;
                    else
						*pos = '\0';    //  Remove the parameter
                }
                else
					*pos = '\0';
					// Remove the parameter
  
                lstrcat(pos, _T(" "));
                lstrcat(pos, url);
  
				// FIXME: should use CreateProcess
				hResult = (HINSTANCE)WinExec( CStringA(key),showcmd);
			}
		}
	}
  
	return hResult;
}


STDMETHODIMP CMgaLauncher::ShowHelp(IMgaObject* obj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COMTRY
	{
		CComPtr<IMgaFolder> imf;
		CComPtr<IMgaFCO> imfco;
		CString url;

		if (obj != NULL) 
		{
			if (FAILED(obj->QueryInterface(&imf)) &&
				FAILED(obj->QueryInterface(&imfco)))
			{
				COMTHROW(E_INVALIDARG);
			}

			CComBSTR bstrVal;
			CComBSTR bstr = L"help";
			COMTHROW((imf != NULL) ? 
				imf->get_RegistryValue(bstr,&bstrVal)
				:
				imfco->get_RegistryValue(bstr,&bstrVal)
				);

			
			CopyTo(bstrVal,url);

			// Resolving $PARADIGMDIR
			CComPtr<IMgaProject> project;
			if (imf)
				COMTHROW(imf->get_Project(&project));
			else
				COMTHROW(imfco->get_Project(&project));
			_bstr_t paradigmConnStr;
			COMTHROW(project->get_ParadigmConnStr(paradigmConnStr.GetAddress()));
			if (_wcsnicmp(_T("MGA="), paradigmConnStr, 4) == 0) 
			{
				CString filename, dirname;
				GetFullPathName(CString(static_cast<const TCHAR*>(paradigmConnStr) + 4), filename, dirname);

				if (dirname != "")
				{
					url.Replace(_T("$PARADIGMDIR"), dirname);
				}
			}


			// Supporting one environment variable with $VARNAME$ syntax
			int firstIdx = url.Find(_T('$'));
			if(firstIdx  != -1 && url.GetLength() > firstIdx+2) // Reference to environment variable with a non-empty name
			{
				int nextIdx = url.Find(_T('$'), firstIdx +1); // Finding the closing $

				if(nextIdx != -1 && nextIdx > firstIdx +1)
				{
					CString variableName = url.Mid(firstIdx+1, nextIdx - firstIdx -1);
					TCHAR* value = _tgetenv(variableName);

					if(value != NULL)
					{
						CString variableString;
						variableString.Format(_T("$%s$"), variableName);
						url.Replace(variableString, value);
					}

				}

			}

			CString name;
			COMTHROW(obj->get_Name(PutOut(name)));
		}

		if (!url.IsEmpty())
		{
			GotoURL(url,SW_SHOWMAXIMIZED);
		}
		else
		{
			if (obj == NULL)
			{
				url = GME_UMAN_CONTENTS;
			} 
			else
			{
				objtype_enum obj_t;
				COMTHROW( obj->get_ObjType(&obj_t) );

				switch (obj_t) {

					case OBJTYPE_MODEL : {
						url = GME_UMAN_MODEL;
						break;
					}
					case OBJTYPE_ATOM : {
						url = GME_UMAN_ATOM;
						break;
					}
					case OBJTYPE_REFERENCE : {
						url = GME_UMAN_REFERENCE;
						break;
					}
					case OBJTYPE_SET : {
						url = GME_UMAN_SET;
						break;
					}
					case OBJTYPE_CONNECTION : {
						url = GME_UMAN_CONNECTION;
						break;
					}
					case OBJTYPE_FOLDER : {
						url = GME_UMAN_FOLDER;
						break;
					}
				}
			}

			if (!url.IsEmpty())
			{
				CString gmeRoot(_T("../"));
				// Use an absolute path based on the GME_ROOT environment variable, instead of a relative path if we can
				TCHAR* gme_root_env = NULL;
				gme_root_env = _tgetenv(_T("GME_ROOT"));
				if (gme_root_env) {
					long len = _tcslen(gme_root_env);
					bool hasSlashAtTheEnd = (gme_root_env[len - 1] == '\\' || gme_root_env[len - 1] == '/');
					gmeRoot = gme_root_env;
					if (!hasSlashAtTheEnd)
						gmeRoot = gmeRoot + "/";
				}
				gmeRoot.Replace(_T("\\"), _T("/"));
				CString fullUrl = CString(_T("ms-its:")) + gmeRoot + GME_UMAN_HOME + url;
				CWnd* mainWnd = AfxGetMainWnd();
				HWND hwndCaller = NULL;
				if (mainWnd != NULL)
					hwndCaller = mainWnd->m_hWnd;
				HWND helpWnd = NULL;
#ifndef _M_X64
				helpWnd = ::HtmlHelp(hwndCaller, fullUrl, HH_DISPLAY_TOPIC, 0);
				if (helpWnd == NULL && url != GME_UMAN_CONTENTS) {
					fullUrl = CString(_T("ms-its:")) + gmeRoot + GME_UMAN_HOME + GME_UMAN_CONTENTS;
					helpWnd = ::HtmlHelp(hwndCaller, fullUrl, HH_DISPLAY_TOPIC, 0);
				}
#endif
				if (helpWnd == NULL)
					AfxMessageBox(_T("Couldn't find help file or help topic: ") + fullUrl, MB_OK | MB_ICONSTOP);
			}
			else
			{
				AfxMessageBox(_T("No default help is available for selection!"), MB_OK | MB_ICONSTOP);
			}
		}
	}
	COMCATCH(;)
}

static int __stdcall nopExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS* ep) {
	// FIXME: if (ep->ExceptionRecord->ExceptionFlags & EXCEPTION_NONCONTINUABLE)
	return EXCEPTION_EXECUTE_HANDLER;
}

// returns 0 if an exception was caught
bool __stdcall InvokeExWithCrashRpt(IMgaComponentEx* compex, IMgaProject* project, IMgaFCO* focusobj, IMgaFCOs* selectedobjs, long param, HRESULT& hr) {
	__try {
		__try {
			hr = compex->InvokeEx(project, focusobj, selectedobjs, param);
		} __except(crExceptionFilter(GetExceptionCode(), GetExceptionInformation())) {
			return 0;
		}
		// If run outside of GME, CrashRpt will not be set up, and crExceptionFilter will return EXCEPTION_CONTINUE_SEARCH
	} __except(nopExceptionFilter(GetExceptionCode(), GetExceptionInformation())) {
		return 0;
	}
	return 1;
}

STDMETHODIMP CMgaLauncher::RunComponent(BSTR progid, IMgaProject *project, IMgaFCO *focusobj, IMgaFCOs *selectedobjs, long param)
{
	if( project == NULL )
		COMRETURN(E_POINTER);

	COMTRY
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CComObjPtr<IMgaRegistrar> registrar;
		COMTHROW( registrar.CoCreateInstance(OLESTR("MGA.MgaRegistrar")) );
	
		CComBSTR prgid = progid;
		if (prgid.Length() == 0) {
			CComObjPtr<IMgaMetaProject> paradigm;
			COMTHROW( project->get_RootMeta(PutOut(paradigm)) );
			ASSERT( paradigm != NULL );
	
			CComBstrObj parname;
			COMTHROW( paradigm->get_Name(PutOut(parname)) );


			CComVariant v;
			COMTHROW( registrar->get_AssociatedComponents(parname, 
			COMPONENTTYPE_INTERPRETER, REGACCESS_BOTH, PutOut(v)) );

			CStringArray comps;
			CopyTo(v, comps);

			if( comps.GetSize() == 0 )
			{
				if( AfxMessageBox(_T("There are no registered interpreters for this paradigm. ")
					_T("Do you want to install your interpreter now?"), MB_YESNO) != IDYES )
					COMRETURN(S_FALSE);
			}
		
			if( comps.GetSize() == 1 )
				prgid = comps[0];
			else
			{
				CCompDlg dlg;
				dlg.onOKoper = _T("Interpret...");

				dlg.paradigm = parname;
				dlg.type = COMPONENTTYPE_INTERPRETER;

				if( dlg.DoModal() != IDOK )
					COMRETURN(S_OK);

				prgid = dlg.progid;
			}

			if (prgid.Length() == 0)
				COMRETURN(S_FALSE);
		}


		componenttype_enum type;
		if (FAILED(registrar->QueryComponent(prgid, &type, NULL, REGACCESS_PRIORITY)))
			type = COMPONENTTYPE_INTERPRETER;
		CComPtr<IMgaComponent> component;
		// Before DispatchProxy: CComObjPtr<IMgaComponent> component;
		CString compname = prgid;
		if(type & COMPONENTTYPE_SCRIPT) {
			CComBSTR engine, scriptfile;
			COMTHROW(registrar->get_ComponentExtraInfo(REGACCESS_PRIORITY, prgid, CComBSTR(L"ExecEngine"), &engine));
			COMTHROW(registrar->get_ComponentExtraInfo(REGACCESS_PRIORITY, prgid, CComBSTR(L"ScriptFile"), &scriptfile));
			if(!engine || !scriptfile) {
				ThrowCOMError(E_FAIL, _T("Incomplete registration for script component ") + compname);
			}
			compname += _T(" (") + CString(engine) + _T(")");
			COMTHROW( component.CoCreateInstance(engine) );
			CComQIPtr<IMgaComponentEx> compex = component;
			if(!compex) {
				ThrowCOMError(E_MGA_NOT_SUPPORTED, L"Exec.engine is only supported with extended component interface");
			}
			if(scriptfile) {
				compex->ComponentParameter[_bstr_t(L"script")] = CComVariant(scriptfile);
			}
		}
		else
		{
			HRESULT hr = CreateMgaComponent(component, prgid);
			if (FAILED(hr))
				return hr;
		}


		if(component == NULL) {
			ThrowCOMError(E_FAIL, L"Could not start component '" + compname + "'");
		}
		else {
			IGMEVersionInfoPtr vi = (IMgaComponent*) component;
			if (vi)
			{
				GMEInterfaceVersion vv = vi->version;
				if(vv != INTERFACE_VERSION) {
					CString aa;
					aa.Format(_T("The interface version number of this component (%d.%d) differs from the GME version (%d.%d)\n")
								_T("This will probably result in serious malfunctions\n")
								_T("Do you want to proceed anyway?"), vv/0x10000,vv%0x10000, INTERFACE_VERSION/0x10000, INTERFACE_VERSION%0x10000); 
					if(AfxMessageBox(aa, MB_YESNO) !=IDYES) return S_OK;
				}
			}
			if(parameter.vt != VT_BOOL || parameter.boolVal != VARIANT_TRUE) { 
				CComQIPtr<IMgaComponentEx> compex = component;
				if (compex) {
					CComQIPtr<ISupportErrorInfo> supportErrorInfo = component;
					HRESULT hr = component->Initialize(project);
					// Need to catch SEH exceptions (especially for Win7 x64: see GME-318)
					if (SUCCEEDED(hr) && !InvokeExWithCrashRpt(compex, project, focusobj, selectedobjs, param, hr)) {
						if (project->ProjectStatus & 8)
							project->AbortTransaction();
						ThrowCOMError(E_POINTER, _T("An error has occurred in component ") + compname + _T(".\n")
							_T("GME may not be in a stable state.\n")
							_T("Please save your work and restart GME."));
					} else {
						if (!SUCCEEDED(hr)) {
							_bstr_t desc;
							if (supportErrorInfo && GetErrorInfo(desc.GetAddress())) {
								CString msg = static_cast<const TCHAR*>(desc);
								msg = "Interpreter returned error: " + msg;
								ThrowCOMError(hr, msg);
							} else {
								ThrowCOMError(hr, _T("Interpreter returned error"));
							}
						}
						// Sometimes interpreters forget to close transactions, even when returning S_OK
						if (project->ProjectStatus & 8)
							project->AbortTransaction();
					}
				} else {
					try	{
						COMTHROW(component->Initialize(project));
						CComPtr<IMgaTerritory> terr;
						COMTHROW(project->CreateTerritory(NULL, &terr, 0));
						COMTHROW(project->BeginTransaction(terr, TRANSACTION_GENERAL));
						try	{
							COMTHROW( component->Invoke(project, selectedobjs, param) );
							COMTHROW(project->CommitTransaction());
						}
						catch(...)
						{
							project->AbortTransaction();
							throw;
						}
					}
					catch(hresult_exception &e)	{
						project->AbortTransaction();
						ThrowCOMError(e.hr, _T("Interpreter returned error"));
					}
					catch(...)
					{
						project->AbortTransaction();
						AfxMessageBox(_T("An error has occurred in component ") + compname + _T(".\n")
							_T("GME may not be in a stable state.\n")
							_T("Please save your work and restart GME."));
					} 
				}
			}
			else {		// running unprotected
				try	{
					COMTHROW(component->Initialize(project));
					IMgaComponentExPtr compex = (IMgaComponent*)component;
					if(compex) {
						compex->__InvokeEx(project, focusobj, CComQIPtr<IMgaFCOs>(selectedobjs), param);
					}
					else {
						CComPtr<IMgaTerritory> terr;
						COMTHROW(project->CreateTerritory(NULL, &terr, 0));
						COMTHROW(project->BeginTransaction(terr, TRANSACTION_GENERAL));
						try	{		
							COMTHROW( component->Invoke(project, selectedobjs, param) );
							COMTHROW(project->CommitTransaction());
						}
						catch(...)
						{
							project->AbortTransaction();
							throw;
						}
					}
				}
				catch(hresult_exception &e)	{
					DisplayError(_T("Interpreter returned error"), e.hr);
				}
				if (project->ProjectStatus & 8)
					project->AbortTransaction();
			}				
		}
		component.Release();
		CoFreeUnusedLibraries();
	} COMCATCH(CoFreeUnusedLibraries(););
}

// ------ Helper functions

CString CMgaLauncher::PruneConnectionString(const CString &conn)
{
	CString ret;

	int p = 0;
	while( p < conn.GetLength() )
	{
		int q = conn.Find(';', p);
		if( q < 0 )
			q = conn.GetLength();

		CString part((const TCHAR*)conn + p, q-p);

		int r = part.Find('=');
		if( r < 0 )
			r = part.GetLength();

		CString key((const TCHAR*)part, r);

		if( key == _T("UID") ||
			key == _T("PWD") ||
			key == _T("USER") ||
			key == _T("PASSWORD") )
		{
			if( !ret.IsEmpty() )
				ret += _T(";");

			ret += part;
		}

		p = q+1;
	}

	return ret;
}


STDMETHODIMP CMgaLauncher::RegistryBrowser(IMgaObject *obj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COMTRY
	{
		
		CRegistryBrowserDlg dlg(obj);
		if (dlg.DoModal() == IDOK) {
			dlg.UpdateRegistry();
		}
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaLauncher::AnnotationBrowser(IMgaObject *obj, IMgaRegNode *focus)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COMTRY
	{
		CComPtr<IMgaModel> model;
		if ( SUCCEEDED( obj->QueryInterface(&model) )) {
			CComPtr<IMgaRegNode> currAnn(focus);
			CAnnotationBrowserDlg dlg(model, currAnn);

			dlg.m_model = model;
			if (dlg.DoModal() == IDOK) {
				dlg.UpdateAnnotations();
			} else {
				return E_MGA_MUST_ABORT;	// JIRA GME-236: special ret code, indicating that the dialog was cancelled
			}
		}
		else {
			AfxMessageBox(_T("Unsupported object type for Annotation Browser."));
		}
	}
	COMCATCH(;)
}
