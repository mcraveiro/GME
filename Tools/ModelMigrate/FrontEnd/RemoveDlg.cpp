// SimpleRepl.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "RemoveDlg.h"
#include ".\removedlg.h"


// RemoveDlg dialog

IMPLEMENT_DYNAMIC(RemoveDlg, CDialog)
RemoveDlg::RemoveDlg( bool changeTitle, CWnd* pParent /*=NULL*/)
	: CDialog(RemoveDlg::IDD, pParent)
	, m_name(_T(""))
	, m_title( changeTitle?_T("Remove Global Attribute"):_T("Remove Kind"))
{
}

RemoveDlg::~RemoveDlg()
{
}

void RemoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_name);
}


BEGIN_MESSAGE_MAP(RemoveDlg, CDialog)
END_MESSAGE_MAP()
// RemoveDlg message handlers

const CString& RemoveDlg::getPar1() const
{
	return m_name;
}

void RemoveDlg::init( const CString& par1)
{
	m_name = par1;
}


BOOL RemoveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText( (LPCTSTR) m_title);

	return TRUE;
}
