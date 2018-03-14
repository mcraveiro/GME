// MgaMap.h: interface for the CMgaMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MGAMAP_H__4FB2B383_49D6_4C83_AB80_63DFF3C45380__INCLUDED_)
#define AFX_MGAMAP_H__4FB2B383_49D6_4C83_AB80_63DFF3C45380__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MgaObjectProxy.h"
#include "AfxColl.h"



template<class ObjectProxy>
class CMgaMap
{
	class CMgaObjectProxyMapItem : public ObjectProxy
	{
	public:
		CMgaObjectProxyMapItem(CMgaObjectProxy& rSourceMgaObjectProxy, HTREEITEM hTreeItem) : ObjectProxy(rSourceMgaObjectProxy),m_hTreeItem(hTreeItem) {};
		virtual ~CMgaObjectProxyMapItem() {}
		HTREEITEM m_hTreeItem;
	};
public:
	typedef CMap<HTREEITEM,HTREEITEM,LPUNKNOWN,LPUNKNOWN> CMapItem2Object;
	typedef CMap<LPUNKNOWN,LPUNKNOWN,CMgaObjectProxyMapItem*,CMgaObjectProxyMapItem*> CMapObject2Item;

	CMgaMap() {}

	virtual ~CMgaMap() {
		DestroyMap();
	}

