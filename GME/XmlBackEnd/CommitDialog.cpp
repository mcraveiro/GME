// CommitDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "CommitDialog.h"


// CCommitDialog dialog

IMPLEMENT_DYNAMIC(CCommitDialog, CDialog)

CCommitDialog::CCommitDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCommitDialog::IDD, pParent)
{

}

CCommitDialog::~CCommitDialog()
{
}

void CCommitDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_commentEdit);
}


BEGIN_MESSAGE_MAP(CCommitDialog, CDialog)
END_MESSAGE_MAP()


// CCommitDialog message handlers
void CCommitDialog::OnOK() 
{
	CString commentLine;
	for (int i=0; i < m_commentEdit.GetLineCount(); i++) {
		int len = m_commentEdit.LineLength(m_commentEdit.LineIndex(i));
		len = max(len, sizeof(WORD)/sizeof(TCHAR)); // first WORD is used to store the max length internally...
		m_commentEdit.GetLine(i, commentLine.GetBuffer(len), len);
		commentLine.ReleaseBuffer(len);
		m_comment += commentLine + _T("\n");
	}
   CDialog::OnOK();
}
