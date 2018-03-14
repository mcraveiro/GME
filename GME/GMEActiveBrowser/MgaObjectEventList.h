// MgaObjectEventList.h: interface for the CMgaObjectEventList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MGAOBJECTEVENTLIST_H__887A50FF_BF3A_4ED4_9660_E0B83537533F__INCLUDED_)
#define AFX_MGAOBJECTEVENTLIST_H__887A50FF_BF3A_4ED4_9660_E0B83537533F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mga.h"
#include "ObjectEventWrapper.h"

class CMgaObjectEventList: public CPtrList
{
public:
	void RemoveAt( POSITION position );
	void RemoveAll();
	void Add(IMgaObject* pMgaObject,unsigned long eventmask);		
	CMgaObjectEventList();
	virtual ~CMgaObjectEventList();

};

#endif // !defined(AFX_MGAOBJECTEVENTLIST_H__887A50FF_BF3A_4ED4_9660_E0B83537533F__INCLUDED_)
