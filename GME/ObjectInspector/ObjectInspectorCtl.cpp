// ObjectInspectorCtl.cpp : Implementation of the CObjectInspectorCtrl ActiveX Control class.

#include "stdafx.h"
#include "ObjectInspector.h"
#include "ObjectInspectorCtl.h"
#include "ObjectInspectorPpg.h"
#include "Preference.h"
#include "Mga_i.c"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CObjectInspectorCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CObjectInspectorCtrl, COleControl)
	//{{AFX_MSG_MAP(CObjectInspectorCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CObjectInspectorCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CObjectInspectorCtrl)
	DISP_PROPERTY_EX(CObjectInspectorCtrl, "MgaProject", GetMgaProject, SetMgaProject, VT_UNKNOWN)
	DISP_PROPERTY_EX(CObjectInspectorCtrl, "MgaObject", GetMgaObjects, SetMgaObjects, VT_UNKNOWN)
	DISP_FUNCTION(CObjectInspectorCtrl, "ShowPanel", ShowPanel, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CObjectInspectorCtrl, "EditName", EditName, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CObjectInspectorCtrl, "CyclePanel", dispidCyclePanel, CyclePanel, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CObjectInspectorCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CObjectInspectorCtrl, COleControl)
	//{{AFX_EVENT_MAP(CObjectInspectorCtrl)
	EVENT_CUSTOM("RootFolderNameChanged", FireRootFolderNameChanged, VTS_NONE)
	EVENT_CUSTOM("ConvertPathToCustom", FireConvertPathToCustom, VTS_UNKNOWN)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// Native COM interface map - peter
BEGIN_INTERFACE_MAP(CObjectInspectorCtrl, COleControl)
	INTERFACE_PART(CObjectInspectorCtrl, IID_IMgaEventSink, EventSink)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CObjectInspectorCtrl, 1)
	PROPPAGEID(CObjectInspectorPropPage::guid)
END_PROPPAGEIDS(CObjectInspectorCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CObjectInspectorCtrl, "GME.ObjectInspectorCtrl.1",
	0xb786f53c, 0x297f, 0x4db8, 0x80, 0x60, 0xd4, 0x50, 0x57, 0x21, 0x7a, 0xee)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CObjectInspectorCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DObjectInspector =
		{ 0xa7dc846c, 0xdae3, 0x4b3f, { 0xb9, 0x47, 0x71, 0xd8, 0x5b, 0x5f, 0x64, 0x37 } };
