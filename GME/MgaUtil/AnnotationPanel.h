#if !defined(AFX_ANNOTATIONPANEL_H__537F1EAA_8687_4052_AD65_ABD204453B86__INCLUDED_)
#define AFX_ANNOTATIONPANEL_H__537F1EAA_8687_4052_AD65_ABD204453B86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnnotationPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnnotationPanel window

class CAnnotationPanel : public CButton
{
// Construction
public:
	CAnnotationPanel();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnnotationPanel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnnotationPanel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnnotationPanel)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANNOTATIONPANEL_H__537F1EAA_8687_4052_AD65_ABD204453B86__INCLUDED_)
