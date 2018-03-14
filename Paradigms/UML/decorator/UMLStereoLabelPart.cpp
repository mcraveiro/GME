//################################################################################################
//
// UML Stereo label part class (decorator part)
//	UMLStereoLabelPart.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "UMLStereoLabelPart.h"
#include "DecoratorStd.h"


namespace UMLDecor {

//################################################################################################
//
// CLASS : UMLStereoLabelPart
//
//################################################################################################

UMLStereoLabelPart::UMLStereoLabelPart(PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents> eventSink, CComPtr<IMgaFCO>& pFCO, bool bTextEditable):
	DecoratorSDK::StereoLabelPart(pPart, eventSink),
	m_spActualFCO(pFCO)
{
	m_bTextEditable = bTextEditable;
}

UMLStereoLabelPart::~UMLStereoLabelPart()
{
}

void UMLStereoLabelPart::ExecuteOperation(const CString& newString)
{
	// transaction operation begin
	m_strText = newString;
	CComBSTR bstr;
	CopyTo(m_strText, bstr);
	DecoratorSDK::getFacilities().setAttribute(m_spActualFCO, UML_STEREOTYPE_ATTR, m_strText);
	// transaction operation end
}

}; // namespace UMLDecor
