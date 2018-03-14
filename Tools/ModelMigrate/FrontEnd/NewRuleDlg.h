#pragma once
#include "afxwin.h"


// NewRuleDlg dialog

class NewRuleDlg : public CDialog
{
	DECLARE_DYNAMIC(NewRuleDlg)

public:
	NewRuleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~NewRuleDlg();

// Dialog Data
	enum { IDD = IDD_DLG_ADDRULE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_rules;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

public:
	int m_choice;
	CString m_choiceStr;
	afx_msg void OnLbnDblclkList1();
};
