// SearchCtl.cpp : Implementation of the CSearchCtrl ActiveX Control class.

#include "stdafx.h"
#include "Search.h"
#include "SearchCtl.h"
#include "SearchPpg.h"
#include "Mga_i.c"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSearchCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CSearchCtrl, COleControl)
	//{{AFX_MSG_MAP(CSearchCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CSearchCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CSearchCtrl)
	DISP_PROPERTY_EX(CSearchCtrl, "MgaProject", GetMgaProject, SetMgaProject, VT_UNKNOWN)
	DISP_FUNCTION(CSearchCtrl, "OpenProject", OpenProject, VT_EMPTY, VTS_BSTR)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CSearchCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CSearchCtrl, "SelMgaObjects", dispidSelMgaObjects, SelMgaObjects, VT_EMPTY, VTS_UNKNOWN)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CSearchCtrl, COleControl)
	//{{AFX_EVENT_MAP(CSearchCtrl)
	EVENT_CUSTOM("ClickMgaObject", FireClickMgaObject, VTS_UNKNOWN)
	EVENT_CUSTOM("DblClickMgaObject", FireDblClickMgaObject, VTS_UNKNOWN)
	EVENT_CUSTOM("WantToBeClosed", FireWantToBeClosed, VTS_NONE)
	//}}AFX_EVENT_MAP
	EVENT_CUSTOM_ID("LocateMgaObject", eventidLocateMgaObject, FireLocateMgaObject, VTS_BSTR)
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// Native COM interface map - peter
BEGIN_INTERFACE_MAP(CSearchCtrl, COleControl)
	INTERFACE_PART(CSearchCtrl, IID_IMgaEventSink, EventSink)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CSearchCtrl, 1)
	PROPPAGEID(CSearchPropPage::guid)
END_PROPPAGEIDS(CSearchCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CSearchCtrl, "GME.SearchCtrl.1",
	0xe5fe98f1, 0xb7ad, 0x4abe, 0x89, 0xdb, 0x87, 0xdd, 0xd9, 0xdb, 0x75, 0x3e)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CSearchCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DSearch =
		{ 0xb718ccf7, 0x5055, 0x40dd, { 0x9c, 0xbd, 0xab, 0x17, 0xf7, 0x2a, 0xb1, 0x13 } };
