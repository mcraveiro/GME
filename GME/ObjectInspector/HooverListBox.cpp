// HooverListBox.cpp : implementation file
//

#include "stdafx.h"
#include "objectinspector.h"
#include "HooverListBox.h"
#include "InspectorDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHooverListBox

CHooverListBox::CHooverListBox()
{
}

CHooverListBox::~CHooverListBox()
{
}


BEGIN_MESSAGE_MAP(CHooverListBox, CListBox)
	//{{AFX_MSG_MAP(CHooverListBox)
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHooverListBox message handlers

void CHooverListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	BOOL bOutside;
	UINT uIndex=ItemFromPoint(point,bOutside);
	if(!bOutside)
	{
		if((int)uIndex!=GetCurSel())
		{
			SetCurSel(uIndex);
		}
	}
	
	CListBox::OnMouseMove(nFlags, point);
}

void CHooverListBox::OnKillFocus(CWnd* pNewWnd) 
{
	CString strWndText;
	if(pNewWnd)
	{
		pNewWnd->GetWindowText(strWndText);
		if(strWndText==_T("ArrowButton"))
		{
			GetParent()->SetFocus();
		}
	}
	CListBox::OnKillFocus(pNewWnd);
	
	ShowWindow(SW_HIDE);	
}


void CHooverListBox::OnSelChange() 
{
	ShowWindow(SW_HIDE);
	int nSelIndex=GetCurSel();
	if(nSelIndex!=LB_ERR)
	{
		GetParent()->SendMessage(HLB_SELENDOK,nSelIndex,0);
	}
}
