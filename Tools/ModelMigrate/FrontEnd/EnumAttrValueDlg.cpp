// EnumAttrValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "EnumAttrValueDlg.h"
#include ".\enumattrvaluedlg.h"
#include "AttrGlobalDlg.h"


// EnumAttrValueDlg dialog

IMPLEMENT_DYNAMIC(EnumAttrValueDlg, CDialog)
EnumAttrValueDlg::EnumAttrValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EnumAttrValueDlg::IDD, pParent)
	, m_enumAttr(_T(""))
	, m_newEnumV(_T(""))
	, m_global(_T("1"))
	, m_owner(_T(""))
{
}

EnumAttrValueDlg::~EnumAttrValueDlg()
{
}

void EnumAttrValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_enumAttr);
	DDX_Text(pDX, IDC_EDIT2, m_oldEnumV);
	DDX_Text(pDX, IDC_EDIT3, m_newEnumV);
}


BEGIN_MESSAGE_MAP(EnumAttrValueDlg, CDialog)
	ON_BN_CLICKED(IDGLOBAL, OnBnClickedGlobal)
END_MESSAGE_MAP()

// EnumAttrValueDlg message handlers

const CString& EnumAttrValueDlg::getPar1() const
{
	return m_enumAttr;
}

const CString& EnumAttrValueDlg::getPar2() const
{
	return m_oldEnumV;
}

const CString& EnumAttrValueDlg::getPar3() const
{
	return m_newEnumV;
}

const CString& EnumAttrValueDlg::getPar4() const
{
	return m_global;
}

const CString& EnumAttrValueDlg::getPar5() const
{
	return m_owner;
}

void EnumAttrValueDlg::init( const CString& par1, const CString& par2, const CString& par3, const CString& par4, const CString& par5)
{
	m_enumAttr = par1;
	m_oldEnumV = par2;
	m_newEnumV = par3;
	m_global   = par4;
	m_owner    = par5;
}

void EnumAttrValueDlg::OnBnClickedGlobal()
{
	AttrGlobalDlg dlg( m_global, m_owner);
	if( dlg.DoModal() == IDOK)
	{
		m_global = dlg.getGlobal();
		m_owner  = dlg.getOwner();
	}
}
