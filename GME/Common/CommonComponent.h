#ifndef COMMONCOMPONENT_H
#define COMMONCOMPONENT_H

#include "CommonError.h"
#include "Comdef.h"

inline HRESULT CreateMgaComponent(CComPtr<IMgaComponent> &comp, LPCOLESTR ProgID)
{
	
	comp = NULL;

	CComPtr<IUnknown> unk;
	HRESULT hr = unk.CoCreateInstance(ProgID);
	if (SUCCEEDED(hr)) {
		hr = unk.QueryInterface(&comp);
		if (FAILED(hr)) {
			SetStandardOrGMEErrorInfo(hr);
			CComPtr<IDispatch> disp;
			hr = unk.QueryInterface(&disp);
			if (SUCCEEDED(hr)) {
				hr = comp.CoCreateInstance(OLESTR("MgaUtil.ComponentProxy"));
				if (SUCCEEDED(hr)) {
					CComPtr<IMgaComponentProxy> proxy;
					hr = comp.QueryInterface(&proxy);
					if (SUCCEEDED(hr)) {
						hr = proxy->put_DispatchComponent(disp);
					}
				}
			}
		}
	} else {
		_bstr_t error;
		GetErrorInfo(hr, error.GetAddress());
		if (static_cast<LPOLESTR>(error))
		{
			SetErrorInfo(static_cast<LPOLESTR>(error + L": '" + ProgID + L"'"));
		}
	}
	return hr;
}

inline HRESULT CreateMgaComponent(CComPtr<IMgaComponent> &comp, REFCLSID classID)
{
	
	comp = NULL;

	CComPtr<IUnknown> unk;
	HRESULT hr = unk.CoCreateInstance(classID);
	if (SUCCEEDED(hr)) {
		hr = unk.QueryInterface(&comp);
		if (FAILED(hr)) {
			CComPtr<IDispatch> disp;
			hr = unk.QueryInterface(&disp);
			if (SUCCEEDED(hr)) {
				hr = comp.CoCreateInstance(OLESTR("MgaUtil.ComponentProxy"));
				if (SUCCEEDED(hr)) {
					CComPtr<IMgaComponentProxy> proxy;
					hr = comp.QueryInterface(&proxy);
					if (SUCCEEDED(hr)) {
						hr = proxy->put_DispatchComponent(disp);
					}
				}
			}
		}
	}
	return hr;
}


#endif // COMMONCOMPONENT_H
