// SVNDialogPlaintext.cpp : implementation file
//

#include "stdafx.h"
#include "svngui.h"
#include "SVNDialogPlaintext.h"
#include "afxdialogex.h"


// CSVNDialogPlaintext dialog

IMPLEMENT_DYNAMIC(CSVNDialogPlaintext, CDialogEx)

CSVNDialogPlaintext::CSVNDialogPlaintext(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSVNDialogPlaintext::IDD, pParent)
	, realm(_T(""))
{

}

CSVNDialogPlaintext::~CSVNDialogPlaintext()
{
}

void CSVNDialogPlaintext::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, realm);
}


BEGIN_MESSAGE_MAP(CSVNDialogPlaintext, CDialogEx)
END_MESSAGE_MAP()


// CSVNDialogPlaintext message handlers
