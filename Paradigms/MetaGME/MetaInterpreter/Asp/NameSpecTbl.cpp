// AspectSpecTbl.cpp : implementation file
//

#include "stdafx.h"
#include "inplaceedit.h"
#include "inplacelist.h"
#include "NameSpecTbl.h"
#include "NameSpecDlg.h"
extern NameSpecDlg * dlg;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define OFFSET_FIRST    2
#define OFFSET_OTHER    6

/////////////////////////////////////////////////////////////////////////////
// NameSpecTbl

NameSpecTbl::NameSpecTbl()
{
}

NameSpecTbl::~NameSpecTbl()
{
}


BEGIN_MESSAGE_MAP(NameSpecTbl, CListCtrl)
	//{{AFX_MSG_MAP(NameSpecTbl)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NameSpecTbl message handlers

BOOL NameSpecTbl::PreCreateWindow( CREATESTRUCT &cs)
{
	cs.style |= WS_TABSTOP|LVS_REPORT|LVS_SINGLESEL|LVS_NOSORTHEADER|LVS_OWNERDRAWFIXED;

	return CListCtrl::PreCreateWindow( cs);
}

void NameSpecTbl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here

	LV_ITEM	*plvItem = &pDispInfo->item;

	if (plvItem->pszText != NULL)
	{
		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}

	*pResult = TRUE;
}





// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.
// col		- to hold the column index
int NameSpecTbl::HitTestEx(CPoint &point, int *col) const
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
CComboBox* NameSpecTbl::ShowInPlaceList( int nItem, int nCol, 
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

	rect.left += offset+ 1*OFFSET_FIRST;
	rect.right = rect.left + GetColumnWidth( nCol );
	int height = rect.bottom-rect.top;
	rect.bottom += (lstItems.GetCount()+1)*height;
	if( rect.right > rcClient.right) rect.right = rcClient.right;

	DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE
					|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;
	if( rect.bottom > rcClient.bottom + 4*height) // by zolmol
	{
		rect.bottom = rcClient.bottom + 4*height;//close to the page's bottom
		dwStyle |= WS_VSCROLL; // enable scrollbar
	} // end zolmol

	CComboBox *pList = new CInPlaceList(nItem, nCol, &lstItems, nSel);
	pList->Create( dwStyle, rect, this, IDC_IPEDIT );
	pList->SetItemHeight( -1, height-1);
	pList->SetHorizontalExtent( GetColumnWidth( nCol ));
	pList->ShowDropDown();

	
	return pList;
}



// EditSubLabel		- Start edit of a sub item label
// Returns		- Temporary pointer to the new edit control
// nItem		- The row index of the item to edit
// nCol			- The column of the sub item.
CEdit* NameSpecTbl::EditSubLabel( int nItem, int nCol )
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

void NameSpecTbl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}



void NameSpecTbl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( GetFocus() != this ) SetFocus();	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}


void NameSpecTbl::OnLButtonDown(UINT nFlags, CPoint point) 
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
			if(colnum==0)
			{
				char buff[128];
				LVITEM item;
				item.mask = LVIF_TEXT;
				item.iItem = index;
				item.iSubItem = 0;
				item.pszText = buff;
				item.cchTextMax = 127;
				if (GetItem(&item) && CString(item.pszText) != "N/A") {
					CStringList lstItems;
					int rowID = GetItemData( index);
					dlg->GetNames( rowID, CString(item.pszText), lstItems);
					ShowInPlaceList( index, colnum, lstItems, 0 );
				}
			}
			else if(colnum==1)
			{
				char buff[128];
				LVITEM item;
				item.mask = LVIF_TEXT;
				item.iItem = index;
				item.iSubItem = 1;
				item.pszText = buff;
				item.cchTextMax = 127;
				if (GetItem(&item) && CString(item.pszText) != "N/A") {
					CStringList lstItems;
					int rowID = GetItemData( index);
					dlg->GetDispNames( rowID, CString(item.pszText), lstItems);
					ShowInPlaceList( index, colnum, lstItems, 0 );
				}
			}
		}
		else
			SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED , 
					LVIS_SELECTED | LVIS_FOCUSED);
	}
}



