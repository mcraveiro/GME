// PartBrowserCtrl.cpp : Implementation of the CPartBrowserCtrl ActiveX Control class.

#include "stdafx.h"
#include "PartBrowser.h"
#include "PartBrowserCtrl.h"
#include "PartBrowserPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE(CPartBrowserCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CPartBrowserCtrl, COleControl)
	//{{AFX_MSG_MAP(CPartBrowserCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CPartBrowserCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CPartBrowserCtrl)
	DISP_FUNCTION_ID(CPartBrowserCtrl, "SetCurrentProject", dispidSetCurrentProject, SetCurrentProject, VT_EMPTY, VTS_UNKNOWN)
	DISP_FUNCTION_ID(CPartBrowserCtrl, "SetMetaModel", dispidSetMetaModel, SetMetaModel, VT_EMPTY, VTS_UNKNOWN)
	DISP_FUNCTION_ID(CPartBrowserCtrl, "SetBgColor", dispidSetBgColor, SetBgColor, VT_EMPTY, VTS_COLOR)
	DISP_FUNCTION_ID(CPartBrowserCtrl, "ChangeAspect", dispidChangeAspect, ChangeAspect, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CPartBrowserCtrl, "CycleAspect", dispidCycleAspect, CycleAspect, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CPartBrowserCtrl, "RePaint", dispidRePaint, RePaint, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CPartBrowserCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CPartBrowserCtrl, COleControl)
	//{{AFX_EVENT_MAP(CPartBrowserCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Native COM interface map - peter
BEGIN_INTERFACE_MAP(CPartBrowserCtrl, COleControl)
	INTERFACE_PART(CPartBrowserCtrl, __uuidof(IMgaEventSink), EventSink)
END_INTERFACE_MAP()


// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CPartBrowserCtrl, 1)
	PROPPAGEID(CPartBrowserPropPage::guid)
END_PROPPAGEIDS(CPartBrowserCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPartBrowserCtrl, "PARTBROWSER.PartBrowserCtrl.1",
	0x1ff57057, 0xd598, 0x4fc2, 0x8f, 0x9d, 0x70, 0x8f, 0x59, 0xb9, 0xc0, 0x17)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CPartBrowserCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DPartBrowser =
		{ 0x2D05F0EE, 0xEB81, 0x4D1D, { 0xA5, 0xC6, 0x43, 0x14, 0x6F, 0x38, 0xBF, 0x81 } };
const IID BASED_CODE IID_DPartBrowserEvents =
		{ 0x5042C3E4, 0x58A9, 0x4059, { 0xA5, 0xE4, 0xFD, 0x7B, 0xFB, 0x3A, 0xA2, 0x2B } };



// Control type information

static const DWORD BASED_CODE _dwPartBrowserOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CPartBrowserCtrl, IDS_PARTBROWSER, _dwPartBrowserOleMisc)



// CPartBrowserCtrl::CPartBrowserCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CPartBrowserCtrl

BOOL CPartBrowserCtrl::CPartBrowserCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_PARTBROWSER,
			IDB_PARTBROWSER,
			afxRegInsertable | afxRegApartmentThreading,
			_dwPartBrowserOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



