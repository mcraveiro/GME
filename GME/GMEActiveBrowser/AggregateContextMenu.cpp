// AggregateContextMenu.cpp: implementation of the CAggregateContextMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "AggregateContextMenu.h"
#include "GMEActiveBrowserCtl.h"
#include "AggregateOptionsDlg.h"
#include "..\Gme\GMEOLEData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAggregateContextMenu::CAggregateContextMenu(CAggregatePropertyPage* pParent)
{
	m_pParent=pParent;
	
	// Menu IDs for the custom (dynamically inserted) items 
	m_InsertionMinID=m_InsertionMaxID=33000;
}

CAggregateContextMenu::~CAggregateContextMenu()
{

}

BOOL CAggregateContextMenu::Run(CPoint point)
{

	m_nSelectionCount=m_pParent->m_TreeAggregate.GetSelectedCount();
	switch (m_nSelectionCount)
	{
		case 0:
			{
				VERIFY(LoadMenu(CG_IDR_POPUP_AGGREGATE_PROPERTY_PAGE_GENERAL));
			}break;
		case 1:
			{
				CreateForSingleItem();
			}break;
		
		default:
			{
				VERIFY(LoadMenu(CG_IDR_POPUP_AGGREGATE_PROPERTY_PAGE_MULTIPLE_ITEMS));
			}		
	}
	
	/* Initialization for all type of context menus */
	CreateForAll();

	CMenu* pPopup =GetSubMenu(0);
	ASSERT(pPopup != NULL);
	

	UINT nItemID=pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON |TPM_NONOTIFY|TPM_RETURNCMD, point.x, point.y,
							m_pParent);

	TRACE1("Selected menu item ID: %d\n",nItemID);
	return DispatchCommand(nItemID);
}


BOOL CAggregateContextMenu::DispatchCommand(UINT nSelectedID)
{
	if( nSelectedID>=m_InsertionMinID && nSelectedID<m_InsertionMaxID)
	{
		OnCustomItems(nSelectedID);
	}
	else
	{
		switch(nSelectedID)
		{
			case ID_EDIT_CUT:			OnEditCut();break;
			case ID_EDIT_COPY:			OnEditCopy();break;
			case ID_EDIT_COPYCLOSURE:	OnEditCopyClosure();break;
			case ID_EDIT_COPYSMART:		OnEditCopySmart();break;
			case ID_EDIT_PASTE:			OnEditPaste();break;
			case ID_EDIT_PASTESPECIAL_ASREFERENCE:			OnEditPasteSpecialAsReference();break;
			case ID_EDIT_PASTESPECIAL_ASSUBTYPE:			OnEditPasteSpecialAsSubtype();break;
			case ID_EDIT_PASTESPECIAL_ASINSTANCE:			OnEditPasteSpecialAsInstance();break;
			case ID_EDIT_PASTECLOSURE:	OnEditPasteClosure();break;
			case ID_EDIT_PASTESPECIAL_SMART_ADDITIVE: OnEditPasteClosure();break;
			case ID_EDIT_PASTESPECIAL_SMART_MERGE: OnEditPasteClosure(true);break;
			case ID_POPUP_EDIT_DELETE:	OnEditDelete();break;

			case ID_POPUP_ATTACH_LIBRARY: OnAttachLibrary();break;
			case ID_POPUP_REFRESH_LIBRARY: OnRefreshLibrary();break;
			case ID_POPUP_LIBRARY_DEPENDENCIES: OnLibraryDependencies();break;
			case ID_POPUP_LIBRARY_AMBIGUITYCHK: OnLibraryAmbiguityChk();break;
			case ID_USERS_SOURCECONTROLSTATUS: OnMFSourceControlUpdate();break;
			case ID_USERS_ACTIVE: OnUsersActive(); break;
			case ID_USERS_OWNER: OnUsersOwner(); break;

			case ID_POPUP_SORT_CONSTRAINTS_CHECKALL: OnCheckAllConstraint();break;
			case ID_POPUP_CONSTRAINTS_CHECK:		 OnCheckConstraint();break;
			case ID_POPUP_INTERPRET:	OnInterpret();break;

			case ID_POPUP_SORT_NAME:	OnSortName();break;
			case ID_POPUP_SORT_TYPE:	OnSortType();break;	
			case ID_POPUP_SORT_CREATION:OnSortCreation();break;
			
			case ID_POPUP_PROPERTIES:	OnProperties();break;
			case ID_POPUP_ATTRIBUTES:	OnAttributes();break;
			case ID_POPUP_PREFERENCES:	OnPreferences();break;
			case ID_POPUP_REGISTRY:		OnRegistry();break;
			
			case ID_POPUP_DISPLAYOPTIONS: OnOptions();break;
			case ID_POPUP_HELP:			OnHelp();break;
			case ID_POPUP_SHOWINPARENT: OnShowInParent();break;
			case ID_POPUP_FOLLOWREF:    OnFollowRef();break;
			case ID_POPUP_READONLY:     OnReadOnly( true);break;
			case ID_POPUP_READWRITE:    OnReadOnly( false);break;
			
				
			default:
				return FALSE;
		}
	}
	return TRUE;
}


