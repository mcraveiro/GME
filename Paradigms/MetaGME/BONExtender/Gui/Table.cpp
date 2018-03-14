// AspectSpecTbl.cpp : implementation file
//

#include "stdafx.h"
#include "inplaceedit.h"
#include "inplacelist.h"
#include "table.h"
#include "SelConf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define OFFSET_FIRST        2       // offsets for first and other columns in owner draw methods 
#define OFFSET_OTHER        6

#define EXT_COLNUM          2       // column of 'Extended?'
#define RSP_COLNUM          3       // column of 'Capture as' or 'Responsible'

/////////////////////////////////////////////////////////////////////////////
// CTable

CTable::CTable()
	: m_parent(0)
	, m_cxClient(0)
{
}

CTable::~CTable()
{
}

BOOL CTable::PreCreateWindow(CREATESTRUCT &cs)
{
	cs.style |= LVS_OWNERDRAWFIXED;
	return CListCtrl::PreCreateWindow( cs);
}


BEGIN_MESSAGE_MAP(CTable, CListCtrl)
	//{{AFX_MSG_MAP(CTable)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnSort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTable message handlers

void CTable::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here

	LV_ITEM	*plvItem = &pDispInfo->item;

	if (plvItem->pszText != NULL)
	{
		// get old value
		LV_ITEM old_lvItem;
		old_lvItem.mask = LVIF_TEXT;
		old_lvItem.iItem = plvItem->iItem;
		
		CString prev_str;
		old_lvItem.iSubItem = plvItem->iSubItem;
		old_lvItem.pszText = prev_str.GetBuffer(255);
		old_lvItem.cchTextMax = 255;
		GetItem(&old_lvItem);

		// if not changed, return
		if( prev_str == plvItem->pszText) { *pResult = TRUE; return; }

		// change the value
		BOOL res = SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
		
		// if 'Extend?' value changed
		if( res && plvItem->iSubItem == EXT_COLNUM)// mod on 3/16/2004 plvItem->iItem >= 0)
		{
			bool bYes = strcmp( plvItem->pszText, "yes") == 0;
			const char* repValArray[] = { "self", "none"};
			const char* repVal = repValArray[bYes?0:1];

			// update the responsible column accordingly
			SetItemText(plvItem->iItem, RSP_COLNUM, repVal);

			// change the extended and responsible column of the relatives
			std::vector< int > res;
			m_parent->getDescsAncs( !bYes, GetItemData( plvItem->iItem), res); // get Descendants if ext == "no", otherwise Ancestors

			// for each relative
			for( unsigned int desc_i = 0; desc_i < res.size(); ++desc_i)
			{
				unsigned int desc_r_id = res[desc_i];
				for (int i=0;i < GetItemCount();i++)
				{
					// if element found
					if ( GetItemData(i) == desc_r_id)
					{
						// change the extended value as for the current one
						// in case of 'yes' all ancestors need 'yes'
						// in case of 'no'  all descendants need 'no'
						SetItemText( i, EXT_COLNUM, plvItem->pszText);

						// update the responsible column accordingly
						SetItemText( i, RSP_COLNUM, repVal);
					}
				}
			}
		}
	}

	*pResult = TRUE;
}





// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.
// col		- to hold the column index
int CTable::HitTestEx(CPoint &point, int *col) const
{
	int colnum = 0;
	int row = HitTest( point, NULL );
	
	if( col ) *col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;

	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if( bottom > GetItemCount() )
		bottom = GetItemCount();
	
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect( row, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( colnum = 0; colnum < nColumnCount; colnum++ )
			{
				int colwidth = GetColumnWidth(colnum);
				if( point.x >= rect.left 
					&& point.x <= (rect.left + colwidth ) )
				{
					if( col ) *col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}


// ShowInPlaceList		- Creates an in-place drop down list for any 
//				- cell in the list view control
// Returns			- A temporary pointer to the combo-box control
// nItem			- The row index of the cell
// nCol				- The column index of the cell
// lstItems			- A list of strings to populate the control with
// nSel				- Index of the initial selection in the drop down list
CComboBox* CTable::ShowInPlaceList( int nItem, int nCol, 
					CStringList &lstItems, int nSel )
{
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if( !EnsureVisible( nItem, TRUE ) ) return NULL;

	// Make sure that nCol is valid 
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 10 ) 
		return NULL;

	// Get the column offset
	int offset = 0;
	for( int i = 0; i < nCol; i++ )
		offset += GetColumnWidth( i );

	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS );

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}

	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
	int height = rect.bottom-rect.top;
	rect.bottom += (lstItems.GetCount()+1)*height;
	if( rect.right > rcClient.right) rect.right = rcClient.right;

	DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE
					|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;
	CComboBox *pList = new CInPlaceList(nItem, nCol, &lstItems, nSel);
	pList->Create( dwStyle, rect, this, IDC_IPEDIT );
	pList->SetItemHeight( -1, height);
	pList->SetHorizontalExtent( GetColumnWidth( nCol ));

	
	return pList;
}



