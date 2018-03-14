// ActiveBrowserPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ActiveBrowserPropertyPage.h"
#include "GMEActiveBrowser.h"
#include "GMEActiveBrowserCtl.h"
#include "Meta_i.c"
#include "AggregateContextMenu.h"
#include "..\Gme\GMEOLEData.h"
#include "AttachLibDlg.h"
#include <comdef.h>
#include "Parser.h"
#include "CommonMfc.h"
#include <map>
#include <vector>

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#define ICON_NUMBER 11

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CAggregatePropertyPage, CPropertyPage)
IMPLEMENT_DYNCREATE(CInheritancePropertyPage, CPropertyPage)
IMPLEMENT_DYNCREATE(CMetaPropertyPage, CPropertyPage)




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///	CAggregatePropertyPage property page
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








CAggregatePropertyPage::CAggregatePropertyPage() : CPropertyPage(CAggregatePropertyPage::IDD),
	m_ComboEditCtrl			(&m_ComboSearchCtrl),
	m_ComboSearchCtrl		(&m_TreeAggregate),
	m_bProjectOpen			(FALSE),
	m_SearchButtonCtrlBitmap(NULL),
	m_parent(NULL)
{
	//{{AFX_DATA_INIT(CAggregatePropertyPage)
	//}}AFX_DATA_INIT
}

CAggregatePropertyPage::~CAggregatePropertyPage()
{
	m_ImageList.DeleteImageList();
	m_StateImageList.DeleteImageList();
	VERIFY( m_ImageList.GetSafeHandle() == NULL);
	VERIFY( m_StateImageList.GetSafeHandle() == NULL);
	BOOL success = ::DeleteObject(m_SearchButtonCtrlBitmap);
	ASSERT(success == TRUE);
}

void CAggregatePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAggregatePropertyPage)
	DDX_Control(pDX, IDC_COMBO_SEARCH_AGGREGATE, m_ComboSearchCtrl);
	DDX_Control(pDX, IDC_SEARCH, m_SearchButtonCtrl);
	DDX_Control(pDX, IDC_TREE_AGGREGATE, m_TreeAggregate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAggregatePropertyPage, CPropertyPage)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CAggregatePropertyPage)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_AGGREGATE, OnItemExpandingTreeAggregate)
	ON_NOTIFY(NM_CLICK, IDC_TREE_AGGREGATE, OnClickTreeAggregate)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_AGGREGATE, OnSelChangedTreeAggregate)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_AGGREGATE, OnDblclkTreeAggregate)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_AGGREGATE, OnEndLabelEditTreeAggregate)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_AGGREGATE, OnKeyDownTreeAggregate)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TREE_AGGREGATE, OnGetDispInfoTreeAggregate)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_AGGREGATE, OnItemExpandedTreeAggregate)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_AGGREGATE, OnBeginDragTreeAggregate)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



void CAggregatePropertyPage::OnSize(UINT nType, int cx, int cy) 
{

//	CPropertyPage::OnSize(nType, cx, cy);
	if(	::IsWindow(m_ComboSearchCtrl.GetSafeHwnd()) )
	{
		int ySpace = GetSystemMetrics(SM_CYBORDER);
		RECT clientRect;
		m_ComboSearchCtrl.GetClientRect(&clientRect);
		
		m_TreeAggregate.MoveWindow(0, clientRect.bottom + 2*ySpace, cx-1, cy-clientRect.bottom - 2*ySpace - 1);
		m_ComboSearchCtrl.MoveWindow(0, ySpace, cx-1, cy-1);

		//////////////////////////////////////////////////////////////////////////////////////////
		//m_ComboSearchCtrl.MoveWindow(3,3,cx-29,20);
		//m_SearchButtonCtrl.SetWindowPos(NULL,cx-22,2,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);		

	}
}



BOOL CAggregatePropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	

	m_TreeAggregate.ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT
								|TVS_EDITLABELS|TVS_SHOWSELALWAYS );


	m_ImageList.Create(16,16,ILC_MASK|ILC_COLOR24,0,0);
	
	// Creating the state image list
	PrepareStateImageList();


	SetDefID(IDC_SEARCH);

	if (m_SearchButtonCtrlBitmap == NULL)
		m_SearchButtonCtrlBitmap = ::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_SEARCH));
	m_SearchButtonCtrl.SetBitmap(m_SearchButtonCtrlBitmap);

	///////////////////////////////////////
	m_SearchButtonCtrl.ShowWindow(SW_HIDE);
	////////////////////////////////////////

	m_SearchButtonCtrl.EnableWindow(FALSE);
	m_TreeAggregate.EnableWindow(FALSE);
	m_ComboSearchCtrl.EnableWindow(FALSE);


	// Get edit control which happens to be the first child window
	// and subclass it
	m_ComboEditCtrl.SubclassWindow(m_ComboSearchCtrl.GetWindow(GW_CHILD)->GetSafeHwnd());

	m_TreeAggregate.RegisterDropTarget();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAggregatePropertyPage::OnItemExpandingTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult) 
{

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;


	if(pNMTreeView->action==TVE_EXPAND)
	{
		if(m_TreeAggregate.GetItemState(pNMTreeView->itemNew.hItem,TVIS_EXPANDED)
																	&TVIS_EXPANDED)
		{
			return;
		}

		if(m_Options.m_bIsDynamicLoading)
		{
			AfxGetApp()->LoadStandardCursor(IDC_WAIT);
			HTREEITEM hItem=pNMTreeView->itemNew.hItem;
			LPUNKNOWN pUnknown=NULL;

			if(m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
			{
				CComQIPtr<IMgaObject>ccpMgaObject(pUnknown);
				if(ccpMgaObject)
				{
					CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
					CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

					MSGTRY{
						// Starting transaction
						pMgaContext->BeginTransaction();
						reqBuildAggregateTree(hItem,ccpMgaObject,2);
						m_TreeAggregate.SortItems(hItem);
						pMgaContext->CommitTransaction ();								
					}MSGCATCH(_T("Error completing the operation"),pMgaContext->AbortTransaction();)	
					
					
				}
			}
			m_TreeAggregate.EnsureVisible(hItem);
			AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		}
#ifdef _DEBUG
		TRACE("\nEXPAND:\n");
		m_TreeAggregate.m_MgaMap.Dump(afxDump);	
#endif

		int nImage,nSelectedImage;
		m_TreeAggregate.GetItemImage(pNMTreeView->itemNew.hItem,nImage,nSelectedImage);

		if (nImage<8*ICON_NUMBER)
			m_TreeAggregate.SetItemImage(pNMTreeView->itemNew.hItem,nImage+ICON_NUMBER,nSelectedImage+ICON_NUMBER);
		else
			m_TreeAggregate.SetItemImage(pNMTreeView->itemNew.hItem, (nImage & ~1) + 1, (nImage & ~1) + 1);

	}
	else // Collapsing
	{
		if(!(m_TreeAggregate.GetItemState(pNMTreeView->itemNew.hItem,TVIS_EXPANDED)
																	&TVIS_EXPANDED))
		{
			return;
		}

		int nImage,nSelectedImage;
		m_TreeAggregate.GetItemImage(pNMTreeView->itemNew.hItem,nImage,nSelectedImage);
		
		ASSERT(nImage-ICON_NUMBER>=0);
		if (nImage<8*ICON_NUMBER)
			m_TreeAggregate.SetItemImage(pNMTreeView->itemNew.hItem,nImage-ICON_NUMBER,nSelectedImage-ICON_NUMBER);
		else
			m_TreeAggregate.SetItemImage(pNMTreeView->itemNew.hItem, (nImage & ~1), (nImage & ~1));
	}

	

	*pResult = 0;
}

void CAggregatePropertyPage::OnItemExpandedTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult) 
{

	if(!m_bProjectOpen)return;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if(pNMTreeView->action!=TVE_EXPAND)
	{
	
		if(m_Options.m_bIsDynamicLoading)
		{				
			
			m_TreeAggregate.DeleteAllChildren(pNMTreeView->itemNew.hItem);			
			
#ifdef _DEBUG
			TRACE("\nCollapse:\n");
			m_TreeAggregate.m_MgaMap.Dump(afxDump);	
#endif

		}

		m_TreeAggregate.CTreeCtrl::SetItemState(pNMTreeView->itemNew.hItem,0,TVIS_EXPANDEDONCE);
		
	}

	*pResult = 0;
}

int CAggregatePropertyPage::GetSourceControlStateOffset(IMgaObject * obj, int * latent)
{
	int ret_val = 0; bool saved = true;
	long st = 0; // state of the object

	try
	{
		CComQIPtr<IMgaModel> mod( obj);
		if( mod != NULL ) // if model
		{
			COMTHROW( mod->GetSourceControlInfo( &st));
		}
		else
		{
			CComQIPtr<IMgaFolder> fol( obj);
			if( fol != NULL) // if folder
				COMTHROW( fol->GetSourceControlInfo( &st));
			else
				return ret_val;
		}
	}
	catch(...)
	{
		return ret_val;
	}

	const int LOC = 0x1; // "GME_SourceControl_State_local"
	const int OTH = 0x2; // "GME_SourceControl_State_other"

	if( st & LOC)                // "GME_SourceControl_State_local"
		ret_val = 4*ICON_NUMBER; 
	else if( st & OTH)           // "GME_SourceControl_State_other"
		ret_val = 2*ICON_NUMBER; 
	else 
		ret_val = 0;


	//const int UTD = 0x000; // "GME_SourceControl_File_UpToDate"
	const int MBO = 0x100; // "GME_SourceControl_File_ModByOthers"
	const int NYS = 0x200; // "GME_SourceControl_File_NotYetSaved"

	if( st & MBO)                // "GME_SourceControl_File_ModByOthers"
		*latent = 1;
	else if( st & NYS)           // "GME_SourceControl_File_NotYetSaved"
		saved = false, *latent = 0;//ret_val = 6 * ICON_NUMBER;
	else// if( st & UTD)           // "GME_SourceControl_File_UpToDate"
		*latent = 0;

	if( !saved)
		ret_val = 6*ICON_NUMBER, *latent = 0;
	return ret_val;
}

CString CAggregatePropertyPage::GetDisplayedName(IMgaObject *pIMgaObject)
{    
	CComPtr<IMgaObject>ccpMgaObject(pIMgaObject);
	
	CComBSTR pDisplayedName;
	
	COMTHROW(ccpMgaObject->get_Name(&pDisplayedName));

    CString ret = pDisplayedName;

	return ret;
}

// Must be called from transaction
void CAggregatePropertyPage::SetupTree()
{
	
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	m_TreeAggregate.treeIcons.clear();
	m_ImageList.DeleteImageList();
	m_ImageList.Create(16,16,ILC_MASK|ILC_COLOR24,0,0);
	CBitmap bm;
	bm.LoadBitmap(IDB_IMAGELIST_AGGREGATE);
	m_ImageList.Add(&bm,RGB(128, 158, 8));  // Pretty natural transparency color 
											// provided by Tamas Paka
	m_TreeAggregate.SetImageList(&m_ImageList,TVSIL_NORMAL);


	// Getting Project
	CComPtr<IMgaProject> ccpMgaProject=pMgaContext->m_ccpProject;
	

	// Setting Project name
	CComBSTR bszProjectName;
	COMTHROW(pMgaContext->m_ccpProject->get_Name(&bszProjectName));
	m_strProjectName=bszProjectName;

	// Getting RootFolder
	CComPtr<IMgaFolder> ccpRootFolder;
	COMTHROW(pMgaContext->m_ccpProject->get_RootFolder(&ccpRootFolder));

	if(!m_Options.m_bIsDynamicLoading)
	{
		// Load the whole tree
		reqBuildAggregateTree(NULL,ccpRootFolder,-1);
	}
	else
	{
		// Only the root folder will be displayed
		reqBuildAggregateTree(NULL,ccpRootFolder,1);
	}

}

void CAggregatePropertyPage::reqBuildAggregateTree(HTREEITEM hParent,IMgaObject* pIMgaObject, int nDepthLevel, int nCounter, bool bWithUpdate)
{
	// Checking depth level
	if(nDepthLevel==0)return;
	
	CComPtr<IMgaObject> ccpMgaObject(pIMgaObject);

	// Determining Object Type
	objtype_enum otObjectType;
	COMTHROW(ccpMgaObject->get_ObjType(&otObjectType));
	
	switch(otObjectType)
	{
		case OBJTYPE_MODEL:
			{
				CComQIPtr<IMgaModel>ccpMgaModel(ccpMgaObject);
				
				HTREEITEM hItem=hParent;  // Because of possible level skip
				if(m_Options.m_bDispModel)
				{
					if(m_Options.m_bIsDynamicLoading && nCounter)
					{
						// Inserting item into the aggregate tree
						if(bWithUpdate)
						{
							hItem=m_TreeAggregate.InsertItemUpdate(hParent,GetDisplayedName(ccpMgaModel),
																			ccpMgaModel,OBJTYPE_MODEL);
						}
						else
						{
							hItem=m_TreeAggregate.InsertItem(hParent,GetDisplayedName(ccpMgaModel),
																			ccpMgaModel,OBJTYPE_MODEL);
						}
					}
				}
				else
				{
					// Increase depth level, because we did not do anything
					nDepthLevel++;
					nCounter++;
					// Next parent is the same as mine
					hItem=hParent;
				}
				if(hItem==NULL)	// Duplicate element - should not happen here
				{
					ASSERT(FALSE);
					return;
				}
				else
				{
					// Getting children FCOs
					CComPtr<IMgaFCOs> ccpMgaFCOs;
					COMTHROW(ccpMgaModel->get_ChildFCOs(&ccpMgaFCOs));

					if(nDepthLevel!=1)
					{
						// Iterate through the children FCOs
						MGACOLL_ITERATE(IMgaFCO, ccpMgaFCOs) 
						{						
							// Recursive call for each children							
							reqBuildAggregateTree(hItem,MGACOLL_ITER,nDepthLevel-1, nCounter-1,bWithUpdate);
							
						} MGACOLL_ITERATE_END;
					}

				}
			}break;
		
		case OBJTYPE_FOLDER:
			{
				CComQIPtr<IMgaFolder>ccpMgaFolder(ccpMgaObject);	
				
				// Inserting item into the aggregation tree
				HTREEITEM hItem=hParent; // If level skip
				if(hParent==NULL)// Root folder
				{
						if(bWithUpdate)
						{
							hItem=m_TreeAggregate.InsertItemUpdate(hParent,
									m_strProjectName,ccpMgaFolder,OBJTYPE_FOLDER);
						}
						else
						{
							hItem=m_TreeAggregate.InsertItem(hParent,
									m_strProjectName,ccpMgaFolder,OBJTYPE_FOLDER);
						}
				}
				else
				{
					if(m_Options.m_bIsDynamicLoading && nCounter)
					{
						if(bWithUpdate)
						{
							hItem=m_TreeAggregate.InsertItemUpdate(hParent,
								GetDisplayedName(ccpMgaFolder),ccpMgaFolder,OBJTYPE_FOLDER);
						}
						else
						{
							hItem=m_TreeAggregate.InsertItem(hParent,
								GetDisplayedName(ccpMgaFolder),ccpMgaFolder,OBJTYPE_FOLDER);

						}
					}
				}
				if(hItem==NULL)	
				{
					return;
				}
				else
				{

					/*********  Getting Subfolders ***********/
					CComPtr<IMgaFolders> ccpChildFolders;
					COMTHROW(ccpMgaFolder->get_ChildFolders(&ccpChildFolders));
					
					if(ccpChildFolders!=NULL)
					{
						if(nDepthLevel!=1)
						{
							// Iterate through the subfolders
							MGACOLL_ITERATE(IMgaFolder, ccpChildFolders) 
							{
								// Recursive call to each child folder							
								reqBuildAggregateTree(hItem,MGACOLL_ITER,nDepthLevel-1,nCounter-1,bWithUpdate);
							} MGACOLL_ITERATE_END;
						}

					}					
					/********* Getting children FCOs **********/
					CComPtr<IMgaFCOs> ccpChildFCOs;
					COMTHROW(ccpMgaFolder->get_ChildFCOs(&ccpChildFCOs));
					
					if(nDepthLevel!=1)
					{
						// Iterate through the children FCOs
						MGACOLL_ITERATE(IMgaFCO, ccpChildFCOs) 
						{
							// Recursive call to each children
								reqBuildAggregateTree(hItem,MGACOLL_ITER,nDepthLevel-1,nCounter-1,bWithUpdate);
						} MGACOLL_ITERATE_END;
					}

				}

			}break;
		case OBJTYPE_ATOM:
			{
				if(m_Options.m_bDispAtom)
				{
					CComQIPtr<IMgaAtom>ccpMgaAtom(ccpMgaObject);								
					
					// Inserting item into the aggregation tree
					if(bWithUpdate)
					{
						HTREEITEM hItem=m_TreeAggregate.InsertItemUpdate(hParent,
								GetDisplayedName(ccpMgaAtom),ccpMgaAtom,OBJTYPE_ATOM);	
					}
					else
					{
						HTREEITEM hItem=m_TreeAggregate.InsertItem(hParent,
								GetDisplayedName(ccpMgaAtom),ccpMgaAtom,OBJTYPE_ATOM);	
					}
				}
			}break;		

		case OBJTYPE_CONNECTION:
			{
				if(m_Options.m_bDispConnections)//hack!!!
				{
					CComQIPtr<IMgaConnection>ccpMgaConnection(ccpMgaObject);
					if(bWithUpdate)
					{
						HTREEITEM hItem=m_TreeAggregate.InsertItemUpdate(hParent,
							GetDisplayedName(ccpMgaConnection),ccpMgaConnection,OBJTYPE_CONNECTION);
					}
					else
					{
						HTREEITEM hItem=m_TreeAggregate.InsertItem(hParent,
							GetDisplayedName(ccpMgaConnection),ccpMgaConnection,OBJTYPE_CONNECTION);
					}
				}
			}break;

		case OBJTYPE_REFERENCE:
			{
				if(m_Options.m_bDispReference)
				{
					CComQIPtr<IMgaReference>ccpMgaReference(ccpMgaObject);								
					// Inserting item into the aggregation tree
					if(bWithUpdate)
					{
						HTREEITEM hItem=m_TreeAggregate.InsertItemUpdate(hParent,
							GetDisplayedName(ccpMgaReference),ccpMgaReference,OBJTYPE_REFERENCE);
					}
					else
					{
						HTREEITEM hItem=m_TreeAggregate.InsertItem(hParent,
							GetDisplayedName(ccpMgaReference),ccpMgaReference,OBJTYPE_REFERENCE);
					}
				}												
			}break;
		case OBJTYPE_SET:
			{
				if(m_Options.m_bDispSet)
				{
					CComQIPtr<IMgaSet>ccpMgaSet(ccpMgaObject);								
					// Inserting item into the aggregation tree
					if(bWithUpdate)
					{
						HTREEITEM hItem=m_TreeAggregate.InsertItemUpdate(hParent,
									GetDisplayedName(ccpMgaSet),ccpMgaSet,OBJTYPE_SET);
					}
					else
					{
						HTREEITEM hItem=m_TreeAggregate.InsertItem(hParent,
									GetDisplayedName(ccpMgaSet),ccpMgaSet,OBJTYPE_SET);
					}
				}
				return;
			}break;
		
		case OBJTYPE_ATTRIBUTE:;// Not handled
		case OBJTYPE_ROLE:;		// Not handled
		case OBJTYPE_ASPECT:;	// Not handled
		case OBJTYPE_PART:;		// Not handled
		default:
			{
				TRACE(_T("Unknown MGA object type detected building the aggregation tree\n"));
				ASSERT(0);
				return;
			}
	}
}



