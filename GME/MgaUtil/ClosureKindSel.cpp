// ClosureKindSel.cpp : implementation file
//

#include "stdafx.h"
#include "mgautil.h"
#include "ClosureKindSel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClosureKindSel dialog


CClosureKindSel::CClosureKindSel(bool fld, bool mdl, bool atm, bool set, bool ref, CWnd* pParent /*=NULL*/)
	: CDialog(CClosureKindSel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClosureKindSel)
	m_folder = FALSE;
	m_model = FALSE;
	m_atom = FALSE;
	m_set = FALSE;
	m_ref = FALSE;
	//}}AFX_DATA_INIT
	m_folder = fld?TRUE:FALSE;
	m_model  = mdl?TRUE:FALSE;
	m_atom   = atm?TRUE:FALSE;
	m_set    = set?TRUE:FALSE;
	m_ref    = ref?TRUE:FALSE;
}


void CClosureKindSel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClosureKindSel)
	DDX_Check(pDX, IDC_CHECK1, m_folder);
	DDX_Check(pDX, IDC_CHECK2, m_model);
	DDX_Check(pDX, IDC_CHECK3, m_atom);
	DDX_Check(pDX, IDC_CHECK4, m_set);
	DDX_Check(pDX, IDC_CHECK5, m_ref);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClosureKindSel, CDialog)
	//{{AFX_MSG_MAP(CClosureKindSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClosureKindSel message handlers

BOOL CClosureKindSel::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