// EditSubLabel		- Start edit of a sub item label
// Returns		- Temporary pointer to the new edit control
// nItem		- The row index of the item to edit
// nCol			- The column of the sub item.
CEdit* CTable::EditSubLabel( int nItem, int nCol )
{
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if( !EnsureVisible( nItem, TRUE ) ) return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth(nCol) < 5 )
		return NULL;

	// Get the column offset
	int offset = 0;
	for( int i = 0; i < nCol; i++ )
		offset += GetColumnWidth( i );

	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS );

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn( nCol, &lvcol );
	DWORD dwStyle ;
	if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
	if( rect.right > rcClient.right) rect.right = rcClient.right;

	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	CEdit *pEdit = new CInPlaceEdit(nItem, nCol, GetItemText( nItem, nCol ));
	pEdit->Create( dwStyle, rect, this, IDC_IPEDIT );


	return pEdit;
}

void CTable::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}



void CTable::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( GetFocus() != this ) SetFocus();	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CTable::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int index;
	CListCtrl::OnLButtonDown(nFlags, point);

	int colnum;
	if( ( index = HitTestEx( point, &colnum )) != -1 )
	{
		UINT flag = LVIS_FOCUSED;
		if( (GetItemState( index, flag ) & flag) == flag )
		{
			// Add check for LVS_EDITLABELS
			/*if( GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )*/
			/*if(colnum==1)
			{
				char buff[64];
				LVITEM item;
				item.mask = LVIF_TEXT;
				item.iItem = index;
				item.iSubItem = 1;
				item.pszText = buff;
				item.cchTextMax = 63;
				if (GetItem(&item) &&  CString(item.pszText) != "N/A") {
					CStringList lstItems;
					int rowID = GetItemData( index);
					//mmtheAspectDlg->GetAspects( rowID, lstItems);
					ShowInPlaceList( index, colnum, lstItems, 0 );
				}
			}*/
			if(colnum==EXT_COLNUM)
			{
				CStringList lstItems;
				lstItems.AddTail("yes");
				lstItems.AddTail("no");
				ShowInPlaceList( index, colnum, lstItems, 0 );
			}
			else if(colnum==RSP_COLNUM)
			{
				DWORD_PTR rowID = GetItemData( index);
				CString role, repr; bool ext;
				if( getRow( rowID, role, ext, repr) && ext == false) // if not intended for extension
				{
					CStringList lstItems;
					lstItems.AddTail("none");
					if( m_parent->addPossibleAncestors( lstItems, rowID)) // some choices added
						ShowInPlaceList( index, colnum, lstItems, 0 );
					else
						AfxMessageBox("This class does not have a base of the same kind. For BON2 reasons can't be captured by its superclass.");
				}
			}

		}
		else
			SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED , 
					LVIS_SELECTED | LVIS_FOCUSED);
	}
}

int CTable::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	int col0size = 9*GetStringWidth("TipicalLongRolenName")/4;
	int col1size = 4*GetStringWidth("R");
	int col2size = 13*GetStringWidth("Extend")/9;
	int col3size = 3*GetStringWidth("TypicalLongRoleName")/2;

	InsertColumn(0, _T("Class"), LVCFMT_LEFT,  col0size, -1);
	InsertColumn(1, _T("Stereotype"), LVCFMT_LEFT, col1size, -1);
	InsertColumn(EXT_COLNUM, _T("Extend"), LVCFMT_LEFT, col2size, -1);
	InsertColumn(RSP_COLNUM, _T("Capture as"), LVCFMT_LEFT, col3size, -1);

	return 0;
}

void CTable::addRow(int rowID, CString& role, char kind, int clique, bool generate, CString& repr)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = GetItemCount();
	lvItem.iSubItem = 0;
	lvItem.pszText = role.GetBuffer(role.GetLength());
	int index = InsertItem(&lvItem);
	
	char b[2] = { kind, 0 };
	lvItem.iSubItem = 1;
	lvItem.pszText = b;
	SetItem(&lvItem);

	lvItem.iSubItem = EXT_COLNUM;
	lvItem.pszText = generate ? _T("yes"): _T("no");
	SetItem(&lvItem);

	lvItem.iSubItem = RSP_COLNUM;
	lvItem.pszText = generate ?_T("self"): repr.IsEmpty() ? _T("none") : repr;
	SetItem(&lvItem);


	SetItemData(index, rowID);
	
}

bool CTable::getRow(int rowID, CString &role, bool &generate, CString &repr)
{
	LVFINDINFO lvFind;
	lvFind.flags = LVFI_PARAM;
	lvFind.lParam = rowID;
	int idx = FindItem(&lvFind);

	if (idx == -1)
		return false;


	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = idx;
	lvItem.pszText = role.GetBuffer(255);
	lvItem.cchTextMax = 255;
	lvItem.iSubItem = 0;
	GetItem(&lvItem);
	
	CString tmpStr;
	lvItem.pszText = tmpStr.GetBuffer(10);
	lvItem.cchTextMax = 10;
	lvItem.iSubItem = EXT_COLNUM;
	GetItem(&lvItem);
	generate = (tmpStr == "yes");

	lvItem.pszText = repr.GetBuffer(255);
	lvItem.cchTextMax = 255;
	lvItem.iSubItem = RSP_COLNUM;
	GetItem(&lvItem);

	return true;
}

