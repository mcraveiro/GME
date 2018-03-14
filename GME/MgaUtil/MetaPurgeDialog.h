#if !defined(AFX_METAPURGEDIALOG_H__9C38953B_5AE0_414D_B11E_740E9173503F__INCLUDED_)
#define AFX_METAPURGEDIALOG_H__9C38953B_5AE0_414D_B11E_740E9173503F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MetaPurgeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMetaPurgeDialog dialog

class CMetaPurgeDialog : public CDialog
{
// Construction
public:
	CMetaPurgeDialog(CString &paradigmname, IMgaRegistrar *reg, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMetaPurgeDialog)
	enum { IDD = IDD_METAPURGEDIALOG };
	CListCtrl	m_list;
	int		m_delfiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMetaPurgeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	afx_msg void OnParadigmsHeader(NMHDR* pNMHDR, LRESULT* pResult);

	// Generated message map functions
	//{{AFX_MSG(CMetaPurgeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	afx_msg void OnPurge();
	afx_msg void OnSetcurrent();
	afx_msg void OnClose();
	afx_msg void OnOK();
	//}}AFX_MSG
	void OnCheckFiles();
	DECLARE_MESSAGE_MAP()
private:
	void ResetItems();
	CComPtr<IMgaRegistrar> registrar;
	CString paradigm;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METAPURGEDIALOG_H__9C38953B_5AE0_414D_B11E_740E9173503F__INCLUDED_)
