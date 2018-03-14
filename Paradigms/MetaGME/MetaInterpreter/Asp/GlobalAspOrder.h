#if !defined(AFX_GLOBALASPORDER_H__08D873D5_01E0_43ED_82A6_DA11600879BE__INCLUDED_)
#define AFX_GLOBALASPORDER_H__08D873D5_01E0_43ED_82A6_DA11600879BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// GlobalAspOrder.h : header file
//

#include "resource.h"
#include <afxcmn.h>
#include <afxext.h>
#include "AspectRep.h"
#include "vector"


/////////////////////////////////////////////////////////////////////////////
// GlobalAspOrder dialog

class GlobalAspOrder : public CDialog
{
// Construction
public:
	GlobalAspOrder(CWnd* pParent = NULL);   // standard constructor
	void addAspects( const std::vector< AspectRep *>& oo);
	std::vector< AspectRep *> m_resultAspects;

// Dialog Data
	//{{AFX_DATA(GlobalAspOrder)
	enum { IDD = IDD_ASPECTORDER_DLG };
	CButton	m_down;
	CButton	m_up;
	CListBox	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GlobalAspOrder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	std::vector< AspectRep *> m_aspects;

	// Generated message map functions
	//{{AFX_MSG(GlobalAspOrder)
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBALASPORDER_H__08D873D5_01E0_43ED_82A6_DA11600879BE__INCLUDED_)