BOOL CAggregatePropertyPage::reqHasDisplayedChild(IMgaObject* pIMgaObject, int nDepthLevel)
{
	static int nFirstRun=nDepthLevel;

	// Checking depth level
	if(nDepthLevel==0)return FALSE;

	
	CComPtr<IMgaObject> ccpMgaObject(pIMgaObject);

	// Determining Object Type
	objtype_enum otObjectType;
	COMTHROW(ccpMgaObject->get_ObjType(&otObjectType));
	
	switch(otObjectType)
	{
		case OBJTYPE_MODEL:
			{
				CComQIPtr<IMgaModel>ccpMgaModel(ccpMgaObject);
				
				if(nFirstRun!= nDepthLevel && m_Options.IsDisplayedType(OBJTYPE_MODEL)) return TRUE;
				// Getting children FCOs
				CComPtr<IMgaFCOs> ccpMgaFCOs;
				COMTHROW(ccpMgaModel->get_ChildFCOs(&ccpMgaFCOs));

				// Iterate through the children FCOs
				bool doHasChild = false;
				MGACOLL_ITERATE(IMgaFCO, ccpMgaFCOs) 
				{						
					// Recursive call for each children
					if(reqHasDisplayedChild(MGACOLL_ITER,nDepthLevel-1)){
						doHasChild = true;
						break;
					}						
					
				} MGACOLL_ITERATE_END;
				if (doHasChild) {
					return TRUE;
				}

				
			}break;
		
		case OBJTYPE_FOLDER:
			{
				CComQIPtr<IMgaFolder>ccpMgaFolder(ccpMgaObject);	
				
				if(nFirstRun!= nDepthLevel && m_Options.IsDisplayedType(OBJTYPE_FOLDER)) return TRUE;
				
				/*********  Getting Subfolders ***********/
				CComPtr<IMgaFolders> ccpChildFolders;
				COMTHROW(ccpMgaFolder->get_ChildFolders(&ccpChildFolders));
	
				// Iterate through the subfolders
				bool doHasChild = false;
				MGACOLL_ITERATE(IMgaFolder, ccpChildFolders) 
				{
					// Recursive call to each child folder
					if(reqHasDisplayedChild(MGACOLL_ITER,nDepthLevel-1)) {
						doHasChild = true;
						break;
					}
				} MGACOLL_ITERATE_END;
				if (doHasChild) {
					return TRUE;
				}
					
				/********* Getting children FCOs **********/
				CComPtr<IMgaFCOs> ccpChildFCOs;
				COMTHROW(ccpMgaFolder->get_ChildFCOs(&ccpChildFCOs));
				// Iterate through the children FCOs
				MGACOLL_ITERATE(IMgaFCO, ccpChildFCOs) 
				{
					// Recursive call to each children
					if(reqHasDisplayedChild(MGACOLL_ITER,nDepthLevel-1)) {
						doHasChild = true;
						break;
					}
				} MGACOLL_ITERATE_END;
				if (doHasChild) {
					return TRUE;
				}

			}break;
		case OBJTYPE_ATOM:
			{
				return nFirstRun!= nDepthLevel &&  m_Options.IsDisplayedType(OBJTYPE_ATOM);
			}break;		

		case OBJTYPE_CONNECTION:
			{
				return nFirstRun!= nDepthLevel && m_Options.IsDisplayedType(OBJTYPE_CONNECTION);
			}break;

		case OBJTYPE_REFERENCE:
			{
				return nFirstRun!= nDepthLevel && m_Options.IsDisplayedType(OBJTYPE_REFERENCE);
			}break;
		case OBJTYPE_SET:
			{
				return nFirstRun!= nDepthLevel && m_Options.IsDisplayedType(OBJTYPE_SET);
				
			}break;
		
		case OBJTYPE_ATTRIBUTE:;// Not handled
		case OBJTYPE_ROLE:;		// Not handled
		case OBJTYPE_ASPECT:;	// Not handled
		case OBJTYPE_PART:;		// Not handled
		default:
			{
				return FALSE;
			}
	}
	return FALSE;
	
}



void CAggregatePropertyPage::OpenProject()
{
	m_TreeAggregate.EnableWindow(TRUE);
	m_ComboSearchCtrl.EnableWindow(TRUE);
	m_SearchButtonCtrl.EnableWindow(TRUE);

	m_bIsBrowserCreatedNewObject=FALSE;

	AfxGetApp()->LoadStandardCursor(IDC_WAIT);

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	MSGTRY{
		// Starting transaction
		pMgaContext->BeginTransaction();
		

		SetupTree();

		if(m_Options.m_bStoreTreeInRegistry)
		{

			/* Loading tree state from the registry */			
			m_TreeAggregate.LoadTreeStateFromRegistry(m_strProjectName);
						
			
		}

		m_TreeAggregate.SortItems();	
		pMgaContext->CommitTransaction();

	}MSGCATCH(_T("Error opening aggregate tab"),pMgaContext->AbortTransaction();)	
	
	

	


	m_bProjectOpen=TRUE;
	AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	



#ifdef _DEBUG
	m_TreeAggregate.m_MgaMap.Dump(afxDump);
#endif

}

void CAggregatePropertyPage::SetFocus2()
{
	m_TreeAggregate.SetFocus();
}

void CAggregatePropertyPage::CloseProject()
{
	m_bProjectOpen=FALSE;
	AfxGetApp()->LoadStandardCursor(IDC_WAIT);

	// by ZolMol
	if(CGMEDataSource::myData) // it is true when object copied onto clipboard from the active browser tree
		OleFlushClipboard();
	// end
	
		if(m_Options.m_bStoreTreeInRegistry)
		{
			// Saving tree state to registry
			// Opening transaction
			CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
			CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
			MSGTRY{	
				pMgaContext->BeginTransaction();
				
				// Saving to registry
				m_TreeAggregate.SaveTreeStateToRegistry(m_strProjectName);
				// Close transaction
				pMgaContext->CommitTransaction();
			}MSGCATCH(_T("Error closing the aggragate tab"),pMgaContext->AbortTransaction();)	
			
			
		}

		m_TreeAggregate.CleanUp();
		m_TreeAggregate.EnableWindow(FALSE);
		
		m_SearchButtonCtrl.EnableWindow(FALSE);

		m_ComboSearchCtrl.ResetContent();
		m_ComboSearchCtrl.EnableWindow(FALSE);
        m_TreeAggregate.m_highlightedObjects.clear();
		AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	

}

void CAggregatePropertyPage::DoCopy()
{

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
	


	CComObjPtr<IMgaFCOs> ccpSelectedFCOs;
	COMTHROW( ccpSelectedFCOs.CoCreateInstance( L"Mga.MgaFCOs"));
	CComObjPtr<IMgaFolders> ccpSelectedFolders;
	COMTHROW( ccpSelectedFolders.CoCreateInstance( L"Mga.MgaFolders"));

	typedef CTypedPtrList<CPtrList, CRect *> CRectList;
	CRectList rectSelectedList;
	CRect	rectItem;
	CRect rectFrame(0,0,0,0);


	for(HTREEITEM hItem=m_TreeAggregate.GetFirstSelectedItem();hItem;
						hItem=m_TreeAggregate.GetNextSelectedItem(hItem))
	{
		CAggregateMgaObjectProxy ObjectProxy;
		if(m_TreeAggregate.m_MgaMap.LookupObjectProxy(hItem, ObjectProxy))
		{
			if ( ObjectProxy.m_TypeInfo == OBJTYPE_MODEL     ||	// FCOs
			     ObjectProxy.m_TypeInfo == OBJTYPE_ATOM      ||
			     ObjectProxy.m_TypeInfo == OBJTYPE_REFERENCE ||
			     ObjectProxy.m_TypeInfo == OBJTYPE_CONNECTION ||
			     ObjectProxy.m_TypeInfo == OBJTYPE_SET)
			{
				CComQIPtr<IMgaFCO> ccpMgaFCO( ObjectProxy.m_pMgaObject);
				if( ccpMgaFCO)
				{
					// Finding the rectangle of the selected item
					m_TreeAggregate.GetItemRect( hItem, rectItem, TRUE);
					// Computing the frame of all rectangles
					rectFrame.UnionRect( rectFrame, rectItem);

					// Adding to the selected rectangle list to create the image
					rectSelectedList.AddTail(new CRect( rectItem));

					// Append to the selected FCO list
					COMTHROW(ccpSelectedFCOs->Append( ccpMgaFCO));
				}
			}
			else if ( ObjectProxy.m_TypeInfo == OBJTYPE_FOLDER )
			{
				CComQIPtr<IMgaFolder> ccpMgaFolder( ObjectProxy.m_pMgaObject);
				if( ccpMgaFolder)
				{
					// Finding the rectangle of the selected item
					m_TreeAggregate.GetItemRect( hItem, rectItem, TRUE);
					// Computing the frame of all rectangles
					rectFrame.UnionRect( rectFrame, rectItem);

					// Adding to the selected rectangle list to create the image
					rectSelectedList.AddTail( new CRect( rectItem));

					// Append to the selected FCO list
					COMTHROW(ccpSelectedFolders->Append( ccpMgaFolder));
				}
			}
		}		
	}
	
	if( rectSelectedList.GetCount() != 0)
	{
		CRectList dummyAnnList;

		CGMEDataDescriptor GMEDataDescriptor( rectSelectedList, dummyAnnList, rectFrame.TopLeft(), CPoint( 0, 0));

		CGMEDataSource *pGMEDataSource = new CGMEDataSource( pMgaContext->m_ccpProject);
		
		if ( ccpSelectedFCOs)		pGMEDataSource->SetNodes( CComPtr<IMgaFCOs>( ccpSelectedFCOs));
		if ( ccpSelectedFolders)	pGMEDataSource->SetFolders( CComPtr<IMgaFolders>( ccpSelectedFolders));
		
		pGMEDataSource->CacheDescriptor(&GMEDataDescriptor);

		pGMEDataSource->SetClipboard();
	}

	CGMEDataDescriptor::destructList( rectSelectedList);
}

