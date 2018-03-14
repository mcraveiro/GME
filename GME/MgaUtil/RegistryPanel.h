#if !defined(AFX_REGISTRYPANEL_H__0B6B960F_DCC5_4677_868C_115C74AC150E__INCLUDED_)
#define AFX_REGISTRYPANEL_H__0B6B960F_DCC5_4677_868C_115C74AC150E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegistryPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegistryPanel window

class CRegistryPanel : public CButton
{
// Construction
public:
	CRegistryPanel();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegistryPanel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRegistryPanel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRegistryPanel)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTRYPANEL_H__0B6B960F_DCC5_4677_868C_115C74AC150E__INCLUDED_)
