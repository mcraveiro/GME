// ClosureSimple.cpp : implementation file
//

#include "stdafx.h"
#include "MgaUtil.h"
#include "ClosureSimple.h"


// CClosureSimple dialog

IMPLEMENT_DYNAMIC(CClosureSimple, CDialog)
CClosureSimple::CClosureSimple(CWnd* pParent /*=NULL*/)
	: CDialog(CClosureSimple::IDD, pParent)
	, m_enableContainChkBox( true)
	, m_enableFolderChkBox( true)
	, m_bCont(FALSE)
	, m_bFoldCont(FALSE)
	, m_bConn(FALSE)
	, m_bRef(FALSE)
	, m_bAtom(FALSE)
	, m_bSet(FALSE)
{
}

CClosureSimple::~CClosureSimple()
{
}

void CClosureSimple::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_cont);
	DDX_Control(pDX, IDC_CHECK6, m_foldCont);
	DDX_Check(pDX, IDC_CHECK1, m_bCont);
	DDX_Check(pDX, IDC_CHECK6, m_bFoldCont);
	DDX_Check(pDX, IDC_EXCONN, m_bConn);
	DDX_Check(pDX, IDC_EXREF, m_bRef);
	DDX_Check(pDX, IDC_EXATOM, m_bAtom);
	DDX_Check(pDX, IDC_EXSET, m_bSet);
}


BEGIN_MESSAGE_MAP(CClosureSimple, CDialog)
END_MESSAGE_MAP()


// CClosureSimple message handlers
BOOL CClosureSimple::OnInitDialog() 
{
	m_bCont = m_enableContainChkBox?TRUE:FALSE;
	m_bFoldCont = m_enableContainChkBox && m_enableFolderChkBox?TRUE:FALSE;
	
	UpdateData( FALSE);
	CDialog::OnInitDialog();

	m_cont.EnableWindow( m_enableContainChkBox);
	m_foldCont.EnableWindow( m_enableContainChkBox && m_enableFolderChkBox);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
