// AspectSpecTbl.cpp : implementation file
//

#include "stdafx.h"
#include "inplaceedit.h"
#include "inplacelist.h"
#include "aspectspectbl.h"
#include "aspectspecdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAspectSpecTbl

CAspectSpecTbl::CAspectSpecTbl()
{
}

CAspectSpecTbl::~CAspectSpecTbl()
{
}


BEGIN_MESSAGE_MAP(CAspectSpecTbl, CListCtrl)
	//{{AFX_MSG_MAP(CAspectSpecTbl)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAspectSpecTbl message handlers

void CAspectSpecTbl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
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
int CAspectSpecTbl::HitTestEx(CPoint &point, int *col) const
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
CComboBox* CAspectSpecTbl::ShowInPlaceList( int nItem, int nCol, 
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
	if( rect.bottom > rcClient.bottom + 4*height) // by zolmol
	{
		rect.bottom = rcClient.bottom + 4*height;//close to the propertypage's bottom
		dwStyle |= WS_VSCROLL; // enable scrollbar
	} // end zolmol

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
CEdit* CAspectSpecTbl::EditSubLabel( int nItem, int nCol )
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

void CAspectSpecTbl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}



void CAspectSpecTbl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( GetFocus() != this ) SetFocus();	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}


/*
void CAspectSpecTbl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CListCtrl::OnLButtonDown(nFlags, point);

	int index;
	int colnum;
	if( ( index = HitTestEx( point, &colnum )) != -1 )
	{
		UINT flag = LVIS_FOCUSED;
		if( (GetItemState( index, flag ) & flag) == flag && colnum > 0)
		{
			// Add check for LVS_EDITLABELS
			if( GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )
				EditSubLabel( index, colnum );
		}
		else
			SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED ,
				    	LVIS_SELECTED | LVIS_FOCUSED); 
	}

}
*/
void CAspectSpecTbl::OnLButtonDown(UINT nFlags, CPoint point) 
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
			if(colnum==1)
			{
				char buff[64];
				LVITEM item;
				item.mask = LVIF_TEXT;
				item.iItem = index;
				item.iSubItem = 1;
				item.pszText = buff;
				item.cchTextMax = 63;
				if (GetItem(&item) && CString(item.pszText) != "N/A") {
					CStringList lstItems;
					int rowID = GetItemData( index);
					//theAspectDlg->GetAspects(lstItems);
					theAspectDlg->GetAspects( rowID, lstItems);
					ShowInPlaceList( index, colnum, lstItems, 0 );
				}
			}
			if(colnum==2)
			{
				char buff[64];
				LVITEM item;
				item.mask = LVIF_TEXT;
				item.iItem = index;
				item.iSubItem = 2;
				item.pszText = buff;
				item.cchTextMax = 63;
				if (GetItem(&item) && CString(item.pszText) != "N/A") {
					CStringList lstItems;
					lstItems.AddTail("yes");
					lstItems.AddTail("no");
					ShowInPlaceList( index, colnum, lstItems, 0 );
				}
			}

		}
		else
			SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED , 
					LVIS_SELECTED | LVIS_FOCUSED);
	}
}



void CAspectSpecTbl::AddRow(int rowID, CString& role, CString& kindAspect, CString& isPrimary)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = GetItemCount();
	lvItem.iSubItem = 0;
	lvItem.pszText = role.GetBuffer(role.GetLength());
	int index = InsertItem(&lvItem);
	

	lvItem.iSubItem = 1;
	lvItem.pszText = kindAspect.GetBuffer( kindAspect.GetLength()); // modif role.getl
	SetItem(&lvItem);

	lvItem.iSubItem = 2;
	lvItem.pszText = isPrimary.GetBuffer( isPrimary.GetLength());// ? _T("yes"): _T("no");
	// lvItem.stateMask = LVIS_STATEIMAGEMASK;
	// lvItem.state = INDEXTOSTATEIMAGEMASK(isPrimary ? 0 : 1);
	// lvItem.iImage = isPrimary ? 0 : 1;

	SetItem(&lvItem);

	SetItemData(index, rowID);
	
}

int CAspectSpecTbl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// m_checkImages.Create(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), ILC_COLOR4 | ILC_MASK, 2, 1);
	// CWinApp *pApp = AfxGetApp();
	// HICON hIcon = pApp->LoadIcon(IDI_ICON_UNCHECKED);
	// if (hIcon) {
	//	m_checkImages.Add(hIcon);
	//	::DeleteObject(hIcon);
	// }

	// hIcon = pApp->LoadIcon(IDI_ICON_CHECKED);
	// if (hIcon) {
	// 	m_checkImages.Add(hIcon);
	//	::DeleteObject(hIcon);
	// }

	// SetImageList(&m_checkImages, LVSIL_NORMAL);
	
	// ListView_SetExtendedListViewStyle (m_hWnd, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	// TODO: Add your specialized creation code here
	int col1size = 3*GetStringWidth("TipicalContainerN::LongRoleName")/2;
	int col2size = 3*GetStringWidth("LongAspectName")/2;
	int col3size = 3*GetStringWidth("Primary")/2;

	InsertColumn(0, _T("Role"), LVCFMT_LEFT,  col1size, -1);
	InsertColumn(1, _T("KindAspect"), LVCFMT_LEFT, col2size, -1);
	InsertColumn(2, _T("Primary"), LVCFMT_LEFT, col3size, -1);

	return 0;
}

bool CAspectSpecTbl::GetRow(int rowID, CString &role, CString &kindAspect, CString& isPrimary)
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
	lvItem.cchTextMax = 254;
	lvItem.iSubItem = 0;
	GetItem(&lvItem);
	
	

	lvItem.iSubItem = 1;
	lvItem.pszText = kindAspect.GetBuffer(255);
	lvItem.cchTextMax = 254;
	GetItem(&lvItem);


	//CString tmpStr;
	lvItem.pszText = isPrimary.GetBuffer(10); //tmpStr
	lvItem.cchTextMax = 9;
	lvItem.iSubItem = 2;
	GetItem(&lvItem);
	//isPrimary = (tmpStr == "yes");
	
	return true;
}

