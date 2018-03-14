// GMEBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "GMEApp.h"
#include "GMEstd.h"
#include "mainfrm.h"
#include "GMEDoc.h"
#include "GMEView.h"
#include "GMEBrowser.h"
#include "guimeta.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define MULTIPLE_OPEN_VIEW

/////////////////////////////////////////////////////////////////////////////
// CGMEBrowser dialog


CGMEBrowser *CGMEBrowser::theInstance = 0;

CGMEBrowser::CGMEBrowser(CWnd* /*pParent =NULL*/)
{
	//{{AFX_DATA_INIT(CGMEBrowser)
	//}}AFX_DATA_INIT
	VERIFY(theInstance == 0);
	theInstance = this;
}


void CGMEBrowser::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CGMEBrowser)
	DDX_Control(pDX, IDC_GME_ACTIVE_BROWSER_CTRL, m_GMEActiveBrowser);
	//}}AFX_DATA_MAP
}

void CGMEBrowser::CloseProject()
{
	m_GMEActiveBrowser.CloseCurrentProject();
}

bool CGMEBrowser::GetSelectedItems(CComPtr<IMgaObjects> &items)
{
	LPUNKNOWN lpSelObjs = m_GMEActiveBrowser.GetSelectedMgaObjects();
	CComQIPtr<IMgaObjects> objs( lpSelObjs);
	long cnt = 0;
	if( objs) COMTHROW( objs->get_Count( &cnt));
	if( cnt > 0)
		items = objs;

	return cnt > 0;
}

//#pragma warning ( disable : 4100 ) // Unreferenced formal param
bool CGMEBrowser::GetSelectedItem(CComPtr<IMgaObject>&)
{
	HRESULT hr = S_OK;

	return hr == S_OK;
}
//#pragma warning ( default : 4100 ) // Unreferenced formal param

void CGMEBrowser::SetProject(CComPtr<IMgaProject>& mgaProject)
{
	m_GMEActiveBrowser.SetCurrentProject(mgaProject);
}

void CGMEBrowser::RefreshAll()
{
	m_GMEActiveBrowser.RefreshAll();
}

void CGMEBrowser::ShowObject(LPUNKNOWN selected) 
{
	if (CGMEDoc::theInstance) {
		CGMEDoc::theInstance->ShowObject(selected);
	}
}

void CGMEBrowser::ShowObjectInParent( LPUNKNOWN selected)
{
	if (CGMEDoc::theInstance) {
		CGMEDoc::theInstance->ShowObject(selected, true);
	}
}

void CGMEBrowser::ShowFindDlg()
{
	if( CGMEDoc::theInstance && CMainFrame::theInstance)
		CMainFrame::theInstance->ShowFindDlg();
}

void CGMEBrowser::ShowObjInsp()
{
	if( CGMEDoc::theInstance && CMainFrame::theInstance && CGMEObjectInspector::theInstance)
	{
		HWND hwnd = CGMEObjectInspector::theInstance->GetSafeHwnd();
		if( hwnd) ::SetFocus( hwnd);
	}
}

void CGMEBrowser::CycleObjInsp( VARIANT_BOOL frwd)
{
	if( CGMEObjectInspector::theInstance)
	{
		CGMEObjectInspector::theInstance->CyclePanel( frwd);
	}
}

void CGMEBrowser::FocusItem(BSTR Id)
{
	m_GMEActiveBrowser.FocusItem(Id);
}

BEGIN_MESSAGE_MAP(CGMEBrowser, CDockablePane)
	//{{AFX_MSG_MAP(CGMEBrowser)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEBrowser message handlers

int CGMEBrowser::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_GMEActiveBrowser.Create(_T("ttt"),WS_CHILD | WS_VISIBLE,CRect(0,0,100,100),this,IDC_GME_ACTIVE_BROWSER_CTRL))
		return -1;

/*	m_szMin = CSize(200,200);
    m_szFloat = CSize(350,600); 
	m_szHorz = CSize(300,200);
    m_szVert = CSize(250,300);
*/	
	return 0;
}

void CGMEBrowser::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CDockablePane::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);

	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}


void CGMEBrowser::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);
	
	CRect rc;
	GetClientRect(rc);	
	m_GMEActiveBrowser.MoveWindow(rc);
}

