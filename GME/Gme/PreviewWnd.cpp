// PreviewWnd.cpp : implementation file
//

#include "stdafx.h"
#include "gme.h"
#include "PreviewWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewBtn

CPreviewBtn::CPreviewBtn()
{
	m_bitmapDC = NULL;
}

CPreviewBtn::~CPreviewBtn()
{
	if (m_bitmapDC) 
	{
		CBitmap *bmp = m_bitmapDC->GetCurrentBitmap();
		delete m_bitmapDC; 
		if (bmp)
			delete bmp;
	}
}


BEGIN_MESSAGE_MAP(CPreviewBtn, CButton)
	//{{AFX_MSG_MAP(CPreviewBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPreviewBtn message handlers
void CPreviewBtn::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->itemAction == ODA_DRAWENTIRE)
	{
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC); // device context for painting
		RECT rect;
		GetClientRect(&rect);
		CRect crect(rect);
		CRect vrect = crect;

		dc.SetMapMode(MM_TEXT);
		dc.DPtoLP(&vrect);
		if (!m_bitmapDC)
			return;

		m_bitmapDC->SetMapMode(MM_TEXT);
		// TODO: Error checking
		dc.BitBlt(0, 0, vrect.Width(), vrect.Height(), m_bitmapDC, 0, 0, SRCCOPY);
		dc.Detach(); 
	}
}


