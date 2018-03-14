// GMEDoc.cpp : implementation of the CGMEDoc class
//

#include "stdafx.h"

#include "GMEApp.h"
#include "GMEstd.h"

#include "Mainfrm.h"
#include "GMEDoc.h"
#include "GMEView.h"
#include "GuiObject.h"
#include "GuiMeta.h"
#include "GMEEventLogger.h"
#include "Parser.h"
#include "ChildFrm.h"
#include "GMEChildFrame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGMEDoc

CGMEDoc* CGMEDoc::theInstance = 0;

IMPLEMENT_DYNCREATE(CGMEDoc, CDocument)

BEGIN_MESSAGE_MAP(CGMEDoc, CDocument)
	//{{AFX_MSG_MAP(CGMEDoc)
	ON_COMMAND(ID_MODE_AUTOCONNECT, OnModeAutoconnect)
	ON_UPDATE_COMMAND_UI(ID_MODE_AUTOCONNECT, OnUpdateModeAutoconnect)
	ON_COMMAND(ID_MODE_DISCONNECT, OnModeDisconnect)
	ON_UPDATE_COMMAND_UI(ID_MODE_DISCONNECT, OnUpdateModeDisconnect)
	ON_COMMAND(ID_MODE_EDIT, OnModeEdit)
	ON_UPDATE_COMMAND_UI(ID_MODE_EDIT, OnUpdateModeEdit)
	ON_COMMAND(ID_MODE_SET, OnModeSet)
	ON_UPDATE_COMMAND_UI(ID_MODE_SET, OnUpdateModeSet)
	ON_COMMAND(ID_MODE_VISUALIZE, OnModeVisualize)
	ON_UPDATE_COMMAND_UI(ID_MODE_VISUALIZE, OnUpdateModeVisualize)
	ON_COMMAND(ID_MODE_ZOOM, OnModeZoom)
	ON_UPDATE_COMMAND_UI(ID_MODE_ZOOM, OnUpdateModeZoom)
	ON_COMMAND(ID_MODE_SHORTCONNECT, OnModeShortConnect)
	ON_UPDATE_COMMAND_UI(ID_MODE_SHORTCONNECT, OnUpdateModeShortConnect)
	ON_COMMAND(ID_MODE_SHORTDISCONNECT, OnModeShortDisconnect)
	ON_UPDATE_COMMAND_UI(ID_MODE_SHORTDISCONNECT, OnUpdateModeShortDisconnect)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_COMMAND(ID_BUTTON33020, OnBtnBack)
	ON_COMMAND(ID_BUTTON33021, OnBtnFrwd)
	ON_COMMAND(ID_BUTTON33022, OnBtnHome)
	ON_UPDATE_COMMAND_UI(ID_BUTTON33020, OnUpdateBtnBack)
	ON_UPDATE_COMMAND_UI(ID_BUTTON33021, OnUpdateBtnFrwd)
	ON_UPDATE_COMMAND_UI(ID_BUTTON33022, OnUpdateBtnHome)
	ON_COMMAND(ID_BUTTON33027, OnBtnCloseActive)
	ON_COMMAND(ID_BUTTON33028, OnBtnCloseAll)
	ON_COMMAND(ID_BUTTON33033, OnBtnCloseAllButOne)
	ON_COMMAND(ID_BUTTON33035, OnBtnCycleAspect)
	ON_COMMAND(ID_BUTTON33037, OnBtnCycleAllAspects)
	ON_COMMAND(ID_BUTTON33044, OnBtnCycleViews)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEDoc construction/destruction

CGMEDoc::CGMEDoc()
{
	theInstance = this;

	m_isClosing = false;

	editMode = GME_EDIT_MODE;

	// Making the navigation and mode toolbars visible
	CMainFrame::theInstance->ShowNavigationAndModeToolbars(true);

	try {
	    COMTHROW( resolver.CoCreateInstance(L"Mga.MgaResolver"));
	}
	catch(hresult_exception e) {
		AfxMessageBox(_T("Fatal error: cannot create MGA Resolver component!"),MB_OK | MB_ICONSTOP);
	}
}

CGMEDoc::~CGMEDoc()
{
	theInstance = 0;
}

bool CGMEDoc::CreateFcoList(CGuiObjectList* list,CComPtr<IMgaFCOs> &fcos,CGMEView *view)
{
	try {
		view->BeginTransaction(TRANSACTION_READ_ONLY);
		ASSERT(view);

		CComPtr<IMgaFCOs> coll;
		COMTHROW(coll.CoCreateInstance(OLESTR("Mga.MgaFCOs")));
		POSITION pos = list->GetHeadPosition();
		while(pos) {
			COMTHROW(coll->Append(list->GetNext(pos)->mgaFco));
		}
		COMTHROW(view->currentModel->AddInternalConnections(coll,&fcos));
		view->CommitTransaction();
	}
	catch(hresult_exception e) {
		view->AbortTransaction(e.hr);
		return false;
	}
	return true;
}

