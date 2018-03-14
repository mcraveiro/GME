// ComponentProxy.cpp : Implementation of CComponentProxy
#include "stdafx.h"
#include "MgaUtil.h"
#include "ComponentProxy.h"

#define  CHECK_INIT	{ if (m_dispatch == NULL) return E_MGAUTIL_COMPONENTPROXY_NOTINITIALIZED; }

/////////////////////////////////////////////////////////////////////////////
// CComponentProxy

// IMgaComponentProxy implementation
STDMETHODIMP CComponentProxy::put_DispatchComponent(/*[in]*/ IDispatch *component)
{
	m_dispatch = component;
	DISPID dispid;
	return m_dispatch.GetIDOfName(OLESTR("InvokeEx"), &dispid); // Just a simple check on the dispatch interface
	// return S_OK;
}

// IMgaComponent implementation
STDMETHODIMP CComponentProxy::Invoke(/*[in]*/ IMgaProject *project, /*[in]*/ IMgaFCOs *selectedobjs, /*[in]*/ long param)
{	
	CHECK_INIT;
	CComVariant varArgs[3];
	varArgs[2] = project;
	varArgs[1] = selectedobjs;
	varArgs[0] = param;
	return m_dispatch.InvokeN(OLESTR("Invoke"), &varArgs[0], 3, NULL);
}

STDMETHODIMP CComponentProxy::Initialize(IMgaProject *p)
{
	CHECK_INIT;
	CComVariant vp(p);
	return m_dispatch.Invoke1(OLESTR("Initialize"), &vp, NULL);
}

STDMETHODIMP CComponentProxy::Enable(VARIANT_BOOL newVal)
{
	CHECK_INIT;
	CComVariant vnewVal(newVal);
	return m_dispatch.Invoke1(OLESTR("Enable"), &vnewVal, NULL);
}

STDMETHODIMP CComponentProxy::get_InteractiveMode(/*[out, retval]*/ VARIANT_BOOL *enabled)
{
	CHECK_INIT;
	CComVariant venabled;
	venabled.Clear();
	HRESULT	hr = m_dispatch.GetPropertyByName(OLESTR("InteractiveMode"), &venabled);
	if (SUCCEEDED(hr)) {
		hr = venabled.ChangeType(VT_BOOL);
		if (SUCCEEDED(hr)) {
			*enabled = venabled.boolVal;
		}
	}
	return hr;
}

STDMETHODIMP CComponentProxy::put_InteractiveMode(/*[in]*/ VARIANT_BOOL enabled)
{
	CHECK_INIT;
	CComVariant venabled(enabled);
	return m_dispatch.PutPropertyByName(OLESTR("InteractiveMode"), &venabled);
}

STDMETHODIMP CComponentProxy::get_ComponentName(/*[out, retval]*/ BSTR *pVal)
{
	CHECK_INIT;
	CComVariant vval;
	vval.Clear();
	HRESULT	hr = m_dispatch.GetPropertyByName(OLESTR("ComponentName"), &vval);
	if (SUCCEEDED(hr)) {
		hr = vval.ChangeType(VT_BSTR);
		if (SUCCEEDED(hr)) {
			CComBSTR bstr(vval.bstrVal);
			*pVal = bstr.Detach();
		}
	}
	return hr;
}

STDMETHODIMP CComponentProxy::get_ComponentType(/*[out, retval]*/ componenttype_enum *t)
{
	CHECK_INIT;
	CComVariant vval;
	vval.Clear();
	HRESULT	hr = m_dispatch.GetPropertyByName(OLESTR("ComponentType"), &vval);
	if (SUCCEEDED(hr)) {
		hr = vval.ChangeType(VT_I4);
		if (SUCCEEDED(hr)) {
			*t = (componenttype_enum)vval.lVal;
		}
	}
	return hr;
}

STDMETHODIMP CComponentProxy::get_Paradigm(/*[out, retval]*/ BSTR *pVal)
{
	CHECK_INIT;
	CComVariant vval;
	vval.Clear();
	HRESULT	hr = m_dispatch.GetPropertyByName(OLESTR("Paradigm"), &vval);
	if (SUCCEEDED(hr)) {
		hr = vval.ChangeType(VT_BSTR);
		if (SUCCEEDED(hr)) {
			CComBSTR bstr(vval.bstrVal);
			*pVal = bstr.Detach();
		}
	}
	return hr;
}

// IMgaComponentEx implementation
STDMETHODIMP CComponentProxy::InvokeEx(/*[in]*/ IMgaProject *project, /*[in]*/ IMgaFCO *currentobj, /*[in,out]*/ IMgaFCOs *selectedobjs, /*[in]*/ long param)
{
	CHECK_INIT;
	CComVariant varArgs[4];
	varArgs[3] = project;
	varArgs[2] = currentobj;
	varArgs[1] = selectedobjs;
	varArgs[0] = param;
	return m_dispatch.InvokeN(OLESTR("InvokeEx"), &varArgs[0], 4, NULL);
}

STDMETHODIMP CComponentProxy::ObjectsInvokeEx(/*[in]*/ IMgaProject *project, /*[in]*/ IMgaObject *currentobj, /*[in,out]*/ IMgaObjects *selectedobjs, /*[in]*/ long param)
{
	CHECK_INIT;
	CComVariant varArgs[4];
	varArgs[3] = project;
	varArgs[2] = currentobj;
	varArgs[1] = selectedobjs;
	varArgs[0] = param;
	return m_dispatch.InvokeN(OLESTR("ObjectsInvokeEx"), &varArgs[0], 4, NULL);
}

STDMETHODIMP CComponentProxy::get_ComponentProgID(/*[out, retval]*/ BSTR *pVal)
{
	CHECK_INIT;
	CComVariant vval;
	vval.Clear();
	HRESULT	hr = m_dispatch.GetPropertyByName(OLESTR("ComponentProgID"), &vval);
	if (SUCCEEDED(hr)) {
		hr = vval.ChangeType(VT_BSTR);
		if (SUCCEEDED(hr)) {
			CComBSTR bstr(vval.bstrVal);
			*pVal = bstr.Detach();
		}
	}
	return hr;
}

STDMETHODIMP CComponentProxy::put_ComponentParameter(/*[in]*/ BSTR name, /*[in]*/ VARIANT newVal)
{
	CHECK_INIT;
	CComVariant vname(name);
	return m_dispatch.Invoke2(OLESTR("GetComponentParameter"), &vname, &newVal);
}

STDMETHODIMP CComponentProxy::get_ComponentParameter(/*[in]*/ BSTR name, /*[out, retval]*/ VARIANT *pVal)
{
	CHECK_INIT;
	::VariantInit(pVal);
	CComVariant vname(name);
	return m_dispatch.Invoke1(OLESTR("GetComponentParameter"), &vname, pVal);
}

// IGMEVersionInfo implementation
STDMETHODIMP CComponentProxy::get_version(/*[out, retval]*/ GMEInterfaceVersion_enum *pVal)
{
	CHECK_INIT;
	CComVariant vval;
	vval.Clear();
	HRESULT	hr = m_dispatch.GetPropertyByName(OLESTR("Version"), &vval);
	if (SUCCEEDED(hr)) {
		hr = vval.ChangeType(VT_I4);
		if (SUCCEEDED(hr)) {
			*pVal = (GMEInterfaceVersion_enum)vval.lVal;
			if (*pVal == GMEInterfaceVersion_None) {
				*pVal = GMEInterfaceVersion_Current;
			}
		}
	}
	return hr;
}

