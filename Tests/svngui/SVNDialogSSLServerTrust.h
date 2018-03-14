#pragma once


// CSVNDialogSSLServerTrust dialog

class CSVNDialogSSLServerTrust : public CDialogEx
{
	DECLARE_DYNAMIC(CSVNDialogSSLServerTrust)

public:
	CSVNDialogSSLServerTrust(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSVNDialogSSLServerTrust();

// Dialog Data
	enum { IDD = IDD_DIALOG_SVNSSLSERVERTRUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString host;
	CString fingerprint;
	CString issuer;
	CString problems;
	BOOL permanent;
	virtual BOOL OnInitDialog();
	BOOL permanentEnabled;
};
