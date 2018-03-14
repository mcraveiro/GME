#if !defined(AFX_CLOSURERES_H__C9DCEADD_A5A5_451B_9EB2_4BD2B6F6C9C4__INCLUDED_)
#define AFX_CLOSURERES_H__C9DCEADD_A5A5_451B_9EB2_4BD2B6F6C9C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClosureRes.h : header file
//

#include "resource.h"
#include <vector>
#include <string>
/////////////////////////////////////////////////////////////////////////////
// ClosureRes dialog

class CClosureRes : public CDialog
{
// Construction
public:
	CClosureRes(CWnd* pParent = NULL);   // standard constructor
	std::vector< std::wstring >	m_metaKindsAndFolders;

// Dialog Data
	//{{AFX_DATA(CClosureRes)
	enum { IDD = IDD_CLOSURERES };
	CListBox	m_ctrlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClosureRes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClosureRes)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLOSURERES_H__C9DCEADD_A5A5_451B_9EB2_4BD2B6F6C9C4__INCLUDED_)
