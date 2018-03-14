// AnnotationPanel.cpp : implementation file
//

#include "stdafx.h"
#include "mgautil.h"
#include "AnnotationPanel.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnnotationPanel

CAnnotationPanel::CAnnotationPanel()
{
}

CAnnotationPanel::~CAnnotationPanel()
{
}


BEGIN_MESSAGE_MAP(CAnnotationPanel, CButton)
	//{{AFX_MSG_MAP(CAnnotationPanel)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnnotationPanel message handlers

void CAnnotationPanel::OnSize(UINT nType, int cx, int cy) 
{
	CButton::OnSize(nType, cx, cy);
	
	CDialog *prnt = (CDialog*)(GetParent());

	CWnd *ctrl;

	// Labels 
	ctrl = prnt->GetDlgItem(IDC_STATIC_NAME);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,14,13+22,14+8);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_TEXT);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,30,13+17,30+8);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_COLOR);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,97+16,13+45,97+8+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_BGCOLOR);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,113+16,13+45,113+8+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_SHADOWCOLOR);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,129+16,13+45,129+8+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_GRADIENTCOLOR);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,145+16,13+45,145+8+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_GRADIENTDIR);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(75,162+16,75+51,162+8+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_SHADOWDEPTH);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(75,178+16,75+51,178+8+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_SHADOWDIRECTION);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(75,194+16,75+51,194+8+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_FONT);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,226+16,13+17,226+8+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_CORNERRADIUS);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(75,210+16,75+17,210+8+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	// Edit Fields
	ctrl = prnt->GetDlgItem(IDC_EDIT_NAME);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(38,11,38+155,11+14);
		CRect origRect2(192,11,200,11+14);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}
	
	ctrl = prnt->GetDlgItem(IDC_EDIT_TEXT);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(38,30,38+155,30+60);
		CRect origRect2(192,30,200,30+60);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_EDIT_GRADIENTDIR);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(135,158+16,135+55,158+14+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_EDIT_SHADOWDEPTH);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(135,174+16,135+55,174+14+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_EDIT_SHADOWDIRECTION);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(135,190+16,135+55,190+14+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_EDIT_CORNERRADIUS);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(135,206+16,135+55,206+14+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	// inheritable checkbox
	ctrl = prnt->GetDlgItem(IDC_CHECK1);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,94,13+55,94+14);
		//CRect origRect2(192,94,200,94+14);
		prnt->MapDialogRect(&origRect);
		//prnt->MapDialogRect(&origRect2);
		//origRect.right = cx - (origRect2.right - origRect2.left);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	// reDerive 
	ctrl = prnt->GetDlgItem(IDC_REDERIVEBUTTON);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13+55+10,94,13+55+10+54,94+14);
		//CRect origRect2(192,94,200,94+14);
		prnt->MapDialogRect(&origRect);
		//prnt->MapDialogRect(&origRect2);
		//origRect.right = cx - (origRect2.right - origRect2.left);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	// showHide
	ctrl = prnt->GetDlgItem(IDC_SHOWHIDEBUTTON);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13+55+10+54+10,94,13+55+10+54+10+54,94+14);
		CRect origRect2(192,94,200,94+14);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_COMBO_COLOR);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(58,94+16,38+135,94+14+16);
		CRect origRect2(192,94+16,200,94+14+16);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_COMBO_BGCOLOR);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(58,110+16,38+135,110+14+16);
		CRect origRect2(192,110+16,200,110+14+16);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_COMBO_SHADOWCOLOR);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(58,126+16,38+135,126+14+16);
		CRect origRect2(192,126+16,200,126+14+16);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_COMBO_GRADIENTCOLOR);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(58,142+16,38+135,142+14+16);
		CRect origRect2(192,142+16,200,142+14+16);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_CHECK_GRADIENTFILL);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,162+16,13+55,162+10+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_CHECK_CASTSHADOW);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,177+16,13+55,177+10+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_CHECK_ROUNDCORNER);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(13,209+16,13+55,209+10+16);
		prnt->MapDialogRect(&origRect);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_EDIT_FONT);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(58,223+16,58+115,223+14+16);
		CRect origRect2(167,223+16,200,223+14+16);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_BUTTON_FONT);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(173,223+16,173+20,223+14+16);
		CRect origRect2(192,223+16,200,223+14+16);
		prnt->MapDialogRect(&origRect);
		int sx = origRect.right - origRect.left;
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		origRect.left = origRect.right - sx;
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_VISIBILITY);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(3,235+16,3+190,235+70+16);
		CRect origRect2(197,235+16,200,235+70+16);
		CRect origRect3(3,318+16,3+190,318+3+16);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		prnt->MapDialogRect(&origRect3);
		origRect.right = cx - (origRect2.right - origRect2.left);
		origRect.bottom = cy - (origRect3.bottom - origRect3.top);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_LIST_ASPECT);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(7,244+16,7+120,244+55+16);
		CRect origRect2(130,244+16,200,244+55+16);
		CRect origRect3(7,299+16,7+120,299+7+16);
		prnt->MapDialogRect(&origRect);
		prnt->MapDialogRect(&origRect2);
		prnt->MapDialogRect(&origRect3);
		origRect.right = cx - (origRect2.right - origRect2.left);
		origRect.bottom = cy - (origRect3.bottom - origRect3.top);
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_XCOORD);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(141,252+16,141+8,252+8+16);
		CRect origRect2(148,252+16,200,252+8+16);
		prnt->MapDialogRect(&origRect);
		int sx = origRect.right - origRect.left;
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		origRect.left = origRect.right - sx;
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_STATIC_YCOORD);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(141,274+16,141+8,274+8+16);
		CRect origRect2(148,274+16,200,274+8+16);
		prnt->MapDialogRect(&origRect);
		int sx = origRect.right - origRect.left;
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		origRect.left = origRect.right - sx;
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_EDIT_XCOORD);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(151,248+16,151+40,248+14+16);
		CRect origRect2(190,248+16,200,248+14+16);
		prnt->MapDialogRect(&origRect);
		int sx = origRect.right - origRect.left;
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		origRect.left = origRect.right - sx;
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_EDIT_YCOORD);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(151,269+16,151+40,269+14+16);
		CRect origRect2(190,269+16,200,269+14+16);
		prnt->MapDialogRect(&origRect);
		int sx = origRect.right - origRect.left;
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		origRect.left = origRect.right - sx;
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}

	ctrl = prnt->GetDlgItem(IDC_CHECK_DEFPOS);
	if (ctrl->GetSafeHwnd()) {
		CRect origRect(141,289+16,141+53,289+10+16);
		CRect origRect2(193,289+16,200,289+10+16);
		prnt->MapDialogRect(&origRect);
		int sx = origRect.right - origRect.left;
		prnt->MapDialogRect(&origRect2);
		origRect.right = cx - (origRect2.right - origRect2.left);
		origRect.left = origRect.right - sx;
		ClientToScreen(&origRect);
		prnt->ScreenToClient(&origRect);
		ctrl->MoveWindow(&origRect);
	}
}

