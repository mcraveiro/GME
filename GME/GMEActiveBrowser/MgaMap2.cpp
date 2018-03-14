// MgaMap2.cpp: implementation of the CMgaMap class.
//
// Maintains double map to enforce the easy mapping between a 
// tree element and the corresponding mga object and vice versa
// providing fast search based on the CMap hash tables and ensures the consistency
// between the two inner maps.
//
// The first map (m_MapItem2Object) maintans the correspondence between a tree item
// handle as a key and an CMgaObjectProxy type pointer as a value.
// The second map (m_MapObject2Item) has an LPUNKNOWN pointer as a 
// key and a tree item handle.
// So the mapping can be summarized as follows:
//	m_MapItem2Object: HTREEITEM -> CMgaObjectProxy
//	m_MapObject2Item: LPUNKNOWN -> HTREEITEM
//
// 
//
// Tihamer Levendovszky 11/26/2001
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "MgaMap2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMgaMap::CMgaMap()
{

}

CMgaMap::~CMgaMap()
{
	DestroyMap();
}

void CMgaMap::DestroyMap()
{
	// Remove elements from object map and delete the object
	POSITION pos=m_MapObject2Item.GetStartPosition();
	CMgaObjectProxy* pObject;
	LPUNKNOWN pUnknown;

	// Remove the TreeItem handles and LPUNKNOWNs from the item map
	m_MapItem2Object.RemoveAll();


	while(pos!=NULL)
	{
		m_MapObject2Item.GetNextAssoc(pos,pUnknown,pObject);
		m_MapObject2Item.RemoveKey(pUnknown);
		delete pObject;
		pUnknown->Release();
	}

}

// Retreives the corresponding TreeItem handle from an Object Proxy
BOOL CMgaMap::LookupTreeItem(CMgaObjectProxy MgaObjectProxy, HTREEITEM &hTreeItem)
{
	
	return m_MapObject2Item.Lookup(MgaObjectProxy.m_pMgaObject,hTreeItem);

	
}


// Retreives the Object Proxy from the corresponding TreeItem handle
BOOL CMgaMap::LookupObjectProxy(HTREEITEM hTreeItem,CMgaObjectProxy& rObjectProxy)
{
	CMgaObjectProxy* pMgaObjectProxy;

	
	 bResult=m_MapItem2Object.Lookup(hTreeItem,pMgaObjectProxy);
	
	if(bResult)
	{
		rObjectProxy=*pMgaObjectProxy;
	}
	return  bResult;
}


// Removes a tree item maintaining the consistency between the two maps
BOOL CMgaMap::RemoveTreeItem(HTREEITEM hTreeItem)
{
	CMgaObjectProxy* pMgaObjectProxy;

	if(!m_MapItem2Object.Lookup(hTreeItem,pMgaObjectProxy))
	{
		return FALSE;
	}
	else
	{
		m_MapItem2Object.RemoveKey(hTreeItem);
		
		m_MapObject2Item.RemoveKey(pMgaObjectProxy->m_pMgaObject);			
		// Free COM object
		pMgaObjectProxy->m_pMgaObject->Release();
		delete pObjectProxy;
		
		return TRUE;
	}

}


// Removes an Object Proxy maintaining the consistency between the two maps
BOOL CMgaMap::RemoveObjectProxy(CMgaObjectProxy MgaObjectProxy)
{	
	HTREEITEM hTreItem;

	if(!m_MapObject2Item.Lookup(MgaObjectProxy.m_pMgaObject,hTreItem))
	{
		return FALSE;
	}
	else
	{
		m_MapObject2Item.RemoveKey(pObjectProxyMapItem->m_pMgaObject);
		m_MapItem2Object.RemoveKey(hTreeItem);
		
		// Free COM object
		pObjectProxy->m_pMgaObject->Release();
		delete pObjectProxyMap;
		return TRUE;
	}
}


// Adds a new TreeItem Handle - ObjectProxy pair to the map
void CMgaMap::AddEntry(HTREEITEM hTreeItem, CMgaObjectProxy MgaObjectProxy)
{

// Removing the elements from the maps with the keys, because CMap::SetAt would overwrite
// the value belonging to the already existing key 
// resulting an inconsistency with the other map. 
	RemoveTreeItem(hTreeItem);
	RemoveObjectProxy(MgaObjectProxy);	
	
	// Increase reference counter for this map
	MgaObjectProxy.m_pMgaObject->AddRef();

	CMgaObjectProxy* pMgaObjectProxy= new CMgaObjectProxy(MgaObjectProxy);
	m_MapItem2Object.SetAt(hTreeItem,pMgaObjectProxy);

	
	m_MapObject2Item.SetAt(MgaObjectProxy.m_pMgaObject,hTreeItem);

}


// Low level function for efficiency: if one wants IUnknown pointer
// he can get it with one lookup call
BOOL CMgaMap::LookupObjectUnknown(HTREEITEM hTreeItem, LPUNKNOWN &pUnknown)
{
	CMgaObjectProxy* pMgaObjectProxy;
	BOOL bResult=m_MapItem2Object.Lookup(hTreeItem,pMgaObjectProxy);

	if(bResult)
	{
		pUnknown=pMgaObjectProxy->m_pMgaObject;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



BOOL CMgaMap::LookupTreeItem(LPUNKNOWN pUnknown, HTREEITEM &hTreeItem)
{
	
	return m_MapObject2Item.Lookup(pUnknown,hTreeItem);
}



BOOL CMgaMap::bIsInMap(LPUNKNOWN pUnknown)
{
	HTREEITEM hTreeItem;

	return m_MapObject2Item.Lookup(pUnknown,hTreeItem);
}

BOOL CMgaMap::bIsInMap(HTREEITEM hTreeItem)
{
	CMgaObjectProxy* pMgaObjectProxy;
	return m_MapItem2Object.Lookup(hTreeItem,pMgaObjectProxy);
}



// Dump function
#ifdef _DEBUG
void CMgaMap::Dump(CDumpContext& dc ) const
{
	dc<<"______________________ MgaMap Dump begin _______________________";
	// Dumping the Item2Object Map
	dc<<"\n\nMapItem2Object:\n";
	
	HTREEITEM hItem;
	CMgaObjectProxy* pMgaObjectProxy;

	dc<<"ItemHandle  Proxy  Type";
	POSITION pos = m_MapItem2Object.GetStartPosition();
	while(pos!=NULL)
	{
		m_MapItem2Object.GetNextAssoc(pos,hItem,pMgaObjectProxy);
		CString strOut;
		strOut.Format(" %x  |  %x |  %x \n",hItem, pMgaObjectProxy,pMgaObjectProxy->m_TypeInfo);
		dc<<strOut;
	}
	dc<<"\n";

	// Dumping Object2Item Map
	dc<<"\n\nMapObject2Item:\n";
	

	LPUNKNOWN pUnknown;

	dc<<"pUnknown ItemHandle";
	pos = m_MapObject2Item.GetStartPosition();
	while(pos!=NULL)
	{
		m_MapObject2Item.GetNextAssoc(pos,pUnknown,hItem);
		CString strOut;
		strOut.Format(" %x  |  %x\n", pUnknown,hTreeItem,);
		dc<<strOut;
	}
	dc<<"\n";

	dc<<"______________________ MgaMap Dump end _______________________\n\n";
}
#endif