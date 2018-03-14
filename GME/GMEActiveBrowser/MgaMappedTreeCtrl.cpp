// MgaMappedTreeCtrl.cpp: implementation of the CMgaMappedTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "MgaMappedTreeCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CMgaMappedTreeCtrlBase::SetItemState(HTREEITEM hItem, UINT nState)
{

	if(nState&TVIS_BOLD)CTreeCtrl::SetItemState(hItem,TVIS_BOLD,TVIS_BOLD);	
	if(nState&TVIS_SELECTED)CTreeCtrlEx::SelectItemEx(hItem);
	if(nState&TVIS_EXPANDED)Expand(hItem,TVE_EXPAND );

	return TRUE;
}

// Recursively deletes every child
void CMgaMappedTreeCtrlBase::DeleteAllChildren(HTREEITEM hParent)
{
	if (ItemHasChildren(hParent)) // this may indicate true and the GetChildItem may give at the same time back 0x00000000 since that children may not have htreeitem inserted for it
	{
	   HTREEITEM hItem = GetChildItem(hParent);

	   while (hItem != NULL)
	   {
			DeleteAllChildren(hItem);

		   	HTREEITEM hNextItem = GetNextSiblingItem(hItem);		
			// Consistently removes item from map and the tree ctrl
			DeleteItem(hItem);
			hItem=hNextItem;

	   }
	}

}

void CMgaMappedTreeCtrlBase::ExpandAllChildren(HTREEITEM hParent,UINT nCode)
{

	if (ItemHasChildren(hParent))
	{
	   HTREEITEM hItem = GetChildItem(hParent);

	   while (hItem != NULL)
	   {		   
		   Expand(hItem,nCode);
		   hItem = GetNextSiblingItem(hItem);						
	   }
	}

}


BOOL CMgaMappedTreeCtrlBase::IsRelevantDropTarget(CPoint point, CImageList*)
{
	return FALSE;
}

void CMgaMappedTreeCtrlBase::RegisterDropTarget()
{

	m_DropTarget.Register(this);
}

BOOL CMgaMappedTreeCtrlBase::DoDrop(eDragOperation, COleDataObject*, CPoint)
{
	return FALSE;
}
