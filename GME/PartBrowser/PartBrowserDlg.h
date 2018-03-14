#if !defined(AFX_PARTBROWSERDLG_H__6BE75806_C869_11D3_91EA_00104B98EAD9__INCLUDED_)
#define AFX_PARTBROWSERDLG_H__6BE75806_C869_11D3_91EA_00104B98EAD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartBrowserDlg.h : header file
//

#include "StdAfx.h"
#include "PartBrowserPaneFrame.h"

/////////////////////////////////////////////////////////////////////////////
// CPartBrowserDlg dialog

class CPartBrowserCtrl;

class CPartBrowserDlg : public CDialog
{
// Construction
public:
	CPartBrowserDlg(CWnd* pParent = NULL);   // standard constructor

public:
// Dialog Data
	//{{AFX_DATA(CPartBrowserDlg)
	enum { IDD = IDD_PARTBROWSER_DIALOG };
	CTabCtrl		tab;
	//}}AFX_DATA
	CPartBrowserPaneFrame	partFrame;

	CComPtr<IMgaMetaModel>	mgaMetaModel;
	CComPtr<IMgaProject>	mgaProject;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartBrowserDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void SetCurrentProject(CComPtr<IMgaProject> project);
	void SetMetaModel(CComPtr<IMgaMetaModel> meta);
	void SetBgColor(COLORREF bgColor);
	void ChangeAspect(long ind);
	void CycleAspect();
	void SetTabs();
	void Resize();
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPartBrowserDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeAspectTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SendAspectChange(long index);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTBROWSERDLG_H__6BE75806_C869_11D3_91EA_00104B98EAD9__INCLUDED_)
