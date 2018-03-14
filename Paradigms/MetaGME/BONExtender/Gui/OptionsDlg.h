#if !defined(AFX_OPTIONSDLG_H__0A217994_9D2A_429E_B244_4CD2D8F74A26__INCLUDED_)
#define AFX_OPTIONSDLG_H__0A217994_9D2A_429E_B244_4CD2D8F74A26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OptionsDlg dialog
#include "resource.h"

class OptionsDlg : public CDialog
{
// Construction
public:
	OptionsDlg(CWnd* pParent = NULL);   // standard constructor
	//CString m_defName;
	void doInit();

// Dialog Data
	//{{AFX_DATA(OptionsDlg)
	enum { IDD = IDD_DIALOG2 };
	CButton	m_ctrlVersion60;
	CButton	m_ctrlButton3;
	CButton	m_ctrlButton4;
	CEdit	m_ctrlVisitorSource;
	CEdit	m_ctrlVisitorHeader;
	CButton	m_ctrlButton5;
	CEdit	m_ctrlPrevHeaderName;
	CString	m_headerName;
	CString	m_sourceName;
	CString	m_visitorHeaderName;
	CString	m_visitorSourceName;
	CString	m_prevHeaderName;
	CString m_namespaceName;
	BOOL	m_bVisitor;
	BOOL	m_bParse;
	BOOL	m_bInit;
	BOOL	m_bFinalize;
	BOOL	m_bAcceptTrave;
	BOOL	m_bAcceptSpeci;
	int		m_whichStyle;
	BOOL	m_version60;
	int		m_visitorSignature;
	CButton m_ctrlSpAccRetVoid;
	CButton m_ctrlSpAccRetBool;
	int		m_specAcceptRetVal;
	int		m_methodOfOutput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OptionsDlg)
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheck5();
	afx_msg void OnCheck34();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheck6();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__0A217994_9D2A_429E_B244_4CD2D8F74A26__INCLUDED_)
