// GmePrintDialog.cpp : implementation file
//

#include "stdafx.h"
#include "gme.h"
#include "GmePrintDialog.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CGmePrintDialog

//IMPLEMENT_DYNAMIC(CGmePrintDialog, CPrintDialog)

#define MAX_PRNPAGE	5
CGmePrintDialog::CGmePrintDialog(CGMEView *view, CGuiMetaModel *guiMeta, BOOL bPrintSetupOnly, DWORD dwFlags, CWnd* pParentWnd) :
	CPrintDialog(bPrintSetupOnly, dwFlags, pParentWnd)
{
	//{{AFX_DATA_INIT(CGmePrintDialog)
	m_noheader = FALSE;
	m_1page = TRUE;	
	m_autorotate = TRUE;
	m_scale = 1;
	//}}AFX_DATA_INIT
	m_portrait = true;
	m_autoPortrait = true;
	m_selasp = CUR_ASP;
	m_guiMeta = guiMeta;
	m_gmeview = view;
}

void CGmePrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGmePrintDialog)
	DDX_Control(pDX, IDC_PREVIEWBTN, m_demo);
	DDX_Control(pDX, IDC_SCALETXT, m_scaletxt);
	DDX_Control(pDX, IDC_BTNSELECT, m_aspSelBtn);
	DDX_Control(pDX, IDC_BTNDESELECT, m_aspDeselBtn);
	DDX_Control(pDX, IDC_XLANDSCAPE, m_landBtn);
	DDX_Control(pDX, IDC_XPORTRAIT, m_portBtn);
	DDX_Control(pDX, IDC_CURASP1056, m_curraspBtn);
	DDX_Control(pDX, IDC_ALLASP1058, m_allaspBtn);
	DDX_Control(pDX, IDC_SELASP1057, m_selaspBtn);
	DDX_Control(pDX, IDC_CHKNOHEADER, m_noheaderBtn);
	DDX_Control(pDX, IDC_SLIDER2, m_scaleslider);
	DDX_Control(pDX, IDC_ASPECTSLIST, m_asplist);
	DDX_Check(pDX, IDC_CHKNOHEADER, m_noheader);
	DDX_Check(pDX, IDC_FITTO1PAGE, m_1page);
	DDX_Check(pDX, IDC_AUTOROTATE, m_autorotate);
	DDX_Slider(pDX, IDC_SLIDER2, m_scale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGmePrintDialog, CPrintDialog)
	//{{AFX_MSG_MAP(CGmePrintDialog)
	ON_BN_CLICKED(IDC_BTNDESELECT, OnBtndeselect)
	ON_BN_CLICKED(IDC_BTNSELECT, OnBtnselect)
	ON_BN_CLICKED(IDC_XLANDSCAPE, OnXlandscape)
	ON_BN_CLICKED(IDC_XPORTRAIT, OnXportrait)
	ON_BN_CLICKED(IDC_CHKNOHEADER, OnChknoheader)
	ON_BN_CLICKED(IDC_FITTO1PAGE, OnFitto1page)
	ON_BN_CLICKED(IDC_AUTOROTATE, OnAutorotate)
	ON_BN_CLICKED(IDC_SELASP1057, OnSelasp1057)
	ON_BN_CLICKED(IDC_ALLASP1058, OnAllasp1058)
	ON_BN_CLICKED(IDC_CURASP1056, OnCurasp1056)
	ON_BN_CLICKED(IDC_REVERSE, OnReverse)
	ON_WM_HSCROLL()
	ON_WM_DEVMODECHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CGmePrintDialog::OnInitDialog() 
{
	BOOL ret = CPrintDialog::OnInitDialog();
	m_landBtn.SetCheck(0);
	m_portBtn.SetCheck(1);
	m_curraspBtn.SetCheck(1);
	m_allaspBtn.SetCheck(0);
	m_selaspBtn.SetCheck(0);
	m_guiMeta->InitAspectList(&m_asplist);
	m_asplist.EnableWindow(FALSE);
	m_aspSelBtn.EnableWindow(FALSE);
	m_aspDeselBtn.EnableWindow(FALSE);
	m_landBtn.EnableWindow(FALSE);
	m_portBtn.EnableWindow(FALSE);
	m_scaleslider.EnableWindow(FALSE);
	m_scaletxt.EnableWindow(FALSE);
	m_scaleslider.SetRange(1, MAX_PRNPAGE);
	m_scaleslider.SetPos(1);
	m_scale = 1;
	RECT rect;
	m_demo.GetClientRect(&rect);
	m_previewrect = rect; 
	m_selAspects.clear();

	CDC* cdc = CreatePreviewBitmap();
	m_demo.SetDC(cdc);
	return ret;
}


