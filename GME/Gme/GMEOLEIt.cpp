// GMEOLEActiveModel.cpp : implementation file
//

#include "stdafx.h"
#include "gme.h"
#include "GMEOLEIt.h"
#include "GMEOLEAspect.h"
#include "GMEOLEError.h"
#include "GMEOLEColl.h"
#include "GMEDoc.h"
#include "MainFrm.h"
#include "GMEEventLogger.h"
#include "CommonStl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CATCHALL_AND_CALL_END \
	catch( const wchar_t * p) \
	{ \
		end(); \
		SHOW_ERROR( p ); \
	} \
	catch(...) \
	{ \
		end(); \
		SHOW_ERROR( L"Unhandled error occurred" ); \
	} \
	end();

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEIt

IMPLEMENT_DYNCREATE(CGMEOLEIt, CCmdTarget)

CGMEOLEIt::CGMEOLEIt()
	: m_theView(0)
	, m_theMgaModel(0)
	, m_isInUserInitiatedTransaction( false)
{
	EnableAutomation();
}

CGMEOLEIt::~CGMEOLEIt()
{
}


void CGMEOLEIt::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


bool CGMEOLEIt::AmIValid()
{
	m_theView = 0;
	m_theMgaModel = 0;
	CMDIChildWnd *pChild  = CMainFrame::theInstance->MDIGetActive();

	if (pChild) 
	{
		CGMEView *view = (CGMEView*)pChild->GetActiveView();
		if (view)
		{
			CComPtr<IMgaModel> mm = view->GetCurrentModel();	//CComPtr<IMgaModel> &
			if( mm )
			{
				m_theView = view;
				m_theMgaModel = mm;
				return true;
			}
		}
	}
	return false;
}

bool CGMEOLEIt::beginTrans()
{
	CComObjPtr<IMgaTerritory> territory;

	COMTHROW( theApp.mgaProject->CreateTerritory(NULL, PutOut(territory), NULL) );
	COMTHROW( theApp.mgaProject->BeginTransaction(territory, TRANSACTION_GENERAL) ); 

	return true;
}

bool CGMEOLEIt::commitTrans()
{
	try
	{
		CComPtr<IMgaTerritory> act_terr;
		COMTHROW( theApp.mgaProject->get_ActiveTerritory( &act_terr));
		if( act_terr) act_terr->Flush();
		COMTHROW( theApp.mgaProject->CommitTransaction());
		//act_terr = 0;
	}
	catch( ... )
	{
		COMTHROW( theApp.mgaProject->AbortTransaction());
	}

	return true;
}

bool CGMEOLEIt::abortTrans()
{
	try
	{
		CComPtr<IMgaTerritory> act_terr;
		COMTHROW( theApp.mgaProject->get_ActiveTerritory( &act_terr));
		if( act_terr) act_terr->Flush();

		COMTHROW( theApp.mgaProject->AbortTransaction());
	}
	catch( ... )
	{
	}

	return true;
}

CComPtr<IMgaFCO> CGMEOLEIt::putInTerr( const CComPtr<IMgaFCO>& p_oneFCO)
{
	CComPtr<IMgaTerritory> act_terr;
	COMTHROW( theApp.mgaProject->get_ActiveTerritory( &act_terr));

	if( !act_terr && !m_isInUserInitiatedTransaction && !m_initedByMe) return 0;
	if( !p_oneFCO) return 0;

	CComPtr<IMgaFCO> out_fco;
	COMTHROW(act_terr->OpenFCO( p_oneFCO, &out_fco));
	
	return out_fco.Detach();

}

LPDISPATCH CGMEOLEIt::putInTerr( LPDISPATCH p_oneObj)
{
	return putInTerr( CComQIPtr<IMgaFCO>( p_oneObj)).Detach();
}

// if invoked, this method assures that either a user transaction, or a freshly created transaction is open
bool CGMEOLEIt::start()
{
	// a transaction may be started like: project.BeginTransaction() also 
	// not only through the this.BeginTransaction() (this = CGMEOLEIt)
	// that is why we prepare for that as well

	// m_isInUserInitiatedTransaction <==> it has been started with this.BeginTransaction()
	// m_initedByMe <==> there was no active transaction, so we start one in order to function correctly
	//                   and we have to commit it when the method exits
	// 

	CComObjPtr<IMgaTerritory> territory;
	COMTHROW( theApp.mgaProject->get_ActiveTerritory( PutOut( territory)));

	m_initedByMe = false;
	if( !m_isInUserInitiatedTransaction && !territory) // if 0 then no trasaction is going on
	{
		this->beginTrans();
		m_initedByMe = true;
	}

	return true;
}

// closes the transaction if it was created automatically ( not by the user )
bool CGMEOLEIt::end()
{
	if( m_initedByMe)
	{
		this->commitTrans();
	}

	return true;
}

//static
void CGMEOLEIt::rename( CComPtr<IMgaFCO> pFco, LPCTSTR pName)
{
	CComBSTR _name( pName);
	if( pFco)
		COMTHROW( pFco->put_Name( _name));
}

//static
CComPtr<IMgaFCO> CGMEOLEIt::getChildInByName( CComPtr<IMgaModel> model, LPCTSTR name )
{
	return fcoByPath( model, name);
#if(0)
	CComPtr<IMgaFCO> ch;
	CComBSTR nm( (LPCTSTR) name);
	if( model) COMTHROW( model->get_ChildFCO( nm, &ch));
	if( ch)
	{
		return ch;
	}
	return 0;
#endif
}

//static
bool CGMEOLEIt::isValidChildIn( CComPtr<IMgaModel> model, CComPtr<IMgaFCO> fco) // is valid child (aspect related information( hidden, shown is disregarded now)
{
	CComBSTR nm;
	if( fco) COMTHROW( fco->get_Name( &nm ));

	CComPtr<IMgaFCO> ch;
	if( model) COMTHROW( model->get_ChildFCO( nm, &ch));
	if( ch && ch == fco)
	{
		return true;
	}
	return false;
}

// gives back the metarole ptr based on a part's name
//static
CComPtr<IMgaMetaRole> CGMEOLEIt::metaRolePtrInByName( CComPtr<IMgaModel> p_model, CComBSTR p_part )
{
	CComObjPtr<IMgaMetaFCO> fco_meta;
	if( p_model) COMTHROW( p_model->get_Meta( PutOut( fco_meta)));

	CComObjPtr<IMgaMetaModel> model_meta;
	COMTHROW( fco_meta.QueryInterface( model_meta));
	THROW_IF_NULL( model_meta, _T("Invalid Meta object"));

	CComPtr<IMgaMetaRole> metarole;
	// COMTHROW not used since it is common to pass in wrong names:
	model_meta->get_RoleByName( p_part, &metarole); // may throw if bad kind is asked
	THROW_IF_NULL( metarole, _T("No such metarole found"));

	return metarole;
}

// gives back the meta rolename
//static
CComBSTR CGMEOLEIt::roleNameOf( CComPtr<IMgaFCO> one_fco )
{
	CComObjPtr<IMgaMetaFCO> fco_meta;
	if( one_fco) COMTHROW( one_fco->get_Meta( PutOut( fco_meta)));

	CComBSTR mrn;
	if( fco_meta) COMTHROW( fco_meta->get_Name( &mrn));

	return mrn;
}

// gives back a connection metarole ptr based on src and dst and container
//static
CComPtr<IMgaMetaRole> CGMEOLEIt::connMetaRolePtrInBetween( CComPtr<IMgaModel> p_model, CComPtr<IMgaFCO> p_src, CComPtr<IMgaFCO> p_dst )
{
	CComBSTR to_connect_as_src = roleNameOf( p_src);
	CComBSTR to_connect_as_dst = roleNameOf( p_dst);

	return connMetaRolePtrInBetween( p_model, to_connect_as_src, to_connect_as_dst);
}

//static 
CComPtr<IMgaMetaRole> CGMEOLEIt::connMetaRolePtrInBetween( CComPtr<IMgaModel> p_model, CComPtr<IMgaFCO> p_srcRole1, CComPtr<IMgaFCO> p_srcRole2, CComPtr<IMgaFCO> p_dstRole1, CComPtr<IMgaFCO> p_dstRole2)
{
	CComBSTR to_connect_as_src = roleNameOf( p_srcRole1);
	CComBSTR to_connect_as_dst = roleNameOf( p_dstRole1);

	COMTHROW(to_connect_as_src.Append(_T(" ")));
	COMTHROW(to_connect_as_src.AppendBSTR( roleNameOf( p_srcRole2))); // now we have a path like: "model port" composed of roles
	COMTHROW(to_connect_as_dst.Append(_T(" ")));
	COMTHROW(to_connect_as_dst.AppendBSTR( roleNameOf( p_dstRole2)));

	return connMetaRolePtrInBetween( p_model, to_connect_as_src, to_connect_as_dst);
}

//static 
CComPtr<IMgaMetaRole> CGMEOLEIt::connMetaRolePtrInBetween( CComPtr<IMgaModel> p_model, const CComBSTR& src_localpath, const CComBSTR& dst_localpath)
{
	CComBSTR src_nm( L"src");
	CComBSTR dst_nm( L"dst");

	// check for connection allowed in p_model between p_src and p_dst

	CComObjPtr<IMgaMetaFCO> fco_meta;
	if( p_model) COMTHROW( p_model->get_Meta( PutOut( fco_meta)));

	CComObjPtr<IMgaMetaModel> model_meta;
	COMTHROW( fco_meta.QueryInterface( model_meta));

	// the model can contain these connections
	CComObjPtrVector<IMgaMetaRole> conn_metaroles;
	if( model_meta) COMTHROW( model_meta->LegalConnectionRoles( 0, PutOut( conn_metaroles )));

	// will store the possible connection meta roles
	CComObjPtrVector<IMgaMetaRole> valid_conn_metaroles;

	CComObjPtrVector<IMgaMetaRole>::iterator coll_it = conn_metaroles.begin();
	CComObjPtrVector<IMgaMetaRole>::iterator coll_end = conn_metaroles.end();
	for( ; coll_it != coll_end; ++coll_it)
	{
		bool src_found = false;
		bool dst_found = false;

		CComObjPtr<IMgaMetaFCO> fco_meta;
		COMTHROW( (*coll_it)->get_Kind( PutOut( fco_meta)));

		CComObjPtr<IMgaMetaConnection> conn_meta; 
		COMTHROW( fco_meta.QueryInterface( conn_meta ));
		
		// joints the connection may have
		CComObjPtrVector<IMgaMetaConnJoint> jnts;
		if( conn_meta) COMTHROW( conn_meta->get_Joints( PutOut( jnts )));

		CComObjPtrVector<IMgaMetaConnJoint>::iterator jnt_it = jnts.begin();
		CComObjPtrVector<IMgaMetaConnJoint>::iterator jnt_end = jnts.end();
		for( ; jnt_it != jnt_end; ++jnt_it)
		{
			CComObjPtrVector<IMgaMetaPointerSpec> pntspecs;
			COMTHROW( (*jnt_it)->get_PointerSpecs( PutOut( pntspecs)));


			CComObjPtrVector<IMgaMetaPointerSpec>::iterator pntspec_it = pntspecs.begin();
			CComObjPtrVector<IMgaMetaPointerSpec>::iterator pntspec_end = pntspecs.end();
			for( ; pntspec_it != pntspec_end; ++pntspec_it)
			{
				CComBSTR nm; // "src" or "dst" for connections
				COMTHROW( (*pntspec_it)->get_Name( &nm));

				CComObjPtrVector<IMgaMetaPointerItem> pntitems;
				COMTHROW( (*pntspec_it)->get_Items( PutOut( pntitems)));

				CComObjPtrVector<IMgaMetaPointerItem>::iterator item_it = pntitems.begin();
				CComObjPtrVector<IMgaMetaPointerItem>::iterator item_end = pntitems.end();
				for( ; item_it != item_end; ++item_it)
				{
					CComBSTR ds; // "role1 role2" or "role" most tipically (see local path defined in meta.idl)
					COMTHROW( (*item_it)->get_Desc( &ds ));

					if( nm == _T("src") && ds == src_localpath)
						src_found = true;
					if( nm == _T("dst") && ds == dst_localpath)
						dst_found = true;
				}
			}
		}
		if( src_found && dst_found)
		{
			valid_conn_metaroles.push_back( *coll_it);
		}
	}
	
	if( valid_conn_metaroles.empty())
		return 0;
	if( valid_conn_metaroles.size() > 1 )
		return 0;

	return CComPtr<IMgaMetaRole>( valid_conn_metaroles[0].p);
}

