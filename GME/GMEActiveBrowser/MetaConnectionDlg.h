#if !defined(AFX_METACONNECTIONDLG_H__D9CF8796_999D_45DF_ACF9_92DAA165D544__INCLUDED_)
#define AFX_METACONNECTIONDLG_H__D9CF8796_999D_45DF_ACF9_92DAA165D544__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MetaConnectionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMetaConnectionDlg dialog

class CMetaConnectionDlg : public CDialog
{
// Construction
public:
	CMetaConnectionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMetaConnectionDlg)
	enum { IDD = IDD_META_CONNECTION_DIALOG };
	CString	m_strConnectionProperties;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMetaConnectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMetaConnectionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METACONNECTIONDLG_H__D9CF8796_999D_45DF_ACF9_92DAA165D544__INCLUDED_)
