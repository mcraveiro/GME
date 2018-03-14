// GMEViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GMEViewDlg.h"
#include "GMEViewCtrl.h"
#include "GMEViewStd.h"
#include "..\GME\GMEOLEData.h"
#include "Gme_i.c"
#include "memdc.h"

#include <stdlib.h>
#include <time.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


typedef enum { GME_NAME_FONT = 0, GME_PORTNAME_FONT, GME_CONNLABEL_FONT, GME_FONT_KIND_NUM } GMEFontKind;
static int  fontSizes[GME_FONT_KIND_NUM]	= { 18, 15, 12 };
#define MODEL_BOX_BORDER						5

#define WINDOWTEST
#define TESTWINDOWCOUNT		2000
#define TESTWINDOWWIDTH		100
#define TESTWINDOWHEIGHT	25
#define TESTMODELWIDTH		4000
#define TESTMODELHEIGHT		8000
#define TESTWINDOWCOLUMN	40
#define TESTWINDOWROW		TESTWINDOWCOUNT / TESTWINDOWCOLUMN
#define TESTWINDOWSPACING	20


/////////////////////////////////////////////////////////////////////////////
// CGMEViewDlg dialog

bool		CGMEViewDlg::offScreenCreated = false;
CDC*		CGMEViewDlg::offScreen;
CBitmap*	CGMEViewDlg::ofsbmp;
int			CGMEViewDlg::instanceCount = 0;
CString		CGMEViewDlg::strTestWndClass;

IMPLEMENT_DYNCREATE(CGMEViewDlg, CScrollZoomView)

CGMEViewDlg::CGMEViewDlg():
		m_DropTarget(this),
	mgaProject(NULL),
	mgaMetaModel(NULL),
	currentAspect(1),
	defZoomLev("100"),
	m_zoomVal(ZOOM_NO),
	inWindowDragging(false)
{
	instanceCount++;

	//{{AFX_DATA_INIT(CGMEViewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	canvasBgndColor = RGB(0xFF, 0xFF, 0xFF);
	txtMetricFont.CreateFont(fontSizes[GME_NAME_FONT], 0, 0, 0, true, 0, 0, 0, ANSI_CHARSET,
							 OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS,
							 PROOF_QUALITY, FF_SWISS, "Arial");
	modelExtent.SetRectEmpty();
	modelExtentCalculated = false;
	SetScroll();
	srand((unsigned) time(NULL));
}

CGMEViewDlg::~CGMEViewDlg()
{
	DestroyTestWindows();

	// a good idea to release ptrs
/*	baseType.Release();
	parent.Release();
	currentModel.Release();*/

	if (--instanceCount <= 0) {
		delete offScreen;
		delete ofsbmp;
		offScreenCreated = false;
/*		if (CMainFrame::theInstance != NULL) {
			CMainFrame::theInstance->SetPartBrowserMetaModel(NULL);
			CMainFrame::theInstance->SetPartBrowserBg(::GetSysColor(COLOR_APPWORKSPACE));
		}*/
	}

	txtMetricFont.DeleteObject();
}

void CGMEViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CScrollZoomView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGMEViewDlg)
//	DDX_Control(pDX, IDC_ASPECT_TAB, tab);
	//}}AFX_DATA_MAP
}

BOOL CGMEViewDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollZoomView::PreCreateWindow(cs);
}

void CGMEViewDlg::SetCurrentProject(CComPtr<IMgaProject> project)
{
	if (mgaProject != NULL)
		mgaProject.Release();
	mgaProject = project;
	modelExtentCalculated = false;
}

void CGMEViewDlg::SetMetaModel(CComPtr<IMgaMetaModel> meta)
{
	if (mgaMetaModel != NULL)
		mgaMetaModel.Release();
	mgaMetaModel = meta;
	modelExtentCalculated = false;

//	Init propBar fields?
}