bool CGMEDoc::CreateFcoList2(CGuiFcoList* list,CComPtr<IMgaFCOs> &fcos,CGMEView *view, long* p_howmany, bool* array[8])
{
	bool all_containers = *array[ 0 ];
	bool all_refs		= *array[ 1 ];
	bool all_sets		= *array[ 2 ];
	bool all_conns      = *array[ 3 ];

	bool any_container	= *array[ 4 ];
	bool any_ref		= *array[ 5 ];
	bool any_set		= *array[ 6 ];
	bool any_conn		= *array[ 7 ];

	long how_many = 0, now_how_many = 0;
	try {
		view->BeginTransaction(TRANSACTION_READ_ONLY);
		ASSERT(view);

		CComPtr<IMgaFCOs> coll;
		COMTHROW(coll.CoCreateInstance(OLESTR("Mga.MgaFCOs")));
		POSITION pos = list->GetHeadPosition();
		while(pos) {
			CComObjPtr<IMgaFCO> one_fco = list->GetNext(pos)->mgaFco;
			COMTHROW(coll->Append( one_fco));

			objtype_enum type_info;
			COMTHROW( one_fco->get_ObjType(&type_info)); 

			all_containers &= (type_info == OBJTYPE_MODEL || type_info == OBJTYPE_FOLDER);
			all_refs  &= (type_info == OBJTYPE_REFERENCE);
			all_sets  &= (type_info == OBJTYPE_SET);
			all_conns &= (type_info == OBJTYPE_CONNECTION);

			any_container |= (type_info == OBJTYPE_MODEL || type_info == OBJTYPE_FOLDER);
			any_ref  |= (type_info == OBJTYPE_REFERENCE);
			any_set  |= (type_info == OBJTYPE_SET);
			any_conn |= (type_info == OBJTYPE_CONNECTION);
			
			++how_many;
		}
		COMTHROW(view->currentModel->AddInternalConnections(coll,&fcos));

		// some connections might have been added
		COMTHROW( fcos->get_Count( &now_how_many) ); 
		
		if( now_how_many > how_many)
			any_conn = true;

		view->CommitTransaction();
	}
	catch(hresult_exception e) {
		view->AbortTransaction(e.hr);
		return false;
	}
	
	*p_howmany = now_how_many;

	*array[ 0 ] = all_containers;
	*array[ 1 ] = all_refs;
	*array[ 2 ] = all_sets;
	*array[ 3 ] = all_conns;
	*array[ 4 ] = any_container;
	*array[ 5 ] = any_ref;
	*array[ 6 ] = any_set;
	*array[ 7 ] = any_conn;

	return true;
}

bool CGMEDoc::CreateAnnotationList(CGuiAnnotatorList* list,CComPtr<IMgaRegNodes> &anns,CGMEView *view)
{
	try {
		view->BeginTransaction(TRANSACTION_READ_ONLY);
		ASSERT(view);
		COMTHROW(anns.CoCreateInstance(OLESTR("Mga.MgaRegNodes")));
		POSITION pos = list->GetHeadPosition();
		while(pos) {
			COMTHROW(anns->Append(list->GetNext(pos)->rootNode));
		}
		view->CommitTransaction();
	}
	catch(hresult_exception e) {
		view->AbortTransaction(e.hr);
		return false;
	}
	return true;
}

void CGMEDoc::CopyToClipboard(CGuiObjectList* list, CGuiAnnotatorList *annList,CGMEDataDescriptor* desc,CGMEView *view)
{
	ASSERT( list && desc );

	CGMEDataSource* pDataSource = new CGMEDataSource(theApp.mgaProject);
	CComPtr<IMgaFCOs> fcos;
	CreateFcoList(list,fcos,view);
	CComPtr<IMgaRegNodes> anns;
	CreateAnnotationList(annList, anns,view);
	pDataSource->SetNodes(fcos);
	pDataSource->SetAnnotations(anns);
	pDataSource->CacheDescriptor(desc);
	pDataSource->SetClipboard();
	
}

