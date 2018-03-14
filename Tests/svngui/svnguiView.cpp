
// svnguiView.cpp : implementation of the CsvnguiView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "svngui.h"
#endif

#include "svnguiDoc.h"
#include "svnguiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CsvnguiView

IMPLEMENT_DYNCREATE(CsvnguiView, CEditView)

BEGIN_MESSAGE_MAP(CsvnguiView, CEditView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CsvnguiView construction/destruction

CsvnguiView::CsvnguiView()
{
	// TODO: add construction code here

}

CsvnguiView::~CsvnguiView()
{
}

BOOL CsvnguiView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = CEditView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}

void CsvnguiView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CsvnguiView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CsvnguiView diagnostics

#ifdef _DEBUG
void CsvnguiView::AssertValid() const
{
	CEditView::AssertValid();
}

void CsvnguiView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CsvnguiDoc* CsvnguiView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CsvnguiDoc)));
	return (CsvnguiDoc*)m_pDocument;
}
#endif //_DEBUG


// CsvnguiView message handlers