//static
objtype_enum CGMEOLEIt::myTypeIs( CComPtr<IMgaFCO> fco)
{
	CComPtr<IMgaMetaRole> mrole;
	if( fco) COMTHROW( fco->get_MetaRole( &mrole ));
	
	CComPtr<IMgaMetaFCO> mfco;
	if( mrole) COMTHROW( mrole->get_Kind( &mfco));

	objtype_enum type_info;
	if( mfco) COMTHROW( mfco->get_ObjType( &type_info));

	return type_info;
}

//indicates if the two collections contain the same set of fcos (the order is important)
//static 
bool CGMEOLEIt::areCollectionsEqual( const CComPtr<IMgaFCOs>& op1, const CComPtr<IMgaFCOs>& op2)
{
	bool res = false;
	long sz1(0), sz2(0);
	if( op1) COMTHROW( op1->get_Count( &sz1));
	if( op2) COMTHROW( op2->get_Count( &sz2));
	if( sz1 == sz2) 
	{
		bool conti = true;
		for( long i = 1; conti && i <= sz1; ++i)
		{
			conti = false;
			CComPtr<IMgaFCO> it1, it2;
			COMTHROW( op1->get_Item( i, &it1));
			COMTHROW( op2->get_Item( i, &it2));
			if( it1 == it2)
				conti = true;
		}
		res = conti;
	}
	return res;
}

// finds a port with p_portRole name
//     -or inside a model referred by p_ref
//     -or inside a model referred by a reference referred by p_ref
//     -or inside a model (referred by a reference)* referred by p_ref
// and builds the reference collection
// returns: the fcoptr of the port
//          the reference collection
//
//static
void CGMEOLEIt::refPortFinderAndChainBuilder( LPCTSTR p_portRole, CComPtr<IMgaFCO> p_ref, CComPtr<IMgaFCO>& p_portResult, CComPtr<IMgaFCOs>& p_refChainResult )
{
	CComQIPtr<IMgaReference> ref( p_ref );

	bool go_on = true;
	while( go_on && ref)
	{
		CComPtr<IMgaFCO> referd;
		if( ref) COMTHROW( ref->get_Referred( &referd));

		CComQIPtr<IMgaModel> model( referd);
		if( model) // the reference points to a model truly
		{
			p_portResult = getChildInByName( model, p_portRole);

			if( !p_refChainResult)
				COMTHROW(p_refChainResult.CoCreateInstance( L"Mga.MgaFCOs"));

			COMTHROW( p_refChainResult->Append( ref ));

			go_on = false;
		}
		else
		{
			CComQIPtr<IMgaReference> ref2( referd); // the reference (ref) may point 
			// to another reference (ref2)
			// which may point to a model containing the port we are looking for

			if( ref2) //yes it seems
			{
				if( !p_refChainResult)
					COMTHROW(p_refChainResult.CoCreateInstance( L"Mga.MgaFCOs"));

				COMTHROW( p_refChainResult->Append( ref ));
				ref = ref2;
			}
			else
			{
				// no more trouble, the loop ends with the conclusion that the
				// original reference is the connection target and not the model
				// it is referring to, but then raises the question why did the
				// user call the ''ConnectThruPort method at all?
			}
		}
	}
}

// verifies if p_port
//     -is inside a model referred by p_ref
//     -or inside a model referred by a reference referred by p_ref
//     -or inside a model (referred by a reference)* referred by p_ref
// and builds the reference collection (chain)
//
// returns: success or failure
//          the reference collection
//
// used by ConnectThruPortFCO
//static
bool CGMEOLEIt::refChainBuilder2( CComPtr<IMgaFCO> p_ref, CComPtr<IMgaFCO> p_port, CComPtr<IMgaFCOs>& coll) 
{
	CComQIPtr<IMgaReference> ref( p_ref);
	CComPtr<IMgaFCO> tgt;
	if( !coll) 
		COMTHROW(coll.CoCreateInstance( L"Mga.MgaFCOs"));
	VERIFY( coll);

	bool valid = ref != 0;
	while( ref && valid) 
	{
		COMTHROW( coll->Append( ref));
		COMTHROW( ref->get_Referred( &tgt));
		CComQIPtr<IMgaModel>     tgt_model( tgt);
		CComQIPtr<IMgaReference> tgt_isRef( tgt);
		if( tgt_model) 
		{
			ref = CComPtr<IMgaReference>(0);
			valid = isValidChildIn( tgt_model.p, p_port.p);
		}
		else
		{
			ref = tgt_isRef;
			valid = ref != 0;
		}
	}

	return valid;
}

// static
bool CGMEOLEIt::createObjs( CComPtr<IMgaModel>& p_parent, LPCTSTR p_partToCreate, LPCTSTR p_givenName, IMgaFCO** p_pNewObj )
{
	if( !p_parent) return false;

	CComObjPtr<IMgaMetaFCO> fco_meta;
	COMTHROW( p_parent->get_Meta( PutOut( fco_meta)));

	CComObjPtr<IMgaMetaModel> model_meta;
	COMTHROW( fco_meta.QueryInterface( model_meta));

	CComObjPtr<IMgaMetaRole> metarole;
	
	// COMTHROW not used since it is common to pass in wrong names:
	model_meta->get_RoleByName( CComBSTR( p_partToCreate), PutOut( metarole));
	THROW_IF_NULL( metarole, _T("No such metarole found"));
	
	CComObjPtr<IMgaFCO> newfco;
	COMTHROW( p_parent->CreateChildObject( metarole, PutOut( newfco)));
	THROW_IF_NULL( newfco, _T("Invalid new object pointer"));

	CComBSTR new_name(L"New"); new_name.Append( p_partToCreate);
	CComBSTR b_given_name( p_givenName);

	if( newfco)
		COMTHROW( newfco->put_Name( b_given_name == _T("")? new_name : b_given_name ));

	ASSERT( p_pNewObj);
	*p_pNewObj = newfco.Detach();

	return true;
}

//static
bool CGMEOLEIt::cloneObjs( CComPtr<IMgaModel>& p_parent, IMgaFCO* p_existingFcoPtr, LPCTSTR p_existingFcoName, LPCTSTR p_givenName, IMgaFCO** p_pNewObj )
{
	if( !p_parent) return false;

	CComObjPtr<IMgaMetaFCO> fco_meta;
	COMTHROW( p_parent->get_Meta( PutOut( fco_meta)));

	CComObjPtr<IMgaMetaModel> model_meta;
	COMTHROW( fco_meta.QueryInterface( model_meta));

	CComObjPtr<IMgaFCO> existing_fco;
	
	if( p_existingFcoPtr)	// if ptr provided it is assumed is in the territory
	{
		existing_fco = p_existingFcoPtr;
	}
	else // else use p_part parameter to determine the kind
	{
		COMTHROW( p_parent->get_ChildFCO( CComBSTR( p_existingFcoName), PutOut( existing_fco)));
	}

	if( !existing_fco) return false;

	CComPtr<IMgaFCOs> coll;
	COMTHROW( coll.CoCreateInstance( L"Mga.MgaFCOs" ));
	COMTHROW( coll->Append( existing_fco));

	CComObjPtr<IMgaMetaRole> ex_metarole;
	COMTHROW( existing_fco->get_MetaRole( PutOut( ex_metarole)));

	CComPtr<IMgaMetaRoles> col2;
	COMTHROW( col2.CoCreateInstance( L"Mga.MgaMetaRoles"));
	COMTHROW( col2->Append( ex_metarole));

	CComObjPtr<IMgaFCOs> col3;
	COMTHROW( p_parent->CopyFCOs( coll, col2, PutOut( col3 )));

	long l = 0;
	if( col3)
		COMTHROW( col3->get_Count(&l));

	if( l == 1)
	{
		CComPtr<IMgaFCO> newfco;
		COMTHROW( coll->get_Item( 1, &newfco));

		CComBSTR new_name(L"Cloned"); new_name.Append(p_existingFcoName);
		CComBSTR given_name( p_givenName);

		if( newfco)
			COMTHROW( newfco->put_Name( given_name == _T("") ? new_name : given_name));

		ASSERT( p_pNewObj);
		*p_pNewObj = newfco.Detach();
	}
	else
	{
		ASSERT( 0 );
		return false;
	}

	return true;
}


BEGIN_MESSAGE_MAP(CGMEOLEIt, CCmdTarget)
	//{{AFX_MSG_MAP(CGMEOLEIt)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CGMEOLEIt, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CGMEOLEIt)
	DISP_PROPERTY_EX(CGMEOLEIt, "Valid", GetValid, SetValid, VT_BOOL)
	DISP_PROPERTY_EX(CGMEOLEIt, "Aspects", GetAspects, SetAspects, VT_DISPATCH)
	DISP_PROPERTY_EX(CGMEOLEIt, "MgaModel", GetMgaModel, SetMgaModel, VT_DISPATCH)

	DISP_FUNCTION(CGMEOLEIt, "Print", Print, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "PrintDialog", PrintDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "DumpWindowsMetaFile", DumpWindowsMetaFile, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "CheckConstraints", CheckConstraints, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "RunComponent", RunComponent, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "RunComponentDialog", RunComponentDialog, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "Close", Close, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "GrayOutFCO", GrayOutFCO, VT_EMPTY, VTS_BOOL VTS_BOOL VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEIt, "GrayOutHide", GrayOutHide, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "ShowSetMembers", ShowSetMembers, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEIt, "HideSetMembers", HideSetMembers, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "Zoom", Zoom, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CGMEOLEIt, "ZoomTo", ZoomTo, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEIt, "Scroll", Scroll, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CGMEOLEIt, "DumpModelGeometryXML", DumpModelGeometryXML, VT_EMPTY, VTS_BSTR)


	DISP_FUNCTION(CGMEOLEIt, "ShowFCO", ShowFCO, VT_EMPTY, VTS_BSTR VTS_BOOL)
	DISP_FUNCTION(CGMEOLEIt, "ShowFCOPtr", ShowFCOPtr, VT_EMPTY, VTS_DISPATCH VTS_BOOL)
	DISP_FUNCTION(CGMEOLEIt, "Child", Child, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "Create", Create, VT_DISPATCH, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "CreateInChild", CreateInChild, VT_DISPATCH, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "CreateInChildFCO", CreateInChildFCO, VT_DISPATCH, VTS_DISPATCH VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "Duplicate", Duplicate, VT_DISPATCH, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "DuplicateFCO", DuplicateFCO, VT_DISPATCH, VTS_DISPATCH VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "Rename", Rename, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "SetName", SetName, VT_EMPTY, VTS_DISPATCH VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "Include", Include, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "IncludeFCO", IncludeFCO, VT_EMPTY, VTS_DISPATCH VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEIt, "Exclude", Exclude, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "ExcludeFCO", ExcludeFCO, VT_EMPTY, VTS_DISPATCH VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEIt, "Connect", Connect, VT_DISPATCH, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "Disconnect", Disconnect,  VT_EMPTY, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "ConnectThruPort", ConnectThruPort, VT_DISPATCH, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "ConnectThruPortFCO", ConnectThruPortFCO, VT_DISPATCH, VTS_DISPATCH VTS_DISPATCH VTS_DISPATCH VTS_DISPATCH VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "DisconnectThruPort", DisconnectThruPort, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "ConnectFCOs", ConnectFCOs, VT_DISPATCH, VTS_DISPATCH VTS_DISPATCH VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "DisconnectFCOs", DisconnectFCOs, VT_EMPTY, VTS_DISPATCH VTS_DISPATCH VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "Refer", Refer, VT_EMPTY, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "ClearRef", ClearRef, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "FollowRef", FollowRef, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "ReferFCO", ReferFCO, VT_EMPTY, VTS_DISPATCH VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEIt, "ClearRefFCO", ClearRefFCO, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEIt, "FollowRefFCO", FollowRefFCO, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEIt, "NullFCO", NullFCO, VT_DISPATCH, VTS_NONE)

	DISP_FUNCTION(CGMEOLEIt, "SetAttribute", SetAttribute, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CGMEOLEIt, "GetAttribute", GetAttribute, VT_VARIANT, VTS_BSTR VTS_BSTR)

	DISP_FUNCTION(CGMEOLEIt, "SetAttributeFCO", SetAttributeFCO, VT_EMPTY, VTS_DISPATCH VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CGMEOLEIt, "GetAttributeFCO", GetAttributeFCO, VT_VARIANT, VTS_DISPATCH VTS_BSTR)
	
	DISP_FUNCTION(CGMEOLEIt, "SubType", SubType, VT_DISPATCH, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "Instantiate", Instantiate, VT_DISPATCH, VTS_BSTR VTS_BSTR)

	DISP_FUNCTION(CGMEOLEIt, "SubTypeFCO", SubTypeFCO, VT_DISPATCH, VTS_DISPATCH VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "InstantiateFCO", InstantiateFCO, VT_DISPATCH, VTS_DISPATCH VTS_BSTR)

	DISP_FUNCTION(CGMEOLEIt, "BeginTransaction", BeginTransaction, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "CommitTransaction", CommitTransaction, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "AbortTransaction", AbortTransaction, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "IsInTransaction", IsInTransaction, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "PutInTerritory", PutInTerritory, VT_DISPATCH, VTS_DISPATCH)

	DISP_FUNCTION(CGMEOLEIt, "Help", Help, VT_EMPTY, VTS_NONE )

	DISP_FUNCTION(CGMEOLEIt, "SetSelected", SetSelected, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "GetSelected", GetSelected, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "SetSelectedFCOs", SetSelectedFCOs, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CGMEOLEIt, "GetSelectedFCOs", GetSelectedFCOs, VT_DISPATCH, VTS_NONE)

	DISP_FUNCTION(CGMEOLEIt, "GetCurrentAspect", GetCurrentAspect, VT_HRESULT, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "SetCurrentAspect", SetCurrentAspect, VT_HRESULT, VTS_BSTR)
	DISP_FUNCTION(CGMEOLEIt, "NextAspect", NextAspect, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "PrevAspect", PrevAspect, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "Next", Next, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "Prev", Prev, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CGMEOLEIt, "Position", Position, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	DISP_FUNCTION(CGMEOLEIt, "PositionFCO", PositionFCO, VT_EMPTY, VTS_DISPATCH VTS_BSTR VTS_I4 VTS_I4)

	
//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


BEGIN_INTERFACE_MAP(CGMEOLEIt, CCmdTarget)
	INTERFACE_PART(CGMEOLEIt, __uuidof(IGMEOLEIt), Dual)
	DUAL_ERRORINFO_PART(CGMEOLEIt)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMEOLEIt message handlers


BOOL CGMEOLEIt::GetValid() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::GetValid()\r\n"));

	return AmIValid() ? TRUE : FALSE;
}

