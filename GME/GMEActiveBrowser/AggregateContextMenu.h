// AggregateContextMenu.h: interface for the CAggregateContextMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGGREGATECONTEXTMENU_H__E9BEC526_A063_4D7D_AA05_2BE6764245B9__INCLUDED_)
#define AFX_AGGREGATECONTEXTMENU_H__E9BEC526_A063_4D7D_AA05_2BE6764245B9__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ActiveBrowserPropertyPage.h"


typedef CComPtr<IMgaMetaBase>MgaMetaBasePtr;
#define ADAPT(x) CAdapt<MgaMetaBasePtr>(x)

class CAggregateContextMenu : public CMenu  
{
	friend class CAggregatePropertyPage;

	CAggregatePropertyPage*m_pParent;
public:
	void OnRefreshLibrary();
	void OnAttachLibrary();
	void OnLibraryDependencies();
	void OnLibraryAmbiguityChk();
	void OnMFSourceControlUpdate();
	void OnUsersActive();
	void OnUsersOwner();

	UINT m_nSelectionCount;
	BOOL Run(CPoint);
	CAggregateContextMenu(CAggregatePropertyPage* pParent);
	virtual ~CAggregateContextMenu();

protected:
	void OnOptions();
	void OnCheckConstraint();
	void CreateForAll();
	void SetModelChildren(LPUNKNOWN pUnknown);
	void SetFolderChildren(LPUNKNOWN pUnknown);
	void CreateForSingleItem();
	void OnReadOnly( bool ro);
	void OnHelp();
	void OnShowInParent();
	void OnFollowRef();
	void OnRegistry();
	void OnPreferences();
	void OnAttributes();
	void OnProperties();
	void OnSortCreation();
	void OnSortType();
	void OnSortName();
	void OnInterpret();
	void OnCheckAllConstraint();
	void OnEditDelete();
	void OnEditPaste();
	void OnEditPasteSpecialAsReference();
	void OnEditPasteSpecialAsSubtype();
	void OnEditPasteSpecialAsInstance();
	void OnEditPasteClosure( bool merge = false);
	void OnEditCopy();
	void OnEditCopyClosure();
	void OnEditCopySmart();
	void OnEditCut();
	void OnCustomItems(UINT nID);
	BOOL DispatchCommand(UINT nSelectedID);
private:
	UINT m_InsertionMaxID;
	UINT m_InsertionMinID;
	CArray<CAdapt<MgaMetaBasePtr>,CAdapt<MgaMetaBasePtr>&>m_MenuItem2MetaObjArray;
};

#endif // !defined(AFX_AGGREGATECONTEXTMENU_H__E9BEC526_A063_4D7D_AA05_2BE6764245B9__INCLUDED_)
