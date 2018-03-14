// GMEViewCtrl.cpp : Implementation of the CGMEViewCtrl ActiveX Control class.

#include "stdafx.h"
#include "GMEView.h"
#include "GMEViewCtrl.h"
#include "GMEViewPropPage.h"
#include "..\Interfaces\Mga_i.c"
#include "GMEViewStd.h"
//#include <comdef.h> // _bstr_t


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE(CGMEViewCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CGMEViewCtrl, COleControl)
	//{{AFX_MSG_MAP(CGMEViewCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CGMEViewCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CGMEViewCtrl)
	DISP_FUNCTION_ID(CGMEViewCtrl, "SetCurrentProject", dispidSetCurrentProject, SetCurrentProject, VT_EMPTY, VTS_UNKNOWN)
	DISP_FUNCTION_ID(CGMEViewCtrl, "SetMetaModel", dispidSetMetaModel, SetMetaModel, VT_EMPTY, VTS_UNKNOWN)
	DISP_FUNCTION_ID(CGMEViewCtrl, "SetModel", dispidSetModel, SetModel, VT_EMPTY, VTS_UNKNOWN)
	DISP_FUNCTION_ID(CGMEViewCtrl, "ChangeAspect", dispidChangeAspect, ChangeAspect, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CGMEViewCtrl, "CycleAspect", dispidCycleAspect, CycleAspect, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CGMEViewCtrl, "Invalidate", dispidInvalidate, Invalidate, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CGMEViewCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CGMEViewCtrl, COleControl)
	//{{AFX_EVENT_MAP(CGMEViewCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()



/////////////////////////////////////////////////////////////////////////////
// Native COM interface map - peter
BEGIN_INTERFACE_MAP(CGMEViewCtrl, COleControl)
	INTERFACE_PART(CGMEViewCtrl, IID_IMgaEventSink, EventSink)
END_INTERFACE_MAP()


// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CGMEViewCtrl, 1)
	PROPPAGEID(CGMEViewPropPage::guid)
END_PROPPAGEIDS(CGMEViewCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CGMEViewCtrl, "GMEVIEW.GMEViewCtrl.1",
	0x1758edff, 0x969c, 0x4f6e, 0xaf, 0x81, 0xe8, 0xaf, 0x7, 0xa3, 0x37, 0x99)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CGMEViewCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DGMEView =
		{ 0xA038BDA8, 0xD878, 0x41CF, { 0xA5, 0x91, 0x60, 0xA2, 0xDF, 0x3E, 0xDE, 0x70 } };
const IID BASED_CODE IID_DGMEViewEvents =
		{ 0xDF9490E3, 0x4BF6, 0x4DAD, { 0x95, 0x34, 0xAF, 0x8F, 0xD8, 0x1E, 0x68, 0x39 } };



// Control type information

static const DWORD BASED_CODE _dwGMEViewOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CGMEViewCtrl, IDS_GMEVIEW, _dwGMEViewOleMisc)



// CGMEViewCtrl::CGMEViewCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CGMEViewCtrl

BOOL CGMEViewCtrl::CGMEViewCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_GMEVIEW,
			IDB_GMEVIEW,
			afxRegInsertable | afxRegApartmentThreading,
			_dwGMEViewOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



