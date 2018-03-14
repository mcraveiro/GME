#if !defined(AFX_ATTACHLIBDLG_H__E31DD301_7B06_493A_A336_3988CDD1F2DF__INCLUDED_)
#define AFX_ATTACHLIBDLG_H__E31DD301_7B06_493A_A336_3988CDD1F2DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AttachLibDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttachLibDlg dialog

class CAttachLibDlg : public CDialog
{
// Construction
public:
	CString m_strCaption;
	CString m_strParentConnection;
	CString relativePath;
	CString currentMgaPath;
	CAttachLibDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAttachLibDlg)
	enum { IDD = IDD_ATTACH_LIB_DIALOG };
	CString	m_strConnString;
	BOOL    m_bOptimized;
	BOOL    m_bRelativePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttachLibDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAttachLibDlg)
	virtual void OnOK();
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTACHLIBDLG_H__E31DD301_7B06_493A_A336_3988CDD1F2DF__INCLUDED_)
