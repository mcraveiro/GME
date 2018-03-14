// AttrNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "AttrNameDlg.h"
#include ".\attrnamedlg.h"
#include "AttrGlobalDlg.h"

// AttrNameDlg dialog

IMPLEMENT_DYNAMIC(AttrNameDlg, CDialog)
AttrNameDlg::AttrNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AttrNameDlg::IDD, pParent)
	, m_oldV(_T(""))
	, m_newV(_T(""))
	, m_global(_T("1"))
	, m_owner(_T(""))
{
}

AttrNameDlg::~AttrNameDlg()
{
}

void AttrNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_oldV);
	DDX_Text(pDX, IDC_EDIT2, m_newV);
}


BEGIN_MESSAGE_MAP(AttrNameDlg, CDialog)
	ON_BN_CLICKED(IDGLOBAL, OnBnClickedGlobal)
END_MESSAGE_MAP()
// AttrNameDlg message handlers

const CString& AttrNameDlg::getPar1() const
{
	return m_oldV;
}

const CString& AttrNameDlg::getPar2() const
{
	return m_newV;
}

const CString& AttrNameDlg::getPar3() const
{
	return m_global;
}

const CString& AttrNameDlg::getPar4() const
{
	return m_owner;
}

void AttrNameDlg::init( const CString& par1, const CString& par2, const CString& par3, const CString& par4)
{
	m_oldV = par1;
	m_newV = par2;
	m_global = par3;
	m_owner = par4;
}

BOOL AttrNameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void AttrNameDlg::OnBnClickedGlobal()
{
	AttrGlobalDlg dlg( m_global, m_owner);
	if( dlg.DoModal() == IDOK)
	{
		m_global = dlg.getGlobal();
		m_owner  = dlg.getOwner();
	}
}