const IID BASED_CODE IID_DObjectInspectorEvents =
		{ 0xb2858a8, 0x321a, 0x43e1, { 0x99, 0x9d, 0x68, 0x6b, 0x8e, 0xfc, 0xa, 0xf2 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwObjectInspectorOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CObjectInspectorCtrl, IDS_OBJECTINSPECTOR, _dwObjectInspectorOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorCtrl::CObjectInspectorCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CObjectInspectorCtrl

BOOL CObjectInspectorCtrl::CObjectInspectorCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_OBJECTINSPECTOR,
			IDB_OBJECTINSPECTOR,
			afxRegApartmentThreading,
			_dwObjectInspectorOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorCtrl::XEventSink - native COM interface - peter
STDMETHODIMP_(ULONG) CObjectInspectorCtrl::XEventSink::AddRef()
{
	METHOD_PROLOGUE(CObjectInspectorCtrl,EventSink)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CObjectInspectorCtrl::XEventSink::Release()
{
	METHOD_PROLOGUE(CObjectInspectorCtrl,EventSink)
	return pThis->ExternalRelease();
}

STDMETHODIMP CObjectInspectorCtrl::XEventSink::QueryInterface(REFIID riid, void** ppv)
{
	METHOD_PROLOGUE(CObjectInspectorCtrl,EventSink)
	return pThis->ExternalQueryInterface(&riid, ppv);
}

STDMETHODIMP CObjectInspectorCtrl::XEventSink::GlobalEvent(globalevent_enum event)
{
	METHOD_PROLOGUE(CObjectInspectorCtrl,EventSink)
	// TODO: Handle global events here - peter
	pThis->OnMgaGlobalEvent(event);
	return S_OK;
}

STDMETHODIMP CObjectInspectorCtrl::XEventSink::ObjectEvent(IMgaObject * obj, unsigned long eventmask, VARIANT v)
{
	METHOD_PROLOGUE(CObjectInspectorCtrl,EventSink)
	// TODO: Handle object events here - peter
	pThis->m_MgaObjectEventList.Add(obj,eventmask);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorCtrl::CObjectInspectorCtrl - Constructor

CObjectInspectorCtrl::CObjectInspectorCtrl()
{
	InitializeIIDs(&IID_DObjectInspector, &IID_DObjectInspectorEvents);

	// TODO: Initialize your control's instance data here.
	SetInitialSize(INSP_INITIAL_SIZEX, INSP_INITIAL_SIZEY);

	m_territory = NULL;
	m_project = NULL;
	m_objects = NULL;

	ignoreNextEvents = false;
}


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorCtrl::~CObjectInspectorCtrl - Destructor

CObjectInspectorCtrl::~CObjectInspectorCtrl()
{
	// TODO: Cleanup your control's instance data here.
	m_territory = NULL;
	m_project = NULL;
	m_objects = NULL;

	m_FCOList.RemoveAll();
	m_FolderList.RemoveAll();
}


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorCtrl::OnDraw - Drawing function

void CObjectInspectorCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (m_inspectorDlg.GetSafeHwnd()) {
		m_inspectorDlg.MoveWindow(rcBounds, TRUE);
	}
	else {
		AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
		CBrush brush (RGB(255,255,255));
		pdc->FillRect (rcBounds, &brush);

		CRect edge(rcBounds);
		pdc->DrawEdge(&edge, EDGE_BUMP, BF_RECT);
		
		CString label(_T("ObjectInspector OCX"));

		BITMAP bm;
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_OBJECTINSPECTOR);
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
		pdc->TextOut(rcBounds.Width()/2, rcBounds.Height()/2, CString(_T("ObjectInspector ActiveX Control")));
	}
}


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorCtrl::DoPropExchange - Persistence support

void CObjectInspectorCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorCtrl::OnResetState - Reset control to default state

void CObjectInspectorCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorCtrl::AboutBox - Display an "About" box to the user

void CObjectInspectorCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_OBJECTINSPECTOR);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CObjectInspectorCtrl message handlers

int CObjectInspectorCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_inspectorDlg.Create(IDD_INSPECTOR_DIALOG, this);

	return 0;
}

BOOL CObjectInspectorCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTROLPARENT;
	return COleControl::PreCreateWindow(cs);
}

void CObjectInspectorCtrl::ShowPanel(long panelID) 
{
	m_inspectorDlg.ShowPanel(panelID);

}

void CObjectInspectorCtrl::EditName() 
{
	m_inspectorDlg.m_NameCtrl.SetFocus();

}

void CObjectInspectorCtrl::CyclePanel(VARIANT_BOOL frwd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_inspectorDlg.CyclePanel( frwd != VARIANT_FALSE);
}

LPUNKNOWN CObjectInspectorCtrl::GetMgaProject() 
{
	if (m_project == NULL) {
		return NULL;
	}
	CComPtr<IUnknown> unk;
	if (FAILED(m_project.QueryInterface(&unk))) {
		ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast MgaProject."));
		return NULL;
	}
	return unk;
}

void CObjectInspectorCtrl::SetMgaProject(LPUNKNOWN newValue) 
{
	if(newValue==NULL)
	{
		m_objects.Release();
		m_FCOList.RemoveAll();
		m_FolderList.RemoveAll();
		
		if(!m_project)
		{
			return;
		}

		RefreshPanels();

	
		if(m_territory.p!=NULL)
		{
			COMTHROW(m_territory->Destroy() );
		}
		m_territory.Release();
		m_project.Release();
	}
	else
	{
		CComQIPtr<IMgaProject> ccpMgaProject(newValue);

		if (ccpMgaProject==NULL) 
		{
			ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast MgaProject."));
			return;
		}

		m_project=ccpMgaProject;
		SetModifiedFlag();

		// Creating Territory
		COMTHROW( m_project->CreateTerritory(&m_xEventSink,&m_territory,NULL) );	

		m_objects.Release();
		m_FCOList.RemoveAll();
		m_FolderList.RemoveAll();
		RefreshPanels();
	}
}


