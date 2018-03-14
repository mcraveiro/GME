// GlobalAspOrder.cpp : implementation file
//

#include "stdafx.h"
#include "GlobalAspOrder.h"
#include "afxdlgs.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GlobalAspOrder dialog


GlobalAspOrder::GlobalAspOrder(CWnd* pParent /*=NULL*/)
	: CDialog(GlobalAspOrder::IDD, pParent)
{
	//{{AFX_DATA_INIT(GlobalAspOrder)
	//}}AFX_DATA_INIT
}


void GlobalAspOrder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GlobalAspOrder)
	DDX_Control(pDX, IDC_BUTTON2, m_down);
	DDX_Control(pDX, IDC_BUTTON1, m_up);
	DDX_Control(pDX, IDC_LIST3, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GlobalAspOrder, CDialog)
	//{{AFX_MSG_MAP(GlobalAspOrder)
	ON_BN_CLICKED(IDC_BUTTON1, OnMoveUp)
	ON_BN_CLICKED(IDC_BUTTON2, OnMoveDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GlobalAspOrder message handlers

void GlobalAspOrder::OnMoveUp() 
{
	CString text; 
	int idx = m_list.GetCurSel();
	if (LB_ERR != idx && idx > 0)
	{
		int data = m_list.GetItemData( idx);
		m_list.GetText( idx, text);

		m_list.DeleteString( idx);

		m_list.InsertString( idx - 1, text);
		m_list.SetItemData( idx - 1, data);
		m_list.SetCurSel( idx - 1);
	}
}

void GlobalAspOrder::OnMoveDown() 
{
	CString text; 
	int idx = m_list.GetCurSel();
	if (LB_ERR != idx && idx + 1 < m_list.GetCount())
	{
		int data = m_list.GetItemData( idx);
		m_list.GetText( idx, text);

		m_list.DeleteString( idx);

		m_list.InsertString( idx + 1, text);
		m_list.SetItemData( idx + 1, data);
		m_list.SetCurSel( idx + 1);
	}
}


void GlobalAspOrder::addAspects( const std::vector< AspectRep *>& oo)
{
	m_aspects = oo;
}


/*
Presumes that the addAspects has been called!
*/
BOOL GlobalAspOrder::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_down.SetIcon( ::LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICONDN)));
	m_up.SetIcon( ::LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICONUP)));
	
	std::vector< AspectRep *>::const_iterator i;
	unsigned int k;

	for( k = 0, i = m_aspects.begin(); i != m_aspects.end(); ++i, ++k)
	{
		CString asp_name = (*i)->getName().c_str();
		m_list.AddString( asp_name);
		m_list.SetItemData( k, k);
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void GlobalAspOrder::OnOK() 
{
	//std::string mmm;

	int howmany = m_list.GetCount();

	for( int k = 0; k < howmany; ++k)
	{
		unsigned int data = m_list.GetItemData( k);
		if ( data < m_aspects.size())
		{
			m_resultAspects.push_back( m_aspects[data]);
			//mmm += m_aspects[data]->getName() + "\n";
		}
	}
	ASSERT( howmany == m_aspects.size()); // if we've lost aspects

	//AfxMessageBox( mmm.c_str());
	CDialog::OnOK();
}

