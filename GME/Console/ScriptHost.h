// ScriptHost.h : Declaration of the CScriptHost

#ifndef __SCRIPTHOST_H_
#define __SCRIPTHOST_H_

#include "resource.h"       // main symbols
#include "ActivScp.h"
#include "gme.h"

#include <comdef.h>
#include "comdefsp.h"

_COM_SMARTPTR_TYPEDEF(IActiveScript, __uuidof(IActiveScript));
_COM_SMARTPTR_TYPEDEF(IActiveScriptParse, __uuidof(IActiveScriptParse));
_COM_SMARTPTR_TYPEDEF(IActiveScriptSite, __uuidof(IActiveScriptSite));
_COM_SMARTPTR_TYPEDEF(IActiveScriptError, __uuidof(IActiveScriptError));


class CConsoleCtrl;
/////////////////////////////////////////////////////////////////////////////
// CScriptHost
class ATL_NO_VTABLE CScriptHost : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScriptHost, &CLSID_ScriptHost>,
	public IDispatchImpl<IScriptHost, &IID_IScriptHost, &LIBID_CONSOLELib>,
	public IActiveScriptSite
{
public:
	CScriptHost()
		: m_console(0)
		, m_iscript(0)
		, m_iscriptParse(0)
		, m_enginePID("")
	{
	}
	virtual ~CScriptHost()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTHOST)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScriptHost)
	COM_INTERFACE_ENTRY(IScriptHost)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IActiveScriptSite)
END_COM_MAP()

// IScriptHost
public:
	STDMETHOD(ProcessString)(/*[in]*/ BSTR input);
	STDMETHOD(InitEngine)(/*[in]*/ IDispatch* console, /*[in]*/ BSTR engineProgid);

// IActiveScriptSite
 public:
	STDMETHOD(SetGMEApp)(/*[in]*/ IDispatch* gmeapp);
	STDMETHOD(SetGMEProj)(/*[in]*/ IDispatch* gmeproj);
	STDMETHOD(GetLCID)(/*[out]*/ LCID *plcid);
	STDMETHOD(GetItemInfo)(
                /*[in]*/  LPCOLESTR pstrName,
                /*[in]*/  DWORD     dwReturnMask,
                /*[out]*/ IUnknown  **ppiunkItem,
                /*[out]*/ ITypeInfo **ppti);
	STDMETHOD(GetDocVersionString)(/*[out]*/ BSTR      *pbstrVersion);
	STDMETHOD(OnScriptTerminate)(
                /*[in]*/  const VARIANT     *pvarResult,
                /*[in]*/  const EXCEPINFO   *pexcepinfo);
	STDMETHOD(OnStateChange)(
                /*[in]*/  SCRIPTSTATE       ssScriptState);
	STDMETHOD(OnScriptError)(
                /*[in]*/  IActiveScriptError *pscripterror);
	STDMETHOD(OnEnterScript)(void);
	STDMETHOD(OnLeaveScript)(void);

private:
	IDispatchPtr m_console;
	IActiveScriptPtr m_iscript;
	IActiveScriptParsePtr m_iscriptParse;
	_bstr_t m_enginePID;

	CComPtr<IGMEOLEApp>  m_gmeptr;
	CComPtr<IMgaProject> m_mgaproj;
	CComPtr<IGMEOLEIt>   m_actMod;

	void Message(BSTR message, msgtype_enum level);
};

#endif //__SCRIPTHOST_H_
