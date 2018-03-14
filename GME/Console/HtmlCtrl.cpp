// HtmlCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "console.h"
#include "HtmlCtrl.h"
#include "ConsoleCtl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl

IMPLEMENT_DYNAMIC(CHtmlCtrl, CHtmlView)


CHtmlCtrl::CHtmlCtrl()
{
	//{{AFX_DATA_INIT(CHtmlCtrl)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHtmlCtrl::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHtmlCtrl)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHtmlCtrl, CHtmlView)
	//{{AFX_MSG_MAP(CHtmlCtrl)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl diagnostics

#ifdef _DEBUG
//
// MFC manages/stores states in statically linked (small) stubs
// Also, it updates the current module state for window message dispatch
// In this case, message is sent to the hosted IE window (so the module state won't be changed to our DLL), 
// which calls CHtmlView through COM. For some reason CHtmlView does not update module state when called trhough COM, 
// and eventually it calls MFC functions (only this one in our case) which will not find the (correct) module state
//
void CHtmlCtrl::AssertValid() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CHtmlView::AssertValid();
}

void CHtmlCtrl::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl message handlers

//////////////////
// Create control in same position as an existing static control with
// the same ID (could be any kind of control, really)
//
BOOL CHtmlCtrl::CreateFromStatic(UINT nID, CWnd* pParent)
{
	CStatic wndStatic;
	if (!wndStatic.SubclassDlgItem(nID, pParent))
		return FALSE;

	// Get static control rect, convert to parent's client coords.
	CRect rc;
	wndStatic.GetWindowRect(&rc);
	pParent->ScreenToClient(&rc);
	wndStatic.DestroyWindow();

	// create HTML control (CHtmlView)
	return Create(NULL,                  // class name
		NULL,                             // title
		(WS_CHILD | WS_VISIBLE ),         // style
		rc,                               // rectangle
		pParent,                          // parent
		nID,                              // control ID
		NULL);                            // frame/doc context not used
}

////////////////
// Override to avoid CView stuff that assumes a frame.
//
void CHtmlCtrl::OnDestroy()
{
	// This is probably unecessary since ~CHtmlView does it, but
	// safer to mimic CHtmlView::OnDestroy.
	/*if (m_pBrowserApp) {
		m_pBrowserApp->Release();
		m_pBrowserApp = NULL;
	}*/ //commented for vc7, Release its not a public member of IWebBrowser2
	CWnd::OnDestroy(); // bypass CView doc/frame stuff
}

////////////////
// Override to avoid CView stuff that assumes a frame.
//
int CHtmlCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT msg)
{
	// bypass CView doc/frame stuff
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, msg);
}

//////////////////
// Override navigation handler to pass to "app:" links to virtual handler.
// Cancels the navigation in the browser, since app: is a pseudo-protocol.
//
void CHtmlCtrl::OnBeforeNavigate2( LPCTSTR lpszURL,
	DWORD nFlags,
	LPCTSTR lpszTargetFrameName,
	CByteArray& baPostedData,
	LPCTSTR lpszHeaders,
	BOOL* pbCancel )
{
	const TCHAR MGA_PROTOCOL[] = _T("mga:");
	int len = _tcslen(MGA_PROTOCOL);
	if (_tcsnicmp(lpszURL, MGA_PROTOCOL, len)==0) {
		OnMgaURL(lpszURL + len);
		*pbCancel = TRUE;
	}
}

void CHtmlCtrl::OnMgaURL(LPCTSTR lpszWhere)
{
	CWnd *wnd = GetParent();
	if (wnd->IsKindOf(RUNTIME_CLASS(CConsoleCtrl))) {
		CConsoleCtrl *ctrl = STATIC_DOWNCAST(CConsoleCtrl, wnd);
		ctrl->FireClickMGAID(lpszWhere);
	}
}

void CHtmlCtrl::OnDocumentComplete(LPCTSTR lpszURL)
{
	CWnd *wnd = GetParent();
	if (wnd->IsKindOf(RUNTIME_CLASS(CConsoleCtrl))) {
		CConsoleCtrl *ctrl = STATIC_DOWNCAST(CConsoleCtrl, wnd);
		ctrl->AddGMEToScript();
	}
}
