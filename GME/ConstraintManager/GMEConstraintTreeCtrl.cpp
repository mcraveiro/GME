//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMEConstraintTreeCtrl.cpp
//
//###############################################################################################################################################

#include "StdAfx.h"
#include "GMEConstraintTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int static CALLBACK SpecialCompare( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	CConstraintTreeCtrl* pTree = (CConstraintTreeCtrl*) lParamSort;
	int iImNum1, iImNum2;
	pTree->GetItemImage( ( HTREEITEM ) lParam1, iImNum1, iImNum1 );
	pTree->GetItemImage( ( HTREEITEM ) lParam2, iImNum2, iImNum2 );

	CString strCaption1 = pTree->GetItemText( ( HTREEITEM ) lParam1 );
	CString strCaption2 = pTree->GetItemText( ( HTREEITEM ) lParam2 );

	if ( iImNum1 < iImNum2 )
		return -1;
	if ( iImNum1 > iImNum2 )
		return 1;
	return _tcscmp( strCaption1, strCaption2 );
}

//##############################################################################################################################################
//
//	C L A S S : CConstraintTreeCtrl <<< + CTreeCtrlEx
//
//##############################################################################################################################################

IMPLEMENT_DYNAMIC(CConstraintTreeCtrl, CTreeCtrlEx)

CConstraintTreeCtrl::CConstraintTreeCtrl()
	: CTreeCtrlEx(), m_eKind( CConstraintTreeCtrl::TK_UNKNOWN )
{
}

CConstraintTreeCtrl::~CConstraintTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CConstraintTreeCtrl, CTreeCtrlEx)
	//{{AFX_MSG_MAP(CConstraintTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//==============================================================================================================================
// PUBLIC METHOD -> SelectAllItems
//
//	DESCRIPTION:
//		Selects or deselects the part of the tree specified by the root item
//	ARGUMENTS:
// 		hRoot:
//			handle of a tree item which is the begining of the selection
//		bSelect:
//			trivial
//	NOTE:
//		As the base class seems to be a little buggy, we have to select explicitly the first visible item

	void CConstraintTreeCtrl::SelectAllItems( HTREEITEM hRoot, bool bSelect /* = TRUE */ )
	{
		for ( HTREEITEM hItem = hRoot ; hItem != NULL ; hItem = GetNextSiblingItem( hItem ) ) {
			for ( HTREEITEM hItem2 = GetChildItem( hItem ) ; hItem2 != NULL ; hItem2 = GetNextSiblingItem( hItem2 ) )
				SelectAllItems( hItem2, bSelect );
			SetItemState( hItem, ( bSelect ) ? TVIS_SELECTED : 0, TVIS_SELECTED );
		}
		HTREEITEM hItem = GetFirstVisibleItem();
		if ( hItem )
			SelectItemEx( hItem, bSelect );
	}

	void CConstraintTreeCtrl::SetKind( CConstraintTreeCtrl::Kind eKind )
	{
		m_eKind = eKind;
	}

	CConstraintTreeCtrl::Kind CConstraintTreeCtrl::GetKind() const
	{
		return m_eKind;
	}

	void CConstraintTreeCtrl::SortItemChildren( HTREEITEM hItem )
	{
		TVSORTCB tvs;
		tvs.hParent = hItem;
	   	tvs.lpfnCompare = SpecialCompare;
	   	tvs.lParam = (LPARAM) this;

		SortChildrenCB( &tvs );
	}

//==============================================================================================================================
// PROTECTED METHOD -> SendTreeNotification
//
//	DESCRIPTION:
//		Simple helper method in order not to write too much. Sends the specified Tree Notification
//	ARGUMENTS:
//		uiMessage:
//			id of the message
//		hItem:
//			handle of the appropriate treeitem

void CConstraintTreeCtrl::SendTreeNotification( UINT uiMessage, HTREEITEM hItem )
{
	CWnd* pWnd = GetParent();
	if ( pWnd ) {
		NM_TREEVIEW tv;

		tv.hdr.hwndFrom = GetSafeHwnd();
		tv.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );
		tv.hdr.code = uiMessage;

		tv.itemNew.hItem = hItem;
		tv.itemNew.state = GetItemState( hItem, 0xFFFFFFFF );
		tv.itemNew.lParam = GetItemData( hItem );

		pWnd->SendMessage( WM_NOTIFY, tv.hdr.idFrom, (LPARAM) &tv );
	}
}

//================================================================================================================================
// PROTECTED METHOD -> OnLButtonDown <OVERRIDE, VIRTUAL>
//
//	DESCRIPTION:
//		It catches the click if it was performed on the ItemStateIcon, otherwise call the base class.
//	ARGUMENT:
//		See MSDN.
//	TODO:
//		After catching we should send a NM_CLICK Notification :)

void CConstraintTreeCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	UINT flags = 0;
	HTREEITEM hItem = HitTest( point, &flags );
	if ( hItem && ( flags & TVHT_ONITEMSTATEICON ) ) {
		int iOldState = GETSTATE( hItem );
		if ( iOldState > 2 && iOldState < 8 )
			SendTreeNotification( CTVN_CHECKCHANGED, hItem );
	}
	CTreeCtrlEx::OnLButtonDown( nFlags, point );
}

void CConstraintTreeCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
	UINT flags = 0;
	HTREEITEM hItem = HitTest( point, &flags );
	if ( hItem && ( flags & TVHT_ONITEMSTATEICON ) ) {
		int iOldState = GETSTATE( hItem );
		if ( iOldState > 2 && iOldState < 8 )
			SendTreeNotification( CTVN_CHECKCHANGEDREC, hItem );
	}
	CTreeCtrlEx::OnRButtonDown( nFlags, point );
}

//================================================================================================================================
// PROTECTED METHOD -> OnKeyUp <OVERRIDE, VIRTUAL>
//
//	DESCRIPTION:
//		It catches the space key, otherwise call the base class.
//	ARGUMENT:
//		See MSDN.
//	TODO:
//		After catching we should send a NM_KEY Notification :)

void CConstraintTreeCtrl::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if ( nChar == VK_SPACE ) {
		HTREEITEM hItem = GetSelectedItem();
		if ( hItem ) {
			int iOldState = GETSTATE( hItem );
			if ( iOldState > 2 && iOldState < 8 )
				SendTreeNotification( CTVN_CHECKCHANGED, hItem );
		}
		return;
	}
	CTreeCtrlEx::OnKeyUp( nChar, nRepCnt, nFlags );
}

//================================================================================================================================
// PROTECTED METHOD -> OnLButtonDblClk <OVERRIDE, VIRTUAL>
//
//	DESCRIPTION:
//		It catches the double click. If it is on TreeButton the delegate the call to the base class. If it is on either the icon or the label CTVN_SHOWITEM notifcation is sent. In any other case it eats the message.
//	ARGUMENT:
//		See MSDN.
//	TODO:
//		After catching we should send a NM_DBLCLK Notification :)

void CConstraintTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	UINT flags = 0;
	HTREEITEM hItem = HitTest( point, &flags );
	if ( hItem )
		if ( flags & TVHT_ONITEMBUTTON )
			CTreeCtrlEx::OnLButtonDblClk(nFlags, point);
		else
			if ( flags & ( TVHT_ONITEMICON | TVHT_ONITEMLABEL ) )
				SendTreeNotification( CTVN_SHOWITEM, hItem );
}
