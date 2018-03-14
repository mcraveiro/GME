// ClosureSmart.cpp : implementation file
//

#include "stdafx.h"
#include "mgautil.h"
#include "ClosureSmart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClosureSmart dialog


CClosureSmart::CClosureSmart(CWnd* pParent /*=NULL*/)
	: CDialog(CClosureSmart::IDD, pParent)
	//: CPropertyPage(CClosureSmart::IDD)
{
	//{{AFX_DATA_INIT(CClosureSmart)
	m_bConns = FALSE;
	m_bRefs = FALSE;
	m_bSets = FALSE;
	//}}AFX_DATA_INIT

	m_disable = false;
}

void CClosureSmart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClosureSmart)
	DDX_Check(pDX, IDC_CHECKCONN, m_bConns);
	DDX_Check(pDX, IDC_CHECKREF, m_bRefs);
	DDX_Check(pDX, IDC_CHECKSET, m_bSets);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClosureSmart, CDialog)
	//{{AFX_MSG_MAP(CClosureSmart)
	ON_BN_CLICKED(IDC_INVERT, OnInvert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClosureSmart message handlers

BOOL CClosureSmart::OnInitDialog() 
{
	UpdateData( FALSE);
	CDialog::OnInitDialog();
	
	if( m_disable)
	{
		GetDlgItem( IDC_CHECKCONN)->EnableWindow( m_bConns);
		GetDlgItem( IDC_CHECKSET)->EnableWindow( m_bSets);
		GetDlgItem( IDC_CHECKREF)->EnableWindow( m_bRefs);
	}
	else // container case
	{
		CWnd* w; CString t;
		w = GetDlgItem( IDC_CHECKCONN);
		w->GetWindowText( t);
		w->SetWindowText( "Inner " + t);

		w = GetDlgItem( IDC_CHECKSET);
		w->GetWindowText( t);
		w->SetWindowText( "Inner " + t);

		w = GetDlgItem( IDC_CHECKREF);
		w->GetWindowText( t);
		w->SetWindowText( "Inner " + t);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClosureSmart::OnInvert()
{
	UpdateData( TRUE);

	CWnd* w;
	w = GetDlgItem( IDC_CHECKCONN);
	if( w && w->IsWindowEnabled()) m_bConns = !m_bConns;

	w = GetDlgItem( IDC_CHECKSET);
	if( w && w->IsWindowEnabled()) m_bSets = !m_bSets;
	
	w = GetDlgItem( IDC_CHECKREF);
	if( w && w->IsWindowEnabled()) m_bRefs = !m_bRefs;

	UpdateData( FALSE);
}