void CGMEDoc::CopyClosureToClipboard( CGuiObjectList* list, CGuiAnnotatorList *annList, CGMEDataDescriptor* desc, CGMEView *view)
{
	ASSERT( list && desc );

	CComPtr<IMgaFCOs> fcos;
	CreateFcoList( list, fcos, view);

	CComPtr<IMgaClosure> cl;

	VARIANT_BOOL _refersTo           ; VARIANT_BOOL _referredBy;
	VARIANT_BOOL _connections        ; VARIANT_BOOL _connsThroughRefport;
	VARIANT_BOOL _setMembers         ; VARIANT_BOOL _memberOfSets;
	VARIANT_BOOL _modelContainment   ; VARIANT_BOOL _partOfModels;
	VARIANT_BOOL _folderContainment  ; VARIANT_BOOL _partOfFolders;
	VARIANT_BOOL _baseTypeOf         ; VARIANT_BOOL _derivedTypesOf;
	long _libraryElementHandling;         // 0: stubs; 1: stop; 2: continue
	long _wrappingMode;                   // 0: miniproject; 1: automatic; 2: as is
	long _kindFilter;                     // -1 = all; otherwise calculate the 0b000rsamf mask; where f = Folders; m = Models; a = Atoms; s = Sets; r = References
	long _markThem;
	long _options;
	VARIANT_BOOL _directionDown;
	VARIANT_BOOL _copyToFile;             // file or clipboard
	VARIANT_BOOL _okPressed;              // how ended the dialog
	CComBSTR _userSelFileName;

	COMTHROW( cl.CoCreateInstance(L"Mga.MgaClosure") );

	// ask the last closure id in a RO transaction
	view->BeginTransaction( TRANSACTION_READ_ONLY);
	try {
		COMTHROW( cl->GetLastClosureId( theApp.mgaProject, &_markThem));
		view->CommitTransaction();
	} catch( hresult_exception& e) {
		view->AbortTransaction( e.hr);
		_markThem = -2; // will be increased to -1
	}

	++_markThem; // propose this as the next closure id

	COMTHROW( cl->SelectiveClosureDlg
		( VARIANT_FALSE
		, &_refersTo           , &_referredBy
		, &_connections        , &_connsThroughRefport
		, &_setMembers         , &_memberOfSets
		, &_modelContainment   , &_partOfModels
		, &_folderContainment  , &_partOfFolders
		, &_baseTypeOf         , &_derivedTypesOf
		, &_libraryElementHandling          // 0: stubs; 1: stop; 2: continue
		, &_wrappingMode                    // 0: miniproject; 1: automatic; 2: as is
		, &_kindFilter                      // -1 = all; otherwise calculate the 0b000rsamf mask; where f = Folders; m = Models; a = Atoms; s = Sets; r = References
		, &_markThem                        // -1 = do not mark; otherwise mark it with the value
		, &_options
		, &_directionDown
		, &_copyToFile              // file or clipboard
		, &_okPressed               // how ended the dialog
		, &_userSelFileName
		));
	if( _okPressed == VARIANT_FALSE) return;

	if( _markThem != -1) // save this if elements need to be marked
	{
		view->BeginTransaction();
		try {
			COMTHROW( cl->PutLastClosureId( theApp.mgaProject, _markThem));
			view->CommitTransaction();
		} catch( hresult_exception& e) {
			view->AbortTransaction( e.hr);
		}
	}

	CComObjPtr<IMgaFCOs> sel_fcos, top_fcos;
	CComObjPtr<IMgaFolders> sel_folders, top_folders;
	VARIANT_BOOL tops_filled = VARIANT_FALSE;
	CComBstrObj acc_kinds;

	COMTHROW( cl->SelectiveClosureCalc
		( fcos          , 0
		, _refersTo          , _referredBy
		, _connections       , _connsThroughRefport
		, _setMembers        , _memberOfSets
		, _modelContainment  , _partOfModels
		, _folderContainment , _partOfFolders
		, _baseTypeOf        , _derivedTypesOf
		, _libraryElementHandling          // 0: stubs, 1: stop, 2: continue
		, _wrappingMode                    // 0: miniproject, 1: automatic, 2: as is
		, _kindFilter                      // -1 = all, otherwise calculate the 0b000rsamf mask, where f = Folders, m = Models, a = Atoms, s = Sets, r = References
		, _markThem                        // -1 = do not mark;
		, _directionDown
		, PutOut( sel_fcos), PutOut( sel_folders)
		, PutOut( top_fcos), PutOut( top_folders)
		, &tops_filled
		, PutOut( acc_kinds)
		));

	if( !sel_fcos && !sel_folders) return;

	if( _copyToFile == VARIANT_TRUE) 
	{
		CComPtr<IMgaDumper> dumper;
		COMTHROW( dumper.CoCreateInstance(L"Mga.MgaDumper") );

		if( _wrappingMode == 0)
		{
			COMTHROW( dumper->DumpClos( sel_fcos, sel_folders, _userSelFileName, _options));
		}
		else if( _wrappingMode == 1 || _wrappingMode == 2)
		{
			if ( tops_filled == VARIANT_TRUE)
				COMTHROW( dumper->DumpClosR( sel_fcos, sel_folders, _userSelFileName, top_fcos, top_folders, _options, 0, acc_kinds));
			else 
				// dump like in miniproject case from The RootFolder
				// if the data is copied to clipboard then the DumpClosR will call the 
				// DumpClos when it will realize the topfco and topfolders are empty
				COMTHROW( dumper->DumpClos( sel_fcos, sel_folders, _userSelFileName, _options));
		}
	}
	else // _copyToFile == VARIANT_FALSE 
	{
		CGMEClosureDataSource * pDataSource = new CGMEClosureDataSource( theApp.mgaProject);

		if ( sel_fcos)		pDataSource->SetNodes( sel_fcos);
		if ( sel_folders)	pDataSource->SetFolders( sel_folders);
		
		if ( top_fcos)		pDataSource->SetTopNodes( top_fcos);
		if ( top_folders)	pDataSource->SetTopNodes( top_folders);

		pDataSource->SetOptions( _options);
		pDataSource->SetAcceptingKinds( acc_kinds);

		CComPtr<IMgaRegNodes> anns; 
		CreateAnnotationList( annList, anns, view);
		pDataSource->SetAnnotations( anns);
		pDataSource->CacheDescriptor( desc);
		pDataSource->SetClipboard();
		pDataSource->FlushClipboard();
	}
}

void CGMEDoc::CopySmartToClipboard( CGuiFcoList* list, CGuiAnnotatorList *annList, CGMEDataDescriptor* desc, CGMEView *view)
{
	int k = 0;
	bool all_containers = true;
	bool all_refs = true;
	bool all_sets = true;
	bool all_conns = true;

	bool any_container = false;
	bool any_ref = false;
	bool any_set = false;
	bool any_conn = false;

	ASSERT( list && desc );

	CComPtr<IMgaFCOs> fcos;
	bool* b_array[8] = { &all_containers, &all_refs, &all_sets, &all_conns, &any_container, &any_ref, &any_set, &any_conn };
	long number_of_elems = 0;

	if( !CreateFcoList2( list, fcos, view, &number_of_elems, b_array)) return;

	if( number_of_elems == 0) 
	{
		AfxMessageBox(_T("No object selected"), MB_ICONERROR);
		return;
	}

	
	CComPtr<IMgaClosure> cl;
	CComObjPtr<IMgaFCOs> sel_fcos, top_fcos;
	CComObjPtr<IMgaFolders> sel_folders, top_folders;
	long options;
	CComBstrObj acckinds; 
	CComBstrObj path; 

	// commented out when new smart copy introduced
	// if container selected then it must be lonely (no multiple selection)
	// if container selected no other kinds allowed
	//if( any_container && !all_containers || number_of_elems > 1 && all_containers)
	//{
	//	AfxMessageBox(_T("Invalid selection for smart copy. You can select either one container or several non-containers."), MB_ICONERROR);
	//	return;
	//}

	if( any_container) k |= 1024;

	if( any_ref) k |= 2048;
	if( any_set) k |= 4096;
	if( any_conn) k |= 8192;

	COMTHROW( cl.CoCreateInstance(L"Mga.MgaClosure") );
	
	COMTHROW( cl->SmartCopy( 
		fcos, 
		0, 
		PutOut( sel_fcos), 
		PutOut( sel_folders), 
		PutOut( top_fcos), 
		PutOut( top_folders), 
		k, 
		&options, 
		PutOut( acckinds),
		PutOut( path)
		));
	
	if ( sel_fcos || sel_folders) // if both null the operation is not needed (either dumped to file already or unsuccessful)
	{
		CRectList dummyAnnList;

		CGMEClosureDataSource * pDataSource = new CGMEClosureDataSource( theApp.mgaProject);

		if ( sel_fcos)		pDataSource->SetNodes( sel_fcos);
		if ( sel_folders)	pDataSource->SetFolders( sel_folders);
		
		if ( top_fcos)		pDataSource->SetTopNodes( top_fcos);
		if ( top_folders)	pDataSource->SetTopNodes( top_folders);

		pDataSource->SetOptions( options);
		pDataSource->SetAbsPathPart( path);
		pDataSource->SetAcceptingKinds( acckinds);

		CComPtr<IMgaRegNodes> anns; 
		CreateAnnotationList( annList, anns, view);
		pDataSource->SetAnnotations( anns);
		pDataSource->CacheDescriptor( desc);
		pDataSource->SetClipboard();
		pDataSource->FlushClipboard();
	}
}

