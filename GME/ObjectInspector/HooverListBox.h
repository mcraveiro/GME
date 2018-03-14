#if !defined(AFX_HOOVERLISTBOX_H__F5BBB7F5_7A1E_42F3_9383_37489E1BE956__INCLUDED_)
#define AFX_HOOVERLISTBOX_H__F5BBB7F5_7A1E_42F3_9383_37489E1BE956__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HooverListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHooverListBox window

class CHooverListBox : public CListBox
{

// Construction
public:
	CHooverListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHooverListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHooverListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHooverListBox)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSelChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOOVERLISTBOX_H__F5BBB7F5_7A1E_42F3_9383_37489E1BE956__INCLUDED_)