void CAggregatePropertyPage::DoCopyClosure()
{

	bool is_any_folder_selected = false;
	CGMEActiveBrowserApp * pApp = (CGMEActiveBrowserApp*) AfxGetApp();
	CMgaContext * pMgaContext = &pApp->m_CurrentProject.m_MgaContext;
	
	CComObjPtr<IMgaFCOs> ccpSelectedFCOs;
	CComObjPtr<IMgaFolders> ccpSelectedFolders;
	COMTHROW( ccpSelectedFCOs.CoCreateInstance( L"Mga.MgaFCOs"));
	COMTHROW( ccpSelectedFolders.CoCreateInstance( L"Mga.MgaFolders"));

	typedef CTypedPtrList< CPtrList, CRect *> CRectList;
	CRectList rectSelectedList;
	CRect rectItem;
	CRect rectFrame(0,0,0,0);


	for( HTREEITEM hItem = m_TreeAggregate.GetFirstSelectedItem(); 
		hItem; 
		hItem = m_TreeAggregate.GetNextSelectedItem( hItem))
	{
		CAggregateMgaObjectProxy ObjectProxy;
		if( m_TreeAggregate.m_MgaMap.LookupObjectProxy( hItem, ObjectProxy))
		{
			if ( ObjectProxy.m_TypeInfo == OBJTYPE_MODEL			// FCOs: connection too
			   || ObjectProxy.m_TypeInfo == OBJTYPE_ATOM
			   || ObjectProxy.m_TypeInfo == OBJTYPE_REFERENCE
			   || ObjectProxy.m_TypeInfo == OBJTYPE_SET
			   || ObjectProxy.m_TypeInfo == OBJTYPE_CONNECTION)
			{
				CComQIPtr<IMgaFCO> ccpMgaFCO( ObjectProxy.m_pMgaObject);
				if( ccpMgaFCO)
				{
					// Finding the rectangle of the selected item
					m_TreeAggregate.GetItemRect( hItem, rectItem, TRUE);
					// Computing the frame of all rectangles
					rectFrame.UnionRect( rectFrame, rectItem);

					// Adding to the selected rectangle list to create the image
					rectSelectedList.AddTail( new CRect( rectItem));
					
					// Append to the selected FCO list
					COMTHROW(ccpSelectedFCOs->Append( ccpMgaFCO));
				}
			}
			else if ( ObjectProxy.m_TypeInfo == OBJTYPE_FOLDER )
			{
				CComQIPtr<IMgaFolder> ccpMgaFolder( ObjectProxy.m_pMgaObject);
				if( ccpMgaFolder)
				{
					{
						// Finding the rectangle of the selected item
						m_TreeAggregate.GetItemRect( hItem, rectItem, TRUE);
						// Computing the frame of all rectangles
						rectFrame.UnionRect( rectFrame, rectItem);

						// Adding to the selected rectangle list to create the image
						rectSelectedList.AddTail( new CRect( rectItem));
						
						// Append to the selected FCO list
						COMTHROW(ccpSelectedFolders->Append( ccpMgaFolder));
						is_any_folder_selected = true;
					}
				}
			}
		}		
	}
	
	if ( rectSelectedList.GetCount()!=0)
	{
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
		CMgaContext* pMgaContext = &pApp->m_CurrentProject.m_MgaContext;
		pMgaContext->BeginTransaction(true); // readonly
		try {
			COMTHROW( cl->GetLastClosureId( pMgaContext->m_ccpProject, &_markThem));
			pMgaContext->CommitTransaction();
		} catch( hresult_exception& ) {
			pMgaContext->AbortTransaction();
			_markThem = -2; // will be increased to -1
		}

		++_markThem; // propose this as the next closure id

		COMTHROW( cl->SelectiveClosureDlg
			( is_any_folder_selected ? VARIANT_TRUE : VARIANT_FALSE
			, &_refersTo           , &_referredBy
			, &_connections        , &_connsThroughRefport
			, &_setMembers         , &_memberOfSets
			, &_modelContainment   , &_partOfModels
			, &_folderContainment  , &_partOfFolders
			, &_baseTypeOf         , &_derivedTypesOf
			, &_libraryElementHandling          // 0: stubs; 1: stop; 2: continue
			, &_wrappingMode                    // 0: miniproject; 1: automatic; 2: as is
			, &_kindFilter                      // -1 = all; otherwise calculate the 0b000rsamf mask; where f = Folders; m = Models; a = Atoms; s = Sets; r = References
			, &_markThem                        // -1 = do not mark
			, &_options
			, &_directionDown
			, &_copyToFile              // file or clipboard
			, &_okPressed               // how ended the dialog
			, &_userSelFileName
			));
		if( _okPressed == VARIANT_FALSE) return;

		if( _markThem != -1) // save this if elements need to be marked
		{
			pMgaContext->BeginTransaction(false);
			try {
				COMTHROW( cl->PutLastClosureId( pMgaContext->m_ccpProject, _markThem));
				pMgaContext->CommitTransaction();
			} catch( hresult_exception& ) {
				pMgaContext->AbortTransaction();
			}
		}

		CComObjPtr<IMgaFCOs> sel_fcos, top_fcos;
		CComObjPtr<IMgaFolders> sel_folders, top_folders;
		//long options;
		VARIANT_BOOL tops_filled = VARIANT_FALSE;
		CComBstrObj acc_kinds;

		COMTHROW( cl->SelectiveClosureCalc
			( ccpSelectedFCOs    , ccpSelectedFolders
			, _refersTo          , _referredBy
			, _connections       , _connsThroughRefport
			, _setMembers        , _memberOfSets
			, _modelContainment  , _partOfModels
			, _folderContainment , _partOfFolders
			, _baseTypeOf        , _derivedTypesOf
			, _libraryElementHandling          // 0: stubs, 1: stop, 2: continue
			, _wrappingMode                    // 0: miniproject, 1: automatic, 2: as is
			, _kindFilter                      // -1 = all, otherwise calculate the 0b000rsamf mask, where f = Folders, m = Models, a = Atoms, s = Sets, r = References
			, _markThem                        // -1 = do not mark
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
					COMTHROW( dumper->DumpClos( sel_fcos, sel_folders, _userSelFileName, _options) );
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
			CGMEDataDescriptor GMEDataDescriptor( rectSelectedList, CRectList(), rectFrame.TopLeft(), CPoint( 0,0));
			CGMEClosureDataSource * pGMEClosureDataSource = new CGMEClosureDataSource( pMgaContext->m_ccpProject);

			if ( sel_fcos)		pGMEClosureDataSource->SetNodes( sel_fcos);
			if ( sel_folders)	pGMEClosureDataSource->SetFolders( sel_folders);
			
			if ( top_fcos)		pGMEClosureDataSource->SetTopNodes( top_fcos);
			if ( top_folders)	pGMEClosureDataSource->SetTopNodes( top_folders);

			pGMEClosureDataSource->SetOptions( _options);
			pGMEClosureDataSource->SetAcceptingKinds( acc_kinds);

			pGMEClosureDataSource->CacheDescriptor( &GMEDataDescriptor);
			pGMEClosureDataSource->SetClipboard();
			pGMEClosureDataSource->FlushClipboard();
		}
	}
	else
	{
		MessageBox(_T("No object selected"),_T("Copy Error"), MB_ICONERROR);
	}

	CGMEDataDescriptor::destructList( rectSelectedList);
}

void CAggregatePropertyPage::DoCopySmart( int k /*= 0*/)
{
	bool is_any_folder_selected = false;
	bool all_containers = true;
	bool all_refs = true;
	bool all_sets = true;
	bool all_conns = true;

	bool any_container = false;
	bool any_ref = false;
	bool any_set = false;
	bool any_conn = false;

	CGMEActiveBrowserApp * pApp = (CGMEActiveBrowserApp*) AfxGetApp();
	CMgaContext * pMgaContext = &pApp->m_CurrentProject.m_MgaContext;
	
	CComObjPtr<IMgaFCOs> ccpSelectedFCOs;
	CComObjPtr<IMgaFolders> ccpSelectedFolders;
	COMTHROW( ccpSelectedFCOs.CoCreateInstance( L"Mga.MgaFCOs"));
	COMTHROW( ccpSelectedFolders.CoCreateInstance( L"Mga.MgaFolders"));

	typedef CTypedPtrList< CPtrList, CRect *> CRectList;
	CRectList rectSelectedList;
	CRect rectItem;
	CRect rectFrame(0,0,0,0);


	for( HTREEITEM hItem = m_TreeAggregate.GetFirstSelectedItem(); 
		hItem; 
		hItem = m_TreeAggregate.GetNextSelectedItem( hItem))
	{
		CAggregateMgaObjectProxy ObjectProxy;
		if( m_TreeAggregate.m_MgaMap.LookupObjectProxy( hItem, ObjectProxy))
		{
			all_containers &= (ObjectProxy.m_TypeInfo == OBJTYPE_MODEL || ObjectProxy.m_TypeInfo == OBJTYPE_FOLDER);
			all_refs  &= (ObjectProxy.m_TypeInfo == OBJTYPE_REFERENCE);
			all_sets  &= (ObjectProxy.m_TypeInfo == OBJTYPE_SET);
			all_conns &= (ObjectProxy.m_TypeInfo == OBJTYPE_CONNECTION);

			any_container |= (ObjectProxy.m_TypeInfo == OBJTYPE_MODEL || ObjectProxy.m_TypeInfo == OBJTYPE_FOLDER);
			any_ref  |= (ObjectProxy.m_TypeInfo == OBJTYPE_REFERENCE);
			any_set  |= (ObjectProxy.m_TypeInfo == OBJTYPE_SET);
			any_conn |= (ObjectProxy.m_TypeInfo == OBJTYPE_CONNECTION);

			if ( ObjectProxy.m_TypeInfo == OBJTYPE_MODEL			// FCOs: connection too
			   || ObjectProxy.m_TypeInfo == OBJTYPE_ATOM
			   || ObjectProxy.m_TypeInfo == OBJTYPE_REFERENCE
			   || ObjectProxy.m_TypeInfo == OBJTYPE_SET
			   || ObjectProxy.m_TypeInfo == OBJTYPE_CONNECTION)
			{
				CComQIPtr<IMgaFCO> ccpMgaFCO( ObjectProxy.m_pMgaObject);
				if( ccpMgaFCO)
				{
					// Finding the rectangle of the selected item
					m_TreeAggregate.GetItemRect( hItem, rectItem, TRUE);
					// Computing the frame of all rectangles
					rectFrame.UnionRect( rectFrame, rectItem);

					// Adding to the selected rectangle list to create the image
					rectSelectedList.AddTail( new CRect( rectItem));
					
					// Append to the selected FCO list
					COMTHROW(ccpSelectedFCOs->Append( ccpMgaFCO));
				}
			}
			else if ( ObjectProxy.m_TypeInfo == OBJTYPE_FOLDER )
			{
				CComQIPtr<IMgaFolder> ccpMgaFolder( ObjectProxy.m_pMgaObject);
				if( ccpMgaFolder)
				{
					{
						// Finding the rectangle of the selected item
						m_TreeAggregate.GetItemRect( hItem, rectItem, TRUE);
						// Computing the frame of all rectangles
						rectFrame.UnionRect( rectFrame, rectItem);

						// Adding to the selected rectangle list to create the image
						rectSelectedList.AddTail( new CRect( rectItem));
						
						// Append to the selected FCO list
						COMTHROW(ccpSelectedFolders->Append( ccpMgaFolder));
						is_any_folder_selected = true;
					}
				}
			}
		}		
	}
	
	if ( rectSelectedList.GetCount()!=0)
	{
		CComPtr<IMgaClosure> cl;
		CComObjPtr<IMgaFCOs> sel_fcos, top_fcos;
		CComObjPtr<IMgaFolders> sel_folders, top_folders;
		long options;
		CComBstrObj acckinds; 
		CComBstrObj path; 

		// commented out when new smart copy introduced
		// if container selected then it must be lonely (no multiple selection)
		// if container selected no other kinds allowed
		//if( any_container && !all_containers || rectSelectedList.GetCount() > 1 && all_containers)
		//{
		//	MessageBox("Invalid selection for smart copy. You can select either one container or several non-containers.","Copy Error", MB_ICONERROR);
		//	return;
		//}

		if( any_container) k |= 1024;

		if( any_ref) k |= 2048;
		if( any_set) k |= 4096;
		if( any_conn) k |= 8192;
		if( is_any_folder_selected)  k |= 16384;

		COMTHROW( cl.CoCreateInstance(L"Mga.MgaClosure") );
		
		COMTHROW( cl->SmartCopy( 
			ccpSelectedFCOs, 
			ccpSelectedFolders, 
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

			CGMEDataDescriptor GMEDataDescriptor( rectSelectedList, dummyAnnList, rectFrame.TopLeft(), CPoint( 0,0));

			CGMEClosureDataSource * pGMEClosureDataSource = new CGMEClosureDataSource( pMgaContext->m_ccpProject);
			
			if ( sel_fcos)		pGMEClosureDataSource->SetNodes( sel_fcos);
			if ( sel_folders)	pGMEClosureDataSource->SetFolders( sel_folders);

			if ( top_fcos)		pGMEClosureDataSource->SetTopNodes( top_fcos);
			if ( top_folders)	pGMEClosureDataSource->SetTopNodes( top_folders);


			pGMEClosureDataSource->SetOptions( options);
			pGMEClosureDataSource->SetAbsPathPart( path);
			pGMEClosureDataSource->SetAcceptingKinds( acckinds);

			pGMEClosureDataSource->CacheDescriptor( &GMEDataDescriptor);

			pGMEClosureDataSource->SetClipboard();
			pGMEClosureDataSource->FlushClipboard();
		}
	}
	else
	{
		MessageBox(_T("No object selected"),_T("Copy Error"), MB_ICONERROR);
	}

	CGMEDataDescriptor::destructList( rectSelectedList);
}

void CAggregatePropertyPage::DoDrag( CPoint ptDrag)
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
	
	CGMEDataSource GMEDataSource(pMgaContext->m_ccpProject);

	CComObjPtr<IMgaFCOs> ccpSelectedFCOs;
	COMTHROW(ccpSelectedFCOs.CoCreateInstance(L"Mga.MgaFCOs"));
	CComObjPtr<IMgaFolders> ccpSelectedFolders;
	COMTHROW( ccpSelectedFolders.CoCreateInstance( L"Mga.MgaFolders"));

	typedef CTypedPtrList<CPtrList, CRect *> CRectList;
	CRectList rectSelectedList;
	CRect	rectItem;
	CRect rectFrame(0,0,0,0);


	for(HTREEITEM hItem=m_TreeAggregate.GetFirstSelectedItem();hItem;
						hItem=m_TreeAggregate.GetNextSelectedItem(hItem))
	{
		CAggregateMgaObjectProxy ObjectProxy;
		if(m_TreeAggregate.m_MgaMap.LookupObjectProxy(hItem, ObjectProxy))
		{
			if(ObjectProxy.m_TypeInfo==OBJTYPE_MODEL||			// FCOs
			   ObjectProxy.m_TypeInfo==OBJTYPE_ATOM||
			   ObjectProxy.m_TypeInfo==OBJTYPE_CONNECTION||
			   ObjectProxy.m_TypeInfo==OBJTYPE_REFERENCE||
			   ObjectProxy.m_TypeInfo==OBJTYPE_SET)
			{
				CComQIPtr<IMgaFCO>ccpMgaFCO(ObjectProxy.m_pMgaObject);
				if(ccpMgaFCO)
				{
					// Finding the rectangle of the selected item
					m_TreeAggregate.GetItemRect(hItem,rectItem,TRUE);
					// Computing the frame of all rectangles
					rectFrame.UnionRect(rectFrame,rectItem);

					// Adding to the selected rectangle list to create the image
					rectSelectedList.AddTail(new CRect(rectItem));
					
					// Append to the selected FCO list
					COMTHROW(ccpSelectedFCOs->Append(ccpMgaFCO));
				}
			}
			else if ( ObjectProxy.m_TypeInfo == OBJTYPE_FOLDER )
			{
				CComQIPtr<IMgaFolder> ccpMgaFolder( ObjectProxy.m_pMgaObject);
				if( ccpMgaFolder)
				{
					// Finding the rectangle of the selected item
					m_TreeAggregate.GetItemRect( hItem, rectItem, TRUE);
					// Computing the frame of all rectangles
					rectFrame.UnionRect( rectFrame, rectItem);

					// Adding to the selected rectangle list to create the image
					rectSelectedList.AddTail( new CRect( rectItem));

					// Append to the selected FCO list
					COMTHROW(ccpSelectedFolders->Append( ccpMgaFolder));
				}
			}
			else
			{
				m_TreeAggregate.SelectItemEx(hItem,FALSE);
			}
		}		
	}
	
	if(rectSelectedList.GetCount()!=0)
	{
		CRectList dummyAnnList;

		CGMEDataDescriptor GMEDataDescriptor(rectSelectedList, dummyAnnList,rectFrame.TopLeft(),CPoint(0,0));

		if ( ccpSelectedFCOs)		GMEDataSource.SetNodes( CComPtr<IMgaFCOs>(ccpSelectedFCOs));
		if ( ccpSelectedFolders)	GMEDataSource.SetFolders( CComPtr<IMgaFolders>(ccpSelectedFolders));

		GMEDataSource.CacheDescriptor(&GMEDataDescriptor);

		m_TreeAggregate.CreateDragImageEx(ptDrag);		
		GMEDataSource.DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_LINK | DROPEFFECT_MOVE);
		m_TreeAggregate.DeleteDragImageEx();
		
	}

	CGMEDataDescriptor::destructList( rectSelectedList);
}


void CAggregatePropertyPage::OnClickTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CAggregatePropertyPage::OnSelChangedTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	
	// Setting the focus here, because combo could make the selection
	// PETER: I have commented this, since it prevented multiple undo-s (CTRL-Z)
	// m_TreeAggregate.SetFocus();

	// Displaying the first selection in the combo edit
	HTREEITEM hItem=m_TreeAggregate.GetFirstSelectedItem();
	if(hItem)
	{
		m_ComboSearchCtrl.SetSelection(hItem);
		
		LPUNKNOWN pUnknown=NULL;

		if(m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
		{
			TRACE(m_TreeAggregate.GetItemText(hItem));

			// Firing the event
			CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
			CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

			// MOD by Peter: do not fire events from transactions
			if (!pMgaContext->IsInTransaction()) {
				pApp->GetCtrl()->FireClickMgaObject(pUnknown);
			}
		}

	}

	*pResult = 0;
}


void CAggregatePropertyPage::OnDblclkTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult) 
{

	

	DWORD dwMsgPos = ::GetMessagePos();

	CPoint point = CPoint(GET_X_LPARAM(dwMsgPos), GET_Y_LPARAM(dwMsgPos) );
	UINT nHitFlags;
	
	m_TreeAggregate.ScreenToClient(&point);
	m_TreeAggregate.HitTest( point, &nHitFlags );

	
	if(nHitFlags&TVHT_ONITEMBUTTON)
	{
		return;
	}

	HTREEITEM hItem=m_TreeAggregate.GetSelectedItem();
	
	LPUNKNOWN pUnknown=NULL;

	if(m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		TRACE(m_TreeAggregate.GetItemText(hItem));

		// Firing the event
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		if( ::GetKeyState( VK_SHIFT) < 0)
			pApp->GetCtrl()->FireShowInParentMgaObject(pUnknown);
		else
			pApp->GetCtrl()->FireDblClickMgaObject(pUnknown);
	}

	*pResult = -1; // Nonzero return: disables auto expanding (default action)
}

void CAggregatePropertyPage::PrepareStateImageList()
{
	CImageList ImageList;
	ImageList.Create(16,16,ILC_MASK|ILC_COLOR24,0,0);
	
	CBitmap bm;
	bm.LoadBitmap(IDB_IMAGELIST_AGGREGATE_STATE);

	ImageList.Add(&bm,RGB(128, 158, 8));	// Pretty natural transparent color 
											// provided by Tamas Paka


	CClientDC dcClient(this);
	 CDC dcMem;
	 CBitmap Bitmap;

	 if (!dcMem.CreateCompatibleDC(&dcClient))return;

	 if (!Bitmap.CreateCompatibleBitmap(&dcClient,256,16)) return;

	 CBitmap *pOldMemDCBitmap = dcMem.SelectObject(&Bitmap);

	 COLORREF cMaskColor=RGB(128,58,8);
	 dcMem.FillSolidRect(0, 0, 256, 16, cMaskColor);

	 // Paint each Image in the DC

	 for(BYTE i=0;i<=15;i++)
	{
		if(i&0x0001) // Access
		{
			
			ImageList.Draw(&dcMem, 1, CPoint(i*16,0), ILD_TRANSPARENT);

		}
		else
		{
			
		}

		if(i&0x0002) // LibObject
		{
			ImageList.Draw(&dcMem, 0, CPoint(i*16,0), ILD_TRANSPARENT);
		}
		if(i&0x0004) // Instance
		{
			ImageList.Draw(&dcMem, 5, CPoint(i*16,0), ILD_TRANSPARENT);
		}

		if(i&0x0008)	// Derived (subtype)
		{
			ImageList.Draw(&dcMem, 4, CPoint(i*16,0), ILD_TRANSPARENT);
		}
		

	}

/*
 			#ifdef _DEBUG
				   CDC *pWndDC=GetDC();
				   BitBlt(pWndDC->GetSafeHdc(),0,0,rectComplete.Width(),rectComplete.Height(),dcMem.GetSafeHdc(),0,0,SRCCOPY);
				   ReleaseDC(pWndDC);
			#endif
*/
	 dcMem.SelectObject(pOldMemDCBitmap);


	 
	 m_StateImageList.Create(16,16, ILC_COLOR | ILC_MASK, 0, 1);

	 // Green is used as mask color
	 m_StateImageList.Add(&Bitmap, cMaskColor); 

	 Bitmap.DeleteObject();
	

	m_TreeAggregate.SetImageList(&m_StateImageList,TVSIL_STATE);

}

