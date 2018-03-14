//################################################################################################
//
// Annotator rectangle part decorator class
//	AnnotatorRectanglePart.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "AnnotationDefs.h"
#include "AnnotatorRectanglePart.h"
#include "DecoratorExceptions.h"


namespace AnnotatorDecor {

//################################################################################################
//
// CLASS : AnnotatorRectanglePart
//
//################################################################################################

AnnotatorRectanglePart::AnnotatorRectanglePart(PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink):
	VectorPart(pPart, eventSink),
	m_regRoot				(NULL),
	m_crBgColor				(AN_DEFAULT_BGCOLOR),
	m_bRoundCornerRect		(false),
	m_iRoundCornerRadius	(9)
{
	m_crShadow					= AN_DEFAULT_SHADOWCOLOR;
	m_crGradient				= AN_DEFAULT_GRADIENTCOLOR;

	brushColorVariableName	= DecoratorSDK::PREF_FILLCOLOR;
}

AnnotatorRectanglePart::~AnnotatorRectanglePart()
{
}

void AnnotatorRectanglePart::SetParam(const CString& strName, VARIANT vValue)
{
	if (!strName.CompareNoCase(AN_PARAM_ROOTNODE)) {
		CComVariant	variantval(vValue);
		try {
			if (!m_regRoot) {
				COMTHROW(variantval.ChangeType(VT_UNKNOWN));
				CComPtr<IUnknown> unk(variantval.punkVal);
				COMTHROW(unk.QueryInterface(&m_regRoot));
			}
			ReadPreferences();
		}
		catch (hresult_exception&) {
			throw DecoratorException((DecoratorExceptionCode)E_DECORATOR_UNKNOWN_PARAMETER);
		}
	} else {
		throw DecoratorException((DecoratorExceptionCode)E_DECORATOR_UNKNOWN_PARAMETER);
	}
}

bool AnnotatorRectanglePart::GetParam(const CString& strName, VARIANT* pvValue)
{
	if (!strName.CompareNoCase(AN_PARAM_ROOTNODE)) {
		CComVariant	variantval;
		variantval.Attach(pvValue);
		variantval = m_regRoot;
	} else {
		throw DecoratorException((DecoratorExceptionCode)E_DECORATOR_UNKNOWN_PARAMETER);
	}
	return true;
}

void AnnotatorRectanglePart::Draw(CDC* pDC, Gdiplus::Graphics* gdip)
{
	VectorPart::Draw(pDC, gdip);
}

void AnnotatorRectanglePart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
										  HWND parentWnd, DecoratorSDK::PreferenceMap& preferences)
{
	preferences[DecoratorSDK::PREF_FILLCOLOR]			= DecoratorSDK::PreferenceVariant(m_crBgColor);
	preferences[DecoratorSDK::PREF_SHADOWCOLOR]			= DecoratorSDK::PreferenceVariant(m_crShadow);
	preferences[DecoratorSDK::PREF_GRADIENTCOLOR]		= DecoratorSDK::PreferenceVariant(m_crGradient);
	preferences[DecoratorSDK::PREF_ITEMGRADIENTFILL]	= DecoratorSDK::PreferenceVariant(m_bGradientFill);
	preferences[DecoratorSDK::PREF_GRADIENTDIRECTION]	= DecoratorSDK::PreferenceVariant(m_iGradientDirection);
	preferences[DecoratorSDK::PREF_ITEMSHADOWCAST]		= DecoratorSDK::PreferenceVariant(m_bCastShadow);
	preferences[DecoratorSDK::PREF_SHADOWTHICKNESS]		= DecoratorSDK::PreferenceVariant(m_iShadowDepth);
	preferences[DecoratorSDK::PREF_SHADOWDIRECTION]		= DecoratorSDK::PreferenceVariant(m_iShadowDirection);
	preferences[DecoratorSDK::PREF_ROUNDCORNERRECT]		= DecoratorSDK::PreferenceVariant(m_bRoundCornerRect);
	preferences[DecoratorSDK::PREF_ROUNDCORNERRADIUS]	= DecoratorSDK::PreferenceVariant(m_iRoundCornerRadius);

	VectorPart::InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);

	DecoratorSDK::SimpleCoordCommand* leftMost			= new DecoratorSDK::SimpleCoordCommand(DecoratorSDK::LeftMost);
	DecoratorSDK::SimpleCoordCommand* topMost			= new DecoratorSDK::SimpleCoordCommand(DecoratorSDK::TopMost);
	DecoratorSDK::SimpleCoordCommand* rightMost			= new DecoratorSDK::SimpleCoordCommand(DecoratorSDK::RightMost);
	DecoratorSDK::SimpleCoordCommand* bottomMost		= new DecoratorSDK::SimpleCoordCommand(DecoratorSDK::BottomMost);
	DecoratorSDK::AbsoluteCoordCommand* radiusCommand	= new DecoratorSDK::AbsoluteCoordCommand(m_bRoundCornerRect ? m_iRoundCornerRadius : 0);
	m_coordCommands.push_back(leftMost);
	m_coordCommands.push_back(topMost);
	m_coordCommands.push_back(rightMost);
	m_coordCommands.push_back(bottomMost);
	m_coordCommands.push_back(radiusCommand);

	AddCommand(DecoratorSDK::VectorCommand(DecoratorSDK::VectorCommand::BeginPath));
	std::vector<const DecoratorSDK::CoordCommand*> m_roundRectangleParams;
	m_roundRectangleParams.push_back(leftMost);
	m_roundRectangleParams.push_back(topMost);
	m_roundRectangleParams.push_back(rightMost);
	m_roundRectangleParams.push_back(bottomMost);
	m_roundRectangleParams.push_back(radiusCommand);
	AddCommand(DecoratorSDK::VectorCommand(m_roundRectangleParams, DecoratorSDK::VectorCommand::AddRoundRectangleToPath));
	AddCommand(DecoratorSDK::VectorCommand(DecoratorSDK::VectorCommand::EndPath));
	AddCommand(DecoratorSDK::VectorCommand(DecoratorSDK::VectorCommand::CopyShadowPath));
	AddCommand(DecoratorSDK::VectorCommand(DecoratorSDK::VectorCommand::CastShadowPath));
	DecoratorSDK::AbsoluteCoordCommand* colorCmd = new DecoratorSDK::AbsoluteCoordCommand(m_crBgColor);
	DecoratorSDK::AbsoluteCoordCommand* grayedCmd = new DecoratorSDK::AbsoluteCoordCommand(AN_DEFAULT_GRAYEDOUTCOLOR);
	m_coordCommands.push_back(colorCmd);
	m_coordCommands.push_back(grayedCmd);
	AddCommand(DecoratorSDK::VectorCommand(colorCmd, grayedCmd, DecoratorSDK::VectorCommand::FillPath));
}

