// ObjectEventWrapper.h: interface for the CMgaObjectEventWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTEVENTWRAPPER_H__32D44E31_FC8E_4B22_A2F3_7F15C298B8C0__INCLUDED_)
#define AFX_OBJECTEVENTWRAPPER_H__32D44E31_FC8E_4B22_A2F3_7F15C298B8C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mga.h"

class CMgaObjectEventWrapper  
{
public:
	CMgaObjectEventWrapper(IMgaObject *pMgaObject, unsigned long eventmask);
	virtual ~CMgaObjectEventWrapper();
	CComPtr<IMgaObject> m_ccpMgaObject;
	unsigned long m_eventmask;
};

#endif // !defined(AFX_OBJECTEVENTWRAPPER_H__32D44E31_FC8E_4B22_A2F3_7F15C298B8C0__INCLUDED_)