LPUNKNOWN CObjectInspectorCtrl::GetMgaObjects() 
{
	if( !m_objects) {
		return NULL;
	}

	CComPtr<IMgaObjects> obj_coll;
	try
	{
		long len = 0;
		COMTHROW( m_objects->get_Count( &len));

		if( len >= 1)
			COMTHROW( obj_coll.CoCreateInstance( L"Mga.MgaObjects"));

		ASSERT( obj_coll);
		for( long i = 1; i <= len; ++i)
		{
			CComPtr<IMgaObject> obj;
			COMTHROW( m_objects->get_Item( i, &obj));

			ASSERT( obj);
			COMTHROW( obj_coll->Append( obj));
		}
	} catch(...) {
		ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast IMgaObjects collection"));
		return NULL;
	}

	return obj_coll.Detach();
}

void CObjectInspectorCtrl::SetMgaObjects(LPUNKNOWN newValue) 
{
	if( !m_project) {
		m_objects.Release();
		m_FCOList.RemoveAll();
		m_FolderList.RemoveAll();
		return;
	}
	CComQIPtr<IMgaObjects> ccpMgaObjects(newValue);
	if(!ccpMgaObjects)
	{
		ThrowError(CTL_E_INVALIDPROPERTYVALUE, _T("Unable to cast to IMgaObjects."));
		return;
	}

	
	m_FCOList.RemoveAll();
	m_FolderList.RemoveAll();

	try
	{
		COMTHROW(m_project->BeginTransaction(m_territory,TRANSACTION_READ_ONLY));
		// Iterate through the objects
		MGACOLL_ITERATE(IMgaObject, ccpMgaObjects) 
		{
			CComPtr<IMgaObject>ccpMgaObject;
			COMTHROW(m_territory->OpenObj(MGACOLL_ITER,&ccpMgaObject));
			objtype_enum oeType;
			 
			COMTHROW(ccpMgaObject->get_ObjType(&oeType));
			if(oeType==OBJTYPE_FOLDER)
			{
				CComQIPtr<IMgaFolder> ccpMgaFolder(ccpMgaObject);
				if(ccpMgaFolder)
				{
					CComPtr<IMgaFolder> ccpMgaFolderElement=ccpMgaFolder;
					m_FolderList.AddTail(ADAPT_FOLDER(ccpMgaFolderElement));
				}
				else
				{
					ASSERT(0);
				}

			}
			else
			{
				CComQIPtr<IMgaFCO> ccpMgaFCO(ccpMgaObject);
				if(ccpMgaFCO)
				{
					CComPtr<IMgaFCO> ccpMgaFCOElement=ccpMgaFCO;
					m_FCOList.AddTail(ADAPT_FCO(ccpMgaFCOElement));
				}
				else
				{
					ASSERT(0);
				}
			}
			
		} MGACOLL_ITERATE_END;	

		COMTHROW(m_project->CommitTransaction());

		RefreshPanels();
		m_objects = ccpMgaObjects;
		
	}
	catch (hresult_exception e)
	{
		m_project->AbortTransaction();
		m_FCOList.RemoveAll();
		m_FolderList.RemoveAll();
		m_objects=NULL;

		CWnd::MessageBox(_T("Object Inspector could not hand over the project data due to an unexpected error. We apologize for the inconvenience."),_T("Object inspector"),MB_ICONERROR);

	}
	SetModifiedFlag();
}

void CObjectInspectorCtrl::RefreshPanels()
{
	try
	{
		long lProjectStatus;
		COMTHROW(m_project->get_ProjectStatus(&lProjectStatus));
		bool bInTransaction = ((lProjectStatus & 0x08) != 0);

		if(!bInTransaction)
		{
			COMTHROW(m_project->BeginTransaction(m_territory,TRANSACTION_READ_ONLY));
		}	
			// Refresh Attribute Panel
			RefreshAttributePanel();	

			// Refresh Reference Panel
			RefreshReferencePanel();

			// Refresh Properties Panel
			RefreshPropertyPanel();

			// Refresh name field
			RefreshName();

			CString strEmpty;
			m_inspectorDlg.SetHelp(strEmpty,strEmpty);
		if(!bInTransaction)
		{
			COMTHROW(m_project->CommitTransaction());
		}

		
	}
	catch (hresult_exception e)
	{
		ASSERT(0);
		m_project->AbortTransaction();
		CWnd::MessageBox(_T("Object Inspector could not load object data due to an unexpected MGA error. We apologize for the inconvenience."),_T("Object inspector"),MB_ICONERROR);
	}

	m_inspectorDlg.Refresh();
}

bool CObjectInspectorCtrl::IsInitialized(bool withObject) const
{
	return ( (m_project != NULL) && ( (!withObject) || (m_objects != NULL) ) );
}

