// NewRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "NewRuleDlg.h"
#include ".\newruledlg.h"
#include "Rule.h"

// NewRuleDlg dialog

IMPLEMENT_DYNAMIC(NewRuleDlg, CDialog)
NewRuleDlg::NewRuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NewRuleDlg::IDD, pParent)
{
}

NewRuleDlg::~NewRuleDlg()
{
}

void NewRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_rules);
}


BEGIN_MESSAGE_MAP(NewRuleDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
END_MESSAGE_MAP()


// NewRuleDlg message handlers

BOOL NewRuleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for( int i = 0; i < Rule::rule_nmb; ++i)
	{
		m_rules.AddString( Rule::ruleTypeStr( i));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void NewRuleDlg::OnBnClickedOk()
{
	m_choice = m_rules.GetCurSel();
	if( m_choice != -1)
	{
		m_rules.GetText( m_choice, m_choiceStr);
		OnOK();
	}
}

void NewRuleDlg::OnLbnDblclkList1()
{
	OnBnClickedOk();
}