void CGMEOLEIt::SetValid(BOOL) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::SetValid()\r\n"));

	SetNotSupported();
}

LPDISPATCH CGMEOLEIt::GetMgaModel() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::GetMgaModel()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComObjPtr<IMgaTerritory> territory;
	COMTHROW( theApp.mgaProject->get_ActiveTerritory( PutOut( territory)));
	
	if( territory)
	{
		// if transaction is going on, then put the pointer 
		// into its territory
		CComObjPtr<IMgaObject> obj;
		COMTHROW(territory->OpenObj( m_theMgaModel, PutOut(obj)));
		CComObjPtr<IMgaModel> model;
		COMTHROW(obj.QueryInterface( model));

		return model.Detach();
	}
	else
	{
		CComObjPtr<IMgaModel> model( m_theMgaModel);
		return model.Detach();
	}
}

void CGMEOLEIt::SetMgaModel(LPDISPATCH) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::SetMgaModel()\r\n"));

	SetNotSupported();
}

void CGMEOLEIt::Print() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Print()\r\n"));

	PRECONDITION_VALID_MODEL

	m_theView->SendMessage(WM_COMMAND, ID_FILE_PRINT_DIRECT);
}

void CGMEOLEIt::PrintDialog() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::PrintDialog()\r\n"));

	PRECONDITION_VALID_MODEL

	m_theView->SendMessage(WM_COMMAND, ID_FILE_PRINT);

}

void CGMEOLEIt::DumpWindowsMetaFile(LPCTSTR filePath) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::DumpWindowsMetaFile()\r\n"));

	PRECONDITION_VALID_MODEL

	CMetaFileDC cDC;
	BOOL ret = cDC.CreateEnhanced(m_theView->GetDC(),filePath,NULL,_T("GME Model"));
	if (ret == FALSE) {
		AfxMessageBox(_T("Unable to create metafile."), MB_OK | MB_ICONSTOP);
		return;
	}

	cDC.m_bPrinting = TRUE;		// HACK by Peter (c)
	m_theView->OnDraw(&cDC);

	HENHMETAFILE hEmf = cDC.CloseEnhanced();
	if ( hEmf ) {
		DeleteEnhMetaFile(hEmf);
	}
}

void CGMEOLEIt::CheckConstraints() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::CheckConstraints()\r\n"));

	PRECONDITION_VALID_MODEL
	PRECONDITION_ACTIVE_CONSTMGR

	m_theView->OnFileCheck();
}

void CGMEOLEIt::RunComponent(LPCTSTR appID) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::RunComponent()\r\n"));

	PRECONDITION_VALID_MODEL

	m_theView->RunComponent(appID);
}

void CGMEOLEIt::RunComponentDialog() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::RunComponentDialog()\r\n"));

	PRECONDITION_VALID_MODEL

	m_theView->RunComponent(_T(""));
}

LPDISPATCH CGMEOLEIt::GetAspects() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::GetAspects()\r\n"));

	PRECONDITION_VALID_MODEL

	CGMEOLEColl* coll = new CGMEOLEColl();

	POSITION pos = m_theView->guiMeta->aspects.GetHeadPosition();
	while (pos) {
		CGuiMetaAspect* guiaspect = (CGuiMetaAspect*)(m_theView->guiMeta->aspects.GetNext(pos));
		ASSERT(guiaspect);
		CGMEOLEAspect* aspect = new CGMEOLEAspect();
		aspect->SetGuiParams(m_theView, guiaspect);
		coll->Add(aspect->GetIDispatch(FALSE));
		aspect->GetIDispatch(FALSE)->Release();	// We do not hold reference, hopefully the collection will hold one
	}

	return coll->GetIDispatch(FALSE);
}

void CGMEOLEIt::SetAspects(LPDISPATCH) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::SetAspects()\r\n"));

	SetNotSupported();
}

void CGMEOLEIt::Close() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Close()\r\n"));

	PRECONDITION_VALID_MODEL;

	m_theView->GetParentFrame()->DestroyWindow();
}

void CGMEOLEIt::GrayOutFCO(BOOL bGray, BOOL bNeighbours, LPDISPATCH mgaFCOs) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::GrayOutFCO()\r\n"));

	PRECONDITION_VALID_MODEL;

	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(5);

	CComPtr<IDispatch> alienFCOs(mgaFCOs);
	CComPtr<IMgaFCOs> mFCOColl;
	if (!SUCCEEDED(alienFCOs.QueryInterface(&mFCOColl))) {
		return;
	}

	long l = 0;
	COMTHROW( mFCOColl->get_Count( &l ));
	
	for( long i = 1; i <= l; ++i)
	{
		CComPtr<IMgaFCO> item;
		COMTHROW( mFCOColl->get_Item( i, &item));

		// collection is coming from an unknown territory, do not use it as-is. (except for this search)
		CGuiObject *gObj = CGuiObject::FindObject(item, m_theView->children);
		CGuiConnection *gConn = CGuiConnection::FindConnection(item, m_theView->connections);

		if (gObj) {
			gObj->GrayOut(bGray == TRUE);
			if (bNeighbours == TRUE) {
				gObj->GrayOutNeighbors();
			}
			CGuiFco::GrayOutNonInternalConnections(m_theView->connections);
		}
		else if (gConn) {
			gConn->GrayOut(bGray == TRUE);
			gConn->GrayOutEndPoints();
			if (bNeighbours == TRUE) {
				CGuiFco::GrayOutNonInternalConnections(m_theView->connections);
			}
		}
	}

	m_theView->Invalidate();
}

void CGMEOLEIt::GrayOutHide() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::GrayOutHide()\r\n"));

	PRECONDITION_VALID_MODEL;

	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(5);

	CGuiFco::GrayOutFcos(m_theView->children, TRUE);
	CGuiFco::GrayOutFcos(m_theView->connections, TRUE);
	m_theView->Invalidate();
}

void CGMEOLEIt::ShowSetMembers(LPDISPATCH mgaFCO) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ShowSetMembers()\r\n"));

	PRECONDITION_VALID_MODEL;
  
	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(3);

	CGuiFco::GrayOutFcos(m_theView->children, TRUE);

	CComPtr<IDispatch> alienFCO(mgaFCO);
	CComPtr<IMgaFCO> mFCO;
	if (!SUCCEEDED(alienFCO.QueryInterface(&mFCO))) 
		return;

	// mgaFCO is comming from an unknown territory, do not use it as-is. (except for this search)
	CGuiObject *gObj = CGuiObject::FindObject(mFCO, m_theView->children);
	if (gObj) 
	{
		gObj->GrayOut(false);

		// members of the set too 
		CComPtr<IMgaSet> mSet;
		if (!SUCCEEDED(mFCO.QueryInterface(&mSet))) 
			return;
		m_theView->BeginTransaction();

		try
		{
			CComPtr<IMgaFCOs> mSetMembers = NULL;
			COMTHROW(mSet->get_Members(&mSetMembers));

			long num = 0;
			COMTHROW(mSetMembers->get_Count(&num));
			for (int i=1; i<=num; i++)
			{
				CComPtr<IMgaFCO> memb = NULL;
				COMTHROW(mSetMembers->get_Item(i, &memb));
				CGuiObject *gObj = CGuiObject::FindObject(memb, m_theView->children);
				if (gObj) 
					gObj->GrayOut(false);
			}
			m_theView->CommitTransaction();
		}
		catch(hresult_exception &e) 
		{
			m_theView->AbortTransaction(e.hr);
		}
	}

	m_theView->Invalidate();
}

void CGMEOLEIt::HideSetMembers() 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::HideSetMembers()\r\n"));

	PRECONDITION_VALID_MODEL;
	 
	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(3);

	CGuiFco::GrayOutFcos(m_theView->children, TRUE);
	m_theView->Invalidate();
}

void CGMEOLEIt::Zoom(long percent) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Zoom()\r\n"));

	PRECONDITION_VALID_MODEL;

	m_theView->ZoomPercent(percent);
	m_theView->Invalidate();
}

void CGMEOLEIt::ZoomTo(LPDISPATCH mgaFCOs) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ZoomTo()\r\n"));

	PRECONDITION_VALID_MODEL;
 
	if (CGMEDoc::theInstance) 
		CGMEDoc::theInstance->SetMode(4);

	CComPtr<IDispatch> disp(mgaFCOs);
	CComPtr<IMgaFCOs> fcos;
	if (!SUCCEEDED(disp.QueryInterface(&fcos))) 
		return;

	// calculate the total size of them
	CRect totalr(0,0,0,0);
	m_theView->BeginTransaction();
	try
	{
		long num = 0;
		COMTHROW(fcos->get_Count(&num));
		for (int i=1; i<=num; i++)
		{
			CComPtr<IMgaFCO> memb = NULL;
			COMTHROW(fcos->get_Item(i, &memb));
			// size ??
			CGuiObject *gObj = CGuiObject::FindObject(memb, m_theView->children);
			if (gObj) 
			{
				CRect rec = gObj->GetLocation();
				totalr.UnionRect(totalr, rec);
			}
		}
		m_theView->CommitTransaction();
	}
	catch(hresult_exception &e) 
	{
		m_theView->AbortTransaction(e.hr);
	}

	m_theView->ZoomToFCOs(totalr);
	m_theView->Invalidate();

}

void CGMEOLEIt::Scroll(long bar, long scroll) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Scroll()\r\n"));

	PRECONDITION_VALID_MODEL;

	UINT code = SB_LINEDOWN;
	switch (scroll)
	{
	case 0:
		code = SB_LINEUP;
		break;
	case 1: 
		code = SB_LINEDOWN;
		break;
	case 2:
		code = SB_PAGEUP;
		break;
	case 3: 
		code = SB_PAGEDOWN;
		break;
	case 4:
		code = SB_TOP;
		break;
	case 5:
		code = SB_BOTTOM;
		break;
	}

	if (bar == 0)
		m_theView->OnHScroll(code, 1, NULL);
	else
		m_theView->OnVScroll(code, 1, NULL);

	m_theView->Invalidate();
}

void CGMEOLEIt::DumpModelGeometryXML(LPCTSTR filePath) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::DumpModelGeometryXML()\r\n"));

	PRECONDITION_VALID_MODEL

	HRESULT hr = m_theView->DumpModelGeometryXML(filePath);
	if (FAILED(hr))
	{
		_bstr_t err;
		GetErrorInfo(err.GetAddress());
		if (err.length())
			AfxThrowOleDispatchException(201, err);
	}
}