void CGMEViewDlg::SetModel(CComPtr<IMgaModel> model)
{
	if (mgaModel != NULL)
		mgaModel.Release();
	mgaModel = model;
#if defined (WINDOWTEST)
	if (strTestWndClass.IsEmpty()) {
		try {
			strTestWndClass = AfxRegisterWndClass(
				CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW,
				::LoadCursor(NULL, IDC_ARROW),
				(HBRUSH) ::GetStockObject(WHITE_BRUSH),
				::LoadIcon(NULL, IDI_APPLICATION));
		}
		catch (CResourceException* pEx) {
			TRACE0("Couldn't register class! (Already registered?)");
			pEx->Delete();
		}
	}

	DestroyTestWindows();
	if (mgaModel != NULL)
		CreateTestWindows();

//	modelExtent.SetRect(0, 0, TESTMODELWIDTH, TESTMODELHEIGHT);
	modelExtent.SetRect(0, 0, TESTWINDOWCOLUMN * (TESTWINDOWWIDTH + TESTWINDOWSPACING) + TESTWINDOWSPACING,
						TESTWINDOWROW * (TESTWINDOWHEIGHT + TESTWINDOWSPACING) + TESTWINDOWSPACING);
	modelExtentCalculated = true;
	SetScroll();
#else
	modelExtentCalculated = false;
#endif
}

void CGMEViewDlg::ChangeAspect(int aspect)
{
	currentAspect = aspect;
	Invalidate();
}

void CGMEViewDlg::Invalidate(void)
{
	// TODO
}

void CGMEViewDlg::SetScroll(void)
{
	CSize s((int) (modelExtent.right * EXTENT_ERROR_CORR), (int) (modelExtent.bottom * EXTENT_ERROR_CORR));

	if (m_zoomVal == ZOOM_NO)
		SetScrollSizes(MM_TEXT, s, m_zoomVal);
	else
		SetScrollSizes(MM_ISOTROPIC, s, m_zoomVal);
}

void CGMEViewDlg::CreateOffScreen(CDC* dc)
{
	if (offScreenCreated)
		return;
	offScreenCreated = true;
	offScreen = new CDC;
	offScreen->CreateCompatibleDC(dc);
	int offScreenWidth = dc->GetDeviceCaps(HORZRES);
	int offScreenHeight = dc->GetDeviceCaps(VERTRES);
	ofsbmp = new CBitmap;
	ofsbmp->CreateCompatibleBitmap(dc,offScreenWidth,offScreenHeight);
	// HACK: what about palettes?
	offScreen->SelectObject(ofsbmp);
}

int CGMEViewDlg::RangedRand(int range_min, int range_max)
{
	int u = (int) ((double) rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min);
	return u;
}

void CGMEViewDlg::CreateTestWindows(void)
{
	for (long i = 0; i < TESTWINDOWCOUNT; i++) {
		char title[256];
		sprintf(title, "%ld. wnd", i);

		int left = RangedRand(0, TESTMODELWIDTH - TESTWINDOWWIDTH);
		int top = RangedRand(0, TESTMODELHEIGHT - TESTWINDOWHEIGHT);

//		int left = (i % TESTWINDOWCOLUMN) * (TESTWINDOWWIDTH + TESTWINDOWSPACING) + TESTWINDOWSPACING;
//		int top = (i / TESTWINDOWCOLUMN) * (TESTWINDOWHEIGHT + TESTWINDOWSPACING) + TESTWINDOWSPACING;
//		TRACE3("%ld. wnd: %ld, %ld\n", i, left, top);
		CRect rectClient;
		rectClient.SetRect(left, top, left + TESTWINDOWWIDTH, top + TESTWINDOWHEIGHT);

		CTestWindow* testWindow = new CTestWindow(title, rectClient, this);
//		testWindow->CreateWindowSpecial(strTestWndClass);
		testWindows.push_back(testWindow);
	}
}

void CGMEViewDlg::DestroyTestWindows(void)
{
	for (std::vector<CTestWindow*>::iterator ii = testWindows.begin(); ii != testWindows.end(); ++ii) {
		(*ii)->DestroyWindowSpecial();
		delete (*ii);
	}
	testWindows.clear();
}

std::vector<CTestWindow*> CGMEViewDlg::GetWindowsInRect(const CRect& rect)
{
	std::vector<CTestWindow*> wndsInRect;
	for (std::vector<CTestWindow*>::iterator ii = testWindows.begin(); ii != testWindows.end(); ++ii) {
		if ((*ii)->IsIntersectRect(rect))
			wndsInRect.push_back(*ii);
	}
	return wndsInRect;
}


BEGIN_MESSAGE_MAP(CGMEViewDlg, CScrollZoomView)
	//{{AFX_MSG_MAP(CGMEViewDlg)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_ASPECT, OnSelchangeAspectTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEViewDlg message handlers

int CGMEViewDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// TODO: should contain GMEView::OnInitialUpdate() ???
	if (CScrollZoomView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_DropTarget.Register(this);

	return 0;
}

void CGMEViewDlg::OnClose()
{
	CScrollZoomView::OnClose();

	// CMDIChildWnd::OnClose: when the last ChildWnd is closed
	// the document is considered closed and the title changes to Paradigm
	// that's why we call this:
//	theApp.UpdateMainTitle();
}

void CGMEViewDlg::OnSize(UINT nType, int cx, int cy)
{
	TRACE2("Size Message: %ld, %ld\n", cx, cy);

	CScrollZoomView::OnSize(nType, cx, cy);
}

void CGMEViewDlg::OnMove(int x, int y) 
{
	TRACE2("Move Message: %ld, %ld\n", x, y);

	CScrollZoomView::OnMove(x, y);
}

void CGMEViewDlg::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CScrollZoomView::OnWindowPosChanged(lpwndpos);
}

HRESULT GetPosition(CComPtr<IMgaFCO> fco, CString regPath, long* x, long* y)
{
	COMTRY {
		CComBSTR bb;
		COMTHROW(fco->get_RegistryValue(CComBSTR(regPath), &bb));
		long ld, *lx = x?x:&ld, *ly = y?y:&ld;
		if (!bb) {
			*lx = -1;
			*ly = -1;
		} else {
			if (swscanf(bb, OLESTR("%ld,%ld"), lx, ly) != 2)
				COMTHROW(E_MGA_BAD_POSITIONVALUE);
		}
	} COMCATCH(;)
}

