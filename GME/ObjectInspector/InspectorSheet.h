#if !defined(AFX_INSPECTORSHEET_H__52CF239B_133C_4F6B_8F49_B3088F49FE9A__INCLUDED_)
#define AFX_INSPECTORSHEET_H__52CF239B_133C_4F6B_8F49_B3088F49FE9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InspectorList.h"
#include "InspectorDefs.h"

// InspectorSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInspectorSheet window

class CInspectorSheet : public CTabCtrl
{
// Construction
public:
	CInspectorSheet();

// Attributes
public:

// Operations
public:
	void SetInspectorLists(CInspectorList **inspectorLists);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectorSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInspectorSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInspectorSheet)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnItemChanged(WPARAM  wParam, LPARAM lParam);	

protected:
	CInspectorList	**m_inspectorLists;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSPECTORSHEET_H__52CF239B_133C_4F6B_8F49_B3088F49FE9A__INCLUDED_)
