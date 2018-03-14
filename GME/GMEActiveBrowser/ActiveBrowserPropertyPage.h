// ActiveBrowserPropertyPage.h : header file
//

#ifndef __ACTIVEBROWSERPROPERTYPAGE_H__
#define __ACTIVEBROWSERPROPERTYPAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CAggregatePropertyPage dialog

#include "MgaMappedTreeCtrl.h"
#include "MetaTreeCtrl.h"
#include "AggregateTreeCtrl.h"
#include "InheritanceTreeCtrl.h"
#include "AggregateOptions.h"
#include "AutoComboBox.h"
#include "ComboEdit.h"	// Added by ClassView
#include "MetaConnectionDlg.h"

class CActiveBrowserPropertySheet;


class CAggregatePropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAggregatePropertyPage)

	friend class CAggregateContextMenu;
// Construction
public:
	void RefreshLibrary();
	void AttachLibrary();
	void LibraryDependencies();
	void LibraryAmbiguityChk();
	void ProjectSourceControlUpdate( CComBSTR p_id);
	void SourceControlObjectOwner( CComBSTR p_id);
	void SourceControlActiveUsers();
	bool askUserAndDeleteLibrary( CComPtr<IMgaFolder> p_ccpFolder);

	void PrepareStateImageList();
	void DoDrag(CPoint ptDrag);
	void DoCopy();
	void DoCopyClosure();
	void DoCopySmart( int k = 0);
	bool askUserAndDetach( CComPtr<IMgaObject> obj); // called when archetype is deleted
	void accessRights( bool pbProtectIt);
	void HighlightItem(IMgaObject* pUnk, int highlight);

	
	CComboEdit m_ComboEditCtrl;

	int GetSourceControlStateOffset(IMgaObject * obj, int * latent);

	BOOL HasDisplayedChild(HTREEITEM hItem);

	CAggregateOptions m_Options;

	void GotoIUnk(BSTR Id);
	void GotoIUnkPtr( IMgaObject * p_obj);
	void Refresh();
	void OnMgaEvent(CComPtr<IMgaObject> ccpMgaObject, unsigned long lEventMask);

	// Called on opening the project
	void OpenProject();
	void SetFocus2();
	// Called on closing the project
	void CloseProject();

	BOOL reqHasDisplayedChild(IMgaObject* pIMgaObject, int nDepthLevel=-1);
	
	CActiveBrowserPropertySheet* m_parent;
		   
	CAggregatePropertyPage();
	~CAggregatePropertyPage();

// Dialog Data
	//{{AFX_DATA(CAggregatePropertyPage)
	enum { IDD = IDD_PROPPAGE_AGGREGATE };
	CAutoComboBox	m_ComboSearchCtrl;
	CButton	m_SearchButtonCtrl;
	CAggregateTreeCtrl	m_TreeAggregate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAggregatePropertyPage)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	// Generated message map functions
	//{{AFX_MSG(CAggregatePropertyPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnItemExpandingTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangedTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDownTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDispInfoTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemExpandedTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSearch();
	afx_msg void OnBeginDragTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CImageList m_ImageList;
	CImageList m_StateImageList;
	CImageList* m_pDragImageList;
	void OnSetFocus( CWnd* pOldWnd );


private:

	BOOL m_bProjectOpen;
	BOOL m_bIsBrowserCreatedNewObject;
	CString m_strProjectName;
	HBITMAP m_SearchButtonCtrlBitmap;

	// Creates the tree		
	void SetupTree();

    
	// Gets the Name property of an MgaObject
	CString GetDisplayedName(IMgaObject *pIMgaObject);
	// Recursively build up the tree traversing the meta hierarchy
	void reqBuildAggregateTree(HTREEITEM hParent,IMgaObject* pIMgaObject, int nDepthLevel, int nCounter=0, bool bWithUpdate=false);

	static void composeInfo( CString p_msgText, CComPtr<IMgaFolders>& coll, CString& p_msg);

};


/////////////////////////////////////////////////////////////////////////////
// CInheritancePropertyPage dialog

class CInheritancePropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CInheritancePropertyPage)