/////////////////////////////////////////////////////////////////////////////
// CGMEViewCtrl::XEventSink - native COM interface - peter
STDMETHODIMP_(ULONG) CGMEViewCtrl::XEventSink::AddRef()
{
	METHOD_PROLOGUE(CGMEViewCtrl,EventSink)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CGMEViewCtrl::XEventSink::Release()
{
	METHOD_PROLOGUE(CGMEViewCtrl,EventSink)
	return pThis->ExternalRelease();
}

STDMETHODIMP CGMEViewCtrl::XEventSink::QueryInterface(REFIID riid, void** ppv)
{
	METHOD_PROLOGUE(CGMEViewCtrl,EventSink)
	return pThis->ExternalQueryInterface(&riid, ppv);
}

STDMETHODIMP CGMEViewCtrl::XEventSink::GlobalEvent(globalevent_enum event)
{
	METHOD_PROLOGUE(CGMEViewCtrl,EventSink)
	// TODO: Handle global events here - peter
	pThis->OnMgaGlobalEvent(event);
	return S_OK;
}

STDMETHODIMP CGMEViewCtrl::XEventSink::ObjectEvent(IMgaObject* obj, unsigned long eventmask, VARIANT v)
{
	METHOD_PROLOGUE(CGMEViewCtrl,EventSink)
	// TODO: Handle object events here - peter
	pThis->m_MgaObjectEventList.Add(obj,eventmask);
	return S_OK;
}


// CGMEViewCtrl::CGMEViewCtrl - Constructor

CGMEViewCtrl::CGMEViewCtrl():
	m_gmeViewFrame(NULL),
	aspect(0)
{
	InitializeIIDs(&IID_DGMEView, &IID_DGMEViewEvents);
	// TODO: Initialize your control's instance data here.
}



// CGMEViewCtrl::~CGMEViewCtrl - Destructor

CGMEViewCtrl::~CGMEViewCtrl()
{
	// TODO: Cleanup your control's instance data here.

	m_gmeViewFrame = NULL;
}



// CGMEViewCtrl::OnDraw - Drawing function

void CGMEViewCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	if (m_gmeViewFrame != NULL) {
		m_gmeViewFrame->MoveWindow(rcBounds, TRUE);
	} else {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBrush brush(RGB(255, 255, 255));
		pdc->FillRect(rcBounds, &brush);

		CRect edge(rcBounds);
		pdc->DrawEdge(&edge, EDGE_BUMP, BF_RECT);
		
		CString label("GMEView OCX");

		BITMAP bm;
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_GMEVIEW);
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
		pdc->TextOut(rcBounds.Width() / 2, rcBounds.Height() / 2, CString("GMEView ActiveX Control"));
	}
}



// CGMEViewCtrl::DoPropExchange - Persistence support

void CGMEViewCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CGMEViewCtrl::OnResetState - Reset control to default state

void CGMEViewCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CGMEViewCtrl::AboutBox - Display an "About" box to the user

void CGMEViewCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_GMEVIEW);
	dlgAbout.DoModal();
}



// CGMEViewCtrl message handlers

int CGMEViewCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

//	if (m_gmeViewFrame->Create(IDD_GMEVIEW_DIALOG, this) == FALSE)
//		return -1;

	if (m_gmeViewFrame == NULL) {
		m_gmeViewFrame = new CGMEViewFrame;
		CRect rect(0, 0, 0, 0);
		CString strTitle;
//		VERIFY(strTitle.LoadString(IDS_PROPSHT_CAPTION));
		strTitle = "DummyProbe Title";

		if (!m_gmeViewFrame->Create(NULL, strTitle, WS_CHILD, rect, this)) {
			delete m_gmeViewFrame;
			m_gmeViewFrame = NULL;
			return -1;
		} else {
			if (m_gmeViewFrame != NULL) {
				m_gmeViewFrame->SetCurrentProject(mgaProject);
				m_gmeViewFrame->SetMetaModel(mgaMetaModel);
				m_gmeViewFrame->SetModel(mgaModel);
				m_gmeViewFrame->ChangeAspect(aspect);

				if (!m_gmeViewFrame->IsWindowVisible())
					m_gmeViewFrame->ShowWindow(SW_SHOW);
				m_gmeViewFrame->Invalidate();
			}
		}
	}

	return 0;
}

BOOL CGMEViewCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTROLPARENT;
	return COleControl::PreCreateWindow(cs);
}

void CGMEViewCtrl::SetCurrentProject(IUnknown* project)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (mgaProject != NULL)
		mgaProject.Release();

	CComQIPtr<IMgaProject> ccpMgaProject(project);

	if (project != NULL && ccpMgaProject == NULL) {
		ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast MgaProject."));
		return;
	}

	if (m_gmeViewFrame != NULL)
		m_gmeViewFrame->SetCurrentProject(ccpMgaProject);
	else
		mgaProject = ccpMgaProject;
}

