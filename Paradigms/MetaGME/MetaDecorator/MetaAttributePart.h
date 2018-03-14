//################################################################################################
//
// Meta attribute part class (decorator part)
//	MetaAttributePart.h
//
//################################################################################################

#ifndef __METAATTRIBUTEPART_H_
#define __METAATTRIBUTEPART_H_


#include "StdAfx.h"
#include "AttributePart.h"


namespace MetaDecor {

//################################################################################################
//
// CLASS : MetaAttributePart
//
//################################################################################################

class MetaAttributePart: public DecoratorSDK::AttributePart
{
protected:
	CComPtr<IMgaFCO>	m_spActualFCO;
	CPoint				m_boxPos;

public:
	MetaAttributePart(PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink,
					  const CString& nameStr, const CString& typeStr, const CPoint& boxPos, CComPtr<IMgaFCO>& pFCO);
	virtual ~MetaAttributePart();


	virtual void	InitializeEx				(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart,
												 CComPtr<IMgaFCO>& pFCO, HWND parentWnd, DecoratorSDK::PreferenceMap& preferences);

	virtual void	ExecuteOperation			(const CString& newString);
	virtual bool	IsLesser					(const AttributePart* other);

	CPoint			GetBoxPos					(void) const { return m_boxPos; };
};

}; // namespace MetaDecor

#endif //__METAATTRIBUTEPART_H_