void CAggregatePropertyPage::AttachLibrary()
{

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	CAttachLibDlg dlg;	
	_bstr_t conn;
	COMTHROW(pMgaContext->m_ccpProject->get_ProjectConnStr(conn.GetAddress()));
	dlg.m_strParentConnection = static_cast<const wchar_t*>(conn);
	dlg.m_strCaption=_T("Attach Library");
	
	if(dlg.DoModal() == IDOK && !dlg.m_strConnString.IsEmpty()) 
	{
		// be flexible in MGA case ( if _T("MGX") is not present )
		if( dlg.m_strConnString.Left(4) == _T("MGX="))
		{
			// add quotes to the folderpath if not there
			if( dlg.m_strConnString.Mid(4,1) != _T("\"") && dlg.m_strConnString.Right(1) != _T("\""))
				dlg.m_strConnString = _T("MGX=\"") + dlg.m_strConnString.Mid( 4) + _T("\"");
		}
		else // MGA is presumed in this case
		{
			if (dlg.relativePath != "")
				dlg.m_strConnString = dlg.relativePath;
		}

		MSGTRY {
			HTREEITEM hItem = m_TreeAggregate.GetFirstSelectedItem();
			if (hItem != NULL) 
			{ 
				
				LPUNKNOWN pUnknown;
				
				if(m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
				{
					
					CComQIPtr<IMgaFolder> ccpFolder(pUnknown);
					if(!ccpFolder)return;
					
					CGMEActiveBrowserApp* pApp = (CGMEActiveBrowserApp*)AfxGetApp();
					CMgaContext* pMgaContext = &pApp->m_CurrentProject.m_MgaContext;
					
					// toggle off Constraint Manager
					CComPtr<IMgaComponentEx> constrMgr;
					if (pMgaContext) {
						constrMgr = pMgaContext->FindConstraintManager();
						if (constrMgr)
							COMTHROW(constrMgr->Enable(VARIANT_FALSE));
						if (pMgaContext->m_ccpProject)
							COMTHROW(pMgaContext->m_ccpProject->Notify(APPEVENT_LIB_ATTACH_BEGIN));
					}

					pMgaContext->BeginTransaction(TRANSACTION_NON_NESTED);

					COMTHROW(ccpFolder->AttachLibraryV3( CComBSTR( dlg.m_strConnString), dlg.m_bOptimized ? VARIANT_TRUE : VARIANT_FALSE, NULL));
					pMgaContext->CommitTransaction();
					
					// toggle back Constraint Manager (done only after commit, so that it will not catch the events happened in the transaction)
					if (pMgaContext) {
						if (constrMgr)
							COMTHROW(constrMgr->Enable(VARIANT_TRUE));
						if(pMgaContext->m_ccpProject)
							COMTHROW(pMgaContext->m_ccpProject->Notify(APPEVENT_LIB_ATTACH_END));
					}

					CComBSTR msg( L"Library attached: ");
					COMTHROW(msg.Append( dlg.m_strConnString));
					COMTHROW(msg.Append( L" (Note: Constraint Manager was turned off during attach to speed it up.)"));
					Utils::put2Console( Utils::get_GME( pMgaContext->m_ccpProject)
						, msg
						, MSG_INFO);
				}
			}
	
		}
		MSGCATCH(_T("Error while trying to attach library"),pMgaContext->AbortTransaction();)	
		Refresh();
	}
}

void CAggregatePropertyPage::RefreshLibrary()
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	HTREEITEM hItem = m_TreeAggregate.GetFirstSelectedItem();
	if (hItem != NULL) 
	{ 
		
		LPUNKNOWN pUnknown;
		
		if(!m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))return;
					
		CComQIPtr<IMgaFolder> ccpFolder(pUnknown);
		if(!ccpFolder)return;

		MSGTRY {
			
			CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
			CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

			pMgaContext->BeginTransaction(true);

			CComBSTR bszLibName;						
			COMTHROW(ccpFolder->get_LibraryName(&bszLibName));
			ASSERT( bszLibName); // it should be a library

			bool was_opt = false;
			if( bszLibName)
				was_opt = CAggregateTreeCtrl::IsUngroupedLibrary( ccpFolder); // was optimized/ungrouped

			pMgaContext->CommitTransaction();
		
			if(!bszLibName)
			{
		
				AfxMessageBox(_T("Selected node is not a library"));
				return;
			}
		
			CAttachLibDlg dlg;
			_bstr_t conn;
			COMTHROW(pMgaContext->m_ccpProject->get_ProjectConnStr(conn.GetAddress()));
			dlg.m_strParentConnection = static_cast<const wchar_t*>(conn);
			dlg.m_strConnString=bszLibName;
			dlg.m_strCaption=_T("Refresh Library");
			dlg.m_bOptimized = was_opt;

			if(dlg.DoModal() == IDOK) 
			{
				pMgaContext->BeginTransaction(TRANSACTION_NON_NESTED);
					
				long errs;
				if (dlg.relativePath != "") {
					COMTHROW(ccpFolder->RefreshLibraryV3( CComBSTR( dlg.relativePath), dlg.m_bOptimized ? VARIANT_TRUE : VARIANT_FALSE, &errs) );
				} else {
					COMTHROW(ccpFolder->RefreshLibraryV3( CComBSTR( dlg.m_strConnString), dlg.m_bOptimized ? VARIANT_TRUE : VARIANT_FALSE, &errs) );
				}
				ccpFolder.Release();
				pMgaContext->CommitTransaction();
			}
		} MSGCATCH(_T("Error while refreshing library"),pMgaContext->AbortTransaction();)	
		Refresh();
	}
}

void CAggregatePropertyPage::LibraryDependencies()
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	HTREEITEM hItem = m_TreeAggregate.GetFirstSelectedItem();
	if (hItem != NULL) 
	{ 
		
		LPUNKNOWN pUnknown;
		
		if(!m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))return;
					
		CComQIPtr<IMgaFolder> ccpFolder(pUnknown);
		if(!ccpFolder)return;

		MSGTRY {
			
			CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
			CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

			pMgaContext->BeginTransaction(false);

			CComBSTR bszLibName;						
			COMTHROW(ccpFolder->get_LibraryName(&bszLibName));
		
			if(!bszLibName)
			{
				AfxMessageBox(_T("Selected node is not a library"));
			}
			else
			{
				CString msg0; CopyTo( bszLibName, msg0);
				msg0 = _T("Library [") + msg0 + _T("] dependencies");

				CComPtr<IMgaFolders> coll_inc, coll_iby;

				COMTHROW( ccpFolder->GetVirtuallyIncludedBy( &coll_iby));
				COMTHROW( ccpFolder->GetVirtuallyIncludes  ( &coll_inc));

				CString msg1, msg2;
				composeInfo( _T("Included virtually in:\n"), coll_iby, msg1);
				composeInfo( _T("Virtually includes:\n"), coll_inc, msg2);

				AfxMessageBox( msg0 + _T("\n\n") + msg1 + _T("\n\n") + msg2, MB_ICONINFORMATION);
			}

			//pMgaContext->CommitTransaction();
			pMgaContext->AbortTransaction();

		} MSGCATCH(_T("Error while viewing library dependencies"),pMgaContext->AbortTransaction();)	
		//Refresh();
	}
}

bool CAggregatePropertyPage::askUserAndDeleteLibrary( CComPtr<IMgaFolder> p_ccpFolder)
{
	if( !p_ccpFolder) return false;
	CComBSTR bszLibName;
	COMTHROW( p_ccpFolder->get_LibraryName(&bszLibName));

	if( !bszLibName)
	{
		ASSERT( 0);
		AfxMessageBox( _T("Selected node is not a library"));
		return false;
	}

	CString ln;
	CopyTo( bszLibName, ln);

	CString deps; CComBSTR depb;
	CComBSTR path( L"_includedBy");

	COMTHROW( p_ccpFolder->get_RegistryValue( path, &depb));

	CopyTo( depb, deps);
	if( deps.IsEmpty())
	{
		deps = _T("No dependent libraries found.");
	}
	else
	{
		deps = _T("Dependent libraries found:\n") + deps;
	}
	deps += _T("\n\nDelete ") + ln + _T("?");
	if( IDYES == AfxMessageBox( deps, MB_YESNO))
	{
		//p_ccpFolder->PutReadOnlyAccess( VARIANT_FALSE);
		COMTHROW(p_ccpFolder->DestroyObject());
		return true;
	}

	return false;
}

void CAggregatePropertyPage::OnContextMenu(CWnd*, CPoint point)
{

	if(!m_TreeAggregate.IsWindowEnabled())return;

	if (point.x == -1 && point.y == -1){
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	MSGTRY{
		CAggregateContextMenu menu(this);
		menu.Run(point);
	} MSGCATCH(_T("Cannot complete operation"),pMgaContext->AbortTransaction();)


}


void CAggregatePropertyPage::OnEndLabelEditTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
	if(!pTVDispInfo->item.pszText)return; // Edit cancelled

	CString strNewLabel(pTVDispInfo->item.pszText);

	CComBSTR bszDispName(strNewLabel);

	HTREEITEM hSelItem=m_TreeAggregate.GetFirstSelectedItem();
	LPUNKNOWN pUnknown;
	if(m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hSelItem,pUnknown))
	{
		CComQIPtr<IMgaObject> ccpMgaObject(pUnknown);
		if(ccpMgaObject)
		{
			// Starting transaction
			CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
			CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

			MSGTRY{
				pMgaContext->BeginTransaction(false);

				bool isLibraryRoot = false;

				// The name is special if folder is a library root
				CComPtr<IMgaFolder>ccpMgaFolder;
				if (SUCCEEDED(ccpMgaObject.QueryInterface(&ccpMgaFolder))) {
					CComBSTR dummyName;
					COMTHROW(ccpMgaFolder->get_LibraryName(&dummyName));
					if (dummyName != NULL) {
						isLibraryRoot = true;
						COMTHROW(ccpMgaFolder->put_LibraryName(bszDispName));
					}
				}

				if (!isLibraryRoot) {
					COMTHROW(ccpMgaObject->put_Name(bszDispName));
				}


				// Checking if it is root folder
				// If it is, change project name as well
				objtype_enum oeType;
				CComPtr<IMgaObject> ccpParent;

				COMTHROW(ccpMgaObject->GetParent(&ccpParent,&oeType));
				
				if(ccpParent==NULL)	// Root Folder
				{
					COMTHROW( pMgaContext->m_ccpProject->put_Name(bszDispName));
				}

				// Ending transaction
				pMgaContext->CommitTransaction();

				if( !ccpParent) // parentless, means> Root Folder 
				{
					// FireRootFolderNameChanged event fired
					CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
					pApp->GetCtrl()->FireRootFolderNameChanged();
				}

			}MSGCATCH(_T("Error retrieving object name"),pMgaContext->AbortTransaction();)	
		}
	}


	*pResult = 0;
}

#define DEBUG_EVENTS


#ifdef DEBUG_EVENTS
 #define EVENT_TRACE(x) TRACE(x)
#else
 #define EVENT_TRACE(x)
#endif 


