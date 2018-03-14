// InPlaceCommon.cpp : implementation file
//
// Common functions used by in-place popup
// (color, combo-box, compass-check, compass-opt, etc.) dialogs
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InPlaceCommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void RelayMouseClickToInspectorList(CWnd* inspectorList, CPoint screenPoint)
{
	ASSERT(inspectorList != NULL);
	if (inspectorList == NULL)
		return;

	CPoint cPoint = screenPoint;
	inspectorList->ScreenToClient(&cPoint);
	inspectorList->PostMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(cPoint.x, cPoint.y));
	inspectorList->PostMessage(WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(cPoint.x, cPoint.y));
}
