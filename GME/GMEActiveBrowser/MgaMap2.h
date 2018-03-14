// MgaMap2.h: interface for the CMgaMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MGAMAP2_H__4FB2B383_49D6_4C83_AB80_63DFF3C45380__INCLUDED_)
#define AFX_MGAMAP2_H__4FB2B383_49D6_4C83_AB80_63DFF3C45380__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MgaObjectProxy.h"
#include "AfxColl.h"

typedef CMap<HTREEITEM,HTREEITEM,CMgaObjectProxy*,CMgaObjectProxy*> CMapItem2Object;
typedef CMap<LPUNKNOWN,LPUNKNOWN,HTREEITEM,HTREEITEM> CMapObject2Item;


class CMgaMap  
{
public:

	// High level functions for complex operations eg. context menu
	void AddEntry(HTREEITEM hTreeItem, CMgaObjectProxy ObjectProxy);
	BOOL RemoveObjectProxy(CMgaObjectProxy MgaObjectProxy);
	BOOL RemoveTreeItem( HTREEITEM hTreeItem);
	BOOL LookupTreeItem(CMgaObjectProxy MgaObjectProxy, HTREEITEM& hTreeItem);
	BOOL LookupTreeItem(LPUNKNOWN pUnknown, HTREEITEM &hTreeItem);
	BOOL LookupObjectProxy(HTREEITEM hTreeItem,CMgaObjectProxy& rObjectProxy);
	
	// Low level functions (involving IUnknown-based search) for efficiency
	// of the simple oprations
	BOOL bIsInMap(HTREEITEM hTreeItem);
	BOOL bIsInMap(LPUNKNOWN pUnknown);
	BOOL LookupObjectUnknown(HTREEITEM hTreeItem, LPUNKNOWN& pUnknown);
	void DestroyMap();
	CMgaMap();
	virtual ~CMgaMap();

	// Dump function
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc ) const; 
#endif


private:
	CMapObject2Item m_MapObject2Item;
	CMapItem2Object m_MapItem2Object; 

};

#endif // !defined(AFX_MGAMAP_H__4FB2B383_49D6_4C83_AB80_63DFF3C45380__INCLUDED_)