// Construction
public:

	CComboEdit m_ComboEditCtrl;

	// Called on opening the project
	void OpenProject();
	// Called on closing the project
	void CloseProject();
	CInheritancePropertyPage();
	~CInheritancePropertyPage();
	
	// Resets the last root object, used by the CAggregatePropertyPage also
	void ResetRoot();
	void Refresh();
	void OnMgaEvent(CComPtr<IMgaObject> ccpMgaObject, unsigned long lEventMask);
	void PrepareStateImageList();

	CActiveBrowserPropertySheet* m_parent;

// Dialog Data
	//{{AFX_DATA(CInheritancePropertyPage)
	enum { IDD = IDD_PROPPAGE_INHERITANCE };
	CAutoComboBox	m_ComboSearchCtrl;
	CInheritanceTreeCtrl	m_TreeInheritance;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInheritancePropertyPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInheritancePropertyPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangedTreeInheritance(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeInheritance(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemExpandingTreeInheritance(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDownTreeInheritance(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CImageList m_ImageList;
	CImageList m_StateImageList;

private:
	// Remembering the current root object (in the inheritance tree)
	CComPtr<IMgaFCO> m_theCurrentRootFCO;
	// Remembering the previously selected item in the aggregation window
	LPUNKNOWN m_pPrevSelectedItem; 
	// Gets the Name property of an MgaObject
	CString GetDisplayedName(IMgaObject *pIMgaObject);
	// Creates the tree
	void SetupTree();
	// Creates the tree using the m_theCurrentRootFCO var as root / used for refresh
	void SetupTree2();
	// Recursively build up the tree traversing the inheritance hierarchy
	void reqBuildInheritanceTree(HTREEITEM hParent, IMgaFCO* pIMgaFCO, int nDepthLevel);
	// Finds the root of the inheritance hierarchy
	void reqFindInheritanceRoot(CComPtr<IMgaFCO>&ccpMgaFCO);

};


/////////////////////////////////////////////////////////////////////////////
// CMetaPropertyPage dialog

typedef CComPtr<IMgaMetaBase>MetaObjectPtr;
#define ADAPT_META_OBJECT(x) CAdapt<MetaObjectPtr>(x)
typedef CList<CAdapt<MetaObjectPtr>,CAdapt<MetaObjectPtr>&>CMetaObjectList;


class CMetaPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMetaPropertyPage)

// Construction
public:

	CComboEdit m_ComboEditCtrl;

	// Called on opening the project
	void OpenProject();
	// Called on closing the project
	void CloseProject();
	CMetaPropertyPage();
	~CMetaPropertyPage();

	CActiveBrowserPropertySheet* m_parent;

// Dialog Data
	//{{AFX_DATA(CMetaPropertyPage)
	enum { IDD = IDD_PROPPAGE_META };
	CAutoComboBox	m_ComboSearchCtrl;
	CMetaTreeCtrl	m_TreeMeta;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMetaPropertyPage)
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMetaPropertyPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkTreeMeta(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangedTreeMeta(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemExpandingTreeMeta(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDownTreeMeta(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CImageList m_ImageList;
	
	private:
	// Strores project name to replace the label of the root folder
	CString m_strProjectName;
	// Processes the data structure of the Attribute data type
	void ProcessAttributes(HTREEITEM hParent, IMgaMetaFCO* pIMgaMetaFCO);
	// Processes the complex data structure of the Connection data type
	void ProcessConnection(CString &strConnectionToolTip, IMgaMetaConnection *pIMetaConnection);
	
	// Gets the DisplayedName property of a MetaBase	
	CString GetDisplayedName(IMgaMetaBase* pIMgaMetaBase);
	// Creates the tree		
	void SetupTree();

	HTREEITEM InsertIntoMetaTree(HTREEITEM hParent,IMgaMetaBase* IMetaObject, CString strRoleName);
	int InsertChildren(CComPtr<IMgaMetaBase>& ccpMetaObject,CMetaObjectList& MetaObjectList);


};



#endif // __ACTIVEBROWSERPROPERTYPAGE_H__