void CAggregateContextMenu::OnCustomItems(UINT nID)
{
	UINT nCmd=nID-m_InsertionMinID;

	/*********************  Custom insertions  ************************/
	// The meta object of the insertable items has been stored in the
	// m_MenuItem2MetaObjArray. The the index of the selected meta
	// can be found in the array at the position nCmd 
	if(m_nSelectionCount==1)
	{
		m_pParent->m_bIsBrowserCreatedNewObject=TRUE;
		HTREEITEM hItem=m_pParent->m_TreeAggregate.GetSelectedItem();
		CAggregateMgaObjectProxy MgaObjectProxy;

		if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectProxy(hItem,MgaObjectProxy))
		{
			// The selected type to be parent is of the type...
			switch(MgaObjectProxy.m_TypeInfo)
			{
				case OBJTYPE_FOLDER:
				{
					CComQIPtr<IMgaMetaFolder> ccpMetaFolder((CComPtr<IMgaMetaBase>)m_MenuItem2MetaObjArray.GetAt(nCmd));
					if(ccpMetaFolder) // The type to create is a folder
					{
						CComQIPtr<IMgaFolder>ccpParentFolder(MgaObjectProxy.m_pMgaObject);
						if(ccpParentFolder)
						{
							CComPtr<IMgaFolder> ccpNewFolder;

							CComBSTR bszDispName;
							ccpMetaFolder->get_DisplayedName(&bszDispName);
							bszDispName = CString(bszDispName);
							

							// Starting transaction
							CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
							CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
							pMgaContext->BeginTransaction(false);
							COMTHROW( ccpParentFolder->CreateFolder(ccpMetaFolder,&ccpNewFolder) );
							COMTHROW( ccpNewFolder->put_Name(bszDispName) );
							// Ending transaction
							pMgaContext->CommitTransaction();
							
						}
					}
					else // The type to create is an FCO
					{
						CComQIPtr<IMgaMetaFCO> ccpMetaFCO((CComPtr<IMgaMetaBase>)m_MenuItem2MetaObjArray.GetAt(nCmd));
						if(!ccpMetaFCO)break;

						CComQIPtr<IMgaFolder>ccpParentFolder(MgaObjectProxy.m_pMgaObject);
						if(ccpParentFolder)
						{
							CComPtr<IMgaFCO> ccpNewFCO;

							// Setting name to the newly created object
							CComBSTR bszDispName;
							ccpMetaFCO->get_DisplayedName(&bszDispName);
							bszDispName = CString(bszDispName);
							
							// Starting transaction
							CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
							CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
							pMgaContext->BeginTransaction(false);
						
							// Creating and naming the new FCO
							COMTHROW( ccpParentFolder->CreateRootObject(ccpMetaFCO,&ccpNewFCO) );
							COMTHROW( ccpNewFCO->put_Name(bszDispName) );
							// Ending transaction
							pMgaContext->CommitTransaction();
						}

					}		
				}break;

				case OBJTYPE_MODEL:
				{
					CComQIPtr<IMgaMetaRole> ccpMetaRole((CComPtr<IMgaMetaBase>)m_MenuItem2MetaObjArray.GetAt(nCmd));
					if(!ccpMetaRole)break;

					CComQIPtr<IMgaModel>ccpParentModel(MgaObjectProxy.m_pMgaObject);
					if(ccpParentModel)
					{
						CComPtr<IMgaFCO> ccpNewFCO;

						// Starting transaction
						CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
						CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
						pMgaContext->BeginTransaction(false);

						COMTHROW( ccpParentModel->CreateChildObject(ccpMetaRole,&ccpNewFCO) );
						// Setting name to the newly created object
						if (ccpMetaRole->Name == ccpMetaRole->Kind->Name)
						{
							ccpNewFCO->Name = ccpMetaRole->Kind->DisplayedName;
						}
						else
						{
							ccpNewFCO->Name = ccpMetaRole->DisplayedName;
						}

						// Ending transaction
						pMgaContext->CommitTransaction();
					}

				}break;
			}
		}
	}

}

void CAggregateContextMenu::OnEditCut()
{

}

void CAggregateContextMenu::OnEditCopy()
{

	CPoint ptDummy;
	m_pParent->DoCopy();
}

void CAggregateContextMenu::OnEditCopyClosure()
{
	CPoint ptDummy;
	m_pParent->DoCopyClosure();
}

void CAggregateContextMenu::OnEditCopySmart()
{
	CPoint ptDummy;
	m_pParent->DoCopySmart();
}

void CAggregateContextMenu::OnEditPaste()
{
	COleDataObject OleDataObject;

	if (OleDataObject.AttachClipboard() == FALSE) 
	{
		m_pParent->MessageBox(_T("Cannot recover data from the clipboard."),_T("Paste Error"),MB_ICONERROR);
	}
	else
	{
		if(!m_pParent->m_TreeAggregate.DoDrop(DRAGOP_COPY,&OleDataObject,CPoint(0,0)))
		{
			m_pParent->MessageBox(_T("Cannot paste data from the clipboard. Please select valid target item."),_T("Paste Error"),MB_ICONERROR);
		}
	}
}

void CAggregateContextMenu::OnEditPasteSpecialAsReference()
{
	COleDataObject OleDataObject;

	if (OleDataObject.AttachClipboard() == FALSE) 
	{
		m_pParent->MessageBox(_T("Cannot recover data from the clipboard."),_T("Paste Error"),MB_ICONERROR);
	}
	else
	{
		if(!m_pParent->m_TreeAggregate.DoDrop(DRAGOP_REFERENCE,&OleDataObject,CPoint(0,0)))
		{
			m_pParent->MessageBox(_T("Cannot paste data from the clipboard. Please select valid target item."),_T("Paste Error"),MB_ICONERROR);
		}
	}
}

void CAggregateContextMenu::OnEditPasteSpecialAsSubtype()
{
	COleDataObject OleDataObject;

	if (OleDataObject.AttachClipboard() == FALSE) 
	{
		m_pParent->MessageBox(_T("Cannot recover data from the clipboard."),_T("Paste Error"),MB_ICONERROR);
	}
	else
	{
		if(!m_pParent->m_TreeAggregate.DoDrop(DRAGOP_SUBTYPE,&OleDataObject,CPoint(0,0)))
		{
			m_pParent->MessageBox(_T("Cannot paste data from the clipboard. Please select valid target item."),_T("Paste Error"),MB_ICONERROR);
		}
	}
}

void CAggregateContextMenu::OnEditPasteSpecialAsInstance()
{
	COleDataObject OleDataObject;

	if (OleDataObject.AttachClipboard() == FALSE) 
	{
		m_pParent->MessageBox(_T("Cannot recover data from the clipboard."),_T("Paste Error"),MB_ICONERROR);
	}
	else
	{
		if(!m_pParent->m_TreeAggregate.DoDrop(DRAGOP_INSTANCE,&OleDataObject,CPoint(0,0)))
		{
			m_pParent->MessageBox(_T("Cannot paste data from the clipboard. Please select valid target item."),_T("Paste Error"),MB_ICONERROR);
		}
	}
}