void AnnotatorRectanglePart::ReadPreferences(void)
{
	try {
		CComBSTR bstr;
		CComPtr<IMgaRegNode> bgcolNode;
		CComBSTR bgcolName(AN_BGCOLOR_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(bgcolName, &bgcolNode));
		if (bgcolNode != NULL) {
			COMTHROW(bgcolNode->get_Value(&bstr));
		}
		CString strVal(bstr);
		unsigned int val;
		if (_stscanf(strVal,_T("%x"),&val) == 1) {
			unsigned int r = (val & 0xff0000) >> 16;
			unsigned int g = (val & 0xff00) >> 8;
			unsigned int b = val & 0xff;
			m_crBgColor = RGB(r,g,b);
		} else {
			m_crBgColor = AN_DEFAULT_BGCOLOR;
		}
	}
	catch (hresult_exception&) {
		m_crBgColor = AN_DEFAULT_BGCOLOR;
	}

	try {
		CComBSTR bstr;
		CComPtr<IMgaRegNode> shadowcolNode;
		CComBSTR shadowcolName(AN_SHADOWCOLOR_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(shadowcolName, &shadowcolNode));
		if (shadowcolNode != NULL) {
			COMTHROW(shadowcolNode->get_Value(&bstr));
		}
		CString strVal(bstr);
		unsigned int val;
		if (_stscanf(strVal,_T("%x"),&val) == 1) {
			unsigned int r = (val & 0xff0000) >> 16;
			unsigned int g = (val & 0xff00) >> 8;
			unsigned int b = val & 0xff;
			m_crShadow = RGB(r,g,b);
		} else {
			m_crShadow = AN_DEFAULT_SHADOWCOLOR;
		}
	}
	catch (hresult_exception&) {
		m_crShadow = AN_DEFAULT_SHADOWCOLOR;
	}

	try {
		CComBSTR bstr;
		CComPtr<IMgaRegNode> gradientcolNode;
		CComBSTR gradientcolName(AN_GRADIENTCOLOR_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(gradientcolName, &gradientcolNode));
		if (gradientcolNode != NULL) {
			COMTHROW(gradientcolNode->get_Value(&bstr));
		}
		CString strVal(bstr);
		unsigned int val;
		if (_stscanf(strVal,_T("%x"),&val) == 1) {
			unsigned int r = (val & 0xff0000) >> 16;
			unsigned int g = (val & 0xff00) >> 8;
			unsigned int b = val & 0xff;
			m_crGradient = RGB(r,g,b);
		} else {
			m_crGradient = AN_DEFAULT_GRADIENTCOLOR;
		}
	}
	catch (hresult_exception&) {
		m_crGradient = AN_DEFAULT_GRADIENTCOLOR;
	}

	try {
		m_bGradientFill = AN_DEFAULT_GRADIENTFILL;
		CComBSTR bstr;
		CComPtr<IMgaRegNode> lfNode;
		CComBSTR lfName(AN_GRADIENTFILL_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(lfName, &lfNode));
		if (lfNode != NULL) {
			COMTHROW(lfNode->get_Value(&bstr));
			if (bstr == "1")
				m_bGradientFill = true;
			else
				m_bGradientFill = false;
		}
	}
	catch (hresult_exception &) {
		m_bGradientFill = AN_DEFAULT_GRADIENTFILL;
	}

	// 'GradientDirection'
	try {
		m_iGradientDirection = AN_DEFAULT_GRADIENTDIRECTION;
		CComBSTR bstr;
		CComPtr<IMgaRegNode> lfNode;
		CComBSTR lfName(AN_GRADIENTDIRECTION_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(lfName, &lfNode));
		if (lfNode != NULL) {
			COMTHROW(lfNode->get_Value(&bstr));
			CString strVal(bstr);
			if (_stscanf(strVal,_T("%ld"),&m_iGradientDirection) != 1) {
				m_iGradientDirection = AN_DEFAULT_GRADIENTDIRECTION;
			}
		}
	}
	catch (hresult_exception &) {
		m_iGradientDirection = AN_DEFAULT_GRADIENTDIRECTION;
	}

	// 'CastShadow'
	try {
		m_bCastShadow = AN_DEFAULT_CASTSHADOW;
		CComBSTR bstr;
		CComPtr<IMgaRegNode> lfNode;
		CComBSTR lfName(AN_CASTSHADOW_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(lfName, &lfNode));
		if (lfNode != NULL) {
			COMTHROW(lfNode->get_Value(&bstr));
			if (bstr == "1")
				m_bCastShadow = true;
			else
				m_bCastShadow = false;
		}
	}
	catch (hresult_exception &) {
		m_bCastShadow = AN_DEFAULT_CASTSHADOW;
	}

	// 'ShadowDepth'
	try {
		m_iShadowDepth = AN_DEFAULT_SHADOWDEPTH;
		CComBSTR bstr;
		CComPtr<IMgaRegNode> lfNode;
		CComBSTR lfName(AN_SHADOWDEPTH_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(lfName, &lfNode));
		if (lfNode != NULL) {
			COMTHROW(lfNode->get_Value(&bstr));
			CString strVal(bstr);
			if (_stscanf(strVal,_T("%ld"),&m_iShadowDepth) != 1) {
				m_iShadowDepth = AN_DEFAULT_SHADOWDEPTH;
			}
		}
	}
	catch (hresult_exception &) {
		m_iShadowDepth = AN_DEFAULT_SHADOWDEPTH;
	}

	// 'ShadowDirection'
	try {
		m_iShadowDirection = AN_DEFAULT_SHADOWDIRECTION;
		CComBSTR bstr;
		CComPtr<IMgaRegNode> lfNode;
		CComBSTR lfName(AN_SHADOWDIRECTION_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(lfName, &lfNode));
		if (lfNode != NULL) {
			COMTHROW(lfNode->get_Value(&bstr));
			CString strVal(bstr);
			if (_stscanf(strVal,_T("%ld"),&m_iShadowDirection) != 1) {
				m_iShadowDirection = AN_DEFAULT_SHADOWDIRECTION;
			}
		}
	}
	catch (hresult_exception &) {
		m_iShadowDirection = AN_DEFAULT_SHADOWDIRECTION;
	}

	// 'RoundCornerRect'
	try {
		m_bRoundCornerRect = AN_DEFAULT_ROUNDCORNERRECT;
		CComBSTR bstr;
		CComPtr<IMgaRegNode> lfNode;
		CComBSTR lfName(AN_ROUNDCORNERRECT_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(lfName, &lfNode));
		if (lfNode != NULL) {
			COMTHROW(lfNode->get_Value(&bstr));
			if (bstr == "1")
				m_bRoundCornerRect = true;
			else
				m_bRoundCornerRect = false;
		}
	}
	catch (hresult_exception &) {
		m_bRoundCornerRect = AN_DEFAULT_ROUNDCORNERRECT;
	}

	// 'RoundCornerRadius'
	try {
		m_iRoundCornerRadius = AN_DEFAULT_ROUNDCORNERRADIUS;
		CComBSTR bstr;
		CComPtr<IMgaRegNode> lfNode;
		CComBSTR lfName(AN_ROUNDCORNERRADIUS_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(lfName, &lfNode));
		if (lfNode != NULL) {
			COMTHROW(lfNode->get_Value(&bstr));
			CString strVal(bstr);
			if (_stscanf(strVal,_T("%ld"),&m_iRoundCornerRadius) != 1) {
				m_iRoundCornerRadius = AN_DEFAULT_ROUNDCORNERRADIUS;
			}
		}
	}
	catch (hresult_exception &) {
		m_iRoundCornerRadius = AN_DEFAULT_ROUNDCORNERRADIUS;
	}
}

}; // namespace AnnotatorDecor
