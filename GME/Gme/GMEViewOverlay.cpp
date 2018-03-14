// GMEViewOverlay.cpp : implementation file
//

#include "stdafx.h"
#include "GMEViewOverlay.h"


const UINT_PTR ID_TIMER = 1;

// GMEViewOverlay

IMPLEMENT_DYNAMIC(GMEViewOverlay, CWnd)

GMEViewOverlay::GMEViewOverlay()
: m_timerID(0)
{
}

GMEViewOverlay::~GMEViewOverlay()
{
	if (this->m_hWnd)
	{
		DestroyWindow();
	}
}

void GMEViewOverlay::OnDraw(CDC* pDC)
{
	Gdiplus::Graphics gdip(pDC->m_hDC);
	gdip.SetPageUnit(Gdiplus::UnitPixel);

	Gdiplus::Pen pen(Gdiplus::Color::Blue);
	CRect rect;
	GetWindowRect(&rect);
	Gdiplus::Rect grect(rect.left, rect.top, rect.Width(), rect.Height());
	VERIFY(gdip.DrawRectangle(&pen, grect) == Gdiplus::Ok);
}

int GMEViewOverlay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->dwExStyle |= GMEViewOverlay::exstyle;
	lpCreateStruct->style |= GMEViewOverlay::style;
	//lpCreateStruct->style |= WS_DISABLED;
	// SystemParametersInfo(SPI_GETSELECTIONFADE, 0, &fFade, 0);
	//CreateWindowEx(exstyle, 
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

template<typename int_>
int_ RoundUpTo4(int_ val)
{
	int_ mask = 0;
	mask = (~mask) ^ 3;
	return (val + 3) & mask;
}

