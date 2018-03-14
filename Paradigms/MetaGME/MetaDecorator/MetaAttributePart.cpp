//################################################################################################
//
// Meta attribute part class (decorator part)
//	MetaAttributePart.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "MetaAttributePart.h"

#include "MetaClassPart.h"


namespace MetaDecor {

//################################################################################################
//
// CLASS : MetaAttributePart
//
//################################################################################################

MetaAttributePart::MetaAttributePart(PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink,
									 const CString& nameStr, const CString& typeStr, const CPoint& boxPos,
									 CComPtr<IMgaFCO>& pFCO):
	AttributePart	(pPart, eventSink),
	m_boxPos		(boxPos),
	m_spActualFCO	(pFCO)
{
	SetName(nameStr);
	SetType(typeStr);
}

MetaAttributePart::~MetaAttributePart()
{
}

void MetaAttributePart::ExecuteOperation(const CString& newString)
{
	// transaction operation begin
	CComBSTR bstr;
	CopyTo(newString, bstr);
	HRESULT hr = m_spActualFCO->put_Name(bstr);
	if (hr != E_MGA_NAME_DUPLICATE)
		COMTHROW(hr);
	m_strText = newString;
	// transaction operation end
}

bool MetaAttributePart::IsLesser(const AttributePart* other)
{
	const MetaAttributePart* otherMa = static_cast<const MetaAttributePart*> (other);
	CPoint boxPos = otherMa->GetBoxPos();
	return (m_boxPos.y < boxPos.y || m_boxPos.y == boxPos.y && m_boxPos.x < boxPos.x);
}

void MetaAttributePart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO, HWND parentWnd, DecoratorSDK::PreferenceMap& preferences)
{
	__super::InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);
	VARIANT_BOOL isLibObject = VARIANT_TRUE;
	m_spActualFCO->get_IsLibObject(&isLibObject);
	VARIANT_BOOL isReadOnly = VARIANT_TRUE;
	m_spActualFCO->HasReadOnlyAccess(&isReadOnly);
	m_bTextEditable = isLibObject == VARIANT_FALSE && isReadOnly == VARIANT_FALSE;
}

}; // namespace MetaDecor