//static
CComPtr<IMgaObject> CGMEOLEIt::myParent( const CComPtr<IMgaObject>& obj)
{
	if( !obj) return 0;

	CComPtr<IMgaModel> parent_m;
	CComPtr<IMgaFolder> parent_f;

	CComQIPtr<IMgaFCO> fco( obj);
	if( fco)
	{
		COMTHROW( fco->get_ParentModel( &parent_m));
		if ( !parent_m)
			COMTHROW( fco->get_ParentFolder( &parent_f));
	}else
	{
		CComQIPtr<IMgaFolder> fol( obj);
		if( !fol) 
			return 0;
		COMTHROW( fol->get_ParentFolder( &parent_f));
	}

	return parent_m ? CComPtr<IMgaObject>(parent_m) : CComPtr<IMgaObject>(parent_f);
}

//static
CComPtr<IMgaFCO> CGMEOLEIt::fcoByPath( const CComPtr<IMgaModel>& mod, LPCTSTR p_objPath)
{
// can handle absolute paths: those which start with '/'  ie.> '/MainModel1'
// can handle actual model reference with './'            ie.> './sybling'
// can step up one step in hierarchy with '..'            ie.> '../../fco1'
	CComPtr<IMgaObject> newparent = mod;

	LPCTSTR path_ptr = p_objPath;
	if( *path_ptr == '/') // absolute path used, starting from rootfolder
	{
		++path_ptr;
		CComPtr<IMgaObject> t_par; // to store the last valid value
		while( newparent) // go up in the hierarchy until rootfolder
		{
			t_par = newparent;
			newparent = myParent( newparent);
		}
		newparent = t_par; // the last non null value is the rootfolder
	}


	std::wstring path( path_ptr);
	if( path.compare( _T("./")) == 0) return CComPtr<IMgaFCO>( mod);

	bool went_up = false;
	bool null_ptr_found = newparent == 0;
	while( !null_ptr_found && !path.empty() &&
		   ( path.compare(_T("..")) == 0 
		   || path.substr(0, 3).compare( _T("../")) == 0))
	{
		newparent = myParent( newparent);
		null_ptr_found = newparent == 0;
		went_up = true;

		if( path.length() > 2) path = path.substr( 3); // passing one directory : '../'
		else path = _T("");
	}

	if( null_ptr_found) // rootfolder reached => invalid path
		return 0;
	
	if( path.empty() && went_up ) // thus allow '../../' as input, but if did not go up, then interpret the '' as empty child name
		return CComQIPtr<IMgaFCO>( newparent);

	CComBSTR b_path( path.c_str());
	CComPtr<IMgaObject> obj_ptr;
	newparent->get_ObjectByPath( b_path, &obj_ptr);

	return CComQIPtr<IMgaFCO>( obj_ptr);
}

void CGMEOLEIt::ShowFCO( LPCTSTR p_objName, BOOL p_inParent)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ShowFCO\r\n"));

	PRECONDITION_VALID_MODEL			// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> obj_ptr;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		obj_ptr = fcoByPath( model, p_objName);
	}
	CATCHALL_AND_CALL_END;

	if( obj_ptr && CMainFrame::theInstance != NULL )
	{
		CMainFrame::theInstance->mGmeOleApp->ShowFCO( obj_ptr, p_inParent);
	}
}

void CGMEOLEIt::ShowFCOPtr( LPDISPATCH p_obj, BOOL p_inParent)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ShowFCOPtr\r\n"));

	PRECONDITION_VALID_MODEL			// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComQIPtr<IMgaFCO> obj_ptr( p_obj);

	if( obj_ptr && CMainFrame::theInstance != NULL )
	{
		CMainFrame::theInstance->mGmeOleApp->ShowFCO( obj_ptr, p_inParent);
	}
}

LPDISPATCH CGMEOLEIt::Child( LPCTSTR p_objName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Child\r\n"));

	PRECONDITION_VALID_MODEL			// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> obj_ptr;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));
		obj_ptr = getChildInByName( model, p_objName);
	}
	CATCHALL_AND_CALL_END;
	return obj_ptr.Detach();
}


LPDISPATCH CGMEOLEIt::Create( LPCTSTR p_partName, LPCTSTR p_objname)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Create\r\n"));

	PRECONDITION_VALID_MODEL			// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> obj_ptr;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));
		createObjs( model, p_partName, p_objname, &obj_ptr);
	}
	CATCHALL_AND_CALL_END;
	return obj_ptr.Detach();
}

LPDISPATCH CGMEOLEIt::CreateInChild( LPCTSTR p_childAsParent, LPCTSTR p_part, LPCTSTR p_objname)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::CreateInChild\r\n"));

	PRECONDITION_VALID_MODEL			// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> obj_ptr;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaModel> child( getChildInByName( model, p_childAsParent));
		THROW_IF_NULL( child, _T("Invalid child model specified"));

		createObjs( child, p_part, p_objname, &obj_ptr);
	}
	CATCHALL_AND_CALL_END;
	return obj_ptr.Detach();
}

LPDISPATCH CGMEOLEIt::CreateInChildFCO( LPDISPATCH p_childAsParent, LPCTSTR p_part, LPCTSTR p_objname)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Create\r\n"));

	PRECONDITION_VALID_MODEL			// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> obj_ptr;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaModel> child( putInTerr( p_childAsParent));
		if( !child || !isValidChildIn( model.p, child.p))
			THROW_IF_NULL( 0, _T("Invalid child model specified"));

		createObjs( child, p_part, p_objname, &obj_ptr);
	}
	CATCHALL_AND_CALL_END;
	return obj_ptr.Detach();
}


LPDISPATCH CGMEOLEIt::Duplicate( LPCTSTR p_existingObjName, LPCTSTR p_objName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Duplicate\r\n"));

	PRECONDITION_VALID_MODEL			// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> obj_ptr;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));
		CComPtr<IMgaFCO> exist_obj( getChildInByName( model, p_existingObjName));
		THROW_IF_NULL( exist_obj, _T("Duplicable object not found"));
		cloneObjs( model, exist_obj, _T("") , p_objName, &obj_ptr);
	}
	CATCHALL_AND_CALL_END;
	return obj_ptr.Detach();
}

LPDISPATCH CGMEOLEIt::DuplicateFCO( LPDISPATCH p_existingFCO, LPCTSTR p_objName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::DuplicateFCO\r\n"));

	PRECONDITION_VALID_MODEL			// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> obj_ptr;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));
		CComQIPtr<IMgaFCO> exist_fco( putInTerr( p_existingFCO));
		THROW_IF_NULL( exist_fco, _T("Invalid object pointer"));

		cloneObjs( model, exist_fco, _T(""), p_objName, &obj_ptr);
	}
	CATCHALL_AND_CALL_END;

	return obj_ptr.Detach();
}

void CGMEOLEIt::Rename( LPCTSTR p_oldName, LPCTSTR p_newName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Rename\r\n"));

	PRECONDITION_VALID_MODEL			// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));
		CComPtr<IMgaFCO> child( getChildInByName( model, p_oldName));
		THROW_IF_NULL( child, _T("Object not found"));

		rename( child, p_newName);
	}
	CATCHALL_AND_CALL_END;
}

void CGMEOLEIt::SetName( LPDISPATCH p_lpDisp, LPCTSTR p_name)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::SetName\r\n"));

	PRECONDITION_VALID_MODEL			// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaFCO> child( putInTerr( p_lpDisp));
		THROW_IF_NULL( child, _T("Invalid object pointer"));

		rename( child, p_name);
	}
	CATCHALL_AND_CALL_END;
}

void CGMEOLEIt::Include( LPCTSTR psetnm, LPCTSTR pfconm)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Include\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));
		CComQIPtr<IMgaSet> set( getChildInByName( model, psetnm));
		THROW_IF_NULL( set, _T("Set not found"));
		CComPtr<IMgaFCO> fco( getChildInByName( model, pfconm));
		THROW_IF_NULL( fco, _T("Member Fco not found"));

		COMTHROW( set->AddMember( fco ));
	}
	CATCHALL_AND_CALL_END;
}

void CGMEOLEIt::IncludeFCO( LPDISPATCH pset, LPDISPATCH pfco)
{ 
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::IncludeFCO\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));
		CComQIPtr<IMgaSet> set( putInTerr( pset));
		THROW_IF_NULL( set, _T("Invalid set pointer"));
		CComQIPtr<IMgaFCO> fco( putInTerr( pfco));
		THROW_IF_NULL( fco, _T("Invalid Member Fco pointer"));

		// in case of sets we require that the set and fco must be in the same model:
		if( isValidChildIn( model.p, set.p) && isValidChildIn( model.p, fco.p))
			COMTHROW( set->AddMember( fco ));
	}
	CATCHALL_AND_CALL_END;
}
void CGMEOLEIt::Exclude( LPCTSTR psetnm, LPCTSTR pfconm)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Exclude\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaSet> set( getChildInByName( model, psetnm));
		THROW_IF_NULL( set, _T("Set not found"));
		CComPtr<IMgaFCO> fco( getChildInByName( model, pfconm));
		THROW_IF_NULL( fco, _T("Member Fco not found"));

		COMTHROW( set->RemoveMember( fco ));
	}
	CATCHALL_AND_CALL_END;
}
void CGMEOLEIt::ExcludeFCO( LPDISPATCH pset, LPDISPATCH pfco)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ExcludeFCO\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaSet> set( putInTerr( pset));
		THROW_IF_NULL( set, _T("Invalid Set pointer"));
		CComQIPtr<IMgaFCO> fco( putInTerr( pfco));
		THROW_IF_NULL( fco, _T("Invalid Member Fco pointer"));

		// in case of sets we require that the set and fco must be in the same model:
		if( isValidChildIn( model.p, set.p) && isValidChildIn( model.p, fco.p))
			COMTHROW( set->RemoveMember( fco ));
	}
	CATCHALL_AND_CALL_END;
}

LPDISPATCH CGMEOLEIt::Connect( LPCTSTR p_srcName, LPCTSTR p_dstName, LPCTSTR p_connName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Connect\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> new_conn;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));
		CComPtr<IMgaFCO> end1( getChildInByName( model, p_srcName));
		THROW_IF_NULL( end1, _T("Source object not found"));
		CComPtr<IMgaFCO> end2( getChildInByName( model, p_dstName));
		THROW_IF_NULL( end2, _T("Destination object not found"));

		CComPtr<IMgaMetaRole> metarole;
		CComBSTR conn_name( p_connName);
		try
		{
			if( conn_name != _T(""))
				metarole = metaRolePtrInByName( model, conn_name);
		}catch(...) 
		{ 
			THROW_IF_BOOL( true, _T("No such connection role found"));
		}

		if( !metarole)
			metarole = connMetaRolePtrInBetween( model, end1, end2 ); // create the only possible connection if 1 exists

		THROW_IF_NULL( metarole, _T("No possible connection found"));
		COMTHROW( model->CreateSimpleConn( metarole, end1, end2, 0, 0, &new_conn));
	}
	CATCHALL_AND_CALL_END;

	return new_conn.Detach();
}

void CGMEOLEIt::Disconnect( LPCTSTR p_srcName, LPCTSTR p_dstName, LPCTSTR p_connName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Disconnect\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	bool found_any = false;
	start();
	try
	{
		CComBSTR src_role( L"src");
		CComBSTR dst_role( L"dst");

		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComPtr<IMgaFCO> end1( getChildInByName( model, p_srcName));
		THROW_IF_NULL( end1, _T("Source object not found"));
		CComPtr<IMgaFCO> end2( getChildInByName( model, p_dstName));
		THROW_IF_NULL( end2, _T("Destination object not found"));
		// will look for a connection between end1 and end2

		CComObjPtrVector<IMgaFCO> children;
		COMTHROW( model->get_ChildFCOs( PutOut( children )));

		CComObjPtrVector<IMgaFCO>::iterator ch_it = children.begin();
		CComObjPtrVector<IMgaFCO>::iterator ch_end = children.end();
		for( ; ch_it != ch_end; ++ch_it)
		{
			if( myTypeIs( CComPtr<IMgaFCO>( (*ch_it).p)) == OBJTYPE_CONNECTION )
			{
				CComQIPtr<IMgaConnection> conn( *ch_it);
				CComObjPtrVector<IMgaConnPoint> cps;
				COMTHROW( conn->get_ConnPoints( PutOut( cps)));

				bool src_found = false;
				bool dst_found = false;

				CComObjPtrVector<IMgaConnPoint>::iterator cp_it = cps.begin();
				CComObjPtrVector<IMgaConnPoint>::iterator cp_end = cps.end();
				for( ; cp_it != cp_end; ++cp_it)
				{
					CComPtr<IMgaFCO> tg;
					COMTHROW( (*cp_it)->get_Target( &tg));

					CComBSTR connrole;
					COMTHROW( (*cp_it)->get_ConnRole( &connrole));

					if( connrole == src_role && tg == end1)
					{
						src_found = true;
					}
					else if( connrole == dst_role && tg == end2)
					{
						dst_found = true;
					}
				}
				bool name_checked = true; // name or kind could be checked. The Disconnect uses: NAME
				CComBSTR conn_name( p_connName);
				if( conn_name != _T("")) // if name provided
				{
					name_checked = false;

					CComBSTR nm;
					COMTHROW( conn->get_Name( &nm));

					if( nm == conn_name)
						name_checked = true;

				}

				if( src_found && dst_found && name_checked) // regarding src and dst it is ok
				{
					COMTHROW( (*ch_it)->DestroyObject());
					found_any = true;
				}
			}
		}
	}
	CATCHALL_AND_CALL_END;

	if( !found_any)
		SHOW_ERROR( _T("No connection found"));
}

