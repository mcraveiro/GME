// AttrGlobalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "AttrGlobalDlg.h"


// AttrGlobalDlg dialog

IMPLEMENT_DYNAMIC(AttrGlobalDlg, CDialog)
AttrGlobalDlg::AttrGlobalDlg( const CString& pGlobal, const CString& pOwner, CWnd* pParent /*=NULL*/)
	: CDialog(AttrGlobalDlg::IDD, pParent)
	, m_iGlobalLocal( pGlobal=="1"?0:1)
	, m_ownerKind( pOwner)
{
}

AttrGlobalDlg::~AttrGlobalDlg()
{
}

void AttrGlobalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_iGlobalLocal);
	DDX_Text(pDX, IDC_EDIT3, m_ownerKind);
}


BEGIN_MESSAGE_MAP(AttrGlobalDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
END_MESSAGE_MAP()


// AttrGlobalDlg message handlers

void AttrGlobalDlg::OnBnClickedRadio1()
{
	if( GetDlgItem( IDC_EDIT3)) GetDlgItem( IDC_EDIT3)->EnableWindow( FALSE);
}

void AttrGlobalDlg::OnBnClickedRadio2()
{
	if( GetDlgItem( IDC_EDIT3)) GetDlgItem( IDC_EDIT3)->EnableWindow( TRUE);
}

BOOL AttrGlobalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( m_iGlobalLocal == 0 && GetDlgItem( IDC_EDIT3)) GetDlgItem( IDC_EDIT3)->EnableWindow( FALSE); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void AttrGlobalDlg::OnOK()
{
	UpdateData( TRUE);

	if( m_iGlobalLocal == 1 && m_ownerKind == "")
	{
		AfxMessageBox( "No owner specified for local attribute!", MB_ICONSTOP);
	}
	else
	{
		EndDialog( IDOK);
		DestroyWindow();
	}
}

CString AttrGlobalDlg::getGlobal() const
{
	return (m_iGlobalLocal == 0)?"1":"0";
}

const CString& AttrGlobalDlg::getOwner() const
{
	return m_ownerKind;
}

