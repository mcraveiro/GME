// MoveDownElem.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "MoveDownElem.h"
#include ".\movedownelem.h"


// MoveDownElem dialog

IMPLEMENT_DYNAMIC(MoveDownElem, CDialog)
MoveDownElem::MoveDownElem(CWnd* pParent /*=NULL*/)
	: CDialog(MoveDownElem::IDD, pParent)
{
}

MoveDownElem::~MoveDownElem()
{
}

void MoveDownElem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREETWO, m_tree);
	DDX_Control(pDX, IDC_EDITKIND, m_editKind);
	DDX_Control(pDX, IDC_EDITPARENT, m_parentKind);
}


BEGIN_MESSAGE_MAP(MoveDownElem, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_INSERTNODE, OnBnClickedButtonInsertnode)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREETWO, OnTvnEndlabeleditTree1)
	ON_NOTIFY(NM_RETURN, IDC_TREETWO, OnNMReturnTree1)
	ON_BN_CLICKED(IDC_BUTTON_DELETENODE, OnBnClickedButtonDeletenode)
END_MESSAGE_MAP()


// MoveDownElem message handlers

void MoveDownElem::addChild( const char * pLabel)
{
	HTREEITEM hLastGoodItem = lastChild();
	HTREEITEM hNewItem = m_tree.InsertItem( _T(pLabel), hLastGoodItem);
	m_tree.Expand( hLastGoodItem, TVE_EXPAND);
	m_tree.EnsureVisible( hNewItem);
}

void MoveDownElem::OnBnClickedButtonInsertnode()
{
	addChild( "Node");
	HTREEITEM lc = lastChild();
	if( lc) m_tree.EditLabel( lc);
}

void MoveDownElem::OnBnClickedButtonDeletenode()
{
	remChild();
}

HTREEITEM MoveDownElem::lastChild()
{
	HTREEITEM hLastGoodItem = TVI_ROOT;
	HTREEITEM h = m_tree.GetRootItem();
	while ( h != NULL)
	{
		m_tree.Expand( h, TVE_EXPAND);

		hLastGoodItem = h;
		h = m_tree.GetChildItem( h);
	}

	return hLastGoodItem;
}

void MoveDownElem::remChild()
{
	HTREEITEM hLastGoodItem = lastChild();

	if( hLastGoodItem && hLastGoodItem != TVI_ROOT) m_tree.DeleteItem( hLastGoodItem);
	else
		AfxMessageBox( "No more elements to delete!");
}

void MoveDownElem::closeDlg( int pResult)
{
	EndDialog( pResult);
	DestroyWindow();
}

void MoveDownElem::OnOK()
{
	if( AfxMessageBox( "Close dialog and save changes?", MB_YESNOCANCEL) == IDYES)
	{
		m_storage = getValues();
		closeDlg( IDOK);
	}
}

void MoveDownElem::OnCancel()
{
	if( AfxMessageBox( "Close dialog and discard changes?", MB_YESNOCANCEL) == IDYES)
		closeDlg( IDCANCEL);
}


void MoveDownElem::OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if( !pTVDispInfo->item.pszText) return; // if 0 means edit cancelled

	m_tree.SetItem( &pTVDispInfo->item);

	*pResult = 0;
}

void MoveDownElem::OnNMReturnTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

std::string MoveDownElem::getKindField()
{
	CString txt;
	m_editKind.GetWindowText( txt);
	return (LPCTSTR) txt;
}

std::string MoveDownElem::getParentField()
{
	CString txt;
	m_parentKind.GetWindowText( txt);
	return (LPCTSTR) txt;
}

std::vector< std::string> MoveDownElem::getValues()
{
	std::vector< std::string> res;
	
	// #0 is the element to move
	res.push_back( getKindField());

	// #1 is the parent of this
	res.push_back( getParentField());
	
	// now the wrappers
	HTREEITEM h = m_tree.GetRootItem();
	while ( h != NULL)
	{
		res.push_back( (LPCTSTR) m_tree.GetItemText( h));
		
		h = m_tree.GetChildItem( h);
	}
	return res;
}

std::vector< std::string> MoveDownElem::getSequence()
{
	return m_storage;
}

void MoveDownElem::init( const std::vector< std::string>& pars)
{
	m_storage = pars;
}

BOOL MoveDownElem::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( !m_storage.empty())
		m_editKind.SetWindowText( m_storage[0].c_str());
	else
		m_editKind.SetWindowText( "<Specify here the kind that will be wrapped>");

	if( m_storage.size() >= 2)
		m_parentKind.SetWindowText( m_storage[1].c_str());
	else
		m_parentKind.SetWindowText("<Its parent must be specified here>");

	// m_storage[0] is the to-be-moved kind
	// m_storage[1] is the parent of the to-be-moved kind

	for( unsigned int i = 2; i < m_storage.size(); ++i)
	{
		addChild( m_storage[i].c_str());
	}
	
	m_storage.clear();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
