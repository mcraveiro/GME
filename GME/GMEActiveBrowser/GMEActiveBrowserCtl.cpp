// GMEActiveBrowserCtl.cpp : Implementation of the CGMEActiveBrowserCtrl ActiveX Control class.

#include "stdafx.h"
#include "GMEActiveBrowser.h"
#include "GMEActiveBrowserCtl.h"
#include "GMEActiveBrowserPpg.h"
#include "Mga_i.c"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




IMPLEMENT_DYNCREATE(CGMEActiveBrowserCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CGMEActiveBrowserCtrl, COleControl)
	//{{AFX_MSG_MAP(CGMEActiveBrowserCtrl)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CGMEActiveBrowserCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CGMEActiveBrowserCtrl)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "SetCurrentProject", SetCurrentProject, VT_I2, VTS_UNKNOWN)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "CloseCurrentProject", CloseCurrentProject, VT_I2, VTS_NONE)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "RefreshAll", RefreshAll, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "GetSelectedMgaObjects", GetSelectedMgaObjects, VT_UNKNOWN, VTS_NONE)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "OpenProject", OpenProject, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "FocusItem", FocusItem, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "OpenItem", OpenItem, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "OpenSubtree", OpenSubtree, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "CloseSubtree", CloseSubtree, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "Up", Up, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "Down", Down, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "ChangePropPage", ChangePropPage, VT_EMPTY, VTS_I2)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION(CGMEActiveBrowserCtrl, "HighlightItem", HighlightItem, VT_EMPTY, VTS_UNKNOWN VTS_I4)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CGMEActiveBrowserCtrl, COleControl)
	//{{AFX_EVENT_MAP(CGMEActiveBrowserCtrl)
	EVENT_CUSTOM("DblClickMgaObject", FireDblClickMgaObject, VTS_UNKNOWN)
	EVENT_CUSTOM("ShowAttributes", FireShowAttributes, VTS_UNKNOWN)
	EVENT_CUSTOM("ShowPreferences", FireShowPreferences, VTS_UNKNOWN)
	EVENT_CUSTOM("ShowProperties", FireShowProperties, VTS_NONE)
	EVENT_CUSTOM("ClickMgaObject", FireClickMgaObject, VTS_UNKNOWN)
	EVENT_CUSTOM("ShowInParentMgaObject", FireShowInParentMgaObject, VTS_UNKNOWN)
	EVENT_CUSTOM("RootFolderNameChanged", FireRootFolderNameChanged, VTS_NONE)
	EVENT_CUSTOM("ShowFindDlg", FireShowFindDlg, VTS_NONE)
	EVENT_CUSTOM("ShowObjInspDlg", FireShowObjInspDlg, VTS_NONE)
	EVENT_CUSTOM("CycleObjInsp", CycleObjInsp, VTS_BOOL)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// Native COM interface map - peter
BEGIN_INTERFACE_MAP(CGMEActiveBrowserCtrl, COleControl)
	INTERFACE_PART(CGMEActiveBrowserCtrl, IID_IMgaEventSink, EventSink)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CGMEActiveBrowserCtrl, 1)
	PROPPAGEID(CGMEActiveBrowserPropPage::guid)
END_PROPPAGEIDS(CGMEActiveBrowserCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CGMEActiveBrowserCtrl, "GME.GMEActiveBrowserCtrl.1",
	0xdd4f2deb, 0xd064, 0x4844, 0x9e, 0xe1, 0x4, 0x67, 0x3c, 0x87, 0x2e, 0x7b)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CGMEActiveBrowserCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DGMEActiveBrowser =
		{ 0x9d345706, 0xde7, 0x4a6a, { 0xb3, 0x2a, 0xdd, 0x1b, 0xb2, 0xf8, 0xb8, 0x1f } };
