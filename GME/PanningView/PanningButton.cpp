// PannButton.cpp : implementation file
//

#include "stdafx.h"
#include "PanningButton.h"
#include "PanningViewCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPanningButton

CPanningButton::CPanningButton()
{
	m_viewRectOnScreen	= CRect(0, 0, 0, 0);
	m_viewRectInMove	= CRect(0, 0, 0, 0);
	m_viewRectOnStored	= CRect(0, 0, 0, 0);
	m_bitmapDC			= NULL;
	m_oldBmp			= NULL;
	m_bitmapSizeStored	= CRect(0, 0, 0, 0);
	m_bitmapSizeOri		= CRect(0, 0, 0, 0);
	m_owner				= NULL;
	m_backgorund		= RGB(255, 255, 255);
	m_sizeallh			= NULL;
	m_arrowh			= NULL;
	m_inMove			= false;
	m_storedToScreenRx	= 0;
	m_storedToScreenRy	= 0;
	m_oriToStoredRx		= 0;
	m_oriToStoredRy		= 0;
}

CPanningButton::~CPanningButton()
{
	if (m_bitmapDC != NULL)
		DeleteDeviceContext(m_bitmapDC, m_oldBmp);
}


BEGIN_MESSAGE_MAP(CPanningButton, CWnd)
	//{{AFX_MSG_MAP(CPanningButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanningButton message handlers

bool CPanningButton::Create()
{
	m_sizeallh = ::LoadCursor(NULL, IDC_SIZEALL);
	m_arrowh = ::LoadCursor(NULL, IDC_ARROW);
	return true;
}

void CPanningButton::SetBitmapDC(HWND owner, HDC bdc, HBITMAP oldBmp, CRect& orisize, CRect& actsize, COLORREF& bkgrnd) // may be empty
{
	if (m_bitmapDC != NULL)
		DeleteDeviceContext(m_bitmapDC, m_oldBmp);
	m_owner = owner;
	m_bitmapDC = bdc;
	m_oldBmp = oldBmp;
	m_bitmapSizeOri = orisize;
	m_bitmapSizeStored = actsize;
	m_backgorund = bkgrnd;

	Invalidate();
	UpdateWindow();
}

void CPanningButton::SetViewRect(CRect irect)
{
	// the method can be called even if the panning window is not visible
	if (m_bitmapSizeOri.IsRectEmpty() || m_bitmapSizeStored.IsRectEmpty())
		return;
	if (!::IsWindow(GetSafeHwnd()))
		return;

	m_oriToStoredRx = (double)(m_bitmapSizeStored.Width()) / m_bitmapSizeOri.Width();
	m_oriToStoredRy = (double)(m_bitmapSizeStored.Height()) / m_bitmapSizeOri.Height();
	CRect newViewRectOnStored;
	newViewRectOnStored.left	= (int)(irect.left		* m_oriToStoredRx);
	newViewRectOnStored.right	= (int)(irect.right		* m_oriToStoredRx);
	newViewRectOnStored.top		= (int)(irect.top		* m_oriToStoredRy);
	newViewRectOnStored.bottom	= (int)(irect.bottom	* m_oriToStoredRy);
	if (m_viewRectOnStored.EqualRect(&newViewRectOnStored))
		return;
	else
		m_viewRectOnStored = newViewRectOnStored;

	CRect vRect(0, 0, 0, 0);
	CRect client;
	GetParent()->GetClientRect(&client);

	BmpToScreen(client);
	ViewRectToScreen(vRect);

	InvalidateRect(&m_viewRectOnScreen, FALSE);
	m_viewRectOnScreen = vRect;
	InvalidateRect(&vRect, FALSE);
	UpdateWindow();
}

void CPanningButton::BmpToScreen(CRect clientr)
{
	double dx = (double)(clientr.Width()) / m_bitmapSizeStored.Width();
	double dy = (double)(clientr.Height()) / m_bitmapSizeStored.Height();
	double dd = (dx <= dy) ? dx: dy;
	dd = (dd < 1)? dd: 1;

	clientr.right = min(clientr.right, (LONG)(dd * m_bitmapSizeStored.Width()));
	clientr.bottom = min(clientr.bottom, (LONG)(dd * m_bitmapSizeStored.Height()));
	m_bitmapOnScreen = clientr;
}

void CPanningButton::ViewRectToScreen(CRect &vRect)
{
	m_storedToScreenRx = (double)(m_bitmapOnScreen.Width()) / m_bitmapSizeStored.Width();
	m_storedToScreenRy = (double)(m_bitmapOnScreen.Height()) / m_bitmapSizeStored.Height();
	vRect = CRect(0, 0, 0, 0);
	vRect.left = (LONG)(m_viewRectOnStored.left * m_storedToScreenRx);
	vRect.right = (LONG)(m_viewRectOnStored.right * m_storedToScreenRx);
	vRect.top = (LONG)(m_viewRectOnStored.top * m_storedToScreenRy);
	vRect.bottom = (LONG)(m_viewRectOnStored.bottom * m_storedToScreenRy);
	vRect.right = min(vRect.right, m_bitmapOnScreen.right);
	vRect.bottom = min(vRect.bottom, m_bitmapOnScreen.bottom);
}

