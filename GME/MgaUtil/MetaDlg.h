#if !defined(AFX_METADLG_H__D03EC328_447B_11D4_B3F6_005004D38590__INCLUDED_)
#define AFX_METADLG_H__D03EC328_447B_11D4_B3F6_005004D38590__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MetaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMetaDlg dialog

class CMetaDlg : public CDialog
{
// Construction
public:
	CMetaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMetaDlg)
	enum { IDD = IDD_METADLG };
	CButton	m_purge;
	CButton	m_remove;
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMetaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMetaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddfile();
	afx_msg void OnRemove();
	virtual void OnOK();
	afx_msg void OnPurge();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void ResetItems();

	CComObjPtr<IMgaRegistrar> registrar;

	CString to_select;
public:
	CString name;		// dialog return data
	CString connstr;	// dialog return data 
	metadlg_enum flags;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METADLG_H__D03EC328_447B_11D4_B3F6_005004D38590__INCLUDED_)
