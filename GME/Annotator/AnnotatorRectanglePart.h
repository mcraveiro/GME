//################################################################################################
//
// Annotator rectangle part decorator class
//	AnnotatorRectanglePart.h
//
//################################################################################################

#ifndef __ANNOTATORRECTANGLEPART_H_
#define __ANNOTATORRECTANGLEPART_H_


#include "StdAfx.h"
#include "VectorPart.h"


namespace AnnotatorDecor {

//################################################################################################
//
// CLASS : AnnotatorRectanglePart
//
//################################################################################################

class AnnotatorRectanglePart: public DecoratorSDK::VectorPart
{
protected:
	CComPtr<IMgaRegNode>	m_regRoot;
	COLORREF				m_crBgColor;
	COLORREF				m_crShadow;
	COLORREF				m_crGradient;
	bool					m_bGradientFill;
	long					m_iGradientDirection;
	bool					m_bCastShadow;
	long					m_iShadowDepth;
	long					m_iShadowDirection;
	bool					m_bRoundCornerRect;
	long					m_iRoundCornerRadius;

public:
	AnnotatorRectanglePart(PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink);
	virtual ~AnnotatorRectanglePart();

// =============== resembles IMgaElementDecorator
public:
	virtual void			SetParam			(const CString& strName, VARIANT vValue);
	virtual bool			GetParam			(const CString& strName, VARIANT* pvValue);

	virtual void			Draw				(CDC* pDC, Gdiplus::Graphics* gdip);
	virtual void			InitializeEx		(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart,
												 CComPtr<IMgaFCO>& pFCO, HWND parentWnd, DecoratorSDK::PreferenceMap& preferences);

protected:
	void					ReadPreferences		(void);
};

}; // namespace AnnotatorDecor

#endif //__ANNOTATORRECTANGLEPART_H_
