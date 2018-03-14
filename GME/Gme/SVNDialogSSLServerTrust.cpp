// SVNDialogSSLServerTrust.cpp : implementation file
//

#include "stdafx.h"
#include "SVNDialogSSLServerTrust.h"
#include "afxdialogex.h"


// CSVNDialogSSLServerTrust dialog

IMPLEMENT_DYNAMIC(CSVNDialogSSLServerTrust, CDialogEx)

CSVNDialogSSLServerTrust::CSVNDialogSSLServerTrust(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSVNDialogSSLServerTrust::IDD, pParent)
	, host(_T(""))
	, fingerprint(_T(""))
	, issuer(_T(""))
	, problems(_T(""))
	, permanent(FALSE)
	, permanentEnabled(TRUE)
{

}

CSVNDialogSSLServerTrust::~CSVNDialogSSLServerTrust()
{
}

void CSVNDialogSSLServerTrust::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HOST, host);
	DDX_Text(pDX, IDC_EDIT_FINGERPRINT, fingerprint);
	DDX_Text(pDX, IDC_EDIT_ISSUER, issuer);
	DDX_Text(pDX, IDC_EDIT_PROBLEMS, problems);
	DDX_Check(pDX, IDC_CHECK_PERMANENT, permanent);
}


BEGIN_MESSAGE_MAP(CSVNDialogSSLServerTrust, CDialogEx)
END_MESSAGE_MAP()


// CSVNDialogSSLServerTrust message handlers


BOOL CSVNDialogSSLServerTrust::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_CHECK_PERMANENT)->EnableWindow(permanentEnabled);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
