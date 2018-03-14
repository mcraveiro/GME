#pragma once


// CSVNDialogLogin dialog

class CSVNDialogLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CSVNDialogLogin)

public:
	CSVNDialogLogin(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSVNDialogLogin();

// Dialog Data
	enum { IDD = IDD_DIALOG_SVNLOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnBnClickedCheckSavecreds();
	BOOL permanent;
	CString realm;
	CString username;
	CString password;
	BOOL permanentEnabled;
	BOOL passwordEnabled;
	virtual BOOL OnInitDialog();
};