const IID BASED_CODE IID_DGMEActiveBrowserEvents =
		{ 0x9953ea94, 0x3294, 0x4f9e, { 0xbf, 0x48, 0x99, 0xae, 0x7f, 0x47, 0x3, 0x8b } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwGMEActiveBrowserOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_IGNOREACTIVATEWHENVISIBLE |	// TODO: this ignores the previous value, why is this needed?
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CGMEActiveBrowserCtrl, IDS_GMEACTIVEBROWSER, _dwGMEActiveBrowserOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserCtrl::CGMEActiveBrowserCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CGMEActiveBrowserCtrl

BOOL CGMEActiveBrowserCtrl::CGMEActiveBrowserCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_GMEACTIVEBROWSER,
			IDB_GMEACTIVEBROWSER,
			afxRegInsertable | afxRegApartmentThreading,
			_dwGMEActiveBrowserOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserCtrl::XEventSink - native COM interface - peter
STDMETHODIMP_(ULONG) CGMEActiveBrowserCtrl::XEventSink::AddRef()
{
	METHOD_PROLOGUE(CGMEActiveBrowserCtrl,EventSink)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CGMEActiveBrowserCtrl::XEventSink::Release()
{
	METHOD_PROLOGUE(CGMEActiveBrowserCtrl,EventSink)
	return pThis->ExternalRelease();
}

STDMETHODIMP CGMEActiveBrowserCtrl::XEventSink::QueryInterface(REFIID riid, void** ppv)
{
	METHOD_PROLOGUE(CGMEActiveBrowserCtrl,EventSink)
	return pThis->ExternalQueryInterface(&riid, ppv);
}

STDMETHODIMP CGMEActiveBrowserCtrl::XEventSink::GlobalEvent(globalevent_enum event)
{
	METHOD_PROLOGUE(CGMEActiveBrowserCtrl,EventSink)

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	// Stopping transaction handling
	pMgaContext->SetEventTransactionMode(true);

	// Forwarding MGA messages to the control
	pThis->OnMgaGlobalEvent(event);

	// Starting transaction handling
	pMgaContext->SetEventTransactionMode(false);
	return S_OK;
}

STDMETHODIMP CGMEActiveBrowserCtrl::XEventSink::ObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v)
{
	METHOD_PROLOGUE(CGMEActiveBrowserCtrl,EventSink)

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;


	// Stopping transaction handling
	pMgaContext->SetEventTransactionMode(TRUE);

	// Forwarding MGA messages to the control
	pThis->OnMgaObjectEvent(obj, eventmask, v);

	// Starting transaction handling
	pMgaContext->SetEventTransactionMode(FALSE);

	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserCtrl::CGMEActiveBrowserCtrl - Constructor

CGMEActiveBrowserCtrl::CGMEActiveBrowserCtrl()
{
	InitializeIIDs(&IID_DGMEActiveBrowser, &IID_DGMEActiveBrowserEvents);
	
	m_pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	m_pApp->SetCtrl(this);
	m_pPropFrame=NULL;


	
}


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserCtrl::~CGMEActiveBrowserCtrl - Destructor

CGMEActiveBrowserCtrl::~CGMEActiveBrowserCtrl()
{
	// TODO: Cleanup your control's instance data here.	


}


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserCtrl::OnDraw - Drawing function

void CGMEActiveBrowserCtrl::OnDraw( CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (m_pPropFrame != NULL) {
		m_pPropFrame->MoveWindow(rcBounds, TRUE);
	} else {
		pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH) GetStockObject(WHITE_BRUSH)));
		pdc->TextOut(rcBounds.left + 1, rcBounds.top + 1, _T("GME ActiveBrowser"));
	}
}


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserCtrl::DoPropExchange - Persistence support

void CGMEActiveBrowserCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
// For information on using these flags, please see MFC technical note
// #nnn, "Optimizing an ActiveX Control".
DWORD CGMEActiveBrowserCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control can receive mouse notifications when inactive.
	// TODO: if you write handlers for WM_SETCURSOR and WM_MOUSEMOVE,
	//		avoid using the m_hWnd member variable without first
	//		checking that its value is non-NULL.
	dwFlags |= pointerInactive;
	return dwFlags;
}


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserCtrl::OnResetState - Reset control to default state

void CGMEActiveBrowserCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CGMEActiveBrowserCtrl message handlers

int CGMEActiveBrowserCtrl::CreateActiveBrowser()
{
	if (m_pPropFrame == NULL) {
		m_pPropFrame = new CActiveBrowserPropertyFrame;
		CRect rect(0, 0, 0, 0);
		CString strTitle;
		VERIFY(strTitle.LoadString(IDS_PROPSHT_CAPTION));

		if (!m_pPropFrame->CreateEx(WS_EX_CONTROLPARENT, NULL, strTitle,  WS_CHILD, rect, this)) {
			delete m_pPropFrame;
			m_pPropFrame = NULL;
			return -1;
		}
	}

	// Before unhiding the modeless property sheet, update its
	// settings appropriately.  For example, if you are reflecting
	// the state of the currently selected item, pick up that
	// information from the active view and change the property
	// sheet settings now.

	if (m_pPropFrame != NULL && !m_pPropFrame->IsWindowVisible())
		m_pPropFrame->ShowWindow(SW_SHOW);
	
	return 0;
}

int CGMEActiveBrowserCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return CreateActiveBrowser();
}

