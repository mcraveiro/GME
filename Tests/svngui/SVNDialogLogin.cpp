// SVNDialogLogin.cpp : implementation file
//

#include "stdafx.h"
#include "svngui.h"
#include "SVNDialogLogin.h"
#include "afxdialogex.h"


// CSVNDialogLogin dialog

IMPLEMENT_DYNAMIC(CSVNDialogLogin, CDialogEx)

CSVNDialogLogin::CSVNDialogLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSVNDialogLogin::IDD, pParent)
	, realm(_T(""))
	, username(_T(""))
	, password(_T(""))
{

}

CSVNDialogLogin::~CSVNDialogLogin()
{
}

void CSVNDialogLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_PERMANENT, permanent);
	DDX_Text(pDX, IDC_EDIT_REALM, realm);
	DDX_Text(pDX, IDC_EDIT_USERNAME, username);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, password);
}


BEGIN_MESSAGE_MAP(CSVNDialogLogin, CDialogEx)
//	ON_BN_CLICKED(IDC_CHECK_SAVECREDS, &CSVNDialogLogin::OnBnClickedCheckSavecreds)
END_MESSAGE_MAP()


// CSVNDialogLogin message handlers


//void CSVNDialogLogin::OnBnClickedCheckSavecreds()
//{
//	// TODO: Add your control notification handler code here
//}


BOOL CSVNDialogLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_CHECK_PERMANENT)->EnableWindow(permanentEnabled);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(passwordEnabled);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
