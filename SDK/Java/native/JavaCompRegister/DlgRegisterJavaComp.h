#if !defined(AFX_DLGREGISTERJAVACOMP_H__B760E032_2E13_40A1_8ACB_DC5A966081E8__INCLUDED_)
#define AFX_DLGREGISTERJAVACOMP_H__B760E032_2E13_40A1_8ACB_DC5A966081E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRegisterJavaComp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRegisterJavaComp dialog

class CDlgRegisterJavaComp : public CDialog
{
// Construction
public:
	CDlgRegisterJavaComp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRegisterJavaComp)
	enum { IDD = IDD_DIALOG_ADDNEWCOMP };
	CString	m_strClass;
	CString	m_strClassPath;
	CString	m_strName;
	CString	m_strGuid;
	CString	m_strProgid;
	CString	m_strDesc;
	CString	m_strParadigm;
	CString	m_strMenu;
	BOOL	m_boolSystemwide;
	//}}AFX_DATA

    GUID    m_guid;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRegisterJavaComp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRegisterJavaComp)
	virtual void OnOK();
	afx_msg void OnChangeEditName();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREGISTERJAVACOMP_H__B760E032_2E13_40A1_8ACB_DC5A966081E8__INCLUDED_)
