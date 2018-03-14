//################################################################################################
//
// New Annotator decorator composite part class
//	AnnotatorCompositePart.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "AnnotatorCompositePart.h"

#include "AnnotatorRectanglePart.h"
#include "AnnotatorTextPart.h"
#include "DecoratorExceptions.h"


namespace AnnotatorDecor {

//################################################################################################
//
// CLASS : AnnotatorCompositePart
//
//################################################################################################

AnnotatorCompositePart::AnnotatorCompositePart(DecoratorSDK::PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink):
	ObjectAndTextPart(pPart, eventSink)
{
}

AnnotatorCompositePart::~AnnotatorCompositePart()
{
}

void AnnotatorCompositePart::SetParam(const CString& strName, VARIANT vValue)
{
	m_strName = strName;
	m_vValue = vValue;
}

CSize AnnotatorCompositePart::GetPreferredSize(void) const
{
	HRESULT retVal = S_OK;
	std::vector<PartBase*>::const_iterator ii = m_compositeParts.begin();
	if (m_compositeParts.size() > 1)
		++ii;	// we expect that the second part is the label if there's more than one part
	if (ii != m_compositeParts.end()) {
		try {
			return (*ii)->GetPreferredSize();
		}
		catch(hresult_exception& e) {
			retVal = e.hr;
		}
		catch(DecoratorException& e) {
			retVal = e.GetHResult();
		}
	}

	throw DecoratorException((DecoratorExceptionCode)retVal);
}

CRect AnnotatorCompositePart::GetPortLocation(CComPtr<IMgaFCO>& fco) const
{
	throw PortNotFoundException();
}

CRect AnnotatorCompositePart::GetLabelLocation(void) const
{
	HRESULT retVal = S_OK;
	std::vector<PartBase*>::const_iterator ii = m_compositeParts.begin();
	if (m_compositeParts.size() > 1)
		++ii;	// we expect that the second part is the label if there's more than one part
	if (ii != m_compositeParts.end()) {
		try {
			return (*ii)->GetLabelLocation();
		}
		catch(hresult_exception& e) {
			retVal = e.hr;
		}
		catch(DecoratorException& e) {
			retVal = e.GetHResult();
		}
	}

	throw DecoratorException((DecoratorExceptionCode)retVal);
}

// New functions
void AnnotatorCompositePart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
										  HWND parentWnd, DecoratorSDK::PreferenceMap& preferences)
{
	try {
		if (pProject) {
			AddObjectPart(new AnnotatorRectanglePart(this, m_eventSink));
			AddTextPart(new AnnotatorTextPart(this, m_eventSink));
		}
	}
	catch (hresult_exception& e) {
		throw DecoratorException((DecoratorExceptionCode)e.hr);
	}

	PartBase::InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);
	DecoratorSDK::TextPart* txtPart = GetTextPart();
	PartBase* objPart = GetObjectPart();
	txtPart->SetParam(m_strName, m_vValue);
	objPart->SetParam(m_strName, m_vValue);
	txtPart->InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);
	objPart->InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);
}

void AnnotatorCompositePart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
										  HWND parentWnd)
{
	DecoratorSDK::getFacilities().loadPathes(pProject, true);

	DecoratorSDK::PreferenceMap preferencesMap;
	InitializeEx(pProject, pPart, pFCO, parentWnd, preferencesMap);
}

}; // namespace AnnotatorDecor
