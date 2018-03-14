//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMEConstraintBrowserDialog.h
//
//###############################################################################################################################################

#if !defined(AFX_GMECONSTRAINTBROWSERDIALOG_H__25C4CBC0_D9A0_4003_BD98_5AB843DD921A__INCLUDED_)
#define AFX_GMECONSTRAINTBROWSERDIALOG_H__25C4CBC0_D9A0_4003_BD98_5AB843DD921A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GMEConstraintTreeCtrl.h"
#include "OCLCommonEx.h"
#include "GMEConstraintEx.h"

namespace OclGmeCM {

	class Facade;

//##############################################################################################################################################
//
//	C L A S S : CSettingsPage <<< + CDialog
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

class CSettingsPage : public CDialog
{
// Construction
public:
	CSettingsPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsPage)
	enum { IDD = IDD_BROWSER_SETTINGS_PAGE };
	CEdit	m_edtViolationCount;
	CButton	m_chkEnableTracking;
	CButton	m_chkEnableLogical;
	CButton	m_chkEnableIterator;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingsPage)
	afx_msg void OnRadioButtonClicked();
	afx_msg void OnViolationCountChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//##############################################################################################################################################
//
//	C L A S S : CConstraintBrowserDialog<<< + CDialog
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	enum ConstraintStateImage
	{
		CSIMG_OK = 1,
		CSIMG_DEPENDENCY_ERROR = 2,
		CSIMG_SYNTAX_ERROR = 3,
		CSIMG_CONTEXT_ERROR = 4,
		CSIMG_SEMANTIC_ERROR = 5
	};

	enum State
	{
		NS_CHECKED_DISABLED = 1,
		NS_UNCHECKED_DISABLED = 2,
		NS_CHECKED = 3,
		NS_UNCHECKED = 4,
		NS_CHECKED_INHERITED = 5,
		NS_UNCHECKED_INHERITED = 6,
		NS_MISCELLANEOUS = 7,
		NS_UNKNOWN = 8,
		NS_ERROR = 9
	};

	typedef std::map< std::string , OclGme::ConstraintVector > 	ConstraintVectorMap;
	typedef std::map< std::string , OclCommonEx::SPObject >	ObjectMap;
	typedef std::map< HTREEITEM , OclGme::SpConstraint >	ConstraintMap;

	typedef std::map< HTREEITEM, std::string >					MapH2ID;
	typedef std::map< std::string, HTREEITEM >					MapID2H;

	typedef std::set< HTREEITEM >							HandlerSet;

class CConstraintBrowserDialog : public CDialog
{

// D I A L O G   E X T E N S I O N   B Y   I M P L E M E N T O R   B E G I N
// ============================================================

	private :
		CSettingsPage 			m_pageSettings;
		CComPtr<IUnknown>		m_gotoPunk;

	public :
		void GetGotoPunk(IUnknown ** ppunk) 
		{
			*ppunk=NULL; 
			if (m_gotoPunk) 
				m_gotoPunk.CopyTo(ppunk); 
		}

		CImageList 				m_lstObjectImages;
		CImageList				m_lstStateImages;
		CImageList				m_lstCStateImages;

		CConstraintTreeCtrl		m_treeConstraints;
		CConstraintTreeCtrl		m_treeObjects;
		CConstraintTreeCtrl::Kind	m_eKindActiveTree;
		bool						m_bLoaded;

		Facade*					m_pFacade;

		CComPtr<IMgaObject>	m_spObject;
		CComPtr<IMgaObjects>	m_spObjects;

		ConstraintVectorMap		m_mapConstraints;
		ConstraintMap				m_mapConstraint;

		MapH2ID					m_mapH2ID;
		MapID2H					m_mapID2H;

		HandlerSet				m_setClicked;
		HandlerSet				m_setSelecteds;

	public :
		CConstraintBrowserDialog( Facade* pFacade, CComPtr<IMgaObject> spObject = NULL, CComPtr<IMgaObjects> spObjects = NULL, CWnd* pParent = NULL  );
		~CConstraintBrowserDialog();

	private :
		void 			SetView( CConstraintTreeCtrl::Kind eKind );

		void			LoadConstraints();
		void 			LoadKinds();
		void 			LoadSettings();
		void 			AddConstraint( OclGme::SpConstraint spConstraint );
		void 			ExpandKind( const std::string& strKind, HTREEITEM hItem );
		void 			LoadObject( CComPtr<IMgaObject> spObject, HTREEITEM hParent, const OclGme::ConstraintVector& vecConstraints );

		void 			InsertConstraint( OclGme::SpConstraint spConstraint );
		void 			RemoveConstraint( OclGme::SpConstraint spConstraint, bool bRemoveEnableInfo = true );
		void 			RefreshKind( const std::string& strKind );

		void 			RefreshRootState( HTREEITEM hItem );
		int 				GetAncestorState( HTREEITEM hItem, const std::string& strCID );
		HTREEITEM 	FindConstraintNode( HTREEITEM hItem, const std::string& strCID );
		void 			SetNewState( HTREEITEM hItem, int iNewS, bool bInheritance );
		void			SelectItem( HTREEITEM hItem, bool bSelect );
		void			PlaceWindow( CWnd& window, int iGap = -1 );

		void 			ApplyChanges();

// D I A L O G   E X T E N S I O N   B Y   I M P L E M E N T O R   E N D
// ============================================================

public :
// Dialog Data
	//{{AFX_DATA(CConstraintBrowserDialog)
	enum { IDD = IDD_BROWSER_DIALOG };
	CButton	m_btnLoad;
	CButton	m_btnApply;
	CTabCtrl	m_tabTrees;
	CButton	m_btnRemove;
	CButton	m_btnOK;
	CButton	m_btnCheck;
	CButton	m_btnCancel;
	CButton	m_btnAdd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConstraintBrowserDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConstraintBrowserDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickAdd();
	afx_msg void OnClickCancel();
	afx_msg void OnClickCheck();
	afx_msg void OnClickOK();
	afx_msg void OnClickRemove();
	afx_msg void OnSelectionChangedTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickApply();
	afx_msg void OnClickLoad();
	//}}AFX_MSG
	afx_msg void OnItemExpanding( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnCheckChanged( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnCheckChangedRecursive( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnShowItem( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnSelectionChangedTreeConstraints( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnSelectionChangedTreeObjects( NMHDR* pNMHDR, LRESULT* pResult );

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}; // namespace OclGmeCM

#endif // !defined(AFX_GMECONSTRAINTBROWSERDIALOG_H__25C4CBC0_D9A0_4003_BD98_5AB843DD921A__INCLUDED_)
