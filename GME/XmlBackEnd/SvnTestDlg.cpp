// SvnTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GME.h"
#include "SvnTestDlg.h"


// CSvnTestDlg dialog

IMPLEMENT_DYNAMIC(CSvnTestDlg, CDialog)
CSvnTestDlg::CSvnTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSvnTestDlg::IDD, pParent)
{
}

CSvnTestDlg::~CSvnTestDlg()
{
}

void CSvnTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_field);
	DDX_Control(pDX, IDOK,      m_btn);
}


BEGIN_MESSAGE_MAP(CSvnTestDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSvnTestDlg message handlers

void CSvnTestDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect field_rect, btn_rect;
	if (m_field.GetSafeHwnd() && m_btn.GetSafeHwnd()) {
		m_field.GetWindowRect( &field_rect);
		ScreenToClient( &field_rect);

		m_btn.GetWindowRect( &btn_rect);
		ScreenToClient( &btn_rect);

		int pad = field_rect.left;
		int spc = btn_rect.top - field_rect.bottom;

		//m_field.SetWindowPos( NULL, 0, 0, cx, cy - btn_rect.Height(), SWP_NOZORDER);
		m_field.SetWindowPos( NULL, pad, field_rect.top, cx - 2*pad, cy - field_rect.top - pad - spc - btn_rect.Height(), SWP_NOZORDER);
		m_btn.SetWindowPos( NULL, cx - btn_rect.Width() - pad, cy - btn_rect.Height() - pad, btn_rect.Width(), btn_rect.Height(), SWP_NOZORDER);
	}
}

void CSvnTestDlg::setContent( const CString& p_cont)
{
	m_defContent = p_cont;
}

BOOL CSvnTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_field.SetWindowText( m_defContent);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
