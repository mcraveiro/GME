// CredentialDlg.cpp : implementation file
//

#include "stdafx.h"
#include ".\CredentialDlg.h"


// CCredentialDlg dialog

IMPLEMENT_DYNAMIC(CCredentialDlg, CDialog)
CCredentialDlg::CCredentialDlg( bool p_userNameOnly, bool p_visibleMaySave, bool p_maySave, const std::string& p_uName, const char* p_realmStr, CWnd* pParent /*=NULL*/)
	: CDialog(CCredentialDlg::IDD, pParent)
	, m_userNameOnly( p_userNameOnly)
	, m_visibleMaySave( p_visibleMaySave)
	, m_maySave( p_maySave)
	, m_suggestedName( p_uName)
	, m_realmStrPtr( p_realmStr)
	, m_leftPad( 20)
	, m_rightPad( 20)
{
}

CCredentialDlg::~CCredentialDlg()
{
}

std::string CCredentialDlg::name()
{
	return m_resName;
}

std::string CCredentialDlg::word()
{
	return m_resWord;
}

bool CCredentialDlg::maySave()
{
	return m_resMaySave;
}

void CCredentialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAYBESAVED, m_btnMaySave);
	DDX_Control(pDX, IDC_EDITNAME, m_edtName);
	DDX_Control(pDX, IDC_EDITWORD, m_edtWord);
	DDX_Control(pDX, IDC_EDIT_REALM, m_edtRealm);
	DDX_Control(pDX, IDC_STATIC1, m_msgAtTheTop);
}


BEGIN_MESSAGE_MAP(CCredentialDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCredentialDlg message handlers

BOOL CCredentialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edtName.SetWindowText( m_suggestedName.c_str());
	m_edtWord.EnableWindow( !m_userNameOnly);

	if( m_userNameOnly)
		m_msgAtTheTop.SetWindowText( "Please specify your username:");
	else if( m_realmStrPtr)
	{
		m_edtRealm.ShowWindow( SW_SHOW);
		m_edtRealm.SetWindowText( CString( "Realm: ") + m_realmStrPtr);//m_msgAtTheTop.SetWindowText( CString( "Please authenticate yourself for realm: \n") + m_realmStrPtr);
	}

	m_btnMaySave.ShowWindow( m_visibleMaySave? SW_SHOW: SW_HIDE);
	m_btnMaySave.EnableWindow( m_maySave);
	// maySave checked initially (if allowed)
	m_btnMaySave.SetCheck( m_maySave? BST_CHECKED: BST_UNCHECKED);

	// size related calc
	CRect rm_rect;
	if( m_edtRealm.GetSafeHwnd()) {
		m_edtRealm.GetWindowRect( &rm_rect);
		ScreenToClient( &rm_rect);

		CRect win_rect;
		GetWindowRect( &win_rect);
		ScreenToClient( &win_rect);

		// initing m_leftPad, m_rightPad with the initial lf and rt margins
		m_leftPad = rm_rect.left - win_rect.left;
		m_rightPad = win_rect.right - rm_rect.right;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCredentialDlg::OnBnClickedOk()
{
	CString res;
	
	m_edtName.GetWindowText( res);
	m_resName = (LPCTSTR) res;

	m_edtWord.GetWindowText( res);
	m_resWord = (LPCTSTR) res;

	m_resMaySave = m_btnMaySave.GetCheck() == BST_CHECKED;

	OnOK();
}

void CCredentialDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CButton* ok_btn = (CButton*) GetDlgItem( IDOK);
	CButton* ca_btn = (CButton*) GetDlgItem( IDCANCEL);

	CRect rm_rect, ok_rect, ca_rect;
	if ( m_edtRealm.GetSafeHwnd()
		&& ok_btn && ok_btn->GetSafeHwnd()
		&& ca_btn && ca_btn->GetSafeHwnd())
	{
		m_edtRealm.GetWindowRect( &rm_rect);
		ScreenToClient( &rm_rect);

		ok_btn->GetWindowRect( &ok_rect);
		ScreenToClient( &ok_rect);

		ca_btn->GetWindowRect( &ca_rect);
		ScreenToClient( &ca_rect);

		m_edtRealm.SetWindowPos( NULL, rm_rect.left, rm_rect.top, cx - m_leftPad - m_rightPad, rm_rect.Height(), SWP_NOZORDER);
		ok_btn->SetWindowPos( NULL, cx - ok_rect.Width() - m_leftPad, ok_rect.top, ok_rect.Width(), ok_rect.Height(), SWP_NOZORDER);
		ca_btn->SetWindowPos( NULL, cx - ca_rect.Width() - m_leftPad, ca_rect.top, ca_rect.Width(), ca_rect.Height(), SWP_NOZORDER);
	}

}