	void DestroyMap()
	{
		// Remove elements from object map and delete the object
		POSITION pos=m_MapObject2Item.GetStartPosition();
		CMgaObjectProxyMapItem* pObject;
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
	BOOL LookupTreeItem(CMgaObjectProxy MgaObjectProxy, HTREEITEM &hTreeItem)
	{
		return LookupTreeItem(MgaObjectProxy.m_pMgaObject, hTreeItem);
	}


	// Retreives the Object Proxy from the corresponding TreeItem handle
	BOOL LookupObjectProxy(HTREEITEM hTreeItem, ObjectProxy& rObjectProxy)
	{
		CMgaObjectProxyMapItem* pMgaObjectProxyMapItem;
		LPUNKNOWN pUnknown;
	
		BOOL bResult=m_MapItem2Object.Lookup(hTreeItem,pUnknown);
	
		if(bResult)
		{
			if( (bResult=m_MapObject2Item.Lookup(pUnknown,pMgaObjectProxyMapItem)) )
			{
				rObjectProxy=*pMgaObjectProxyMapItem;
			}
		}
		return  bResult;
	}


	// Removes a tree item maintaining the consistency between the two maps
	BOOL RemoveTreeItem(HTREEITEM hTreeItem)
	{
		LPUNKNOWN pUnknown;

		if(!m_MapItem2Object.Lookup(hTreeItem,pUnknown))
		{
			return FALSE;
		}
		else
		{
			m_MapItem2Object.RemoveKey(hTreeItem);

			CMgaObjectProxyMapItem* pObjectProxyMapItem;
			if(m_MapObject2Item.Lookup(pUnknown,pObjectProxyMapItem))
			{
				m_MapObject2Item.RemoveKey(pUnknown);			
				// Free COM object
				pUnknown->Release();
				delete pObjectProxyMapItem;
			}
			return TRUE;
		}

	}


	// Removes an Object Proxy maintaining the consistency between the two maps
	BOOL RemoveObjectProxy(CMgaObjectProxy MgaObjectProxy)
	{	
		CMgaObjectProxyMapItem* pObjectProxyMapItem;

		CComPtr<IUnknown> pUnk;
		MgaObjectProxy.m_pMgaObject->QueryInterface(IID_IUnknown, (void**)&pUnk);
		if(!m_MapObject2Item.Lookup(pUnk, pObjectProxyMapItem))
		{
			return FALSE;
		}
		else
		{
			m_MapObject2Item.RemoveKey(pObjectProxyMapItem->m_pMgaObject);
			m_MapItem2Object.RemoveKey(pObjectProxyMapItem->m_hTreeItem);
		
			// Free COM object
			pObjectProxyMapItem->m_pMgaObject->Release();
			delete pObjectProxyMapItem;
			return TRUE;
		}
	}


	// Adds a new TreeItem Handle - ObjectProxy pair to the map
	ObjectProxy& AddEntry(HTREEITEM hTreeItem, ObjectProxy MgaObjectProxy)
	{

	// Removing the elements from the maps with the keys, because CMap::SetAt would overwrite
	// the value belonging to the already existing key 
	// resulting an inconsistency with the other map. 
		RemoveTreeItem(hTreeItem);
		RemoveObjectProxy(MgaObjectProxy);
	
		// Increase reference counter for this map
		IUnknown* pUnk = MgaObjectProxy.m_pMgaObject;
		MgaObjectProxy.m_pMgaObject = NULL;
		pUnk->QueryInterface(IID_IUnknown, (void**)&MgaObjectProxy.m_pMgaObject);

		m_MapItem2Object.SetAt(hTreeItem,MgaObjectProxy.m_pMgaObject);

		CMgaObjectProxyMapItem* pMgaObjectProxyMapItem= new CMgaObjectProxyMapItem(MgaObjectProxy, hTreeItem);
		m_MapObject2Item.SetAt(MgaObjectProxy.m_pMgaObject,pMgaObjectProxyMapItem);

		return *pMgaObjectProxyMapItem;
	}


	// Low level function for efficiency: if one wants IUnknown pointer
	// he can get it with one lookup call
	BOOL LookupObjectUnknown(HTREEITEM hTreeItem, LPUNKNOWN &pUnknown)
	{

		return m_MapItem2Object.Lookup(hTreeItem,pUnknown);
	}


	_COM_SMARTPTR_TYPEDEF(IMgaObject, __uuidof(IMgaObject));

	BOOL SearchTreeItem(BSTR Id, HTREEITEM &hTreeItem, IUnknown* &punk)
	{
		// read all looking for the ID

	   LPUNKNOWN	nKey;
	   CMgaObjectProxyMapItem *	pt;
	   POSITION pos = m_MapObject2Item.GetStartPosition();
	   bool found = false;
	   while (pos != NULL)
	   {
		  m_MapObject2Item.GetNextAssoc( pos, nKey, pt);
		  IMgaObjectPtr mgao(nKey);
		  IUnknownPtr punkptr(nKey);

		  punk = punkptr;
		  _bstr_t boid;
		  COMTHROW(mgao->get_ID(boid.GetAddress()));
		  _bstr_t bid = Id;
		  if (bid == boid)
		  {
			  found = true;
			  break;
		  }
	   }

		if(found) 
			hTreeItem = pt->m_hTreeItem;

		return (found);
	}


	BOOL LookupTreeItem(LPUNKNOWN pUnknown, HTREEITEM &hTreeItem)
	{
		CComPtr<IUnknown> pUnk;
		pUnknown->QueryInterface(IID_IUnknown, (void**)&pUnk);
		CMgaObjectProxyMapItem* pMgaObjectProxyMapItem;

		BOOL bResult=m_MapObject2Item.Lookup(pUnk,pMgaObjectProxyMapItem);

		if (bResult)
			hTreeItem=pMgaObjectProxyMapItem->m_hTreeItem;

		return bResult;
	}



	BOOL bIsInMap(LPUNKNOWN pUnknown)
	{
		CComPtr<IUnknown> pUnk;
		pUnknown->QueryInterface(IID_IUnknown, (void**)&pUnk);

		CMgaObjectProxyMapItem* pMgaObjectProxyMapItem;
		return m_MapObject2Item.Lookup(pUnk,pMgaObjectProxyMapItem);
	}

	BOOL bIsInMap(HTREEITEM hTreeItem)
	{
		LPUNKNOWN pUnknown;
		return m_MapItem2Object.Lookup(hTreeItem,pUnknown);
	}



	// Dump function
#ifdef _DEBUG
	void Dump(CDumpContext& dc ) const
	{
		return;
		dc<<"______________________ MgaMap Dump begin _______________________";
		// Dumping the Item2Object Map
		dc<<"\n\nMapItem2Object:\n";
	
		HTREEITEM hItem;
		LPUNKNOWN pUnknown;

		POSITION pos = m_MapItem2Object.GetStartPosition();
		while(pos!=NULL)
		{
			m_MapItem2Object.GetNextAssoc(pos,hItem,pUnknown);
			CString strOut;
			strOut.Format(_T(" %x  |  %x\n"),hItem, pUnknown);
			dc<<strOut;
		}
		dc<<"\n";

		// Dumping Object2Item Map
		dc<<"\n\nMapObject2Item:\n";
	

		CMgaObjectProxyMapItem* pMgaObjectProxyMapItem;


		pos = m_MapObject2Item.GetStartPosition();
		while(pos!=NULL)
		{
			m_MapObject2Item.GetNextAssoc(pos,pUnknown,pMgaObjectProxyMapItem);
			CString strOut;
			strOut.Format(_T(" %x  |  %x  |  %d\n"), pUnknown, pMgaObjectProxyMapItem->m_hTreeItem,pMgaObjectProxyMapItem->m_TypeInfo);
			dc<<strOut;
		}
		dc<<"\n";

		dc<<"______________________ MgaMap Dump end _______________________\n\n";
	}
#endif

private:
	CMapObject2Item m_MapObject2Item;
	CMapItem2Object m_MapItem2Object; 

};

#endif // !defined(AFX_MGAMAP_H__4FB2B383_49D6_4C83_AB80_63DFF3C45380__INCLUDED_)
