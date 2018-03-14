// ConstraintMgr.h : Declaration of the CConstraintMgr

#ifndef __CONSTRAINTMGR_H_
#define __CONSTRAINTMGR_H_

#include "resource.h"       // main symbols

#include "commonsmart.h"
#include "commonMFC.h"
#include "commonerror.h"
#include "CommonVersionInfo.h"
#include "OCLGMECMFacade.h"

#define MGR_COMPONENT_PROGID "Mga.AddOn.ConstraintManager"
#define MGR_COMPONENT_NAME "ConstraintManager"
#define MGR_CETYPE ((componenttype_enum)(COMPONENTTYPE_ADDON | COMPONENTTYPE_PARADIGM_INDEPENDENT))
#define MGR_PARADIGM "*"

/////////////////////////////////////////////////////////////////////////////
// CConstraintMgr

class CConstraintMgr;

class ATL_NO_VTABLE CMgrEventSink :
public CComObjectRootEx<CComSingleThreadModel>, public IMgaEventSink {
public:
	CConstraintMgr *cm;
BEGIN_COM_MAP(CMgrEventSink)
	COM_INTERFACE_ENTRY(IMgaEventSink)
END_COM_MAP()
	STDMETHOD(GlobalEvent) (globalevent_enum event);
	STDMETHOD(ObjectEvent) (IMgaObject * obj, unsigned long eventmask, VARIANT v);
};


class ATL_NO_VTABLE CConstraintMgr :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CConstraintMgr, &CLSID_ConstraintManager>,
	public IDispatchImpl<IMgaComponentEx, &IID_IMgaComponentEx, &LIBID_CONSTRAINTMANAGERLib>,
	public ISupportErrorInfo,
	public IGMEVersionInfoImpl
{
public:
	CConstraintMgr()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CONSTRAINTMGR)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CConstraintMgr)
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
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid) { ASSERT(InlineIsEqualGUID(riid,IID_IMgaComponentEx) ||
																InlineIsEqualGUID(riid,IID_IMgaComponent)	); return S_OK; };
private:

	void GotoViolatorObject(CComPtr<IUnknown> &gotoPunk);
	CComObjPtr<IMgaTerritory> 	m_spTerritory;
	CComObjPtr<CMgrEventSink> 	m_spEventSink;
	CComObjPtr<IMgaProject> 		m_spProject;
	CComPtr<IMgaAddOn> 			m_spAddOn;

	OclGmeCM::Facade				m_Facade;
};

#endif //__CONSTRAINTMGR_H_
