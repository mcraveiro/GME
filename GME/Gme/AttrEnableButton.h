#if !defined(AFX_ATTRENABLEBUTTON_H__5FEE3D23_E89B_11D3_AEF1_444553540000__INCLUDED_)
#define AFX_ATTRENABLEBUTTON_H__5FEE3D23_E89B_11D3_AEF1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AttrEnableButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttrEnableButton window

class CGuiMetaAttribute;

class CAttrEnableButton : public CButton
{
// Construction
public:
	CAttrEnableButton(CGuiMetaAttribute *metaAttr);

// Attributes
public:
	bool on;
	CGuiMetaAttribute *guiMetaAttr;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttrEnableButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAttrEnableButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAttrEnableButton)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRENABLEBUTTON_H__5FEE3D23_E89B_11D3_AEF1_444553540000__INCLUDED_)