// This function is indirectly initiated by MGA, 
// so we do not need transaction handling
void CAggregatePropertyPage::OnMgaEvent(CComPtr<IMgaObject> ccpMgaObject, unsigned long lEventMask)
{
	EVENT_TRACE("\n__________ MGA Event Handling Begin _________\n");

	// Handling Object Events

	if(lEventMask&OBJEVENT_PROPERTIES)
	{
		EVENT_TRACE("OBJEVENT_PROPERTIES handled.\n");
		
		// Finding the object in the tree
		HTREEITEM hItem;
		
		if(m_TreeAggregate.m_MgaMap.LookupTreeItem(ccpMgaObject,hItem)) // The changed object is in the tree
		{
			// Change name
			m_TreeAggregate.SetItemText(hItem,GetDisplayedName(ccpMgaObject));
			m_TreeAggregate.SortItems();

			// Setting the state icons
			m_TreeAggregate.SetItemProperties(hItem);

		}
	}

	if(lEventMask&OBJEVENT_CREATED)
	{
		EVENT_TRACE("OBJEVENT_CREATED handled.\n");

		CComPtr<IMgaObject> ccpParent;
		objtype_enum otParentType;
		//objtype_enum otMgaObjectType;
		COMTHROW( ccpMgaObject->GetParent(&ccpParent,&otParentType) );
		if(ccpParent)
		{
			// Finding the parent in the tree
			HTREEITEM hParentItem;
			
			if(m_TreeAggregate.m_MgaMap.LookupTreeItem(ccpParent,hParentItem)) // The parent of the created object is in the tree
			{
				// Setting parent state
				if(m_Options.m_bIsDynamicLoading &&
					!(m_TreeAggregate.GetItemState(hParentItem,TVIS_EXPANDED)&TVIS_EXPANDED)) 
				{
					TVITEM tvItem;
					tvItem.hItem=hParentItem;
					tvItem.mask=TVIF_CHILDREN|TVIF_HANDLE;
					tvItem.cChildren=1;
					m_TreeAggregate.SetItem(&tvItem);

					if(m_bIsBrowserCreatedNewObject)
					{
						// Deleting TVIS_EXPANDEDONCE otherwise parent does not
						// get notification
						m_TreeAggregate.CTreeCtrl::SetItemState(hParentItem,0,TVIS_EXPANDEDONCE);
						m_TreeAggregate.Expand(hParentItem,TVE_EXPAND);
						HTREEITEM hNewItem;
						if(m_TreeAggregate.m_MgaMap.LookupTreeItem(ccpMgaObject,hNewItem))
						{
							m_TreeAggregate.EnsureVisible(hNewItem);
							m_TreeAggregate.ClearSelection(TRUE);
							m_TreeAggregate.SelectItem(hNewItem);
							m_TreeAggregate.SetFocus();			
							m_TreeAggregate.Invalidate();
							m_TreeAggregate.EditLabel(hNewItem);
						}
					}
		

				}
				else
				{
					if(m_bIsBrowserCreatedNewObject||
						m_TreeAggregate.GetItemState(hParentItem,TVIS_EXPANDED)&TVIS_EXPANDED) 
					{
						BOOL bisBrwCreNwObj = m_bIsBrowserCreatedNewObject; // save the value
						Refresh(); // will reset m_bIsBrowserCreatedNewObject

						// inserted to correct the behaviour that labels were edited
						// by default only when 
						//    1. the first child has been created in a container
						//    2. a child was created in a container, but the container was not expanded
						// Now it will always edit the label!
						HTREEITEM hNewItem;
						if( bisBrwCreNwObj // browser created new object
						 && m_TreeAggregate.m_MgaMap.LookupTreeItem(ccpMgaObject,hNewItem))
						{
							m_TreeAggregate.EnsureVisible(hNewItem);
							m_TreeAggregate.ClearSelection(TRUE);
							m_TreeAggregate.SelectItem(hNewItem);
							m_TreeAggregate.SetFocus();			
							m_TreeAggregate.Invalidate();
							m_TreeAggregate.EditLabel(hNewItem);
						}
						// end of insertion by Zolmol

						/*COMTHROW( ccpMgaObject->get_ObjType(&otMgaObjectType) );
						HTREEITEM hNewItem=m_TreeAggregate.InsertItem(hParentItem,GetDisplayedName(ccpMgaObject),
														ccpMgaObject,otMgaObjectType);
					
						m_TreeAggregate.SortItems(hParentItem);
						// If the browser created the object select it, and offer editing
						if(m_bIsBrowserCreatedNewObject)
						{			
							m_TreeAggregate.EnsureVisible(hNewItem);
							m_TreeAggregate.SetFocus();			
							m_TreeAggregate.SelectItem(hNewItem);
							m_TreeAggregate.Invalidate(TRUE);							
							m_TreeAggregate.EditLabel(hNewItem);//->MoveWindow(rectItem,TRUE);
						}	*/
					
					}
				}
			}
		}
		m_bIsBrowserCreatedNewObject=FALSE;
	}
	if(lEventMask&OBJEVENT_DESTROYED)
	{
		EVENT_TRACE("OBJEVENT_DESTROYED handled.\n");
		
		// Finding the object in the tree
		HTREEITEM hItem = NULL, hParentItem = NULL;
		
		if(m_TreeAggregate.m_MgaMap.LookupTreeItem(ccpMgaObject,hItem)) // The deleted object is in the tree
		{	
			// Remember the parent
			hParentItem=m_TreeAggregate.GetParentItem(hItem);
			// Remove item			
			m_TreeAggregate.DeleteItem(hItem);
		}
		
		CComBSTR id;
		ccpMgaObject->get_ID(&id);
		auto highlightIt = m_TreeAggregate.m_highlightedObjects.find(id);
		if (highlightIt != m_TreeAggregate.m_highlightedObjects.end())
		{
			m_TreeAggregate.m_highlightedObjects.erase(highlightIt);
		}

		// If the parent is in the object tree and there is dynamic loading
		// we set the +/- button
		if(m_Options.m_bIsDynamicLoading)
		{
			if(hParentItem) // The parent of the deleted object is in the tree
			{
				TVITEM tvItem;
				tvItem.hItem=hParentItem;
				tvItem.mask=TVIF_CHILDREN|TVIF_HANDLE;
				tvItem.cChildren=HasDisplayedChild(hParentItem);

				// Setting icons
				int nImage,nSelectedImage;
				m_TreeAggregate.GetItemImage(hParentItem,nImage,nSelectedImage);				

				bool bIsExpanded =!!(m_TreeAggregate.GetItemState(hParentItem,TVIS_EXPANDED)&TVIS_EXPANDED);
				if (nImage >= ICON_NUMBER * 8)
				{
					bIsExpanded &= !!tvItem.cChildren;
					m_TreeAggregate.SetItemImage(hParentItem, (nImage & ~1) + (bIsExpanded ? 1 : 0), (nImage & ~1) + (bIsExpanded ? 1 : 0));
				}
				else
				{
					if(tvItem.cChildren)
					{
						if(bIsExpanded && nImage<ICON_NUMBER)
						{
							m_TreeAggregate.SetItemImage(hParentItem,nImage+ICON_NUMBER,nSelectedImage+ICON_NUMBER);
						}
					}
					else
					{	if (nImage>=ICON_NUMBER)
						{
							m_TreeAggregate.SetItemImage(hParentItem,nImage-ICON_NUMBER,nSelectedImage-ICON_NUMBER);
						}
					}
				}

				m_TreeAggregate.SetItem(&tvItem);
			}
		}

	}
	if(lEventMask&OBJEVENT_PARENT)
	{
		EVENT_TRACE("OBJEVENT_PARENT handled.\n");
				
		// Finding the object in the tree
		HTREEITEM hItem, hParentItem;
		
		if(m_TreeAggregate.m_MgaMap.LookupTreeItem(ccpMgaObject,hItem)) // The deleted object is in the tree
		{	
			// Remember the parent
			hParentItem=m_TreeAggregate.GetParentItem(hItem);

			// Remove the children of the item
			if(m_Options.m_bIsDynamicLoading) // inserted by ZolMol to correct the bug when a model/folder is moved elsewhere in expanded state
				m_TreeAggregate.DeleteAllChildren( hItem);

			// Remove item
			m_TreeAggregate.DeleteItem(hItem);
		}
		// If the parent is in the object tree and there is dynamic loading
		// we set the +/- button
		if(m_Options.m_bIsDynamicLoading)
		{
			if(hParentItem) // The parent of the deleted object is in the tree
			{
				TVITEM tvItem;
				tvItem.hItem=hParentItem;
				tvItem.mask=TVIF_CHILDREN|TVIF_HANDLE;
				tvItem.cChildren=HasDisplayedChild(hParentItem);
	
				// Setting icons
				int nImage,nSelectedImage;
				m_TreeAggregate.GetItemImage(hParentItem,nImage,nSelectedImage);

				bool bIsExpanded = !!(m_TreeAggregate.GetItemState(hParentItem,TVIS_EXPANDED)&TVIS_EXPANDED);
				if (nImage >= ICON_NUMBER * 8)
				{
					bIsExpanded &= !!tvItem.cChildren;
					m_TreeAggregate.SetItemImage(hParentItem, (nImage & ~1) + (bIsExpanded ? 1 : 0), (nImage & ~1) + (bIsExpanded ? 1 : 0));
				}
				else
				{
					if(tvItem.cChildren)
					{
						if(bIsExpanded && nImage<ICON_NUMBER)
						{
							m_TreeAggregate.SetItemImage(hParentItem,nImage+ICON_NUMBER,nSelectedImage+ICON_NUMBER);
						}
					}
					else
					{	
						if(nImage>=ICON_NUMBER)
						{
							m_TreeAggregate.SetItemImage(hParentItem,nImage-ICON_NUMBER,nSelectedImage-ICON_NUMBER);
						}
					}
				}

				m_TreeAggregate.SetItem(&tvItem);
			}
		}	
	}

	if(lEventMask&OBJEVENT_NEWCHILD)
	{
		EVENT_TRACE("OBJEVENT_NEWCHILD handled.\n");

		HTREEITEM hParentItem;
		if(m_TreeAggregate.m_MgaMap.LookupTreeItem(ccpMgaObject,hParentItem))
		{
			if(m_Options.m_bIsDynamicLoading)
			{
				bool bIsExpanded=!!(m_TreeAggregate.GetItemState(hParentItem,TVIS_EXPANDED)&TVIS_EXPANDED);
				if(bIsExpanded)
				{
					reqBuildAggregateTree(hParentItem,ccpMgaObject,2,0,true);
				}

				TVITEM tvItem;
				tvItem.hItem=hParentItem;
				tvItem.mask=TVIF_CHILDREN|TVIF_HANDLE;
				tvItem.cChildren=HasDisplayedChild(hParentItem);

				// Setting icons
				int nImage,nSelectedImage;
				m_TreeAggregate.GetItemImage(hParentItem,nImage,nSelectedImage);				

				if (nImage >= ICON_NUMBER * 8)
				{
					m_TreeAggregate.SetItemImage(hParentItem, (nImage & ~1) + (bIsExpanded ? 1 : 0), (nImage & ~1) + (bIsExpanded ? 1 : 0));
				}
				else
				{
					if(tvItem.cChildren)
					{
						if(bIsExpanded && nImage<ICON_NUMBER)
						{
							m_TreeAggregate.SetItemImage(hParentItem,nImage+ICON_NUMBER,nSelectedImage+ICON_NUMBER);
						}
					}
					else
					{	if(nImage>=ICON_NUMBER)
						{
							m_TreeAggregate.SetItemImage(hParentItem,nImage-ICON_NUMBER,nSelectedImage-ICON_NUMBER);
						}

					}
				}

				m_TreeAggregate.SetItem(&tvItem);
			}
			else
			{
				Refresh();
			}
		}
	}

	if(lEventMask&OBJEVENT_LOSTCHILD)
	{
		EVENT_TRACE("OBJEVENT_LOSTCHILD is partially handled.\n");		

		// Finding the object in the tree
		HTREEITEM hParentItem = NULL;
		
		if(m_TreeAggregate.m_MgaMap.LookupTreeItem(ccpMgaObject,hParentItem))		// If parent is in the tree
		{
			if(m_Options.m_bIsDynamicLoading)
			{
				bool bIsExpanded=!!(m_TreeAggregate.GetItemState(hParentItem,TVIS_EXPANDED)&TVIS_EXPANDED);
				if(!bIsExpanded)	// If the parent was expanded, the whole stuff was handled by OBJECT_DESTROYED
				{
					TVITEM tvItem;
					tvItem.hItem=hParentItem;
					tvItem.mask=TVIF_CHILDREN|TVIF_HANDLE;
					tvItem.cChildren=reqHasDisplayedChild(ccpMgaObject);

					// Setting icons
					int nImage,nSelectedImage;
					m_TreeAggregate.GetItemImage(hParentItem,nImage,nSelectedImage);				

					if (nImage >= ICON_NUMBER * 8)
					{
						bIsExpanded &= !!tvItem.cChildren;
						m_TreeAggregate.SetItemImage(hParentItem, (nImage & ~1) + (bIsExpanded ? 1 : 0), (nImage & ~1) + (bIsExpanded ? 1 : 0));
					}
					else
					// if no children then if opened icon used previously, change it to the closed icon
					{	
						if(nImage>=ICON_NUMBER)
						{
							m_TreeAggregate.SetItemImage(hParentItem,nImage-ICON_NUMBER,nSelectedImage-ICON_NUMBER);
						}
					}

					m_TreeAggregate.SetItem(&tvItem);
				}
			}
		}
	}


	if(lEventMask&OBJEVENT_REGISTRY)
	{
		EVENT_TRACE("OBJEVENT_REGISTRY is partially  handled.\n");

		// PETER: We need this, because of Library names

		// Finding the object in the tree
		HTREEITEM hItem;
		
		if(m_TreeAggregate.m_MgaMap.LookupTreeItem(ccpMgaObject,hItem)) // The changed object is in the tree
		{
			// Change name
			m_TreeAggregate.SetItemText(hItem,GetDisplayedName(ccpMgaObject));
			m_TreeAggregate.SortItems();

			// Setting the state icons
			m_TreeAggregate.SetItemProperties(hItem);

		}
	}

	
#ifdef _DEBUG
	// Messages that are not handled are not included in release, 
	// but useful for further development, so left here

	if(lEventMask&OBJEVENT_ATTR)
	{
		EVENT_TRACE("OBJEVENT_ATTR not handled.\n");
	}
	
	if(lEventMask&OBJEVENT_RELATION)
	{
		EVENT_TRACE("OBJEVENT_RELATION not handled.\n");
	}
	if(lEventMask&OBJEVENT_SUBT_INST)
	{
		EVENT_TRACE("OBJEVENT_SUBT_INST not handled.\n");
	}
	if(lEventMask&OBJEVENT_REFERENCED)
	{
		EVENT_TRACE("OBJEVENT_REFERENCED not handled.\n");
	}
	if(lEventMask&OBJEVENT_CONNECTED)
	{
		EVENT_TRACE("OBJEVENT_CONNECTED not handled.\n");
	}
	if(lEventMask&OBJEVENT_SETINCLUDED)
	{
		EVENT_TRACE("OBJEVENT_SETINCLUDED not handled.\n");
	}
	if(lEventMask&OBJEVENT_REFRELEASED)
	{
		EVENT_TRACE("OBJEVENT_REFRELEASED not handled.\n");
	}
	if(lEventMask&OBJEVENT_DISCONNECTED)
	{
		EVENT_TRACE("OBJEVENT_DISCONNECTED not handled.\n");
	}
	if(lEventMask&OBJEVENT_SETEXCLUDED)
	{
		EVENT_TRACE("OBJEVENT_SETEXCLUDED not handled.\n");
	}
	if(lEventMask&OBJEVENT_USERBITS)
	{
		EVENT_TRACE("OBJEVENT_USERBITS not handled.\n");
	}
	if(lEventMask&OBJEVENT_CLOSEMODEL)
	{
		EVENT_TRACE("OBJEVENT_CLOSEMODEL not handled.\n");
	}

	EVENT_TRACE("_________ MGA Event Handling End _______\n");
#endif /*_DEBUG */

}


void CAggregatePropertyPage::GotoIUnk(BSTR Id)
{
	bool worked = false;
	IUnknown *pUnknown = NULL;
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
	MSGTRY{
		// Starting transaction
		pMgaContext->BeginTransaction();
		HTREEITEM hNewItem;
		if(m_TreeAggregate.m_MgaMap.SearchTreeItem(Id,hNewItem, pUnknown))
		{
						m_TreeAggregate.EnsureVisible(hNewItem);
						m_TreeAggregate.ClearSelection(TRUE);
						m_TreeAggregate.SelectItem(hNewItem);
						m_TreeAggregate.SetFocus();			
						m_TreeAggregate.Invalidate();
						worked = true;
		}
		else  // expand the tree until item reached
		{
			_bstr_t id;
			_bstr_t bId = Id;
			CComPtr<IMgaObject> iitem;
			CComPtr<IMgaFCO> fiitem;
			std::vector<_bstr_t> item;
			int itemcount = 0;
			CComPtr<IMgaObject> parent;
			
			pMgaContext->BeginTransaction();
			CComPtr<IMgaProject> project = pMgaContext->m_ccpProject;
			COMTHROW(project->GetFCOByID(bId, &fiitem));
			COMTHROW(fiitem.QueryInterface(&iitem));
			item.push_back(Id);
			itemcount++;
			do
			{
				objtype_enum objtype;
				COMTHROW(iitem->GetParent(&parent, &objtype));
				_bstr_t bstrid;
				COMTHROW(parent->get_ID(bstrid.GetAddress()));
				id = bstrid;
				item.push_back(id);
				itemcount++;
				iitem = parent;
				parent.Release();
			} while (!m_TreeAggregate.m_MgaMap.SearchTreeItem(id,hNewItem, pUnknown));
			pMgaContext->CommitTransaction();
			// open the parents in backward order
			worked = true;
			m_TreeAggregate.EnsureVisible(hNewItem);
			m_TreeAggregate.ClearSelection(TRUE);
			m_TreeAggregate.SelectItem(hNewItem);
			m_TreeAggregate.SendMessage(WM_KEYDOWN, (WPARAM)107, (LPARAM)0);
			for (itemcount-=2; itemcount > 0; itemcount--)
			{
				if (m_TreeAggregate.m_MgaMap.SearchTreeItem(item[itemcount],hNewItem, pUnknown))
				{
					m_TreeAggregate.EnsureVisible(hNewItem);
					m_TreeAggregate.ClearSelection(TRUE);
					m_TreeAggregate.SelectItem(hNewItem);
					m_TreeAggregate.SendMessage(WM_KEYDOWN, (WPARAM)107, (LPARAM)0);
				}
			}
			if (m_TreeAggregate.m_MgaMap.SearchTreeItem(item[0],hNewItem, pUnknown))
			{
				m_TreeAggregate.EnsureVisible(hNewItem);
				m_TreeAggregate.ClearSelection(TRUE);
				m_TreeAggregate.SelectItem(hNewItem);
			}
		}
		pMgaContext->CommitTransaction ();								
	}MSGCATCH(_T("Error completing the operation"),pMgaContext->AbortTransaction();)	
	if (worked)
		pApp->GetCtrl()->FireClickMgaObject(pUnknown);

}

void CAggregatePropertyPage::GotoIUnkPtr( IMgaObject * p_obj)
{
	if( !p_obj) return;

	bool worked = false;
	IUnknown *pUnknown = NULL;

	CGMEActiveBrowserApp* pApp = (CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext = &pApp->m_CurrentProject.m_MgaContext;

	MSGTRY
	{
		// Starting transaction
		pMgaContext->BeginTransaction();
		HTREEITEM hNewItem;

		CComPtr<IMgaObject>  obj        ( p_obj);
		COMTHROW( p_obj->QueryInterface( &pUnknown));

		if( !obj) 
			throw hresult_exception( E_INVALIDARG);

		//if(m_TreeAggregate.m_MgaMap.SearchTreeItem(Id,hNewItem, pUnknown))
		if( m_TreeAggregate.m_MgaMap.LookupTreeItem( pUnknown, hNewItem))
		{
						m_TreeAggregate.EnsureVisible(hNewItem);
						m_TreeAggregate.ClearSelection(TRUE);
						m_TreeAggregate.SelectItem(hNewItem);
						m_TreeAggregate.SetFocus();			
						m_TreeAggregate.Invalidate();
						worked = true;
		}
		else // expand the tree until item reached
		{
			//CComPtr<IMgaProject> project = pMgaContext->m_ccpProject;
			CComPtr<IMgaObject> parent;
			CComPtr<IMgaObject> iitem = obj;

			// coll of ptrs
			CComObjPtr<IMgaObjects> coll;
			COMTHROW( coll.CoCreateInstance( L"Mga.MgaObjects"));
			if( !coll) 
				throw hresult_exception( E_INVALIDARG);

			// strings of ids
			_bstr_t id;
			_bstr_t item[ 100];
			int     itemcount = 0;

			pMgaContext->BeginTransaction(true); // read-only transaction

			// coll of ptrs
			COMTHROW( coll->Append( obj));

			// strings of ids
			CComBSTR Id;
			COMTHROW( obj->get_ID( &Id));
			item[ itemcount++] = Id;

			do
			{
				objtype_enum objtype;
				COMTHROW(iitem->GetParent(&parent, &objtype));

				// coll of ptrs
				COMTHROW( coll->Append( parent));

				// strings of ids
				BSTR bstrid = NULL;
				COMTHROW(parent->get_ID(&bstrid));
				id = bstrid;
				item[itemcount++] = id;

				iitem = parent;
				parent.Release();
			} while( !m_TreeAggregate.m_MgaMap.LookupTreeItem( iitem, hNewItem));
			//} while( !m_TreeAggregate.m_MgaMap.SearchTreeItem( (wchar_t*)((char*)id), hNewItem, pUnknown));

			pMgaContext->CommitTransaction();
			
			// open the parents in backward order
			worked = true;
			m_TreeAggregate.EnsureVisible(hNewItem);
			m_TreeAggregate.ClearSelection(TRUE);
			m_TreeAggregate.SelectItem(hNewItem);
			m_TreeAggregate.SendMessage(WM_KEYDOWN, (WPARAM)107, (LPARAM)0);
			for (itemcount-=2; itemcount > 0; itemcount--) // valid itemcount range will be: from last-1 to 1
			{
				CComPtr<IMgaObject> obj_i;
				COMTHROW( coll->get_Item( itemcount + 1, &obj_i)); // plus 1

				//if (m_TreeAggregate.m_MgaMap.SearchTreeItem((wchar_t*)((char*)item[itemcount]),hNewItem, pUnknown))
				if( m_TreeAggregate.m_MgaMap.LookupTreeItem( obj_i, hNewItem))
				{
					m_TreeAggregate.EnsureVisible( hNewItem);
					m_TreeAggregate.ClearSelection( TRUE);
					m_TreeAggregate.SelectItem( hNewItem);
					m_TreeAggregate.SendMessage( WM_KEYDOWN, (WPARAM)107, (LPARAM)0);
				}
			}
			
			CComPtr<IMgaObject> obj_1st;
			COMTHROW( coll->get_Item( 1, &obj_1st));
			//if (m_TreeAggregate.m_MgaMap.SearchTreeItem(item[0],hNewItem, pUnknown))
			if( m_TreeAggregate.m_MgaMap.LookupTreeItem( obj_1st, hNewItem))
			{
				m_TreeAggregate.EnsureVisible( hNewItem);
				m_TreeAggregate.ClearSelection( TRUE);
				m_TreeAggregate.SelectItem( hNewItem);
			}
		}
		pMgaContext->CommitTransaction();
	}
	MSGCATCH( _T("Error completing the operation"), pMgaContext->AbortTransaction();)

	if( worked)
		pApp->GetCtrl()->FireClickMgaObject( pUnknown);
}


void CAggregatePropertyPage::Refresh()
{

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
	
	MSGTRY{
		// Starting transaction
		pMgaContext->BeginTransaction();

		m_TreeAggregate.StoreState();

		m_TreeAggregate.CleanUp();
		m_ComboSearchCtrl.ResetContent();

		m_bIsBrowserCreatedNewObject=FALSE;
		SetupTree();
		m_TreeAggregate.SortItems();

		m_TreeAggregate.RestoreState();
	}MSGCATCH(_T("Error refreshing tree browser data"),pMgaContext->AbortTransaction();)	
	// Ending transaction
	pMgaContext->CommitTransaction();


}

void CAggregatePropertyPage::OnKeyDownTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	switch(pTVKeyDown->wVKey)
	{
	case VK_DELETE: // Delete selected items
		{
			CAggregateContextMenu menu(this);
			menu.OnEditDelete();
		}break;
	case VK_RETURN: // Display the first selected item in GME editor
		{
				HTREEITEM hItem=m_TreeAggregate.GetSelectedItem();
				LPUNKNOWN pUnknown=NULL;

				if(m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
				{
					TRACE(m_TreeAggregate.GetItemText(hItem));

					// Firing the event
					CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
					if( ::GetKeyState( VK_SHIFT) < 0)
						pApp->GetCtrl()->FireShowInParentMgaObject(pUnknown);
					else
						pApp->GetCtrl()->FireDblClickMgaObject(pUnknown);
					m_TreeAggregate.SetFocus();
				}
		}break;
	case VK_TAB:
		{
			if( ::GetKeyState( VK_CONTROL) < 0) {
				m_parent->nextTab( ::GetKeyState( VK_SHIFT) >= 0);
			}
			else {
				m_ComboEditCtrl.SetFocus();
			}
		}break;
	case 0x43: //VK_C
		{
			if( ::GetKeyState( VK_CONTROL) < 0) {
				this->DoCopy();
			}
		}break;
	case 0x59: //VK_Y
		{
			if( ::GetKeyState( VK_CONTROL) < 0) {
				CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
				CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
				
				short nRedoSize,nUndoSize;
				COMTHROW(pMgaContext->m_ccpProject->UndoRedoSize(&nUndoSize,&nRedoSize));

				if( nRedoSize > 0)
				{
					COMTHROW(pMgaContext->m_ccpProject->Redo());
					m_TreeAggregate.SetFocus();
				}
			}
		}break;
	case 0x5A: // VK_Z
		{
			if( ::GetKeyState( VK_CONTROL) < 0) {
				CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
				CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
				
				short nRedoSize,nUndoSize;
				COMTHROW(pMgaContext->m_ccpProject->UndoRedoSize(&nUndoSize,&nRedoSize));

				if( nUndoSize > 0)
				{
					COMTHROW(pMgaContext->m_ccpProject->Undo());
					m_TreeAggregate.SetFocus();
				}
			}
		}break;
	case 0x56: //VK_V
		{
			if( ::GetKeyState( VK_CONTROL) < 0) {
				COleDataObject OleDataObject;
				CString msg;
				if( OleDataObject.AttachClipboard()) {
					auto op = DRAGOP_COPY;
					if (::GetKeyState(VK_SHIFT) & 0x8000)
					{
						op = DRAGOP_REFERENCE;
					}
					if( m_TreeAggregate.DoDrop(op, &OleDataObject, CPoint( 0, 0))) {
						// what about set focus back
						m_TreeAggregate.SetFocus();
					}
				} else msg = _T("Cannot recover data from the clipboard.");

				if( !msg.IsEmpty()) MessageBox( msg, _T("Paste Error"), MB_ICONERROR);
			}
		}break;
	case 0x46: //VK_F:
		{
			if( ::GetKeyState( VK_CONTROL) < 0) { // launch Find Box
				CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
				pApp->GetCtrl()->FireShowFindDlg();
			}
		}break;
	case 0x41: // VK_A
	case 0x49: // VK_I
		{
			if( ::GetKeyState( VK_CONTROL) < 0) { // launch ObjectInspector Box
				CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
				pApp->GetCtrl()->FireShowObjInspDlg();
			}
			else if( ::GetKeyState( VK_MENU) < 0) { // alt pressed
				CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
				pApp->GetCtrl()->FireCycleObjInsp( (::GetKeyState( VK_SHIFT) >= 0)? VARIANT_TRUE:VARIANT_FALSE);
			}
		}break;
	case VK_F2: // rename
		{
				HTREEITEM hItem=m_TreeAggregate.GetSelectedItem();
				m_TreeAggregate.EditLabel(hItem);
		}break;
	}
	
	*pResult = 0;
}

void CAggregatePropertyPage::OnGetDispInfoTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

/*
	// Children callback
	if(pTVDispInfo->item.mask&TVIF_CHILDREN)
	{
		if(m_Options.m_bIsDynamicLoading)
		{
			if(HasDisplayedChild(pTVDispInfo->item.hItem))
			{
				pTVDispInfo->item.cChildren = 1;
			}
			else
			{
				pTVDispInfo->item.cChildren = 0;
			}
		}
	}
*/	
	*pResult = 0;
}