void CTable::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CListCtrl& ListCtrl = *this;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	UINT uiFlags = ILD_TRANSPARENT;
	int nItem = lpDrawItemStruct->itemID;
	BOOL bFocus = (GetFocus() == this);
	COLORREF clrTextSave, clrBkSave;
	static _TCHAR szBuff[MAX_PATH];
	static _TCHAR szBuff2[MAX_PATH] = _T("c");
	LPCTSTR pszText;

// get item data

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff);
	lvi.stateMask = 0xFFFF;     // get all state flags
	ListCtrl.GetItem(&lvi);

	BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

// set colors if item is selected

	CRect rcAllLabels;
	ListCtrl.GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);

	CRect rcLabel;
	ListCtrl.GetItemRect(nItem, rcLabel, LVIR_LABEL);

	rcAllLabels.left = rcLabel.left;
	if (rcAllLabels.right<m_cxClient)
		rcAllLabels.right = m_cxClient;

	if (bSelected)
	{
		// the return value is the previous color, so save it
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		pDC->FillRect(rcAllLabels, &CBrush( ::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else
		pDC->FillRect(rcAllLabels, &CBrush( ::GetSysColor(COLOR_WINDOW)));

// draw item label

	ListCtrl.GetItemRect(nItem, rcItem, LVIR_LABEL);

	pszText = MakeShortString(pDC, szBuff, rcItem.right-rcItem.left, 2*OFFSET_FIRST);

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	pDC->DrawText(pszText,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);


// draw labels for extra columns

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

	for(int nColumn = 1; ListCtrl.GetColumn(nColumn, &lvc); nColumn++)
	{
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		int nRetLen = ListCtrl.GetItemText(nItem, nColumn,
						szBuff, sizeof(szBuff));
		if (nRetLen == 0)
			continue;

		pszText = MakeShortString(pDC, szBuff,
			rcItem.right - rcItem.left, 2*OFFSET_OTHER);

		UINT nJustify = DT_LEFT;

		if(pszText == szBuff)
		{
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
			{
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;
				break;
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
			default:
				break;
			}
		}

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText, -1, rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	}

	// draw focus rectangle if item has focus
	//if (lvi.state & LVIS_FOCUSED && bFocus) pDC->DrawFocusRect(rcAllLabels);

	
// set original colors if item was selected

	if (bSelected)
	{
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

LPCTSTR CTable::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
	static const _TCHAR szThreeDots[] = _T("...");

	int nStringLen = lstrlen(lpszLong);

	if(nStringLen == 0 ||
		(pDC->GetTextExtent(lpszLong, nStringLen).cx + nOffset) <= nColumnLen)
	{
		return(lpszLong);
	}

	static _TCHAR szShort[MAX_PATH];

	lstrcpy(szShort,lpszLong);
	int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

	for(int i = nStringLen-1; i > 0; i--)
	{
		szShort[i] = 0;
		if((pDC->GetTextExtent(szShort, i).cx + nOffset + nAddLen)
			<= nColumnLen)
		{
			break;
		}
	}

	lstrcat(szShort, szThreeDots);
	return(szShort);
}

void CTable::OnSize(UINT nType, int cx, int cy)
{
	m_cxClient = cx;
	CListCtrl::OnSize(nType, cx, cy);
}

void CTable::OnPaint()
{
	// full row select mode: need to extend the clipping region
	// so we can paint a selection all the way to the right
	if ((GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
	{
		CRect rcAllLabels;
		GetItemRect(0, rcAllLabels, LVIR_BOUNDS);

		if(rcAllLabels.right < m_cxClient)
		{
			// need to call BeginPaint (in CPaintDC c-tor)
			// to get correct clipping rect
			CPaintDC dc(this);

			CRect rcClip;
			dc.GetClipBox(rcClip);

			rcClip.left = min(rcAllLabels.right-1, rcClip.left);
			rcClip.right = m_cxClient;

			InvalidateRect(rcClip, FALSE);
			// EndPaint will be called in CPaintDC d-tor
		}
	}

	CListCtrl::OnPaint();
}

void CTable::OnSort(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pNMLV = (NMLISTVIEW*) pNMHDR;
	ASSERT( pNMLV->iItem == -1);
	ASSERT( pNMLV->iSubItem >= 0);

	switch( pNMLV->iSubItem) {
		case 0:
			this->SortItems( SelConf::MyNameCompare, (LPARAM) m_parent);
			break;
		case 1:
			this->SortItems( SelConf::MyKindCompare, (LPARAM) m_parent);
			break;
		case 2:
			this->SortItems( SelConf::MyExtdCompare, (LPARAM) m_parent);
			break;
		case 3:
			this->SortItems( SelConf::MyReprCompare, (LPARAM) m_parent);
			break;
		default:
			ASSERT(0);
	};
	
	// after sort is done we revert the direction of the next sort with:
	SelConf::m_sortOrder *= -1;
}
