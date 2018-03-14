// MgaMappedTreeCtrl.h: interface for the CMgaMappedTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MGAMAPPEDTREECTRL_H__01ABA150_667F_4191_95BC_DA45058B98C9__INCLUDED_)
#define AFX_MGAMAPPEDTREECTRL_H__01ABA150_667F_4191_95BC_DA45058B98C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TreeCtrlEx.h"
#include "MgaMap.h"	// Added by ClassView
#include "GMEActiveBrowserDropTarget.h"	// Added by ClassView

#define MGATREECTRL_LOGEVENT(eventstring) \
{ \
	if(m_EventLogger != NULL) \
	{ \
		CComBSTR eventBSTR = eventstring; \
		m_EventLogger->LogEvent(eventBSTR); \
	} \
} \

class CMgaMappedTreeCtrlBase : public CTreeCtrlEx
{
public:
	CMgaMappedTreeCtrlBase() : m_DropTarget(this) {};
	virtual BOOL DoDrop(eDragOperation, COleDataObject*,CPoint);
	void RegisterDropTarget();
	virtual BOOL IsRelevantDropTarget(CPoint point,CImageList*);
	void ExpandAllChildren(HTREEITEM hParent,UINT nCode);
	void DeleteAllChildren(HTREEITEM hParent);
	BOOL SetItemState(HTREEITEM hItem, UINT nState);
	virtual BOOL DeleteItem(HTREEITEM hItem) = 0;
	virtual void CleanUp() = 0;
	virtual BOOL LookupObjectUnknown(HTREEITEM hTreeItem, IUnknown*& out) = 0;

protected:
	CGMEActiveBrowserDropTarget m_DropTarget;

	CComPtr<IMgaEventLogger> m_EventLogger;
};


template<class ObjectProxy>
class CMgaMappedTreeCtrl : public CMgaMappedTreeCtrlBase
{
public:
	CMgaMappedTreeCtrl() {
		m_EventLogger.CoCreateInstance(L"Mga.MgaEventLogger");
	}

	BOOL GetSelectedItemProxy(ObjectProxy& rMgaObjectProxy) {
		HTREEITEM hTreeItem = __super::GetSelectedItem();

		if (hTreeItem == NULL)
			return FALSE; // No selected item

		return m_MgaMap.LookupObjectProxy(hTreeItem, rMgaObjectProxy);
	}

	virtual ~CMgaMappedTreeCtrl() {}

	virtual BOOL DeleteItem(HTREEITEM hItem) {
		m_MgaMap.RemoveTreeItem(hItem);
		return CTreeCtrlEx::DeleteItem(hItem);
	}

	virtual void CleanUp() {
		CTreeCtrlEx::DeleteAllItems();
		m_MgaMap.DestroyMap();
	}

	virtual BOOL LookupObjectUnknown(HTREEITEM hTreeItem, IUnknown*& out) {
		return m_MgaMap.LookupObjectUnknown(hTreeItem, out);
	}

	CMgaMap<ObjectProxy> m_MgaMap;
};

#endif // !defined(AFX_MGAMAPPEDTREECTRL_H__01ABA150_667F_4191_95BC_DA45058B98C9__INCLUDED_)