void CGMEViewDlg::OnDraw(CDC* dc)
{
	CRect rcBounds;
	dc->GetBoundsRect(rcBounds, 0);
	TRACE("CGMEViewDlg::OnDraw Rect Bounds %ld %ld %ld %ld\n", rcBounds.left, rcBounds.top, rcBounds.right, rcBounds.bottom);
	CMemDC pDC(dc);
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CBrush brush(canvasBgndColor);
		pDC->FillRect(rcBounds, &brush);

#if defined (WINDOWTEST)
		CRect clientRect;
		GetClientRect(&clientRect);
//		TRACE("CGMEViewDlg::OnDraw ClientRect Bounds %ld %ld %ld %ld\n", clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);
		CPoint scrollPos = GetDeviceScrollPosition();
		clientRect.OffsetRect(scrollPos);
//		TRACE("CGMEViewDlg::OnDraw ClientArea Bounds %ld %ld %ld %ld\n", clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);

		std::vector<CTestWindow*> newArea = GetWindowsInRect(clientRect);
		for (std::vector<CTestWindow*>::iterator ii = displayedWindows.begin(); ii != displayedWindows.end(); ++ii) {
			bool found = false;
			for (std::vector<CTestWindow*>::iterator jj = newArea.begin(); jj != newArea.end(); ++jj) {
				if (*ii == *jj) {
					found = true;
					break;
				}
			}
			if (!found)
				(*ii)->DestroyWindowSpecial();
		}
//		TRACE0("Windows in area:\n");
		for (std::vector<CTestWindow*>::iterator kk = newArea.begin(); kk != newArea.end(); ++kk) {
//			TRACE1("%s\n", (*kk)->GetTitle());
			bool found = false;
			for (std::vector<CTestWindow*>::iterator ll = displayedWindows.begin(); ll != displayedWindows.end(); ++ll) {
				if (*kk == *ll) {
					found = true;
					break;
				}
			}
			if (!found)
				(*kk)->CreateWindowSpecial(strTestWndClass);
		}
		displayedWindows = newArea;
#else
		if (mgaModel) {
//			CGMEEventLogger::LogGMEEvent("CGMEDoc::tellHistorian(ptr,asp)\r\n");

			try	{
				long status;
				COMTHROW(mgaProject->get_ProjectStatus(&status));
				bool inTrans = (status & 0x08L) != 0;
				CComPtr<IMgaTerritory> terr;
				if (!inTrans) {
					COMTHROW(mgaProject->CreateTerritory(NULL, &terr));
					COMTHROW(mgaProject->BeginTransaction(terr, TRANSACTION_READ_ONLY));
				} else {
					COMTHROW(mgaProject->get_ActiveTerritory(&terr));
				}

				CComPtr<IMgaFCO> modelFco;
				COMTHROW(terr->OpenFCO(mgaModel, &modelFco));
				CComQIPtr<IMgaModel> model = modelFco;

				status = OBJECT_ZOMBIE;
				COMTHROW(model->get_Status(&status));
				if (status == OBJECT_EXISTS) {
					CComBSTR nameBStr;
					COMTHROW(model->get_Name(&nameBStr));
					CString nameCString;
					CopyTo(nameBStr, nameCString);
					TRACE0("Model (");
					TRACE0(nameCString);
					TRACE0("), aspect: ");

					// Get aspect name
					bool first = true;
					regPath = "PartRegs/";
					strAspect = "";
					if (mgaMetaModel) {
						CComPtr<IMgaMetaAspects> mmAspects;
						COMTHROW(mgaMetaModel->get_Aspects(&mmAspects));
						CComPtr<IMgaMetaAspect> mmAspect;
						COMTHROW(mmAspects->get_Item(currentAspect, &mmAspect));
						CComBSTR bstr;
						COMTHROW(mmAspect->get_DisplayedName(&bstr));
						CopyTo(bstr, strAspect);
					}
					regPath += strAspect;
					regPath += "/Position";
					TRACE0(strAspect);
					TRACE0("\n");

					// create a DC for text metric
					CFont* oldCFont = pDC->SelectObject(&txtMetricFont);
					HFONT oldFont = (HFONT)oldCFont->GetSafeHandle();

					// Iterate through the children FCOs
					CComPtr<IMgaFCOs> ccpChildFCOs;
					COMTHROW(model->get_ChildFCOs(&ccpChildFCOs));

					if (!modelExtentCalculated)
						modelExtent.SetRectEmpty();
					MGACOLL_ITERATE(IMgaFCO, ccpChildFCOs) {
						CComPtr<IMgaFCO> ccpChildFCO(MGACOLL_ITER);
						CComPtr<IMgaFCO> childFco;
						COMTHROW(terr->OpenFCO(ccpChildFCO, &childFco));

						CComBSTR nameBStr;
						COMTHROW(childFco->get_Name(&nameBStr));
						CString nameCString;
						CopyTo(nameBStr, nameCString);

						long x, y;
						GetPosition(childFco, regPath, &x, &y);
						char buf[64];
						sprintf(buf, " pos(%d, %d)\n", x, y);

						if (x != -1 && y != -1) {
							CSize textSize = pDC->GetTextExtent(nameCString);
							if (!modelExtentCalculated) {
								// Update extent information
								if (modelExtent.left > x)
									modelExtent.left = x;
								if (modelExtent.top > y)
									modelExtent.top = y;
								if (modelExtent.right < x + textSize.cx)
									modelExtent.right = x + textSize.cx;
								if (modelExtent.bottom < y + textSize.cy)
									modelExtent.bottom = y + textSize.cy;
							}

							CRect edge(x - MODEL_BOX_BORDER, y - MODEL_BOX_BORDER,
									   x + textSize.cx + MODEL_BOX_BORDER, y + textSize.cy + MODEL_BOX_BORDER);

							CRect intersectRect;
							intersectRect.IntersectRect(edge, rcBounds);
							if (!intersectRect.IsRectEmpty()) {
#if !defined (WINDOWTEST)
								pDC->DrawEdge(edge, EDGE_BUMP, BF_RECT);

								pDC->SetTextAlign(TA_CENTER);
								pDC->TextOut(x + textSize.cx / 2, y, nameCString);
#endif
							}
						}

						TRACE0("FCO: ");
						TRACE0(nameCString);
						TRACE0(buf);
					}
					MGACOLL_ITERATE_END;

					if (!modelExtentCalculated) {
						SetScroll();
						modelExtentCalculated = true;
					}

					CFont* oldCFont2 = CFont::FromHandle(oldFont);
					pDC->SelectObject(oldCFont2);
				}

				if (!inTrans) {
					mgaProject->CommitTransaction();
				}
			} catch(...) {
				ASSERT(0);
			}
		}
#endif
	}

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CGMEViewDlg::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;	// kill this message--no default processing
}

void CGMEViewDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Needed to flush the Object Inspector and the Browser (JIRA #GME-52)
/*	SetFocus();

	CPartBrowserPaneFrame* parent = (CPartBrowserPaneFrame*)GetParent();
	ASSERT(parent != NULL);

	point.y += parent->GetScrollPosition ();

	PartWithDecorator pdt;
	bool found = FindObject(point, pdt);

	if (found) {
		CRect rectAwake = CRect(point.x, point.y, point.x + 1, point.y + 1);
		rectAwake.InflateRect(3, 3);

		long x1 = 0; long y1 = 0;
		long x2 = 0; long y2 = 0;
		COMTHROW(pdt.decorator->GetLocation(&x1, &y1, &x2, &y2));
		CRect partRect (x1, y1, x2, y2);
		CPoint ptClickOffset(point.x - partRect.left, point.y - partRect.top);

		CRectList rects;
		rects.AddTail(&partRect);

		CRectList dummyAnnList;
		CGMEDataDescriptor desc(rects, dummyAnnList, point, ptClickOffset);
//		CGMEDataDescriptor::destructList(rects);

		CGMEDataSource dataSource(mgaProject);
		CComPtr<IMgaMetaRole> mmRole;
		COMTHROW(pdt.part->get_Role(&mmRole));
		dataSource.SetMetaRole(mmRole);
		dataSource.CacheDescriptor(&desc);
		DROPEFFECT de = dataSource.DoDragDrop(DROPEFFECT_COPY, &rectAwake);
	}*/

#if defined (WINDOWTEST)
	inWindowDragging = true;
	lastDragPoint = point;
//	TRACE2("CGMEViewDlg::OnLButtonDown, point: %ld, %ld\n", point.x, point.y);
#else
	CWnd::OnLButtonDown(nFlags, point);
#endif
}

void CGMEViewDlg::OnMouseMove(UINT nFlags, CPoint point)
{
#if defined (WINDOWTEST)
	if (inWindowDragging) {
		int deltax = point.x - lastDragPoint.x;
		int deltay = point.y - lastDragPoint.y;
//		TRACE("CGMEViewDlg::OnMouseMove, delta: %ld, %ld, point: %ld, %ld\n", deltax, deltay, point.x, point.y);
		for (std::vector<CTestWindow*>::iterator ii = testWindows.begin(); ii != testWindows.end(); ++ii) {
			(*ii)->MoveWindowSpecial(deltax, deltay);
		}
		lastDragPoint = point;
	}
#endif
}

void CGMEViewDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
#if defined (WINDOWTEST)
//	TRACE2("CGMEViewDlg::OnLButtonUp, point: %ld, %ld\n", point.x, point.y);
	inWindowDragging = false;
#endif
}

void CGMEViewDlg::OnSelchangeAspectTab(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
//	long s = tab.GetCurSel();
//	if (s >= 0) {
//		partFrame.GetPane().ChangeAspect(s);
//		SendAspectChange(s);
//	}
//	Invalidate();
	*pResult = 0;
}

void CGMEViewDlg::SendAspectChange(long index)
{
	CWnd* wnd = GetParent();
	if (wnd->IsKindOf(RUNTIME_CLASS(CGMEViewFrame))) {
		CGMEViewFrame* frame = STATIC_DOWNCAST(CGMEViewFrame, wnd);
		frame->SendAspectChange(index);
	}
}

void CGMEViewDlg::SendZoomChange(long index)
{
	CWnd* wnd = GetParent();
	if (wnd->IsKindOf(RUNTIME_CLASS(CGMEViewFrame))) {
		CGMEViewFrame* frame = STATIC_DOWNCAST(CGMEViewFrame, wnd);
		frame->SendZoomChange(index);
	}
}

void CGMEViewDlg::SendWriteStatusZoom(long zoomVal)
{
	CWnd* wnd = GetParent();
	if (wnd->IsKindOf(RUNTIME_CLASS(CGMEViewFrame))) {
		CGMEViewFrame* frame = STATIC_DOWNCAST(CGMEViewFrame, wnd);
		frame->SendWriteStatusZoom(zoomVal);
	}
}

