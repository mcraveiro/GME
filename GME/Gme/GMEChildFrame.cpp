// GMEChildFrame.cpp: implementation of the CGMEChildFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMEDoc.h"
#include "GMEChildFrame.h"
#include "GMEApp.h"
#include "MainFrm.h"


/////////////////////////////////////////////////////////////////////////////
// CGMEChildFrame

IMPLEMENT_DYNCREATE(CGMEChildFrame, CView)

CGMEChildFrame* CGMEChildFrame::theInstance = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGMEChildFrame::CGMEChildFrame():
	guiMetaModel (NULL)
{
	//{{AFX_DATA_INIT(CGMEChildFrame)
	//}}AFX_DATA_INIT

	VERIFY(theInstance == 0);
	theInstance = this;
}


BEGIN_MESSAGE_MAP(CGMEChildFrame, CView)
	//{{AFX_MSG_MAP(CGMEChildFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CGMEChildFrame, CView)
	//{{AFX_EVENTSINK_MAP(CGMEChildFrame)
	ON_EVENT(CGMEChildFrame, IDC_CHILDFRAMECTRL1, 1 /* AspectChanged */, OnAspectChangedGmeViewCtrl, VTS_I4)
	ON_EVENT(CGMEChildFrame, IDC_CHILDFRAMECTRL1, 2 /* WriteStatusZoom */, OnWriteStatusZoomGmeViewCtrl, VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEChildFrame message handlers

int CGMEChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ChildFrame.Create(_T("ChildFrame"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 150), this, IDC_CHILDFRAMECTRL1))
		return -1;

	return 0;
}


void CGMEChildFrame::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(rc);

	m_ChildFrame.MoveWindow(rc);
}


void CGMEChildFrame::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CGMEChildFrame)	
	DDX_Control(pDX, IDC_CHILDFRAMECTRL1, m_ChildFrame);
	//}}AFX_DATA_MAP
}


void CGMEChildFrame::OnDraw(CDC* /*pDC*/)
{
}


/* 
ActiveX controls do not have their message pump, it is owned by their containers.
The container in case of GME is a kind of control bar, which is treated as a dialog.
Dialog box messages are filtered by ::IsDialogMessage, which does not allow the 
default dialog kestroke messages (ESC - close dialog, ENTER - push default button
TAB - next item in the tab order etc...) to be propagated to the controls placed on 
the dialog.

Here we avoid calling the default PreTranslateMessage which filtered by 
::IsDialogMessage, dispatch it directly to the controls.

Tihamer

*/

BOOL CGMEChildFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
			case VK_RETURN:
			case VK_ESCAPE:
			case VK_DELETE:
			// Modification by Volgyesi (undo problems)
			case VK_CONTROL:
			case 'z':
			case 'Z':
			// Modification End
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
		}
	}

	return CView::PreTranslateMessage(pMsg);
}


void CGMEChildFrame::SetProject(CComPtr<IMgaProject> mgaProject)
{
	m_ChildFrame.SetCurrentProject(mgaProject);
}


void CGMEChildFrame::SetMetaModel(CGuiMetaModel* meta)
{
	LPUNKNOWN pMeta = NULL;
	guiMetaModel = meta;
	if (meta != NULL) {
		CComQIPtr<IMgaMetaModel> iMeta;
		iMeta = meta->mgaMeta;
		if (iMeta)
			pMeta = iMeta;
	}
	m_ChildFrame.SetMetaModel(pMeta);
}


void CGMEChildFrame::SetModel(CComPtr<IMgaModel> mgaModel)
{
	m_ChildFrame.SetModel(mgaModel);
}


void CGMEChildFrame::ChangeAspect(int aspcet)
{
	m_ChildFrame.ChangeAspect(aspcet);
}


void CGMEChildFrame::CycleAspect(void)
{
	m_ChildFrame.CycleAspect();
}


void CGMEChildFrame::Invalidate(void)
{
	m_ChildFrame.Invalidate();
}


void CGMEChildFrame::OnAspectChangedGmeViewCtrl(LONG index)
{
	CMainFrame::theInstance->ChangePartBrowserAspect(index);
}


void CGMEChildFrame::OnWriteStatusZoomGmeViewCtrl(LONG zoomVal)
{
	if (CMainFrame::theInstance != NULL) {
		CMainFrame::theInstance->WriteStatusZoom(zoomVal);
	}
}
