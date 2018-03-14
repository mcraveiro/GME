//################################################################################################
//
// UML stereo label part class (decorator part)
//	UMLStereoLabelPart.h
//
//################################################################################################

#ifndef __UMLSTEREOLABELPART_H_
#define __UMLSTEREOLABELPART_H_


#include "StdAfx.h"
#include "StereoLabelPart.h"


namespace UMLDecor {

//################################################################################################
//
// CLASS : UMLStereoLabelPart
//
//################################################################################################

class UMLStereoLabelPart: public DecoratorSDK::StereoLabelPart
{
protected:
	CComPtr<IMgaFCO>	m_spActualFCO;

public:
	UMLStereoLabelPart(PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents> eventSink, CComPtr<IMgaFCO>& pFCO, bool bTextEditable);
	virtual ~UMLStereoLabelPart();

	virtual void	ExecuteOperation			(const CString& newString);
};

}; // namespace UMLDecor

#endif //__UMLSTEREOLABELPART_H_
