// GMEObjectInspector.cpp: implementation of the CGMEObjectInspector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMEApp.h"
#include "GMEstd.h"
#include "mainfrm.h"
#include "GMEDoc.h"
#include "GMEView.h"
#include "GMEObjectInspector.h"
#include "guimeta.h"

CGMEObjectInspector* CGMEObjectInspector::theInstance = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGMEObjectInspector::CGMEObjectInspector()
{
	//{{AFX_DATA_INIT(CGMEObjectInspector)
	//}}AFX_DATA_INIT

	VERIFY(theInstance == 0);
	theInstance = this;
}


void CGMEObjectInspector::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CGMEObjectInspector)	
	DDX_Control(pDX, IDC_OBJECT_INSPECTOR_CTRL, m_ObjectInspector);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CGMEObjectInspector, CDockablePane)
	//{{AFX_MSG_MAP(CGMEObjectInspector)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CGMEObjectInspector, CDockablePane)
    //{{AFX_EVENTSINK_MAP(CGMEBrowser)
	ON_EVENT(CGMEObjectInspector, IDC_OBJECT_INSPECTOR_CTRL, 1 /* RootFolderNameChanged */, OnRootFolderNameChangedGmeObjectInspectorCtrl, VTS_NONE)
	ON_EVENT(CGMEObjectInspector, IDC_OBJECT_INSPECTOR_CTRL, 2 /* ConvertPathToCustom */, OnConvertPathToCustomGmeObjectInspectorCtrl, VTS_UNKNOWN)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

int CGMEObjectInspector::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if(!m_ObjectInspector.Create(_T("ObjectInspector"),WS_CHILD | WS_VISIBLE,CRect(0,0,230,300),this,IDC_OBJECT_INSPECTOR_CTRL))
		return -1;

	return 0;
}

void CGMEObjectInspector::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(rc);	
	m_ObjectInspector.MoveWindow(rc);
	
}


void CGMEObjectInspector::SetProject(CComPtr<IMgaProject>& mgaProject)
{
	m_ObjectInspector.SetMgaProject(mgaProject);
}


void CGMEObjectInspector::CloseProject()
{
	m_ObjectInspector.SetMgaProject(NULL);
}


void CGMEObjectInspector::SetObjects(LPUNKNOWN pUnknown)
{
	m_ObjectInspector.SetMgaObject(pUnknown);
}

LPUNKNOWN CGMEObjectInspector::GetObjects()
{
	return m_ObjectInspector.GetMgaObject();
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

BOOL CGMEObjectInspector::PreTranslateMessage(MSG* pMsg) 
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


void CGMEObjectInspector::ShowPanel(long panelID)
{
    CMainFrame::theInstance->ShowObjectInspector();
	m_ObjectInspector.ShowPanel(panelID);
}

void CGMEObjectInspector::CyclePanel( VARIANT_BOOL frwd)
{
    CMainFrame::theInstance->ShowObjectInspector();
	m_ObjectInspector.CyclePanel( frwd);
}

void CGMEObjectInspector::OnRootFolderNameChangedGmeObjectInspectorCtrl()
{
	theApp.UpdateProjectName();
}

void CGMEObjectInspector::OnConvertPathToCustomGmeObjectInspectorCtrl(LPUNKNOWN pMgaObject)
{
	CComPtr<IUnknown> sunk(pMgaObject);
	CGMEDoc::theInstance->ConvertPathToCustom(sunk);
}
