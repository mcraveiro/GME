// GMESearch.cpp : implementation file
//

#include "stdafx.h"
#include "GMEApp.h"
#include "GMEstd.h"
#include "mainfrm.h"
#include "GMEDoc.h"
#include "GMEView.h"
#include "GMESearch.h"
#include "guimeta.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGMESearch* CGMESearch::theInstance = NULL;

/////////////////////////////////////////////////////////////////////////////
// CGMESearch dialog


CGMESearch::CGMESearch()
: CDockablePane()
{
	//{{AFX_DATA_INIT(CGMESearch)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	VERIFY(theInstance == NULL);
	theInstance = this;
}


void CGMESearch::DoDataExchange(CDataExchange* pDX)
{
	CDockablePane::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGMESearch)
	DDX_Control(pDX, IDC_SEARCHCTRL, m_search);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGMESearch, CDockablePane)
	//{{AFX_MSG_MAP(CGMESearch)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMESearch message handlers

BEGIN_EVENTSINK_MAP(CGMESearch, CDockablePane)
    //{{AFX_EVENTSINK_MAP(CGMESearch)
	ON_EVENT(CGMESearch, IDC_SEARCHCTRL, 1 /* ClickMgaObject */, OnClickMgaObjectSearchctrl, VTS_UNKNOWN)
	ON_EVENT(CGMESearch, IDC_SEARCHCTRL, 2 /* DblClickMgaObject */, OnDblClickMgaObjectSearchctrl, VTS_UNKNOWN)
	ON_EVENT(CGMESearch, IDC_SEARCHCTRL, 3 /* WantToBeClosed */, OnWantToBeClosedSearchctrl, VTS_NONE)
	ON_EVENT(CGMESearch, IDC_SEARCHCTRL, 4 /* LocateMgaObject */, OnLocateMgaObjectSearchctrl, VTS_BSTR)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CGMESearch::OnClickMgaObjectSearchctrl(LPUNKNOWN mgaObject) 
{
	SelectObject(mgaObject);
	
}

void CGMESearch::OnDblClickMgaObjectSearchctrl(LPUNKNOWN mgaObject) 
{
	ShowObject(mgaObject);
	
}

void CGMESearch::OnWantToBeClosedSearchctrl() 
{
	// was not implemented yet
	CMainFrame::theInstance->HideFindDlg();
	// by zolmol
}

void CGMESearch::OnLocateMgaObjectSearchctrl(LPCTSTR mgaObjectId) 
{
	LocateObject( mgaObjectId);
}

BOOL CGMESearch::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		switch(pMsg->wParam)
		{
			//case VK_RETURN:
			//case VK_TAB:
			case VK_ESCAPE:
			case VK_DELETE:
			case VK_CONTROL:
			case 'z':
			case 'Z':
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
		}
	}
	
	return CDockablePane::PreTranslateMessage(pMsg);
}

int CGMESearch::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

   /* //if no project is open don't draw
    if(m_search.GetMgaProject()==NULL)
        return 0;*/

    if(!m_search.Create(_T("Search"),WS_CHILD | WS_VISIBLE,CRect(0,0,230,300),this,IDC_SEARCHCTRL))
        return -1;
	
//	if(!m_search.Create("Search",WS_CHILD | WS_VISIBLE,CRect(0,0,230,300),this,IDC_SEARCHCTRL))
//		return -1;
	
	return 0;
}

// OnSizing implemented as well (see below)
void CGMESearch::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (m_search.GetSafeHwnd()) {
		CRect rc;
		GetClientRect(rc);	
		m_search.MoveWindow(rc);
	}	
}

/////////////////////////////////////////////////////////////////////////////
// CGMESearch public methods

void CGMESearch::SetProject(CComPtr<IMgaProject>& mgaProject)
{
  	m_search.SetMgaProject(mgaProject);    
}

void CGMESearch::SetSelMgaObjects( CComPtr<IMgaObjects> p_mgaObjects)
{
	m_search.SelMgaObjects( p_mgaObjects);
}

void CGMESearch::CloseProject()
{
	m_search.SetMgaProject(NULL);
}

void CGMESearch::ShowObject(LPUNKNOWN mgaObject)
{
	if (CGMEDoc::theInstance && mgaObject) {
		CGMEDoc::theInstance->ShowObject(mgaObject);
	}
}

void CGMESearch::LocateObject(LPCTSTR mgaObjectId)
{
	if( CGMEDoc::theInstance
	 && CGMEBrowser::theInstance
	 && mgaObjectId)
	{
		CGMEBrowser::theInstance->FocusItem( CComBSTR( mgaObjectId));
        //added :kiran
        CGMEDoc::theInstance->ShowObject(mgaObjectId);
        
	}
}

void CGMESearch::SelectObject(LPUNKNOWN mgaObject)
{
	CComQIPtr<IMgaObject> ccpmgaObject = mgaObject;
	if (ccpmgaObject) {
		CComPtr<IMgaObjects> ccpSelectedObjects;
		COMTHROW(ccpSelectedObjects.CoCreateInstance(OLESTR("Mga.MgaObjects")));
		COMTHROW(ccpSelectedObjects->Append(ccpmgaObject));
		CGMEObjectInspector::theInstance->SetObjects(ccpSelectedObjects);
	} else {
		CComQIPtr<IMgaObjects> ccpSelectedObjects = mgaObject;
		if (ccpSelectedObjects) {
			CGMEObjectInspector::theInstance->SetObjects(ccpSelectedObjects);
		}
	}
}

// OnSize implemented as well (see above)
void CGMESearch::OnSizing(UINT fwSide, LPRECT pRect)
{
	static const int desiredX = 370, desiredY = 400;
	// let's prevent window shrink by imposing some limits
	if( pRect->bottom - pRect->top < desiredY) pRect->bottom = pRect->top + desiredY;
	if( pRect->right - pRect->left < desiredX) pRect->right = pRect->left + desiredX;

	CDockablePane::OnSizing(fwSide, pRect);
}
