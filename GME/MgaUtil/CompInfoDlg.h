#if !defined(AFX_COMPINFODLG_H_INCLUDED_)
#define AFX_COMPINFODLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCompInfoDlg dialog

class CCompInfoDlg : public CDialog
{
	CComPtr<IMgaRegistrar> registrar;
// Construction
public:
	CCompInfoDlg(IMgaRegistrar *reg, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCompInfoDlg)
	enum { IDD = IDD_COMPINFODLG };
	CComboBox	m_execeng;
	CString	m_name;
	CString	m_paradigm;
	CString	m_engine;
	CString	m_filename;
	CString	m_description;
	CString	m_progid;
	CString	m_version;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public: 
	CString engine;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCompInfoDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPINFODLG_H_INCLUDED_)
