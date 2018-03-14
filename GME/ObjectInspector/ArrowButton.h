#if !defined(AFX_ARROWBUTTON_H__53BECF4F_C424_485A_9963_BD27770FFD23__INCLUDED_)
#define AFX_ARROWBUTTON_H__53BECF4F_C424_485A_9963_BD27770FFD23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArrowButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CArrowButton window

class CArrowButton : public CButton
{
// Construction
public:
	CArrowButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArrowButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CArrowButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CArrowButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARROWBUTTON_H__53BECF4F_C424_485A_9963_BD27770FFD23__INCLUDED_)
