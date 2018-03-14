// ExpressionChecker.cpp : Implementation of CExpressionChecker

#include "StdAfx.h"
#include "ConstraintManager.h"
#include "ExpressionChecker.h"
#include "GMESyntacticSemanticDialog.h"
#include "GMESmallMessageBox.h"
#include "OCLCommonEx.h"

//##############################################################################################################################################
//
//	C L A S S : CExpEventSink
//
//##############################################################################################################################################

STDMETHODIMP CExpEventSink::GlobalEvent( globalevent_enum event )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	return ec->GlobalEvent( event );
}

STDMETHODIMP CExpEventSink::ObjectEvent( IMgaObject *obj, unsigned long eventmask, VARIANT v )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	return ec->ObjectEvent( obj, eventmask, v );
}

//##############################################################################################################################################
//
//	C L A S S : CExpressionChecker
//
//##############################################################################################################################################

STDMETHODIMP CExpressionChecker::ObjectsInvokeEx( IMgaProject *p, IMgaObject *o, IMgaObjects* os, long k )
{
	HRESULT ret = E_ABORT;
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	if ( ! m_Facade.m_bEnabled )
		return S_OK;

	//AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	COMTRY {

		CWaitCursor crsrWait;

		OclGme::ConstraintFunctionVector vecConstraintFunctions;
		m_Facade.CheckAllConstraintFunctions( vecConstraintFunctions );
		OclGme::ConstraintVector vecConstraints;
		m_Facade.CheckAllConstraints( vecConstraints );

		crsrWait.Restore();

		if ( ! vecConstraints.empty() || ! vecConstraintFunctions.empty() ) {
			CSyntacticSemanticDialog dlgErrors( AfxGetApp()->m_pActiveWnd );
			for ( unsigned int i = 0 ; i < vecConstraintFunctions.size() ; i++ )
				dlgErrors.AddItem( vecConstraintFunctions[ i ] );
			for ( unsigned int i = 0 ; i < vecConstraints.size() ; i++ )
				dlgErrors.AddItem( vecConstraints[ i ] );
			dlgErrors.DoModal();
			ret = S_FALSE;
		}
		else
		{
			CSmallMessageBox().DoModal();
			ret = S_OK;
		}

		m_Facade.Finalize();

		return ret;
	} COMCATCH( ASSERT( 0 ); )
}

STDMETHODIMP CExpressionChecker::GlobalEvent( globalevent_enum event )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	switch( event ) {
		case GLOBALEVENT_CLOSE_PROJECT: {
			m_spAddOn = NULL;
			break;
		}

		case GLOBALEVENT_COMMIT_TRANSACTION:
		case GLOBALEVENT_ABORT_TRANSACTION: {
			m_Facade.m_bEnabledEvents = true;
			break;
		}

		case GLOBALEVENT_NOTIFICATION_READY: {
			m_Facade.m_bEnabledEvents = true;
			break;
		}

		case APPEVENT_XML_IMPORT_BEGIN:
		//case APPEVENT_XML_IMPORT_FCOS_BEGIN:
		//case APPEVENT_XML_IMPORT_SPECIAL_BEGIN:
		case APPEVENT_LIB_ATTACH_BEGIN: {
			m_Facade.m_bEnabledEvents = false;
			break;
		}

		case APPEVENT_XML_IMPORT_END:
		//case APPEVENT_XML_IMPORT_FCOS_END:
		//case APPEVENT_XML_IMPORT_SPECIAL_END:
		case APPEVENT_LIB_ATTACH_END: {
			m_Facade.m_bEnabledEvents = true;
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP CExpressionChecker::ObjectEvent( IMgaObject *obj, unsigned long eventmask, VARIANT v )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z

	if ( ! m_Facade.m_bEnabled || ! m_Facade.m_bEnabledInteractions || ! m_Facade.m_bEnabledEvents ) {
		return S_OK;
	}

	CWnd* pMainWindow = AfxGetApp()->m_pActiveWnd;
	
	COMTRY {

		if( ( eventmask & OBJEVENT_ATTR ) || ( eventmask & OBJEVENT_CONNECTED ) ) {

			CComQIPtr<IMgaFCO> spFCO = obj;
			if( spFCO ) {
				std::string strKind = OclCommonEx::GetObjectKind( spFCO.p );
				if ( strKind == "Constraint" ) {
					CWaitCursor crsrWait;
					OclGme::ConstraintVector vecConstraints;
					m_Facade.CheckConstraint( spFCO.p, vecConstraints, false );
					if ( ! vecConstraints.empty() ) {
						CSyntacticSemanticDialog dlgErrors( pMainWindow );
						for ( unsigned int i = 0 ; i < vecConstraints.size() ; i++ )
							dlgErrors.AddItem( vecConstraints[ i ] );
						crsrWait.Restore();
						dlgErrors.DoModal();
					}
					else
						crsrWait.Restore();
				}
				else if ( strKind == "ConstraintFunc" ) {
					CWaitCursor crsrWait;
					OclGme::ConstraintFunctionVector vecConstraintFunctions;
					m_Facade.CheckConstraintFunction( spFCO.p, vecConstraintFunctions, false );
					if ( ! vecConstraintFunctions.empty() ) {
						CSyntacticSemanticDialog dlgErrors( pMainWindow );
						for ( unsigned int i = 0 ; i < vecConstraintFunctions.size() ; i++ )
							dlgErrors.AddItem( vecConstraintFunctions[ i ] );
						crsrWait.Restore();
						dlgErrors.DoModal();
					}
					else
						crsrWait.Restore();
				}
				m_Facade.Finalize();
			}
		} else
			ASSERT(false); // Shouldn't get other events because of put_EventMask

	} COMCATCH( ASSERT( 0 ); )
}

STDMETHODIMP CExpressionChecker::Initialize( IMgaProject *p )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_spProject = p;

	CreateComObject( m_spEventSink );
	m_spEventSink->ec = this;

	m_spAddOn = NULL;
	COMTRY {
		COMTHROW( m_spProject->CreateAddOn( m_spEventSink, &m_spAddOn ) );
		COMTHROW(m_spAddOn->put_EventMask(OBJEVENT_ATTR | OBJEVENT_CONNECTED));

		m_Facade.Initialize( p );
	} COMCATCH( ASSERT( 0 ); )

}

