// StereotypeCastDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "StereotypeCastDlg.h"
#include ".\stereotypecastdlg.h"


// StereotypeCastDlg dialog

IMPLEMENT_DYNAMIC(StereotypeCastDlg, CDialog)
StereotypeCastDlg::StereotypeCastDlg( bool pCastAtom2Model, CWnd* pParent /*=NULL*/)
	: CDialog(StereotypeCastDlg::IDD, pParent)
	, m_kind(_T(""))
	, m_title( pCastAtom2Model? _T("Cast Atom to Model"):_T("Cast Model to Atom"))
{
}

StereotypeCastDlg::~StereotypeCastDlg()
{
}

void StereotypeCastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_kind);
}


BEGIN_MESSAGE_MAP(StereotypeCastDlg, CDialog)
END_MESSAGE_MAP()


// StereotypeCastDlg message handlers

const CString& StereotypeCastDlg::getPar1() const
{
	return m_kind;
}

void StereotypeCastDlg::init( const CString& par1)
{
	m_kind = par1;
}

BOOL StereotypeCastDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText( (LPCTSTR) m_title);

	return TRUE;
}
