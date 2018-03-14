// FrontEndDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "RuleListCtrl.h"
#include "afxwin.h"

// CFrontEndDlg dialog
class CFrontEndDlg : public CDialog
{
// Construction
public:
	CFrontEndDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FRONTEND_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();


// Implementation
protected:
	void closeDlg( int pResult = 0);
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnLbnDblclkList2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton2();

	DECLARE_MESSAGE_MAP()

public:
	RuleListCtrl m_list;
	CListBox m_choice;

};
