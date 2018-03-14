//################################################################################################
//
// Meta decorator composite part class
//	MetaCompositePart.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "MetaCompositePart.h"

#include "Resource.h"
#include "TypeableLabelPart.h"
#include "ConnectorVectorPart.h"
#include "InheritanceVectorPart.h"
#include "DiamondVectorPart.h"
#include "ConstraintVectorPart.h"
#include "ConstraintFunctionVectorPart.h"
#include "MetaClassPart.h"
#include "DecoratorExceptions.h"
#include "MetaDecoratorUtil.h"


namespace MetaDecor {

//################################################################################################
//
// CLASS : MetaCompositePart
//
//################################################################################################

MetaCompositePart::MetaCompositePart(DecoratorSDK::PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink):
	ObjectAndTextPart(pPart, eventSink)
{
}

MetaCompositePart::~MetaCompositePart()
{
}

CRect MetaCompositePart::GetPortLocation(CComPtr<IMgaFCO>& fco) const
{
	throw PortNotFoundException();
}

CRect MetaCompositePart::GetLabelLocation(void) const
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
void MetaCompositePart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
									 HWND parentWnd, DecoratorSDK::PreferenceMap& preferences)
{
	HRESULT retVal = S_OK;
	try {
		if (!DecoratorSDK::getFacilities().getMetaFCO(pPart, m_spMetaFCO))
			throw DecoratorException((DecoratorExceptionCode)E_DECORATOR_INIT_WITH_NULL);
		if (pProject && pPart) {
			m_spFCO = pFCO;		// pFCO == NULL, if we are in the PartBrowser

			CComBSTR bstr;
			COMTHROW(m_spMetaFCO->get_Name(&bstr));
			CString stereotypeName = bstr;

			// Get ShapeCode
			ShapeCode shape = MetaDecor::GetDecorUtils().GetShapeCode(stereotypeName);
			if (shape == NULLSHAPE) {
				throw DecoratorException((DecoratorExceptionCode)E_METADECORATOR_KINDNOTSUPPORTED);
			} else if (shape == CONNECTOR) {
				AddObjectPart(new DecoratorSDK::ConnectorVectorPart(this, m_eventSink,
																	static_cast<long> (2 * META_CONNECTOR_RAIDUS),
																	static_cast<long> (2 * META_CONNECTOR_RAIDUS)));
				if (!pFCO)
					AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else if (shape == CONSTRAINT) {
				AddObjectPart(new DecoratorSDK::ConstraintVectorPart(this, m_eventSink, DecoratorSDK::COLOR_RED,
																	 META_CONSTRAINT_THICKNESS,
																	 static_cast<long> (META_CONSTRAINT_WIDTH),
																	 static_cast<long> (META_CONSTRAINT_HEIGHT)));
				AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else if (shape == CONSTRAINTFUNC) {
				AddObjectPart(new DecoratorSDK::ConstraintFunctionVectorPart(this, m_eventSink, DecoratorSDK::COLOR_RED,
																			 META_CONSTRAINT_THICKNESS,
																			 static_cast<long> (META_CONSTRAINT_WIDTH),
																			 static_cast<long> (META_CONSTRAINT_HEIGHT)));
				AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else if (shape == INHERITANCE) {
				AddObjectPart(new DecoratorSDK::InheritanceVectorPart(this, m_eventSink,
																	  static_cast<long> (META_INHERITANCE_WIDTH),
																	  static_cast<long> (META_INHERITANCE_HEIGHT),
																	  DecoratorSDK::NormalInheritance));
				if (!pFCO)
					AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else if (shape == IMPINHERITANCE) {
				AddObjectPart(new DecoratorSDK::InheritanceVectorPart(this, m_eventSink,
																	  static_cast<long> (META_INHERITANCE_WIDTH),
																	  static_cast<long> (META_INHERITANCE_HEIGHT),
																	  DecoratorSDK::ImplementationInheritance));
				if (!pFCO)
					AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else if (shape == INTINHERITANCE) {
				AddObjectPart(new DecoratorSDK::InheritanceVectorPart(this, m_eventSink,
																	  static_cast<long> (META_INHERITANCE_WIDTH),
																	  static_cast<long> (META_INHERITANCE_HEIGHT),
																	  DecoratorSDK::InterfaceInheritance));
				if (!pFCO)
					AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else if (shape == EQUIVALENCE) {
				AddObjectPart(new DecoratorSDK::DiamondVectorPart(this, m_eventSink,
																  static_cast<long> (META_EQUIVALENCE_WIDTH),
																  static_cast<long> (META_EQUIVALENCE_HEIGHT)));
				if (!pFCO)
					AddTextPart(new DecoratorSDK::TypeableLabelPart(this, m_eventSink));
			} else {	// This must be a class
				COMTHROW(m_spMetaFCO->get_DisplayedName(&bstr));
				if (bstr.Length() != 0)
					stereotypeName = bstr;

				AddObjectPart(new MetaClassPart(this, m_eventSink, shape, stereotypeName));
				// The MetaClassPart handles the label also
			}
		}
	}
	catch (hresult_exception& e) {
		throw DecoratorException((DecoratorExceptionCode)e.hr);
	}

	CompositePart::InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);
}

void MetaCompositePart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
									HWND parentWnd)
{
	DecoratorSDK::getFacilities().loadPathes(pProject, true);

	DecoratorSDK::PreferenceMap preferencesMap;
	InitializeEx(pProject, pPart, pFCO, parentWnd, preferencesMap);
}

}; // namespace MetaDecor
