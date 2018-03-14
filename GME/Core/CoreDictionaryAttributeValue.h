#pragma once

#include <unordered_map>

struct CComBSTR_Length {
	::std::size_t operator ()(const CComBSTR& bstr) const {
		return bstr.Length();
	}
};

class ATL_NO_VTABLE CCoreDictionaryAttributeValue :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICoreDictionaryAttributeValue, &IID_ICoreDictionaryAttributeValue, &LIBID_MGACoreLib>
{
public:
	CCoreDictionaryAttributeValue() {}
	~CCoreDictionaryAttributeValue() {}

//DECLARE_REGISTRY_RESOURCEID(IDR_)
//DECLARE_PROTECT_FINAL_CONSTRUCT()
//	DECLARE_INTERFACE

BEGIN_COM_MAP(CCoreDictionaryAttributeValue)
	COM_INTERFACE_ENTRY(ICoreDictionaryAttributeValue)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

public:
	STDMETHOD(put_Value)(BSTR Key, BSTR Value);
	STDMETHOD(get_Value)(BSTR Key, BSTR* Value);
	STDMETHOD(get_Keys)(VARIANT* Keys);
	STDMETHOD(get_Map)(VARIANT* Map) {
		Map->vt = VT_I8;
		Map->llVal = (size_t)(void*)&m_dict;
		return S_OK;
	}

	STDMETHOD(put_Map)(VARIANT Map) {
		m_dict = std::move(*(map_type*)(void*)(size_t)Map.llVal);
		return S_OK;
	}

	STDMETHOD(Clone)(ICoreDictionaryAttributeValue** Clone)
	{
		CCoreDictionaryAttributeValue* out;
		typedef CComObject< CCoreDictionaryAttributeValue > COMTYPE;
		HRESULT hr = COMTYPE::CreateInstance((COMTYPE **)&out);
		if (SUCCEEDED(hr))
		{
			out->AddRef();
			out->m_dict = this->m_dict;
			*Clone = out;
		}
		return hr;
	}

	typedef std::unordered_map<CComBSTR, CComBSTR, CComBSTR_Length> map_type;
	map_type m_dict;
};

static void CopyTo(const VARIANT& from, CComPtr<CCoreDictionaryAttributeValue>& to)
{
	if (from.vt == VT_DISPATCH) {
		to = 0;
		CComPtr<ICoreDictionaryAttributeValue> fro;
		COMTHROW(from.pdispVal->QueryInterface(&fro));
		COMTHROW(fro->Clone((ICoreDictionaryAttributeValue**)&to));
	}
}
