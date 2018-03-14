//################################################################################################
//
// New Annotator decorator composite part class
//	AnnotatorCompositePart.h
//
//################################################################################################

#ifndef __ANNOTATORCOMPOSITEPART_H_
#define __ANNOTATORCOMPOSITEPART_H_


#include "StdAfx.h"
#include "ObjectAndTextPart.h"


namespace AnnotatorDecor {

//################################################################################################
//
// CLASS : AnnotatorCompositePart
//
//################################################################################################

class AnnotatorCompositePart: public DecoratorSDK::ObjectAndTextPart
{
protected:
	CString								m_strName;
	VARIANT								m_vValue;

public:
	AnnotatorCompositePart(DecoratorSDK::PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink);
	virtual ~AnnotatorCompositePart();

// =============== resembles IMgaElementDecorator
public:
	virtual void	SetParam					(const CString& strName, VARIANT vValue);
	virtual CSize	GetPreferredSize			(void) const;
	virtual CRect	GetPortLocation				(CComPtr<IMgaFCO>& fco) const;
	virtual CRect	GetLabelLocation			(void) const;

	virtual void	InitializeEx				(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart,
												 CComPtr<IMgaFCO>& pFCO, HWND parentWnd, DecoratorSDK::PreferenceMap& preferences);
	virtual void	InitializeEx				(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart,
												 CComPtr<IMgaFCO>& pFCO, HWND parentWnd);
};

}; // namespace AnnotatorDecor

#endif //__ANNOTATORCOMPOSITEPART_H_
