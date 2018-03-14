//################################################################################################
//
// Annotator text part class
//	AnnotatorTextPart.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "AnnotationDefs.h"
#include "AnnotatorTextPart.h"
#include "..\MgaUtil\AnnotationUtil.h"
#include "DecoratorExceptions.h"


namespace AnnotatorDecor {

//################################################################################################
//
// CLASS : AnnotatorTextPart
//
//################################################################################################

AnnotatorTextPart::AnnotatorTextPart(DecoratorSDK::PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink):
	TextPart		(pPart, eventSink),
	m_regRoot		(NULL)
{
	m_crText					= AN_DEFAULT_COLOR;
	memset(&m_logFont, 0, sizeof(LOGFONT));

}

AnnotatorTextPart::~AnnotatorTextPart()
{
}

void AnnotatorTextPart::SetParam(const CString& strName, VARIANT vValue)
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

bool AnnotatorTextPart::GetParam(const CString& strName, VARIANT* pvValue)
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

CSize AnnotatorTextPart::GetPreferredSize(void) const
{
	CDC	dc;
	dc.CreateCompatibleDC(NULL);
	Gdiplus::Font myFont(dc.m_hDC, &m_logFont);
	CSize size = DecoratorSDK::getFacilities().MeasureText(NULL, &myFont, m_strText);
	size.cx += 2 * AN_MARGIN;
	size.cy += 2 * AN_MARGIN;
	return size;
}

void AnnotatorTextPart::Draw(CDC* pDC, Gdiplus::Graphics* gdip)
{
	if (m_bTextEnabled) {
		CRect loc = GetLocation();
		loc.InflateRect(-AN_MARGIN, -AN_MARGIN, 0, 0);
		Gdiplus::Font myFont(pDC->m_hDC, &m_logFont);
		DecoratorSDK::getFacilities().DrawString(gdip,
												 m_strText,
												 CRect(loc.left, loc.top, loc.right, loc.bottom),
												 &myFont,
												 (m_bActive) ? m_crText : DecoratorSDK::COLOR_GRAYED_OUT,
												 TA_LEFT | TA_TOP,
												 -1, // annotation texts should not be cropped
												 "",
												 "",
												 false);
	}
	if (m_spFCO)
		resizeLogic.Draw(pDC, gdip);
}

// New functions
void AnnotatorTextPart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
									 HWND parentWnd, DecoratorSDK::PreferenceMap& preferences)
{
	preferences[DecoratorSDK::PREF_LABELLENGTH]			= DecoratorSDK::PreferenceVariant((long)-1);
	preferences[DecoratorSDK::PREF_TEXTOVERRIDE]		= DecoratorSDK::PreferenceVariant(true);
	preferences[DecoratorSDK::PREF_TEXTCOLOROVERRIDE]	= DecoratorSDK::PreferenceVariant(true);
	preferences[DecoratorSDK::PREF_MULTILINEINPLACEEDIT]= DecoratorSDK::PreferenceVariant(true);
	TextPart::InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);
}

CPoint AnnotatorTextPart::GetTextPosition(CDC* pDC, Gdiplus::Graphics* gdip) const
{
	return GetTextLocation(pDC, gdip).TopLeft();
}

CRect AnnotatorTextPart::GetTextLocation(CDC* pDC, Gdiplus::Graphics* gdip) const
{
	return GetLocation();
}

void AnnotatorTextPart::ExecuteOperation(const CString& newString)
{
	// transaction operation begin
	try {
		m_strText = newString;
		CString tmpstr = CAnnotationUtil::ResolveNewLinesToLF(newString);
		CComBSTR bstr;
		CopyTo(tmpstr, bstr);
		COMTHROW(m_regRoot->put_Value(bstr));
	}
	catch (hresult_exception&) {
//		m_strText = "Unable to write annotation!";
	}
	// transaction operation end
}

void AnnotatorTextPart::ReadPreferences(void)
{
	try {
		CComBSTR bstr;
		COMTHROW(m_regRoot->get_Value(&bstr));
		m_strText = bstr;
		m_strText = CAnnotationUtil::ResolveNewLinesToCRLF(m_strText);
	}
	catch (hresult_exception&) {
		m_strText = "Unable to read annotation !";
	}

	try {
		CComBSTR bstr;
		COMTHROW(m_regRoot->get_Name(&bstr));
		m_strName = bstr;
	}
	catch (hresult_exception&) {
		m_strName = "Unknown";
	}

	try {
		CComBSTR bstr;
		CComPtr<IMgaRegNode> lfNode;
		CComBSTR lfName(AN_FONT_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(lfName, &lfNode));
		if (lfNode != NULL) {
			COMTHROW(lfNode->get_Value(&bstr));
		}
		CString str(bstr);
		if (!CAnnotationUtil::LogfontDecode(str, &m_logFont)) {
			// throw hresult_exception();
			CAnnotationUtil::FillLogFontWithDefault(&m_logFont);
		}
	}
	catch (hresult_exception&) {
		CAnnotationUtil::FillLogFontWithDefault(&m_logFont);
	}

	try {
		CComBSTR bstr;
		CComPtr<IMgaRegNode> colNode;
		CComBSTR colName(AN_COLOR_PREF);
		COMTHROW(m_regRoot->get_SubNodeByName(colName, &colNode));
		if (colNode != NULL) {
			COMTHROW(colNode->get_Value(&bstr));
		}
		CString strVal(bstr);
		unsigned int val;
		if (_stscanf(strVal,_T("%x"),&val) == 1) {
			unsigned int r = (val & 0xff0000) >> 16;
			unsigned int g = (val & 0xff00) >> 8;
			unsigned int b = val & 0xff;
			m_crText = RGB(r,g,b);
		} else {
			m_crText = AN_DEFAULT_COLOR;
		}
	}
	catch (hresult_exception&) {
		m_crText = AN_DEFAULT_COLOR;
	}
}

}; // namespace AnnotatorDecor
