// AggregateTreeCtrl.h: interface for the CAggregateTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGGREGATETREECTRL_H__BAA3D537_E504_4801_B793_6F96B703380C__INCLUDED_)
#define AFX_AGGREGATETREECTRL_H__BAA3D537_E504_4801_B793_6F96B703380C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MgaMappedTreeCtrl.h"

#include <GdiPlus.h>
#include <memory>
#include <map>
#include <unordered_map>

struct CComBSTR_hashfunc : public stdext::hash_compare<CComBSTR> 
{
	size_t operator()(const CComBSTR &p_ob) const
	{
		size_t c = 0;
		for(unsigned int i = 0; i < p_ob.Length(); i+= 3) c += p_ob[i];
		return c;
	}
	bool operator()(const CComBSTR &p_ob1, const CComBSTR &p_ob2) const
	{
		return p_ob1 < p_ob2;
	}
};

struct CAggregateMgaObjectProxy : public CMgaObjectProxy
{
	CAggregateMgaObjectProxy() : CMgaObjectProxy() { };
	CAggregateMgaObjectProxy(const CMgaObjectProxy& proxy) : CMgaObjectProxy(proxy) {};
	CAggregateMgaObjectProxy(LPUNKNOWN pMgaObject, objtype_enum type) :
	    CMgaObjectProxy(pMgaObject, type) {};
};

class CAggregateTreeCtrl : public CMgaMappedTreeCtrl<CAggregateMgaObjectProxy>
{
	friend class CAggregatePropertyPage;
	friend class CAggregateContextMenu;

	CMapStringToString m_StateBuffer;

	BOOL m_bIsStateStored;
public:
	BOOL IsLibrary(LPUNKNOWN pUnknown, bool *p_ptrIsDependent = 0);
	static bool IsUngroupedLibrary(CComPtr<IMgaFolder> ptr);

    
	void SetItemProperties(HTREEITEM hItem, int p_fileLatentState=0, CAggregateMgaObjectProxy* insertedProxy=nullptr);
	std::map<_bstr_t, int> treeIcons;
	std::unordered_map<CComBSTR, int, CComBSTR_hashfunc> m_highlightedObjects;
	void GetCustomTreeIcon(IMgaObject* ccpMgaObject, TVITEM& tvItem);
	BOOL DoDrop(eDragOperation doDragOp, COleDataObject *pDataObject, CPoint point);
	BOOL DoDropWithoutChecking(eDragOperation doDragOp, COleDataObject *pDataObject, CPoint point);
	void MakeSureGUIDIsUniqueForSmartCopy( CComPtr<IMgaFCO>& fco);
	BOOL IsRelevantDropTarget(CPoint point,CImageList*);
	void RestoreState();
	void StoreState();
	void MaintainRegistry();
	void SortItems(HTREEITEM hItem=NULL);
	static int CALLBACK ItemCompareProc(LPARAM lParamItem1, LPARAM lParamItem2, LPARAM lParamSort);
	void SaveTreeStateToRegistry(CString&);
	void LoadTreeStateFromRegistry(CString&);
	void LoadItemStateFromRegistry(CString& strProjectName, HTREEITEM hItem);
	HTREEITEM InsertItem(HTREEITEM hParent, CString strObjectName, LPUNKNOWN pUnknown, objtype_enum otObjectType, bool update=false);
	HTREEITEM InsertItemUpdate(HTREEITEM hParent, CString strObjectName, LPUNKNOWN pUnknown, objtype_enum otObjectType);

	CAggregateTreeCtrl();
	virtual ~CAggregateTreeCtrl();

};

#endif // !defined(AFX_AGGREGATETREECTRL_H__BAA3D537_E504_4801_B793_6F96B703380C__INCLUDED_)
