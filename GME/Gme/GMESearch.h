//{{AFX_INCLUDES()
#include "search.h"
//}}AFX_INCLUDES
#if !defined(AFX_GMESEARCH_H__6DF3FCDB_48F7_4076_A721_31A27500E019__INCLUDED_)
#define AFX_GMESEARCH_H__6DF3FCDB_48F7_4076_A721_31A27500E019__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GMESearch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGMESearch dialog

class CGMESearch :  public CDockablePane
{
// Construction
public:
	CGMESearch();   // standard constructor
	virtual ~CGMESearch() { theInstance = NULL; }

// Dialog Data
	//{{AFX_DATA(CGMESearch)
	enum { IDD = IDD_SEARCH_DIALOG };
	CSearch	m_search;
	//}}AFX_DATA

	static CGMESearch *theInstance;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMESearch)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGMESearch)
	afx_msg void OnClickMgaObjectSearchctrl(LPUNKNOWN mgaObject);
	afx_msg void OnDblClickMgaObjectSearchctrl(LPUNKNOWN mgaObject);
	afx_msg void OnWantToBeClosedSearchctrl();
	afx_msg void OnLocateMgaObjectSearchctrl(LPCTSTR mgaObjectId);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetProject(CComPtr<IMgaProject>& mgaProject);
	void SetSelMgaObjects(CComPtr<IMgaObjects> p_mgaObjects);
	void CloseProject();
	void ShowObject(LPUNKNOWN selected);
	void SelectObject(LPUNKNOWN mgaObject);
	void LocateObject(LPCTSTR mgaObjectId);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMESEARCH_H__6DF3FCDB_48F7_4076_A721_31A27500E019__INCLUDED_)