/////////////////////////////////////////////////////////////////////////////
// CPartBrowserCtrl::XEventSink - native COM interface - peter
STDMETHODIMP_(ULONG) CPartBrowserCtrl::XEventSink::AddRef()
{
	METHOD_PROLOGUE(CPartBrowserCtrl,EventSink)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CPartBrowserCtrl::XEventSink::Release()
{
	METHOD_PROLOGUE(CPartBrowserCtrl,EventSink)
	return pThis->ExternalRelease();
}

STDMETHODIMP CPartBrowserCtrl::XEventSink::QueryInterface(REFIID riid, void** ppv)
{
	METHOD_PROLOGUE(CPartBrowserCtrl,EventSink)
	return pThis->ExternalQueryInterface(&riid, ppv);
}

STDMETHODIMP CPartBrowserCtrl::XEventSink::GlobalEvent(globalevent_enum event)
{
	METHOD_PROLOGUE(CPartBrowserCtrl,EventSink)
	// TODO: Handle global events here - peter
	pThis->OnMgaGlobalEvent(event);
	return S_OK;
}

STDMETHODIMP CPartBrowserCtrl::XEventSink::ObjectEvent(IMgaObject* obj, unsigned long eventmask, VARIANT v)
{
	METHOD_PROLOGUE(CPartBrowserCtrl,EventSink)
	// TODO: Handle object events here - peter
	pThis->m_MgaObjectEventList.Add(obj,eventmask);
	return S_OK;
}


// CPartBrowserCtrl::CPartBrowserCtrl - Constructor

CPartBrowserCtrl::CPartBrowserCtrl()
{
	InitializeIIDs(&IID_DPartBrowser, &IID_DPartBrowserEvents);
	// TODO: Initialize your control's instance data here.
}



// CPartBrowserCtrl::~CPartBrowserCtrl - Destructor

CPartBrowserCtrl::~CPartBrowserCtrl()
{
	// TODO: Cleanup your control's instance data here.
}



// CPartBrowserCtrl::OnDraw - Drawing function

void CPartBrowserCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	if (m_partBrowserDlg.GetSafeHwnd()) {
		m_partBrowserDlg.MoveWindow(rcBounds, TRUE);
	} else {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		CBrush brush(RGB(255, 255, 255));
		pdc->FillRect(rcBounds, &brush);

		CRect edge(rcBounds);
		pdc->DrawEdge(&edge, EDGE_BUMP, BF_RECT);

		CString label("PartBrowser OCX");

		BITMAP bm;
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_PARTBROWSER);
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
		pdc->TextOut(rcBounds.Width() / 2, rcBounds.Height() / 2, CString("PartBrowser ActiveX Control"));
	}
}


// CPartBrowserCtrl::DoPropExchange - Persistence support

void CPartBrowserCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CPartBrowserCtrl::OnResetState - Reset control to default state

void CPartBrowserCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CPartBrowserCtrl::AboutBox - Display an "About" box to the user

void CPartBrowserCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_PARTBROWSER);
	dlgAbout.DoModal();
}



// CPartBrowserCtrl message handlers

int CPartBrowserCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_partBrowserDlg.Create(IDD_PARTBROWSER_DIALOG, this) == FALSE)
		return -1;

	return 0;
}

BOOL CPartBrowserCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTROLPARENT;
	return COleControl::PreCreateWindow(cs);
}

void CPartBrowserCtrl::SetCurrentProject(IUnknown* project)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CComQIPtr<IMgaProject> ccpMgaProject(project);

	if (project != NULL && ccpMgaProject == NULL) {
		ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast MgaProject."));
		return;
	}

	m_partBrowserDlg.SetCurrentProject(ccpMgaProject);
}

void CPartBrowserCtrl::SetMetaModel(IUnknown* meta)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CComQIPtr<IMgaMetaModel> ccpMgaMetaModel(meta);

	if (meta != NULL && ccpMgaMetaModel == NULL) {
		ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast MgaMetaModel."));
		return;
	}

	m_partBrowserDlg.SetMetaModel(ccpMgaMetaModel);
}

void CPartBrowserCtrl::SetBgColor(OLE_COLOR oleBgColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	COLORREF bgColor = TranslateColor(oleBgColor);
	m_partBrowserDlg.SetBgColor(bgColor);
}

void CPartBrowserCtrl::ChangeAspect(LONG index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_partBrowserDlg.ChangeAspect(index);
}

void CPartBrowserCtrl::CycleAspect(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_partBrowserDlg.CycleAspect();
}

void CPartBrowserCtrl::RePaint(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_partBrowserDlg.PostMessage(WM_PAINT);
}

void CPartBrowserCtrl::OnMgaObjectEvent(IMgaObject* obj, unsigned long eventmask)
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

void CPartBrowserCtrl::OnMgaGlobalEvent(globalevent_enum event)
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

void CPartBrowserCtrl::SendAspectChanged(LONG index)
{
	this->FireAspectChanged(index);
}

void CPartBrowserCtrl::PropagateMgaMessages()
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

