// TestWindow.cpp : implementation file
//

#include "stdafx.h"
#include "TestWindow.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// from GME
typedef enum { GME_NAME_FONT = 0, GME_PORTNAME_FONT, GME_CONNLABEL_FONT, GME_FONT_KIND_NUM } GMEFontKind;
static int  fontSizes[GME_FONT_KIND_NUM]	= { 18, 15, 12 };

/////////////////////////////////////////////////////////////////////////////
// CTestWindow

CTestWindow::CTestWindow(const CString& titleStr, const CRect& pos, CScrollZoomView* parent):
	parentWindowPtr(parent),
	title(titleStr),
	position(pos),
	created(false)
{
	backgroundColor = ::GetSysColor(COLOR_APPWORKSPACE);
	canvasBgndColor = RGB(0xFF, 0xFF, 0xFF);
//	txtMetricFont.CreateFont(fontSizes[GME_NAME_FONT], 0, 0, 0, true, 0, 0, 0, ANSI_CHARSET,
//							 OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS,
//							 PROOF_QUALITY, FF_SWISS, "Arial");
}

CTestWindow::~CTestWindow()
{
//	txtMetricFont.DeleteObject();
}


void CTestWindow::CreateWindowSpecial(const CString& wndClassStr)
{
	CRect createPos = position;
	CPoint scrollPos = parentWindowPtr->GetDeviceScrollPosition();
	scrollPos.x *= -1;
	scrollPos.y *= -1;
	createPos.OffsetRect(scrollPos);
	if (!Create(wndClassStr, title, WS_VISIBLE | WS_CHILD, createPos, parentWindowPtr, 0)) {
		DWORD dw = GetLastError();
		// 0x00000008 ERROR_NOT_ENOUGH_MEMORY "Not enough storage is available to process this command."
		// 0x00000486 ERROR_NO_MORE_USER_HANDLES: The current process has used all of its system allowance of handles for Window Manager objects.
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL);
		OutputDebugString((LPCSTR)lpMsgBuf);
		LocalFree(lpMsgBuf);
		TRACE1("Failed to create %s\n", (const char*)title);

		return;
	}
	created = true;
}


void CTestWindow::DestroyWindowSpecial(void)
{
	DestroyWindow();
	created = false;
}


void CTestWindow::MoveWindowSpecial(int deltax, int deltay)
{
	position.OffsetRect(deltax, deltay);

	if (created) {
		CRect clientRect;
		GetClientRect(&clientRect);
//		TRACE("ClientRect: %ld, %ld, %ld, %ld\n", clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);
		ClientToScreen(&clientRect);
//		TRACE("AfterScreen: %ld, %ld, %ld, %ld\n", clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);

		CRect parentWindowRect;
		parentWindowPtr->GetWindowRect(&parentWindowRect);
		clientRect.OffsetRect(deltax - parentWindowRect.left, deltay - parentWindowRect.top);
//		TRACE("AfterOffset: %ld, %ld, %ld, %ld\n", clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);
		MoveWindow(clientRect);
	}
}


bool CTestWindow::IsIntersectRect(const CRect& rect)
{
	CRect intersectRect;
	intersectRect.IntersectRect(position, rect);
	return !intersectRect.IsRectEmpty();
}


BEGIN_MESSAGE_MAP(CTestWindow, CWnd)
	//{{AFX_MSG_MAP(CTestWindow)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTestWindow message handlers

void CTestWindow::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rcBounds;
//	dc.GetBoundsRect(rcBounds, 0);
	GetClientRect(&rcBounds);

//	TRACE("Paint %s: %ld, %ld, %ld, %ld\n", title, rcBounds.left, rcBounds.top, rcBounds.right, rcBounds.bottom);

	CBrush brush(canvasBgndColor);
	dc.FillRect(rcBounds, &brush);
	dc.DrawEdge(rcBounds, EDGE_BUMP, BF_RECT);

	CSize textSize = dc.GetTextExtent(title);
	dc.SetTextAlign(TA_CENTER);
	dc.TextOut(rcBounds.Width() / 2, 2, title);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CTestWindow::OnEraseBkgnd(CDC* pDC)
{
/*	RECT r;
	GetClientRect(&r);
	pDC->FillSolidRect(&r, backgroundColor);
	return TRUE;*/

//	return CWnd::OnEraseBkgnd(pDC);

	return FALSE;	// kill this message--no default processing
}

void CTestWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	CGMEEventLogger::LogGMEEvent("CTestWindow::OnLButtonDown\r\n");

	CWnd::OnLButtonDown(nFlags, point);
}