void CGmePrintDialog::OnBtndeselect() 
{
	m_asplist.SelItemRange(FALSE, 0, m_asplist.GetCount());
}

void CGmePrintDialog::OnBtnselect() 
{
	m_asplist.SelItemRange(TRUE, 0, m_asplist.GetCount());
}

void CGmePrintDialog::OnReverse() 
{
	for (int i=0; i < m_asplist.GetCount(); i++)
		m_asplist.SetSel(i, m_asplist.GetSel(i) == 0);
}

void CGmePrintDialog::OnXlandscape() 
{
	UpdateData(TRUE);
	m_portrait = false;
	m_landBtn.SetCheck(1);
	m_portBtn.SetCheck(0);
	m_selaspBtn.SetCheck(0);
	CDC* cdc = CreatePreviewBitmap();
	if (cdc)
	{
		m_demo.SetDC(cdc);
		m_demo.Invalidate();
		m_demo.UpdateWindow();
	}
	// átrajzomni a preview-t
	UpdateData(FALSE);
}

void CGmePrintDialog::OnXportrait() 
{
	UpdateData(TRUE);
	m_portrait = true;
	m_landBtn.SetCheck(0);
	m_portBtn.SetCheck(1);
	CDC* cdc = CreatePreviewBitmap();
	if (cdc)
	{
		m_demo.SetDC(cdc);
		m_demo.Invalidate();
		m_demo.UpdateWindow();
	}
	// átrajzomni a preview-t
	UpdateData(FALSE);
}

