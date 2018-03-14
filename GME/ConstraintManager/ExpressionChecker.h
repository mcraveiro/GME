// ExpressionChecker.h : Declaration of the CExpressionChecker

#ifndef __EXPRESSIONCHECKER_H_
#define __EXPRESSIONCHECKER_H_

#include "resource.h"       // main symbols

#include "commonsmart.h"
#include "commonMFC.h"
#include "commonerror.h"
#include "CommonVersionInfo.h"
#include "OCLGMEECFacade.h"

#define EXP_COMPONENT_PROGID "Mga.AddOn.ExpressionChecker"
#define EXP_COMPONENT_NAME "ExpressionChecker"
#define EXP_CETYPE ((componenttype_enum)(COMPONENTTYPE_ADDON))
#define EXP_PARADIGM "MetaGME"

/////////////////////////////////////////////////////////////////////////////
// CExpressionChecker
class CExpressionChecker;

class ATL_NO_VTABLE CExpEventSink :
public CComObjectRootEx<CComSingleThreadModel>, public IMgaEventSink {
public:
	CExpressionChecker *ec;
BEGIN_COM_MAP(CExpEventSink)
	COM_INTERFACE_ENTRY(IMgaEventSink)
END_COM_MAP()
	STDMETHOD(GlobalEvent) (globalevent_enum event);
	STDMETHOD(ObjectEvent) (IMgaObject * obj, unsigned long eventmask, VARIANT v);
};


class ATL_NO_VTABLE CExpressionChecker :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CExpressionChecker, &CLSID_ExpressionChecker>,
	public ISupportErrorInfo,
	public IDispatchImpl<IMgaComponentEx, &IID_IMgaComponentEx, &LIBID_CONSTRAINTMANAGERLib>,
	public IGMEVersionInfoImpl
{
public:
	CExpressionChecker()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_EXPRESSIONCHECKER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CExpressionChecker)
	COM_INTERFACE_ENTRY(IMgaComponentEx)
	COM_INTERFACE_ENTRY_IID(IID_IMgaComponent, IMgaComponentEx)
	COM_INTERFACE_ENTRY_IID(IID_IDispatch, IMgaComponentEx)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_IID(IID_IGMEVersionInfo, IGMEVersionInfoImpl)
END_COM_MAP()

// IMgaComponentEx
public:
	STDMETHOD(Invoke)(IMgaProject *project, IMgaFCOs *selectedobjs, long param);
	STDMETHOD(InvokeEx)(IMgaProject *project, IMgaFCO *currentobj, IMgaFCOs *selectedobjs, long param);
	STDMETHOD(ObjectsInvokeEx)(IMgaProject *project, IMgaObject *currentobj, IMgaObjects *selectedobjs, long param);

	STDMETHOD(Initialize)(IMgaProject *p);
	STDMETHOD(Enable)(VARIANT_BOOL enabled);
	STDMETHOD(get_InteractiveMode)(VARIANT_BOOL * enabled);
	STDMETHOD(put_InteractiveMode)(VARIANT_BOOL enabled);
	STDMETHOD(get_ComponentName)(BSTR *rVal);
	STDMETHOD(get_ComponentType)(componenttype_enum *t);
	STDMETHOD(get_Paradigm)(BSTR *rVal);
	STDMETHOD(get_ComponentProgID)(BSTR *rVal);
	STDMETHOD(get_ComponentParameter)(BSTR name, VARIANT *pVal);
	STDMETHOD(put_ComponentParameter)(BSTR name, VARIANT newVal);


	STDMETHOD(GlobalEvent) (globalevent_enum event);
	STDMETHOD(ObjectEvent) (IMgaObject * obj, unsigned long eventmask, VARIANT v);

	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid) {
		ASSERT( InlineIsEqualGUID( riid,IID_IMgaComponentEx ) || InlineIsEqualGUID( riid,IID_IMgaComponent ) );
		return S_OK;
	};

private:
	CComObjPtr<IMgaTerritory> 	m_spTerritory;
	CComObjPtr<CExpEventSink> 	m_spEventSink;
	CComObjPtr<IMgaProject> 		m_spProject;
	CComPtr<IMgaAddOn> 			m_spAddOn;

	OclGmeEC::Facade					m_Facade;
};

#endif //__EXPRESSIONCHECKER_H_
