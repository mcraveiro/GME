// ComponentProxy.h : Declaration of the CComponentProxy

#ifndef __COMPONENTPROXY_H_
#define __COMPONENTPROXY_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CComponentProxy
class ATL_NO_VTABLE CComponentProxy : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CComponentProxy, &__uuidof(ComponentProxy)>,
	public IMgaComponentProxy,
	public IMgaComponentEx,
	public IGMEVersionInfo
{
public:
	CComponentProxy()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_COMPONENTPROXY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CComponentProxy)
	COM_INTERFACE_ENTRY(IMgaComponentProxy)
	COM_INTERFACE_ENTRY(IMgaComponent)
	COM_INTERFACE_ENTRY(IMgaComponentEx)
	COM_INTERFACE_ENTRY(IGMEVersionInfo)
END_COM_MAP()


public:

	// IMgaComponentProxy
	STDMETHOD(put_DispatchComponent)(/*[in]*/ IDispatch *component);
	
	// IMgaComponent
	STDMETHOD(Invoke)(/*[in]*/ IMgaProject *project, /*[in]*/ IMgaFCOs *selectedobjs, /*[in]*/ long param);
	STDMETHOD(Initialize)(IMgaProject *p);
	STDMETHOD(Enable)(VARIANT_BOOL newVal);
	STDMETHOD(get_InteractiveMode)(/*[out, retval]*/ VARIANT_BOOL * enabled);
	STDMETHOD(put_InteractiveMode)(/*[in]*/ VARIANT_BOOL enabled);
	STDMETHOD(get_ComponentName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ComponentType)(/*[out, retval]*/ componenttype_enum *t);
	STDMETHOD(get_Paradigm)(/*[out, retval]*/ BSTR *pVal);
	
	// IMgaComponentEx
	STDMETHOD(InvokeEx)(/*[in]*/ IMgaProject *project, /*[in]*/ IMgaFCO *currentobj, /*[in,out]*/ IMgaFCOs *selectedobjs, /*[in]*/ long param);
	STDMETHOD(ObjectsInvokeEx)(/*[in]*/ IMgaProject *project, /*[in]*/ IMgaObject *currentobj, /*[in,out]*/ IMgaObjects *selectedobjs, /*[in]*/ long param);
	STDMETHOD(get_ComponentProgID)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ComponentParameter)(/*[in]*/ BSTR name, /*[in]*/ VARIANT newVal);
	STDMETHOD(get_ComponentParameter)(/*[in]*/ BSTR name, /*[out, retval]*/ VARIANT *pVal);
	
	// IGMEVersionInfo
	STDMETHOD(get_version)(/*[out, retval]*/ GMEInterfaceVersion_enum *pVal);

protected:
	CComDispatchDriver m_dispatch;
};

#endif //__COMPONENTPROXY_H_
