// MgaContext.cpp: implementation of the CMgaContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gmeactivebrowser.h"
#include "MgaContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMgaContext::CMgaContext()
{
	// transactions will be enabled only after opening a project
	m_nPendingTransactions=0; 
	m_bEventTransactionMode=FALSE;
}

CMgaContext::~CMgaContext()
{

}

bool CMgaContext::IsInTransaction()
{
	if ( m_bEventTransactionMode || (m_nPendingTransactions > 0) ) {
		return true;
	}
	else {
		return false;
	}
}

bool CMgaContext::BeginTransaction(bool bIsReadOnly)
{
	return BeginTransaction(bIsReadOnly ? TRANSACTION_READ_ONLY : TRANSACTION_GENERAL);
}

bool CMgaContext::BeginTransaction(transactiontype_enum type)
{
	// In the event handlers we are already in transaction
	if(m_bEventTransactionMode)
		return true;

	
	if(m_nPendingTransactions==0) // Not in transactions
	{		
		try
		{
			COMTHROW(m_ccpProject->BeginTransaction(m_ccpTerritory, type));
		}
		catch (hresult_exception)
		{
			ASSERT(false);
			return false;
		}		
	}
	m_nPendingTransactions++;		
	return true;

}

bool CMgaContext::CommitTransaction()
{
	// In the event handlers we are already in transaction
	if(m_bEventTransactionMode)return true;

	if(m_nPendingTransactions==0) // We are not in transaction. What to commit?
	{
		return false;
	}
	
	if(m_nPendingTransactions==1)
	{				
		COMTHROW(m_ccpProject->CommitTransaction());				
	}
	m_nPendingTransactions--;
	return true;

}

bool CMgaContext::AbortTransaction()
{
	if(m_bEventTransactionMode)
		return true;

	if(m_nPendingTransactions==0)
		return false;

	m_ccpProject->AbortTransaction();
	m_nPendingTransactions=0;
	m_bEventTransactionMode=false;

	return true;
}

void CMgaContext::SetTransactionState(int nValue)
{
	m_nPendingTransactions=nValue;
}

void CMgaContext::CloseContext()
{
	ASSERT(("Not commited transaction found. Check BeginTransaction - Commit Transaction pairs.",m_nPendingTransactions==0));
	// Disabling Transactions	
	SetTransactionState(0);
	
	// Deleting Territory
	if(m_ccpTerritory.p!=NULL)
	{
		COMTHROW(m_ccpTerritory->Destroy() );
	}
	m_ccpTerritory.Release();
	
	m_ccpProject.Release();

}

void CMgaContext::CreateContext(IMgaEventSink &rEventSink,LPUNKNOWN pMgaProject)
{
	// Creating Project
	CComQIPtr<IMgaProject> _project( pMgaProject);
	m_ccpProject = _project;
	//m_ccpProject.Attach((IMgaProject *) pMgaProject);
	// previously was used Attach but this does not call AddRef
	// on m_ccpProject, although in CloseContext Release is used
	// this will fix the JIRA-
	
	// Creating Territory
	COMTHROW( m_ccpProject->CreateTerritory(&rEventSink,&m_ccpTerritory,NULL) );

	SetTransactionState(0);
}

CComPtr<IMgaComponentEx> CMgaContext::FindConstraintManager()
{
	CComPtr<IMgaComponentEx> constrMgr;
	// Finding constraint manager among the addons
	CComPtr<IMgaComponents> comps;
	COMTHROW( m_ccpProject->get_AddOnComponents(&comps));
	MGACOLL_ITERATE(IMgaComponent, comps) 
	{
		CComBSTR name;
		COMTHROW(MGACOLL_ITER->get_ComponentName(&name));
		if(name == L"ConstraintManager") 
		{
			constrMgr = CComQIPtr<IMgaComponentEx>(MGACOLL_ITER); 
			break;
		}
	} MGACOLL_ITERATE_END;

	return constrMgr;
}

void CMgaContext::SetEventTransactionMode(bool bOnOff)
{
	m_bEventTransactionMode=bOnOff;
}
