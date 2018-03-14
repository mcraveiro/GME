#pragma once

#include "resource.h"

// AlterNmspDlg dialog

class AlterNmspDlg : public CDialog
{
	DECLARE_DYNAMIC(AlterNmspDlg)

public:
	AlterNmspDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AlterNmspDlg();

// Dialog Data
	enum { IDD = IDD_ALTERNMSPDLG };
	CEdit	m_edtPrefix;
	CEdit	m_edtTrunc;
	CButton m_chkAlterSticky;
	CButton m_radAlter0;
	CButton m_radAlter1;
	CButton m_radAlter2;
	CButton m_radAlter3;

	CString m_strPrefix;
	CString m_strTrunc;
	int		m_vRadAlter;
	BOOL	m_bAlterSticky;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedRadAlter0();
	afx_msg void OnBnClickedRadAlter1();
	afx_msg void OnBnClickedRadAlter2();
	afx_msg void OnBnClickedRadAlter3();

public:
	//CString m_storedStrPrefix;
	//CString m_storedStrTrunc;
	//int     m_storedAltOption;
	//BOOL    m_storedAltSticky;

	afx_msg void OnBnClickedButton1();
};