DROPEFFECT CGMEDoc::DoDragDrop(CGuiObjectList* list, CGuiAnnotatorList *annList, CGMEDataDescriptor* desc,
	DWORD dwEffects, LPCRECT lpRectStartDrag,CGMEView *view)
{
	ASSERT( list && desc && annList );

	CGMEDataSource dataSource(theApp.mgaProject);
	CComPtr<IMgaFCOs> fcos;
	CreateFcoList(list,fcos,view);
	CComPtr<IMgaRegNodes> anns;
	CreateAnnotationList(annList, anns,view);
	dataSource.SetNodes(fcos);
	dataSource.SetAnnotations(anns);
	dataSource.CacheDescriptor(desc);
	DROPEFFECT de = dataSource.DoDragDrop(dwEffects,lpRectStartDrag);
	return de;
}

DROPEFFECT CGMEDoc::DoDragDrop(CGuiObject *guiObj, CGMEDataDescriptor* desc,
	DWORD dwEffects, LPCRECT lpRectStartDrag)
{
	CGMEDataSource dataSource(theApp.mgaProject);
	dataSource.SetMetaRole(guiObj->metaRole);
	dataSource.CacheDescriptor(desc);
	DROPEFFECT de = dataSource.DoDragDrop(dwEffects,lpRectStartDrag);
	return de;
}

#if !defined (ACTIVEXGMEVIEW)
CGMEView
#else
CGMEChildFrame
#endif
*CGMEDoc::FindView(CComPtr<IMgaModel> model)
{
	if(model != 0) {
		POSITION pos = GetFirstViewPosition();
		while (pos != NULL) {
#if !defined (ACTIVEXGMEVIEW)
			CGMEView* view = (CGMEView *)GetNextView(pos);
			VARIANT_BOOL b; 
			// COMTHROW(model->get_IsEqual(view->GetCurrentModel(), &b));  Territory problems
			// bool b = model.IsEqualObject(view->GetCurrentModel());
			view->BeginTransaction(TRANSACTION_READ_ONLY);
			// FIXME: need to adopt when inserting a model from model editor context menu
			COMTHROW(view->GetCurrentModel()->get_IsEqual(model, &b));
			view->CommitTransaction();
			if(b)
				return view;
#else
			CGMEChildFrame* pView = (CGMEChildFrame *)GetNextView(pos);
			VARIANT_BOOL b;
			long status;
			COMTHROW(theApp.mgaProject->get_ProjectStatus(&status));
			bool inTrans = (status & 0x08L) != 0;
			CComPtr<IMgaTerritory> terr;
			if (!inTrans) {
				COMTHROW(theApp.mgaProject->CreateTerritory(NULL, &terr));
				COMTHROW(theApp.mgaProject->BeginTransaction(terr, TRANSACTION_READ_ONLY));
			} else {
				COMTHROW(theApp.mgaProject->get_ActiveTerritory(&terr));
			}

			CComPtr<IMgaModel> mgaModel = pView->GetMgaModel();
			if (mgaModel)
				COMTHROW(mgaModel->get_IsEqual(model, &b));
			if (!inTrans) {
				theApp.mgaProject->CommitTransaction();
			}
			if (mgaModel && b)
				return pView;
#endif
		}
	}
	return 0;
}

void CGMEDoc::DoOnViewRefresh(void)
{
	OnViewRefresh();
}

void CGMEDoc::InvalidateAllViews(bool thorough,bool /*fullAutoRoute*/)
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL) {
#if !defined (ACTIVEXGMEVIEW)
		CGMEView* pView = (CGMEView *)GetNextView(pos);
/*
		if(!pView->IsIconic())
			fullAutoRoute ? pView->AutoRoute() : pView->IncrementalAutoRoute();
*/
		pView->Invalidate(thorough);
#endif
	}
	if(thorough) {
		CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
		CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
		((CGMEView *)(pChild->GetActiveView()))->Invalidate(thorough);
	}
}

