// InspectorSheet.cpp : implementation file
//

#include "stdafx.h"
#include "objectinspector.h"
#include "InspectorSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInspectorSheet

CInspectorSheet::CInspectorSheet()
{
}

CInspectorSheet::~CInspectorSheet()
{
}


void CInspectorSheet::SetInspectorLists(CInspectorList **inspectorList) {
	m_inspectorLists = inspectorList;
}

BEGIN_MESSAGE_MAP(CInspectorSheet, CTabCtrl)
	//{{AFX_MSG_MAP(CInspectorSheet)
	ON_WM_SIZE()
	ON_MESSAGE(LBN_ON_ITEM_CHANGED, OnItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectorSheet message handlers

void CInspectorSheet::OnSize(UINT nType, int cx, int cy) 
{
	CTabCtrl::OnSize(nType, cx, cy);
	
	CRect listRect;
	GetClientRect(&listRect);
	AdjustRect(FALSE, &listRect);
	for (int i = 0; i < INSP_PANEL_NUM; i++) {
		m_inspectorLists[i]->MoveWindow(listRect);
	}	
}

LRESULT CInspectorSheet::OnItemChanged(WPARAM wParam, LPARAM /*lParam*/)
{
/*
#ifdef _DEBUG
	CString strTemp;
	strTemp.Format(_T("Item Changed: %u"),wParam);
	MessageBox(strTemp,_T("Item Changed"));
#endif
*/
	GetParent()->SendMessage(LBN_ON_ITEM_CHANGED,wParam,GetCurFocus());

	return TRUE;
}