// ConstraintMgr.cpp : Implementation of CConstraintMgr
#include "StdAfx.h"
#include "ConstraintManager.h"
#include "ConstraintMgr.h"
#include "GMESyntacticSemanticDialog.h"
#include "GMEConstraintBrowserDialog.h"
#include "OCLCommonEx.h"
#include "Gme.h"

//##############################################################################################################################################
//
//	C L A S S : CMgrEventSink
//
//##############################################################################################################################################

STDMETHODIMP CMgrEventSink::GlobalEvent( globalevent_enum event )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	return cm->GlobalEvent(event);
}

STDMETHODIMP CMgrEventSink::ObjectEvent(IMgaObject *obj, unsigned long eventmask, VARIANT v)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	return cm->ObjectEvent( obj, eventmask, v );
}

//##############################################################################################################################################
//
//	C L A S S : CConstraintMgr
//
//##############################################################################################################################################
void CConstraintMgr::GotoViolatorObject(CComPtr<IUnknown> &gotoPunk)
{
	if (!gotoPunk)
		return;
		
	COMTRY 
	{	
		CComBSTR app("GME.Application");
		CComPtr<IMgaClient> client;
		COMTHROW(m_spProject->GetClientByName((BSTR)app, &client));

		CComPtr<IDispatch> gui;
		COMTHROW(client->get_OLEServer(&gui));
		CComPtr<IGMEOLEApp> oleapp;
		COMTHROW(gui.QueryInterface(&oleapp));
/*		COleDispatchDriver dispdrv(gui); 
		
		const BYTE BASED_CODE partypes[] = VTS_UNKNOWN;
		try
		{
			dispdrv.InvokeHelper(33, DISPATCH_METHOD, VT_EMPTY, NULL, partypes, (IUnknown*)gotoPunk);
		}
*/
		try
		{
			CComPtr<IMgaFCO> fco;
			COMTHROW(gotoPunk.QueryInterface(&fco));
			COMTHROW(oleapp->ShowFCO(fco, FALSE));
		}
		catch (CException*)
		{
		}

	} 
	catch(hresult_exception &)
	{ 
		return ; 
	} 
}

STDMETHODIMP CConstraintMgr::ObjectsInvokeEx( IMgaProject *p, IMgaObject *o, IMgaObjects* os, long k )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if ( ! m_Facade.m_bEnabled )
		return S_OK;

	CComPtr<IUnknown> gotoPunk;
	COMTRY {

		// Show ConstraintBrowser Dialog

		if ( k == CONSTMGR_SHOW_CONSTRAINTS ) {

			m_Facade.m_bEnabledEvents = false;

			CComPtr<IMgaTerritory> spTerritory;
			COMTHROW( m_spProject->CreateTerritory( NULL, &spTerritory ) );
			COMTHROW( m_spProject->BeginTransaction( spTerritory, TRANSACTION_GENERAL));

			OclGmeCM::CConstraintBrowserDialog dlgConstraints( &m_Facade, NULL, NULL, NULL );
			if ( dlgConstraints.DoModal() == IDOK )
				COMTHROW( m_spProject->CommitTransaction() );
			else
				COMTHROW( m_spProject->AbortTransaction() );
			
			m_Facade.m_bEnabledEvents = true;

			dlgConstraints.GetGotoPunk(&gotoPunk);
			GotoViolatorObject(gotoPunk);
			return S_OK;
		}

		// Evaluate All Constraints

		CComPtr<IMgaTerritory> spTerritory;
		COMTHROW( m_spProject->CreateTerritory( NULL, &spTerritory ) );
		COMTHROW( m_spProject->BeginTransaction( spTerritory, TRANSACTION_READ_ONLY ) );

		try {

			if ( ! o && ! os )
				m_Facade.EvaluateAll(&gotoPunk);
			else if ( o ) {
				CComPtr<IMgaObject> spObject;
				COMTHROW( spTerritory->OpenObj( o, &spObject ) );
				m_Facade.Evaluate( spObject, &gotoPunk );
			}
			if ( os ) {
				OclCommonEx::ObjectVector vecObjects;
				MGACOLL_ITERATE( IMgaObject, os ) {
					CComPtr<IMgaObject> spObject;
					COMTHROW( spTerritory->OpenObj( MGACOLL_ITER, &spObject ) );
					vecObjects.push_back( spObject.p );
				} MGACOLL_ITERATE_END;
				m_Facade.Evaluate( vecObjects, &gotoPunk );
			}
			COMTHROW( p->AbortTransaction() );
			// ?? !! gotoPunk
			GotoViolatorObject(gotoPunk);


		} catch ( hresult_exception hEx ) {
			COMTHROW( p->AbortTransaction() );
			throw hEx;
		}

	} COMCATCH( ; )

}