#define x_min(x,y) ((x<y)? (x): (y))
#define x_max(x,y) ((x>y)? (x): (y))
#define THIRDINCH 84  // 1/10 mm
CDC * CGmePrintDialog::CreatePreviewBitmap()
{
	CRect bitRect = m_previewrect;
	// extent of graphics
	CRect extent, objext, annext;
	CGuiObject::GetExtent(m_gmeview->children,objext);
	CGuiAnnotator::GetExtent(m_gmeview->annotators,annext);
	extent.UnionRect(&objext, &annext);
	extent.right = (int)(extent.right*EXTENT_ERROR_CORR); // ??
	extent.bottom = (int)(extent.bottom*EXTENT_ERROR_CORR); // ??
	// papersize
	//	DEVMODE *lpdevm = GetDevMode();
	//	lpdevm->dmPaperLength // 1/10 mm
	//	lpdevm->dmPaperWidth // 1/10 mm
	//	double pWidth = lpdevm->dmPaperWidth;	// the smaller // x_min(lpdevm->dmPaperWidth, lpdevm->dmPaperLength);
	//	double pLength = lpdevm->dmPaperLength; // the longer // x_max(lpdevm->dmPaperWidth, lpdevm->dmPaperLength);
	// same sizes as in case of real printing
	HDC phdc = CreatePrinterDC();
	CDC dc;
	dc.Attach(phdc);
	double wmargin = GetDeviceCaps(phdc,PHYSICALOFFSETX);
	double hmargin = GetDeviceCaps(phdc,PHYSICALOFFSETY);
	double w = GetDeviceCaps(phdc,PHYSICALWIDTH) - 2*wmargin;
	double h = GetDeviceCaps(phdc,PHYSICALHEIGHT)- 2*hmargin;
	double pWidth = x_min(w, h);
	double pLength = x_max(w, h);

	double xdr, ydr;
	bool oriPortrait = m_portrait;
	if (m_autorotate) // rotate PAGE to fit drawing better to the page
	{   // orientation changes !! automatically - user cannot change
		if (extent.Width() > extent.Height()) // rotate it
		{
			oriPortrait = false;
			m_autoPortrait = false;
		}
		else
		{
			oriPortrait = true;
			m_autoPortrait = true;
		}
	}
	if (oriPortrait)
	{
		xdr = m_scale*pWidth/m_previewrect.Width(); //  1/10 mm per pixel
		ydr = m_scale*pLength/m_previewrect.Height(); //  1/10 mm per pixel
	}
	else // landscape
	{
		xdr = m_scale*pLength/m_previewrect.Width(); //  1/10 mm per pixel
		ydr = m_scale*pWidth/m_previewrect.Height(); //  1/10 mm per pixel
	}
	
	if (xdr >= ydr)
		bitRect.bottom = (int)((double)m_previewrect.bottom*ydr/xdr);
	else // ydr > xdr
		bitRect.right = (int)((double)m_previewrect.right*xdr/ydr);

//	m_demo.SetWindowPos(NULL, 0, 0, bitRect.Width(), bitRect.Height(), 
//					SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOREDRAW|SWP_NOZORDER);

	CDC*  peviewDC = new CDC();
	CClientDC demoDC(&m_demo);

	// TODO: Error checking
	peviewDC->CreateCompatibleDC(&demoDC);
	CBitmap * prevBmp = new CBitmap();


	// TODO: Error checking
	prevBmp->CreateCompatibleBitmap(&demoDC, bitRect.Width(), bitRect.Height());
	peviewDC->SelectObject(prevBmp);

	// set background
	DWORD dw1 = GetSysColor(COLOR_WINDOW);
	BYTE r1 = GetRValue(dw1);
	BYTE g1 = GetGValue(dw1); 
	BYTE b1 = GetBValue(dw1); 
	peviewDC->SetMapMode(MM_TEXT);
	peviewDC->FillSolidRect(&bitRect, RGB(r1,g1,b1));

	// draw papes
	DWORD pgdw = GetSysColor(COLOR_ACTIVECAPTION);
	BYTE pr1 = GetRValue(pgdw);
	BYTE pg1 = GetGValue(pgdw); 
	BYTE pb1 = GetBValue(pgdw); 
	CPoint currp = peviewDC->GetCurrentPosition();
	CPen pagePen(PS_SOLID, 1, RGB(pr1, pg1, pb1));
	CPen *oriPen = peviewDC->SelectObject(&pagePen);
	for (int p=1; p<m_scale; p++)
	{ 

		int l = (int)((double)bitRect.Height()/m_scale*p);
		peviewDC->MoveTo(bitRect.left, bitRect.top + l);
		peviewDC->LineTo(bitRect.right, bitRect.top + l);
		int b = (int)((double)bitRect.Width()/m_scale*p);
		peviewDC->MoveTo(bitRect.left + b, bitRect.top);
		peviewDC->LineTo(bitRect.left + b, bitRect.bottom);
	}
	peviewDC->MoveTo(currp);
	peviewDC->SelectObject(oriPen);

	// 1/3 inch for header, draw rectangle, make the paper shorter with 1/3 inch 
	// 1 inch = 254 1/10 mm -> 1/3 inch ~= 84 1/10 mm
	if (!m_noheader)
	{
		CBrush brush;
		DWORD dw = GetSysColor(COLOR_WINDOWTEXT);
		BYTE r = GetRValue(dw);
		BYTE g = GetGValue(dw); 
		BYTE b = GetBValue(dw); 
		brush.CreateHatchBrush(HS_BDIAGONAL, RGB(r,g,b));
		peviewDC->SetMapMode(MM_TEXT);
		for (int line=0; line<m_scale; line++)
		{
			for (int col=0; col<m_scale; col++)
			{
				CRect header;
				header.left = bitRect.left + bitRect.Width()/m_scale*col;
				header.right = header.left + bitRect.Width()/m_scale;
				header.top = bitRect.top + bitRect.Height()/m_scale*line;
				header.bottom = header.top + (int)(THIRDINCH/ydr);
				peviewDC->FillRect(&header, &brush);
			}
		}
		bitRect.top += m_scale * (int)(THIRDINCH/ydr); 
	}

	m_gmeview->SetPreview(true);
	peviewDC->SetMapMode(MM_ISOTROPIC);
	// skip the header - written in an other mapping mode
	int z = (int)((double)extent.bottom /bitRect.Height() *bitRect.top);
	peviewDC->SetWindowExt(extent.right, extent.bottom +z);
	peviewDC->SetViewportExt((int)bitRect.Width(), (int)bitRect.Height() +bitRect.top);
	peviewDC->SetWindowOrg(0,-z);

	// test
/*	CBrush brush1;
	DWORD dw = GetSysColor(COLOR_ACTIVECAPTION);
	BYTE r = GetRValue(dw);
	BYTE g = GetGValue(dw); 
	BYTE b = GetBValue(dw); 
	brush1.CreateHatchBrush(HS_CROSS, RGB(r,g,b));
	peviewDC->FillRect(&extent, &brush1);
*/
	m_gmeview->OnDraw(peviewDC); // allways shows the current Aspect in the preview
	m_gmeview->SetPreview(false);
	peviewDC->SetWindowOrg(0,0);

	return peviewDC;
}

void CGmePrintDialog::OnChknoheader() 
{
	UpdateData(TRUE);
	CDC* cdc = CreatePreviewBitmap();
	if (cdc)
	{
		m_demo.SetDC(cdc);
		m_demo.Invalidate();
		m_demo.UpdateWindow();
	}
	UpdateData(FALSE);
}