BOOL CAggregatePropertyPage::HasDisplayedChild(HTREEITEM hItem)
{
	
	LPUNKNOWN pUnknown=NULL;


	if(m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		CComQIPtr<IMgaObject> ccpMgaObject(pUnknown);
		if(ccpMgaObject)
		{
			long status;
			COMTHROW(ccpMgaObject->get_Status(&status));
			if (status != OBJECT_EXISTS) {
				return FALSE;
			}

			return reqHasDisplayedChild(ccpMgaObject);
		}
	}
	return FALSE;


}


BOOL CAggregatePropertyPage::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CAggregatePropertyPage::OnSearch() 
{
	AfxMessageBox(_T("Pushed."));	
}

BOOL CAggregatePropertyPage::OnSetActive() 
{
	CActiveBrowserPropertySheet* pParent=(CActiveBrowserPropertySheet*)GetParent();
	pParent->m_PageInheritance.ResetRoot();
	pParent->SendMessage(DM_SETDEFID,IDC_SEARCH,0);
	::SetFocus(GetDlgItem(IDC_SEARCH)->GetSafeHwnd());
	return CPropertyPage::OnSetActive();
}


void CAggregatePropertyPage::OnBeginDragTreeAggregate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;


	MSGTRY{
		DoDrag(pNMTreeView->ptDrag);
	} MSGCATCH(_T("Error while dragging"),pMgaContext->AbortTransaction();)
	

	*pResult = 0;
}












/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///	CInheritancePropertyPage property page
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////











CInheritancePropertyPage::CInheritancePropertyPage() 
	: CPropertyPage(CInheritancePropertyPage::IDD)
	, m_ComboEditCtrl(&m_ComboSearchCtrl)
	, m_ComboSearchCtrl(&m_TreeInheritance)
	, m_theCurrentRootFCO( NULL)
	, m_pPrevSelectedItem( NULL)
{
	//{{AFX_DATA_INIT(CInheritancePropertyPage)
	//}}AFX_DATA_INIT
}

CInheritancePropertyPage::~CInheritancePropertyPage()
{
	m_theCurrentRootFCO = NULL;
	m_pPrevSelectedItem = NULL;
	m_ImageList.DeleteImageList();
	m_StateImageList.DeleteImageList();
	VERIFY( m_ImageList.GetSafeHandle() == NULL);
	VERIFY( m_StateImageList.GetSafeHandle() == NULL);
}

void CInheritancePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInheritancePropertyPage)
	DDX_Control(pDX, IDC__COMBO_SEARCH_INHERITANCE, m_ComboSearchCtrl);
	DDX_Control(pDX, IDC_TREE_INHERITANCE, m_TreeInheritance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInheritancePropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CInheritancePropertyPage)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_INHERITANCE, OnSelChangedTreeInheritance)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_INHERITANCE, OnDblclkTreeInheritance)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_INHERITANCE, OnItemExpandingTreeInheritance)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_INHERITANCE, OnKeyDownTreeInheritance)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CInheritancePropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	
	m_TreeInheritance.ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT
								|TVS_SHOWSELALWAYS );

	m_ImageList.Create(16,16,ILC_MASK|ILC_COLOR24,0,0);
	CBitmap bm;
	bm.LoadBitmap(IDB_IMAGELIST_INHERITANCE);
	m_ImageList.Add(&bm,RGB(128, 158, 8));

	m_TreeInheritance.SetImageList(&m_ImageList,TVSIL_NORMAL);

	// Creating the state image list
	PrepareStateImageList();

	m_TreeInheritance.EnableWindow(FALSE);
	m_ComboSearchCtrl.EnableWindow(FALSE);

	// Get edit control which happens to be the first child window
	// and subclass it
	m_ComboEditCtrl.SubclassWindow(m_ComboSearchCtrl.GetWindow(GW_CHILD)->GetSafeHwnd());


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInheritancePropertyPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	if(	::IsWindow(m_ComboSearchCtrl.GetSafeHwnd()) )
	{
		int ySpace = GetSystemMetrics(SM_CYBORDER);
		RECT clientRect;
		m_ComboSearchCtrl.GetClientRect(&clientRect);
		
		m_TreeInheritance.MoveWindow(0, clientRect.bottom + 2*ySpace, cx-1, cy-clientRect.bottom - 2*ySpace - 1);
		m_ComboSearchCtrl.MoveWindow(0, ySpace, cx-1, cy-1);
	}
	
}

void CInheritancePropertyPage::OpenProject()
{
	m_TreeInheritance.EnableWindow(TRUE);
	m_ComboSearchCtrl.EnableWindow(TRUE);
	m_theCurrentRootFCO = NULL;
	m_pPrevSelectedItem=NULL;

}

void CInheritancePropertyPage::CloseProject()
{
	m_TreeInheritance.CleanUp();
	m_TreeInheritance.EnableWindow(FALSE);
	
	m_ComboSearchCtrl.ResetContent();
	m_ComboSearchCtrl.EnableWindow(FALSE);

	m_theCurrentRootFCO = NULL;
	m_pPrevSelectedItem=NULL;

}

void CInheritancePropertyPage::PrepareStateImageList()
{
	CImageList ImageList;
	ImageList.Create(16,16,ILC_MASK|ILC_COLOR24,0,0);

	CBitmap bm;
	bm.LoadBitmap(IDB_IMAGELIST_AGGREGATE_STATE);

	ImageList.Add(&bm,RGB(128, 158, 8));	// Pretty natural transparent color 
											// provided by Tamas Paka

	CClientDC dcClient(this);
	CDC dcMem;
	CBitmap Bitmap;

	if (!dcMem.CreateCompatibleDC(&dcClient)) return;

	if (!Bitmap.CreateCompatibleBitmap(&dcClient,256,16)) return;

	CBitmap *pOldMemDCBitmap = dcMem.SelectObject(&Bitmap);

	COLORREF cMaskColor=RGB(128,58,8);
	dcMem.FillSolidRect(0, 0, 256, 16, cMaskColor);

	// Paint each Image in the DC
	for(BYTE i=0;i<=15;i++)
	{
		if(i&0x0001) // Access
		{
			ImageList.Draw(&dcMem, 1, CPoint(i*16,0), ILD_TRANSPARENT);
		}

		if(i&0x0002) // LibObject
		{
			ImageList.Draw(&dcMem, 0, CPoint(i*16,0), ILD_TRANSPARENT);
		}
		if(i&0x0004) // Instance
		{
			ImageList.Draw(&dcMem, 5, CPoint(i*16,0), ILD_TRANSPARENT);
		}

		if(i&0x0008)	// Derived (subtype)
		{
			ImageList.Draw(&dcMem, 4, CPoint(i*16,0), ILD_TRANSPARENT);
		}
	}
	dcMem.SelectObject(pOldMemDCBitmap);

	m_StateImageList.Create(16,16, ILC_COLOR | ILC_MASK, 0, 1);

	// Green is used as mask color
	m_StateImageList.Add(&Bitmap, cMaskColor); 

	Bitmap.DeleteObject();

	m_TreeInheritance.SetImageList(&m_StateImageList,TVSIL_STATE);
}

// This function is indirectly initiated by MGA, 
// so we do not need transaction handling
void CInheritancePropertyPage::OnMgaEvent(CComPtr<IMgaObject> ccpMgaObject, unsigned long lEventMask)
{
	if( !m_theCurrentRootFCO) return;
	EVENT_TRACE("\n__________ MGA Event Handling Begin _________\n");

	// Handling Object Events

	if(lEventMask&OBJEVENT_CREATED)
	{
		EVENT_TRACE("OBJEVENT_CREATED handled.\n");

		m_TreeInheritance.StoreState();
		m_TreeInheritance.CleanUp();
		SetupTree2();
		m_TreeInheritance.RestoreState();
	}

	if(lEventMask&OBJEVENT_DESTROYED)
	{
		EVENT_TRACE("OBJEVENT_DESTROYED handled.\n");

		m_TreeInheritance.StoreState();
		m_TreeInheritance.CleanUp();
		SetupTree2();
		m_TreeInheritance.RestoreState();
	}

	if(lEventMask&OBJEVENT_PROPERTIES)
	{
		EVENT_TRACE("OBJEVENT_PROPERTIES handled.\n");

		// Finding the object in the tree
		HTREEITEM hItem;
		if(m_TreeInheritance.m_MgaMap.LookupTreeItem(ccpMgaObject,hItem)) // The changed object is in the tree
		{
			// Change name
			m_TreeInheritance.SetItemText(hItem,GetDisplayedName(ccpMgaObject));

			// Setting the state icons
			//m_TreeInheritance.SetItemProperties(hItem);
		}
	}

	if(lEventMask&OBJEVENT_REGISTRY)
	{
		EVENT_TRACE("OBJEVENT_REGISTRY is partially  handled.\n");

		// PETER: We need this, because of Library names

		// Finding the object in the tree
		HTREEITEM hItem;
		if(m_TreeInheritance.m_MgaMap.LookupTreeItem(ccpMgaObject,hItem)) // The changed object is in the tree
		{
			// Change name
			m_TreeInheritance.SetItemText(hItem,GetDisplayedName(ccpMgaObject));

			// Setting the state icons
			//m_TreeInheritance.SetItemProperties(hItem);
		}
	}
}

void CInheritancePropertyPage::OnKeyDownTreeInheritance(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	switch(pTVKeyDown->wVKey)
	{
	case VK_RETURN: // Display the first selected item in GME editor
		{
				HTREEITEM hItem=m_TreeInheritance.GetSelectedItem();
				LPUNKNOWN pUnknown=NULL;

				if(m_TreeInheritance.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
				{
					TRACE(m_TreeInheritance.GetItemText(hItem));

					// Firing the event
					CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
					if( ::GetKeyState( VK_SHIFT) < 0)
						pApp->GetCtrl()->FireShowInParentMgaObject(pUnknown);
					else
						pApp->GetCtrl()->FireDblClickMgaObject(pUnknown);
					m_TreeInheritance.SetFocus();
				}
		}break;
	case VK_TAB:
		{
			if( ::GetKeyState( VK_CONTROL) < 0) {
				m_parent->nextTab( ::GetKeyState( VK_SHIFT) >= 0);
			}
			else {
				m_ComboEditCtrl.SetFocus();
			}
		}break;
	}

	
	*pResult = 0;
}


void CInheritancePropertyPage::OnItemExpandingTreeInheritance(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		
	if(pNMTreeView->action==TVE_EXPAND)
	{
		if(m_TreeInheritance.GetItemState(pNMTreeView->itemNew.hItem,TVIS_EXPANDED)
																	&TVIS_EXPANDED)
		{
			return;
		}

		int nImage,nSelectedImage;
		m_TreeInheritance.GetItemImage(pNMTreeView->itemNew.hItem,nImage,nSelectedImage);
		m_TreeInheritance.SetItemImage(pNMTreeView->itemNew.hItem,nImage+ICON_NUMBER,nSelectedImage+ICON_NUMBER);
	}
	else
	{
		if(!(m_TreeInheritance.GetItemState(pNMTreeView->itemNew.hItem,TVIS_EXPANDED)
																	&TVIS_EXPANDED))
		{
			return;
		}

		int nImage,nSelectedImage;
		m_TreeInheritance.GetItemImage(pNMTreeView->itemNew.hItem,nImage,nSelectedImage);
		m_TreeInheritance.SetItemImage(pNMTreeView->itemNew.hItem,nImage-ICON_NUMBER,nSelectedImage-ICON_NUMBER);

	}
	
	*pResult = 0;
}


CString CInheritancePropertyPage::GetDisplayedName(IMgaObject *pIMgaObject)
{

	CComPtr<IMgaObject>ccpMgaObject(pIMgaObject);
	CComBSTR pDisplayedName;
	
	COMTHROW(ccpMgaObject->get_Name(&pDisplayedName));

	CString ret = pDisplayedName;
	
	return ret;
}


void CInheritancePropertyPage::SetupTree()
{

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
	
	// No open poject - error
	if(pApp->m_CurrentProject.GetCurrentProjectState()!=CURRENT_PROJECT_OPEN) return;

	CActiveBrowserPropertySheet* pParent=(CActiveBrowserPropertySheet*)GetParent();
	
	// Getting selection from aggregate tree
	CAggregateMgaObjectProxy MgaObjectProxy;
	

	// condition modified> inheritance tree empty for all folders
	// previously was empty for rootfolder only
	if(!pParent->m_PageAggregate.m_TreeAggregate.GetSelectedItemProxy(MgaObjectProxy)
	|| MgaObjectProxy.m_TypeInfo == OBJTYPE_FOLDER) // all folders, by zolmol
	{
		// No selection in the aggregate tree - or root wa selected
		m_TreeInheritance.CleanUp();
		m_ComboSearchCtrl.ResetContent();
		//m_TreeInheritance.EnableWindow(FALSE);
		//m_ComboSearchCtrl.EnableWindow(FALSE);
		// previously disabled, but let's not do that anymore, 
		// thus we allow seamless CTRL+TABbing through the PropertyPages
		// without the focus wandering away from the tree ctrl
		// by zolmol
		return;
	}
	m_TreeInheritance.EnableWindow(TRUE);
	m_ComboSearchCtrl.EnableWindow(TRUE);

	
	// We maintain the previous selection in  
	// m_pPrevSelectedItem for efficiency reasons
	// If the selection is the same as previous, return
	if(m_pPrevSelectedItem==MgaObjectProxy.m_pMgaObject)
	{
		//return; // commented by ZolMol, recreate the tree always
	}
	else
	{
		m_pPrevSelectedItem=MgaObjectProxy.m_pMgaObject;
	}

	// Casting to FCO
	CComQIPtr<IMgaFCO>ccpMgaFCO(MgaObjectProxy.m_pMgaObject);
	if(ccpMgaFCO==NULL)
	{
		// No selection in the aggreagate tree
		m_TreeInheritance.CleanUp();
		m_ComboSearchCtrl.ResetContent();
		return;
	}

	// Cleaning up
	m_TreeInheritance.CleanUp();
	m_ComboSearchCtrl.ResetContent();
	
	MSGTRY{
		// Starting transaction
		pMgaContext->BeginTransaction();

		CComPtr<IMgaFCO> ccpMgaRootFCO(ccpMgaFCO);
		// Getting the root
		reqFindInheritanceRoot(ccpMgaRootFCO);
		
		// Storing it in a member
		m_theCurrentRootFCO = ccpMgaRootFCO;

		// Building inheritance tree
		reqBuildInheritanceTree(NULL,ccpMgaRootFCO,-1);
	
		// Ending transaction
		pMgaContext->CommitTransaction();

	}MSGCATCH(_T("Error retrieving data for inheritance tab"),pMgaContext->AbortTransaction();)	


	// Select the current item
	HTREEITEM hSelectedItem;
	m_TreeInheritance.m_MgaMap.LookupTreeItem(ccpMgaFCO,hSelectedItem);
	m_TreeInheritance.SelectItem(hSelectedItem);
	m_TreeInheritance.EnsureVisible(hSelectedItem);

}

void CInheritancePropertyPage::ResetRoot()
{
	m_theCurrentRootFCO = 0;
}

void CInheritancePropertyPage::SetupTree2()
{
	ASSERT( m_theCurrentRootFCO != 0);
	if( !m_theCurrentRootFCO) return; // if root not set then return

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
	
	// No open poject - error
	if(pApp->m_CurrentProject.GetCurrentProjectState()!=CURRENT_PROJECT_OPEN) return;

	CActiveBrowserPropertySheet* pParent=(CActiveBrowserPropertySheet*)GetParent();
	
	m_TreeInheritance.EnableWindow(TRUE);
	m_ComboSearchCtrl.EnableWindow(TRUE);

	// Cleaning up
	m_TreeInheritance.CleanUp();
	m_ComboSearchCtrl.ResetContent();
	
	MSGTRY{
		// Starting transaction
		pMgaContext->BeginTransaction();

		long status;
		COMTHROW(m_theCurrentRootFCO->get_Status(&status));
		if (status != OBJECT_EXISTS) { // if zombie then return
			return;
		}

		// Getting the root
		CComPtr<IMgaFCO> ccpMgaRootFCO;
		ccpMgaRootFCO = m_theCurrentRootFCO;

		// Building inheritance tree
		reqBuildInheritanceTree(NULL,ccpMgaRootFCO,-1);
	
		// Ending transaction
		pMgaContext->CommitTransaction();

	}MSGCATCH(_T("Error retrieving data for inheritance tab"),pMgaContext->AbortTransaction();)	
}

void CInheritancePropertyPage::Refresh()
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
	
	MSGTRY{
		// Starting transaction
		pMgaContext->BeginTransaction();

		m_ComboSearchCtrl.ResetContent();

		m_TreeInheritance.StoreState();
		m_TreeInheritance.CleanUp();
		if( m_theCurrentRootFCO)
			SetupTree2();
		else
			SetupTree(); 
		m_TreeInheritance.RestoreState();

	}MSGCATCH(_T("Error refreshing tree browser data"),pMgaContext->AbortTransaction();)	
	// Ending transaction
	pMgaContext->CommitTransaction();
}

