#include "StdAfx.h"

#include "CoreDictionaryAttributeValue.h"

STDMETHODIMP CCoreDictionaryAttributeValue::put_Value(BSTR Key, BSTR Value)
{
COMTRY {
	m_dict[CComBSTR(Key)] = CComBSTR(Value);
} COMCATCH(;)
}
STDMETHODIMP CCoreDictionaryAttributeValue::get_Value(BSTR Key, BSTR* Value)
{
COMTRY {
	CComBSTR _key;
	_key.Attach(Key);
	auto ent = m_dict.find(_key);
	_key.Detach();
	if (ent == m_dict.end())
		return E_NOTFOUND;
	*Value = CComBSTR(ent->second).Detach();
} COMCATCH(;)
}

STDMETHODIMP CCoreDictionaryAttributeValue::get_Keys(VARIANT* Keys)
{
COMTRY {
	return E_NOTIMPL;
} COMCATCH(;)
}
