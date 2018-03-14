#pragma once
#include "resource.h"

// CSvnTestDlg dialog

class CSvnTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CSvnTestDlg)

public:
	CSvnTestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSvnTestDlg();

// Dialog Data
	enum { IDD = IDD_TESTRESULTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CEdit m_field;
	CButton m_btn;

public:
	void setContent( const CString& p_cont);

protected:
	CString     m_defContent;
public:
	virtual BOOL OnInitDialog();
};
