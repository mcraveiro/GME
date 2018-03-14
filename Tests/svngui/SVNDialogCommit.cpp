// SVNDialogCommit.cpp : implementation file
//

#include "stdafx.h"
#include "svngui.h"
#include "SVNDialogCommit.h"
#include "afxdialogex.h"


// CSVNDialogCommit dialog

IMPLEMENT_DYNAMIC(CSVNDialogCommit, CDialogEx)

CSVNDialogCommit::CSVNDialogCommit(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSVNDialogCommit::IDD, pParent)
	, filename(_T(""))
	, repository(_T(""))
	, revision(0)
	, logMessage(_T(""))
{

}

CSVNDialogCommit::~CSVNDialogCommit()
{
}

void CSVNDialogCommit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SVNCOMMIT_FILENAME, filename);
	DDX_Text(pDX, IDC_EDIT_SVNCOMMIT_REPO, repository);
	DDX_Text(pDX, IDC_EDIT_SVNCOMMIT_REVISION, revision);
	DDX_Text(pDX, IDC_EDIT_SVNCOMMIT_LOG, logMessage);
}


BEGIN_MESSAGE_MAP(CSVNDialogCommit, CDialogEx)
END_MESSAGE_MAP()


// CSVNDialogCommit message handlers