short CGMEActiveBrowserCtrl::SetCurrentProject(LPUNKNOWN pMgaProject) 
{
	return m_pApp->m_CurrentProject.CreateCurrentProject(pMgaProject);
}

short CGMEActiveBrowserCtrl::CloseCurrentProject() 
{
	m_pApp->m_CurrentProject.CloseCurrentProject();
	return 0;
}

void CGMEActiveBrowserCtrl::RefreshAll() 
{
	m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.Refresh();
}


LPUNKNOWN CGMEActiveBrowserCtrl::GetSelectedMgaObjects() 
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	CMgaMappedTreeCtrlBase* pTreeCtrl = &m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.m_TreeAggregate;

	if( 1 == m_pPropFrame->m_pModelessPropSheet->GetActiveIndex()) // if the INHER tree is active use that tree
		pTreeCtrl = &m_pPropFrame->m_pModelessPropSheet->m_PageInheritance.m_TreeInheritance;
	

	CComPtr<IMgaObjects> ccpSelectedObjects;

	try
	{
		pMgaContext->BeginTransaction(true);
		
		COMTHROW(ccpSelectedObjects.CoCreateInstance(L"Mga.MgaObjects"));
		
		HTREEITEM hItem=pTreeCtrl->GetFirstSelectedItem();

		while(hItem)
		{
			LPUNKNOWN pUnknown;
			pTreeCtrl->LookupObjectUnknown(hItem, pUnknown);
			CComQIPtr<IMgaObject> ccpMgaObject(pUnknown);
			
			ASSERT(ccpMgaObject!=NULL);
			COMTHROW(ccpSelectedObjects->Append(ccpMgaObject));

			hItem=pTreeCtrl->GetNextSelectedItem(hItem);
		}
		

		pMgaContext->CommitTransaction();
	}
	catch (hresult_exception& )
	{
		pMgaContext->AbortTransaction();
	}

	return ccpSelectedObjects.Detach();
}


void CGMEActiveBrowserCtrl::FocusItem(BSTR Id) 
{
	m_pPropFrame->m_pModelessPropSheet->SetActivePage(0);
	m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.GotoIUnk(Id);
}


void CGMEActiveBrowserCtrl::OpenItem(BSTR Id) 
{
	// focus + enter
	m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.GotoIUnk(Id);
	m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.m_TreeAggregate.SendMessage(WM_KEYDOWN, (WPARAM)VK_RETURN, (LPARAM)0);
}

void CGMEActiveBrowserCtrl::OpenSubtree(BSTR Id) 
{
	 // focus + '+'
	m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.GotoIUnk(Id);
	m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.m_TreeAggregate.SendMessage(WM_KEYDOWN, (WPARAM)107, (LPARAM)0);
}

void CGMEActiveBrowserCtrl::CloseSubtree(BSTR Id) 
{
	 // focus + '-'
	m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.GotoIUnk(Id);
	m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.m_TreeAggregate.SendMessage(WM_KEYDOWN, (WPARAM)109, (LPARAM)0);
}

void CGMEActiveBrowserCtrl::Up() 
{
	m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.m_TreeAggregate.SendMessage(WM_KEYDOWN, (WPARAM)VK_UP, (LPARAM)0);
}

void CGMEActiveBrowserCtrl::Down() 
{
	m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.m_TreeAggregate.SendMessage(WM_KEYDOWN, (WPARAM)VK_DOWN, (LPARAM)0);
}

void CGMEActiveBrowserCtrl::ChangePropPage(browser_page page) 
{
	 switch (page)
	 {
	 case 	MODE_AGGRE:
		 m_pPropFrame->m_pModelessPropSheet->SetActivePage(0);
		 break;
	 case 	MODE_INHERI:
		 m_pPropFrame->m_pModelessPropSheet->SetActivePage(1);
		 break;
	 case 	MODE_META:
		 m_pPropFrame->m_pModelessPropSheet->SetActivePage(2);
		 break;
	 }
}