void CAggregateContextMenu::OnEditPasteClosure( bool merge)
{
	COleDataObject OleDataObject;

	if (OleDataObject.AttachClipboard() == FALSE) 
	{
		m_pParent->MessageBox(_T("Cannot recover data from the clipboard."),_T("Paste Error"),MB_ICONERROR);
	}
	else
	{
		if(!m_pParent->m_TreeAggregate.DoDropWithoutChecking( merge?DRAGOP_CLOSURE_MERGE:DRAGOP_CLOSURE, &OleDataObject,CPoint(0,0)))
  		{
  			m_pParent->MessageBox(_T("Cannot paste data from the clipboard. Please select valid target item."),_T("Paste Error"),MB_ICONERROR);
		}
	}
}
void CAggregateContextMenu::OnEditDelete()
{
    bool detach_answered = false;
    CMgaContext * pMgaContext = NULL;
    try
    {
	    HTREEITEM hSelItem=m_pParent->m_TreeAggregate.GetFirstSelectedItem();
	    LPUNKNOWN pUnknown;

	    // Starting transaction
	    CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	    pMgaContext = &pApp->m_CurrentProject.m_MgaContext;
	    pMgaContext->BeginTransaction(TRANSACTION_NON_NESTED);

	    while(hSelItem)
	    {
		    /////////////////////////////////////////////////////////////////////////
		    // The MGA layer does not send consistent messages with its actual state
		    // So we collapse the item to be deleted, to avoid the inconsistency
		    // caused by the order of deleting the child items and the parents in MGA
			if( m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hSelItem,pUnknown))
			{
				bool has_dependency = true;
				// deletion of libraries which have some dependency
				// (either _includes or _includedBy) is confirmed
				if( m_pParent->m_TreeAggregate.IsLibrary(pUnknown, &has_dependency) && has_dependency)
				{
					CComQIPtr<IMgaFolder> ccpMgaFolder(pUnknown);
					bool deld = m_pParent->askUserAndDeleteLibrary( ccpMgaFolder);
					if( deld) m_pParent->m_TreeAggregate.DeleteAllChildren(hSelItem);
				}
				else
				{
					m_pParent->m_TreeAggregate.DeleteAllChildren(hSelItem);
					CComQIPtr<IMgaObject> ccpMgaObject(pUnknown);
					// throws E_MGA_MUST_ABORT if user selects CANCEL
					detach_answered = m_pParent->askUserAndDetach( ccpMgaObject);
					// Deleting object
					COMTHROW(ccpMgaObject->DestroyObject());
				}
			}
		    hSelItem=m_pParent->m_TreeAggregate.GetNextSelectedItem(hSelItem);
	    }
	    // Ending transaction
	    pMgaContext->CommitTransaction();
    }
    catch(hresult_exception &)
    {
        pMgaContext->AbortTransaction();
    }
    if( detach_answered)
        m_pParent->Refresh();
}

void CAggregateContextMenu::OnCheckAllConstraint()
{
	CGMEActiveBrowserApp* pApp = (CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext = &pApp->m_CurrentProject.m_MgaContext;

	CComPtr<IMgaComponentEx> constrMgr;
	if (pMgaContext)
		constrMgr = pMgaContext->FindConstraintManager();
	ASSERT(constrMgr);
	if (constrMgr)	// Invoke constraint manager
		COMTHROW(constrMgr->ObjectsInvokeEx(pMgaContext->m_ccpProject, NULL, NULL, GME_BROWSER_START));
}

void CAggregateContextMenu::OnInterpret()
{

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	HTREEITEM hSelItem=NULL;
	LPUNKNOWN pUnknown=NULL;

	// Collection for the selected FCOs
	CComPtr<IMgaFCOs> ccpSelFCOs;
	COMTHROW(ccpSelFCOs.CoCreateInstance(L"Mga.MgaFCOs"));

	
	/* Iterating through the selected items in the tree control creating an FCO collection*/
	// Starting transaction
	pMgaContext->BeginTransaction();
	// Iteration
	hSelItem=m_pParent->m_TreeAggregate.GetFirstSelectedItem();
	while(hSelItem)
	{

		if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hSelItem,pUnknown))
		{
			CComQIPtr<IMgaFCO> ccpMgaFCO(pUnknown);
			if(ccpMgaFCO!=NULL) // If the selected object is FCO, add to the collection
			{
				COMTHROW(ccpSelFCOs->Append(ccpMgaFCO));
				// BUGFIX by PETER
				// COMTHROW(ccpMgaFCO->CreateCollection(&ccpSelFCOs));
			}
		}

		hSelItem=m_pParent->m_TreeAggregate.GetNextSelectedItem(hSelItem);
	}
	// Ending transaction
	pMgaContext->CommitTransaction();


	// Creating launcher
	CComObjPtr<IMgaLauncher> ccpMgaLauncher;
	COMTHROW( ccpMgaLauncher.CoCreateInstance(L"Mga.MgaLauncher") );
	
	// Calling the interpreter via MGA
	// Disable the DCOM wait dialogs: if interpreters want them, they can do it themselves; but if they don't want them, they need to link to GME's mfc1xxu.dll
	COleMessageFilter* messageFilter = AfxOleGetMessageFilter();
	std::shared_ptr<COleMessageFilter> busyRestore;
	std::shared_ptr<COleMessageFilter> notRespondingRestore;
	if (messageFilter)
	{
		messageFilter->EnableBusyDialog(FALSE);
		messageFilter->EnableNotRespondingDialog(FALSE);
		busyRestore = std::shared_ptr<COleMessageFilter>(messageFilter, [](COleMessageFilter* filter){ filter->EnableBusyDialog(TRUE); } );
		notRespondingRestore = std::shared_ptr<COleMessageFilter>(messageFilter, [](COleMessageFilter* filter){ filter->EnableNotRespondingDialog(TRUE); } );
	}
	COMTHROW( ccpMgaLauncher->RunComponent(NULL,pMgaContext->m_ccpProject, NULL,
				ccpSelFCOs,GME_BROWSER_START) );
}

