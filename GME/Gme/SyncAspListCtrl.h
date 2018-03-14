#if !defined(AFX_SYNCASPLISTCTRL_H__99342177_D454_4980_8043_49F45C60B25B__INCLUDED_)
#define AFX_SYNCASPLISTCTRL_H__99342177_D454_4980_8043_49F45C60B25B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SyncAspListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSyncAspListCtrl window

class CSyncAspListCtrl : public CListCtrl
{
// Construction
public:
	CSyncAspListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyncAspListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	static int CALLBACK AspectCompare(LPARAM lParam1, LPARAM lParam2,  LPARAM lParamSort);
	virtual ~CSyncAspListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSyncAspListCtrl)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	static bool isAscending[1];

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNCASPLISTCTRL_H__99342177_D454_4980_8043_49F45C60B25B__INCLUDED_)