void CPanningButton::DeleteDeviceContext(HDC bDC, HBITMAP oldBmp)
{
	HBITMAP hBmp = (HBITMAP)::SelectObject(bDC, (HBITMAP)oldBmp);
	BOOL succ = FALSE;
	if (hBmp != NULL) {
		succ = ::DeleteObject(hBmp);
		ASSERT(succ != FALSE);
	}
	succ = ::DeleteDC(bDC);
	ASSERT(succ != FALSE);
}

void CPanningButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	dc.SetMapMode(MM_TEXT);

	// clear it
	CRect clientr;
	GetParent()->GetClientRect(&clientr);
	CBrush brush;
	COLORREF col;
	if (m_bitmapDC == NULL) {
		DWORD dw = ::GetSysColor(COLOR_3DFACE);
		BYTE r = GetRValue(dw);
		BYTE g = GetGValue(dw);
		BYTE b = GetBValue(dw);
		col = RGB(r,g,b);
	} else {
		col = m_backgorund;
	}
	brush.CreateSolidBrush(col);
	dc.FillRect(&clientr, &brush);

	// no bitmap - return
	if (m_bitmapDC == NULL)
		return;

	::SetMapMode(m_bitmapDC, MM_TEXT);

	// transform bitmap to screen
	BmpToScreen(clientr);
	BOOL ret = ::StretchBlt(dc.m_hDC, 0, 0, m_bitmapOnScreen.Width(), m_bitmapOnScreen.Height(), m_bitmapDC,
							0, 0, m_bitmapSizeStored.Width(), m_bitmapSizeStored.Height(), SRCCOPY);
	if (!ret)
		return;

	CBrush brush1;
	DWORD dw1 = GetSysColor(COLOR_WINDOWTEXT);
	BYTE r1 = GetRValue(dw1);
	BYTE g1 = GetGValue(dw1);
	BYTE b1 = GetBValue(dw1);
	brush1.CreateSolidBrush(RGB(r1, g1, b1));
	if (!m_viewRectOnStored.IsRectEmpty() && m_viewRectOnScreen != m_bitmapOnScreen &&
		!m_viewRectOnScreen.IsRectEmpty())
	{
		if (!m_inMove) {
			CRect vRect(0, 0, 0, 0);
			ViewRectToScreen(vRect);
			m_viewRectOnScreen = vRect;
			dc.FrameRect(&m_viewRectOnScreen, &brush1);
		} else {
			dc.FrameRect(&m_viewRectInMove, &brush1);
		}
	}
}

BOOL CPanningButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CPanningButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_viewRectOnScreen.PtInRect(point) || m_inMove)
		SetCursor(m_sizeallh);
	else
		SetCursor(m_arrowh);

	if (m_inMove) {
		// move panning frame only
		int dx = point.x - m_moveStartPoint.x;
		int dy = point.y - m_moveStartPoint.y;
		CRect trackRect = m_viewRectOnScreen;
		trackRect.OffsetRect(dx, dy);
		CRect shadow;
		shadow.IntersectRect(&trackRect, &m_bitmapOnScreen);
		if (shadow.Width() != trackRect.Width() && shadow.Height() != trackRect.Height())
			return;
		if (m_viewRectInMove.IsRectEmpty())
			InvalidateRect(&m_viewRectOnScreen, FALSE);
		else
			InvalidateRect(&m_viewRectInMove, FALSE);

		if (shadow.Width() != trackRect.Width()) {
			if (trackRect.left < 0)
				trackRect.OffsetRect(-trackRect.left, 0);
			else
				trackRect.OffsetRect(m_bitmapOnScreen.right - trackRect.right, 0);
		} else if (shadow.Height() != trackRect.Height()) {
			if (trackRect.top < 0)
				trackRect.OffsetRect(0, -trackRect.top);
			else
				trackRect.OffsetRect(0, m_bitmapOnScreen.bottom - trackRect.bottom);
		}
		m_viewRectInMove = trackRect;

		InvalidateRect(&m_viewRectInMove, FALSE);
		UpdateWindow();
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CPanningButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_viewRectOnScreen.PtInRect(point)) {
		SetCursor(m_sizeallh);
		SetCapture();
		m_inMove = true;
		m_moveStartPoint = point;
		m_moveStartRect = m_viewRectOnScreen;
	} else {
		SetCursor(m_arrowh);
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CPanningButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_inMove) {
		ReleaseCapture();
		m_inMove = false;
//		CWnd* mainw = AfxGetMainWnd();
		if (m_owner) { // mainw)
			double dx = point.x - m_moveStartPoint.x;
			double dy = point.y - m_moveStartPoint.y;
			// dx, dy in pann win screen coordinates
			// calculate real screen coordinates
			dx /= m_storedToScreenRx;
			dy /= m_storedToScreenRy;
			dx /= m_oriToStoredRx;
			dy /= m_oriToStoredRy;
			::PostMessage(m_owner, WM_PANN_SCROLL, (WPARAM)(int)dx, (LPARAM)(int)dy);
			Invalidate();
		}
		m_viewRectInMove = CRect(0, 0, 0, 0);
		m_moveStartPoint = CPoint(0, 0);
		m_moveStartRect = CRect(0, 0, 0, 0);
	}

	CWnd::OnLButtonUp(nFlags, point);
}
