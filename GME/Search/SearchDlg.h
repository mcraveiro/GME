#if !defined(AFX_SEARCHDLG_H__E59A9A8F_05E1_48EB_BB09_897251FD3943__INCLUDED_)
#define AFX_SEARCHDLG_H__E59A9A8F_05E1_48EB_BB09_897251FD3943__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSearchCtrl;

#include "ComHelp.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
#include <algorithm>
// SearchDlg.h : header file
//

template<typename FCO, typename FCOs>
bool RemoveZombies(CComPtr<FCOs>& results) {
	if (results == nullptr)
	{
		return false;
	}
	bool zombieFound = false;
	long position = 1; //IMgaFCOs is 1-based
	MGACOLL_ITERATE(FCO, results)
	{
		long oStatus;
		COMTHROW(MGACOLL_ITER->get_Status(&oStatus));
		if (oStatus != OBJECT_EXISTS)
		{
			COMTHROW(results->Remove(position));
			//removing the zombie causes the next item to have the same position
			//that the zombie just had, so don't advance the position counter
			zombieFound = true;
		}
		else
		{
			position++;
		}

	}MGACOLL_ITERATE_END;
	return zombieFound;
}

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog

class CSearchDropTarget : public COleDropTarget
{
public:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
};

//struct used for sending information to sort function usd by List Control

struct SortParam
{
    CListCtrl * listCtrl;
    bool ascending;
    int columnIndex;
    SortParam(CListCtrl *listCtrl,int columnIndex, bool ascending)
    {
        this->listCtrl=listCtrl;
        this->columnIndex=columnIndex;
        this->ascending=ascending;
    }
};


static int CALLBACK 
ListItemCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
     // lParamSort contains a pointer to the sort parameters.

   SortParam* sortParam = (SortParam*) lParamSort;
   CListCtrl *pListCtrl=sortParam->listCtrl;

   //get the 2 items to be compared
   CString    strItem1 = pListCtrl->GetItemText(lParam1, sortParam->columnIndex);
   CString    strItem2 = pListCtrl->GetItemText(lParam2, sortParam->columnIndex);

   if(sortParam->ascending)
       return _tcscmp(strItem1,strItem2);

   return _tcscmp(strItem2, strItem1);
}

