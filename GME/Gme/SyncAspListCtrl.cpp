// SyncAspListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GMEApp.h"
#include "SyncAspListCtrl.h"
#include "GMEstd.h"
#include "GuiMeta.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSyncAspListCtrl

bool CSyncAspListCtrl::isAscending[] = {true};

CSyncAspListCtrl::CSyncAspListCtrl()
{
}

CSyncAspListCtrl::~CSyncAspListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSyncAspListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSyncAspListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncAspListCtrl message handlers

void CSyncAspListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	SortItems(AspectCompare, pNMListView->iSubItem);
	
	*pResult = 0;
}

int CALLBACK CSyncAspListCtrl::AspectCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CGuiMetaAspect* pItem1 = (CGuiMetaAspect*) lParam1;
	CGuiMetaAspect* pItem2 = (CGuiMetaAspect*) lParam2;
	int nResult = 0;

	switch (lParamSort) {

	case 0: // Aspect name.
		nResult = (isAscending[0] ? 1 : -1) * pItem1->name.CompareNoCase(pItem2->name);
		isAscending[0] = !isAscending[0];
		break;
	}
	return nResult;
}
