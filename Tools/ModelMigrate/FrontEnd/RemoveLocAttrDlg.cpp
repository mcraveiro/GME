// SimpleRepl.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "RemoveLocAttrDlg.h"


// RemoveLocalAttrDlg dialog

IMPLEMENT_DYNAMIC(RemoveLocalAttrDlg, CDialog)
RemoveLocalAttrDlg::RemoveLocalAttrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(RemoveLocalAttrDlg::IDD, pParent)
	, m_attr(_T(""))
	, m_owner(_T(""))
{
}

RemoveLocalAttrDlg::~RemoveLocalAttrDlg()
{
}

void RemoveLocalAttrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_attr);
	DDX_Text(pDX, IDC_EDIT2, m_owner);
}


BEGIN_MESSAGE_MAP(RemoveLocalAttrDlg, CDialog)
END_MESSAGE_MAP()
// RemoveLocalAttrDlg message handlers

const CString& RemoveLocalAttrDlg::getPar1() const
{
	return m_attr;
}

const CString& RemoveLocalAttrDlg::getPar2() const
{
	return m_owner;
}

void RemoveLocalAttrDlg::init( const CString& par1, const CString& par2)
{
	m_attr = par1;
	m_owner = par2;
}

