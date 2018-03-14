// ActiveBrowsePropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ActiveBrowsePropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActiveBrowserPropertySheet

IMPLEMENT_DYNAMIC(CActiveBrowserPropertySheet, CPropertySheet)

CActiveBrowserPropertySheet::CActiveBrowserPropertySheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	AddPage(&m_PageAggregate);
	AddPage(&m_PageInheritance);
	AddPage(&m_PageMeta);
	m_PageAggregate.m_parent = this;
	m_PageInheritance.m_parent = this;
	m_PageMeta.m_parent = this;
}

CActiveBrowserPropertySheet::~CActiveBrowserPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CActiveBrowserPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CActiveBrowserPropertySheet)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CActiveBrowserPropertySheet message handlers

void CActiveBrowserPropertySheet::PostNcDestroy()
{
	CPropertySheet::PostNcDestroy();
	delete this;
}



void CActiveBrowserPropertySheet::OnSize(UINT nType, int cx, int cy) 
{
	if (::IsWindow(m_PageAggregate.GetSafeHwnd()) ||
		::IsWindow(m_PageInheritance.GetSafeHwnd()) ||
		::IsWindow(m_PageMeta.GetSafeHwnd()))
	{
		CTabCtrl* pTabCtrl=GetTabControl();
		pTabCtrl->MoveWindow(0, 0, cx, cy, TRUE);
		CRect TabRect;
		pTabCtrl->GetWindowRect(&TabRect);

		if (::IsWindow(m_PageAggregate.GetSafeHwnd()))
			m_PageAggregate.MoveWindow(TabRect.TopLeft().x, TabRect.BottomRight().y, cx, cy, TRUE);
		if (::IsWindow(m_PageInheritance.GetSafeHwnd()))
			m_PageInheritance.MoveWindow(TabRect.TopLeft().x, TabRect.BottomRight().y, cx, cy, TRUE);
		if (::IsWindow(m_PageMeta.GetSafeHwnd()))
			m_PageMeta.MoveWindow(TabRect.TopLeft().x, TabRect.BottomRight().y, cx, cy, TRUE);

		SetActivePage(GetActiveIndex());
	}
}



BOOL CActiveBrowserPropertySheet::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	return CPropertySheet::PreTranslateMessage(pMsg);
}

void CActiveBrowserPropertySheet::nextTab( bool pOrder)
{
	SetActivePage( ( GetActiveIndex() + ( pOrder? 1:( GetPageCount() -1))) % GetPageCount());
}

