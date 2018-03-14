//################################################################################################
//
// Annotator text part class
//	AnnotatorTextPart.h
//
//################################################################################################

#ifndef __ANNOTATORTEXTPART_H_
#define __ANNOTATORTEXTPART_H_


#include "StdAfx.h"
#include "TextPart.h"


namespace AnnotatorDecor {

//################################################################################################
//
// CLASS : AnnotatorTextPart
//
//################################################################################################

class AnnotatorTextPart: public DecoratorSDK::TextPart
{
protected:
	CComPtr<IMgaRegNode>	m_regRoot;
	CString					m_strName;
	LOGFONT					m_logFont;

public:
	AnnotatorTextPart(DecoratorSDK::PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink);
	virtual ~AnnotatorTextPart();

// =============== resembles IMgaElementDecorator
public:
	virtual void	SetParam					(const CString& strName, VARIANT vValue);
	virtual bool	GetParam					(const CString& strName, VARIANT* pvValue);
	virtual CSize	GetPreferredSize			(void) const;
	virtual void	Draw						(CDC* pDC, Gdiplus::Graphics* gdip);
	virtual void	InitializeEx				(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart,
												 CComPtr<IMgaFCO>& pFCO, HWND parentWnd, DecoratorSDK::PreferenceMap& preferences);

	virtual CPoint	GetTextPosition				(CDC* pDC, Gdiplus::Graphics* gdip) const;
	virtual void	SetTextRelYPosition			(long relYPosition) {};	// This doesn't make sense in case of annotator decorator
	virtual CRect	GetTextLocation				(CDC* pDC, Gdiplus::Graphics* gdip) const;
	virtual void	ExecuteOperation			(const CString& newString);

protected:
	void			ReadPreferences				(void);
};

}; // namespace AnnotatorDecor

#endif //__ANNOTATORTEXTPART_H_
