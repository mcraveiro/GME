#if !defined(AFX_CLOSUREKINDSEL_H__E19FB4C7_A578_4BD4_95D3_D632A022A13E__INCLUDED_)
#define AFX_CLOSUREKINDSEL_H__E19FB4C7_A578_4BD4_95D3_D632A022A13E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClosureKindSel.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CClosureKindSel dialog

class CClosureKindSel : public CDialog
{
// Construction
public:
	CClosureKindSel(bool fld, bool mdl, bool atm, bool set, bool ref, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CClosureKindSel)
	enum { IDD = IDD_CLOSUREKINDSEL };
	BOOL	m_folder;
	BOOL	m_model;
	BOOL	m_atom;
	BOOL	m_set;
	BOOL	m_ref;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClosureKindSel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	/*bool m_fld;
	bool m_mdl;
	bool m_atm;
	bool m_set;
	bool m_ref;*/
	// Generated message map functions
	//{{AFX_MSG(CClosureKindSel)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLOSUREKINDSEL_H__E19FB4C7_A578_4BD4_95D3_D632A022A13E__INCLUDED_)