void CAggregateContextMenu::OnSortName()
{
	m_pParent->m_Options.m_soSortOptions=SORT_BYNAME;
	m_pParent->m_TreeAggregate.SortItems();
}

void CAggregateContextMenu::OnSortType()
{
	m_pParent->m_Options.m_soSortOptions=SORT_BYTYPE;
	m_pParent->m_TreeAggregate.SortItems();
}

void CAggregateContextMenu::OnSortCreation()
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
	
	m_pParent->m_Options.m_soSortOptions=SORT_BYCREATION;

	// Starting transaction
	pMgaContext->BeginTransaction();
	m_pParent->m_TreeAggregate.SortItems();
	// Ending transaction
	pMgaContext->CommitTransaction();
}

void CAggregateContextMenu::OnProperties()
{	
	
	HTREEITEM hItem=m_pParent->m_TreeAggregate.GetSelectedItem();
	LPUNKNOWN pUnknown=NULL;

	if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		// Firing the event
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		pApp->GetCtrl()->FireShowProperties();
	}
}

void CAggregateContextMenu::OnAttributes()
{
	HTREEITEM hItem=m_pParent->m_TreeAggregate.GetSelectedItem();

	LPUNKNOWN pUnknown=NULL;

	if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		TRACE(m_pParent->m_TreeAggregate.GetItemText(hItem));

		// Firing the event
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		pApp->GetCtrl()->FireShowAttributes(pUnknown);
	}
}

void CAggregateContextMenu::OnPreferences()
{
	HTREEITEM hItem=m_pParent->m_TreeAggregate.GetSelectedItem();
	LPUNKNOWN pUnknown=NULL;

	if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		TRACE(m_pParent->m_TreeAggregate.GetItemText(hItem));

		// Firing the event
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		pApp->GetCtrl()->FireShowPreferences(pUnknown);
	}
}

void CAggregateContextMenu::OnRegistry()
{
	HTREEITEM hItem=m_pParent->m_TreeAggregate.GetSelectedItem();
	LPUNKNOWN pUnknown=NULL;

	if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		CComQIPtr<IMgaObject> ccpMgaObject(pUnknown);
		if(ccpMgaObject!=NULL)
		{
			// Creating launcher
			CComObjPtr<IMgaLauncher> ccpMgaLauncher;
			COMTHROW( ccpMgaLauncher.CoCreateInstance(L"Mga.MgaLauncher") );

			CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
			CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
			
			// Starting transaction
			pMgaContext->BeginTransaction(false);
			COMTHROW( ccpMgaLauncher->RegistryBrowser(ccpMgaObject));
			// Ending transaction
			pMgaContext->CommitTransaction();
		}
	}
}


void CAggregateContextMenu::OnHelp()
{		
	HTREEITEM hItem=m_pParent->m_TreeAggregate.GetSelectedItem();
	LPUNKNOWN pUnknown=NULL;

	if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		CComQIPtr<IMgaObject> ccpMgaObject(pUnknown);
		if(ccpMgaObject!=NULL)
		{
			// Creating launcher
			CComObjPtr<IMgaLauncher> ccpMgaLauncher;
			COMTHROW( ccpMgaLauncher.CoCreateInstance(L"Mga.MgaLauncher") );

			CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
			CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
			
			// Starting transaction
			pMgaContext->BeginTransaction();
			COMTHROW( ccpMgaLauncher->ShowHelp(ccpMgaObject));
			// Ending transaction
			pMgaContext->CommitTransaction();

		}
	}
}

