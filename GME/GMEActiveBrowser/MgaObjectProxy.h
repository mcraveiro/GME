// MgaObjectProxy.h: interface for the CMgaObjectProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MGAOBJECTPROXY_H__EA2347EB_A339_4227_92F6_686F7CC8023D__INCLUDED_)
#define AFX_MGAOBJECTPROXY_H__EA2347EB_A339_4227_92F6_686F7CC8023D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

class CMgaObjectProxy  
{
public:
	CMgaObjectProxy(){};
	CMgaObjectProxy(LPUNKNOWN pMgaObject, objtype_enum type) :
		m_pMgaObject(pMgaObject),
		m_TypeInfo(type)
	{
	}

	CMgaObjectProxy(const CMgaObjectProxy& that) { *this = that; }
	virtual ~CMgaObjectProxy() {}

	void CMgaObjectProxy::operator =(const CMgaObjectProxy &rSourceObjectProxy)
	{
		m_pMgaObject = rSourceObjectProxy.m_pMgaObject;
		m_TypeInfo = rSourceObjectProxy.m_TypeInfo;
	}

	LPUNKNOWN m_pMgaObject;
	objtype_enum m_TypeInfo;
};

#endif // !defined(AFX_MGAOBJECTPROXY_H__EA2347EB_A339_4227_92F6_686F7CC8023D__INCLUDED_)