void CGMEViewCtrl::SetMetaModel(IUnknown* meta)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (mgaMetaModel != NULL)
		mgaMetaModel.Release();

	CComQIPtr<IMgaMetaModel> ccpMgaMetaModel(meta);

	if (meta != NULL && ccpMgaMetaModel == NULL) {
		ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast MgaMetaModel."));
		return;
	}

	if (m_gmeViewFrame != NULL)
		m_gmeViewFrame->SetMetaModel(ccpMgaMetaModel);
	else
		mgaMetaModel = ccpMgaMetaModel;
}

void CGMEViewCtrl::SetModel(IUnknown* model)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (mgaModel != NULL)
		mgaModel.Release();

	CComQIPtr<IMgaModel> ccpMgaModel(model);

	if (model != NULL && ccpMgaModel == NULL) {
		ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast MgaModel."));
		return;
	}

	if (m_gmeViewFrame != NULL)
		m_gmeViewFrame->SetModel(ccpMgaModel);
	else
		mgaModel = ccpMgaModel;
}

void CGMEViewCtrl::ChangeAspect(LONG index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_gmeViewFrame != NULL)
		m_gmeViewFrame->ChangeAspect(index);
	else
		aspect = index;
}

void CGMEViewCtrl::CycleAspect(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO
/*	if (m_gmeViewFrame != NULL)
		m_gmeViewFrame->CycleAspect();
	else
		aspect = index;*/
}

void CGMEViewCtrl::Invalidate(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_gmeViewFrame != NULL)
		m_gmeViewFrame->Invalidate();
}

void CGMEViewCtrl::OnMgaObjectEvent(IMgaObject* obj, unsigned long eventmask)
{
	CComPtr<IMgaObject> ccpMgaObject(obj);
	// Handling Object Events
	EVENT_TRACE("\n__________ Object Event Begin _________\n");

#ifdef _DEBUG
	if (eventmask & OBJEVENT_DESTROYED) {
		EVENT_TRACE("OBJEVENT_DESTROYED\n");
	}
	if (eventmask & OBJEVENT_REGISTRY) {
		EVENT_TRACE("OBJEVENT_REGISTRY\n");
	}
	if (eventmask & OBJEVENT_PROPERTIES) {
		EVENT_TRACE("OBJEVENT_PROPERTIES\n");
	}
	if (eventmask & OBJEVENT_CREATED) {
		EVENT_TRACE("OBJEVENT_CREATED\n");
	}
	if (eventmask & OBJEVENT_ATTR) {
		EVENT_TRACE("OBJEVENT_ATTR\n");
	}
	if(eventmask & OBJEVENT_NEWCHILD) {
		EVENT_TRACE("OBJEVENT_NEWCHILD\n");
	}
	if(eventmask & OBJEVENT_RELATION) {
		EVENT_TRACE("OBJEVENT_RELATION\n");
	}
	if(eventmask & OBJEVENT_SUBT_INST) {
		EVENT_TRACE("OBJEVENT_SUBT_INST\n");
	}
	if(eventmask & OBJEVENT_PARENT) {
		EVENT_TRACE("OBJEVENT_PARENT\n");
	}
	if(eventmask & OBJEVENT_LOSTCHILD) {
		EVENT_TRACE("OBJEVENT_LOSTCHILD\n");
	}
	if(eventmask & OBJEVENT_REFERENCED) {
		EVENT_TRACE("OBJEVENT_REFERENCED\n");
	}
	if(eventmask & OBJEVENT_CONNECTED) {
		EVENT_TRACE("OBJEVENT_CONNECTED\n");
	}
	if(eventmask & OBJEVENT_SETINCLUDED) {
		EVENT_TRACE("OBJEVENT_SETINCLUDED\n");
	}
	if(eventmask & OBJEVENT_REFRELEASED) {
		EVENT_TRACE("OBJEVENT_REFRELEASED\n");
	}
	if(eventmask & OBJEVENT_DISCONNECTED) {
		EVENT_TRACE("OBJEVENT_DISCONNECTED\n");
	}
	if(eventmask & OBJEVENT_SETEXCLUDED) {
		EVENT_TRACE("OBJEVENT_SETEXCLUDED\n");
	}
	if(eventmask & OBJEVENT_USERBITS) {
		EVENT_TRACE("OBJEVENT_USERBITS\n");
	}
	if(eventmask & OBJEVENT_CLOSEMODEL) {
		EVENT_TRACE("OBJEVENT_CLOSEMODEL\n");
	}
#endif /*_DEBUG */

	EVENT_TRACE("_________ Object Event End _______\n");
}