void CGMEActiveBrowserCtrl::OnMgaObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v)
{

	m_MgaObjectEventList.Add(obj,eventmask);

#ifdef _DEBUG
	EVENT_TRACE("\n__________ Object Event Begin _________\n");

	// Handling Object Events
	if(eventmask&OBJEVENT_CREATED)
	{
		EVENT_TRACE("OBJEVENT_CREATED\n");

	}
	if(eventmask&OBJEVENT_DESTROYED)
	{
		EVENT_TRACE("OBJEVENT_DESTROYED\n");

	}
	if(eventmask&OBJEVENT_ATTR)
	{
		EVENT_TRACE("OBJEVENT_ATTR\n");

	}
	if(eventmask&OBJEVENT_REGISTRY)
	{
		EVENT_TRACE("OBJEVENT_REGISTRY\n");
	}
	if(eventmask&OBJEVENT_NEWCHILD)
	{
		EVENT_TRACE("OBJEVENT_NEWCHILD\n");
	}
	if(eventmask&OBJEVENT_RELATION)
	{
		EVENT_TRACE("OBJEVENT_RELATION\n");
	}
	if(eventmask&OBJEVENT_PROPERTIES)
	{
		EVENT_TRACE("OBJEVENT_PROPERTIES\n");
	}
	if(eventmask&OBJEVENT_SUBT_INST)
	{
		EVENT_TRACE("OBJEVENT_SUBT_INST\n");
	}
	if(eventmask&OBJEVENT_PARENT)
	{
		EVENT_TRACE("OBJEVENT_PARENT\n");
	}
	if(eventmask&OBJEVENT_LOSTCHILD)
	{
		EVENT_TRACE("OBJEVENT_LOSTCHILD\n");
	}
	if(eventmask&OBJEVENT_REFERENCED)
	{
		EVENT_TRACE("OBJEVENT_REFERENCED\n");
	}
	if(eventmask&OBJEVENT_CONNECTED)
	{
		EVENT_TRACE("OBJEVENT_CONNECTED\n");
	}
	if(eventmask&OBJEVENT_SETINCLUDED)
	{
		EVENT_TRACE("OBJEVENT_SETINCLUDED\n");
	}
	if(eventmask&OBJEVENT_REFRELEASED)
	{
		EVENT_TRACE("OBJEVENT_REFRELEASED\n");
	}
	if(eventmask&OBJEVENT_DISCONNECTED)
	{
		EVENT_TRACE("OBJEVENT_DISCONNECTED\n");
	}
	if(eventmask&OBJEVENT_SETEXCLUDED)
	{
		EVENT_TRACE("OBJEVENT_SETEXCLUDED\n");
	}
	if(eventmask&OBJEVENT_USERBITS)
	{
		EVENT_TRACE("OBJEVENT_USERBITS\n");
	}
	if(eventmask&OBJEVENT_CLOSEMODEL)
	{
		EVENT_TRACE("OBJEVENT_CLOSEMODEL\n");
	}
#endif /*_DEBUG */

	EVENT_TRACE("_________ Object Event End _______\n");

}


void CGMEActiveBrowserCtrl::OnMgaGlobalEvent(globalevent_enum event)
{
	EVENT_TRACE("\n________ Global Event Begin _________\n");

	// Handling Global Events
	if(event==GLOBALEVENT_COMMIT_TRANSACTION)
	{
		EVENT_TRACE("GLOBALEVENT_COMMIT_TRANSACTION\n");

		// Send message to UI
		PropagateMgaMessages();
	}
	else if(event==GLOBALEVENT_NOTIFICATION_READY)
	{
		EVENT_TRACE("GLOBALEVENT_NOTIFICATION_READY\n");

		// Send message to UI
		PropagateMgaMessages();
	}
	else if(event==GLOBALEVENT_ABORT_TRANSACTION)
	{
		EVENT_TRACE("GLOBALEVENT_ABORT_TRANSACTION\n");

        //m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.Refresh();

		// Removing the elements
		m_MgaObjectEventList.RemoveAll();

		m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.Refresh();
		m_pPropFrame->m_pModelessPropSheet->m_PageInheritance.Refresh();
	}
	else if(event==GLOBALEVENT_UNDO)
	{
		EVENT_TRACE("GLOBALEVENT_UNDO\n");
		m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.Refresh();
		m_pPropFrame->m_pModelessPropSheet->m_PageInheritance.Refresh();
	}
	else if(event==GLOBALEVENT_REDO)
	{
		m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.Refresh();
		m_pPropFrame->m_pModelessPropSheet->m_PageInheritance.Refresh();
		EVENT_TRACE("GLOBALEVENT_REDO\n");
	}

	// Not handled messages are not in the release
	// but useful for development
#ifdef _DEBUG
	if(event==GLOBALEVENT_PROJECT_PROPERTIES)
	{
		EVENT_TRACE("GLOBALEVENT_PROJECT_PROPERTIES\n");
	}

	if(event==GLOBALEVENT_OPEN_PROJECT)
	{
		EVENT_TRACE("GLOBALEVENT_OPEN_PROJECT\n");
	}
	if(event==GLOBALEVENT_CLOSE_PROJECT)
	{
		EVENT_TRACE("GLOBALEVENT_CLOSE_PROJECT\n");
	}
	if(event==GLOBALEVENT_NEW_TERRITORY)
	{
		EVENT_TRACE("GLOBALEVENT_NEW_TERRITORY\n");
	}
	if(event==GLOBALEVENT_DESTROY_TERRITORY)
	{
		EVENT_TRACE("GLOBALEVENT_DESTROY_TERRITORY\n");
	}
	if(event==APPEVENT_XML_IMPORT_BEGIN)
	{
		EVENT_TRACE("APPEVENT_XML_IMPORT_BEGIN\n");
	}
	if(event==APPEVENT_XML_IMPORT_END)
	{
		EVENT_TRACE("APPEVENT_XML_IMPORT_END\n");
	}
	if(event==APPEVENT_XML_IMPORT_FCOS_BEGIN)
	{
		EVENT_TRACE("APPEVENT_XML_IMPORT_FCOS_BEGIN\n");
	}
	if(event==APPEVENT_XML_IMPORT_FCOS_END)
	{
		EVENT_TRACE("APPEVENT_XML_IMPORT_FCOS_END\n");
	}
	if(event==APPEVENT_XML_IMPORT_SPECIAL_BEGIN)
	{
		EVENT_TRACE("APPEVENT_XML_IMPORT_SPECIAL_BEGIN\n");
	}
	if(event==APPEVENT_XML_IMPORT_SPECIAL_END)
	{
		EVENT_TRACE("APPEVENT_XML_IMPORT_SPECIAL_END\n");
	}

#endif /* _DEBUG */

	EVENT_TRACE("_________ Global Event End _________\n");
	

}

