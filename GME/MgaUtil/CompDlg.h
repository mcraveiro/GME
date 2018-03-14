#if !defined(AFX_COMPDLG_H__A649CFE3_4781_11D4_B3F9_005004D38590__INCLUDED_)
#define AFX_COMPDLG_H__A649CFE3_4781_11D4_B3F9_005004D38590__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompDlg.h : header file
//
#include <fstream> //<fstream.h>

/////////////////////////////////////////////////////////////////////////////
// CCompDlg dialog

class CCompDlg : public CDialog
{
// Construction
	CImageList iconlist, sticonlist;
	CString to_select;
public:
	CCompDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCompDlg)
	enum { IDD = IDD_COMPDLG };
	CButton	m_enable_disable;
	CButton	m_remove;
	CButton m_install;
	CListCtrl	m_list;
	int		m_accessmode;
	int		m_dispmode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCompDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRemove();
	afx_msg void OnInstall();
	afx_msg void OnActivedisp();
	afx_msg void OnActiveInactive();
	afx_msg void OnAllcomps();
	afx_msg void OnEnableDisable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void RefreshShieldIcons();
	void ResetItems();
	void RegisterDll(const CString &path);
	void RegisterPattern(const CString &path);
	void RegisterUnknownFile(const CString &path);
	void UpdateEnableDisable();

	CComPtr<IMgaRegistrar> m_elevatedRegistrar;
	HRESULT GetElevatedRegistrar(IMgaRegistrar** registrar);

public:
	CString paradigm;
	componenttype_enum type;
	CComVariant parameter;
	CString progid;
	CString onOKoper;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedRadiosys();
	afx_msg void OnBnClickedRadiouser();
	afx_msg void OnBnClickedRadioboth();
	afx_msg void OnAllCompsHeader(NMHDR* pNMHDR, LRESULT* pResult);
	static int CALLBACK CCompDlg::SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

private:
	bool	firstResize;
	CRect	lastRect;
	CRect	GetWindowSizeFromResource(void) const;
	void	MoveControl(int nID, int offsetX, int offsetY, int deltaWidth, int deltaHeight,
						bool defer = false, HDWP* pdwp = NULL);
public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPDLG_H__A649CFE3_4781_11D4_B3F9_005004D38590__INCLUDED_)