LPDISPATCH CGMEOLEIt::ConnectThruPort( LPCTSTR p_srcRole1, LPCTSTR p_srcRole2, LPCTSTR p_dstRole1, LPCTSTR p_dstRole2, LPCTSTR p_connName) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ConnectThruPort\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> new_conn;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComPtr<IMgaFCO> src1( getChildInByName( model, p_srcRole1));
		THROW_IF_NULL( src1, _T("SourceRole1 object not found"));
		CComPtr<IMgaFCO> dst1( getChildInByName( model, p_dstRole1));
		THROW_IF_NULL( dst1, _T("DestinationRole1 object not found"));
		CComPtr<IMgaFCO> sport;
		CComPtr<IMgaFCO> dport;
		CComPtr<IMgaFCOs> srefs;
		CComPtr<IMgaFCOs> drefs;
		
		objtype_enum type1_info = myTypeIs( src1);
		objtype_enum type2_info = myTypeIs( dst1);

		if( type1_info == OBJTYPE_MODEL && CComBSTR( p_srcRole2) != _T(""))
		{
			// for models only:
			sport = getChildInByName( CComQIPtr<IMgaModel>(src1), p_srcRole2);
		}
		else if( type1_info == OBJTYPE_REFERENCE && CComBSTR( p_srcRole2) != _T(""))
		{
			refPortFinderAndChainBuilder( p_srcRole2, src1, sport, srefs );
		}

		if( type2_info == OBJTYPE_MODEL && CComBSTR( p_dstRole2) != _T(""))
		{
			dport = getChildInByName( CComQIPtr<IMgaModel>(dst1), p_dstRole2);
		}
		else if( type2_info == OBJTYPE_REFERENCE && CComBSTR( p_dstRole2) != _T(""))
		{
			refPortFinderAndChainBuilder( p_dstRole2, dst1, dport, drefs);
		}

		bool error;
		error = CComBSTR( p_srcRole2) != _T("") && !sport;  // if sport == NULL and the p_srcRole2 != NULL then error state
		THROW_IF_BOOL( error, _T("SourceRole2 object not found"));
		error = CComBSTR( p_dstRole2) != _T("") && !dport;       // if dport == NULL and the p_dstRole2 != NULL then error state
		THROW_IF_BOOL( error, _T("DestinationRole2 object not found"));

		CComPtr<IMgaMetaRole> metarole;
		CComBSTR conn_name( p_connName);
		try
		{
			if( conn_name != _T(""))
				metarole = metaRolePtrInByName( model, conn_name);
		}catch(...)
		{
			THROW_IF_BOOL( true, _T("No such connection role found"));
		}

		if( !metarole)
			metarole = connMetaRolePtrInBetween( model, src1, sport, dst1, dport ); // create the only possible connection if 1 exists
		
		THROW_IF_NULL( metarole, _T("No possible connection found"));

		// will create a connection inside the model
		// between
		// src1 sport - dst1 dport
		// src1 sport - dst1
		// src1       - dst1 dport
		// src1       - dst1
		// 

		if( sport && dport)
			COMTHROW( model->CreateSimpleConn( metarole, sport, dport, srefs, drefs, &new_conn));
		else if( sport && dst1)
			COMTHROW( model->CreateSimpleConn( metarole, sport, dst1, srefs, drefs, &new_conn));
		else if( dport && src1)
			COMTHROW( model->CreateSimpleConn( metarole, src1, dport, srefs, drefs, &new_conn));
		else if( src1 && dst1 )
			COMTHROW( model->CreateSimpleConn( metarole, src1, dst1, srefs, drefs, &new_conn));
		else
			THROW_IF_BOOL( true, _T("Unexpected case"));
	}
	CATCHALL_AND_CALL_END;

	return new_conn.Detach();
}

LPDISPATCH CGMEOLEIt::ConnectThruPortFCO( LPDISPATCH p_src1, LPDISPATCH p_src2Port, LPDISPATCH p_dst1, LPDISPATCH p_dst2Port, LPCTSTR p_connName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ConnectThruPortFCO\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> new_conn;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		//CComPtr<IMgaFCO> src1( getChildInByName( model, p_srcRole1));
		CComQIPtr<IMgaFCO> src1( putInTerr( p_src1));
		THROW_IF_NULL( src1, _T("SourceRole1 object not found"));
		//CComPtr<IMgaFCO> dst1( getChildInByName( model, p_dstRole1));
		CComQIPtr<IMgaFCO> dst1( putInTerr( p_dst1));
		THROW_IF_NULL( dst1, _T("DestinationRole1 object not found"));

		if( !isValidChildIn( model.p, src1.p))
			THROW_IF_NULL( 0, _T("SourceRole1 object not found in active model"));
		if( !isValidChildIn( model.p, dst1.p))
			THROW_IF_NULL( 0, _T("DestinationRole1 object not found in active model"));

		objtype_enum type1_info = myTypeIs( src1);
		objtype_enum type2_info = myTypeIs( dst1);

		CComPtr<IMgaFCOs> srefs;
		CComPtr<IMgaFCOs> drefs;
		
		CComQIPtr<IMgaFCO> sport( putInTerr( p_src2Port));
		CComQIPtr<IMgaFCO> dport( putInTerr( p_dst2Port));
		if( sport) // non-zero means really port is needed to be connected
		{
			if( type1_info == OBJTYPE_MODEL)
			{
				CComQIPtr<IMgaModel> s_m( src1);
				bool valid = s_m && isValidChildIn( s_m.p, sport.p);
				THROW_IF_NULL( valid, _T("Source port object not found in source model specified"));
			}
			else if( type1_info == OBJTYPE_REFERENCE)
			{
				bool succ = refChainBuilder2( src1, sport.p, srefs);
				THROW_IF_NULL( succ, _T("Source port object not found inside the target of the specified reference"));
			}
		}

		if( dport) // non-zero means really port is needed to be connected
		{
			if( type2_info == OBJTYPE_MODEL)
			{
				CComQIPtr<IMgaModel> d_m( dst1);
				bool valid = d_m && isValidChildIn( d_m.p, dport.p);
				THROW_IF_NULL( valid, _T("Destination port object not found in destination model specified"));
			}
			else if( type2_info == OBJTYPE_REFERENCE)
			{
				bool succ = refChainBuilder2( dst1.p, dport.p, drefs);
				THROW_IF_NULL( succ, _T("Destination port object not found inside the target of the specified reference"));
			}
		}

		bool error;
		error = p_src2Port && !sport;  // if sport == NULL and the p_src2Port != NULL then error state
		THROW_IF_BOOL( error, _T("SourceRole2 object not found"));
		error = p_dst2Port && !dport;  // if dport == NULL and the p_dst2Port != NULL then error state
		THROW_IF_BOOL( error, _T("DestinationRole2 object not found"));

		CComPtr<IMgaMetaRole> metarole;
		CComBSTR conn_name( p_connName);
		try
		{
			if( conn_name != _T(""))
				metarole = metaRolePtrInByName( model, conn_name);
		}catch(...)
		{
			THROW_IF_BOOL( true, _T("No such connection role found"));
		}

		if( !metarole)
			metarole = connMetaRolePtrInBetween( model, src1, sport, dst1, dport ); // create the only possible connection if 1 exists
		
		THROW_IF_NULL( metarole, _T("No possible connection found"));

		// will create a connection inside the model
		// between
		// src1 sport - dst1 dport
		// src1 sport - dst1
		// src1       - dst1 dport
		// src1       - dst1
		// 

		if( sport && dport)
			COMTHROW( model->CreateSimpleConn( metarole, sport, dport, srefs, drefs, &new_conn));
		else if( sport && dst1)
			COMTHROW( model->CreateSimpleConn( metarole, sport, dst1, srefs, drefs, &new_conn));
		else if( dport && src1)
			COMTHROW( model->CreateSimpleConn( metarole, src1, dport, srefs, drefs, &new_conn));
		else if( src1 && dst1 )
			COMTHROW( model->CreateSimpleConn( metarole, src1, dst1, srefs, drefs, &new_conn));
		else
			THROW_IF_BOOL( true, _T("Unexpected case"));
	}
	CATCHALL_AND_CALL_END;

	return new_conn.Detach();
}

void CGMEOLEIt::DisconnectThruPort( LPCTSTR p_srcRole1, LPCTSTR p_srcRole2, LPCTSTR p_dstRole1, LPCTSTR p_dstRole2, LPCTSTR p_connName)
{
	CGMEEventLogger::LogGMEEvent("CGMEOLEIt::DisconnectThruPort\r\n");

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	bool found_any = false;
	start();
	try
	{
		CComBSTR src_role( L"src");
		CComBSTR dst_role( L"dst");

		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));
		
		CComPtr<IMgaFCO> src1( getChildInByName( model, p_srcRole1));
		THROW_IF_NULL( src1, _T("SourceRole1 object not found"));
		CComPtr<IMgaFCO> dst1( getChildInByName( model, p_dstRole1));
		THROW_IF_NULL( dst1, _T("DestinationRole1 object not found"));
		CComPtr<IMgaFCO> sport;
		CComPtr<IMgaFCO> dport;
		CComPtr<IMgaFCOs> srefs;
		CComPtr<IMgaFCOs> drefs;
		
		objtype_enum type1_info = myTypeIs( src1);
		objtype_enum type2_info = myTypeIs( dst1);

		if( type1_info == OBJTYPE_MODEL)
		{
			// for models only: sport is the port inside with the given name
			sport = getChildInByName( CComQIPtr<IMgaModel>(src1), p_srcRole2);
		}
		else if( type1_info == OBJTYPE_REFERENCE)
		{
			refPortFinderAndChainBuilder( p_srcRole2, src1, sport, srefs );//sport will be filled if found
		}

		if( type2_info == OBJTYPE_MODEL)
		{
			dport = getChildInByName( CComQIPtr<IMgaModel>(dst1), p_dstRole2);
		}
		else if( type2_info == OBJTYPE_REFERENCE)
		{
			refPortFinderAndChainBuilder( p_dstRole2, dst1, dport, drefs);
		}

		bool error;
		error = CComBSTR( p_srcRole2) != _T("") && !sport;
		THROW_IF_BOOL( error, _T("SourceRole2 object not found"));
		error = CComBSTR( p_dstRole2) != _T("") && !dport;
		THROW_IF_BOOL( error, _T("DestinationRole2 object not found"));
		// will look for a connection inside model
		// between 
		// src1 sport - dst1 dport
		// src1 sport - dst1
		// src1       - dst1 dport
		// src1       - dst1
		// 

		CComObjPtrVector<IMgaFCO> children;
		COMTHROW( model->get_ChildFCOs( PutOut( children )));

		CComObjPtrVector<IMgaFCO>::iterator ch_it = children.begin();
		CComObjPtrVector<IMgaFCO>::iterator ch_end = children.end();
		for( ; ch_it != ch_end; ++ch_it)
		{
			if( myTypeIs( CComPtr<IMgaFCO>((*ch_it).p)) == OBJTYPE_CONNECTION )
			{
				CComQIPtr<IMgaConnection> conn( *ch_it);
				CComObjPtrVector<IMgaConnPoint> cps;
				COMTHROW( conn->get_ConnPoints( PutOut( cps)));

				bool src_found = false;
				bool dst_found = false;

				CComObjPtrVector<IMgaConnPoint>::iterator cp_it = cps.begin();
				CComObjPtrVector<IMgaConnPoint>::iterator cp_end = cps.end();
				for( ; cp_it != cp_end; ++cp_it)
				{
					CComPtr<IMgaFCO> tg;
					COMTHROW( (*cp_it)->get_Target( &tg));

					CComBSTR connrole;
					COMTHROW( (*cp_it)->get_ConnRole( &connrole));

					CComPtr<IMgaFCOs> refchain;
					COMTHROW( (*cp_it)->get_References( &refchain));

					if( connrole == src_role)
					{
						if( sport && tg == sport && areCollectionsEqual( refchain, srefs))
							src_found = true;
						else if( src1 && tg == src1)
							src_found = true;
					}
					else if( connrole == dst_role)
					{
						if( dport && tg == dport && areCollectionsEqual( refchain, drefs))
							dst_found = true;
						else if( dst1 && tg == dst1)
							dst_found = true;
					}
				}
				bool name_checked = true;
				CComBSTR conn_name( p_connName);
				if( conn_name != _T("")) // if name provided
				{
					name_checked = false;

					CComBSTR nm;
					COMTHROW( conn->get_Name( &nm));

					if( nm == conn_name)
						name_checked = true;

				}

				if( src_found && dst_found && name_checked) // regarding src and dst it is ok
				{
					COMTHROW( (*ch_it)->DestroyObject());
					found_any = true;
				}
			}
		}
	}
	CATCHALL_AND_CALL_END;

	if( !found_any)
		SHOW_ERROR( _T("No connection found"));
}