class AutocompleteComboBox : public CComboBox
{
	CString m_previous;
	std::vector<CString> m_options;
public:
	void SetOptions(std::vector<CString>&& options)
	{
		m_options = std::move(options);
	}
	DECLARE_MESSAGE_MAP()
	void OnEditChange();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class CSearchDlg : public CDialog
{
// Construction
public:
	CSearchDlg();   // standard constructor
    ~CSearchDlg();
	BOOL OnInitDialog();

//  These functions should be implemented to work with CSearchCtl
	void RemoveAll();						// must remove all search result
	void RemoveZombies();					// must remove results belong to zombie objects
	void EnableSearch();					// enable search functions
	void DisableSearch();					// must disable search functions
	void EnableScoped( BOOL enable);

	inline CSearchCtrl* getMyParent() { return GetCtrl(); }
	void clickGo();
	void itemDblClicked();
	void itemClicked();
   
    //added -kiran
    void SaveSearchHistory();
    void LoadSearchHistory();
    void CreateSearchHistory();

    //virtual LRESULT WindowProc(UINT message,  WPARAM wParam, LPARAM lParam);
    //virtual BOOL PreTranslateMessage(MSG *pMsg);

// Dialog Data
	//{{AFX_DATA(CSearchDlg)
	enum { IDD = IDD_SEARCH_DIALOG };
	//CStatic	m_stcRefCtrl;
	CProgressCtrl	m_pgsSearch;

	CString	m_edtNameFirst;
	CString	m_edtKindNameFirst;
	CString	m_edtRoleNameFirst;
    CString	m_edtAttributeFirst;
	
    //remove
    CString	m_edtAttrValue;

    CString m_edtNameSecond;
    CString m_edtRoleNameSecond;
    CString m_edtKindNameSecond;
    CString m_edtAttributeSecond;

	BOOL	m_chkAtom;
	BOOL	m_chkFullWord;
	BOOL	m_chkMod;
	BOOL	m_chkRef;
	BOOL	m_chkSet;
	BOOL	m_chkLocate;
	BOOL	m_chkMatchCase;
	
	CButton	m_chkMatchCaseCtrl;
	CButton	m_chkFullWordCtrl;
	CButton	m_chkRefCtrl;
	CButton	m_chkAtomCtrl;
	CButton	m_chkSetCtrl;
	CButton	m_chkModCtrl;
	CComboBox	m_cmbCtrl;
	CButton	m_btnGO;
	CListCtrl	m_lstResults;
    CTreeCtrl m_treeSearchHistory;

    //first search criteria controls
	CComboBox	m_edtNameCtrlFirst;
	AutocompleteComboBox m_edtKindNameCtrlFirst;
	CComboBox	m_edtRoleNameCtrlFirst;
	CComboBox	m_edtAttributeCtrlFirst;
	
    //second search criteria controls
    CComboBox m_edtNameCtrlSecond;
    CComboBox m_edtRoleNameCtrlSecond;
    AutocompleteComboBox m_edtKindNameCtrlSecond;
    CComboBox m_edtAttributeCtrlSecond;
     
    CButton m_logicalGrp;
    CComboBox m_cmbCtrl2;
    int m_radioScope;
    BOOL m_searchResults;
    int m_radioLogical;

    //sort indicators for four result columns
    bool ascending[4];

private:
    //insert history to combobox
    void InsertHistory(CString string);

    //prepare history string by appending search term name and value pairs. value is enclose by quotes
    void PrepareHistoryString(const CString &strCriteriaName,CString & strSearchValue,HTREEITEM hParent,CString &strSearch);
    void PrepareHistoryString(const CString &strCriteriaName,int & strSearchValue,HTREEITEM hParent,CString &strSearch);
    
    //reverse of prepare extract search value from the saved string
    //string and int version with string extracting string value and int version extracting
    //integer values
    void ReadHistoryValue(const CString &strCriteriaName, CString &strHistory, CString &strValue);
    void ReadHistoryValue(const CString &strCriteriaName,CString &strHistory, int &value);
   
    //called to enter history text to combobox
    //checks first if it exists at the first index of items
    //if it does it is not inserted, else inserted
    void InsertTextToControl(CString& strSearchTerm,CComboBox& control);

    
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
    CSearchCtrl *GetCtrl() { return (CSearchCtrl*)GetParent(); }
	void BuildExtendedName(IMgaFCO *named, CString &extName);
	void BuildExtendedName(IMgaFolder *named, CString &extName);
	void DisplayResults();
    void SearchResults();
   
   	CComPtr<IMgaFCOs> results;
	CComPtr<IMgaFCO> specialSearchFCO;
public:
	void GetKinds(CComPtr<IMgaProject>& project)
	{
		std::vector<CString> kinds;
		CComPtr<IMgaMetaProject> meta;
		project->get_RootMeta(&meta);
		CComPtr<IMgaMetaFolder> root;
		meta->get_RootFolder(&root);
		CComPtr<IMgaMetaFCOs> metaFCOs;
		root->get_DefinedFCOs(&metaFCOs);
		MGACOLL_ITERATE(IMgaMetaFCO, metaFCOs)
		{
			_bstr_t name;
			MGACOLL_ITER->get_Name(name.GetAddress());
			kinds.push_back(CString(static_cast<const wchar_t*>(name)));
		} MGACOLL_ITERATE_END;
		std::sort(kinds.begin(), kinds.end());
		m_edtKindNameCtrlFirst.SetOptions(std::vector<CString>(kinds));
		m_edtKindNameCtrlSecond.SetOptions(std::move(kinds));
	}

	template<typename F>
	void ForEachSelectedFCO(F f)
	{
		POSITION pos = m_lstResults.GetFirstSelectedItemPosition();
		while (pos)
		{
			int nItem = m_lstResults.GetNextSelectedItem(pos);
			DWORD_PTR lParam = m_lstResults.GetItemData(nItem);
			CComPtr<IMgaFCO> selected;
			COMTHROW(results->get_Item(lParam + 1, &selected));
			f(selected);
		}
	}

protected:
	BOOL m_scopedCtrlEnabled; // whether to enable scoped search at all


	// Generated message map functions
	//{{AFX_MSG(CSearchDlg)
	afx_msg void OnButtonGo();
	afx_msg void OnClickListResults(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListResults(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDownListResults(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CSearchDropTarget;
	CSearchDropTarget dropTarget;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg void OnCbnSelchangeCombotype();
    afx_msg void OnNMDblclkTreeSearchHistory(NMHDR *pNMHDR, LRESULT *pResult);
    // if the connections are to be searched for
    BOOL m_chkConnection;
    BOOL m_chkSplSearch;
    afx_msg void OnCheckSplSearch();
    // reference static text ctrl
    CStatic m_stcRefCtrl;
    CButton m_chkConnCtrl;
    afx_msg void OnLvnColumnclickListresults(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonClear();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHDLG_H__E59A9A8F_05E1_48EB_BB09_897251FD3943__INCLUDED_)
