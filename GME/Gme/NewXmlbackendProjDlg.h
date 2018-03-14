#if !defined(AFX_NEWXMLBACKENDPROJDLG_H__F62C6470_808C_4392_82A1_84C3ADD22E50__INCLUDED_)
#define AFX_NEWXMLBACKENDPROJDLG_H__F62C6470_808C_4392_82A1_84C3ADD22E50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewXmlbackendProjDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CNewXmlbackendProjDlg dialog

class CNewXmlbackendProjDlg : public CDialog
{
// Construction
public:
	CNewXmlbackendProjDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewXmlbackendProjDlg)
	enum { IDD = IDD_NEW_MULTIUSER_PROJ_DLG };
	CEdit   m_svnUrlCtrl;
	CStatic m_svnUrlLabel;
	CButton m_svnBorder;
	CString	m_location;
	CString	m_projectName;
	CString m_svnUrl;
	int     m_sourceControlType;
	int     m_hashedFileStorage;
	CComboBox m_hashAlgoControl;
	//}}AFX_DATA

    CString m_connectionString;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewXmlbackendProjDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void enableSubversionControls( bool enable );

	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CNewXmlbackendProjDlg)
	afx_msg void OnButtonBrowseLoc();
	virtual void OnOK();
	afx_msg void OnSourceControlChanged();
	afx_msg void OnBnClickedCheckSplittosubdirs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWXMLBACKENDPROJDLG_H__F62C6470_808C_4392_82A1_84C3ADD22E50__INCLUDED_)