const IID BASED_CODE IID_DSearchEvents =
		{ 0xb240c566, 0xa493, 0x4529, { 0x8a, 0xd7, 0xb9, 0xb7, 0xb, 0x17, 0xbb, 0xa1 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwSearchOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CSearchCtrl, IDS_SEARCH, _dwSearchOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CSearchCtrl::CSearchCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CSearchCtrl

BOOL CSearchCtrl::CSearchCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_SEARCH,
			IDB_SEARCH,
			afxRegApartmentThreading,
			_dwSearchOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CSearchCtrl::XEventSink - native COM interface - peter
STDMETHODIMP_(ULONG) CSearchCtrl::XEventSink::AddRef()
{
	METHOD_PROLOGUE(CSearchCtrl,EventSink)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CSearchCtrl::XEventSink::Release()
{
	METHOD_PROLOGUE(CSearchCtrl,EventSink)
	return pThis->ExternalRelease();
}

STDMETHODIMP CSearchCtrl::XEventSink::QueryInterface(REFIID riid, void** ppv)
{
	METHOD_PROLOGUE(CSearchCtrl,EventSink)
	return pThis->ExternalQueryInterface(&riid, ppv);
}

STDMETHODIMP CSearchCtrl::XEventSink::GlobalEvent(globalevent_enum event)
{
	METHOD_PROLOGUE(CSearchCtrl,EventSink)
	pThis->OnMgaGlobalEvent(event);
	return S_OK;
}

STDMETHODIMP CSearchCtrl::XEventSink::ObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v)
{
	METHOD_PROLOGUE(CSearchCtrl,EventSink)
	pThis->OnMgaObjectEvent(obj, eventmask);
	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CSearchCtrl::CSearchCtrl - Constructor

CSearchCtrl::CSearchCtrl()
{
	InitializeIIDs(&IID_DSearch, &IID_DSearchEvents);

	SetInitialSize(SEARCH_INITIAL_SIZEX, SEARCH_INITIAL_SIZEY);
	m_territory = NULL;
	m_project = NULL;
    m_MgaObjs=NULL;
	m_transactionCnt = 0;
	m_inEventTransactionMode = false;
}


/////////////////////////////////////////////////////////////////////////////
// CSearchCtrl::~CSearchCtrl - Destructor

CSearchCtrl::~CSearchCtrl()
{
    m_territory = NULL;
	m_project = NULL;
    m_MgaObjs=NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CSearchCtrl::OnDraw - Drawing function

void CSearchCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (m_searchDlg.GetSafeHwnd()) {
		m_searchDlg.MoveWindow(rcBounds, TRUE);
	}
	else {
		AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
		CBrush brush (RGB(255,255,255));
		pdc->FillRect (rcBounds, &brush);

		CRect edge(rcBounds);
		pdc->DrawEdge(&edge, EDGE_BUMP, BF_RECT);
		
		CString label("GME Search OCX");

		BITMAP bm;
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_SEARCH);
		bitmap.GetBitmap(&bm);
		CSize size(bm.bmWidth, bm.bmHeight);
		pdc->DPtoLP(&size);
		CPoint org(0,0);
		pdc->DPtoLP(&org);
		CPoint pos(rcBounds.Width()/2, rcBounds.Height()/2 - (pdc->GetTextExtent(label).cy) );
		CDC dcMem;
		dcMem.CreateCompatibleDC(pdc);
		CBitmap *oldbitmap = dcMem.SelectObject(&bitmap);
		dcMem.SetMapMode (pdc->GetMapMode());
		pdc->BitBlt(pos.x, pos.y, size.cx, size.cy, &dcMem, org.x, org.y, SRCCOPY);
		dcMem.SelectObject(oldbitmap);
		
		pdc->SetTextAlign(TA_CENTER);
		pdc->TextOut(rcBounds.Width()/2, rcBounds.Height()/2, CString("GME Search ActiveX Control"));
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSearchCtrl::DoPropExchange - Persistence support

void CSearchCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CSearchCtrl::OnResetState - Reset control to default state

void CSearchCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CSearchCtrl::AboutBox - Display an "About" box to the user

void CSearchCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_SEARCH);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CSearchCtrl message handlers

int CSearchCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_searchDlg.Create(IDD_SEARCH_DIALOG, this);
	
	return 0;
}

BOOL CSearchCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTROLPARENT;
	return COleControl::PreCreateWindow(cs);
}


LPUNKNOWN CSearchCtrl::GetMgaProject() 
{
	CComPtr<IUnknown> unk;
	if (m_project != NULL) {
		if (FAILED(m_project.QueryInterface(&unk))) {
			ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast MgaProject."));
			return NULL;
		}
	}
	return unk;
}

void CSearchCtrl::SetMgaProject(LPUNKNOWN newValue) 
{
	if(m_project != NULL)
	{
		m_searchDlg.RemoveAll();
		if(m_territory != NULL)
		{
			COMTHROW(m_territory->Destroy() );
		}
	}

	if (newValue != NULL) {
		CComQIPtr<IMgaProject> ccpMgaProject(newValue);
		if (ccpMgaProject == NULL) 
		{
			ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast MgaProject."));
			return;
		}

		m_project = ccpMgaProject;

		// Creating Territory
		m_territory = NULL;
		COMTHROW( m_project->CreateTerritory(&m_xEventSink,&m_territory,NULL) );
		m_searchDlg.EnableSearch();
		m_searchDlg.GetKinds(m_project);

	}
	else {
		m_searchDlg.DisableSearch();
		m_project = NULL;
		m_territory = NULL;
        m_MgaObjs.Release();
	}
}

void CSearchCtrl::OpenProject(LPCTSTR connStr) 
{
	CComPtr<IMgaProject> ccpMgaProject;
	try
	{
		COMTHROW(ccpMgaProject.CoCreateInstance(OLESTR("Mga.MgaProject")));
	
		VARIANT_BOOL vtbMode;	
		CComBSTR bszProjectConnStr(connStr);
	
		COMTHROW(ccpMgaProject->Open(bszProjectConnStr, &vtbMode));
		SetMgaProject(ccpMgaProject);

	}
	catch (hresult_exception &)
	{
		ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Error opening project."));
		return;
	}
}


/////////////////////////////////////////////////////////////////////////////
// MGA Event handlers

void CSearchCtrl::OnMgaObjectEvent(IMgaObject  *obj, unsigned long eventmask)
{
	m_inEventTransactionMode = true;
	// We do not process individual object events
	m_inEventTransactionMode = false;
	return;
}

void CSearchCtrl::OnMgaGlobalEvent(globalevent_enum event)
{

	m_inEventTransactionMode = true;
	
	// Handling Global Events
	if(event==GLOBALEVENT_COMMIT_TRANSACTION)
	{
		m_searchDlg.RemoveZombies();
		RemoveZombies<IMgaObject, IMgaObjects>(m_MgaObjs);
	}
	else if(event==GLOBALEVENT_NOTIFICATION_READY)
	{
		m_searchDlg.RemoveZombies();
		RemoveZombies<IMgaObject, IMgaObjects>(m_MgaObjs);
	}

	else if(event==GLOBALEVENT_ABORT_TRANSACTION)
	{
		m_searchDlg.RemoveAll();
		RemoveZombies<IMgaObject, IMgaObjects>(m_MgaObjs);
	}
	else if(event==GLOBALEVENT_UNDO)
	{
		m_searchDlg.RemoveZombies();
		RemoveZombies<IMgaObject, IMgaObjects>(m_MgaObjs);
	}
	else if(event==GLOBALEVENT_REDO)
	{		
		m_searchDlg.RemoveZombies();
		RemoveZombies<IMgaObject, IMgaObjects>(m_MgaObjs);
	}
	m_inEventTransactionMode = false;
}

/////////////////////////////////////////////////////////////////////////////
// These functions can be used from the dialog

void CSearchCtrl::BeginTransaction(transactiontype_enum type)
{
	// In the event handlers we are already in transaction
	if(m_inEventTransactionMode) {
		return;
	}
	
	if(m_transactionCnt == 0) // Not in transactions
	{		
		COMTHROW(
				m_project->BeginTransaction(
					m_territory, 	
					type
				)
			);
	}
	m_transactionCnt++;		
}

void CSearchCtrl::CommitTransaction()
{
	// In the event handlers we are already in transaction
	if(m_inEventTransactionMode) {
		return;
	}

	if(m_transactionCnt == 0) // We are not in transaction. What to commit?
	{
		return;
	}
	
	if(m_transactionCnt == 1)
	{				
		COMTHROW(m_project->CommitTransaction());				
	}
	m_transactionCnt--;
}

void CSearchCtrl::AbortTransaction()
{
	if(m_transactionCnt == 0) {
		return;
	}

	COMTHROW(m_project->AbortTransaction());
	m_transactionCnt = 0;
	m_inEventTransactionMode = false;

	return;
}

CComPtr<IMgaTerritory> CSearchCtrl::Terr()
{
	return m_territory;
}

// needs to be called in a transaction
CComPtr<IMgaObjects> CSearchCtrl::PutInMyTerritory(CComPtr<IMgaObjects> &p_inColl)
{
	CComPtr<IMgaObjects> coll_in_active_terr;
	coll_in_active_terr.CoCreateInstance( L"Mga.MgaObjects");

	long c = 0;
	if( p_inColl)
		COMTHROW(p_inColl->get_Count( &c));

	// nothing to copy? no territory? failed to create out coll? or no transaction active?
	if( c > 0 && m_territory && coll_in_active_terr && m_transactionCnt > 0) 
	{
		for( long i = 1; i <= c; ++i)                 // for all items in coll
		{
			CComPtr<IMgaObject> obj, item;
			COMTHROW(p_inColl->get_Item( i, &item));

			COMTHROW(m_territory->OpenObj( item, &obj));        // puts in our territory
			COMTHROW(coll_in_active_terr->Append( obj));
		}
	}

	return coll_in_active_terr;
}

void CSearchCtrl::ClickOnObject(CComPtr<IMgaObject> object)
{
	CComPtr<IUnknown> unk;
	COMTHROW(object.QueryInterface(&unk));
	FireClickMgaObject(unk);
}

void CSearchCtrl::ClickOnObject(CComPtr<IMgaObjects> object)
{
	CComPtr<IUnknown> unk;
	COMTHROW(object.QueryInterface(&unk));
	FireClickMgaObject(unk);
}


void CSearchCtrl::DblClickOnObject(CComPtr<IMgaObject> object)
{
	CComPtr<IUnknown> unk;
	COMTHROW(object.QueryInterface(&unk));
	FireDblClickMgaObject(unk);
}

void CSearchCtrl::WantToBeClosed()
{
	FireWantToBeClosed();
}

void CSearchCtrl::LocateMgaObject(CComPtr<IMgaObject> object)
{
	try
	{
		// obtain object id
		BeginTransaction();
		CBstr bstr_id;
		COMTHROW( object->get_ID( bstr_id));
		CommitTransaction();

		// fire event
		FireLocateMgaObject( (LPCTSTR) (CString) bstr_id);

	} catch( hresult_exception& ) {
		AbortTransaction();
	}
}

void CSearchCtrl::SelMgaObjects(IUnknown* p_selMgaObjs)
{
    //if project is undefined no point in setting objects -kiran
    if(!m_project)
    {
        m_MgaObjs.Release();
        return;
    }
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (p_selMgaObjs != NULL) {
		CComQIPtr<IMgaObjects> ccpMgaObjects( p_selMgaObjs);
		if (ccpMgaObjects == NULL) 
		{
			ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast MgaObjects."));
			//m_searchDlg.EnableScoped( FALSE);
			return;
		}

		m_MgaObjs = ccpMgaObjects;
		
	}
    else { // called when Dlg is Hidden, in order to release the Mga ptrs
		m_MgaObjs.Release();
	}
}

//added to make enter key work in dialog
//the message from the parent CGMESearch is transmitted to OLE control
//which is this, after this message is obtained its forwarded to dialog
//so that it is properly taken care of

BOOL CSearchCtrl::PreTranslateMessage(MSG *pMsg)
{
    if( pMsg->message == WM_KEYDOWN )
	{
		switch(pMsg->wParam)
		{
			case VK_RETURN:
			case VK_TAB:
                return this->m_searchDlg.PreTranslateMessage(pMsg);
			
		}
	}
    return FALSE;
}