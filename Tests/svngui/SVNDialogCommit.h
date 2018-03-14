#pragma once


// CSVNDialogCommit dialog

class CSVNDialogCommit : public CDialogEx
{
	DECLARE_DYNAMIC(CSVNDialogCommit)

public:
	CSVNDialogCommit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSVNDialogCommit();

// Dialog Data
	enum { IDD = IDD_DIALOG_SVNCOMMIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString filename;
	CString repository;
	long revision;
	CString logMessage;
};