void CAggregateContextMenu::CreateForSingleItem()
{
	VERIFY(LoadMenu(CG_IDR_POPUP_AGGREGATE_PROPERTY_PAGE_SINGLE_ITEM));

	// Check constraint
	CGMEActiveBrowserApp* pApp = (CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext = &pApp->m_CurrentProject.m_MgaContext;
	CComPtr<IMgaComponentEx> constrMgr = pMgaContext->FindConstraintManager();
	if (!constrMgr)
		EnableMenuItem(ID_POPUP_CONSTRAINTS_CHECK, MF_GRAYED);

	// Setting Paste
	COleDataObject OleDataObj;
	BOOL bEnable =  OleDataObj.AttachClipboard() &&
		( CGMEDataSource::IsGmeNativeDataAvailable(&OleDataObj,pMgaContext->m_ccpProject) ||
		  CGMEDataSource::IsXMLDataAvailable(&OleDataObj) );
	if(bEnable)
	{
		EnableMenuItem(ID_EDIT_PASTE,MF_ENABLED);
	}
	else
	{
		EnableMenuItem(ID_EDIT_PASTE,MF_GRAYED);
	}


	HTREEITEM hItem=m_pParent->m_TreeAggregate.GetSelectedItem();
	CAggregateMgaObjectProxy MgaObjectProxy;

	if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectProxy(hItem,MgaObjectProxy))
	{
		{ // Remove Multi-user submenu if not an mgx
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
		CComBSTR connStr;
		if (SUCCEEDED(pMgaContext->m_ccpProject->get_ProjectConnStr(&connStr))) {
			if (wmemcmp(L"MGX=", connStr.m_str, 4) != 0) {
				GetSubMenu(0)->DeleteMenu(21, MF_BYPOSITION);
			}
		}
		}

		int insertionsSeparatorPosition = 6;
		switch(MgaObjectProxy.m_TypeInfo)
		{
			case OBJTYPE_FOLDER:
			{
				// Folder is not FCO - limited operations				
				DeleteMenu(ID_POPUP_PREFERENCES, MF_BYCOMMAND);

				if( hItem == m_pParent->m_TreeAggregate.GetRootItem()) // RootFolder
				{
					ModifyMenu( ID_POPUP_REFRESH_LIBRARY, MF_BYCOMMAND, ID_POPUP_LIBRARY_AMBIGUITYCHK, _T("Chec&k GUID ambiguity..."));
					ModifyMenu( ID_POPUP_ATTRIBUTES, MF_BYCOMMAND|MF_STRING, ID_POPUP_ATTRIBUTES, _T("&Project Preferences"));
				}
				else
				{
					DeleteMenu( ID_POPUP_ATTRIBUTES, MF_BYCOMMAND);    // no Attributes

					// it is a regular folder or library folder
					bool has_dependency;
					if( m_pParent->m_TreeAggregate.IsLibrary(MgaObjectProxy.m_pMgaObject, &has_dependency))
					{   // library folder
						if( has_dependency)
							InsertMenu( ID_POPUP_REFRESH_LIBRARY, MF_BYCOMMAND|MF_STRING, ID_POPUP_LIBRARY_DEPENDENCIES, _T("D&ependencies..."));
						
						DeleteMenu( ID_POPUP_ATTACH_LIBRARY, MF_BYCOMMAND);
					}
					else
					{   // regular folder
						DeleteMenu( ID_POPUP_REFRESH_LIBRARY, MF_BYCOMMAND);
						DeleteMenu( ID_POPUP_ATTACH_LIBRARY, MF_BYCOMMAND);
					}
				}



				
				EnableMenuItem(ID_POPUP_CONSTRAINTS_CHECK,MF_GRAYED);

				SetFolderChildren(MgaObjectProxy.m_pMgaObject);
				
				// Deleting INSERTIONS menu item
				CMenu* pPopup=GetSubMenu(0);
				if(m_InsertionMinID==m_InsertionMaxID)	 // There were no insertions
				{										 // deleting INSERTIONS
					pPopup->DeleteMenu(ID_POPUP_INSERTIONS,MF_BYCOMMAND);
					pPopup->DeleteMenu(insertionsSeparatorPosition, MF_BYPOSITION); // deleting separator
				}
				else
				{
					// deleting INSERTIONS
					pPopup->DeleteMenu(ID_POPUP_INSERTIONS,MF_BYCOMMAND);
				}
				
				// a folder can't be viewed in its parent
				// EnableMenuItem(ID_POPUP_SHOWINPARENT,MF_GRAYED); 
				pPopup->DeleteMenu( ID_POPUP_SHOWINPARENT, MF_BYCOMMAND);

			}break;

			case OBJTYPE_MODEL:
				{
					SetModelChildren(MgaObjectProxy.m_pMgaObject);
					
					// Deleting INSERTIONS menu item
					CMenu* pPopup=GetSubMenu(0);
					if(m_InsertionMinID==m_InsertionMaxID)
					{	
						pPopup->DeleteMenu(ID_POPUP_INSERTIONS,MF_BYCOMMAND); // deleting INSERTIONS
						pPopup->DeleteMenu(insertionsSeparatorPosition, MF_BYPOSITION); // deleting separator
					}
					else
					{
						// deleting INSERTIONS
						pPopup->DeleteMenu(ID_POPUP_INSERTIONS,MF_BYCOMMAND);
					}

					DeleteMenu( ID_POPUP_REFRESH_LIBRARY, MF_BYCOMMAND);
					DeleteMenu( ID_POPUP_ATTACH_LIBRARY, MF_BYCOMMAND);

				}break;

			case OBJTYPE_REFERENCE:
				{
					InsertMenu( ID_POPUP_REFRESH_LIBRARY, MF_BYCOMMAND, ID_POPUP_FOLLOWREF, _T("&Follow Reference"));
					insertionsSeparatorPosition++;
				}// no break here!
			default:
				{
					// Delete "INSERTIONS" item plus one separator
					CMenu* pPopup=GetSubMenu(0);
					pPopup->DeleteMenu(ID_POPUP_INSERTIONS,MF_BYCOMMAND);
					pPopup->DeleteMenu(insertionsSeparatorPosition, MF_BYPOSITION);
					
					DeleteMenu( ID_POPUP_REFRESH_LIBRARY, MF_BYCOMMAND);
					DeleteMenu( ID_POPUP_ATTACH_LIBRARY, MF_BYCOMMAND);
				}
		}

		// determining whether to enable/disable the 'ShowInParent' menuitem
		if( OBJTYPE_FOLDER != MgaObjectProxy.m_TypeInfo) // for folders we already deleted the feature
		{
			HTREEITEM hItemsParent = m_pParent->m_TreeAggregate.GetParentItem( hItem);
			CAggregateMgaObjectProxy MgaParentProxy; // if parent is a model, then enable
			if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectProxy(hItemsParent,MgaParentProxy))
				EnableMenuItem( ID_POPUP_SHOWINPARENT, OBJTYPE_MODEL == MgaParentProxy.m_TypeInfo?MF_ENABLED:MF_GRAYED);
		}
	}
}

