#if !defined(AFX_CLOSUREDLG_H__48D48570_0158_4740_B202_295FDF50C394__INCLUDED_)
#define AFX_CLOSUREDLG_H__48D48570_0158_4740_B202_295FDF50C394__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClosureDlg.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CClosureDlg dialog
#include <afxcmn.h>

class CClosureDlg 
	//: public CPropertyPage
	: public CDialog
{
// Construction
public:
	CClosureDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CClosureDlg)
	enum { IDD = IDD_CLOSUREDLG };
	CButton	m_ctrlFilter;
	CButton	m_ctrlRefPortConnection;
	CButton m_ctrlIncludeAll;
	CButton m_ctrlIncludeSome;
	CButton	m_ctrlRadio1;
	CButton	m_ctrlRadio2;
	CButton	m_ctrlRadio3;
	CButton	m_ctrlLibRadio1;
	CButton	m_ctrlLibRadio2;
	CButton	m_ctrlLibRadio3;
	CButton	m_ctrlPartOfModels;
	CButton	m_ctrlPartOfFolders;
	CButton	m_ctrlContainment;
	CButton	m_ctrlOk;
	CButton	m_ctrlBrowse;
	CEdit	m_ctrlOutputFile;
	CButton	m_ctrlFolderContainment;
	BOOL	m_bConnection;
	BOOL	m_bContainment;
	BOOL	m_bFolderContainment;
	BOOL	m_bRefersTo;
	BOOL	m_bSetMember;
	BOOL	m_bMemberOfSets;
	BOOL	m_bReferredBy;
	BOOL	m_bBaseTypes;
	BOOL	m_bDerivedTypes;
	BOOL	m_bPartOfModels;
	BOOL	m_bPartOfFolders;
	CString	m_strOutputFile;
	int		m_outputOption;
	int		m_direction;
	int		m_wrappingOption;
	int		m_libraryHandling;
	int		m_includeAllKindsRadio;
	BOOL	m_bRefPortConnection;
	BOOL	m_bMarkObjs;
	CEdit	m_ctrlMarkVal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClosureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClosureDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBrowse();
	afx_msg void OnOutputtoclipboard();
	afx_msg void OnOutputtofile();
	afx_msg void OnDirectionup();
	afx_msg void OnDirectiondn();
	afx_msg void OnCheckconnection();
	afx_msg void OnCheckfoldercontainment();
	afx_msg void OnCheckcontainment();
	afx_msg void OnFilter();
	afx_msg void OnIncludeSome();
	afx_msg void OnIncludeAll();
	afx_msg void OnWrapMiniProj();
	afx_msg void OnWrapAuto();
	afx_msg void OnWrapAsIs();
	afx_msg void OnMarkClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// user added:
public:
	// indicates whether any folder is involved, so the folder containment checkbox 
	// is allowed if this value is true and the closuredirection is "Down"
	bool m_activeFolderContainment;

	bool m_includeFolders;
	bool m_includeModels;
	bool m_includeAtoms;
	bool m_includeSets;
	bool m_includeReferences;

	long calcKindMask();
	int  m_iMarkVal;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLOSUREDLG_H__48D48570_0158_4740_B202_295FDF50C394__INCLUDED_)
