// GMEPanningWindow.cpp: implementation of the CGMEPanningWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMEPanningWindow.h"
#include "GMEApp.h"
#include "MainFrm.h"
#include "GMEView.h"


/////////////////////////////////////////////////////////////////////////////
// CGMEPanningWindow

CGMEPanningWindow* CGMEPanningWindow::theInstance = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGMEPanningWindow::CGMEPanningWindow()
{
	//{{AFX_DATA_INIT(CGMEPanningWindow)
	//}}AFX_DATA_INIT

	VERIFY(theInstance == 0);
	theInstance = this;
}


BEGIN_MESSAGE_MAP(CGMEPanningWindow, CDockablePane)
	//{{AFX_MSG_MAP(CGMEPanningWindow)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CGMEPanningWindow, CDockablePane)
	//{{AFX_EVENTSINK_MAP(CGMEPanningWindow)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEPanningWindow message handlers

int CGMEPanningWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->cx = lpCreateStruct->cy = 150;
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_PanningWindowWrapper.Create(_T("PanningWindow"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 150, 150), this, IDC_PANNINGWINDOWCTRL1))
		return -1;

	return 0;
}


void CGMEPanningWindow::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CDockablePane::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);

	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}


void CGMEPanningWindow::OnSize(UINT nType, int cx, int cy)
{
	// cx = cy = 100;
	CDockablePane::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(rc);

	m_PanningWindowWrapper.MoveWindow(rc);
}


void CGMEPanningWindow::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CGMEPanningWindow)
	DDX_Control(pDX, IDC_PANNINGWINDOWCTRL1, m_PanningWindowWrapper);
	//}}AFX_DATA_MAP
}


/* 
ActiveX controls do not have their message pump, it is owned by their containers.
The container in case of GME is a kind of control bar, which is treated as a dialog.
Dialog box messages are filtered by ::IsDialogMessage, which does not allow the 
default dialog kestroke messages (ESC - close dialog, ENTER - push default button
TAB - next item in the tab order etc...) to be propagated to the controls placed on 
the dialog.

Here we avoid calling the default PreTranslateMessage which filtered by 
::IsDialogMessage, dispatch it directly to the controls.

Tihamer

*/

BOOL CGMEPanningWindow::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
			case VK_RETURN:
			case VK_ESCAPE:
			case VK_DELETE:
			// Modification by Volgyesi (undo problems)
			case VK_CONTROL:
			case 'z':
			case 'Z':
			// Modification End
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
		}
	}

	return CDockablePane::PreTranslateMessage(pMsg);
}


void CGMEPanningWindow::SetBitmapDC(HWND owner, HDC bdc, HBITMAP oldBmp, CRect& ori, CRect& rect, COLORREF& bkgrnd)
{
	m_PanningWindowWrapper.SetBitmapDC((ULONGLONG)owner, (ULONGLONG)bdc, (ULONGLONG)oldBmp, ori.left, ori.top, ori.Width(), ori.Height(),
										rect.left, rect.top, rect.Width(), rect.Height(), (OLE_COLOR)bkgrnd);
}


void CGMEPanningWindow::SetViewRect(CRect vrect)
{
	m_PanningWindowWrapper.SetViewRect(vrect.left, vrect.top, vrect.Width(), vrect.Height());
}

void CGMEPanningWindow::ShowPane(BOOL bShow, BOOL bDelay, BOOL bActivate/* = TRUE*/)
{
	CDockablePane::ShowPane(bShow, bDelay, bActivate);
	if (bShow)
	{
		CMDIChildWnd *pChild  = CMainFrame::theInstance->MDIGetActive();
		if (pChild) 
		{
			CGMEView *view = (CGMEView*)pChild->GetActiveView();
			if (view) 
			{
				view->DoPannWinRefresh();
				view->notifyPanning(view->GetDeviceScrollPosition());
			}
		}
	}
}
