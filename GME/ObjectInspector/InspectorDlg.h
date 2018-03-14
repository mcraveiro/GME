#if !defined(AFX_INSPECTORDLG_H__23328BD1_ED69_4FFD_BD49_464E75C57493__INCLUDED_)
#define AFX_INSPECTORDLG_H__23328BD1_ED69_4FFD_BD49_464E75C57493__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InspectorDefs.h"
#include "SplitterBar.h"
#include "InspectorList.h"
#include "InspectorSheet.h"
#include "InspectorEntry.h"
#include "NameEdit.h"

// InspectorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInspectorDlg dialog

class CInspectorDlg : public CDialog
{
	friend class CObjectInspectorCtrl;
// Construction
public:
	void Refresh();

	void ShowPanel(int nPanel,bool bIsVisible);

	void SetName(const CString& strName,bool bIsReadOnly,bool bIsEnabled);
	void SetHelp(const CString& strTitle, const CString& strText);

	CString m_strName;

	CInspectorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInspectorDlg();
	CNameEdit	m_NameCtrl;
	void Reset(bool preserveScrollPos = false);
	void ShowPanel(int panel);
	void CyclePanel(bool frwd);
// Dialog Data
	//{{AFX_DATA(CInspectorDlg)
	enum { IDD = IDD_INSPECTOR_DIALOG };
	CInspectorSheet	m_inspectorSheet;
	CStatic	m_wndHelp;
	BOOL	m_forKind;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInspectorDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeInspectorSheet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnNameEditEndOK(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNameEditEndCancel(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnItemChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOpenRefered(WPARAM wParam, LPARAM lParam);
	void OnItemChangedPreference(int nItem);
	void OnItemChangedAttribute(int nItem);

protected:
	void SwitchPanel(int panel);

	CSplitterBar m_wndSplitterBar;
	CInspectorList	*m_inspectorLists[INSP_PANEL_NUM];
	CInspectorEntryList	*m_inspectorEntries[INSP_PANEL_NUM];
	CSize		 m_oldSize;
	int			 m_currentPanel;
	CFont m_BoldFont;


	bool m_bIsPanelVisible[INSP_PANEL_NUM];


	friend class CInspectorList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSPECTORDLG_H__23328BD1_ED69_4FFD_BD49_464E75C57493__INCLUDED_)
