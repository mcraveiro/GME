//################################################################################################
//
// Meta Stereo label part class (decorator part)
//	MetaStereoLabelPart.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "MetaStereoLabelPart.h"
#include "DecoratorStd.h"


namespace MetaDecor {

//################################################################################################
//
// CLASS : MetaStereoLabelPart
//
//################################################################################################

MetaStereoLabelPart::MetaStereoLabelPart(PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink):
	DecoratorSDK::StereoLabelPart(pPart, eventSink)
{
}

MetaStereoLabelPart::~MetaStereoLabelPart()
{
}

// New functions
void MetaStereoLabelPart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
									   HWND parentWnd, DecoratorSDK::PreferenceMap& preferences)
{
	DecoratorSDK::getFacilities().getMetaFCO(pPart, m_spMetaFCO);
	preferences[DecoratorSDK::PREF_ITEMEDITABLE] = DecoratorSDK::PreferenceVariant(false);
	StereoLabelPart::InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);
	preferences[DecoratorSDK::PREF_ITEMEDITABLE] = DecoratorSDK::PreferenceVariant(true);
}

void MetaStereoLabelPart::ExecuteOperation(const CString& newString)
{
	// transaction operation begin
/*	// Mustn't execute this editing, you can't just change stereotype name in case of Meta Paradigm
	// TODO: supress editing
	m_strText = newString;
	CComBSTR bstr;
	CopyTo(m_strText, bstr);
	COMTHROW(m_spMetaFCO->put_Name(bstr));*/
	// transaction operation end
}

}; // namespace MetaDecor
