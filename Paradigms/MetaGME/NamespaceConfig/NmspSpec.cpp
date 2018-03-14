// NmspSpec.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "NmspSpec.h"
#include ".\nmspspec.h"
#include <list>

// NmspSpec dialog

IMPLEMENT_DYNAMIC(NmspSpec, CDialog)
NmspSpec::NmspSpec(CWnd* pParent /*=NULL*/)
	: CDialog(NmspSpec::IDD, pParent)
	, m_edChanged( false)
{
}

NmspSpec::~NmspSpec()
{
}

void NmspSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edName);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Control(pDX, IDC_CHECK1, m_bEnabled);
	DDX_Control(pDX, IDC_SAVE, m_bSave);
}


BEGIN_MESSAGE_MAP(NmspSpec, CDialog)
	ON_EN_KILLFOCUS(IDC_EDIT1, OnEnKillfocusEdit1)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnNMClickTree1)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE1, OnNMSetfocusTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_EN_SETFOCUS(IDC_EDIT1, OnEnSetfocusEdit1)
END_MESSAGE_MAP()


// NmspSpec message handlers

void NmspSpec::updateItemInfo( int pIdx, CString pTxt)
{
	int idx = 0;
	for( MetaBON::MetaGMEVisitor::VectorOfPairs::iterator it = m_data.begin()
		; it != m_data.end()
		; ++it, ++idx)
	{
		if( idx == pIdx && !it->first->isInLibrary())
		{
			it->second = (LPCTSTR) pTxt;
		}
	}
	m_bSave.EnableWindow( m_edChanged = false);
}

bool NmspSpec::loadItemInfo( int pIdx, CString& pTxt)
{
	bool editable = false;
	int idx = 0;
	for( MetaBON::MetaGMEVisitor::VectorOfPairs::iterator it = m_data.begin()
		; it != m_data.end()
		; ++it, ++idx)
	{
		if( idx == pIdx)
		{
			pTxt = it->second.c_str();
			editable = !it->first->isInLibrary();
		}
	}
	return editable;
}

void NmspSpec::setData( MetaBON::MetaGMEVisitor::VectorOfPairs& pData)
{
	m_data = pData;
}

void NmspSpec::getData( MetaBON::MetaGMEVisitor::VectorOfPairs& pResu)
{
	pResu = m_data;
}

void NmspSpec::initElems()
{
	int p = 0;
	for( MetaBON::MetaGMEVisitor::VectorOfPairs::const_iterator it = m_data.begin()
		; it != m_data.end()
		; ++it, ++p)
	{
		insert( p, it);
	}
}

void NmspSpec::insert( int pIdx, MetaBON::MetaGMEVisitor::VectorOfPairs::const_iterator it)
{
	std::string pr = it->first->getName();
	std::string nm = it->second;

	HTREEITEM ele = m_tree.InsertItem( LVIF_TEXT|TVIF_PARAM, pr.c_str(), 0, 0, 0, 0, pIdx, pIdx?m_rootElem:TVI_ROOT, TVI_LAST);

	if( pIdx == 0) m_rootElem = ele;
}

void NmspSpec::updateEditField( int pSel /* = -1 */)
{
	int hm = pSel; // pSel might send the value of just selected/highlighted item
	if( hm < 0)
	{
		// get the currently selected item
		TVITEM tv;
		tv.mask = TVIF_PARAM | TVIF_HANDLE;
		tv.hItem = m_tree.GetNextItem( TVI_ROOT, TVGN_CARET);
		if( tv.hItem && m_tree.GetItem( &tv))
		{
			hm = tv.lParam;
		}
	}

	if( hm >= 0)
	{
		CString itm;
		bool editable = loadItemInfo( hm, itm);
		m_edName.EnableWindow( editable);
		m_edName.SetWindowText( (LPCTSTR) itm);
	}
	else
		m_edName.SetWindowText( "");
}

BOOL NmspSpec::OnInitDialog()
{
	CDialog::OnInitDialog();

	initElems();
	m_tree.Expand( m_rootElem, TVE_EXPAND);
	m_bSave.EnableWindow( m_edChanged = false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void NmspSpec::OnEnKillfocusEdit1()
{
	m_bSave.EnableWindow( m_edChanged);
	return;
}

void NmspSpec::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	m_bSave.EnableWindow( m_edChanged = true);
	// TODO:  Add your control notification handler code here
}

void NmspSpec::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	updateEditField();
	*pResult = 0;
}

void NmspSpec::OnNMSetfocusTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	updateEditField();
	m_bSave.EnableWindow( m_edChanged = false);
	*pResult = 0;
}

void NmspSpec::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	updateEditField( pNMTreeView ? pNMTreeView->itemNew.lParam : 0);
	m_bSave.EnableWindow( m_edChanged = false);
	*pResult = 0;
}

void NmspSpec::OnBnClickedSave()
{
	CString txt;
	m_edName.GetWindowText( txt);

	// apply it to the selected library
	TVITEM tv;
	tv.mask = TVIF_PARAM | TVIF_HANDLE;
	tv.hItem = m_tree.GetNextItem( TVI_ROOT, TVGN_CARET);
	if( tv.hItem && m_tree.GetItem( &tv))
	{
		updateItemInfo( tv.lParam, txt);
		//AfxMessageBox("updatIteminfo");
		m_bSave.EnableWindow( m_edChanged = false);
		m_tree.SetFocus();
	}
}

void NmspSpec::OnEnSetfocusEdit1()
{
	m_bSave.EnableWindow( m_edChanged);
}
