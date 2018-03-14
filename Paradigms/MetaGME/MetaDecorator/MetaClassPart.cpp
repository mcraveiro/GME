//################################################################################################
//
// Meta decorator composite part class
//	MetaClassPart.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "MetaClassPart.h"

#include "Resource.h"
#include "DecoratorStd.h"
#include "MetaStereoLabelPart.h"
#include "MetaAttributePart.h"
#include "MetaStereoLabelPart.h"
#include "DecoratorExceptions.h"
#include "MetaDecoratorUtil.h"
#include <algorithm>


namespace MetaDecor {

//################################################################################################
//
// CLASS : MetaClassPart
//
//################################################################################################

MetaClassPart::MetaClassPart(DecoratorSDK::PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink, ShapeCode shape,
							 const CString& stereotypeName):
	ClassComplexPart(pPart, eventSink),
	m_shape				(shape),
	m_stereotypeName	(stereotypeName),
	m_showAttributes	(META_DEFAULT_SHOWATTRIBUTES),
	m_showAbstract		(META_DEFAULT_SHOWABSTRACT),
	m_isAbstract		(false)
{
}

MetaClassPart::~MetaClassPart()
{
}

void MetaClassPart::SetParam(const CString& strName, VARIANT vValue)
{
	bool showAttribSet = !strName.CompareNoCase(META_PARAM_SHOWATTRIBUTES);
	bool showAbstractSet = !strName.CompareNoCase(META_PARAM_SHOWABSTRACT);
	if (showAttribSet || showAbstractSet) {
		CComVariant	variantval(vValue);
		try {
			COMTHROW(variantval.ChangeType(VT_BSTR));
			CString val = variantval.bstrVal;
			if (!val.CompareNoCase(META_PARAM_VAL_TRUE)) {
				if (showAttribSet)
					m_showAttributes = true;
				else if (showAbstractSet)
					m_showAbstract = true;
			} else if (!val.CompareNoCase(META_PARAM_VAL_FALSE)) {
				if (showAttribSet)
					m_showAttributes = false;
				else if (showAbstractSet)
					m_showAbstract = false;
			} else {
				throw DecoratorException((DecoratorExceptionCode)E_METADECORATOR_INVALID_PARAMVALUE);
			}
		}
		catch (hresult_exception& e) {
			throw DecoratorException((DecoratorExceptionCode)e.hr);
		}
	} else {
		throw DecoratorException((DecoratorExceptionCode)E_DECORATOR_UNKNOWN_PARAMETER);
	}
}

bool MetaClassPart::GetParam(const CString& strName, VARIANT* pvValue)
{
	if (!strName.CompareNoCase(META_PARAM_SHOWATTRIBUTES)) {
		CComVariant	variantval;
		variantval.Attach(pvValue);
		variantval = (m_showAttributes ? META_PARAM_VAL_TRUE : META_PARAM_VAL_FALSE);
	} else if (!strName.CompareNoCase(META_PARAM_SHOWABSTRACT)) {
		CComVariant	variantval;
		variantval.Attach(pvValue);
		variantval = (m_showAttributes ? META_PARAM_VAL_TRUE : META_PARAM_VAL_FALSE);
	} else {
		throw DecoratorException((DecoratorExceptionCode)E_DECORATOR_UNKNOWN_PARAMETER);
	}
	return true;
}

// New functions
void MetaClassPart::InitializeEx(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart, CComPtr<IMgaFCO>& pFCO,
								 HWND parentWnd, DecoratorSDK::PreferenceMap& preferences)
{
	try {
		DecoratorSDK::getFacilities().getMetaFCO(pPart, m_spMetaFCO);
		preferences[DecoratorSDK::PREF_LABELFONT]		= DecoratorSDK::PreferenceVariant((long)DecoratorSDK::FONT_PORTNAME);
		preferences[DecoratorSDK::PREF_LABELLENGTH]		= DecoratorSDK::PreferenceVariant((long)-1);
		preferences[DecoratorSDK::PREF_LABELENABLED]	= DecoratorSDK::PreferenceVariant(true);
		preferences[DecoratorSDK::PREF_LABELWRAP]		= DecoratorSDK::PreferenceVariant((long)0);
		preferences[DecoratorSDK::PREF_TEXTOVERRIDE]	= DecoratorSDK::PreferenceVariant(true);

		if (m_spFCO) {
			if (MetaDecor::GetDecorUtils().IsFCO(m_stereotypeName)) {
				if (m_showAbstract || m_showAttributes) {
					CComPtr<IMgaFCOs> coll;
					try {
						// Collect proxies & the real class
						CComPtr<IMgaReference> ref;
						CComPtr<IMgaFCO> real;
						if (SUCCEEDED(m_spFCO.QueryInterface(&ref))) {
							COMTHROW(ref->get_Referred(&real));
							if (!real) {
								COMTHROW(coll.CoCreateInstance(OLESTR("Mga.MgaFCOs")));
								COMTHROW(coll->Append(m_spFCO));
							}
						} else {
							real = m_spFCO;
						}
						if (real) {
							COMTHROW(real->get_ReferencedBy(&coll));
							COMTHROW(coll->Append(real));
						}

						CComPtr<IMgaFCO> fco;
						MGACOLL_ITERATE(IMgaFCO,coll) {
							fco = MGACOLL_ITER;
							// Figure out isAbstract
							bool isAbstract;
							if (DecoratorSDK::getFacilities().getAttribute(fco ? fco : m_spFCO, META_ABSTRACT_ATTR, isAbstract) && isAbstract) {
								m_isAbstract = true;
							}
							// Collect attributes
							if (m_showAttributes) {
								CollectAttributes(fco);
								SortAttributes();
							}
						} MGACOLL_ITERATE_END;
					}
					catch (hresult_exception&) {
						m_isAbstract = false;
					}
				}
			}
			CComBSTR bstr;
			COMTHROW(m_spFCO->get_Name(&bstr));
			CString labelStr = bstr;
			m_LabelPart = new DecoratorSDK::ClassLabelPart(this, m_eventSink, m_showAbstract && m_isAbstract);
			m_LabelPart->SetText(labelStr);
		}

		m_StereotypePart = new MetaStereoLabelPart(this, m_eventSink);
		m_StereotypePart->SetText(m_stereotypeName);

		if (m_shape == CLASSPROXY) {
			m_copySignPart = new DecoratorSDK::MaskedBitmapPart(this, m_eventSink, IDB_REFERENCE_SIGN,
																DecoratorSDK::COLOR_TRANSPARENT,
																DecoratorSDK::COLOR_GRAYED_OUT);
		}

		// TODO: stereo label

		COLORREF color = GME_BLACK_COLOR;
		DecoratorSDK::getFacilities().getPreference(m_spFCO, m_spMetaFCO, DecoratorSDK::PREF_COLOR, color);
		preferences[DecoratorSDK::PREF_COLOR] = DecoratorSDK::PreferenceVariant(color);

		COLORREF nameColor = GME_BLACK_COLOR;
		DecoratorSDK::getFacilities().getPreference(m_spFCO, m_spMetaFCO, DecoratorSDK::PREF_LABELCOLOR, nameColor);
		preferences[DecoratorSDK::PREF_LABELCOLOR] = DecoratorSDK::PreferenceVariant(nameColor);
	}
	catch(hresult_exception &e)
	{
		ASSERT( FAILED(e.hr) );
		SetErrorInfo(e.hr);
	}

	ClassComplexPart::InitializeEx(pProject, pPart, pFCO, parentWnd, preferences);

	CalcRelPositions();
}

void MetaClassPart::CollectAttributes(CComPtr<IMgaFCO> mgaFco)
{
	CComPtr<IMgaConnPoints>	connPoints;
	COMTHROW(mgaFco->get_PartOfConns(&connPoints));
	CComPtr<IMgaConnPoint> connPoint;
	MGACOLL_ITERATE(IMgaConnPoint,connPoints) {
		connPoint = MGACOLL_ITER;

		CComPtr<IMgaConnection> conn;
		COMTHROW(connPoint->get_Owner(&conn));
		CComPtr<IMgaConnPoints>	targetConnPoints;

		COMTHROW(conn->get_ConnPoints(&targetConnPoints));
		CComPtr<IMgaConnPoint> targetConnPoint;
		MGACOLL_ITERATE(IMgaConnPoint,targetConnPoints) {
			targetConnPoint = MGACOLL_ITER;
			if (!connPoint.IsEqualObject(targetConnPoint)) {
				CComPtr<IMgaFCO> targetFco;
				COMTHROW(targetConnPoint->get_Target(&targetFco));
				CComPtr<IMgaMetaFCO> targetMetaFco;
				COMTHROW(targetFco->get_Meta(&targetMetaFco));
				CComBSTR bstr;
				COMTHROW(targetMetaFco->get_Name(&bstr));
				
				CComBSTR attrPosBstr;	// zolmol added: sort by [y, x]
				COMTHROW(targetFco->get_RegistryValue(L"PartRegs/Attributes/Position", &attrPosBstr));
				CString attr_pos = attrPosBstr;
				CPoint pos(0, 0);
				int res = _stscanf(attr_pos, _T("%d,%d"), &pos.x, &pos.y);
				if (res != 2)
					pos = CPoint(0, 0);

				CString targetKind = bstr;
				if (targetKind == META_BOOLEANATTR_KIND ||
					targetKind == META_ENUMATTR_KIND ||
					targetKind == META_FIELDATTR_KIND)
				{
					CComBSTR nameBstr;
					COMTHROW(targetFco->get_Name(&nameBstr));
					CString targetName = nameBstr;
					CString targetType;
					if (targetKind == META_BOOLEANATTR_KIND)
						targetType = META_BOOLEANATTR_LABEL;
					else if (targetKind == META_ENUMATTR_KIND)
						targetType = META_ENUMATTR_LABEL;
					else if (targetKind == META_FIELDATTR_KIND)
						targetType = META_FIELDATTR_LABEL;
					m_AttributeParts.push_back(new MetaAttributePart(this, m_eventSink, targetName, targetType,
																	 pos, targetFco));
				}
			}
		} MGACOLL_ITERATE_END;

	} MGACOLL_ITERATE_END;
}

}; // namespace MetaDecor
