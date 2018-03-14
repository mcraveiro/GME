#if !defined(AFX_REGISTRYTREE_H__E072C51F_DB18_49A9_AE5A_76015F6779B4__INCLUDED_)
#define AFX_REGISTRYTREE_H__E072C51F_DB18_49A9_AE5A_76015F6779B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegistryTree.h : header file
//

#include "RegBrwNode.h"

/////////////////////////////////////////////////////////////////////////////
// CRegistryTree window

class CRegistryTree : public CTreeCtrl
{
// Construction
public:
	CRegistryTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegistryTree)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	HTREEITEM m_cntxSelected;
	void RemoveSubTree(HTREEITEM hItem);
	virtual ~CRegistryTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRegistryTree)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCntxAddnode();
	afx_msg void OnCntxClearnode();
	afx_msg void OnCntxRemoveall();
	afx_msg void OnCntxRemovetree();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCntxRenamenode();
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTRYTREE_H__E072C51F_DB18_49A9_AE5A_76015F6779B4__INCLUDED_)