LPDISPATCH CGMEOLEIt::ConnectFCOs( LPDISPATCH p_src, LPDISPATCH p_dst, LPCTSTR p_connName) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ConnectFCOs\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> new_conn;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaFCO> end1( putInTerr( p_src));
		THROW_IF_NULL( end1, _T("Invalid Source pointer"));
		CComQIPtr<IMgaFCO> end2( putInTerr( p_dst));
		THROW_IF_NULL( end2, _T("Invalid Destination pointer"));

		CComPtr<IMgaMetaRole> metarole;
		CComBSTR conn_name( p_connName);

		try
		{
			if( conn_name != _T(""))
				metarole = metaRolePtrInByName( model, conn_name);
		}catch(...)
		{
			THROW_IF_BOOL( true, _T("No such connection role found"));
		}
		if( !metarole)
			metarole = connMetaRolePtrInBetween( model, end1, end2 ); // create the only possible connection if 1 exists
		
		THROW_IF_NULL( metarole, _T("No possible connection found"));
		COMTHROW( model->CreateSimpleConn( metarole, end1, end2, 0, 0, &new_conn));
	}
	CATCHALL_AND_CALL_END;

	return new_conn.Detach();
}

void CGMEOLEIt::DisconnectFCOs( LPDISPATCH p_src, LPDISPATCH p_dst, LPCTSTR p_connName) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::DisconnectFCOs\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	bool found_any = false;
	start();
	try
	{
		CComBSTR src_role( L"src");
		CComBSTR dst_role( L"dst");

		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaFCO> end1( putInTerr( p_src));
		THROW_IF_NULL( end1, _T("Invalid Source pointer"));
		CComQIPtr<IMgaFCO> end2( putInTerr( p_dst));
		THROW_IF_NULL( end2, _T("Invalid Destination pointer"));
		// will look for a connection between end1 and end2

		CComObjPtrVector<IMgaFCO> children;
		COMTHROW( model->get_ChildFCOs( PutOut( children )));

		CComObjPtrVector<IMgaFCO>::iterator ch_it = children.begin();
		CComObjPtrVector<IMgaFCO>::iterator ch_end = children.end();
		for( ; ch_it != ch_end; ++ch_it)
		{
			if( myTypeIs( CComPtr<IMgaFCO>( (*ch_it).p)) == OBJTYPE_CONNECTION )
			{
				CComQIPtr<IMgaConnection> conn( *ch_it);
				CComObjPtrVector<IMgaConnPoint> cps;
				COMTHROW( conn->get_ConnPoints( PutOut( cps)));

				bool src_found = false;
				bool dst_found = false;

				CComObjPtrVector<IMgaConnPoint>::iterator cp_it = cps.begin();
				CComObjPtrVector<IMgaConnPoint>::iterator cp_end = cps.end();
				for( ; cp_it != cp_end; ++cp_it)
				{
					CComPtr<IMgaFCO> tg;
					COMTHROW( (*cp_it)->get_Target( &tg));

					CComBSTR connrole;
					COMTHROW( (*cp_it)->get_ConnRole( &connrole));

					if( connrole == src_role && tg == end1)
					{
						src_found = true;
					}
					else if( connrole == dst_role && tg == end2)
					{
						dst_found = true;
					}
				}
				bool name_checked = true; // name or kind could be checked. DisconnectFCO uses: NAME
				CComBSTR conn_name( p_connName);
				if( conn_name != _T("")) // if name provided
				{
					name_checked = false;

					CComBSTR nm;
					COMTHROW( conn->get_Name( &nm));

					if( nm == conn_name)
						name_checked = true;

				}

				if( src_found && dst_found && name_checked) // regarding src and dst it is ok
				{
					COMTHROW( (*ch_it)->DestroyObject());
					found_any = true;
				}
			}
		}
	}
	CATCHALL_AND_CALL_END;

	if( !found_any)
		SHOW_ERROR( _T("No connection found"));
}

void CGMEOLEIt::Refer( LPCTSTR prefnm, LPCTSTR pfconm)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Refer\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaReference> ref( getChildInByName( model, prefnm));
		THROW_IF_NULL( ref, _T("Reference object not found"));
		CComPtr<IMgaFCO> fco( getChildInByName( model, pfconm));
		THROW_IF_NULL( fco, _T("To-Be-Referred object not found"));

		COMTHROW( ref->put_Referred( fco ));
	}
	CATCHALL_AND_CALL_END;
}

void CGMEOLEIt::ReferFCO( LPDISPATCH pref, LPDISPATCH pfco)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ReferFCO\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaReference> ref( putInTerr( pref));
		THROW_IF_NULL( ref, _T("Invalid Reference pointer"));
		CComQIPtr<IMgaFCO> fco( putInTerr( pfco));
		THROW_IF_NULL( fco, _T("Invalid To-Be-Referred Fco pointer"));

		// we require that the reference is inside the model, but not the fco ptr
		if( isValidChildIn( model.p, ref.p))
			COMTHROW( ref->put_Referred(fco) );
	}
	CATCHALL_AND_CALL_END;
}

void CGMEOLEIt::ClearRef( LPCTSTR prefnm)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ClearRef\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaReference> ref( getChildInByName( model, prefnm));
		THROW_IF_NULL( ref, _T("Reference object not found"));
		CComPtr<IMgaFCO> emptyfco;
		COMTHROW( ref->put_Referred( emptyfco ));
	}
	CATCHALL_AND_CALL_END;
}

void CGMEOLEIt::ClearRefFCO( LPDISPATCH pref)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::ClearRefFCO\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaReference> ref( putInTerr( pref));
		THROW_IF_NULL( ref, _T("Invalid Reference pointer"));
		CComQIPtr<IMgaFCO> emptyfco;

		// we require that the reference is inside the model, but not the fco ptr
		if( isValidChildIn( model.p, ref.p))
			COMTHROW( ref->put_Referred( emptyfco) );
	}
	CATCHALL_AND_CALL_END;
}

void CGMEOLEIt::FollowRef( LPCTSTR prefnm)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::FollowRef\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> tgt;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaReference> ref( getChildInByName( model, prefnm));
		THROW_IF_NULL( ref, _T("Reference object not found"));

		COMTHROW( ref->get_Referred( &tgt ));

	}
	CATCHALL_AND_CALL_END;

	if( tgt) ShowFCOPtr( tgt, 1);
}

void CGMEOLEIt::FollowRefFCO( LPDISPATCH pref)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::FollowRefFCO\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComQIPtr<IMgaFCO> tgt;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaReference> ref( putInTerr( pref));
		THROW_IF_NULL( ref, _T("Invalid Reference pointer"));
		
		COMTHROW( ref->get_Referred( &tgt) );
	}
	CATCHALL_AND_CALL_END;

	if( tgt) ShowFCOPtr( tgt, 1);
}

LPDISPATCH CGMEOLEIt::NullFCO()
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::NullFCO\r\n"));
	CComPtr<IMgaFCO> nullobj;
	return nullobj.Detach();
}

void CGMEOLEIt::SetAttribute( LPCTSTR p_partName, LPCTSTR p_attrName, VARIANT& p_attrValue)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::SetAttribute\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComPtr<IMgaFCO> fco( getChildInByName( model, p_partName));
		THROW_IF_NULL( fco, _T("Fco object not found"));

		CComBSTR attr_name( p_attrName);
		CComVariant attr_value( p_attrValue);

		COMTHROW( fco->put_AttributeByName( attr_name, attr_value));
	}
	CATCHALL_AND_CALL_END;
}


VARIANT CGMEOLEIt::GetAttribute( LPCTSTR p_partName, LPCTSTR p_attrName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::GetAttribute\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComVariant attr_value;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComPtr<IMgaFCO> fco( getChildInByName( model, p_partName));
		THROW_IF_NULL( fco, _T("Fco object not found"));

		CComBSTR attr_name( p_attrName);

		COMTHROW( fco->get_AttributeByName( attr_name, &attr_value));
	}
	CATCHALL_AND_CALL_END;

	return attr_value;
}

void CGMEOLEIt::SetAttributeFCO( LPDISPATCH p_fco, LPCTSTR p_attrName, VARIANT& p_attrValue)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::SetAttributeFCO\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaFCO> fco( putInTerr( p_fco));
		THROW_IF_NULL( fco, _T("Invalid Fco pointer"));

		CComBSTR attr_name( p_attrName);
		CComVariant attr_value( p_attrValue);

		if( isValidChildIn( CComPtr<IMgaModel>(model.p), fco)) // valid child of model (could be aspect aware)
		{
			COMTHROW( fco->put_AttributeByName( attr_name, attr_value));
		}
	}
	CATCHALL_AND_CALL_END;
}

VARIANT CGMEOLEIt::GetAttributeFCO( LPDISPATCH p_fco, LPCTSTR p_attrName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::GetAttributeFCO\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComVariant attr_value;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaFCO> fco( putInTerr( p_fco));
		THROW_IF_NULL( fco, _T("Invalid Fco pointer"));

		CComBSTR attr_name( p_attrName);

		if( isValidChildIn( CComPtr<IMgaModel>( model.p), fco )) // valid child of model (could be aspect aware)
		{
			COMTHROW( fco->get_AttributeByName( attr_name, &attr_value));
		}
	}
	CATCHALL_AND_CALL_END;

	return attr_value;
}

LPDISPATCH CGMEOLEIt::SubType( LPCTSTR p_baseName, LPCTSTR p_subtypeName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Subtype\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> newobj; 
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComPtr<IMgaFCO> base( getChildInByName( model, p_baseName));
		THROW_IF_NULL( base, _T("Base object not found"));
		
		CComPtr<IMgaMetaRole> metarole;
		COMTHROW( base->get_MetaRole( &metarole));

		COMTHROW( model->DeriveChildObject( base, metarole, VARIANT_FALSE, &newobj));

		THROW_IF_NULL( newobj, _T("Invalid new object pointer"));
		rename( newobj, p_subtypeName);
	}
	CATCHALL_AND_CALL_END;
	
	return newobj.Detach();
}

LPDISPATCH CGMEOLEIt::Instantiate( LPCTSTR p_baseName, LPCTSTR p_instanceName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Instantiate\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> newobj; 
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComPtr<IMgaFCO> base( getChildInByName( model, p_baseName));
		THROW_IF_NULL( base, _T("Base object not found"));

		CComPtr<IMgaMetaRole> metarole;
		COMTHROW( base->get_MetaRole( &metarole));

		COMTHROW( model->DeriveChildObject( base, metarole, VARIANT_TRUE, &newobj));

		THROW_IF_NULL( newobj, _T("Invalid new object pointer"));
		rename( newobj, p_instanceName);
	}
	CATCHALL_AND_CALL_END;

	return newobj.Detach();
}

LPDISPATCH CGMEOLEIt::SubTypeFCO( LPDISPATCH p_base, LPCTSTR p_subtypeName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::SubTypeFCO\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> newobj; 
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaFCO> base( putInTerr( p_base));
		THROW_IF_NULL( base, _T("Invalid Base pointer"));

		//if( isValidChildIn( CComPtr<IMgaModel>( model.p), fco ) // valid child of model (could be aspect aware)
		// it should not be forced to be the child of the model
		CComPtr<IMgaMetaRole> metarole;
		COMTHROW( base->get_MetaRole( &metarole));

		COMTHROW( model->DeriveChildObject( base, metarole, VARIANT_FALSE, &newobj));

		THROW_IF_NULL( newobj, _T("Invalid new object pointer"));
		rename( newobj, p_subtypeName);
	}
	CATCHALL_AND_CALL_END;

	return newobj.Detach();
}

