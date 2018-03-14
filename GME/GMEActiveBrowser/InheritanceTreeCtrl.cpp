// InheritanceTreeCtrl.cpp: implementation of the CInheritanceTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "InheritanceTreeCtrl.h"
#include "ActiveBrowserPropertyPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInheritanceTreeCtrl::CInheritanceTreeCtrl()
{

}

CInheritanceTreeCtrl::~CInheritanceTreeCtrl()
{

}

HTREEITEM CInheritanceTreeCtrl::InsertItem(HTREEITEM hParent, CString strObjectName, LPUNKNOWN pUnknown, objtype_enum otObjectType)
{
	MGATREECTRL_LOGEVENT("CInheritanceTreeCtrl::InsertItem "+strObjectName+"\r\n");
	
	if(m_MgaMap.bIsInMap(pUnknown)) // Should not happen
	{
		TRACE(" Duplicate element found inserting a new element into the aggregation tree map.\n");
		ASSERT(FALSE);
		

		return NULL;
	}
	else
	{
		HTREEITEM hItem=CTreeCtrlEx::InsertItem(strObjectName,(int)otObjectType,(int)otObjectType,hParent);	
		CMgaObjectProxy ObjectProxy(pUnknown,otObjectType);
		m_MgaMap.AddEntry(hItem,ObjectProxy);
		SetItemData(hItem,(DWORD)0);

		SetItemProperties(hItem);

		return hItem;
	}

}

void CInheritanceTreeCtrl::SetItemProperties(HTREEITEM hItem)
{
	CMgaObjectProxy ObjectProxy;

	if(!m_MgaMap.LookupObjectProxy(hItem, ObjectProxy))return;

	CComQIPtr<IMgaObject> ccpObject(ObjectProxy.m_pMgaObject);

	if(!ccpObject)return; // Not an MgaObject
	
	BYTE cState=0;

	CGMEActiveBrowserApp* pApp=(CGMEActiveBrowserApp*)AfxGetApp();
	CMgaContext* pMgaContext=&pApp->m_CurrentProject.m_MgaContext;

	pMgaContext->BeginTransaction();


	// Checking access rights
	VARIANT_BOOL vtbIsWritable=VARIANT_FALSE;
	COMTHROW( ccpObject->get_IsWritable(&vtbIsWritable));

	if (vtbIsWritable != VARIANT_TRUE)
	{
		cState|=0x0001;// Read only object
	} // If the object is writable then nothing


	VARIANT_BOOL vtbIsLibObject=VARIANT_FALSE;;
	COMTHROW(ccpObject->get_IsLibObject(&vtbIsLibObject));
	
	if(vtbIsLibObject!=VARIANT_TRUE)
	{ } // Not a LibObject
	else
	{		
		cState|=0x0002;// LibObject
	}

	pMgaContext->CommitTransaction();

	/////////////// If it is an FCO cast it //////////////
	// If not an FCO
	if(ObjectProxy.m_TypeInfo==OBJTYPE_FOLDER)return;

	CComQIPtr<IMgaFCO> ccpMgaFCO(ObjectProxy.m_pMgaObject);
	if(!ccpMgaFCO)return; // Not an FCO

	pMgaContext->BeginTransaction();
	// Is it instance?
	VARIANT_BOOL vtbIsInstance=VARIANT_FALSE;
	COMTHROW(ccpMgaFCO->get_IsInstance(&vtbIsInstance));

	if(vtbIsInstance!=VARIANT_TRUE)
	{
		CComPtr<IMgaFCO> ccpBaseType;
		COMTHROW(ccpMgaFCO->get_BaseType(&ccpBaseType));

		if(!ccpBaseType)
		{ } // Not derived
		else
		{
			cState|=0x0008;// Derived
		}
	}
	else
	{
		cState|=0x0004;// Instance
	}

	pMgaContext->CommitTransaction();	
	

	if(cState)
	{
		CTreeCtrl::SetItemState( hItem,INDEXTOSTATEIMAGEMASK(cState), TVIS_STATEIMAGEMASK );
	}
	else
	{		
		CTreeCtrl::SetItemState( hItem,0, TVIS_STATEIMAGEMASK );
	}
}

void CInheritanceTreeCtrl::StoreState()
{

	m_StateBuffer.RemoveAll();

	// Traverse all items in tree control
	HTREEITEM hItem= GetRootItem();

	while ( hItem )
	{
		/* Saving the items to the state buffer		*/
		// Getting item state
		UINT nItemState=CTreeCtrlEx::GetItemState(hItem,0x000000ff);
		CString strItemState;
		strItemState.Format(_T("%ul"),nItemState);

		// Searching the map for the Mga pointer
		LPUNKNOWN pUnknown;
		if(m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
		{
			CComQIPtr<IMgaObject>ccpMgaObject(pUnknown);
			if(ccpMgaObject) // succesful conversion
			{
				CComBSTR IDObj;
				COMTHROW(ccpMgaObject->get_ID(&IDObj));

				// Put Object id and item state in the buffer
				CString strID(IDObj);
				m_StateBuffer.SetAt(strID,strItemState);
				
			}
		}

		// Get first child node
		HTREEITEM hNextItem = GetChildItem( hItem );

		if ( !hNextItem )
		{
			// Get next sibling child
			hNextItem = GetNextSiblingItem( hItem );

			if ( !hNextItem )
			{
				HTREEITEM hParentItem=hItem;
				while ( !hNextItem && hParentItem )
				{
					// No more children: Get next sibling to parent
					hParentItem = GetParentItem( hParentItem );
					hNextItem = GetNextSiblingItem( hParentItem );
				}
			}
		}

		hItem = hNextItem;
	}

	m_bIsStateStored=TRUE;
	return;

}



void CInheritanceTreeCtrl::RestoreState()
{
	if(!m_bIsStateStored)return;

	// Traverse all items in tree control
	HTREEITEM hItem= GetRootItem();

	while ( hItem )
	{
		/* Getting the item from the map		*/
		
		// Searching the map for the Mga pointer
		LPUNKNOWN pUnknown;
		if(m_MgaMap.LookupObjectUnknown(hItem,pUnknown))
		{
			CComQIPtr<IMgaObject>ccpMgaObject(pUnknown);
			if(ccpMgaObject) // succesful conversion
			{
				CComBSTR IDObj;
				COMTHROW(ccpMgaObject->get_ID(&IDObj));

				// Get Object id and item state from the map
				CString strID(IDObj);
				CString strItemState;
				if(m_StateBuffer.Lookup(strID,strItemState))
				{
					TCHAR* pszEndPtr=NULL;
					UINT nItemState = _tcstoul(strItemState, &pszEndPtr, 10);
					CMgaMappedTreeCtrl::SetItemState(hItem,nItemState);
				}
			}
		}
		
		// Get first child node
		HTREEITEM hNextItem = GetChildItem( hItem );

		if ( !hNextItem )
		{
			// Get next sibling child
			hNextItem = GetNextSiblingItem( hItem );

			if ( !hNextItem )
			{
				HTREEITEM hParentItem=hItem;
				while ( !hNextItem && hParentItem )
				{
					// No more children: Get next sibling to parent
					hParentItem = GetParentItem( hParentItem );
					hNextItem = GetNextSiblingItem( hParentItem );
				}
			}
		}

		hItem = hNextItem;
	}

	m_StateBuffer.RemoveAll();
	m_bIsStateStored=FALSE;
	return;
}