BEGIN_EVENTSINK_MAP(CGMEBrowser, CDockablePane)
    //{{AFX_EVENTSINK_MAP(CGMEBrowser)
	ON_EVENT(CGMEBrowser, IDC_GME_ACTIVE_BROWSER_CTRL, 1 /* DblClickMgaObject */, OnDblClickMgaObjectGmeActiveBrowserCtrl, VTS_UNKNOWN)
	ON_EVENT(CGMEBrowser, IDC_GME_ACTIVE_BROWSER_CTRL, 2 /* ShowAttributes */, OnShowAttributesGmeActiveBrowserCtrl, VTS_UNKNOWN)
	ON_EVENT(CGMEBrowser, IDC_GME_ACTIVE_BROWSER_CTRL, 3 /* ShowPreferences */, OnShowPreferencesGmeActiveBrowserCtrl, VTS_UNKNOWN)
	ON_EVENT(CGMEBrowser, IDC_GME_ACTIVE_BROWSER_CTRL, 4 /* ShowProperties */, OnShowPropertiesGmeActiveBrowserCtrl, VTS_NONE)
	ON_EVENT(CGMEBrowser, IDC_GME_ACTIVE_BROWSER_CTRL, 5 /* ClickMgaObject */, OnClickMgaObjectGmeActiveBrowserCtrl, VTS_UNKNOWN)
	ON_EVENT(CGMEBrowser, IDC_GME_ACTIVE_BROWSER_CTRL, 6 /* ShowInParentMgaObject */, OnShowInParentMgaObjectGmeActiveBrowserCtrl, VTS_UNKNOWN)
	ON_EVENT(CGMEBrowser, IDC_GME_ACTIVE_BROWSER_CTRL, 7 /* RootFolderNameChanged */, OnRootFolderNameChangedGmeActiveBrowserCtrl, VTS_NONE)
	ON_EVENT(CGMEBrowser, IDC_GME_ACTIVE_BROWSER_CTRL, 8 /* ShowFindDlg */, OnShowFindDlgGmeActiveBrowserCtrl, VTS_NONE)
	ON_EVENT(CGMEBrowser, IDC_GME_ACTIVE_BROWSER_CTRL, 9 /* ShowObjInspDlg */, OnShowObjInspDlgGmeActiveBrowserCtrl, VTS_NONE)
	ON_EVENT(CGMEBrowser, IDC_GME_ACTIVE_BROWSER_CTRL, 10 /*CycleObjInsp*/ , OnCycleObjsInspGmeActiveBrowserCtrl, VTS_BOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CGMEBrowser::OnDblClickMgaObjectActivebrowserCtlr1(LPUNKNOWN selected) 
{
	ShowObject(selected);
}

void CGMEBrowser::OnCRonObjectActivebrowserCtlr1(LPUNKNOWN selected) 
{
	ShowObject(selected);
}

BOOL CGMEBrowser::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		switch(pMsg->wParam)
		{
			case VK_RETURN:
			case VK_ESCAPE:
			case VK_DELETE:
			// Modification by Volgyesi (undo problems)
			case VK_CONTROL:
			case 'z':
			case 'Z':
			// Modification End
			case VK_TAB:	// for JIRA GME-178
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return TRUE;
		}
	}

	return CDockablePane::PreTranslateMessage(pMsg);
}

void CGMEBrowser::ShowAttrPref(bool isAttr, LPUNKNOWN selected) {

	CComPtr<IUnknown> sunk = selected;

	/* Converting single selection to multiple 
	CComPtr<IMgaObjects> ccpSelectedObjects;
	COMTHROW(ccpSelectedObjects.CoCreateInstance(L"Mga.MgaObjects"));
	CComPtr<IMgaObject>ccpMgaObject(fco);
	ccpSelectedObjects->Append(ccpMgaObject);
	CGMEObjectInspector::theInstance->SetFCOs(ccpSelectedObjects);
	*/

	
	SetCurrObject(selected);
	CGMEObjectInspector::theInstance->ShowPanel(!isAttr);
}

void CGMEBrowser::SetCurrObject(LPUNKNOWN pMgaObject)
{
	CComPtr<IUnknown> sunk = pMgaObject;
	LPUNKNOWN pUnknown=m_GMEActiveBrowser.GetSelectedMgaObjects();
	CComQIPtr<IMgaObjects> ccpSelectedObjects(pUnknown);
	pUnknown->Release();
	ASSERT(ccpSelectedObjects!=NULL);
//	CComQIPtr<IMgaObjects> ccpSelectedObjects(pMgaObject);
	CGMEObjectInspector::theInstance->SetObjects(ccpSelectedObjects);
    CGMESearch::theInstance->SetSelMgaObjects(ccpSelectedObjects);
}

void CGMEBrowser::OnShowAttributesActivebrowserctrl1(LPUNKNOWN object) 
{
	ShowAttrPref(true, object);
}

void CGMEBrowser::OnShowPreferencesActivebrowserctrl1(LPUNKNOWN object) 
{
	ShowAttrPref(false, object);
}

void CGMEBrowser::OnDblClickMgaObjectGmeActiveBrowserCtrl(LPUNKNOWN pMgaObject) 
{
	ShowObject(pMgaObject);
//	ShowAttrPref(true, pMgaObject);
	
}

void CGMEBrowser::OnShowAttributesGmeActiveBrowserCtrl(LPUNKNOWN pMgaObject) 
{
	ShowAttrPref(true, pMgaObject);
	
}

void CGMEBrowser::OnShowPreferencesGmeActiveBrowserCtrl(LPUNKNOWN pMgaObject) 
{
	ShowAttrPref(false, pMgaObject);
}

void CGMEBrowser::OnShowPropertiesGmeActiveBrowserCtrl() 
{
	
	LPUNKNOWN pUnknown=m_GMEActiveBrowser.GetSelectedMgaObjects();
	CComQIPtr<IMgaObjects> ccpSelectedObjects(pUnknown);
	pUnknown->Release();
	ASSERT(ccpSelectedObjects!=NULL);
	CGMEObjectInspector::theInstance->SetObjects(ccpSelectedObjects);

	CGMEObjectInspector::theInstance->ShowPanel(2);
	
}

void CGMEBrowser::OnClickMgaObjectGmeActiveBrowserCtrl(LPUNKNOWN pMgaObject) 
{
	SetCurrObject(pMgaObject);
}

void CGMEBrowser::OnShowInParentMgaObjectGmeActiveBrowserCtrl(LPUNKNOWN pMgaObject) 
{
	ShowObjectInParent(pMgaObject);
}

void CGMEBrowser::OnShowFindDlgGmeActiveBrowserCtrl()
{
	ShowFindDlg();
}

void CGMEBrowser::OnShowObjInspDlgGmeActiveBrowserCtrl()
{
	ShowObjInsp();
}

void CGMEBrowser::OnCycleObjsInspGmeActiveBrowserCtrl(VARIANT_BOOL frwd)
{
	CycleObjInsp( frwd);
}

void CGMEBrowser::OnRootFolderNameChangedGmeActiveBrowserCtrl()
{
	theApp.UpdateProjectName(true);
}