LPDISPATCH CGMEOLEIt::InstantiateFCO( LPDISPATCH p_base, LPCTSTR p_instanceName)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::InstantiateFCO\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCO> newobj;
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComQIPtr<IMgaFCO> base( putInTerr( p_base));
		THROW_IF_NULL( base, _T("Invalid Base pointer"));

		//if( isValidChildIn( CComPtr<IMgaModel>( model.p), base ) 
		// it should not be forced to be the child of the model
		CComPtr<IMgaMetaRole> metarole;
		COMTHROW( base->get_MetaRole( &metarole));

		COMTHROW( model->DeriveChildObject( base, metarole, VARIANT_TRUE, &newobj));

		THROW_IF_NULL( newobj, _T("Invalid new object pointer"));
		rename( newobj, p_instanceName);
	}
	CATCHALL_AND_CALL_END;

	return newobj.Detach();
}


void CGMEOLEIt::BeginTransaction( )
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::BeginTransaction\r\n"));

	beginTrans();
	m_isInUserInitiatedTransaction = true;

}

void CGMEOLEIt::CommitTransaction( )
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::CommitTransaction\r\n"));

	commitTrans();
	m_isInUserInitiatedTransaction = false;
}

void CGMEOLEIt::AbortTransaction( )
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::AbortTransaction\r\n"));

	abortTrans();
	m_isInUserInitiatedTransaction = false;
}

VARIANT_BOOL CGMEOLEIt::IsInTransaction( )
{ 
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::IsInTransaction\r\n"));

	CComPtr<IMgaTerritory> act_terr;
	COMTHROW( theApp.mgaProject->get_ActiveTerritory( &act_terr));

	return (act_terr || m_isInUserInitiatedTransaction)? VARIANT_TRUE : VARIANT_FALSE;
}

LPDISPATCH CGMEOLEIt::PutInTerritory( LPDISPATCH one_obj)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::PutInTerritory\r\n"));

	return putInTerr( one_obj);
}


void CGMEOLEIt::Help(  )
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Help\r\n"));

	static const wchar_t * htmlhelp = 
							L"Scripting HELP<br>\
							Predefined variables available in this scripting environment: &quot;gme&quot;, &quot;project&quot; and &quot;it&quot;.<br>\
							<b>gme</b> implements the IGMEOLEApp interface. i.e.: gme.OpenProject('MGA=c:&#92;myproj.mga') <br>\
							<b>project</b> implements the IMgaProject interface. i.e.: myrootfolder = project.RootFolder<br>\
							<b>it</b> implements the IGMEOLEIt interface and represents the currently opened model window.<br>\
							It can be used for speeding up repetitive tasks, like creating several fcos:<br>\
							e.g.(in Python): for i in range(0,5): it.Create('CompoundParts', 'NewlyCreatedCompound' + str(i)).<br>\
							The methods of 'it' can be used either inside, or outside GME transactions.";

	if( CMainFrame::theInstance != NULL )
	{
		CMainFrame::theInstance->m_console.Message(htmlhelp, 1 );
	}
}

void CGMEOLEIt::SetSelected( LPCTSTR list) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::SetSelected()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CGuiObjectList new_selection; // will store the selected objects
	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));

		CComPtr<IMgaFCOs> coll;
		COMTHROW(coll.CoCreateInstance( L"Mga.MgaFCOs"));
		
		CString tok,m = list;
		int i = 0;
		tok = m.Tokenize( _T(" "), i);
		while( tok != _T(""))
		{
			CComPtr<IMgaFCO> ch;
			CComBSTR nm( (LPCTSTR) tok);
			COMTHROW( model->get_ChildFCO( nm, &ch));
			if( ch)
			{
				COMTHROW( coll->Append( ch ));
			}
			tok = m.Tokenize( _T(" "), i);
		}
		
		long l = 0;
		COMTHROW( coll->get_Count( &l ));
		
		for( long i = 1; i <= l; ++i)
		{
			CComPtr<IMgaFCO> local_item;
			COMTHROW( coll->get_Item( i, &local_item));

			// search for local_item among children and build up the "new_selection"
			bool found = false;
			POSITION pos = m_theView->children.GetHeadPosition();
			while ( pos  && !found)
			{
				CGuiFco* gui_fco = m_theView->children.GetNext( pos);
				if( gui_fco->IsVisible()) // aspect aware
				{
					CComQIPtr<IMgaFCO> mga_fco( putInTerr( gui_fco->mgaFco));

					if( mga_fco && local_item == mga_fco) // must not be null
					{
						found = true;
						CGuiObject* gui_obj = NULL;
						if (gui_fco)
							gui_obj = gui_fco->dynamic_cast_CGuiObject();
						if( gui_obj) new_selection.AddTail( gui_obj);
					}
				}
			}
		}
	}
	CATCHALL_AND_CALL_END;

	if( m_theView)// && ! m_theView->selected.IsEmpty())
	{
		m_theView->selected.RemoveAll();
		m_theView->selected.AddTail( &new_selection);
		m_theView->Invalidate();
	}
}

BSTR CGMEOLEIt::GetSelected( ) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::GetSelected()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComBSTR string_res;
	start();
	try
	{
		CComPtr<IMgaFCOs> coll, col2;
		COMTHROW(col2.CoCreateInstance(OLESTR("Mga.MgaFCOs")));
		POSITION pos = m_theView->selected.GetHeadPosition();
		while(pos) {
			COMTHROW(col2->Append(m_theView->selected.GetNext(pos)->mgaFco));
		}

		//COMTHROW(m_theView->currentModel->AddInternalConnections(col2,&coll)); // copy from coll to coll
		CComQIPtr<IMgaModel> curmodel( putInTerr( m_theView->GetCurrentModel()));
		if( curmodel) COMTHROW( curmodel->AddInternalConnections( col2, &coll));

		long sz;
		COMTHROW( coll->get_Count( &sz));
		if( sz > 0 )
		{
			for( long i = 1; i <= sz; ++i)
			{
				CComPtr<IMgaFCO> fco;
				COMTHROW( coll->get_Item( i, &fco));

				CComQIPtr<IMgaFCO> tfco( putInTerr( fco));
				CComBSTR nm;
				COMTHROW( tfco->get_Name( &nm));
				TRACE( _T("setselectedFCO input collection: %s"), nm);
				if (!string_res.Length() == 0) 
					COMTHROW(string_res.Append(_T(" ")));
				COMTHROW(string_res.AppendBSTR( nm));
			}
		}
	}
	CATCHALL_AND_CALL_END;

	return string_res.Detach();
}

void CGMEOLEIt::SetSelectedFCOs( LPDISPATCH p_dispColl)
{ 
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::SetSelectedFCOs()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CGuiObjectList new_selection; // will store the selected objects
	start();
	try
	{
		CComPtr<IMgaFCOs> coll;
		p_dispColl->QueryInterface(__uuidof(IMgaFCOs), (LPVOID*)&coll);
		
		long l = 0;
		COMTHROW( coll->get_Count( &l ));
		
		for( long i = 1; i <= l; ++i)
		{
			CComPtr<IMgaFCO> item;
			COMTHROW( coll->get_Item( i, &item));
			CComQIPtr<IMgaFCO> local_item( putInTerr( item ));

			// search for local_item among children and build up the "new_selection"
			bool found = false;
			POSITION pos = m_theView->children.GetHeadPosition();
			while ( pos  && !found)
			{
				CGuiFco* gui_fco = m_theView->children.GetNext( pos);
				CComQIPtr<IMgaFCO> mga_fco( putInTerr( gui_fco->mgaFco));

				if( gui_fco->IsVisible()) // aspect aware
				{
					if( mga_fco && local_item == mga_fco) // test non-null
					{
						found = true;
						CGuiObject* gui_obj = NULL;
						if (gui_fco)
							gui_obj = gui_fco->dynamic_cast_CGuiObject();
						if( gui_obj) new_selection.AddTail( gui_obj);
					}
				}
			}
		}
	}
	CATCHALL_AND_CALL_END;

	if( m_theView)
	{
		m_theView->selected.RemoveAll();
		m_theView->selected.AddTail( &new_selection);
		m_theView->Invalidate();
	}
}


LPDISPATCH CGMEOLEIt::GetSelectedFCOs( ) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::GetSelectedFCOs()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	CComPtr<IMgaFCOs> coll;
	start();
	try
	{
		CComPtr<IMgaFCOs> col2;
		COMTHROW(col2.CoCreateInstance(OLESTR("Mga.MgaFCOs")));
		POSITION pos = m_theView->selected.GetHeadPosition();
		while(pos) {
			COMTHROW(col2->Append(m_theView->selected.GetNext(pos)->mgaFco));
		}
		
		//COMTHROW(m_theView->currentModel->AddInternalConnections(col2,&coll)); // copy from coll to coll
		CComQIPtr<IMgaModel> curmodel( putInTerr( m_theView->GetCurrentModel()));
		if( curmodel) COMTHROW( curmodel->AddInternalConnections( col2, &coll));
	}
	CATCHALL_AND_CALL_END;

	return coll.Detach();
}

void CGMEOLEIt::moveAspect( int dir)
{
	CGuiMetaAspect* guiaspect = (CGuiMetaAspect*)m_theView->guiMeta->FindAspect(m_theView->currentAspect->name);
	int ca = guiaspect->index;
	ca += dir;

	if( ca < 0)
		ca = m_theView->guiMeta->NumberOfAspects() - 1;
	
	if( ca >= m_theView->guiMeta->NumberOfAspects())
		ca = 0;

	m_theView->ChangeAspect( ca);

}

CString CGMEOLEIt::GetCurrentAspect(void) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::GetCurrentAspect()\r\n"));

	PRECONDITION_VALID_MODEL

	return m_theView->GetCurrentAspectName();
}

void CGMEOLEIt::SetCurrentAspect(const CString& aspectName) 
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::SetCurrentAspect()\r\n"));

	PRECONDITION_VALID_MODEL

	m_theView->ChangeAspect(aspectName);
}

void CGMEOLEIt::NextAspect( )
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::NextAspect()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	moveAspect( 1 );
}

void CGMEOLEIt::PrevAspect( )
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::PrevAspect()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	moveAspect( -1 );
}

void CGMEOLEIt::moveView( int dir )
{
	POSITION pos = CGMEDoc::theInstance->GetFirstViewPosition();
	POSITION pos_1st = pos;

	CGMEView* first_view = (CGMEView *)CGMEDoc::theInstance->GetNextView(pos_1st);
	CGMEView* new_view = 0;
	CGMEView* prev_view = 0;
	CGMEView* it_view = 0;

	while (pos != NULL && !new_view)
	{
		prev_view = it_view;
		it_view = (CGMEView *)CGMEDoc::theInstance->GetNextView(pos);

		if( it_view == m_theView)
		{
			if( dir == 1) // next
			{
				if( pos != NULL) // if not the last
				{
					new_view = (CGMEView *)CGMEDoc::theInstance->GetNextView(pos);
				}
				else // if last use the first
				{
					new_view = first_view;
				}
			}
			else if( dir == -1) // previous
			{
				if( prev_view) // if not first use the previous
				{
					new_view = prev_view;
				}
				else
				{
					new_view = it_view;
					while (pos != NULL)
						new_view = (CGMEView *)CGMEDoc::theInstance->GetNextView(pos);
					// new_view now is the last one
				}
			}
		}
	}
	ASSERT( new_view);
	if( new_view)
	{
		CMainFrame::theInstance->ActivateView(new_view);//MDIActivate( new_view);
	}
}
void CGMEOLEIt::Next( )
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Next()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	moveView( 1 );
}

void CGMEOLEIt::Prev( )
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Prev()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	moveView( -1 );
}

void CGMEOLEIt::setObjPos( CComPtr<IMgaFCO> pChild, LPCTSTR pAspectName, long parX, long parY)
{
	ASSERT( pChild);
	if( !pChild) return;

	CString path;
	path.Format( _T("PartRegs/%s/Position"), pAspectName);
	CString valu;
	valu.Format( _T("%u,%u"), parX, parY);
	COMTHROW( pChild->put_RegistryValue( PutInBstr(path), PutInBstr(valu)));
}

void CGMEOLEIt::Position( LPCTSTR pObjName, LPCTSTR pAspectName, long parX, long parY)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::Position()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaModel> model( putInTerr( m_theMgaModel));
		THROW_IF_NULL( model, _T("Invalid parent model"));

		CComPtr<IMgaFCO> child( getChildInByName( model, pObjName));
		THROW_IF_NULL( child, _T("Child object not found"));
		
		setObjPos( child, pAspectName, parX, parY);
	}
	CATCHALL_AND_CALL_END;
}

