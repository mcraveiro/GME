#if !defined(AFX_PROPERTIESDLG_H__90EB4F81_E3E6_11D3_AEF1_99E5F548FF7A__INCLUDED_)
#define AFX_PROPERTIESDLG_H__90EB4F81_E3E6_11D3_AEF1_99E5F548FF7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertiesDlg dialog

class CPropertiesDlg : public CDialog
{
// Construction
public:
	CPropertiesDlg(IMgaFCO *fcopt,/*int nmPos,*/ CWnd* pParent = NULL);   // standard constructor

//	CGMEView *view;
	CComPtr<IMgaFCO> fco;

// Dialog Data
	//{{AFX_DATA(CPropertiesDlg)
	enum { IDD = IDD_PROPERTIES_DIALOG };
	CEdit	dep;
	CEdit	nameBox;
	CEdit	kind;
	CEdit	objectid;
	CEdit	etc;
	CEdit	aspect;
	CEdit	role;
	CEdit	type;
	CEdit	metaid;
	CString	name;
	CString	m_relid;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	bool editflag;
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertiesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetrelid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:

	CString m_sz_pre_edit_name;
	long relid;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESDLG_H__90EB4F81_E3E6_11D3_AEF1_99E5F548FF7A__INCLUDED_)
