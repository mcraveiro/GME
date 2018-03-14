#pragma once


// CSVNDialogPlaintext dialog

class CSVNDialogPlaintext : public CDialogEx
{
	DECLARE_DYNAMIC(CSVNDialogPlaintext)

public:
	CSVNDialogPlaintext(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSVNDialogPlaintext();

// Dialog Data
	enum { IDD = IDD_DIALOG_SVNPLAINTEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString realm;
};
