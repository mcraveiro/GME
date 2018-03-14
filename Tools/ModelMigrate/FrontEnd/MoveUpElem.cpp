// MoveUpElem.cpp : implementation file
//

#include "stdafx.h"
#include "ModelMigrate.h"
#include "MoveUpElem.h"
#include ".\moveupelem.h"


// MoveUpElem dialog

IMPLEMENT_DYNAMIC(MoveUpElem, CDialog)
MoveUpElem::MoveUpElem(CWnd* pParent /*=NULL*/)
	: CDialog(MoveUpElem::IDD, pParent)
{
}

MoveUpElem::~MoveUpElem()
{
}

void MoveUpElem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tree);
}


BEGIN_MESSAGE_MAP(MoveUpElem, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_INSERTNODE, OnBnClickedButtonInsertnode)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE1, OnTvnEndlabeleditTree1)
	ON_NOTIFY(NM_RETURN, IDC_TREE1, OnNMReturnTree1)
	ON_BN_CLICKED(IDC_BUTTON_DELETENODE, OnBnClickedButtonDeletenode)
END_MESSAGE_MAP()


// MoveUpElem message handlers

void MoveUpElem::addChild( const char * pLabel)
{
	HTREEITEM hLastGoodItem = lastChild();
	HTREEITEM hNewItem = m_tree.InsertItem( _T(pLabel), hLastGoodItem);
	m_tree.Expand( hLastGoodItem, TVE_EXPAND);
	m_tree.EnsureVisible( hNewItem);
}

void MoveUpElem::OnBnClickedButtonInsertnode()
{
	addChild( "Node");
	HTREEITEM lc = lastChild();
	if( lc) m_tree.EditLabel( lc);
}

void MoveUpElem::OnBnClickedButtonDeletenode()
{
	remChild();
}

HTREEITEM MoveUpElem::lastChild()
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

void MoveUpElem::remChild()
{
	HTREEITEM hLastGoodItem = lastChild();
	if( hLastGoodItem && hLastGoodItem != TVI_ROOT) m_tree.DeleteItem( hLastGoodItem);
	else
		AfxMessageBox( "No more elements to delete!");
}

void MoveUpElem::closeDlg( int pResult)
{
	EndDialog( pResult);
	DestroyWindow();
}

void MoveUpElem::OnOK()
{
	if( AfxMessageBox( "Close dialog and save changes?", MB_YESNOCANCEL) == IDYES)
	{
		m_storage = getValues();
		closeDlg( IDOK);
	}
}

void MoveUpElem::OnCancel()
{
	if( AfxMessageBox( "Close dialog and discard changes?", MB_YESNOCANCEL) == IDYES)
		closeDlg( IDCANCEL);
}


void MoveUpElem::OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if( !pTVDispInfo->item.pszText) return; // if 0 means edit cancelled

	m_tree.SetItem( &pTVDispInfo->item);

	*pResult = 0;
}

void MoveUpElem::OnNMReturnTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

std::vector< std::string> MoveUpElem::getValues()
{
	std::vector< std::string> res;
	HTREEITEM h = m_tree.GetRootItem();
	while ( h != NULL)
	{
		res.push_back( (LPCTSTR) m_tree.GetItemText( h));
		h = m_tree.GetChildItem( h);
	}

	return res;
}

std::vector< std::string> MoveUpElem::getSequence()
{
	return m_storage;
}

void MoveUpElem::init( const std::vector< std::string>& pars)
{
	m_storage = pars;
}

BOOL MoveUpElem::OnInitDialog()
{
	CDialog::OnInitDialog();

	for( unsigned int i = 0; i < m_storage.size(); ++i)
	{
		addChild( m_storage[i].c_str());
	}
	
	m_storage.clear();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