void CInheritancePropertyPage::reqBuildInheritanceTree(HTREEITEM hParent, IMgaFCO* pIMgaFCO, int nDepthLevel)
{

	CComQIPtr<IMgaFCO> ccpMgaFCO(pIMgaFCO);
	CComPtr<IMgaFCOs> ccpChildrenFCOs;

	// Insert FCO into the tree
	enum objtype_enum eObjectType;
	COMTHROW(ccpMgaFCO->get_ObjType(&eObjectType)); // Getting object type

	HTREEITEM hItem=m_TreeInheritance.InsertItem(hParent,GetDisplayedName(ccpMgaFCO),
														ccpMgaFCO,eObjectType);


	// Getting children
	COMTHROW(ccpMgaFCO->get_DerivedObjects(&ccpChildrenFCOs));


	if(ccpChildrenFCOs)
	{
		// Iterate through the children FCOs
		MGACOLL_ITERATE(IMgaFCO, ccpChildrenFCOs)
		{
			// Recursive call for each children
			reqBuildInheritanceTree(hItem,MGACOLL_ITER,nDepthLevel-1);

		} MGACOLL_ITERATE_END;
	}
	
}

void CInheritancePropertyPage::reqFindInheritanceRoot(CComPtr<IMgaFCO>&ccpMgaFCO)
{

	CComPtr<IMgaFCO> ccpParentFCO;

	// Getting parent
	COMTHROW(ccpMgaFCO->get_DerivedFrom(&ccpParentFCO));

	if(ccpParentFCO==NULL) // No parent, we got the root object
	{		
		return;
	}
	else
	{
		ccpMgaFCO = ccpParentFCO;
		reqFindInheritanceRoot(ccpMgaFCO);
	}

}

BOOL CInheritancePropertyPage::OnSetActive() 
{
	ResetRoot();
	SetupTree();
	return CPropertyPage::OnSetActive();
}

void CInheritancePropertyPage::OnSelChangedTreeInheritance(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// Setting the focus here, because combo could make the selection
	m_TreeInheritance.SetFocus();

	// Displaying the first selection in the combo edit
	HTREEITEM hItem=m_TreeInheritance.GetFirstSelectedItem();
	if(hItem)
	{
		m_ComboSearchCtrl.SetSelection(hItem);

		LPUNKNOWN pUnknown=NULL;

		if(m_TreeInheritance.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
		{
			TRACE(m_TreeInheritance.GetItemText(hItem));

			// Firing the event
			CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
			CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

			// MOD by Peter: do not fire events from transactions
			if (!pMgaContext->IsInTransaction()) {
				pApp->GetCtrl()->FireClickMgaObject(pUnknown);
			}
		}

	}
	
	*pResult = 0;
}


void CInheritancePropertyPage::OnDblclkTreeInheritance(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hItem=m_TreeInheritance.GetSelectedItem();
	
	LPUNKNOWN pUnknown=NULL;

	if(m_TreeInheritance.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
	{
		TRACE(m_TreeInheritance.GetItemText(hItem));

		// Firing the event
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		if( ::GetKeyState( VK_SHIFT) < 0)
			pApp->GetCtrl()->FireShowInParentMgaObject(pUnknown);
		else
			pApp->GetCtrl()->FireDblClickMgaObject(pUnknown);
	}

	*pResult = -1; // Nonzero return: disables auto expanding (default action)	
}











/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///	CMetaPropertyPage property page
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










CMetaPropertyPage::CMetaPropertyPage() : CPropertyPage(CMetaPropertyPage::IDD),
			m_ComboEditCtrl(&m_ComboSearchCtrl), m_ComboSearchCtrl(&m_TreeMeta)
{
	//{{AFX_DATA_INIT(CMetaPropertyPage)
	//}}AFX_DATA_INIT
}

CMetaPropertyPage::~CMetaPropertyPage()
{
	m_ImageList.DeleteImageList();
	VERIFY( m_ImageList.GetSafeHandle() == NULL);
}

void CMetaPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMetaPropertyPage)
	DDX_Control(pDX, IDC_COMBO_SERACH_META, m_ComboSearchCtrl);
	DDX_Control(pDX, IDC_TREE_META, m_TreeMeta);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMetaPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMetaPropertyPage)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_META, OnDblclkTreeMeta)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_META, OnSelChangedTreeMeta)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_META, OnItemExpandingTreeMeta)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_META, OnKeyDownTreeMeta)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CMetaPropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_TreeMeta.ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT
									|TVS_SHOWSELALWAYS );

	m_ImageList.Create(16,16,ILC_MASK|ILC_COLOR24,0,0);
	
	CBitmap bm;
	bm.LoadBitmap(IDB_IMAGELIST_META);
	


	m_ImageList.Add(&bm,RGB(128, 158, 8));

	

	m_TreeMeta.SetImageList(&m_ImageList,TVSIL_NORMAL);

	
	m_TreeMeta.EnableWindow(FALSE);
	m_ComboSearchCtrl.EnableWindow(FALSE);


	// Get edit control which happens to be the first child window
	// and subclass it
	m_ComboEditCtrl.SubclassWindow(m_ComboSearchCtrl.GetWindow(GW_CHILD)->GetSafeHwnd());


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMetaPropertyPage::OnSetActive() 
{
	CActiveBrowserPropertySheet* pParent=(CActiveBrowserPropertySheet*)GetParent();
	pParent->m_PageInheritance.ResetRoot();
	return CPropertyPage::OnSetActive();
}

void CMetaPropertyPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	if(	::IsWindow(m_ComboSearchCtrl.GetSafeHwnd()) )
	{
		int ySpace = GetSystemMetrics(SM_CYBORDER);
		RECT clientRect;
		m_ComboSearchCtrl.GetClientRect(&clientRect);
		
		m_TreeMeta.MoveWindow(0, clientRect.bottom + 2*ySpace, cx-1, cy-clientRect.bottom - 2*ySpace - 1);
		m_ComboSearchCtrl.MoveWindow(0, ySpace, cx-1, cy-1);
	}
	
}

void CMetaPropertyPage::SetupTree()
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	// Getting Metaproject
	CComPtr<IMgaMetaProject> ccpMetaProject;
	COMTHROW(pMgaContext->m_ccpProject->get_RootMeta(&ccpMetaProject));

	// Setting Project name
	CComBSTR bszProjectName;
	ccpMetaProject->get_DisplayedName(&bszProjectName);
	m_strProjectName=bszProjectName;

	// Getting RootFolder
	CComPtr<IMgaMetaFolder> ccpRootMetaFolder;
	COMTHROW(ccpMetaProject->get_RootFolder(&ccpRootMetaFolder));

	
	InsertIntoMetaTree(NULL,ccpRootMetaFolder,_T(""));
	
	CMetaObjectList ObjListParents;
	CMetaObjectList ObjListChildren;

	CComPtr<IMgaMetaBase> ccpMetaBase(ccpRootMetaFolder);
	ObjListParents.AddTail(ADAPT_META_OBJECT(ccpMetaBase));


	// Performing breadth first search
	while(!ObjListParents.IsEmpty())
	{
		ObjListChildren.RemoveAll();
		
		POSITION pos=ObjListParents.GetHeadPosition();
		
		while(pos)
		{
			CComPtr<IMgaMetaBase> ccpMetaObject=ObjListParents.GetNext(pos);
			InsertChildren(ccpMetaObject,ObjListChildren);
		}

		ObjListParents.RemoveAll();
		ObjListParents.AddTail(&ObjListChildren);
	}
	
}

int CMetaPropertyPage::InsertChildren(CComPtr<IMgaMetaBase>& ccpMetaObject,CMetaObjectList& MetaObjectList)
{
	int c=0;
	// Determining Object Type
	objtype_enum otObjectType;
	COMTHROW(ccpMetaObject->get_ObjType(&otObjectType));
	
	HTREEITEM hParent;
	if(!m_TreeMeta.m_MgaMap.LookupTreeItem(ccpMetaObject,hParent))
	{
		ASSERT(0);
		return 0;
	}


	switch(otObjectType)
	{
		case OBJTYPE_MODEL:
			{
				CComQIPtr<IMgaMetaModel>ccpMgaMetaModel(ccpMetaObject);											
	
				// Getting children roles
				CComPtr<IMgaMetaRoles> ccpMgaMetaRoles;
				COMTHROW(ccpMgaMetaModel->get_Roles(&ccpMgaMetaRoles));

				// Iterate through the children roles
				MGACOLL_ITERATE(IMgaMetaRole, ccpMgaMetaRoles) 
				{
					CComPtr<IMgaMetaFCO> ccpRoleKind=NULL;
					COMTHROW(MGACOLL_ITER->get_Kind(&ccpRoleKind));
					if(ccpRoleKind.p!=NULL)
					{	
						// If it is already in the tree we do not add it
						if(InsertIntoMetaTree(hParent,ccpRoleKind,GetDisplayedName(MGACOLL_ITER)))
						{
							CComPtr<IMgaMetaBase> ccpMetaBase(ccpRoleKind);

							MetaObjectList.AddTail(ADAPT_META_OBJECT(ccpMetaBase));
							c++;
						}
					}
				} MGACOLL_ITERATE_END;

			}break;
		
		case OBJTYPE_FOLDER: 
			{				
				CComQIPtr<IMgaMetaFolder>ccpMgaMetaFolder(ccpMetaObject);	

				/*********  Getting Subfolders ***********/
				CComPtr<IMgaMetaFolders> ccpLegalFolders;
				COMTHROW(ccpMgaMetaFolder->get_LegalChildFolders(&ccpLegalFolders));
				
				// Iterate through the subfolders
				MGACOLL_ITERATE(IMgaMetaFolder, ccpLegalFolders) 
				{
					// If it is already in the tree we do not add it
					if(InsertIntoMetaTree(hParent,MGACOLL_ITER,GetDisplayedName(MGACOLL_ITER)))
					{
						CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);

						MetaObjectList.AddTail(ADAPT_META_OBJECT(ccpMetaBase));
						c++;
					}
				} MGACOLL_ITERATE_END;

				
				/********* Getting children FCOs **********/
				CComPtr<IMgaMetaFCOs> ccpLegalRootObjects;
				COMTHROW(ccpMgaMetaFolder->get_LegalRootObjects(&ccpLegalRootObjects));
				// Iterate through the children FCOs
				MGACOLL_ITERATE(IMgaMetaFCO, ccpLegalRootObjects) 
				{
					// If it is already in the tree we do not add it
					if(InsertIntoMetaTree(hParent,MGACOLL_ITER,GetDisplayedName(MGACOLL_ITER)))
					{
						CComPtr<IMgaMetaBase> ccpMetaBase(MGACOLL_ITER);
						MetaObjectList.AddTail(ADAPT_META_OBJECT(ccpMetaBase));
						c++;
					}
				
				} MGACOLL_ITERATE_END;

			}break;
	}
	return c;
}




void CMetaPropertyPage::ProcessConnection(CString &strConnectionToolTip, IMgaMetaConnection *pIMetaConnection)
{
	CComPtr<IMgaMetaConnection> ccpMetaConnection(pIMetaConnection);
	
	strConnectionToolTip=_T("Name: ")+GetDisplayedName(ccpMetaConnection)+_T("\r\n");
	
	// Is the connection simple? Not simple connections are not implemented here!!!
	VARIANT_BOOL bIsSimple=VARIANT_FALSE;
	COMTHROW(ccpMetaConnection->get_IsSimple(&bIsSimple));
	
	// Simple connection case
	if(bIsSimple!=FALSE)
	{
		// Getting connection joins
		CComPtr<IMgaMetaConnJoints> ccpMgaMetaConnJoints;
		CComPtr<IMgaMetaConnJoint> ccpMgaMetaConnJoint;
		COMTHROW(ccpMetaConnection->get_Joints(&ccpMgaMetaConnJoints));		

		// Iterate through the joints
		int i=0; // Numbering joins
		MGACOLL_ITERATE(IMgaMetaConnJoint, ccpMgaMetaConnJoints) 
		{
			i++;
			// Formatting tooltip
			CString strJoint;
			strJoint.Format(_T("Joint %d: \r\n"),i);
			strConnectionToolTip+=strJoint;

			// Iterator renamed for sake of readability
			ccpMgaMetaConnJoint = MGACOLL_ITER;
	
			// Getting pointer specifications (source and destination)
			CComBSTR bszSourceName(_T("src"));
			CComBSTR bszDestName(_T("dst"));

			CComPtr<IMgaMetaPointerSpec> ccpMgaMetaPointerSpecSrc;
			CComPtr<IMgaMetaPointerSpec> ccpMgaMetaPointerSpecDest;
			

			if(E_NOTFOUND!= ccpMgaMetaConnJoint->get_PointerSpecByName(bszSourceName, 
														&ccpMgaMetaPointerSpecSrc) )
			{
				// Source side *************************************************
				CString strSrcToolTip(_T("[Source:]\r\n"));

				CComPtr<IMgaMetaPointerItems> ccpMgaMetaPointerItemsSrc;
				ccpMgaMetaPointerSpecSrc->get_Items(&ccpMgaMetaPointerItemsSrc);
				
				// Iterating through the pointer items
				MGACOLL_ITERATE(IMgaMetaPointerItem,ccpMgaMetaPointerItemsSrc) 
				{
					// Getting item description
					CComBSTR bszDesc;
					COMTHROW(MGACOLL_ITER->get_Desc(&bszDesc));

					// Formatting description
					strSrcToolTip+=_T("\t")+CString(bszDesc)+_T("\r\n");			

				}MGACOLL_ITERATE_END;  // Source Pointer Items
				strConnectionToolTip+=strSrcToolTip;	
			}
			else
			{
				CString strSrcToolTip(_T("[Source:] <not found>\r\n"));
				strConnectionToolTip+=strSrcToolTip;	
			}

			if(E_NOTFOUND!= ccpMgaMetaConnJoint->get_PointerSpecByName(bszDestName, 
														&ccpMgaMetaPointerSpecDest))
			{
				// Destination side ********************************************
				CString strDestToolTip(_T("[Destination:] \r\n"));

				CComPtr<IMgaMetaPointerItems> ccpMgaMetaPointerItemsDest;
				ccpMgaMetaPointerSpecDest->get_Items(&ccpMgaMetaPointerItemsDest);
				
				// Iterating through the pointer items
				MGACOLL_ITERATE(IMgaMetaPointerItem,ccpMgaMetaPointerItemsDest) 
				{
					// Getting item description
					CComBSTR bszDesc;
					COMTHROW(MGACOLL_ITER->get_Desc(&bszDesc));

					// Formatting description
					strDestToolTip+=_T("\t")+CString(bszDesc)+_T("\r\n");					
				}MGACOLL_ITERATE_END; // Destination pointer items
				strConnectionToolTip+=strDestToolTip;	
			}
			else
			{
				CString strDestToolTip(_T("[Destination:] <not found>\r\n"));
				strConnectionToolTip+=strDestToolTip;	
			}
						
		}MGACOLL_ITERATE_END; // Joints

	}
	else
	{
		// Not simple connection case is not implemented!!!!		
		strConnectionToolTip=_T("Information available only for simple connections.");
	}

}


