#pragma once


// OptionsDlg dialog

class OptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(OptionsDlg)

public:
	OptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~OptionsDlg();

// Dialog Data
	enum { IDD = IDD_DLG_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CString m_dir;
	CString m_scr;
	CString m_append;
	CString m_outputDir;
	CString m_intermediateFilesDir;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton6();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
};