void CObjectInspectorCtrl::IgnoreNextEvents(void)
{
	ignoreNextEvents = true;
}

void CObjectInspectorCtrl::RefreshReferencePanel()
{

	INT_PTR nCount=m_FCOList.GetCount();

	if(!m_FolderList.IsEmpty())
	{		
		CMgaFCOPtrList EmptyFCOList;
		
		m_inspectorDlg.ShowPanel(INSP_PREF_PANEL,false);	
		m_Preference.CreateList(EmptyFCOList);
	}
	else
	{	
		if(nCount==0)
		{
			m_inspectorDlg.ShowPanel(INSP_PREF_PANEL,false);
		}
		else
		{
			m_inspectorDlg.ShowPanel(INSP_PREF_PANEL,true);
		}

		m_Preference.CreateList(m_FCOList);
	}
	
	m_inspectorDlg.m_inspectorLists[INSP_PREF_PANEL]->
									UpdateItems(m_Preference.m_ListItemArray);
	
}


void CObjectInspectorCtrl::RefreshAttributePanel()
{
	INT_PTR nCount = m_FCOList.GetCount();

	CArray<CListItem,CListItem&> ListItemArray;
	
	if(!m_FolderList.IsEmpty())
	{	
		CComPtr<IMgaFolder> ccpMgaFolder = m_FolderList.GetHead();
		objtype_enum oeType;
		CComPtr<IMgaObject> ccpParent;
		COMTHROW(ccpMgaFolder->GetParent(&ccpParent,&oeType));

		if ((nCount == 0) && (m_FolderList.GetCount() == 1) && (ccpParent == NULL)) {
			// Root Folder - Show project attributes
			m_inspectorDlg.ShowPanel(INSP_ATTR_PANEL,true);
			m_Attribute.CreateList(m_project, ListItemArray);
		}
		else {
			CMgaFCOPtrList EmptyFCOList;
			EmptyFCOList.RemoveAll();
			
			m_inspectorDlg.ShowPanel(INSP_ATTR_PANEL,false);	
			m_Attribute.CreateList(EmptyFCOList,ListItemArray);
		}
	}
	else
	{
		if(nCount==0)
		{

			m_inspectorDlg.ShowPanel(INSP_ATTR_PANEL,false);	
		}	
		else
		{
			m_inspectorDlg.ShowPanel(INSP_ATTR_PANEL,true);
		}
		m_Attribute.CreateList(m_FCOList,ListItemArray);
	}

	m_inspectorDlg.m_inspectorLists[INSP_ATTR_PANEL]->UpdateItems(ListItemArray);
}



void CObjectInspectorCtrl::RefreshPropertyPanel()
{
	INT_PTR nCount = m_FCOList.GetCount() + m_FolderList.GetCount();

	
	if(nCount==0)
	{
		m_inspectorDlg.ShowPanel(INSP_PROP_PANEL,false);		
	}	
	else
	{
		m_inspectorDlg.ShowPanel(INSP_PROP_PANEL,true);
	}

	CArray<CListItem,CListItem&> ListItemArray;
	m_Property.CreateList(m_FCOList,m_FolderList,ListItemArray);

	m_inspectorDlg.m_inspectorLists[INSP_PROP_PANEL]->UpdateItems(ListItemArray);

}


