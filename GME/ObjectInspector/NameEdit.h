#if !defined(AFX_NAMEEDIT_H__C84128F3_F642_462D_84EE_2AA5ACA926B8__INCLUDED_)
#define AFX_NAMEEDIT_H__C84128F3_F642_462D_84EE_2AA5ACA926B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NameEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNameEdit window

class CNameEdit : public CEdit
{
// Construction
public:
	CNameEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameEdit)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNameEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNameEdit)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAMEEDIT_H__C84128F3_F642_462D_84EE_2AA5ACA926B8__INCLUDED_)