void CGmePrintDialog::OnFitto1page() 
{
	UpdateData(TRUE);
	if (m_1page)
	{
		m_scale = 1;
		m_scaleslider.EnableWindow(FALSE);
		m_scaletxt.EnableWindow(FALSE);
		m_noheaderBtn.EnableWindow(TRUE);
	}
	else
	{
		m_scaleslider.EnableWindow(TRUE);
		m_scaletxt.EnableWindow(TRUE);
		m_noheader = true;
		m_noheaderBtn.EnableWindow(FALSE);
	}
	CDC* cdc = CreatePreviewBitmap();
	if (cdc)
	{
		m_demo.SetDC(cdc);
		m_demo.Invalidate();
		m_demo.UpdateWindow();
	}
	UpdateData(FALSE);
}

void CGmePrintDialog::OnAutorotate() 
{
	UpdateData(TRUE);
	if (m_autorotate)
	{
		m_portrait = true;
		m_landBtn.SetCheck(0);
		m_portBtn.SetCheck(1);
		m_landBtn.EnableWindow(FALSE);
		m_portBtn.EnableWindow(FALSE);
	}
	else
	{
		m_landBtn.EnableWindow(TRUE);
		m_portBtn.EnableWindow(TRUE);
	}
	CDC* cdc = CreatePreviewBitmap();
	if (cdc)
	{
		m_demo.SetDC(cdc);
		m_demo.Invalidate();
		m_demo.UpdateWindow();
	}
	UpdateData(FALSE);
}


void CGmePrintDialog::OnSelasp1057() 
{
	m_selasp = SEL_ASP;	

	UpdateData(TRUE);
	m_curraspBtn.SetCheck(0);
	m_allaspBtn.SetCheck(0);
	m_selaspBtn.SetCheck(1);
	m_asplist.EnableWindow(TRUE);
	m_aspSelBtn.EnableWindow(TRUE);
	m_aspDeselBtn.EnableWindow(TRUE);
	UpdateData(FALSE);
}

void CGmePrintDialog::OnAllasp1058() 
{
	m_selasp = ALL_ASP;	

	UpdateData(TRUE);
	m_curraspBtn.SetCheck(0);
	m_allaspBtn.SetCheck(1);
	m_selaspBtn.SetCheck(0);
	m_asplist.EnableWindow(FALSE);
	m_aspSelBtn.EnableWindow(FALSE);
	m_aspDeselBtn.EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CGmePrintDialog::OnCurasp1056() 
{
	m_selasp = ALL_ASP;	

	UpdateData(TRUE);
	m_curraspBtn.SetCheck(1);
	m_allaspBtn.SetCheck(0);
	m_selaspBtn.SetCheck(0);
	m_asplist.EnableWindow(FALSE);
	m_aspSelBtn.EnableWindow(FALSE);
	m_aspDeselBtn.EnableWindow(FALSE);
	UpdateData(FALSE);
}


void CGmePrintDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CPrintDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	if (m_scaleslider.m_hWnd == pScrollBar->m_hWnd)
	{
		UpdateData(TRUE);
		CDC* cdc = CreatePreviewBitmap();
		if (cdc)
		{
			m_demo.SetDC(cdc);
			m_demo.Invalidate();
			m_demo.UpdateWindow();
		}
		UpdateData(FALSE);
	}
}


bool CGmePrintDialog::IsSelectedAspect(CString aspect)
{ // if no selected, answers all of them selected
	if (m_selasp == CUR_ASP)
		return true;
	AspectList::iterator it = m_selAspects.find(aspect);
	return (it != m_selAspects.end());	
}


void CGmePrintDialog::OnOK() 
{
	// save m_asplist
	m_selAspects.clear();
	int sel = m_asplist.GetSelCount();
	bool all = false;
	if ((m_selasp == SEL_ASP  &&  sel == 0)  ||  m_selasp == ALL_ASP)
		all = true;

	if (m_selasp != CUR_ASP)
	{ // copy 
		int count = m_asplist.GetCount();
		for (int i=0; i<count; i++)
		{
			if (all ||  m_asplist.GetSel(i)>0)
			{
				CString str;
				m_asplist.GetText(i, str);		
				m_selAspects.insert(str);
			}
		}
	}

	CPrintDialog::OnOK();
}

BOOL CGmePrintDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// update preview if the printer settings changed 
	CWnd* win = GetDlgItem(1154);
	if (win  &&  IsWindowVisible()  &&
		  wParam == MAKEWPARAM((WORD)1154, (WORD)EN_UPDATE)  &&
		  (HWND)lParam == win->m_hWnd)
	{
		UpdateData(TRUE);
		CDC* cdc = CreatePreviewBitmap();
		if (cdc)
		{
			m_demo.SetDC(cdc);
			m_demo.Invalidate();
			m_demo.UpdateWindow();
		}
		UpdateData(FALSE);
	}
	return CPrintDialog::OnCommand(wParam, lParam);
}
