#if !defined(AFX_CLOSURESMART_H__17980AEC_7C70_49EC_80A0_CF0733E7FF6E__INCLUDED_)
#define AFX_CLOSURESMART_H__17980AEC_7C70_49EC_80A0_CF0733E7FF6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

// ClosureSmart.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClosureSmart dialog

class CClosureSmart 
	: public CDialog
	//: public CPropertyPage
{
// Construction
public:
	CClosureSmart(CWnd* pParent = NULL);   // standard constructor
	bool m_disable;

// Dialog Data
	//{{AFX_DATA(CClosureSmart)
	enum { IDD = IDD_CLOSURESMARTCP };
	BOOL	m_bConns;
	BOOL	m_bRefs;
	BOOL	m_bSets;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClosureSmart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClosureSmart)
	virtual BOOL OnInitDialog();
	afx_msg void OnInvert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CButton m_atomsAndModels;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLOSURESMART_H__17980AEC_7C70_49EC_80A0_CF0733E7FF6E__INCLUDED_)
