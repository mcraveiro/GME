#if !defined(AFX_PROJECTPROPERTIESDLG_H__A0B46DC1_4836_11D4_84A2_0001031BFAA7__INCLUDED_)
#define AFX_PROJECTPROPERTIESDLG_H__A0B46DC1_4836_11D4_84A2_0001031BFAA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectPropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjectPropertiesDlg dialog

class CProjectPropertiesDlg : public CDialog
{
// Construction
public:
	CProjectPropertiesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectPropertiesDlg)
	enum { IDD = IDD_PROJECTPROPERTIES_DIALOG };
	CEdit	guid;
	CEdit	version;
	CEdit	m_parversion;
	CEdit	m_parguid;
	CEdit	m_parname;
	CEdit	modified;
	CEdit	created;
	CEdit	comment;
	CEdit	author;
	CEdit	name;
	CEdit	conn;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectPropertiesDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonParadigm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTPROPERTIESDLG_H__A0B46DC1_4836_11D4_84A2_0001031BFAA7__INCLUDED_)
