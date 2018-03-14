// SyncObjsListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GMEApp.h"
#include "SyncObjsListCtrl.h"
#include "GMEstd.h"
#include "GuiObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSyncObjsListCtrl

bool CSyncObjsListCtrl::isAscending[] = {true, true, true};

CSyncObjsListCtrl::CSyncObjsListCtrl()
{
}

CSyncObjsListCtrl::~CSyncObjsListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSyncObjsListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSyncObjsListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncObjsListCtrl message handlers

void CSyncObjsListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	SortItems(ObjsCompare, pNMListView->iSubItem);
	
	*pResult = 0;
}

int CALLBACK CSyncObjsListCtrl::ObjsCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CGuiObject* pItem1 = (CGuiObject*) lParam1;
	CGuiObject* pItem2 = (CGuiObject*) lParam2;
	int nResult = 0;

	switch (lParamSort) {

	case 0: // Object name.
		nResult = (isAscending[0] ? 1 : -1) * pItem1->name.CompareNoCase(pItem2->name);
		isAscending[0] = !isAscending[0];
		break;
	case 1: // Kind name.
		nResult = (isAscending[1] ? 1 : -1) * pItem1->kindName.CompareNoCase(pItem2->kindName);
		isAscending[1] = !isAscending[1];
		break;
	case 2: // Role name.
		nResult = (isAscending[2] ? 1 : -1) * pItem1->roleName.CompareNoCase(pItem2->roleName);
		isAscending[2] = !isAscending[2];
		break;
	}
	return nResult;
}