void CObjectInspectorCtrl::OnMgaObjectEvent(IMgaObject  *obj, unsigned long eventmask)
{
	if (ignoreNextEvents) {
		ignoreNextEvents = false;
		return;
	}

	CComPtr<IMgaObject> ccpMgaObject(obj);
	// Handling Object Events
	EVENT_TRACE("\n__________ Object Event Begin _________\n");

	if(eventmask&OBJEVENT_DESTROYED)
	{
		EVENT_TRACE("OBJEVENT_DESTROYED\n");
		
		// Remove object from the list

		POSITION posFCO=m_FCOList.GetHeadPosition();
		POSITION posPrevFCO=posFCO;
		while(posFCO)
		{
			posPrevFCO=posFCO;
			CComPtr<IMgaFCO> ccpMgaFCO=m_FCOList.GetNext(posFCO);
			if(ccpMgaFCO==ccpMgaObject)
			{
				m_FCOList.RemoveAt(posPrevFCO);
				return;
			}
		}
		
		POSITION posFolder=m_FolderList.GetHeadPosition();
		POSITION posPrevFolder=posFolder;
		while(posFolder)
		{
			posPrevFolder=posFolder;
			CComPtr<IMgaFolder> ccpMgaFolder=m_FolderList.GetNext(posFolder);
			if(ccpMgaFolder==ccpMgaObject)
			{
				m_FolderList.RemoveAt(posPrevFolder);
				return;
			}
		}



	}

#ifdef _DEBUG
	if(eventmask&OBJEVENT_REGISTRY)
	{
		EVENT_TRACE("OBJEVENT_REGISTRY\n");
	}

	if(eventmask&OBJEVENT_PROPERTIES)
	{
		EVENT_TRACE("OBJEVENT_PROPERTIES\n");
	}

	if(eventmask&OBJEVENT_CREATED)
	{
		EVENT_TRACE("OBJEVENT_CREATED\n");

	}
	if(eventmask&OBJEVENT_ATTR)
	{
		EVENT_TRACE("OBJEVENT_ATTR\n");

	}
		if(eventmask&OBJEVENT_NEWCHILD)
	{
		EVENT_TRACE("OBJEVENT_NEWCHILD\n");
	}
	if(eventmask&OBJEVENT_RELATION)
	{
		EVENT_TRACE("OBJEVENT_RELATION\n");
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

void CObjectInspectorCtrl::OnMgaGlobalEvent(globalevent_enum event)
{
	if (ignoreNextEvents)
		return;

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
		return;
	}

	else if(event==GLOBALEVENT_ABORT_TRANSACTION)
	{
		EVENT_TRACE("GLOBALEVENT_ABORT_TRANSACTION\n");

		// Removing the elements
		m_MgaObjectEventList.RemoveAll();
	}
	else if(event==GLOBALEVENT_UNDO)
	{
		EVENT_TRACE("GLOBALEVENT_UNDO\n");
		RemoveZombies();
		// Refresh
	}
	else if(event==GLOBALEVENT_REDO)
	{		
		EVENT_TRACE("GLOBALEVENT_REDO\n");
		RemoveZombies();
		// Refresh
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

#endif /* _DEBUG */

	EVENT_TRACE("_________ Global Event End _________\n");


	RefreshPanels();
}

void CObjectInspectorCtrl::RemoveZombies()
{
	POSITION posFCO=m_FCOList.GetHeadPosition();
	POSITION posPrevFCO=posFCO;
	while(posFCO)
	{
		posPrevFCO=posFCO;
		CComPtr<IMgaFCO> ccpMgaFCO=m_FCOList.GetNext(posFCO);
		long oStatus;
		COMTHROW(ccpMgaFCO->get_Status(&oStatus));
		if(oStatus != OBJECT_EXISTS)
		{
			m_FCOList.RemoveAt(posPrevFCO);
		}
	}
	
	POSITION posFolder=m_FolderList.GetHeadPosition();
	POSITION posPrevFolder=posFolder;
	while(posFolder)
	{
		posPrevFolder=posFolder;
		CComPtr<IMgaFolder> ccpMgaFolder=m_FolderList.GetNext(posFolder);
		long fStatus;
		COMTHROW(ccpMgaFolder->get_Status(&fStatus));
		if(fStatus != OBJECT_EXISTS)
		{
			m_FolderList.RemoveAt(posPrevFolder);
		}
	}
}

template<typename F>
void CObjectInspectorCtrl::WriteToMga(F f)
{
	CComPtr<IMgaMetaProject> ccpMetaProject;
	try
	{
		long lProjectStatus;
		COMTHROW(m_project->get_ProjectStatus(&lProjectStatus));
		bool bInTransaction = ((lProjectStatus & 0x08) != 0);

		ASSERT(!bInTransaction);

		if (!bInTransaction)
		{
			COMTHROW(m_project->BeginTransaction(m_territory, TRANSACTION_GENERAL));
		}

		f();

		if (!bInTransaction)
		{
			HRESULT hResult = m_project->CommitTransaction();
			if (FAILED(hResult)) {
				if (hResult == E_MGA_CONSTRAINT_VIOLATION)
					COMTHROW(m_project->AbortTransaction());
				else
					COMTHROW(hResult);
			}
		}
	}
	catch (hresult_exception e)
	{
		ASSERT(0);
		m_project->AbortTransaction();
		if (e.hr == E_MGA_LIBOBJECT)
			CWnd::MessageBox(_T("Library objects cannot be modified."), _T("GME"), MB_ICONERROR);
		else
			CWnd::MessageBox(_T("Object Inspector could not write attribute data due to an unexpected error. We apologize for the inconvenience."), _T("Object inspector"), MB_ICONERROR);
	}
	catch (_com_error& e)
	{
		ASSERT(0);
		m_project->AbortTransaction();
		CWnd::MessageBox(CString(L"Object Inspector could not write attribute data: ") + static_cast<const wchar_t*>(e.Description()), L"Object inspector", MB_ICONERROR);
	}

}


void CObjectInspectorCtrl::DetachFromArchetype(CListItem ListItem)
{
	auto write = [&]() {
		if (m_FCOList.IsEmpty()) {
		}
		else {
			MgaFCOPtr ptr = m_FCOList.GetHead();
			ptr->DetachFromArcheType();
		}
	};
	WriteToMga(write);
}

void CObjectInspectorCtrl::WriteAttributeItemToMga(CListItem ListItem)
{
	auto write = [&]() {
		if (m_FCOList.IsEmpty()) {
			m_Attribute.WriteItemToMga(ListItem, m_project);
		}
		else {
			m_Attribute.WriteItemToMga(ListItem, m_FCOList);
		}
	};
	WriteToMga(write);
}

void CObjectInspectorCtrl::OpenRefered()
{
	IMgaFCOPtr referred;
	if (m_FCOList.IsEmpty()) {
	}
	else {
		MgaFCOPtr ptr = m_FCOList.GetHead();
		CComQIPtr<IMgaReference> ref = ptr;
		if (ref) {
			WriteToMga([&]() {
				referred = ref->Referred;
			});
		}
	}
	if (referred) {
		auto gme = get_GME(referred->Project);
		if (gme) {
			gme->ShowFCO(referred, VARIANT_FALSE);
		}
	}
}

void CObjectInspectorCtrl::WritePreferenceItemToMga(CListItem ListItem, bool bIsForKind)
{
	CComPtr<IMgaMetaProject> ccpMetaProject;
	try
	{
		m_Preference.IssuePossiblePathConversion(ListItem, m_FCOList, this);

		long lProjectStatus;
		COMTHROW(m_project->get_ProjectStatus(&lProjectStatus));
		bool bInTransaction = ((lProjectStatus & 0x08) != 0);

		ASSERT(!bInTransaction);

		if(!bInTransaction)
		{
			COMTHROW(m_project->BeginTransaction(m_territory,TRANSACTION_GENERAL));
		}	

		// Here we will modify meta, so need the transaction for the meta
		if(bIsForKind) 
		{				
				COMTHROW(m_project->get_RootMeta(&ccpMetaProject));					
				COMTHROW(ccpMetaProject->BeginTransaction());
		}
		// Writing change to MGA
		m_Preference.WriteItemToMga(ListItem,m_FCOList,bIsForKind);

		if(bIsForKind)
		{
			// Close meta transaction
			COMTHROW(ccpMetaProject->CommitTransaction());
		}

		if(!bInTransaction)
		{
			COMTHROW(m_project->CommitTransaction());
		}

		
	}
	catch (hresult_exception& e)
	{
		ASSERT(0);
		m_project->AbortTransaction();
		if(bIsForKind)
		{
			ccpMetaProject->AbortTransaction();
		}
		if (e.hr == E_MGA_LIBOBJECT)
			CWnd::MessageBox(_T("Library objects cannot be modified."), _T("GME"), MB_ICONERROR);
		else if (e.hr != E_MGA_CONSTRAINT_VIOLATION)
			CWnd::MessageBox(_T("GME could not write object preference data due to an unexpected MGA error. We apologize for the inconvenience."), _T("GME"), MB_ICONERROR);
	}
	catch (_com_error& e)
	{
		m_project->AbortTransaction();
		if(bIsForKind)
		{
			ccpMetaProject->AbortTransaction();
		}
		if (e.Error() != E_MGA_CONSTRAINT_VIOLATION)
			CWnd::MessageBox(CString(L"GME could not write attribute data: ") + static_cast<const wchar_t*>(e.Description()), L"GME",MB_ICONERROR);
	}
	
}


void CObjectInspectorCtrl::RefreshName()
{

	CString strName;
	bool bIsFirst=true;


	POSITION posFCO=m_FCOList.GetHeadPosition();

	while(posFCO)
	{
		CComPtr<IMgaFCO> ccpMgaFCO=m_FCOList.GetNext(posFCO);
		// Getting name
		CComBSTR bstrName;
		COMTHROW(ccpMgaFCO->get_Name(&bstrName));

		if(bIsFirst)
		{
			strName=CString(bstrName);
			bIsFirst=false;
		}
		else
		{
			strName+=_T(";")+CString(bstrName);
		}

	}

	POSITION posFolder=m_FolderList.GetHeadPosition();

	while(posFolder)
	{
		CComPtr<IMgaFolder> ccpMgaFolder=m_FolderList.GetNext(posFolder);

		// Getting name
		CComBSTR bstrName;

		COMTHROW(ccpMgaFolder->get_Name(&bstrName));

		if(bIsFirst)
		{
			strName=CString(bstrName);
			bIsFirst=false;
		}
		else
		{
			strName+=_T(";")+CString(bstrName);
		}

	}

	INT_PTR nCount = m_FolderList.GetCount() + m_FCOList.GetCount();
	m_inspectorDlg.SetName(strName,(nCount>1),!(nCount<1));
	
}


void CObjectInspectorCtrl::WriteNameToMga(const CString &strName)
{
	bool rootfolder_name_edited = false;
	CComPtr<IMgaMetaProject> ccpMetaProject;
	try
	{
		long lProjectStatus;
		COMTHROW(m_project->get_ProjectStatus(&lProjectStatus));
		bool bInTransaction = ((lProjectStatus & 0x08) != 0);


		if(bInTransaction)
		{
			return;	
		}

		COMTHROW(m_project->BeginTransaction(m_territory,TRANSACTION_GENERAL));

		// Writing change to MGA
		POSITION posFCO=m_FCOList.GetHeadPosition();

		while(posFCO)
		{
			CComPtr<IMgaFCO> ccpMgaFCO=m_FCOList.GetNext(posFCO);
			// Setting name
			CComBSTR bstrName=strName;
			COMTHROW(ccpMgaFCO->put_Name(bstrName));


		}

		POSITION posFolder=m_FolderList.GetHeadPosition();

		while(posFolder)
		{
			CComPtr<IMgaFolder> ccpMgaFolder=m_FolderList.GetNext(posFolder);
		
			// Setting name
			CComBSTR bstrName=strName;
			
			// The name is special if folder is a library root
			CComBSTR dummyName;
			COMTHROW(ccpMgaFolder->get_LibraryName(&dummyName));
			if (dummyName != NULL) {
				COMTHROW(ccpMgaFolder->put_LibraryName(bstrName));
			}
			else {
				COMTHROW(ccpMgaFolder->put_Name(bstrName));
			}

			// Checking if it is root folder
			// If it is, change project name as well
			objtype_enum oeType;
			CComPtr<IMgaObject> ccpParent;

			COMTHROW(ccpMgaFolder->GetParent(&ccpParent,&oeType));
			
			if(ccpParent==NULL)	// Root Folder
			{
				COMTHROW(m_project->put_Name(bstrName));
				rootfolder_name_edited = true;
			}



		}

		COMTHROW(m_project->CommitTransaction());

		if( rootfolder_name_edited)
		{
			this->FireRootFolderNameChanged();
		}
	}
	catch (hresult_exception& e)
	{
		m_project->AbortTransaction();
		if (e.hr != E_MGA_CONSTRAINT_VIOLATION)
			CWnd::MessageBox(_T("Object Inspector could not write object name due to an unexpected MGA error. We apologize for the inconvenience."),_T("Object inspector"),MB_ICONERROR);
	}

}


void CObjectInspectorCtrl::PropagateMgaMessages()
{
	// Iterating through the list
	while(!m_MgaObjectEventList.IsEmpty())
	{
		CMgaObjectEventWrapper* pEventWrapper=
			(CMgaObjectEventWrapper*)m_MgaObjectEventList.RemoveHead();
		// Propagating message to AggregatePropertyPage
		
		OnMgaObjectEvent(pEventWrapper->m_ccpMgaObject,pEventWrapper->m_eventmask);

		delete pEventWrapper;
	}
}

void CObjectInspectorCtrl::UndoRedo( bool undo /*= true*/)
{
	short usz( 0), rsz( 0);
	if( m_project)
		COMTHROW(m_project->UndoRedoSize( &usz, &rsz));
	if( undo && usz > 0)
		COMTHROW( m_project->Undo());
	else if( !undo && rsz > 0)
		COMTHROW( m_project->Redo());
}

