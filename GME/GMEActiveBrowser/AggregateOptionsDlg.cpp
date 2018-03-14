// AggregateOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "AggregateOptionsDlg.h"

#include "ActiveBrowserPropertyPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAggregateOptionsDlg dialog


CAggregateOptionsDlg::CAggregateOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAggregateOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAggregateOptionsDlg)
	m_bDisplayAtoms = FALSE;
	m_bDisplayConnections = FALSE;
	m_bDisplayModels = FALSE;
	m_bDisplayReferences = FALSE;
	m_bDisplaySets = FALSE;
	m_SortOptions = -1;
	m_bStoreTreeInRegistry = FALSE;
	//}}AFX_DATA_INIT
}


void CAggregateOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAggregateOptionsDlg)
	DDX_Check(pDX, IDC_DISPLAY_ATOMS, m_bDisplayAtoms);
	DDX_Check(pDX, IDC_DISPLAY_CONNECTIONS, m_bDisplayConnections);
	DDX_Check(pDX, IDC_DISPLAY_MODELS, m_bDisplayModels);
	DDX_Check(pDX, IDC_DISPLAY_REFERENCES, m_bDisplayReferences);
	DDX_Check(pDX, IDC_DISPLAY_SETS, m_bDisplaySets);
	DDX_Radio(pDX, IDC_SORT, m_SortOptions);
	DDX_Check(pDX, IDC_STORE_TREE_REGISTRY, m_bStoreTreeInRegistry);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAggregateOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CAggregateOptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAggregateOptionsDlg message handlers

void CAggregateOptionsDlg::OnOK() 
{
	if(!UpdateData(TRUE))return;

	CAggregatePropertyPage*pParentPage=(CAggregatePropertyPage*)m_pParentWnd;

	if(	pParentPage->m_Options.m_bDispAtom==m_bDisplayAtoms&&
		pParentPage->m_Options.m_bDispConnections==m_bDisplayConnections&&
		pParentPage->m_Options.m_bDispModel==m_bDisplayModels&&
		pParentPage->m_Options.m_bDispReference==m_bDisplayReferences&&
		pParentPage->m_Options.m_bDispSet==m_bDisplaySets)
	{
		m_bIsRefreshNeeded=FALSE;
	}
	else
	{
		pParentPage->m_Options.m_bDispAtom=m_bDisplayAtoms;
		pParentPage->m_Options.m_bDispConnections=m_bDisplayConnections;
		pParentPage->m_Options.m_bDispModel=m_bDisplayModels;
		pParentPage->m_Options.m_bDispReference=m_bDisplayReferences;
		pParentPage->m_Options.m_bDispSet=m_bDisplaySets;
		
		m_bIsRefreshNeeded=TRUE;
	}
	if(pParentPage->m_Options.m_soSortOptions==(eSortOptions)m_SortOptions)
	{
		m_bIsResortNeeded=FALSE;
	}
	else
	{
		pParentPage->m_Options.m_soSortOptions=(eSortOptions)m_SortOptions;
		
		m_bIsResortNeeded=TRUE;
	}
	
	
	pParentPage->m_Options.m_bStoreTreeInRegistry=m_bStoreTreeInRegistry;
	pParentPage->m_Options.SaveToRegistry();


	CDialog::OnOK();
}

void CAggregateOptionsDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CAggregateOptionsDlg::OnInitDialog() 
{

	CDialog::OnInitDialog();
	
	CAggregatePropertyPage*pParentPage=(CAggregatePropertyPage*)m_pParentWnd;
	
	m_bDisplayAtoms=pParentPage->m_Options.m_bDispAtom;
	m_bDisplayConnections=pParentPage->m_Options.m_bDispConnections;
	m_bDisplayModels=pParentPage->m_Options.m_bDispModel;
	m_bDisplayReferences=pParentPage->m_Options.m_bDispReference;
	m_bDisplaySets=pParentPage->m_Options.m_bDispSet;
	
	m_bStoreTreeInRegistry=pParentPage->m_Options.m_bStoreTreeInRegistry;
	m_SortOptions=pParentPage->m_Options.m_soSortOptions;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