void CGMEOLEIt::PositionFCO( LPDISPATCH pObjPtr, LPCTSTR pAspectName, long parX, long parY)
{
	CGMEEventLogger::LogGMEEvent(_T("CGMEOLEIt::PositionFCO()\r\n"));

	PRECONDITION_VALID_MODEL		// calls AmIValid

	ASSERT( m_theView);
	ASSERT( m_theMgaModel);

	start();
	try
	{
		CComQIPtr<IMgaFCO> child( putInTerr( pObjPtr));
		setObjPos( child, pAspectName, parX, parY);
	}
	CATCHALL_AND_CALL_END;
}

DELEGATE_DUAL_INTERFACE(CGMEOLEIt, Dual)

// Implement ISupportErrorInfo to indicate we support the
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CGMEOLEIt, __uuidof(IGMEOLEIt))


STDMETHODIMP CGMEOLEIt::XDual::get_Aspects(IGMEOLEColl** coll)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->GetAspects();
		lpDisp->QueryInterface(__uuidof(IGMEOLEColl), (LPVOID*)coll);
		return NOERROR;
	}
	CATCH_ALL_DUAL
} 

STDMETHODIMP CGMEOLEIt::XDual::Print()
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Print();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::PrintDialog()
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->PrintDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEIt::XDual::DumpWindowsMetaFile(BSTR filePath)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->DumpWindowsMetaFile(CString(filePath));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEIt::XDual::CheckConstraints()
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->CheckConstraints();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEIt::XDual::RunComponent(BSTR appID)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->RunComponent(CString(appID));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEIt::XDual::RunComponentDialog()
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->RunComponentDialog();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEIt::XDual::Close()
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Close();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEIt::XDual::GrayOutFCO(VARIANT_BOOL bGray, VARIANT_BOOL bNeighbours, IMgaFCOs* mgaFCO)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = NULL;
		mgaFCO->QueryInterface(IID_IDispatch, (LPVOID*)&lpDisp);
		pThis->GrayOutFCO(bGray, bNeighbours, lpDisp);
		lpDisp->Release();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEIt::XDual::GrayOutHide()
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->GrayOutHide();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ShowSetMembers(IMgaFCO* mgaFCO)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = NULL;
		mgaFCO->QueryInterface(IID_IDispatch, (LPVOID*)&lpDisp);
		pThis->ShowSetMembers(lpDisp);
		lpDisp->Release();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEIt::XDual::HideSetMembers()
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->HideSetMembers();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Zoom(DWORD percent)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Zoom(percent);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ZoomTo(IMgaFCOs* mgaFCOs)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->ZoomTo(mgaFCOs);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Scroll(bar_enum bar, scroll_enum scroll)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Scroll(bar, scroll);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEIt::XDual::DumpModelGeometryXML(BSTR filePath)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->DumpModelGeometryXML(CString(filePath));
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CGMEOLEIt::XDual::get_Valid(VARIANT_BOOL* p_isActive)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		*p_isActive = (pThis->GetValid() == FALSE) ? VARIANT_FALSE : VARIANT_TRUE;
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::get_MgaModel(IMgaModel** p_pModel)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->GetMgaModel();
		lpDisp->QueryInterface(__uuidof(IMgaModel), (LPVOID*)p_pModel);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ShowFCO( BSTR directory, VARIANT_BOOL inParent)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->ShowFCO( (LPCTSTR) directory, inParent);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ShowFCOPtr( IMgaFCO* child, VARIANT_BOOL inParent)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->ShowFCOPtr( child, inParent);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Child( BSTR child_name, IMgaFCO** new_fco)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->Child( (LPCTSTR) child_name);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) new_fco);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Create( BSTR part, BSTR name, IMgaFCO** new_fco)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->Create( (LPCTSTR)part, (LPCTSTR)name);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) new_fco);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::CreateInChild( BSTR child_as_parent, BSTR part, BSTR name, IMgaFCO** new_fco)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->CreateInChild( (LPCTSTR) child_as_parent, (LPCTSTR) part, (LPCTSTR) name);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) new_fco);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::CreateInChildFCO( IMgaFCO* child_as_parent, BSTR part, BSTR name, IMgaFCO** new_fco)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->CreateInChildFCO( child_as_parent, (LPCTSTR) part, (LPCTSTR) name);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) new_fco);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Duplicate(THIS_ BSTR existing_fco_name, BSTR new_fco_name, IMgaFCO** new_fco)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->Duplicate( (LPCTSTR)existing_fco_name, (LPCTSTR)new_fco_name);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) new_fco);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::DuplicateFCO(THIS_ IMgaFCO* existing_fco, BSTR new_fco_name, IMgaFCO** new_fco)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->DuplicateFCO( existing_fco, (LPCTSTR)new_fco_name);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) new_fco);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Rename(THIS_ BSTR oldname, BSTR newname)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Rename( (LPCTSTR)oldname, (LPCTSTR)newname);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::SetName(THIS_ IMgaFCO* fco, BSTR name)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->SetName( fco, (LPCTSTR)name);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Include(THIS_ BSTR setname, BSTR fconame)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Include( (LPCTSTR) setname, (LPCTSTR) fconame );
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::IncludeFCO(THIS_ IMgaSet* set, IMgaFCO* fco)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->IncludeFCO( set, fco );
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Exclude(THIS_ BSTR setname, BSTR fconame)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Exclude( (LPCTSTR) setname, (LPCTSTR) fconame );
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ExcludeFCO(THIS_ IMgaSet* set, IMgaFCO* fco)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->ExcludeFCO( set, fco );
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Connect(THIS_ BSTR name1, BSTR name2, BSTR connection, IMgaConnection** conn )
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->Connect( (LPCTSTR) name1, (LPCTSTR) name2, (LPCTSTR) connection );
		lpDisp->QueryInterface(__uuidof(IMgaConnection), (LPVOID*) conn);

	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Disconnect(THIS_  BSTR part1, BSTR part2, BSTR connection)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Disconnect( (LPCTSTR) part1, (LPCTSTR) part2, (LPCTSTR) connection );
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ConnectThruPort(THIS_ BSTR part1, BSTR port1, BSTR part2, BSTR port2, BSTR connection, IMgaConnection ** conn )
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->ConnectThruPort( (LPCTSTR) part1, (LPCTSTR) port1, (LPCTSTR) part2, (LPCTSTR) port2, (LPCTSTR) connection);
		lpDisp->QueryInterface(__uuidof(IMgaConnection), (LPVOID*) conn);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ConnectThruPortFCO(THIS_ IMgaFCO* src_part, IMgaFCO* src_ref, IMgaFCO* dst_part, IMgaFCO* dst_ref, BSTR connection_role, IMgaConnection ** conn)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->ConnectThruPortFCO( src_part, src_ref, dst_part, dst_ref, (LPCTSTR) connection_role);
		lpDisp->QueryInterface(__uuidof(IMgaConnection), (LPVOID*) conn);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::DisconnectThruPort(THIS_ BSTR part1, BSTR port1, BSTR part2, BSTR port2, BSTR connection)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->DisconnectThruPort( (LPCTSTR) part1, (LPCTSTR) port1, (LPCTSTR) part2, (LPCTSTR) port2, (LPCTSTR) connection );
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ConnectFCOs(THIS_  IMgaFCO* part1,  IMgaFCO* part2,  BSTR connection, IMgaConnection ** conn )
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->ConnectFCOs( part1, part2, (LPCTSTR) connection );
		lpDisp->QueryInterface(__uuidof(IMgaConnection), (LPVOID*) conn);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::DisconnectFCOs(THIS_  IMgaFCO* part1,  IMgaFCO* part2,  BSTR connection)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->DisconnectFCOs( part1, part2, (LPCTSTR) connection );
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Refer(THIS_  BSTR reference,  BSTR referred)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Refer( (LPCTSTR) reference, (LPCTSTR) referred );
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ReferFCO(THIS_  IMgaReference* reference,  IMgaFCO* referred)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->ReferFCO( reference, referred );
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ClearRef(THIS_  BSTR reference)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->ClearRef( (LPCTSTR) reference);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::ClearRefFCO(THIS_  IMgaReference* reference)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->ClearRefFCO( reference);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::FollowRef(THIS_  BSTR reference)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->FollowRef( (LPCTSTR) reference);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::FollowRefFCO(THIS_  IMgaReference* reference)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->FollowRefFCO( reference);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::NullFCO(THIS_ IMgaFCO** nullobj)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->NullFCO();
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) nullobj);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::SetAttribute(THIS_ BSTR name,  BSTR attr_name,  VARIANT value)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->SetAttribute( (LPCTSTR) name, (LPCTSTR) attr_name, value );
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::GetAttribute(THIS_ BSTR name,  BSTR attr_name, VARIANT* value)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		CComVariant p = pThis->GetAttribute( (LPCTSTR) name, (LPCTSTR) attr_name );
		*value = p;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::SetAttributeFCO(THIS_ IMgaFCO* fco,  BSTR attr_name,  VARIANT value)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->SetAttributeFCO( fco, (LPCTSTR) attr_name, value);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::GetAttributeFCO(THIS_ IMgaFCO* fco,  BSTR attr_name, VARIANT* value)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		CComVariant p = pThis->GetAttributeFCO( fco, (LPCTSTR) attr_name );
		*value = p;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::SubType(THIS_ BSTR base, BSTR name_of_subtype, IMgaFCO** subtype)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->SubType( (LPCTSTR) base, (LPCTSTR) name_of_subtype);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) subtype);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Instantiate(THIS_ BSTR base, BSTR name_of_instance, IMgaFCO** instance)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->Instantiate( (LPCTSTR) base, (LPCTSTR) name_of_instance);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) instance);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::SubTypeFCO(THIS_ IMgaFCO* base_ptr, BSTR name_of_subtype, IMgaFCO** subtype)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->SubTypeFCO( base_ptr, (LPCTSTR) name_of_subtype);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) subtype);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::InstantiateFCO(THIS_ IMgaFCO* base_ptr, BSTR name_of_instance, IMgaFCO** instance)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->InstantiateFCO( base_ptr, (LPCTSTR) name_of_instance);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) instance);
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::BeginTransaction()
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->BeginTransaction();
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::CommitTransaction()
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->CommitTransaction();
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::AbortTransaction()
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->AbortTransaction();
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::IsInTransaction(THIS_ VARIANT_BOOL* intrans)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		*intrans = pThis->IsInTransaction();
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::PutInTerritory(THIS_ IMgaFCO* fco, IMgaFCO** newfco)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		LPDISPATCH lpDisp = pThis->PutInTerritory( fco);
		lpDisp->QueryInterface(__uuidof(IMgaFCO), (LPVOID*) newfco);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Help(THIS)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Help();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::SetSelected(THIS_ BSTR space_delimited_string )
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->SetSelected( (LPCTSTR) space_delimited_string);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::GetSelected(THIS_ BSTR * space_delimited_string )
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		CComBSTR p = pThis->GetSelected();
		*space_delimited_string = p.Detach();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::SetSelectedFCOs(THIS_ IMgaFCOs* fcos_to_select)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		TRACE( _T("XDual::SetSelectedFCOs"));
		pThis->SetSelectedFCOs( fcos_to_select);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::GetSelectedFCOs(THIS_ IMgaFCOs** selected_fcos)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		TRACE( _T("XDual::GetSelectedFCOs"));
		LPDISPATCH lpDisp = pThis->GetSelectedFCOs();
		lpDisp->QueryInterface(__uuidof(IMgaFCOs), (LPVOID*)selected_fcos);

		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::GetCurrentAspect(THIS_ BSTR* aspectName)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		CComBSTR string_asp = pThis->GetCurrentAspect();
		*aspectName = string_asp.Detach();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::SetCurrentAspect(THIS_ BSTR aspectName)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->SetCurrentAspect(CString(aspectName));
		return S_OK;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::NextAspect(THIS)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->NextAspect();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::PrevAspect(THIS)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->PrevAspect();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Next(THIS)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Next();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Prev(THIS)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Prev();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::Position(THIS_ BSTR pChildName, BSTR pAspectName, long parX, long parY)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->Position( (LPCTSTR) pChildName, (LPCTSTR) pAspectName, parX, parY);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CGMEOLEIt::XDual::PositionFCO(THIS_ IMgaFCO* pChildPtr, BSTR pAspectName, long parX, long parY)
{
	METHOD_PROLOGUE(CGMEOLEIt, Dual)

	TRY_DUAL(__uuidof(IGMEOLEIt))
	{
		pThis->PositionFCO( pChildPtr, (LPCTSTR) pAspectName, parX, parY);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}