STDMETHODIMP CConstraintMgr::GlobalEvent( globalevent_enum event )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	switch( event ) {
		case GLOBALEVENT_CLOSE_PROJECT: {
			m_spAddOn = NULL;
			break;
		}

		case GLOBALEVENT_COMMIT_TRANSACTION:
		case GLOBALEVENT_ABORT_TRANSACTION: {
			break;
		}

		case GLOBALEVENT_NOTIFICATION_READY: {
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP CConstraintMgr::ObjectEvent( IMgaObject* obj, unsigned long eventmask, VARIANT v )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if ( ! m_Facade.m_bEnabled || ! m_Facade.m_bEnabledInteractions || ! m_Facade.m_bEnabledEvents ) {
		return S_OK;
	}
	ASSERT(!(eventmask & OBJEVENT_MOUSEOVER)); // shouldn't receive these events because of put_EventMask
	ASSERT(!(eventmask & OBJEVENT_PRE_DESTROYED));

	COMTRY {
		return m_Facade.Evaluate( obj, eventmask);
	} COMCATCH( ; )
}

STDMETHODIMP CConstraintMgr::Initialize( IMgaProject *p )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_spProject = p;

	CreateComObject( m_spEventSink );
	m_spEventSink->cm = this;

	m_spAddOn = NULL;

	COMTRY {
		COMTHROW( m_spProject->CreateAddOn( m_spEventSink, &m_spAddOn ) );
		COMTHROW( m_spAddOn->put_EventMask( ~(OBJEVENT_MOUSEOVER | OBJEVENT_PRE_DESTROYED) )  );

		CComPtr<IMgaTerritory> spTerritory;
		COMTHROW( m_spProject->CreateTerritory( NULL, &spTerritory ) );
		COMTHROW( m_spProject->BeginTransaction( spTerritory, TRANSACTION_READ_ONLY ) );

		m_Facade.Initialize( p );

		CWaitCursor crsrWait;

		OclGme::ConstraintFunctionVector vecConstraintFunctions;
		m_Facade.LoadMetaConstraintFunctions( vecConstraintFunctions );
		OclGme::ConstraintVector vecConstraints;
		m_Facade.LoadMetaConstraints( vecConstraints );
		m_Facade.LoadUserConstraints( vecConstraints );

		crsrWait.Restore();

		if ( ! vecConstraints.empty() || ! vecConstraintFunctions.empty() ) {
			CSyntacticSemanticDialog dlgErrors;
			for ( unsigned int i = 0 ; i < vecConstraintFunctions.size() ; i++ )
				dlgErrors.AddItem( vecConstraintFunctions[ i ] );
			for ( unsigned int i = 0 ; i < vecConstraints.size() ; i++ )
				dlgErrors.AddItem( vecConstraints[ i ] );
			dlgErrors.DoModal();
		}

		COMTHROW( m_spProject->CommitTransaction() );

		return S_OK;

	} COMCATCH( ; )
}

STDMETHODIMP CConstraintMgr::Enable( VARIANT_BOOL enabled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_Facade.m_bEnabled = ( enabled ) ? true : false;
	return S_OK;
}

STDMETHODIMP CConstraintMgr::get_InteractiveMode( VARIANT_BOOL* enabled )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	*enabled = ( m_Facade.m_bEnabledInteractions ) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CConstraintMgr::put_InteractiveMode( VARIANT_BOOL enabled )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	m_Facade.m_bEnabledInteractions = ( enabled ) ? true : false;
	return S_OK;
}

STDMETHODIMP CConstraintMgr::get_ComponentParameter( BSTR name, VARIANT *pVal )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	return S_OK;
}

STDMETHODIMP CConstraintMgr::put_ComponentParameter(BSTR name, VARIANT newVal)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	return S_OK;
}

STDMETHODIMP CConstraintMgr::get_Paradigm( BSTR *pname )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	if( ! pname )
		return E_MGA_OUTPTR_NULL;
	if( *pname )
		return E_MGA_OUTPTR_NONEMPTY;
	*pname = CComBSTR( MGR_PARADIGM ).Detach();
	return S_OK;
}

STDMETHODIMP CConstraintMgr::get_ComponentName( BSTR *pname )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	if( ! pname )
		return E_MGA_OUTPTR_NULL;
	if( *pname )
		return E_MGA_OUTPTR_NONEMPTY;
	*pname = CComBSTR( MGR_COMPONENT_NAME ).Detach();
	return S_OK;
}

STDMETHODIMP CConstraintMgr::get_ComponentProgID( BSTR *pname )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	if( !pname )
		return E_MGA_OUTPTR_NULL;
	if( *pname )
		return E_MGA_OUTPTR_NONEMPTY;
	*pname = CComBSTR( MGR_COMPONENT_PROGID ).Detach();
	return S_OK;
}

STDMETHODIMP CConstraintMgr::get_ComponentType( componenttype_enum *ct )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	if( ! ct )
		return E_MGA_OUTPTR_NULL;
	*ct = MGR_CETYPE;
	return S_OK;
}

STDMETHODIMP CConstraintMgr::Invoke(IMgaProject *p, IMgaFCOs *os, long k) {
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	COMTRY {
		CComPtr<IMgaFCO> o;
		if(os) {
			long l;
			COMTHROW(os->get_Count(&l));
			if(l > 0) COMTHROW(os->get_Item(1, &o));
		}
		CComPtr<IMgaTerritory> t;
		COMTHROW(p->get_ActiveTerritory(&t));
		COMTHROW(p->AbortTransaction());
		COMTHROW(ObjectsInvokeEx(p, o, NULL, k));
		COMTHROW(p->BeginTransaction(t, TRANSACTION_GENERAL));
	} COMCATCH(;)
}

STDMETHODIMP CConstraintMgr::InvokeEx(IMgaProject *p, IMgaFCO *o, IMgaFCOs *, long k)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	return ObjectsInvokeEx( p, o, NULL, k );
}
