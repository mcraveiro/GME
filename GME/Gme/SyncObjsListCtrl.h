#if !defined(AFX_SYNCOBJSLISTCTRL_H__E9398C86_76DF_478F_B4BC_E58F84A643B7__INCLUDED_)
#define AFX_SYNCOBJSLISTCTRL_H__E9398C86_76DF_478F_B4BC_E58F84A643B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SyncObjsListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSyncObjsListCtrl window

class CSyncObjsListCtrl : public CListCtrl
{
// Construction
public:
	CSyncObjsListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyncObjsListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	static int CALLBACK ObjsCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	virtual ~CSyncObjsListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSyncObjsListCtrl)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	static bool isAscending[3];

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNCOBJSLISTCTRL_H__E9398C86_76DF_478F_B4BC_E58F84A643B7__INCLUDED_)
