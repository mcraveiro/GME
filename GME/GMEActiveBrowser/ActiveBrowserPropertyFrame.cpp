// ActiveBrowserPropertyFrame.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ActiveBrowserPropertyFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActiveBrowserPropertyFrame

IMPLEMENT_DYNCREATE(CActiveBrowserPropertyFrame, CMiniFrameWnd)

CActiveBrowserPropertyFrame::CActiveBrowserPropertyFrame()
{
	m_pModelessPropSheet = NULL;
}

CActiveBrowserPropertyFrame::~CActiveBrowserPropertyFrame()
{
}


BEGIN_MESSAGE_MAP(CActiveBrowserPropertyFrame, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CActiveBrowserPropertyFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CActiveBrowserPropertyFrame message handlers

int CActiveBrowserPropertyFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pModelessPropSheet = new CActiveBrowserPropertySheet(this);

	if (!m_pModelessPropSheet->Create(this, 
		WS_CHILD|WS_VISIBLE, 0))
	{
		delete m_pModelessPropSheet;
		m_pModelessPropSheet = NULL;
		return -1;
	}

	m_pModelessPropSheet->ModifyStyleEx(0, WS_EX_CONTROLPARENT);

	// Resize the mini frame so that it fits around the child property
	// sheet.
	CRect rectClient, rectWindow;
	m_pModelessPropSheet->GetWindowRect(rectClient);
	rectWindow = rectClient;

	// CMiniFrameWnd::CalcWindowRect adds the extra width and height
	// needed from the mini frame.
	CalcWindowRect(rectWindow);
	SetWindowPos(NULL, rectWindow.left, rectWindow.top,
		rectWindow.Width(), rectWindow.Height(),
		SWP_NOZORDER | SWP_NOACTIVATE);
	m_pModelessPropSheet->SetWindowPos(NULL, 0, 0,
		rectClient.Width(), rectClient.Height(),
		SWP_NOZORDER | SWP_NOACTIVATE);

	m_pModelessPropSheet->SetParent(this);
	

	return 0;
}

void CActiveBrowserPropertyFrame::OnClose() 
{
	
	// Instead of closing the modeless property sheet, just hide it.
	ShowWindow(SW_HIDE);
}

void CActiveBrowserPropertyFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CMiniFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	// Forward any WM_ACTIVATEs to the property sheet...
	// Like the dialog manager itself, it needs them to save/restore the focus.
	ASSERT_VALID(m_pModelessPropSheet);

	// Use GetCurrentMessage to get unmodified message data.
	const MSG* pMsg = GetCurrentMessage();

	ASSERT(pMsg->message == WM_ACTIVATE);
	m_pModelessPropSheet->SendMessage(pMsg->message, pMsg->wParam,
		pMsg->lParam);
}

BOOL CActiveBrowserPropertyFrame::PreCreateWindow(CREATESTRUCT& cs) 
{

	cs.style&=~WS_CAPTION;
	cs.style|=WS_CHILD;
	return CMiniFrameWnd::PreCreateWindow(cs);
}



void CActiveBrowserPropertyFrame::OnOpenProject()
{

	m_pModelessPropSheet->SetActivePage(1);
	m_pModelessPropSheet->SetActivePage(2);
	m_pModelessPropSheet->SetActivePage(0);

	m_pModelessPropSheet->m_PageAggregate.OpenProject();
	m_pModelessPropSheet->m_PageInheritance.OpenProject();
	m_pModelessPropSheet->m_PageMeta.OpenProject();
	m_pModelessPropSheet->m_PageAggregate.SetFocus2();

}

void CActiveBrowserPropertyFrame::OnCloseProject()
{
	m_pModelessPropSheet->m_PageAggregate.CloseProject();
	m_pModelessPropSheet->m_PageInheritance.CloseProject();
	m_pModelessPropSheet->m_PageMeta.CloseProject();
	
}



void CActiveBrowserPropertyFrame::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CMiniFrameWnd::OnWindowPosChanged(lpwndpos);

	CRect clientRect;
	GetClientRect(clientRect);

	m_pModelessPropSheet->MoveWindow(clientRect,TRUE);
}

void CActiveBrowserPropertyFrame::MoveWindow(LPCRECT lpRect, BOOL bRepaint )
{
	CMiniFrameWnd::MoveWindow(lpRect, bRepaint);	

	m_pModelessPropSheet->MoveWindow(lpRect, TRUE);	
}

void CActiveBrowserPropertyFrame::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{
	CMiniFrameWnd::MoveWindow(x, y, nWidth, nHeight, bRepaint);	
	
	m_pModelessPropSheet->MoveWindow(0, 0, nWidth, nHeight, TRUE);
}