void GMEViewOverlay::Init1()
{
	DWORD exstyle = GMEViewOverlay::exstyle;
	DWORD style = GMEViewOverlay::style;

	VERIFY(ModifyStyleEx(0, exstyle));
	VERIFY(ModifyStyle(0, style));
	//DWORD style = GetStyle();
	//DWORD exstyle = GetExStyle();

	GdiFlush();

	CRect rcWindow;
	GetWindowRect(&rcWindow);
	SIZE size = {rcWindow.Width(), rcWindow.Height()};

	CDC screen;
	screen.Attach(::GetDC(NULL));
	m_memcdc = new CDC();
	m_memcdc->CreateCompatibleDC(&screen);
	// LPVOID bitmapdata = VirtualAlloc(0, RoundUpTo4(size.cx) * RoundUpTo4(size.cy) * 4, MEM_COMMIT, PAGE_READWRITE);
	// m_bitmap = CreateBitmap(RoundUpTo4(size.cx), RoundUpTo4(size.cy), 1, 32, bitmapdata);
	m_bitmap = CreateCompatibleBitmap(screen, size.cx, size.cy);
	ASSERT(m_bitmap);

	BITMAPINFO bmpInfo = {0};
	bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
	GetDIBits(*m_memcdc, m_bitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
	// TODO check bmpInfo.bmiHeader.biBitCount==32

	HGDIOBJ old = ::SelectObject(*m_memcdc, m_bitmap);
	/*/ debugging: draw outline around window
	{
		Gdiplus::Graphics gdip(*m_memcdc);
		gdip.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		gdip.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		{
		CRect rect;
		GetWindowRect(&rect);
		Gdiplus::Rect grect(0, 0, rect.Width()-1, rect.Height()-1);
		Gdiplus::Pen trans(Gdiplus::Color(254, 255, 0, 0), 1);
		VERIFY(gdip.DrawRectangle(&trans, grect) == Gdiplus::Ok);
		}
	}
	// */
}

void GMEViewOverlay::Init2()
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	SIZE size = {rcWindow.Width(), rcWindow.Height()};
	/* debugging: FillRectangle with alpha==255 results in alpha==0
	{
		Gdiplus::Graphics gdip(*m_memcdc);
		gdip.Clear(Gdiplus::Color(0));
		gdip.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		gdip.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
		// doesnt matter gdip.SetCompositingMode( Gdiplus::CompositingModeSourceCopy ); 
		{
		CRect rect;
		GetWindowRect(&rect);
		Gdiplus::Rect grect(0, 0, 32, rect.Height()/2);
		Gdiplus::SolidBrush trans(Gdiplus::Color(180, 255, 0, 0));
		VERIFY(gdip.FillRectangle(&trans, grect) == Gdiplus::Ok);
		}
		{
		CRect rect;
		GetWindowRect(&rect);
		Gdiplus::Rect grect(32, 0, 50, rect.Height()/2);
		Gdiplus::SolidBrush trans(Gdiplus::Color(255, 250, 0, 0));
		VERIFY(gdip.FillRectangle(&trans, grect) == Gdiplus::Ok);
		}
	} */

	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biHeight = size.cy;
	bmi.bmiHeader.biWidth = size.cx;
	bmi.bmiHeader.biPlanes=1;
	LPVOID bits = VirtualAlloc(0, RoundUpTo4(size.cx) * RoundUpTo4(size.cy) * 4, MEM_COMMIT, PAGE_READWRITE);
	//auto dibits2 = GetDIBits(*m_memcdc, m_bitmap, size.cy/2+9, size.cy/2-10, bits, &bmi, DIB_RGB_COLORS);

	int dibits = GetDIBits(*m_memcdc, m_bitmap, 0, size.cy, bits, &bmi, DIB_RGB_COLORS);
	ASSERT(dibits);
	// GDI+ sets the alpha value to 0 if the source is 255. fix it
	//  (GDI always sets alpha to 0. Fix that too)
	for (int i = 0; i < size.cy; i++)
	{
		for (int j = 0; j < size.cx; j++)
		{
			int *argb = ((int*)bits) + size.cx * i + j;
			if ((*argb & 0x00FFFFFF) && (*argb & 0xFF000000) == 0)
			{
				*argb = *argb | 0xFF000000;
			}
			// assert premultiplied
			ASSERT(((*argb & 0xFF000000) >> 24) >= ((*argb & 0x00FF0000) >> 16));
			ASSERT(((*argb & 0xFF000000) >> 24) >= ((*argb & 0x0000FF00) >> 8));
			ASSERT(((*argb & 0xFF000000) >> 24) >= ((*argb & 0x000000FF)));
		}
	}
	dibits = SetDIBits(*m_memcdc, m_bitmap, 0, size.cy, bits, &bmi, DIB_RGB_COLORS);
	VERIFY(dibits);
	VirtualFree(bits, 0, MEM_RELEASE);

	//VERIFY(::SelectObject(*m_memcdc, m_bitmap));

	ASSERT(GetSafeHwnd());
	::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
    POINT ptZero = {0, 0};
	POINT ptDest = {rcWindow.left+1, rcWindow.top+1};
	m_opacity = 0;
	BLENDFUNCTION blend = { AC_SRC_OVER, 0, 254, AC_SRC_ALPHA};
	CDC screen;
	screen.Attach(::GetDC(NULL));
	// m_memcdc RGB is premultiplied with alpha
	VERIFY(UpdateLayeredWindow(&screen, &ptDest, &size, m_memcdc, &ptZero, 0, &blend, ULW_ALPHA));

	m_timerID = SetTimer(2, 75, NULL);
}


BEGIN_MESSAGE_MAP(GMEViewOverlay, CWnd)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// GMEViewOverlay message handlers

void GMEViewOverlay::OnClose()
{
	CWnd::OnClose();
}

// Fade out, in, then out
//[254 - i * 22  for i in range(6)] + [254 - i * 22  for i in range(6, 0, -1)] + [254 - i * 19  for i in range(13)] + [0]
static BYTE opacities[] = { 254, 232, 210, 188, 166, 144, 122, 144, 166, 188, 210, 232, 254, 235, 216, 197, 178, 159, 140, 121, 102, 83, 64, 45, 26, 0 };

void GMEViewOverlay::OnTimer(UINT_PTR nIDEvent)
{
	m_opacity++;
	if (opacities[m_opacity] == 0)
	{
		DestroyWindow();
		return;
	}
	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.SourceConstantAlpha = opacities[m_opacity];
	blend.AlphaFormat = AC_SRC_ALPHA;
 
	VERIFY(UpdateLayeredWindow(NULL, NULL, NULL, NULL, NULL, 0, &blend, ULW_ALPHA));

	CWnd::OnTimer(nIDEvent);
}

BOOL GMEViewOverlay::DestroyWindow()
{
	if (m_timerID)
	{
		int ret = KillTimer(m_timerID);
		if (ret != 0)
		{
			auto err = GetLastError();
			ASSERT(false);
		}
	}
	VERIFY(DeleteObject(m_bitmap));
	VERIFY(m_memcdc->DeleteDC());

	return CWnd::DestroyWindow();
}