void CGMEDoc::ShowObject(CComPtr<IUnknown> alienObject, BOOL inParent)
{
	// Be prepared for in-transaction and out-transaction scenarios
	// mgaFCO must be transfered to current territory

	// Additional note (JIRA GME-219):
	// In case this function is used through the IGMEOLEApp interface, the call won't be successful, because
	// CMainFrame::CreateNewView will initiate a CGMEView::OnInitialUpdate (docTemplate->InitialUpdateFrame),
	// which cannot handle an open transaction

	bool isModel = true;
	if(alienObject == NULL) {
		return;
	}

	if (!theApp.mgaProject) {
		return;
	}

	CComPtr<IMgaFCO> fco;
	CComPtr<IMgaModel> model;
	
	
	long status;
	COMTHROW(theApp.mgaProject->get_ProjectStatus(&status));
	bool inTrans = (status & 0x08L) != 0;
	CComPtr<IMgaTerritory> terr;

	if (!SUCCEEDED(alienObject.QueryInterface(&model))) {  // Alien object is not a modell
		isModel = false;
		if (SUCCEEDED(alienObject.QueryInterface(&fco))) {
			
			try {
				if (!inTrans) {
					COMTHROW(theApp.mgaProject->CreateTerritory(NULL, &terr, NULL));
					COMTHROW(theApp.mgaProject->BeginTransaction(terr, TRANSACTION_GENERAL));
				}
				else {
					COMTHROW(theApp.mgaProject->get_ActiveTerritory(&terr));
				}
				CComPtr<IMgaFCO> tmpfco = fco;
				fco = NULL;
				COMTHROW(terr->OpenFCO(tmpfco, &fco));
			}
			catch (hresult_exception e) {
				return;
			}

			try {
				CComPtr<IMgaObject> parent;
				COMTHROW(fco->GetParent(&parent, NULL));
				model = NULL;
				COMTHROW(parent.QueryInterface(&model));
			}
			catch (hresult_exception e) {
				model = NULL;
				if (!inTrans) 
					theApp.mgaProject->AbortTransaction();
			}

			if (!inTrans) {
				theApp.mgaProject->CommitTransaction();
			}
		}
	}

	if (model  &&  isModel  &&  inParent)
	{

		try 
		{
			if (!inTrans) {
				COMTHROW(theApp.mgaProject->CreateTerritory(NULL, &terr, NULL));
				COMTHROW(theApp.mgaProject->BeginTransaction(terr, TRANSACTION_GENERAL));
				CComPtr<IMgaModel> tmpmod = model; // put model into the active territory
				model = NULL;
				CComPtr<IMgaFCO> fco_mod;
				COMTHROW(terr->OpenFCO(tmpmod, &fco_mod)); // OpenFCO fills an IMgaFCO
				COMTHROW( fco_mod.QueryInterface( &model));// casted to IMgaModel
			}
			CComPtr<IMgaObject> parent;
			COMTHROW(model->GetParent(&parent, NULL));
			COMTHROW(model.QueryInterface(&fco));
			model = NULL;
			COMTHROW(parent.QueryInterface(&model));
			// it might be invoked from the ActiveBrowser (on a top model)
			// in which case line above must throw if parent is a folder

			if (!inTrans) 
				theApp.mgaProject->CommitTransaction();
		}
		catch (hresult_exception e) 
		{
			model = NULL;
			if (!inTrans) 
				theApp.mgaProject->AbortTransaction();
		}
	}
	if (model) {
#if !defined (ACTIVEXGMEVIEW)
		CGMEView *view  = NULL;
#else
		CGMEChildFrame* view = NULL;
#endif

		if (!theApp.multipleView) {
			view = FindView(model);
			if(!view) {
				SetNextToView(model,_T(""), fco);
			}
			else {
#if !defined (ACTIVEXGMEVIEW)
				view->SetCenterObject(fco);
#endif
				if( theApp.isHistoryEnabled())
				{
					//clear history
					clearForwHistory();
				}
			}
		}
		else {
			SetNextToView(model,_T(""), fco);
		}

		CMainFrame::theInstance->CreateNewView(view, model);
		// PETER: This is needed to get the focus (SetFocus does not work, since it uses SendMessage())
		::PostMessage(CMainFrame::theInstance->GetSafeHwnd(), WM_SETFOCUS, 0, 0);
	}
}

void CGMEDoc::ShowObject(LPCTSTR objectID)
{
	CComBSTR bstrID(objectID);
	CComPtr<IMgaObject> object;
	CComPtr<IUnknown> unk;
	bool inTrans = false;


	try {
		long status;
		COMTHROW(theApp.mgaProject->get_ProjectStatus(&status));
		inTrans = (status & 0x08L) != 0;
		CComPtr<IMgaTerritory> terr;
		if (!inTrans) {
			COMTHROW(theApp.mgaProject->CreateTerritory(NULL, &terr, NULL));
			COMTHROW(theApp.mgaProject->BeginTransaction(terr, TRANSACTION_GENERAL));
		}
		else {
			COMTHROW(theApp.mgaProject->get_ActiveTerritory(&terr));
		}

		COMTHROW(theApp.mgaProject->GetObjectByID(bstrID, &object));
		COMTHROW(object.QueryInterface(&unk));

		// CheckRead() to see if this object was deleted
		long relid;
		COMTHROW(object->get_RelID(&relid));
		if (!inTrans) {
			theApp.mgaProject->CommitTransaction();
		}
	}
	catch (hresult_exception e) {
		if (!inTrans) 
			theApp.mgaProject->AbortTransaction();
		return;
	}

	
	ShowObject(unk);
}

GMEModeCode CGMEDoc::GetEditMode() const
{
	return editMode;
}

void CGMEDoc::SetEditMode(GMEModeCode mode, const CString& statusMsg)
{
	editMode = mode;
	CMainFrame::theInstance->WriteStatusMode(statusMsg);
	ViewModeChange();
}

int  CGMEDoc::HowManyViewsAreOpen()
{
	int i = 0;
	POSITION pos = GetFirstViewPosition();
	if(pos) {
		while (pos != NULL) {
			CGMEView* pView = (CGMEView *)GetNextView(pos);
			++i;
		}
	}
	return i;
}

void CGMEDoc::ResetAllViews()
{
	POSITION pos = GetFirstViewPosition();
	if(pos) {
		while (pos != NULL) {
#if !defined (ACTIVEXGMEVIEW)
			CGMEView* pView = (CGMEView *)GetNextView(pos);
			pView->Reset(true);
#endif
		}
//		InvalidateAllViews(true);
	}
}

void CGMEDoc::ChangeAspects(int index, CString aspName)
{
	POSITION pos = GetFirstViewPosition();
	if (pos) {
		while (pos != NULL) {
#if defined(ACTIVEXGMEVIEW)
			CGMEChildFrame* pView = (CGMEChildFrame *)GetNextView(pos);
			pView->ChangeAspect(index);
#else
			CGMEView* pView = (CGMEView *)GetNextView(pos);
			pView->ChangeAspect(aspName);
#endif
		}
	}
}

void CGMEDoc::ConvertPathToCustom(CComPtr<IUnknown>& pMgaObject)
{

	POSITION pos = GetFirstViewPosition();
	if (pos) {
		while (pos != NULL) {
			CGMEView* pView = (CGMEView*)GetNextView(pos);

			pView->BeginTransaction();
			try {
				pView->ConvertPathToCustom(pMgaObject);
				pView->CommitTransaction();
			} catch(hresult_exception& e) {
				pView->AbortTransaction(e.hr);
			}
		}
	}
}

