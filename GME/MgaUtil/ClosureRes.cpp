// ClosureRes.cpp : implementation file
//

#include "stdafx.h"
#include "mgautil.h"
#include "ClosureRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClosureRes dialog


CClosureRes::CClosureRes(CWnd* pParent /*=NULL*/)
	: CDialog(CClosureRes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClosureRes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CClosureRes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClosureRes)
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClosureRes, CDialog)
	//{{AFX_MSG_MAP(CClosureRes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClosureRes message handlers

BOOL CClosureRes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for( unsigned int k = 0; k < m_metaKindsAndFolders.size(); ++k)
	{
		m_ctrlList.InsertString( k, m_metaKindsAndFolders[k].c_str());
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