/*************** Adding dynamic submenus to the selected folder *************/
// Mmodifies the list named m_MenuItem2MetaObjArray, and the allowed 
// insertions will be stored in the same order as they are inserted
// into the menu
void CAggregateContextMenu::SetFolderChildren(LPUNKNOWN pUnknown)
{

	CComQIPtr<IMgaFolder> ccpMgaFolder(pUnknown);

	// Getting the meta of the selected folder
	CComPtr<IMgaMetaFolder> ccpMgaMetaFolder;			
	// Starting transaction
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;	
	pMgaContext->BeginTransaction();
	COMTHROW( ccpMgaFolder->get_MetaFolder(&ccpMgaMetaFolder) );
	// Ending transaction
	pMgaContext->CommitTransaction();
		
	
	/**** Getting legal child folders and iterating through them ****/
	CComPtr<IMgaMetaFolders> ccpMetaFolders;
	COMTHROW( ccpMgaMetaFolder->get_LegalChildFolders(&ccpMetaFolders) );

	CMenu mnuInsertFolder;
	VERIFY( mnuInsertFolder.CreatePopupMenu() );

	MGACOLL_ITERATE(IMgaMetaFolder, ccpMetaFolders) 
	{				
		// Getting displayed name
		CComBSTR bszDispName;		
		COMTHROW( MGACOLL_ITER->get_DisplayedName(&bszDispName) );
		// Add displayed name to the menu
		mnuInsertFolder.AppendMenu(MF_STRING,m_InsertionMaxID++,CString(bszDispName));
		// Add ptr to the list
		CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);
		m_MenuItem2MetaObjArray.Add(ADAPT(ccpMetaBase));
	}MGACOLL_ITERATE_END;
	
	// If the menu is not empty add to the popup, otherwise the destructor
	// will automatically destroy it
	if(mnuInsertFolder.GetMenuItemCount()!=NULL) 
	{
		InsertMenu(ID_POPUP_INSERTIONS,MF_POPUP|MF_STRING,(UINT_PTR)mnuInsertFolder.GetSafeHmenu(),_T("Insert &Folder"));
		mnuInsertFolder.Detach();
	}

	/****    Searching for allowed child FCOs ****/
	// Creating submenus
	CMenu mnuInsertAtom;
	CMenu mnuInsertModel;
	CMenu mnuInsertReference;
	CMenu mnuInsertSet;
	// Initialize submenus
	VERIFY( mnuInsertAtom.CreatePopupMenu() );
	VERIFY( mnuInsertModel.CreatePopupMenu() );
	VERIFY( mnuInsertReference.CreatePopupMenu() );
	VERIFY( mnuInsertSet.CreatePopupMenu() );
	
	// Getting legal children
	CComPtr<IMgaMetaFCOs> ccpChildFCOs;
	COMTHROW( ccpMgaMetaFolder->get_LegalRootObjects(&ccpChildFCOs) );

	// Iterate through the children FCO list
	MGACOLL_ITERATE(IMgaMetaFCO, ccpChildFCOs) 
	{
		// Get displayed name
		CComBSTR bszDispName;		
		COMTHROW( MGACOLL_ITER->get_DisplayedName(&bszDispName) );
		
		objtype_enum otChildType = OBJTYPE_NULL;
		COMTHROW( MGACOLL_ITER->get_ObjType(&otChildType) );

		switch(otChildType)
		{
			case OBJTYPE_ATOM:
				{
					mnuInsertAtom.AppendMenu(MF_STRING,m_InsertionMaxID++,
														CString(bszDispName));
					// Add ptr to the list
					CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);
					m_MenuItem2MetaObjArray.Add(ADAPT(ccpMetaBase));
				}break;
			case OBJTYPE_MODEL:
				{
					mnuInsertModel.AppendMenu(MF_STRING,m_InsertionMaxID++,
														CString(bszDispName));
					// Add ptr to the list
					CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);
					m_MenuItem2MetaObjArray.Add(ADAPT(ccpMetaBase));					
				}break;
			case OBJTYPE_REFERENCE:
				{
					mnuInsertReference.AppendMenu(MF_STRING,m_InsertionMaxID++,
														CString(bszDispName));
					// Add ptr to the list
					CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);
					m_MenuItem2MetaObjArray.Add(ADAPT(ccpMetaBase));

				}break;
			case OBJTYPE_SET:
				{
					mnuInsertSet.AppendMenu(MF_STRING,m_InsertionMaxID++,
														CString(bszDispName));
					// Add ptr to the list
					CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);
					m_MenuItem2MetaObjArray.Add(ADAPT(ccpMetaBase));
				}break;
		}

	}MGACOLL_ITERATE_END;
	
	// Appending the submenus created above
	// If the menu is not empty add to the popup, otherwise the destructor
	// will automatically destroy it
	// Append Atom
	if(mnuInsertAtom.GetMenuItemCount()!=NULL) 
	{
		InsertMenu(ID_POPUP_INSERTIONS,MF_POPUP|MF_STRING,(UINT_PTR)mnuInsertAtom.GetSafeHmenu(),_T("Insert &Atom"));
		mnuInsertAtom.Detach();
	}
	
	// Append Model
	if(mnuInsertModel.GetMenuItemCount()!=NULL) 
	{
		InsertMenu(ID_POPUP_INSERTIONS,MF_POPUP|MF_STRING,(UINT_PTR)mnuInsertModel.GetSafeHmenu(),_T("Insert &Model"));
		mnuInsertModel.Detach();
	}

	// Append Reference
	if(mnuInsertReference.GetMenuItemCount()!=NULL) 
	{
		InsertMenu(ID_POPUP_INSERTIONS,MF_POPUP|MF_STRING,(UINT_PTR)mnuInsertReference.GetSafeHmenu(),_T("Insert &Reference"));
		mnuInsertReference.Detach();
	}

	// Append Set
	if(mnuInsertSet.GetMenuItemCount()!=NULL) 
	{
		InsertMenu(ID_POPUP_INSERTIONS,MF_POPUP|MF_STRING,(UINT_PTR)mnuInsertSet.GetSafeHmenu(),_T("Insert &Set"));
		mnuInsertSet.Detach();
	}

}



/*************** Adding dynamic submenus to the selected folder *************/
// Mmodifies the list named m_MenuItem2MetaObjArray, and the allowed 
// insertions will be stored in the same order as they are inserted
// into the menu

