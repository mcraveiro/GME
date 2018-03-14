#if !defined(AFX_AUTOCOMBOBOX_H__688CB930_C9CC_40DA_A85B_8D7471C9DBEF__INCLUDED_)
#define AFX_AUTOCOMBOBOX_H__688CB930_C9CC_40DA_A85B_8D7471C9DBEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoComboBox window

#include "StringArrayEx.h"
#include "TreeCtrlEx.h"

class CAutoComboBox : public CComboBox
{

	// Two paralell array for the found items
	CStringArrayEx m_strResults;
	CArray<HTREEITEM,HTREEITEM> m_hResults;
	// Remembering the previous combo text 
	CString m_strPrevTxt;


	CTreeCtrlEx* m_pTreeCtrl;
// Construction
public:
	CAutoComboBox(CTreeCtrlEx*);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetSelection(HTREEITEM hItem);
	BOOL Filter(CString & strFilter);
	void Fill(CString&,BOOL bReload=TRUE);
	void OnEditKeyDown(int nVKeyCode);
	virtual ~CAutoComboBox();

	// Generated message map functions
protected:
	void OnKeyEnter();
	//{{AFX_MSG(CAutoComboBox)
	afx_msg void OnEditChange();
	afx_msg void OnSelendOk();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOCOMBOBOX_H__688CB930_C9CC_40DA_A85B_8D7471C9DBEF__INCLUDED_)