void CGMEDoc::ViewModeChange()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL) {
#if !defined (ACTIVEXGMEVIEW)
		CGMEView* pView = (CGMEView *)GetNextView(pos);
		pView->ModeChange();
#endif
	}

	// make sure that the currently open model has the focus
	CMainFrame::theInstance->SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CGMEDoc diagnostics

#ifdef _DEBUG
void CGMEDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGMEDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGMEDoc commands

void CGMEDoc::SetMode(long mode)
{
	switch (mode)
	{
	case 0: // edit
		OnModeEdit();
		break;
	case 1: // connect
		OnModeAutoconnect();
		break;
	case 2: // disconnect
		OnModeDisconnect();
		break;
	case 3: // set
		OnModeSet();
		break;
	case 4: // zoom
		OnModeZoom();
		break;
	case 5: // visualize
		OnModeVisualize();
		break;
	case 6: // short-connect
		OnModeShortConnect();
		break;
	case 7: // short-disconnect
		OnModeShortDisconnect();
		break;
	}

	m_isClosing = false; // in case was turned true
}

void CGMEDoc::OnModeAutoconnect() 
{
	CGMEEventLogger::LogGMEEvent("CGMEDoc::OnModeAutoconnect\r\n");
	SetEditMode(GME_AUTOCONNECT_MODE, _T("AUTOCONNECT"));
}

void CGMEDoc::OnUpdateModeAutoconnect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(editMode == GME_AUTOCONNECT_MODE);
}

void CGMEDoc::OnModeDisconnect() 
{
	CGMEEventLogger::LogGMEEvent("CGMEDoc::OnModeDisconnect\r\n");
	SetEditMode(GME_DISCONNECT_MODE, _T("DISCONNECT"));
}

void CGMEDoc::OnUpdateModeDisconnect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(editMode == GME_DISCONNECT_MODE);
}

void CGMEDoc::OnModeEdit() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::OnModeEdit\r\n"));
	SetEditMode(GME_EDIT_MODE, _T("EDIT"));
}

void CGMEDoc::OnUpdateModeEdit(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(editMode == GME_EDIT_MODE);
}

void CGMEDoc::OnModeSet() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::OnModeSet\r\n"));
	SetEditMode(GME_SET_MODE, _T("SET"));
}

void CGMEDoc::OnUpdateModeSet(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(editMode == GME_SET_MODE);
}

void CGMEDoc::OnModeZoom() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::OnModeZoom\r\n"));
	SetEditMode(GME_ZOOM_MODE, _T("ZOOM"));
}

void CGMEDoc::OnUpdateModeZoom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(editMode == GME_ZOOM_MODE);
}

void CGMEDoc::OnModeVisualize() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::OnModeVisualize\r\n"));
	SetEditMode(GME_VISUAL_MODE, _T("VISUALIZE"));
}

void CGMEDoc::OnUpdateModeVisualize(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(editMode == GME_VISUAL_MODE);
}

void CGMEDoc::OnModeShortConnect()
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::OnModeShortConnect\r\n"));
	SetEditMode(GME_SHORTAUTOCONNECT_MODE, _T("AUTOCONNECT"));
}

void CGMEDoc::OnUpdateModeShortConnect(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(editMode == GME_SHORTAUTOCONNECT_MODE);
}

void CGMEDoc::OnModeShortDisconnect()
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::OnModeShortDisconnect\r\n"));
	SetEditMode(GME_SHORTDISCONNECT_MODE, _T("DISCONNECT"));
}

void CGMEDoc::OnUpdateModeShortDisconnect(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(editMode == GME_SHORTDISCONNECT_MODE);
}

void CGMEDoc::OnUpdateFileClose(CCmdUI* pCmdUI) 
{
	POSITION pos = GetFirstViewPosition();
	pCmdUI->Enable(pos != NULL);
}

void CGMEDoc::OnCloseDocument(bool suppressErrors) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::OnCloseDocument\r\n"));
	m_isClosing = true;
	POSITION pos = GetFirstViewPosition();
	while (pos) {
#if !defined (ACTIVEXGMEVIEW)
		CGMEView* view = dynamic_cast<CGMEView*>(GetNextView(pos));
		ASSERT(view);
		bool canClose = view->SendCloseModelEvent();
		if ((!suppressErrors) && (!canClose)) {
			// View cannot be closed, eg.: constraint violations
			return;
		}
#endif
	}

	
	CDocument::OnCloseDocument();
	resolver = NULL;
	theApp.CloseProject();
	delete this;
}

BOOL CGMEDoc::OnNewDocument()
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::OnNewDocument\r\n"));
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

BOOL CGMEDoc::OnOpenDocument(LPCTSTR /*lpszPathName*/)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::OnOpenDocument\r\n"));
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	// TODO: Add your specialized creation code here
	m_isClosing = false;
	
	return TRUE;
}

void CGMEDoc::OnViewRefresh()
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::OnViewRefresh\r\n"));
	ResetAllViews();
	CGMEBrowser::theInstance->RefreshAll();
}

// ---------------------------------------------------
// ----------- History related methods ---------------
// ---------------------------------------------------
void CGMEDoc::back()
{
	if( theApp.isHistoryEnabled() && m_historian.isEnabledBack())
	{
		Historian::HistoryElem prev, curr;
		m_historian.popB( prev, curr);
		m_historian.pushF( curr);
		if( !prev.id().empty())
		{
			presentModel( prev.id().c_str(), prev.aspect().c_str());
		}
		else
			CGMEConsole::theInstance->Message( _T("Could not go further backward"), 1);
	}
}

void CGMEDoc::forw()
{
	if( theApp.isHistoryEnabled() && m_historian.isEnabledFrwd())
	{
		Historian::HistoryElem e;
		m_historian.popF( e);
		m_historian.pushB( e); // this time we should preserve forward stack
		if( !e.id().empty())
		{
			presentModel( e.id().c_str(), e.aspect().c_str());
		}
		else
			CGMEConsole::theInstance->Message( _T("Could not go further forward"), 1);
	}
}