void CAggregateContextMenu::SetModelChildren(LPUNKNOWN pUnknown)
{
	CComQIPtr<IMgaModel> ccpMgaModel(pUnknown);

	// Getting the meta of the selected model
	CComPtr<IMgaMetaFCO> ccpMgaMetaFCO;			
	// Starting transaction
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;	
	pMgaContext->BeginTransaction();
	COMTHROW( ccpMgaModel->get_Meta(&ccpMgaMetaFCO) );
	// Ending transaction
	pMgaContext->CommitTransaction();
		
	// Converting meta FCO to metamodel
	CComQIPtr<IMgaMetaModel> ccpMgaMetaModel(ccpMgaMetaFCO);
	if(!ccpMgaMetaModel)return;

	/****    Searching for allowed child FCOs ****/
	// Creating submenus
	CMenu mnuInsertAtom;
	CMenu mnuInsertModel;
	CMenu mnuInsertReference;
	CMenu mnuInsertSet;
	// Initialize submenus
	VERIFY( mnuInsertAtom.CreatePopupMenu() );
	VERIFY( mnuInsertModel.CreatePopupMenu() );
	VERIFY( mnuInsertReference.CreatePopupMenu() );
	VERIFY( mnuInsertSet.CreatePopupMenu() );
	
	// Getting role list
	CComPtr<IMgaMetaRoles> ccpMetaRoles;
	COMTHROW( ccpMgaMetaModel->get_Roles(&ccpMetaRoles) );

	// Iterate through the role list
	MGACOLL_ITERATE(IMgaMetaRole, ccpMetaRoles) 
	{
		// Get displayed name
		CComBSTR bszDispName;

		CComPtr<IMgaMetaFCO> kind;
		COMTHROW(MGACOLL_ITER->get_Kind(&kind));
		CComBSTR bstrKindName;
		COMTHROW(kind->get_Name(&bstrKindName));
		CComBSTR bstrRoleName;
		COMTHROW(MGACOLL_ITER->get_Name(&bstrRoleName));
		if (bstrKindName == bstrRoleName) {
			COMTHROW(kind->get_DisplayedName(&bszDispName));
		}
		else {
			bszDispName = bstrRoleName;
		}

		
		// Getting object type
		CComPtr<IMgaMetaFCO> ccpMetaFCO;
		COMTHROW( MGACOLL_ITER->get_Kind(&ccpMetaFCO));
		objtype_enum otChildType = OBJTYPE_NULL;
		if(ccpMetaFCO)
		{			
			COMTHROW( ccpMetaFCO->get_ObjType(&otChildType) );
		}

		switch(otChildType)
		{
			case OBJTYPE_ATOM:
				{
					mnuInsertAtom.AppendMenu(MF_STRING,m_InsertionMaxID++,
														CString(bszDispName));
					// Add ptr to the list
					CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);
					m_MenuItem2MetaObjArray.Add(ADAPT(ccpMetaBase));


				}break;
			case OBJTYPE_MODEL:
				{
					mnuInsertModel.AppendMenu(MF_STRING,m_InsertionMaxID++,
														CString(bszDispName));
					// Add ptr to the list
					CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);
					m_MenuItem2MetaObjArray.Add(ADAPT(ccpMetaBase));

				}break;
			case OBJTYPE_REFERENCE:
				{
					mnuInsertReference.AppendMenu(MF_STRING,m_InsertionMaxID++,
														CString(bszDispName));
					// Add ptr to the list
					CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);
					m_MenuItem2MetaObjArray.Add(ADAPT(ccpMetaBase));

				}break;
			case OBJTYPE_SET:
				{
					mnuInsertSet.AppendMenu(MF_STRING,m_InsertionMaxID++,
														CString(bszDispName));
					// Add ptr to the list
					CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);
					m_MenuItem2MetaObjArray.Add(ADAPT(ccpMetaBase));

				}break;
		}

	}MGACOLL_ITERATE_END;
	
	// Appending the submenus created above
	// If the menu is not empty add to the popup, otherwise the destructor
	// will automatically destroy it
	// Append Atom
	if(mnuInsertAtom.GetMenuItemCount()!=NULL) 
	{
		InsertMenu(ID_POPUP_INSERTIONS,MF_POPUP|MF_STRING,(UINT_PTR)mnuInsertAtom.GetSafeHmenu(),_T("Insert &Atom"));
		mnuInsertAtom.Detach();
	}
	
	// Append Model
	if(mnuInsertModel.GetMenuItemCount()!=NULL) 
	{
		InsertMenu(ID_POPUP_INSERTIONS,MF_POPUP|MF_STRING,(UINT_PTR)mnuInsertModel.GetSafeHmenu(),_T("Insert &Model"));
		mnuInsertModel.Detach();
	}

	// Append Reference
	if(mnuInsertReference.GetMenuItemCount()!=NULL) 
	{
		InsertMenu(ID_POPUP_INSERTIONS,MF_POPUP|MF_STRING,(UINT_PTR)mnuInsertReference.GetSafeHmenu(),_T("Insert &Reference"));
		mnuInsertReference.Detach();
	}

	// Append Set
	if(mnuInsertSet.GetMenuItemCount()!=NULL) 
	{
		InsertMenu(ID_POPUP_INSERTIONS,MF_POPUP|MF_STRING,(UINT_PTR)mnuInsertSet.GetSafeHmenu(),_T("Insert &Set"));
		mnuInsertSet.Detach();
	}
}

void CAggregateContextMenu::CreateForAll()
{
	/* Initialization for all type of context menus */

	// Edit - Undo/Redo functions
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;	
	short nRedoSize,nUndoSize;
	COMTHROW(pMgaContext->m_ccpProject->UndoRedoSize(&nUndoSize,&nRedoSize));

	// Check all constraint
	CComPtr<IMgaComponentEx> constrMgr = pMgaContext->FindConstraintManager();
	if (!constrMgr)
		EnableMenuItem(ID_POPUP_SORT_CONSTRAINTS_CHECKALL, MF_GRAYED);

	// Sort options
	CheckMenuRadioItem(ID_POPUP_SORT_NAME,
			ID_POPUP_SORT_CREATION,
			ID_POPUP_SORT_NAME+m_pParent->m_Options.m_soSortOptions,
			MF_BYCOMMAND
			);

}

void CAggregateContextMenu::OnCheckConstraint()
{
	CGMEActiveBrowserApp* pApp = (CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext = &pApp->m_CurrentProject.m_MgaContext;

	CComPtr<IMgaComponentEx> constrMgr;
	if (pMgaContext)
		constrMgr = pMgaContext->FindConstraintManager();
	if (!constrMgr)
		return;

	HTREEITEM hItem = m_pParent->m_TreeAggregate.GetSelectedItem();
	LPUNKNOWN pUnknown = NULL;

	if (m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem, pUnknown))
	{
		CComQIPtr<IMgaObject> ccpMgaObject(pUnknown);
		if (ccpMgaObject != NULL)
		{
			// Invoke constraint manager
			COMTHROW(constrMgr->ObjectsInvokeEx(pMgaContext->m_ccpProject, ccpMgaObject, NULL, GME_BROWSER_START));
		}
	}
}