void NameSpecTbl::AddRow(int rowID, CString& name, CString& disp_name, CString& kind)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = GetItemCount();
	lvItem.iSubItem = 0;
	lvItem.pszText = name.GetBuffer( name.GetLength());
	int index = InsertItem(&lvItem);
	
	lvItem.iSubItem = 1;
	lvItem.pszText = disp_name.GetBuffer( disp_name.GetLength());
	SetItem(&lvItem);

	lvItem.iSubItem = 2;
	lvItem.pszText = kind.GetBuffer( kind.GetLength());
	SetItem(&lvItem);

	SetItemData(index, rowID);
}

int NameSpecTbl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	int col1size = 3*GetStringWidth("TipicalLongTipicalLongName")/2;
	int col2size = 3*GetStringWidth("KindName")/2;

	InsertColumn(0, _T("Name"), LVCFMT_LEFT,  col1size, -1);
	InsertColumn(1, _T("Displayed Name"), LVCFMT_LEFT,  col1size, -1);
	InsertColumn(2, _T("Kind"), LVCFMT_LEFT, col2size, -1);

	return 0;
}

bool NameSpecTbl::GetRow(int rowID, CString& name, CString& disp_name, CString& kind)
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
	lvItem.pszText = name.GetBuffer(255);
	lvItem.cchTextMax = 254;
	lvItem.iSubItem = 0;
	GetItem(&lvItem);
	
	
	lvItem.iSubItem = 1;
	lvItem.pszText = disp_name.GetBuffer(255);
	lvItem.cchTextMax = 254;
	GetItem(&lvItem);

	lvItem.iSubItem = 2;
	lvItem.pszText = kind.GetBuffer(255);
	lvItem.cchTextMax = 254;
	GetItem(&lvItem);

	return true;
}

void NameSpecTbl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	int nItem = lpDrawItemStruct->itemID;
	BOOL bFocus = (GetFocus() == this);
	COLORREF clrTextSave, clrBkSave;
	static _TCHAR szBuff[MAX_PATH];
	LPCTSTR pszText;

// get item data

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff);
	lvi.stateMask = 0xFFFF;     // get all state flags
	GetItem(&lvi);

	BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

// set colors if item is selected

	CRect rcAllLabels;
	GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);

	CRect rcLabel;
	GetItemRect(nItem, rcLabel, LVIR_LABEL);

	rcAllLabels.left = rcLabel.left;

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

	GetItemRect(nItem, rcItem, LVIR_LABEL);

	pszText = MakeShortString(pDC, szBuff, rcItem.right-rcItem.left, 2*OFFSET_FIRST);

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	pDC->DrawText(pszText,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

	// draw scroll down button MZ
	CRect rc = rcItem;

	rc.left = rc.right - ::GetSystemMetrics(SM_CXHSCROLL);
	//rc.top = rc.bottom - ::GetSystemMetrics(SM_CYVSCROLL);
	//rc.DeflateRect(1,1);


	if( rc.left > OFFSET_FIRST + 1)
		pDC->DrawFrameControl(rc, DFC_SCROLL, DFCS_SCROLLDOWN);

// draw labels for extra columns

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
	{
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		int nRetLen = GetItemText(nItem, nColumn,
						szBuff, sizeof(szBuff));
		if (nColumn != 1 && nRetLen == 0) // in case of the first column we will draw the DFCS_SCROLLDOWN symbol
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
		//rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText, -1, rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

		
		CRect r2 = rcLabel;
		r2.left = r2.right - ::GetSystemMetrics(SM_CXHSCROLL);
		if( nColumn == 1 && r2.left > OFFSET_FIRST + 1)
			pDC->DrawFrameControl(r2, DFC_SCROLL, DFCS_SCROLLDOWN);
	}

// draw focus rectangle if item has focus

	if (lvi.state & LVIS_FOCUSED && bFocus) pDC->DrawFocusRect(rcAllLabels);

	
// set original colors if item was selected

	if (bSelected)
	{
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

LPCTSTR NameSpecTbl::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
	static const _TCHAR szEmpty[] = _T("");
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

	if( nOffset + nAddLen > nColumnLen) return szEmpty;


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
