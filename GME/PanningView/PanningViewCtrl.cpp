// PanningViewCtrl.cpp : Implementation of the CPanningViewCtrl ActiveX Control class.

#include "stdafx.h"
#include "PanningView.h"
#include "PanningViewCtrl.h"
#include "PanningViewPropPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CPanningViewCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CPanningViewCtrl, COleControl)
	//{{AFX_MSG_MAP(CPanningViewCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CPanningViewCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CPanningViewCtrl)
	DISP_FUNCTION_ID(CPanningViewCtrl, "SetBitmapDC", dispidSetBitmapDC, SetBitmapDC, VT_EMPTY, VTS_UI8 VTS_UI8 VTS_UI8 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_COLOR)
	DISP_FUNCTION_ID(CPanningViewCtrl, "SetViewRect", dispidSetViewRect, SetViewRect, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CPanningViewCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CPanningViewCtrl, COleControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CPanningViewCtrl, 1)
	PROPPAGEID(CPanningViewPropPage::guid)
END_PROPPAGEIDS(CPanningViewCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPanningViewCtrl, "PANNINGVIEW.PanningViewCtrl.1",
	0xb0c28baa, 0x7e44, 0x404c, 0xbe, 0xad, 0x83, 0x73, 0x53, 0x56, 0xf1, 0xd2)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CPanningViewCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DPanningView =
		{ 0x3055658C, 0xDEFE, 0x4B68, { 0x9B, 0xC3, 0x21, 0xB1, 0x36, 0xBD, 0xBB, 0xDB } };
const IID BASED_CODE IID_DPanningViewEvents =
		{ 0xBD4F784C, 0xA79F, 0x48F6, { 0x81, 0x8D, 0xB, 0x82, 0x9A, 0x6, 0x1C, 0xC0 } };



// Control type information

static const DWORD BASED_CODE _dwPanningViewOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CPanningViewCtrl, IDS_PANNINGVIEW, _dwPanningViewOleMisc)



// CPanningViewCtrl::CPanningViewCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CPanningViewCtrl

BOOL CPanningViewCtrl::CPanningViewCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_PANNINGVIEW,
			IDB_PANNINGVIEW,
			afxRegApartmentThreading,
			_dwPanningViewOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CPanningViewCtrl::CPanningViewCtrl - Constructor

CPanningViewCtrl::CPanningViewCtrl()
{
	InitializeIIDs(&IID_DPanningView, &IID_DPanningViewEvents);
	// TODO: Initialize your control's instance data here.
}



// CPanningViewCtrl::~CPanningViewCtrl - Destructor

CPanningViewCtrl::~CPanningViewCtrl()
{
	// TODO: Cleanup your control's instance data here.
}



// CPanningViewCtrl::OnDraw - Drawing function

void CPanningViewCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	if (m_panningViewDlg.GetSafeHwnd()) {
		m_panningViewDlg.MoveWindow(rcBounds, TRUE);
	} else {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBrush brush(RGB(255, 255, 255));
		pdc->FillRect(rcBounds, &brush);

		CRect edge(rcBounds);
		pdc->DrawEdge(&edge, EDGE_BUMP, BF_RECT);

		CString label("PanningView OCX");

		BITMAP bm;
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_PANNINGVIEW);
		bitmap.GetBitmap(&bm);
		CSize size(bm.bmWidth, bm.bmHeight);
		pdc->DPtoLP(&size);
		CPoint org(0,0);
		pdc->DPtoLP(&org);
		CPoint pos(rcBounds.Width() / 2, rcBounds.Height() / 2 - (pdc->GetTextExtent(label).cy));
		CDC dcMem;
		dcMem.CreateCompatibleDC(pdc);
		CBitmap* oldbitmap = dcMem.SelectObject(&bitmap);
		dcMem.SetMapMode(pdc->GetMapMode());
		pdc->BitBlt(pos.x, pos.y, size.cx, size.cy, &dcMem, org.x, org.y, SRCCOPY);
		dcMem.SelectObject(oldbitmap);

		pdc->SetTextAlign(TA_CENTER);
		pdc->TextOut(rcBounds.Width() / 2, rcBounds.Height() / 2, CString("PanningView ActiveX Control"));
	}
}



// CPanningViewCtrl::DoPropExchange - Persistence support

void CPanningViewCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CPanningViewCtrl::OnResetState - Reset control to default state

void CPanningViewCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CPanningViewCtrl::AboutBox - Display an "About" box to the user

void CPanningViewCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_PANNINGVIEW);
	dlgAbout.DoModal();
}



// CPanningViewCtrl message handlers

int CPanningViewCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_panningViewDlg.Create(IDD_PANNINGVIEWDLG, this) == FALSE)
		return -1;

	return 0;
}

BOOL CPanningViewCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTROLPARENT;
	return COleControl::PreCreateWindow(cs);
}

void CPanningViewCtrl::SetBitmapDC(ULONGLONG ownerWnd, ULONGLONG bDC, ULONGLONG oldBmp,
								   LONG orix, LONG oriy, LONG oriw, LONG orih,
								   LONG rx, LONG ry, LONG rw, LONG rh,
								   OLE_COLOR bkgrnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HWND owner = (HWND) ownerWnd;
	HDC bdc = (HDC) bDC;
	HBITMAP oBmp = (HBITMAP)oldBmp;
	CRect ori(orix, oriy, orix + oriw, oriy + orih);
	CRect rect(rx, ry, rx + rw, ry + rh);
	COLORREF bgColor = TranslateColor(bkgrnd);

	m_panningViewDlg.SetBitmapDC(owner, bdc, oBmp, ori, rect, bgColor);
}

void CPanningViewCtrl::SetViewRect(LONG vrx, LONG vry, LONG vrw, LONG vrh)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRect vrect(vrx, vry, vrx + vrw, vry + vrh);

	m_panningViewDlg.SetViewRect(vrect);
}
