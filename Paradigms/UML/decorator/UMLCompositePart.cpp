//################################################################################################
//
// UML decorator composite part class
//	UMLCompositePart.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "UMLCompositePart.h"

#include "Resource.h"
#include "DecoratorStd.h"
#include "TriangleVectorPart.h"
#include "ConnectorVectorPart.h"
#include "ConstraintVectorPart.h"
#include "UMLClassPart.h"
#include "TypeableLabelPart.h"
#include "DecoratorExceptions.h"


namespace UMLDecor {

//################################################################################################
//
// CLASS : UMLCompositePart
//
//################################################################################################

UMLCompositePart::UMLCompositePart(DecoratorSDK::PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents> eventSink):
	ObjectAndTextPart(pPart, eventSink)
{
}

UMLCompositePart::~UMLCompositePart()
{
}

CRect UMLCompositePart::GetPortLocation(CComPtr<IMgaFCO>& fco) const
{
	throw PortNotFoundException();
}

CRect UMLCompositePart::GetLabelLocation(void) const
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
void UMLCompositePart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
									HWND parentWnd, DecoratorSDK::PreferenceMap& preferences)
{
	try {
		DecoratorSDK::getFacilities().getMetaFCO(pPart, m_spMetaFCO);
		if (pProject && pPart) {
			ASSERT(m_spMetaFCO);
			CComBSTR bstr;
			COMTHROW(m_spMetaFCO->get_Name(&bstr));
			CString name(bstr);
			if (name == UML_INHERITANCE_NAME || name == UML_INHERITANCE_PATTERN_NAME) {
				AddObjectPart(new DecoratorSDK::TriangleVectorPart(this, m_eventSink,
																   static_cast<long> (UML_INHERITANCE_WIDTH),
																   static_cast<long> (UML_INHERITANCE_HEIGHT)));
				if (!pFCO)
					AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else if (name == UML_CONNECTOR_NAME || name == UML_CONNECTOR_PATTERN_NAME) {
				AddObjectPart(new DecoratorSDK::ConnectorVectorPart(this, m_eventSink,
																	static_cast<long> (UML_CONNECTOR_WIDTH),
																	static_cast<long> (UML_CONNECTOR_HEIGHT)));
				if (!pFCO)
					AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else if (name == UML_CONSTRAINT_NAME) {
				AddObjectPart(new DecoratorSDK::ConstraintVectorPart(this, m_eventSink, DecoratorSDK::COLOR_RED,
																	 UML_CONSTRAINT_THICKNESS,
																	 static_cast<long> (UML_CONSTRAINT_WIDTH),
																	 static_cast<long> (UML_CONSTRAINT_HEIGHT)));
				AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else if (name == UML_CONSTRAINT_DEFINITION_NAME) {
				AddObjectPart(new DecoratorSDK::ConstraintVectorPart(this, m_eventSink, DecoratorSDK::COLOR_BLUE,
																	 UML_CONSTRAINT_THICKNESS,
																	 static_cast<long> (UML_CONSTRAINT_WIDTH),
																	 static_cast<long> (UML_CONSTRAINT_HEIGHT)));
				AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else {	// This must be a class
				AddObjectPart(new UMLClassPart(this, m_eventSink));
				// The UMLClassPart handles the label also
			}
		}
	}
	catch (hresult_exception& e) {
		throw DecoratorException((DecoratorExceptionCode)e.hr);
	}

	CompositePart::InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);
}

void UMLCompositePart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
									HWND parentWnd)
{
	DecoratorSDK::getFacilities().loadPathes(pProject, true);

	DecoratorSDK::PreferenceMap preferencesMap;
	InitializeEx(pProject, pPart, pFCO, parentWnd, preferencesMap);
}

}; // namespace UMLDecor
