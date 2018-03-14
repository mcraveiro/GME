//################################################################################################
//
// UML attribute part class (decorator part)
//	UMLAttributePart.h
//
//################################################################################################

#ifndef __UMLATTRIBUTEPART_H_
#define __UMLATTRIBUTEPART_H_


#include "StdAfx.h"
#include "AttributePart.h"


namespace UMLDecor {

//################################################################################################
//
// CLASS : UMLAttributePart
//
//################################################################################################

class UMLAttributePart: public DecoratorSDK::AttributePart
{
protected:
	CComPtr<IMgaFCO>	m_spActualFCO;

public:
	UMLAttributePart(PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents> eventSink,
					 const CString& nameStr, const CString& typeStr, CComPtr<IMgaFCO>& pFCO);
	virtual ~UMLAttributePart();

	virtual void	ExecuteOperation			(const CString& newString);
	virtual bool	IsLesser					(const AttributePart* other);
};

}; // namespace UMLDecor

#endif //__UMLATTRIBUTEPART_H_