void CGMEDoc::home()
{
	if( theApp.isHistoryEnabled() && !m_historian.isEmptyB())
	{
		Historian::HistoryElem home;
		m_historian.frontB( home);
		if( !home.id().empty())
		{
			presentModel( home.id().c_str(), home.aspect().c_str());
		}
		else
			CGMEConsole::theInstance->Message( _T("Could not go home"), 1);

		//m_historian.totalBrainWash();
	}
}

void CGMEDoc::clearTotalHistory()
{
	m_historian.totalBrainWash();
}

void CGMEDoc::clearForwHistory() // could be called: userAction
{
	m_historian.frwdBrainWash();
}

void CGMEDoc::closeActiveWnd()
{
#if !defined (ACTIVEXGMEVIEW)
	CGMEView* v = CGMEView::GetActiveView();
	if( !v) return;
	v->alive = false;
	v->frame->SetSendEvent(true);
	v->frame->PostMessage(WM_CLOSE);
#endif
}

void CGMEDoc::closeAllWnd()
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL) {
#if !defined (ACTIVEXGMEVIEW)
		CGMEView* v = (CGMEView *) GetNextView( pos);
		v->alive = false;
		v->frame->SetSendEvent(true);
		v->frame->SendMessage(WM_CLOSE);
#endif
	}
}

void CGMEDoc::closeAllButActiveWnd()
{
	CGMEView* actv = CGMEView::GetActiveView();

	POSITION pos = GetFirstViewPosition();
	while (pos != NULL) {
#if !defined (ACTIVEXGMEVIEW)
		CGMEView* v = (CGMEView *) GetNextView( pos);
		if( v != actv)
		{
			v->alive = false;
			v->frame->SetSendEvent(true);
			v->frame->SendMessage(WM_CLOSE);
		}
#endif
	}
}

void CGMEDoc::cycleAspect()
{
#if !defined (ACTIVEXGMEVIEW)
	CGMEView* actv = CGMEView::GetActiveView();
	if( actv)
		actv->CycleAspect();
#endif
}

void CGMEDoc::cycleAllAspects()
{
#if !defined (ACTIVEXGMEVIEW)
	CGMEView* actv = CGMEView::GetActiveView();
	if( actv)
		actv->CycleAllAspects();
#endif
}

void CGMEDoc::cycleViews()
{
#if !defined (ACTIVEXGMEVIEW)
	CGMEView* nexv = 0;
	CGMEView* actv = CGMEView::GetActiveView();
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL) {
		CGMEView* v = (CGMEView *) GetNextView( pos);
		if( v == actv)
		{
			if( pos == NULL) // if at the end, go to the first
				pos = GetFirstViewPosition();
			nexv = (CGMEView *) GetNextView( pos);
			break;
		}
	}

	if( nexv)
	{
		//nexv->SetFocus();
		CMainFrame::theInstance->CreateNewView( nexv, nexv->GetCurrentModel());
		::PostMessage(CMainFrame::theInstance->GetSafeHwnd(), WM_SETFOCUS, 0, 0);
	}
#endif
}

void CGMEDoc::tellHistorian( CComBSTR& modid, CString asp)
{
	if( !theApp.isHistoryEnabled()) return;
	if( theApp.multipleView) return; // do not bother with history in this scenario
	if( !modid || modid.Length() == 0) return;

	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::tellHistorian(str,asp)\r\n"));

	Historian::HistoryElem e( (LPCTSTR) PutInCString( modid), (LPCTSTR) asp);
	m_historian.erasePrevOccurencesB( e);
	m_historian.pushB( e);
}

void CGMEDoc::tellHistorian( CComPtr<IMgaModel>& model, CString asp)
{ 
	if( !theApp.isHistoryEnabled()) return;
	if( theApp.multipleView) return; // do not bother with history in this scenario
	if( !model)              return;

	CGMEEventLogger::LogGMEEvent(_T("CGMEDoc::tellHistorian(ptr,asp)\r\n"));

	try	{
		long status;
		COMTHROW(theApp.mgaProject->get_ProjectStatus(&status));
		bool inTrans = (status & 0x08L) != 0;
		CComPtr<IMgaTerritory> terr;
		if (!inTrans) {
			COMTHROW(theApp.mgaProject->CreateTerritory(NULL, &terr, NULL));
			COMTHROW(theApp.mgaProject->BeginTransaction(terr, TRANSACTION_READ_ONLY));
		}
		else {
			COMTHROW(theApp.mgaProject->get_ActiveTerritory(&terr));
		}

		status = OBJECT_ZOMBIE;
		
		CComPtr<IMgaFCO>  nfco;
		COMTHROW( terr->OpenFCO( CComPtr<IMgaFCO>( model), &nfco));
		COMTHROW( nfco->get_Status( &status));
		if( status == OBJECT_EXISTS) 
		{
			CComBSTR id;
			COMTHROW( nfco->get_ID( &id));
			
			Historian::HistoryElem e( (LPCTSTR) PutInCString( id), (LPCTSTR) asp);
			m_historian.erasePrevOccurencesB( e);
			m_historian.pushB( e);
		}

		if (!inTrans) {
			theApp.mgaProject->CommitTransaction();
		}
	} catch(...) {
		ASSERT(0);
		m_historian.totalBrainWash();
	}
}

void CGMEDoc::eraseFromHistory( LPCTSTR p_id)
{
	if( !theApp.isHistoryEnabled()) return;

	m_historian.eraseOccurences( p_id);
}