STDMETHODIMP CExpressionChecker::Enable( VARIANT_BOOL enabled )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	m_Facade.m_bEnabled = ( enabled != VARIANT_TRUE ) ? true : false;
	return S_OK;
}

STDMETHODIMP CExpressionChecker::get_InteractiveMode( VARIANT_BOOL * enabled )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	*enabled = ( m_Facade.m_bEnabledInteractions ) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CExpressionChecker::put_InteractiveMode( VARIANT_BOOL enabled )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	m_Facade.m_bEnabledInteractions = ( enabled ) ? true : false;
	return S_OK;
}

STDMETHODIMP CExpressionChecker::get_ComponentParameter( BSTR name, VARIANT *pVal )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	return S_OK;
}

STDMETHODIMP CExpressionChecker::put_ComponentParameter( BSTR name, VARIANT newVal )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	return S_OK;
}

STDMETHODIMP CExpressionChecker::get_Paradigm( BSTR *pname )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	if( ! pname )
		return E_MGA_OUTPTR_NULL;
	if( *pname )
		return E_MGA_OUTPTR_NONEMPTY;
	*pname = CComBSTR( EXP_PARADIGM ).Detach();
	return S_OK;
}

STDMETHODIMP CExpressionChecker::get_ComponentName( BSTR *pname )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	if( ! pname )
		return E_MGA_OUTPTR_NULL;
	if( *pname )
		return E_MGA_OUTPTR_NONEMPTY;
	*pname = CComBSTR( EXP_COMPONENT_NAME ).Detach();
	return S_OK;

}

STDMETHODIMP CExpressionChecker::get_ComponentProgID( BSTR *pname )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	if( ! pname )
		return E_MGA_OUTPTR_NULL;
	if( *pname )
		return E_MGA_OUTPTR_NONEMPTY;
	*pname = CComBSTR( EXP_COMPONENT_PROGID ).Detach();
	return S_OK;
}

STDMETHODIMP CExpressionChecker::get_ComponentType( componenttype_enum *ct )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	if( ! ct )
		return E_MGA_OUTPTR_NULL;
	*ct = EXP_CETYPE;
	return S_OK;
}

STDMETHODIMP CExpressionChecker::Invoke( IMgaProject *p, IMgaFCOs *os, long k )
{
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
		HRESULT ret = ObjectsInvokeEx(p, o, NULL, k);
		COMTHROW(ret);
		COMTHROW(p->BeginTransaction(t, TRANSACTION_GENERAL));
		return ret;
	} COMCATCH(;)
}

STDMETHODIMP CExpressionChecker::InvokeEx( IMgaProject *p, IMgaFCO *o, IMgaFCOs *, long k )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState());//z
	return ObjectsInvokeEx(p, o, NULL, k);
}
