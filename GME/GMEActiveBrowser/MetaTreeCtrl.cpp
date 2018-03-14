// MetaTreeCtrl.cpp: implementation of the CMetaTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "MetaTreeCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMetaTreeCtrl::CMetaTreeCtrl()
{

}

CMetaTreeCtrl::~CMetaTreeCtrl()
{

}

// If recursive element returns NULL, otherwise the inserted item tree handle
HTREEITEM CMetaTreeCtrl::InsertItem(HTREEITEM hParent, CString strObjectName, LPUNKNOWN pUnknown, objtype_enum otObjectType)
{
	MGATREECTRL_LOGEVENT("CMetaTreeCtrl::InsertItem "+strObjectName+"\r\n");

	if(hParent==NULL)	// Root folder
	{
		HTREEITEM hItem=CTreeCtrlEx::InsertItem(strObjectName,0,0,TVI_ROOT,TVI_SORT);	
		CMgaObjectProxy ObjectProxy(pUnknown,otObjectType);
		m_MgaMap.AddEntry(hItem,ObjectProxy);
		SetItemData(hItem,(DWORD)0);
		return hItem;
	}

	if(m_MgaMap.bIsInMap(pUnknown)) // If the element has been already insereted in the map
	{
		HTREEITEM hItem=CTreeCtrlEx::InsertItem(strObjectName,(int)otObjectType,(int)otObjectType,hParent,TVI_SORT);	
		HTREEITEM hRefItem;
		m_MgaMap.LookupTreeItem(pUnknown,hRefItem); // Search for the handle of the inserted element
		SetItemData(hItem,(DWORD_PTR)hRefItem); // Set item data to reference that
		return NULL;
	}
	else
	{
		HTREEITEM hItem=CTreeCtrlEx::InsertItem(strObjectName,(int)otObjectType,(int)otObjectType,hParent,TVI_SORT);	
		CMgaObjectProxy ObjectProxy(pUnknown,otObjectType);
		m_MgaMap.AddEntry(hItem,ObjectProxy);
		SetItemData(hItem,(DWORD_PTR)0); // Set to zero if this is the first and hence no referenced element
		return hItem;
	}
}


