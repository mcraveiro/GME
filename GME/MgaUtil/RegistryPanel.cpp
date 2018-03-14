// RegistryPanel.cpp : implementation file
//

#include "stdafx.h"
#include "mgautil.h"
#include "RegistryPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegistryPanel

CRegistryPanel::CRegistryPanel()
{
}

CRegistryPanel::~CRegistryPanel()
{
}


BEGIN_MESSAGE_MAP(CRegistryPanel, CButton)
	//{{AFX_MSG_MAP(CRegistryPanel)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegistryPanel message handlers

void CRegistryPanel::OnSize(UINT nType, int cx, int cy) 
{
	CButton::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
	CDialog *prnt = (CDialog*)(GetParent());

	CWnd *label;
		
	label = prnt->GetDlgItem(IDC_STATIC_PATH);
	if (label->GetSafeHwnd()) {
		CRect origRect(10,20,28,28);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		label->MoveWindow(&origRect);
	}

	label = prnt->GetDlgItem(IDC_STATIC_STATUS);
	if (label->GetSafeHwnd()) {
		CRect origRect(10,40,33,48);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		label->MoveWindow(&origRect);
	}

	label = prnt->GetDlgItem(IDC_STATIC_VALUE);
	if (label->GetSafeHwnd()) {
		CRect origRect(10,60,31,68);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		label->MoveWindow(&origRect);
	}

	label = prnt->GetDlgItem(IDC_REGNODE_PATH);
	if (label->GetSafeHwnd()) {
		CRect origRect(35,17,190,31);
		CRect origRect2(190,17,200,31);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);

		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		label->MoveWindow(&origRect);
	}

	label = prnt->GetDlgItem(IDC_REGNODE_STATUS);
	if (label->GetSafeHwnd()) {
		CRect origRect(35,37,190,51);
		CRect origRect2(190,37,200,51);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);

		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		label->MoveWindow(&origRect);
	}	
	
	label = prnt->GetDlgItem(IDC_REGNODE_VALUE);
	if (label->GetSafeHwnd()) {
		CRect origRect(35,57,190,172);
		CRect origRect2(190,57,200,172);
		CRect origRect3(35,172,200,182);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		prnt->MapDialogRect(&origRect3);
		origRect.right = cx - (origRect2.right - origRect2.left);
		origRect.bottom = cy - (origRect3.bottom - origRect3.top);

		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		label->MoveWindow(&origRect);
	}	
}
