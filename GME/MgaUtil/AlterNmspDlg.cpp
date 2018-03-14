// AlterNmspDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MgaUtil.h"
#include "AlterNmspDlg.h"


// AlterNmspDlg dialog

IMPLEMENT_DYNAMIC(AlterNmspDlg, CDialog)
AlterNmspDlg::AlterNmspDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AlterNmspDlg::IDD, pParent)
{
	m_strPrefix   = _T("");
	m_strTrunc    = _T("");
	m_vRadAlter   = 0;
	m_bAlterSticky= FALSE;

	//m_storedStrPrefix = _T("");
	//m_storedStrTrunc  = _T("");
	//m_storedAltOption = 0;
	//m_storedAltSticky = TRUE;
}

AlterNmspDlg::~AlterNmspDlg()
{
}

void AlterNmspDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_EDTPREFIX, m_edtPrefix);
	DDX_Control( pDX, IDC_EDTTRUNC,  m_edtTrunc);
	DDX_Control( pDX, IDC_RAD_ALTER0, m_radAlter0);
	DDX_Control( pDX, IDC_RAD_ALTER1, m_radAlter1);
	DDX_Control( pDX, IDC_RAD_ALTER2, m_radAlter2);
	DDX_Control( pDX, IDC_RAD_ALTER3, m_radAlter3);
	DDX_Control( pDX, IDC_APPLY_ALL,  m_chkAlterSticky);

	DDX_Text(    pDX, IDC_EDTPREFIX, m_strPrefix);
	DDX_Text(    pDX, IDC_EDTTRUNC,  m_strTrunc);
	DDX_Radio(   pDX, IDC_RAD_ALTER0, m_vRadAlter);
	DDX_Check(   pDX, IDC_APPLY_ALL,  m_bAlterSticky);
}


BEGIN_MESSAGE_MAP(AlterNmspDlg, CDialog)
	ON_BN_CLICKED(IDC_RAD_ALTER0, OnBnClickedRadAlter0)
	ON_BN_CLICKED(IDC_RAD_ALTER1, OnBnClickedRadAlter1)
	ON_BN_CLICKED(IDC_RAD_ALTER2, OnBnClickedRadAlter2)
	ON_BN_CLICKED(IDC_RAD_ALTER3, OnBnClickedRadAlter3)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// AlterNmspDlg message handlers
BOOL AlterNmspDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//m_strPrefix    = m_storedStrPrefix;
	//m_strTrunc     = m_storedStrTrunc;
	//m_vRadAlter    = m_storedAltOption;
	//m_bAlterSticky = m_storedAltSticky;

	m_edtPrefix.EnableWindow( FALSE);
	m_edtTrunc.EnableWindow(  FALSE);
	m_chkAlterSticky.ShowWindow( SW_HIDE);

	UpdateData( FALSE);
	return TRUE;
}

void AlterNmspDlg::OnOK() 
{
	UpdateData( TRUE);

	// TODO: Add extra validation here
	if( m_vRadAlter == 1 && m_strPrefix.IsEmpty()
	 || m_vRadAlter == 2 && m_strTrunc.IsEmpty()
	 || m_vRadAlter == 3 && ( m_strPrefix.IsEmpty() || m_strTrunc.IsEmpty()))
	{
		AfxMessageBox( _T("Please do NOT specify empty string in the edit box!"));
		return; // don't allow these scenarios
	}

	// completing the namespaces with the operator: '::'
	if( !m_strPrefix.IsEmpty() && m_strPrefix.Right(2) != _T("::")) m_strPrefix += _T("::");
	if( !m_strTrunc.IsEmpty()  && m_strTrunc.Right(2)  != _T("::")) m_strTrunc  += _T("::");

	UpdateData( FALSE);
	//m_storedStrPrefix = m_strPrefix;
	//m_storedStrTrunc  = m_strTrunc;
	//m_storedAltOption = m_vRadAlter;
	//m_storedAltSticky = m_bAlterSticky;

	CDialog::OnOK();
}

void AlterNmspDlg::OnBnClickedRadAlter0()
{
	m_edtPrefix.EnableWindow( FALSE);
	m_edtTrunc.EnableWindow(  FALSE);
}

void AlterNmspDlg::OnBnClickedRadAlter1()
{
	m_edtPrefix.EnableWindow( TRUE);
	m_edtTrunc.EnableWindow(  FALSE);
}

void AlterNmspDlg::OnBnClickedRadAlter2()
{
	m_edtPrefix.EnableWindow( FALSE);
	m_edtTrunc.EnableWindow(  TRUE);
}

void AlterNmspDlg::OnBnClickedRadAlter3()
{
	m_edtPrefix.EnableWindow( TRUE);
	m_edtTrunc.EnableWindow(  TRUE);
}
void AlterNmspDlg::OnBnClickedButton1()
{
	AfxMessageBox(_T("Kinds may differ in this paradigm compared to the originating one. You might choose to ")
		_T("prefix the originating object kinds and roles with a specific namespace string. ")
		_T("You might also choose to disregard namespace related information from the XML data by using the Trunc option. ")
		_T("To migrate from one namespace to another you can specify the namespace string to be removed (in the 2nd edit box) and the namespace ")
		_T("string to be used then as a prefix (in the 1st edit box)."), MB_OK | MB_ICONINFORMATION);
}
