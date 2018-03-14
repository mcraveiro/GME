#pragma once
#include "afxcmn.h"
#include "RuleListCtrl.h"
#include "afxwin.h"


// RuleEditorDlg dialog

class RuleEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(RuleEditorDlg)

public:
	RuleEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~RuleEditorDlg();

// Dialog Data
	enum { IDD = IDD_DLG_RULEEDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedGenerate();
	afx_msg void OnBnClickedGeneratesel();
	afx_msg void OnBnClickedAddrule();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLvnKeydownRulelist(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	void closeDlg( int pResult);
	void onReturn();
	void onFileNew();
	void onFileLoad();
	void onFileSave();
	void onFileClose();

public:
	RuleListCtrl m_ruleList;
	CString m_lastTarget;
	bool m_closed;

};
