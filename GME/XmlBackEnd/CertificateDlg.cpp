// CertificateDlg.cpp : implementation file
//

#include "stdafx.h"
#include ".\CertificateDlg.h"


// CCertificateDlg dialog

IMPLEMENT_DYNAMIC(CCertificateDlg, CDialog)
CCertificateDlg::CCertificateDlg( const std::string& p_cert, bool p_permAcceptEnabled, CWnd* pParent /*=NULL*/)
	: CDialog(CCertificateDlg::IDD, pParent)
	, m_certDetails( p_cert)
	, m_permAcceptEnabled( p_permAcceptEnabled)
	, m_leftPad( 20)
	, m_rightPad( 20)
{

}

CCertificateDlg::~CCertificateDlg()
{
}

void CCertificateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITTEXT, m_textBox);
}


BEGIN_MESSAGE_MAP(CCertificateDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCertificateDlg message handlers

BOOL CCertificateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_textBox.SetWindowText( m_certDetails.c_str());
	
	CRect box_rect;
	if( m_textBox.GetSafeHwnd()) {
		m_textBox.GetWindowRect( &box_rect);
		ScreenToClient( &box_rect);

		CRect win_rect;
		GetWindowRect( &win_rect);
		ScreenToClient( &win_rect);

		// initing m_leftPad, m_rightPad with the initial lf and rt margins
		m_leftPad = box_rect.left - win_rect.left;
		m_rightPad = win_rect.right - box_rect.right;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCertificateDlg::OnBnClickedOk()
{
	UpdateData();

	CButton* rdo1 = (CButton*) GetDlgItem( IDC_RADIO_PERMANENTLY);
	CButton* rdo2 = (CButton*) GetDlgItem( IDC_RADIO_TEMPORARILY);
	CButton* rdo3 = (CButton*) GetDlgItem( IDC_RADIO3);

	m_response = PermanentAccept;
	if( rdo1 && rdo1->GetCheck() == BST_CHECKED)
		m_response = PermanentAccept;
	else if( rdo2 && rdo2->GetCheck() == BST_CHECKED)
		m_response = TemoraryAccept;
	else if( rdo3 && rdo3->GetCheck() == BST_CHECKED)
		m_response = Reject;
	else
	{
		AfxMessageBox( "Before closing the dialog please select one option from Reject, Temporary Accept and Permanent Accept!"); 
		return;
	}
	//"Would you like to reject the certificate? It will not allow connections to the server.", 
	if( m_response == Reject && IDNO == AfxMessageBox( "Warning: Rejecting the certificate will cause connections to this server to fail. Continue?", MB_YESNO))
		return;

	if( !m_permAcceptEnabled && m_response == PermanentAccept)
	{
		AfxMessageBox( "Permanent accept is not a valid option!"); 
		return;
	}

	OnOK();
}

CCertificateDlg::Response CCertificateDlg::getResp()
{
	return m_response;
}

void CCertificateDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CButton* ok_btn = (CButton*) GetDlgItem( IDOK);
	CButton* ca_btn = (CButton*) GetDlgItem( IDCANCEL);

	CRect box_rect, ok_rect, ca_rect;
	if ( m_textBox.GetSafeHwnd()
		&& ok_btn && ok_btn->GetSafeHwnd()
		&& ca_btn && ca_btn->GetSafeHwnd()) 
	{
		m_textBox.GetWindowRect( &box_rect);
		ScreenToClient( &box_rect);

		ok_btn->GetWindowRect( &ok_rect);
		ScreenToClient( &ok_rect);

		ca_btn->GetWindowRect( &ca_rect);
		ScreenToClient( &ca_rect);

		m_textBox.SetWindowPos( NULL, m_leftPad, box_rect.top, cx - m_leftPad - m_rightPad, box_rect.Height(), SWP_NOZORDER);
		ok_btn->SetWindowPos( NULL, cx - ok_rect.Width() - m_leftPad, ok_rect.top, ok_rect.Width(), ok_rect.Height(), SWP_NOZORDER);
		ca_btn->SetWindowPos( NULL, cx - ca_rect.Width() - m_leftPad, ca_rect.top, ca_rect.Width(), ca_rect.Height(), SWP_NOZORDER);
	}
}