// Called recursively from reqBuildMetaTree function and calls it again.
void CMetaPropertyPage::ProcessAttributes(HTREEITEM hParent, IMgaMetaFCO *pIMgaMetaFCO)
{
	if(hParent==NULL) return;

	CComPtr<IMgaMetaFCO>ccpMgaMetaFCO(pIMgaMetaFCO);

	/*********  Getting Attributes ***********/
	CComPtr<IMgaMetaAttributes> ccpAttributes;
	COMTHROW(ccpMgaMetaFCO->get_Attributes(&ccpAttributes));
	
	// Iterate through the attributes
	MGACOLL_ITERATE(IMgaMetaAttribute, ccpAttributes) 
	{
		CComPtr<IMgaMetaAttribute>ccpMgaMetaAttribute(MGACOLL_ITER);
		
		// Inserting item into the meta tree
		HTREEITEM hItem=m_TreeMeta.InsertItem(hParent,GetDisplayedName(ccpMgaMetaAttribute),ccpMgaMetaAttribute,OBJTYPE_ATTRIBUTE);
		
	} MGACOLL_ITERATE_END;	


}


void CMetaPropertyPage::OpenProject()
{
	m_TreeMeta.EnableWindow(TRUE);
	m_ComboSearchCtrl.EnableWindow(TRUE);

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	MSGTRY{
		SetupTree();
	}MSGCATCH(_T("Error opening meta tab"),pMgaContext->AbortTransaction();)	

}


void CMetaPropertyPage::CloseProject()
{
	m_TreeMeta.CleanUp();	
	m_TreeMeta.EnableWindow(FALSE);

	m_ComboSearchCtrl.ResetContent();
	m_ComboSearchCtrl.EnableWindow(FALSE);

}



void CMetaPropertyPage::OnDblclkTreeMeta(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DWORD dwMsgPos = ::GetMessagePos();

	CPoint point = CPoint(GET_X_LPARAM(dwMsgPos), GET_Y_LPARAM(dwMsgPos) );
	UINT nHitFlags;
	
	m_TreeMeta.ScreenToClient(&point);
	m_TreeMeta.HitTest( point, &nHitFlags );

	
	if(nHitFlags&TVHT_ONITEMBUTTON)
	{
		return;
	}

	HTREEITEM hItem=m_TreeMeta.GetSelectedItem();
	
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;


	if(hItem)
	{
		HTREEITEM hReferencedItem;
		if( hReferencedItem=(HTREEITEM)m_TreeMeta.GetItemData(hItem) )
		{
			m_TreeMeta.SelectItem(hReferencedItem);
			m_TreeMeta.EnsureVisible(hReferencedItem);

		}
		
		MSGTRY{
			if(GetKeyState(VK_MENU)&0x8000) // Alt + dblclick - connection properties
			{

				HTREEITEM hItem=m_TreeMeta.GetSelectedItem();
				CMgaObjectProxy ObjectProxy;
				if(!m_TreeMeta.m_MgaMap.LookupObjectProxy(hItem,ObjectProxy)) return; // Not in the map
				if(ObjectProxy.m_TypeInfo==OBJTYPE_CONNECTION)
				{
					CComQIPtr<IMgaMetaConnection> ccpMetaConnection(ObjectProxy.m_pMgaObject);
					if(ccpMetaConnection)
					{
						CString strConnDesc;
						ProcessConnection(strConnDesc,ccpMetaConnection);

						CMetaConnectionDlg dlg(this);
						dlg.m_strConnectionProperties=strConnDesc;
						dlg.DoModal();
					}
				}
			}
		}MSGCATCH(_T("Error retrieving connection properties"),pMgaContext->AbortTransaction();)	

	}
	*pResult = -1;
}




void CMetaPropertyPage::OnSelChangedTreeMeta(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// Setting the focus here, because combo could make the selection
	m_TreeMeta.SetFocus();

	// Displaying the first selection in the combo edit
	HTREEITEM hItem=m_TreeMeta.GetFirstSelectedItem();
	if(hItem)
	{
		m_ComboSearchCtrl.SetSelection(hItem);
	}
	
	*pResult = 0;
}







void CMetaPropertyPage::OnItemExpandingTreeMeta(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	
	if(pNMTreeView->action==TVE_EXPAND)
	{
		if(m_TreeMeta.GetItemState(pNMTreeView->itemNew.hItem,TVIS_EXPANDED)
																	&TVIS_EXPANDED)
		{
			return;
		}

	
		int nImage,nSelectedImage;
		m_TreeMeta.GetItemImage(pNMTreeView->itemNew.hItem,nImage,nSelectedImage);
		m_TreeMeta.SetItemImage(pNMTreeView->itemNew.hItem,nImage+ICON_NUMBER,nSelectedImage+ICON_NUMBER);
	}
	else
	{
		if(!(m_TreeMeta.GetItemState(pNMTreeView->itemNew.hItem,TVIS_EXPANDED)
																	&TVIS_EXPANDED))
		{
			return;
		}

		int nImage,nSelectedImage;
		m_TreeMeta.GetItemImage(pNMTreeView->itemNew.hItem,nImage,nSelectedImage);
		m_TreeMeta.SetItemImage(pNMTreeView->itemNew.hItem,nImage-ICON_NUMBER,nSelectedImage-ICON_NUMBER);

	}

	*pResult = 0;
}


void CMetaPropertyPage::OnKeyDownTreeMeta(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	switch(pTVKeyDown->wVKey)
	{
	case VK_TAB:
		{
			if( ::GetKeyState( VK_CONTROL) < 0) {
				m_parent->nextTab( ::GetKeyState( VK_SHIFT) >= 0);
			}
			else {
				m_ComboEditCtrl.SetFocus();
			}
		}break;
	}

	
	*pResult = 0;
}



HTREEITEM CMetaPropertyPage::InsertIntoMetaTree(HTREEITEM hParent,IMgaMetaBase* IMetaObject, CString strRoleName)
{
	
	CComPtr<IMgaMetaBase> ccpMetaObject(IMetaObject);

	// Determining Object Type
	objtype_enum otObjectType;
	COMTHROW(ccpMetaObject->get_ObjType(&otObjectType));

	// FILTERING: these type won't be inserted
	if(otObjectType==OBJTYPE_ASPECT || otObjectType==OBJTYPE_PART)
	{
		return 0;
	}



	// Inserting item into the meta tree
	CString strName;
	if(strRoleName!=_T(""))
	{
		strName=strRoleName+_T(" (")+GetDisplayedName(ccpMetaObject)+_T(")");
	}
	else
	{
		strName=GetDisplayedName(ccpMetaObject);
	}

	HTREEITEM hItem= m_TreeMeta.InsertItem(hParent,strName,ccpMetaObject,otObjectType);
	
	// Inserting attributes 
	CComQIPtr<IMgaMetaFCO>ccpMetaFCO(ccpMetaObject);
	if(ccpMetaFCO)
	{
		ProcessAttributes(hItem,ccpMetaFCO);
	}

	return hItem;
}

CString CMetaPropertyPage::GetDisplayedName(IMgaMetaBase *pIMgaMetaBase)
{

	CComPtr<IMgaMetaBase>ccpMgaMetaBase(pIMgaMetaBase);
	CComBSTR pDisplayedName;
	COMTHROW(ccpMgaMetaBase->get_DisplayedName(&pDisplayedName));
	
	CString ret = pDisplayedName;
	
	return ret;
}

void CAggregatePropertyPage::OnKillFocus(CWnd* pNewWnd) 
{
	CPropertyPage::OnKillFocus(pNewWnd);
	HTREEITEM hItem=m_TreeAggregate.GetFirstSelectedItem();
	
	// TODO: Add your message handler code here
	
}

bool CAggregatePropertyPage::askUserAndDetach( CComPtr<IMgaObject> object)
{
	CComQIPtr<IMgaFCO> fco( object);
	if( !fco) return false;
	// check whether dependends of fco exist
	CComPtr<IMgaFCOs> der_objs;
	COMTHROW(fco->get_DerivedObjects( &der_objs));
	long cnt = 0;
	if( der_objs) COMTHROW( der_objs->get_Count( &cnt));
	if( cnt > 0) // if dependents exist should they be deleted?
	{
		bool question_asked = false;
		bool detach_answered = false;
		MGACOLL_ITERATE(IMgaFCO, der_objs) {
			CComPtr<IMgaFCO> one_derived(MGACOLL_ITER);

			VARIANT_BOOL prim_deriv;
			COMTHROW( one_derived->get_IsPrimaryDerived( &prim_deriv));
			if( prim_deriv == VARIANT_TRUE)
			{
				if( !question_asked) // pop up dialog only for the first time / per basetype
				{
					CComBSTR nm;
					COMTHROW( fco->get_Name( &nm));
					CString msg = _T("There are objects primary derived from: \"");
					msg += nm;
					msg += _T("\". Would you like to delete them as well?\n");
					msg += _T("If you answer 'No' the derived objects will be detached, thus preserved.");

					// this answer will be applied to all deriveds of this fco
					int resp = AfxMessageBox( msg, MB_YESNOCANCEL);
					if( resp == IDCANCEL) COMTHROW(E_MGA_MUST_ABORT);
					else if( resp == IDNO) detach_answered = true;
					
					question_asked = true;
				}

				// if detach and preserve selected by the user:
				if( detach_answered)
					COMTHROW( one_derived->DetachFromArcheType());
			}
		}MGACOLL_ITERATE_END;

		return detach_answered;
	}
	return false;
}

void CAggregatePropertyPage::accessRights( bool pbProtectIt)
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	HTREEITEM hItem = m_TreeAggregate.GetFirstSelectedItem();
	if (hItem != NULL) 
	{ 
		
		LPUNKNOWN pUnknown;
		
		if(!m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))return;
					
		CComQIPtr<IMgaObject> ccpObj(pUnknown);
		if( !ccpObj)
			return;

		MSGTRY {
			
			CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
			CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

			pMgaContext->BeginTransaction(false);

			COMTHROW(ccpObj->PutReadOnlyAccessWithPropagate( pbProtectIt ? VARIANT_TRUE:VARIANT_FALSE));
			pMgaContext->CommitTransaction();

		} MSGCATCH(_T("Error while applying new access rights"),pMgaContext->AbortTransaction();)	
		Refresh();
	}
}

void CAggregatePropertyPage::LibraryAmbiguityChk()
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	HTREEITEM hItem = m_TreeAggregate.GetFirstSelectedItem();
	if (hItem != NULL) 
	{ 
		
		LPUNKNOWN pUnknown;
		
		if(!m_TreeAggregate.m_MgaMap.LookupObjectUnknown(hItem,pUnknown))return;
					
		CComQIPtr<IMgaFolder> ccpFolder(pUnknown);
		if(!ccpFolder)return;

		MSGTRY {
			
			CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
			CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

			CComBSTR msg;
			pMgaContext->BeginTransaction(false);

			CComPtr<IMgaFolders> fols;
			std::map< CComBSTR, std::vector< CComBSTR > > guidmap;
			
			// the rootfolder's guid might have a guid conflict also
			CComBSTR gd_of_project;
			COMTHROW( ccpFolder->GetGuidDisp( &gd_of_project));
			// save its guid
			guidmap[ gd_of_project].push_back( CComBSTR( _T("Main Project")));

			// check toplevel folders
			COMTHROW( ccpFolder->get_ChildFolders( &fols));
			long len = 0;
			if( fols) COMTHROW( fols->get_Count( &len));
			for( long i = 1; i <= len; ++i)
			{
				CComPtr<IMgaFolder> fol;
				COMTHROW( fols->get_Item( i, &fol));

				if( !fol) { ASSERT(0); continue; }

				CComBSTR bszLibName;
				COMTHROW( fol->get_LibraryName(&bszLibName));

				if( bszLibName) // if library
				{
					CComBSTR gd;
					COMTHROW( fol->GetGuidDisp( &gd));

					// save its guid
					guidmap[ gd].push_back( bszLibName);
				}
			}
		
			for( std::map< CComBSTR, std::vector< CComBSTR > >::iterator it = guidmap.begin(), en = guidmap.end(); it != en; ++it)
			{
				if( it->second.size() > 1) // duplicates found
				{
					if (msg.Length() > 0)
						COMTHROW(msg.Append( _T("\n\n")));
					COMTHROW(msg.AppendBSTR( it->first));
					COMTHROW(msg.Append( _T("\nShared by multiple toplevel libraries:")));
					for( unsigned int i = 0; i < it->second.size(); ++i)
					{
						COMTHROW(msg.Append( _T("\n\t")));
						COMTHROW(msg.AppendBSTR( it->second[i]));
					}
				}
			}
			if( msg.Length() != 0)
			{
				CString msgs;
				CopyTo( msg, msgs);
				AfxMessageBox( msgs, MB_OK | MB_ICONSTOP);
			}
			else
				AfxMessageBox( _T("No duplicate GUID found among the toplevel libraries."), MB_ICONINFORMATION);

			//pMgaContext->CommitTransaction();
			pMgaContext->AbortTransaction();

		} MSGCATCH(_T("Error while analyzing library guid ambiguities"),pMgaContext->AbortTransaction();)	
		//Refresh();
	}
}

void CAggregatePropertyPage::ProjectSourceControlUpdate( CComBSTR id)
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;


	MSGTRY 
	{
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

		// no transaction needed
		COMTHROW( pMgaContext->m_ccpProject->UpdateSourceControlInfo( id));
	}
	MSGCATCH(_T("Error while updating source control info!"),;)

	Refresh();
}

void CAggregatePropertyPage::SourceControlObjectOwner( CComBSTR p_id)
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	MSGTRY 
	{
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

		// no transaction needed
		COMTHROW( pMgaContext->m_ccpProject->SourceControlObjectOwner( p_id));
	}
	MSGCATCH(_T("Error while fetching source control info!"),;)

	Refresh();
}

void CAggregatePropertyPage::SourceControlActiveUsers()
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	MSGTRY 
	{
		CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
		CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

		// no transaction needed
		COMTHROW( pMgaContext->m_ccpProject->SourceControlActiveUsers());
	}
	MSGCATCH(_T("Error while fetching source control info!"),;)

	Refresh();
}

//static
void CAggregatePropertyPage::composeInfo( CString p_msgText, CComPtr<IMgaFolders>& coll, CString& p_msg)
{
	CString&       msg = p_msg;
	CString        lst;

	long len = 0;
	if( coll) COMTHROW( coll->get_Count( &len));
	for( long i = 1; i <= len; ++i)
	{
		CComPtr<IMgaFolder> ele;
		COMTHROW( coll->get_Item( i, &ele));

		ASSERT( ele);
		if( ele)
		{
			long stat = OBJECT_ZOMBIE;
			COMTHROW( ele->get_Status( &stat));
			if( stat == OBJECT_EXISTS)
			{
				CComBSTR connstr;
				CComBSTR guiddsp;
				COMTHROW( ele->get_LibraryName( &connstr));
				COMTHROW( ele->GetGuidDisp( &guiddsp));
				if( connstr && connstr.Length() > 0) 
				{
					CString cs, gs;
					CopyTo( connstr, cs);
					CopyTo( guiddsp, gs);
					lst += cs + _T(" ") + gs + _T("\n");
				}
			}
		}
	}

	msg = p_msgText + (lst.IsEmpty()?_T("None"):lst);
}

void CAggregatePropertyPage::HighlightItem(IMgaObject* pObj, int highlight)
{
	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;
	CComPtr<IMgaObject> terrObj;
	pMgaContext->m_ccpTerritory->__OpenObj(pObj, &terrObj);
	CComPtr<IUnknown> pUnk;
	terrObj.QueryInterface(&pUnk.p);

	CComBSTR id = static_cast<BSTR>(pObj->ID);

	auto& highlightedObjects = m_TreeAggregate.m_highlightedObjects;
	if (highlight)
	{
		highlightedObjects[id] = highlight;
	}
	else
	{
		auto it = highlightedObjects.find(id);
		if (it != highlightedObjects.end())
		{
			highlightedObjects.erase(it);
		}
	}
	HTREEITEM hItem;
	IUnknown* punk;
	if (m_TreeAggregate.m_MgaMap.SearchTreeItem(id, hItem, punk))
	{
		m_TreeAggregate.SetItemProperties(hItem);
	}
}