void CGMEViewCtrl::OnMgaGlobalEvent(globalevent_enum event)
{
	// Handling Global Events
	if (event == GLOBALEVENT_COMMIT_TRANSACTION) {
		EVENT_TRACE("GLOBALEVENT_COMMIT_TRANSACTION\n");

		// Send message to UI
		PropagateMgaMessages();
	} else if (event == GLOBALEVENT_NOTIFICATION_READY) {
		EVENT_TRACE("GLOBALEVENT_NOTIFICATION_READY\n");

		// Send message to UI
		PropagateMgaMessages();
	} else if (event == GLOBALEVENT_ABORT_TRANSACTION) {
		EVENT_TRACE("GLOBALEVENT_ABORT_TRANSACTION\n");

		// Removing the elements
		m_MgaObjectEventList.RemoveAll();
	} else if (event == GLOBALEVENT_UNDO) {
		EVENT_TRACE("GLOBALEVENT_UNDO\n");
		// Refresh
	} else if (event==GLOBALEVENT_REDO) {
		EVENT_TRACE("GLOBALEVENT_REDO\n");
		// Refresh
	}

	// Not handled messages are not in the release
	// but useful for development
#ifdef _DEBUG
	if (event == GLOBALEVENT_PROJECT_PROPERTIES) {
		EVENT_TRACE("GLOBALEVENT_PROJECT_PROPERTIES\n");
	}
	if (event == GLOBALEVENT_OPEN_PROJECT) {
		EVENT_TRACE("GLOBALEVENT_OPEN_PROJECT\n");
	}
	if (event == GLOBALEVENT_CLOSE_PROJECT) {
		EVENT_TRACE("GLOBALEVENT_CLOSE_PROJECT\n");
	}
	if (event == GLOBALEVENT_NEW_TERRITORY) {
		EVENT_TRACE("GLOBALEVENT_NEW_TERRITORY\n");
	}
	if (event == GLOBALEVENT_DESTROY_TERRITORY) {
		EVENT_TRACE("GLOBALEVENT_DESTROY_TERRITORY\n");
	}

#endif /* _DEBUG */

	EVENT_TRACE("_________ Global Event End _________\n");
}

void CGMEViewCtrl::SendAspectChanged(LONG index)
{
	this->FireAspectChanged(index);
}

void CGMEViewCtrl::SendZoomChanged(LONG index)
{
	this->FireZoomChanged(index);
}

void CGMEViewCtrl::SendWriteStatusZoom(LONG zoomVal)
{
	this->FireWriteStatusZoom(zoomVal);
}

void CGMEViewCtrl::PropagateMgaMessages()
{
	// Iterating through the list
	while (!m_MgaObjectEventList.IsEmpty()) {
		CMgaObjectEventWrapper* pEventWrapper =
			(CMgaObjectEventWrapper*)m_MgaObjectEventList.RemoveHead();
		// Propagating message to AggregatePropertyPage

		OnMgaObjectEvent(pEventWrapper->m_ccpMgaObject, pEventWrapper->m_eventmask);

		delete pEventWrapper;
	}
}