void CGMEActiveBrowserCtrl::PropagateMgaMessages()
{
	// Iterating through the list
	while(!m_MgaObjectEventList.IsEmpty())
	{
		CMgaObjectEventWrapper* pEventWrapper=
			(CMgaObjectEventWrapper*)m_MgaObjectEventList.RemoveHead();
		// Propagating message to AggregatePropertyPage
		m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.OnMgaEvent\
						(pEventWrapper->m_ccpMgaObject,pEventWrapper->m_eventmask);

		// ... and to the InheritancePropertyPage as well
		m_pPropFrame->m_pModelessPropSheet->m_PageInheritance.OnMgaEvent\
						(pEventWrapper->m_ccpMgaObject,pEventWrapper->m_eventmask);

		delete pEventWrapper;
	}
}


void CGMEActiveBrowserCtrl::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	COleControl::OnWindowPosChanged(lpwndpos);

	CRect clientRect;
	GetClientRect(clientRect);

	m_pPropFrame->MoveWindow(clientRect, TRUE);
}


void CGMEActiveBrowserCtrl::OpenProject(LPCTSTR pProjectName) 
{
	
	CComPtr<IMgaProject> ccpMgaProject;
	try
	{
		COMTHROW(ccpMgaProject.CoCreateInstance(OLESTR("Mga.MgaProject")));
	
		VARIANT_BOOL vtbMode;	
		CComBSTR bszProjectName(pProjectName);
	
		COMTHROW(ccpMgaProject->Open(bszProjectName, &vtbMode));
		SetCurrentProject(ccpMgaProject);

	}catch (hresult_exception &)
	{
		MessageBox(_T("Error opening project"),_T("Error"),MB_ICONERROR|MB_OK);
	}

}

BOOL CGMEActiveBrowserCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTROLPARENT;
	return COleControl::PreCreateWindow(cs);
}


void CGMEActiveBrowserCtrl::HighlightItem(IUnknown* item, int highlight)
{
	if (item == nullptr)
		AfxThrowOleDispatchException(201, _T("First parameter must not be null."));
	CComPtr<IMgaObject> object;
	item->QueryInterface(&object.p);
	if (!object)
		AfxThrowOleDispatchException(202, L"First parameter is of wrong type. It must be IMgaObject.");

	CGMEActiveBrowserApp* pApp = (CGMEActiveBrowserApp*)AfxGetApp();
	try
	{
		if (pApp)
		{
			pApp->m_CurrentProject.m_MgaContext.SetEventTransactionMode(true);
			try
			{
				m_pPropFrame->m_pModelessPropSheet->m_PageAggregate.HighlightItem(object, highlight);
			}
			catch (...)
			{
				pApp->m_CurrentProject.m_MgaContext.SetEventTransactionMode(false);
				throw;
			}
			pApp->m_CurrentProject.m_MgaContext.SetEventTransactionMode(false);
		}
	}
	catch (const _com_error& e)
	{
		AfxThrowOleDispatchException(203, e.Description());
	}
}