void CGMEDoc::presentModel( LPCTSTR p_objectId, LPCTSTR p_aspectName)
{
	if( !theApp.isHistoryEnabled()) return;

	if( theApp.multipleView) return; // do not bother with history in this scenario
	if( !theApp.mgaProject)  return;

	bool               inTrans( false);
	CComBSTR           model_id( p_objectId);
	CComPtr<IMgaFCO>   object;
	CComPtr<IMgaModel> model;

	try {
		long status;
		COMTHROW(theApp.mgaProject->get_ProjectStatus(&status));
		inTrans = (status & 0x08L) != 0;
		CComPtr<IMgaTerritory> terr;
		if (!inTrans) {
			COMTHROW(theApp.mgaProject->CreateTerritory(NULL, &terr, NULL));
			COMTHROW(theApp.mgaProject->BeginTransaction(terr, TRANSACTION_READ_ONLY));
		}
		else
			COMTHROW(theApp.mgaProject->get_ActiveTerritory(&terr));

		COMTHROW( theApp.mgaProject->GetFCOByID( model_id, &object));
		status = OBJECT_ZOMBIE;
		// check if object really exists
		if( object)
		{
			COMTHROW( object.QueryInterface( &model));
			COMTHROW( object->get_Status( &status));
		}

		if( status != OBJECT_EXISTS || !model)
			throw hresult_exception( -1);

		if( !inTrans)
			theApp.mgaProject->CommitTransaction();
	}
	catch (hresult_exception e) {
		if (!inTrans) 
			theApp.mgaProject->AbortTransaction();

		eraseFromHistory( p_objectId);
		return;
	}

	ASSERT( model);
#if !defined (ACTIVEXGMEVIEW)
	CGMEView *view  = FindView(model);
#else
	CGMEChildFrame* view = FindView(model);
#endif
	if( !view)
	{
		SetNextToView( model, p_aspectName, CComPtr<IMgaFCO>()); // p_aspectName used here
	}
#if !defined (ACTIVEXGMEVIEW)
	else if( view->currentAspect && view->currentAspect->name != p_aspectName)
	{
		view->ChangeAspect( p_aspectName, false);
	}
#endif

	// PETER: This is needed to get the focus (SetFocus does not work, since it uses SendMessage())
	CMainFrame::theInstance->CreateNewView(view, model);
	::PostMessage(CMainFrame::theInstance->GetSafeHwnd(), WM_SETFOCUS, 0, 0);
}

//**********************************************************************************************
//**********************************************************************************************
//*********   C L A S S        H I S T O R I A N     *******************************************
//**********************************************************************************************
//**********************************************************************************************
void CGMEDoc::Historian::pushB( const HistoryElem& e)
{
	HistoryElem f = m_backStack.empty()? HistoryElem() : m_backStack.back();
	if( f != e)
		m_backStack.push_back( e);
}
void CGMEDoc::Historian::pushF( const HistoryElem& e)
{
	HistoryElem f = m_frwdStack.empty()? HistoryElem() : m_frwdStack.back();
	if( f != e)
		m_frwdStack.push_back( e);
}

void CGMEDoc::Historian::popB( HistoryElem& p_prev, HistoryElem& p_current)
{
	if( m_backStack.size() <= 1) return;
	p_current = m_backStack.back();
	m_backStack.pop_back(); // get rid of current
	p_prev = m_backStack.back();
}

void CGMEDoc::Historian::popF( HistoryElem& e)
{
	if( m_frwdStack.size() < 1) return;
	e = m_frwdStack.back();
	m_frwdStack.pop_back();
}

void CGMEDoc::Historian::frontB( HistoryElem& p_front)
{
	if( m_backStack.size() == 0) return;
	p_front = m_backStack.front();
}

bool CGMEDoc::Historian::isEmptyB() const
{
	return m_backStack.size() == 0;
}

bool CGMEDoc::Historian::isEnabledBack() const
{
	return m_backStack.size() > 1;
}

bool CGMEDoc::Historian::isEnabledFrwd() const
{
	return m_frwdStack.size() > 0;
}

bool CGMEDoc::Historian::isEnabledHome() const
{
	return !isEmptyB();
}

void CGMEDoc::Historian::totalBrainWash()
{
	m_frwdStack.clear();
	m_backStack.clear();
}

void CGMEDoc::Historian::frwdBrainWash()
{
	m_frwdStack.clear();
}

void CGMEDoc::Historian::erasePrevOccurencesB( const HistoryElem& elem)
{
	HISTLIST_ITER it;
	
	// backward stack
	it = m_backStack.begin();
	while( it != m_backStack.end())
	{
		if( *it == elem)
			it = m_backStack.erase( it); // will progress it
		else
			++it;
	}

	// FIXME: MAX_LEN != MAX_LEN is always false
	const int MAX_LEN = -1;
	while( MAX_LEN != -1 && m_backStack.size() > MAX_LEN)
	{
		m_backStack.pop_front();
	}
}

void CGMEDoc::Historian::eraseOccurences( const std::wstring& p_id)
{
	HISTLIST_ITER it;
	
	// backward stack
	it = m_backStack.begin();
	while( it != m_backStack.end())
	{
		if( it->id() == p_id)
			it = m_backStack.erase( it); // will progress it
		else
			++it;
	}

	// forward stack
	it = m_frwdStack.begin();
	while( it != m_frwdStack.end())
	{
		if( it->id() == p_id)
			it = m_frwdStack.erase( it); // will progress it
		else
			++it;
	}

}
//**********************************************************************************************
//**********************************************************************************************

void CGMEDoc::OnBtnBack()
{
	back();
}

void CGMEDoc::OnBtnFrwd()
{
	forw();
}

void CGMEDoc::OnBtnHome()
{
	home();
}

void CGMEDoc::OnUpdateBtnBack(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( theApp.isHistoryEnabled() && m_historian.isEnabledBack());
}

void CGMEDoc::OnUpdateBtnFrwd(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( theApp.isHistoryEnabled() && m_historian.isEnabledFrwd());
}

void CGMEDoc::OnUpdateBtnHome(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( theApp.isHistoryEnabled() && m_historian.isEnabledHome());
}

void CGMEDoc::OnBtnCloseActive()
{
	closeActiveWnd();
}

void CGMEDoc::OnBtnCloseAll()
{
	closeAllWnd();
}

void CGMEDoc::OnBtnCloseAllButOne()
{
	closeAllButActiveWnd();
}

void CGMEDoc::OnBtnCycleAspect()
{
	cycleAspect();
}

void CGMEDoc::OnBtnCycleAllAspects()
{
	cycleAllAspects();
}

void CGMEDoc::OnBtnCycleViews()
{
	cycleViews();
}
