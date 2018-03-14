//################################################################################################
//
// Meta decorator class part class
//	MetaClassPart.h
//
//################################################################################################

#ifndef __METACLASSPART_H_
#define __METACLASSPART_H_


#include "StdAfx.h"
#include "ClassComplexPart.h"

#include "DecoratorStd.h"


namespace MetaDecor {

//################################################################################################
//
// CLASS : MetaClassPart
//
//################################################################################################

class MetaClassPart: public DecoratorSDK::ClassComplexPart
{
protected:
	ShapeCode		m_shape;
	CString			m_stereotypeName;
	bool			m_showAttributes;
	bool			m_showAbstract;
	bool			m_isAbstract;

public:
	MetaClassPart(DecoratorSDK::PartBase* pPart, CComPtr<IMgaCommonDecoratorEvents>& eventSink, ShapeCode shape,
				  const CString& stereotypeName);
	virtual ~MetaClassPart();

// =============== resembles IMgaElementDecorator
public:
	virtual void	SetParam					(const CString& strName, VARIANT vValue);
	virtual bool	GetParam					(const CString& strName, VARIANT* pvValue);
	virtual void	InitializeEx				(CComPtr<IMgaProject>& pProject, CComPtr<IMgaMetaPart>& pPart,
												 CComPtr<IMgaFCO>& pFCO, HWND parentWnd, DecoratorSDK::PreferenceMap& preferences);

	virtual void	CollectAttributes			(CComPtr<IMgaFCO> mgaFco = NULL);
};

}; // namespace MetaDecor

#endif //__METACLASSPART_H_