void CAggregateContextMenu::OnOptions()
{
	CAggregateOptionsDlg dlg(m_pParent);

	if(dlg.DoModal()==IDOK)
	{
		if(dlg.m_bIsRefreshNeeded)
		{
			m_pParent->Refresh();
		}
		if(dlg.m_bIsResortNeeded)
		{
			if(m_pParent->m_Options.m_soSortOptions==SORT_BYCREATION)
			{
				OnSortCreation();
			}
			else 
			{
				m_pParent->m_TreeAggregate.SortItems();
			}
			
		}

	}
}

void CAggregateContextMenu::OnAttachLibrary()
{
	m_pParent->AttachLibrary();

}

void CAggregateContextMenu::OnRefreshLibrary()
{

	m_pParent->RefreshLibrary();
}

void CAggregateContextMenu::OnLibraryDependencies()
{
	m_pParent->LibraryDependencies();
}

void CAggregateContextMenu::OnLibraryAmbiguityChk()
{
	m_pParent->LibraryAmbiguityChk();
}

void CAggregateContextMenu::OnMFSourceControlUpdate()
{
	HTREEITEM hItem=m_pParent->m_TreeAggregate.GetSelectedItem();
	LPUNKNOWN pUnknown=NULL;

	if( hItem == m_pParent->m_TreeAggregate.GetRootItem()) // RootFolder
	{
		MSGTRY 
		{
			m_pParent->ProjectSourceControlUpdate(0);
		}
		MSGCATCH(_T("Error while updating source control info for the project!"),;)
	}
	else if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		CComQIPtr<IMgaFolder> ccpMgaFolder( pUnknown);
		CComQIPtr<IMgaModel>  ccpMgaModel(  pUnknown);
		CComBSTR              id;

		// preparing for a transaction
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

		MSGTRY 
		{
			// Starting transaction
			pMgaContext->BeginTransaction(true); // it is readonly

			if( ccpMgaModel)
				COMTHROW( ccpMgaModel->get_ID( &id));
			else if( ccpMgaFolder)
				COMTHROW( ccpMgaFolder->get_ID( &id));

			// Ending transaction
			pMgaContext->CommitTransaction();

		} 
		MSGCATCH(_T("Error while updating source control info!"),pMgaContext->AbortTransaction();)

		MSGTRY 
		{
			// it will open and commit its own transaction
			m_pParent->ProjectSourceControlUpdate( id);
		}
		MSGCATCH(_T("Error while updating source control info!"),;)

		//if( ccpMgaModel || ccpMgaFolder)
		//	m_pParent->Refresh();
	}
}

void CAggregateContextMenu::OnUsersActive()
{
	MSGTRY 
	{
		// it will open and commit its own transaction
		m_pParent->SourceControlActiveUsers();
	} 
	MSGCATCH(_T("Error while updating source control info!"),;)
}

void CAggregateContextMenu::OnUsersOwner()
{
	HTREEITEM hItem=m_pParent->m_TreeAggregate.GetSelectedItem();
	LPUNKNOWN pUnknown=NULL;

	if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		CComQIPtr<IMgaFolder> ccpMgaFolder( pUnknown);
		CComQIPtr<IMgaModel>  ccpMgaModel(  pUnknown);
		CComBSTR              id;

		// preparing for a transaction
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;


		MSGTRY 
		{
			// Starting transaction
			pMgaContext->BeginTransaction(true); // it is readonly

			if( ccpMgaModel)
				COMTHROW( ccpMgaModel->get_ID( &id));
			else if( ccpMgaFolder)
				COMTHROW( ccpMgaFolder->get_ID( &id));

			// Ending transaction
			pMgaContext->CommitTransaction();
		} 
		MSGCATCH(_T("Error while updating source control info!"),pMgaContext->AbortTransaction();)

		MSGTRY
		{
			// it will open and commit its own transaction
			m_pParent->SourceControlObjectOwner( id);
		}
		MSGCATCH(_T("Error while updating source control info!"),;)


		//if( ccpMgaModel || ccpMgaFolder)
		//	m_pParent->Refresh();
	}
}

// ShowInParentMgaObject event fired also when Shift is pressed while DoubleClick or Enter
void CAggregateContextMenu::OnShowInParent()
{
	HTREEITEM hItem=m_pParent->m_TreeAggregate.GetSelectedItem();
	LPUNKNOWN pUnknown=NULL;

	if(m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		TRACE(m_pParent->m_TreeAggregate.GetItemText(hItem));

		// Firing the event
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		pApp->GetCtrl()->FireShowInParentMgaObject( pUnknown);
	}
}

void CAggregateContextMenu::OnFollowRef()
{
	//// Firing the event
	//CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	//pApp->GetCtrl()->FireShowInParentMgaObject( pUnknown);

	HTREEITEM hItem    = m_pParent->m_TreeAggregate.GetSelectedItem();
	LPUNKNOWN pUnknown = NULL;

	if( m_pParent->m_TreeAggregate.m_MgaMap.LookupObjectUnknown( hItem, pUnknown))
	{
		CComQIPtr<IMgaReference>  ccpMgaRef( pUnknown);
		CComPtr<IMgaFCO>          ccpTgt;

		// preparing for a transaction
		CGMEActiveBrowserApp* pApp = (CGMEActiveBrowserApp*) AfxGetApp();
		CMgaContext* pMgaContext   = &pApp->m_CurrentProject.m_MgaContext;

		MSGTRY 
		{
			// Starting transaction
			pMgaContext->BeginTransaction(true); // it is readonly


			if( ccpMgaRef)
			{
				COMTHROW( ccpMgaRef->get_Referred( &ccpTgt));
			}
			
			
			// Ending transaction
			pMgaContext->CommitTransaction();
		} 
		MSGCATCH( _T("Error while following reference!"), pMgaContext->AbortTransaction();)

		MSGTRY			
		{
			if( !ccpTgt) AfxMessageBox( _T("Null reference can't be followed!"));
			else         m_pParent->GotoIUnkPtr( ccpTgt);
		} 
		MSGCATCH( _T("Error while following reference!"), ;)
	}
}

void CAggregateContextMenu::OnReadOnly( bool pProtectMe)
{
	m_pParent->accessRights( pProtectMe);
}