BOOL CGMEViewDlg::DoDrop(eDragOperation doDragOp, COleDataObject *pDataObject, CPoint point)
{
//	MGATREECTRL_LOGEVENT("CAggregateTreeCtrl::DoDrop\r\n");
	CString _t = "";
	if( doDragOp == DRAGOP_REFERENCE)
		_t = " REFERENCE\r\n";
	if( doDragOp == DRAGOP_SUBTYPE)
		_t = " DERIVE\r\n";
	if( doDragOp == DRAGOP_INSTANCE)
		_t = " INSTANCE\r\n";
	if( doDragOp == DRAGOP_COPY)
		_t = " COPY\r\n";
	if( doDragOp == DRAGOP_MOVE)
		_t = " MOVE\r\n";
	if( doDragOp == DRAGOP_CLOSURE)
		_t = " CLOSURE COPY\r\n";
	if( doDragOp == DRAGOP_CLOSURE_MERGE)
		_t = " COPY MERGE\r\n";
//	MGATREECTRL_LOGEVENT( _t);

	// PETER: Checking clipboard, paste XML clipboard.
	if (!CGMEDataSource::IsGmeNativeDataAvailable(pDataObject, mgaProject))  {
		if (!CGMEDataSource::IsXMLDataAvailable(pDataObject)) {
			MessageBox("Unknown clipboard format.", "Error", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		if (doDragOp != DRAGOP_COPY && doDragOp != DRAGOP_CLOSURE && doDragOp != DRAGOP_CLOSURE_MERGE) {
			MessageBox("Only copy operation is supported on GME XML clipboard format.", "Error", MB_OK | MB_ICONERROR);
			return FALSE;
		}

		bool res = CGMEDataSource::ParseXMLData(pDataObject, mgaModel, doDragOp == DRAGOP_CLOSURE_MERGE);

		return res ? TRUE : FALSE;
	}
	// PETER: end


	CComPtr<IDataObject> ccpDataObject = pDataObject->GetIDataObject(FALSE);
	CComPtr<IMgaDataSource> ccpMgaDataSource;
	COMTHROW(ccpDataObject.QueryInterface(&ccpMgaDataSource));

	CComPtr<IUnknown> ccpData;
	COMTHROW(ccpMgaDataSource->get_Data(&ccpData));
	
	CComPtr<IUnknown> ccpFolders;
	COMTHROW(ccpMgaDataSource->get_Folders(&ccpFolders));
	
	CComPtr<IMgaResolver> ccpMgaResolver;
	COMTHROW(ccpMgaResolver.CoCreateInstance(L"Mga.MgaResolver"));

	CComQIPtr<IMgaFCOs> ccpDroppedFCOs(ccpData);
	CComQIPtr<IMgaFolders> ccpDroppedFolders(ccpFolders);

	if (!ccpDroppedFCOs && !ccpDroppedFolders) {
		BOOL bRetVal = FALSE;

		MSGTRY {
			CComQIPtr<IMgaMetaRole> metaRole(ccpData);
			if (metaRole) {
				CComPtr<IMgaFCO> child;

				try	{
					long status;
					COMTHROW(mgaProject->get_ProjectStatus(&status));
					bool inTrans = (status & 0x08L) != 0;
					CComPtr<IMgaTerritory> terr;
					if (!inTrans) {
						COMTHROW(mgaProject->CreateTerritory(NULL, &terr));
						COMTHROW(mgaProject->BeginTransaction(terr, TRANSACTION_GENERAL));
					} else {
						COMTHROW(mgaProject->get_ActiveTerritory(&terr));
					}

					CComPtr<IMgaFCO> modelFco;
					COMTHROW(terr->OpenFCO(mgaModel, &modelFco));
					CComQIPtr<IMgaModel> model = modelFco;

					status = OBJECT_ZOMBIE;
					COMTHROW(model->get_Status(&status));
					if (status == OBJECT_EXISTS) {
						switch (doDragOp) {
							case DRAGOP_MOVE:
							case DRAGOP_COPY:
							{
								COMTHROW(model->CreateChildObject(metaRole, &child));
							} break;				
						}	// switch
						if (child) {
							CComBSTR nm;
							COMTHROW(metaRole->get_DisplayedName(&nm));
							COMTHROW(child->put_Name(nm));

							char buf[64];
							sprintf(buf, "%ld, %ld", point.x, point.y);
							CComBSTR bb = buf;
							CComBSTR regP = regPath;
							COMTHROW(child->put_RegistryValue(regP, bb));
						}
					}

					if (!inTrans) {
						mgaProject->CommitTransaction();
					}
				} catch(...) {
					ASSERT(0);
				}
			}
		} MSGCATCH ("Error completing GME View drop operation", mgaProject->AbortTransaction ();)	

		return bRetVal;
	}

	return FALSE;
}
