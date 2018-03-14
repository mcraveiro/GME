#include "stdafx.h"
#include "MgaLibRefr.h"
#include "MgaLibOps.h"
#include "MgaComplexOps.h"
#include "MgaConnection.h"

#ifdef max
#undef max
#endif

#define DONT_OPTIM              0
// defined in MgaLibOps.cpp
void steal(CoreObj &o, CoreObj &n, attrid_type ai);

/*static*/
bool SearchTool::m_optimized = false;

short RefreshManager::distance( CoreObj& derd)
{
	short res = 0;
	CoreObj cur = derd;
	bool goon = true;
	while( goon)
	{
		CoreObj nxt = cur[ATTRID_DERIVED];
		if( nxt && nxt.IsFCO())
		{
			++res;
			cur = nxt;
		}
		else
			goon = false;
	}

	return res;
}

void RefreshManager::getElemDetails( CComPtr<IMgaFCO>& peer, elem_struct& elem)
{
	VARIANT_BOOL in_lib;
	COMTHROW( peer->get_IsLibObject( &in_lib));
	bool in_refrd_lib = false; // in the refreshed lib exactly?
	if( in_lib == VARIANT_TRUE)
	{
		BinGuid libgd1, libgd2;
		libgd1 = Identifier::getTopLibRootOf( CoreObj( peer));
		libgd2 = Identifier::getTopLibRootOf( m_oldLib);
		in_refrd_lib = libgd1 == libgd2;
	}

	// if m_oldlib is virtually included there might be dependent elements (like subtypes)
	// in other libraries (a previous container library)
	// from the Refresh's perspective those elements are fixed elements (can be found based on ID)
	// and are not found by the SearchTool methods which look down from a given container element
	// usually the new version of the refreshed library: m_newLib
	// these libraries can be refreshed either with or without the optimized flag (that's why we
	// rely on m_isIncluded rather than on SearchTool::m_optimized

	// if optimized then additional conditions apply
	// if !optimized then in_lib is enough
	if( in_lib == VARIANT_TRUE && !m_isVirtuallyIncluded 
		|| in_refrd_lib        &&  m_isVirtuallyIncluded) // has guid, other things will change
	{
		CoreObj cp( peer);
		
		elem.uid = Identifier::getDetailsOf( cp);
		elem.inverysamelib = true;//elem.inlib = true;
		elem.inanotherlib  = false;
	}
	else // in project, must not have guid
	{
		PROJ_ID id;
		COMTHROW( peer->get_ID( &id));
		elem.id = id;
		elem.inverysamelib = false;//elem.inlib = false;
		elem.inanotherlib  = in_lib == VARIANT_TRUE;
	}
}

void RefreshManager::getElemDetails( CoreObj& peer, elem_struct& elem)
{
	VARIANT_BOOL in_lib;
	COMTHROW( ObjForCore(peer)->get_IsLibObject( &in_lib));
	bool in_refrd_lib = false; // in the refreshed lib exactly?
	if( in_lib == VARIANT_TRUE)
	{
		BinGuid libgd1, libgd2;
		libgd1 = Identifier::getTopLibRootOf( peer);
		libgd2 = Identifier::getTopLibRootOf( m_oldLib);
		in_refrd_lib = libgd1 == libgd2;
	}

	// if optimized then additional conditions apply
	// if !optimized then in_lib is enough
	if( in_lib == VARIANT_TRUE && !m_isVirtuallyIncluded 
		|| in_refrd_lib        &&  m_isVirtuallyIncluded) // has guid, other things will change
	{
		elem.uid = Identifier::getDetailsOf( peer);
		elem.inverysamelib = true;//elem.inlib = true;
		elem.inanotherlib  = false;
	}
	else // in project, must not have guid
	{
		PROJ_ID id;
		COMTHROW( ObjForCore(peer)->get_ID( &id));
		elem.id = id;
		elem.inverysamelib = false;//elem.inlib = false;
		elem.inanotherlib  = in_lib == VARIANT_TRUE;
	}
}

bool RefreshManager::isDerivedRef( CComPtr<IMgaFCO>& p_ref, CComPtr<IMgaFCO>& p_tgt
								  , bool *p_derdFromTheSameLib, bool *p_isRedirected
								  , bool *p_isSecondaryDerd)
{
	bool retv = false;
	CComQIPtr<IMgaReference> ref( p_ref);

	ASSERT(ref);
	if( !ref) return retv;

	ASSERT( p_derdFromTheSameLib);
	ASSERT( p_isRedirected);
	*p_derdFromTheSameLib = false;
	*p_isRedirected       = false;
	*p_isSecondaryDerd    = false;

	CComPtr<IMgaFCO> base;
	ref->get_DerivedFrom( &base); // ignore failure

	if( base)
	{
		VARIANT_BOOL prim;
		COMTHROW( ref->get_IsPrimaryDerived( &prim));
		*p_isSecondaryDerd = prim == VARIANT_FALSE; // secondary derd

		retv = true;
		elem_struct elem_base;
		getElemDetails( base, elem_base);

		//if( elem_base.inlib 
		if( elem_base.inverysamelib // TODO?
		 && Identifier::getTopLibRootOf( CoreObj( p_tgt)) == Identifier::getTopLibRootOf( CoreObj(base)))
			*p_derdFromTheSameLib = true; // sec.derd ref from the same library as its target

		// decide whether is redirected
		CComQIPtr<IMgaReference> derdRef( ref), baseRef( base);
		if( !derdRef || !baseRef) { ASSERT( 0); return retv; }

		/*CComPtr<IMgaFCO> derdTarg, baseTarg;
		COMTHROW( derdRef->get_Referred( &derdTarg));
		COMTHROW( baseRef->get_Referred( &baseTarg));*/
		
		CComBSTR nm;
		COMTHROW(derdRef->get_Name( &nm));
		short st;
		COMTHROW(derdRef->CompareToBase( &st));
		/*bool x = COM_EQUAL( baseTarg, derdTarg);
		ASSERT( x == (st==0));
		if(x != (st==0))
			m_reporter.show( MyCComBSTR( nm));*/

		if( st != 0)//!COM_EQUAL( baseTarg, derdTarg)) // different target
		{
			// the only scenario this can happen is when the library contains baseTarg
			// and derdTarg as well
			*p_isRedirected = true;
		}
		// else: no more needed than the default action (derivation will make sure the derdRef will point to baseTarg)


		// the topmost base counts, so if the topmost base is in the library we consider it
		// lib derived
		if( *p_derdFromTheSameLib == false)
		{
			CComPtr<IMgaFCO> topmost_base = base;
			bool stop = false;
			while( !stop)
			{
				CComPtr<IMgaFCO> nxt_base;
				COMTHROW( topmost_base->get_DerivedFrom( &nxt_base));
				if( nxt_base)
					topmost_base = nxt_base;
				else
					stop = true;
			}

			elem_struct elem_topbase;
			getElemDetails( topmost_base, elem_topbase);

			//if( elem_topbase.inlib 
			if( elem_topbase.inverysamelib // TODO?
			&& Identifier::getTopLibRootOf( CoreObj( p_tgt)) == Identifier::getTopLibRootOf( CoreObj( topmost_base)))
				*p_derdFromTheSameLib = true; // actually derd ref from the same library as its target
		}
	}

	return retv;
}

void RefreshManager::saveOwnedStuff( CComPtr<IMgaConnection>& p_owner, connEnds_struct& p_one_conn)
{
	CoreObj owner( p_owner);
	ASSERT(!p_one_conn.saver);
	COMTHROW(m_mgaproject->dataproject->CreateObject( GetMetaID(owner), &p_one_conn.saver.ComPtr()));

	steal( owner, p_one_conn.saver, ATTRID_ATTRPARENT);
	steal( owner, p_one_conn.saver, ATTRID_CONSTROWNER);
}

void RefreshManager::loadSavedStuff( CComPtr<IMgaFCO>& p_nConn, connEnds_struct& p_one_conn)
{
	CoreObj inheritor( p_nConn);
	steal( p_one_conn.saver, inheritor, ATTRID_ATTRPARENT);
	steal( p_one_conn.saver, inheritor, ATTRID_CONSTROWNER);

	SingleObjTreeDelete( p_one_conn.saver, true);
}

void RefreshManager::getConnPointDetails( CComPtr<IMgaConnPoint>& cp, conndetails_struct& det, bool* p_ptrIsDestination)
{
	CComPtr<IMgaFCO> peer;
	COMTHROW( cp->get_Target( &peer));

	CComBSTR role;
	COMTHROW( cp->get_ConnRole( &role));
	if( role == "dst")
		*p_ptrIsDestination = true;
	else
		*p_ptrIsDestination = false;

	getElemDetails( peer, det.end);

	CComPtr<IMgaFCOs> refs;
	COMTHROW( cp->get_References( &refs));
	long l;
	COMTHROW( refs->get_Count( &l));
	for( long i = 1; i <= l; ++i)
	{
		CComPtr<IMgaFCO> ref;
		COMTHROW( refs->get_Item( i, &ref));

		elem_struct chain_elem;
		getElemDetails( ref, chain_elem);

		det.chain.push_back( chain_elem);
	}
}

void RefreshManager::getConnectionDetails( CComPtr<IMgaConnection>& owner, connEnds_struct& one_conn)
{
	saveOwnedStuff( owner, one_conn);
	// the connection's info stored in: one_conn
				
	// fill the conn id field, will help prevent duplicating loop connections
	COMTHROW( owner->get_ID( &one_conn.connid)); 
	COMTHROW( owner->get_Name( &one_conn.connname));
	COMTHROW( owner->GetGuidDisp( &one_conn.guid));

	CComPtr<IMgaModel> parentM;
	COMTHROW( owner->get_ParentModel( &parentM));
	CComPtr<IMgaFCO> parent = parentM;
	// fill the parent field
	getElemDetails( parent, one_conn.parent_model);

	one_conn.is_derived = one_conn.is_instance = false;
	one_conn.is_sec_derived = 0;

	// is it derived?
	CComPtr<IMgaFCO> arch;
	COMTHROW( owner->get_DerivedFrom( &arch));
	one_conn.is_derived = arch != 0;
	
	if( one_conn.is_derived)
	{
		one_conn.baseConnInLib = false;
		elem_struct arch_info;
		getElemDetails( arch, arch_info);
		if( arch_info.inverysamelib) // TODO? //if( arch_info.inlib) // the connection's basetype is in the library
		{
			one_conn.baseConnId    = arch_info.uid;
			one_conn.baseConnInLib = true;
		}
	}

	// is it instance?
	VARIANT_BOOL is_inst;
	COMTHROW( owner->get_IsInstance( &is_inst));
	one_conn.is_instance = is_inst == VARIANT_TRUE;

	// is it sec derived?
	VARIANT_BOOL is_prim;
	COMTHROW( owner->get_IsPrimaryDerived( &is_prim));
	one_conn.is_sec_derived = is_prim == VARIANT_FALSE?1:0;

	CComPtr<IMgaMetaRole> mrole;
	COMTHROW( owner->get_MetaRole( &mrole));
	// fill the role field the connection is inserted into its parent
	COMTHROW( mrole->get_Name( &one_conn.role));

	CComPtr<IMgaConnPoints> connected;
	COMTHROW( owner->get_ConnPoints( &connected));
	long lc;
	COMTHROW( connected->get_Count( &lc));
	for( long i = 1; i <= lc; ++i)
	{
		CComPtr<IMgaConnPoint> conn_peer;
		COMTHROW( connected->get_Item( i, &conn_peer));
		// if the connection point is different from the originating one, that we started from
		//if( conn_peer == cp)
		//{
		//}

		conndetails_struct cdd;
		bool isDestination;
		getConnPointDetails( conn_peer, cdd, &isDestination);
		if( isDestination)
			one_conn.dst = cdd;
		else
			one_conn.src = cdd;
	}
}

void RefreshManager::addChangedLib( elem_struct& elem)
{
	if( !elem.inanotherlib) return;

	if( m_changedLibs.end() == std::find( m_changedLibs.begin(), m_changedLibs.end(), elem.id))
		m_changedLibs.push_back( elem.id);
}

void RefreshManager::collectMixedConns( CoreObj& one_fco)
{
	auto obj = ObjForCore(one_fco);
	if( obj)
	{
		CComPtr<IMgaConnPoints> cps1;
		COMTHROW( obj->get_PartOfConns( &cps1));
		long l1;
		COMTHROW( cps1->get_Count( &l1));
		// for all my connection points
		for( long i = 1; i <= l1; ++i)
		{
			CComPtr<IMgaConnPoint> cp;
			COMTHROW( cps1->get_Item( i, &cp));

			// the connection itself
			CComPtr<IMgaConnection> owner;
			COMTHROW( cp->get_Owner( &owner));

			VARIANT_BOOL in_lib;
			COMTHROW( owner->get_IsLibObject( &in_lib));
			
			elem_struct elem_conn;
			getElemDetails( CComQIPtr<IMgaFCO>( owner), elem_conn);
			/*if( in_lib == VARIANT_TRUE && elem_conn.inlib) // which lib? it matters!
			{
				if( elem_conn.uid.libId == Identifier::getLibRootOf( one_fco)
					|| elem_conn.uid.libId == Identifier::getTopLibRootOf( one_fco)
					)
					continue;
				else
					if( m_changedLibs.end() == std::find( m_changedLibs.begin(), m_changedLibs.end(), elem_conn.uid.libId))
						m_changedLibs.push_back( elem_conn.uid.libId);
			}*/

			/*if( in_lib == VARIANT_TRUE) // the connection also in the library
				continue;*/
			if( elem_conn.inverysamelib)
				continue;
			//WAS// 
			//else if( elem_conn.inanotherlib)
			//	addChangedLib( elem_conn);

			CComPtr<IMgaFCO> myself;
			COMTHROW( cp->get_Target( &myself));
			//ASSERT( myself == obj);

			// the connection's info stored in:
			connEnds_struct one_conn;
			getConnectionDetails( owner, one_conn);

			// if sec derived from a project conn (non library conn)
			// then no need to store it, since its base will be recreated
			// and then the change propagated down into this place
			if( one_conn.is_derived && one_conn.is_sec_derived)
			{
				if( one_conn.baseConnId.libId != Identifier::getLibRootOf( one_fco)
					|| one_conn.baseConnId.libId != Identifier::getTopLibRootOf( one_fco)
					) // which lib? it matters!
					continue;
			}
			/*if( one_conn.is_derived && one_conn.is_sec_derived && !one_conn.baseConnInLib)
				continue;*/

			// even a sec-derd connection from library needs to be stored
			// because it needs to be deleted while the reference involved
			// is redirected, thus later needs restoration

			// examine if this connection has already been inserted into the vector
			if( conns_stored.end() == std::find_if( conns_stored.begin(), conns_stored.end(), HasThisConn(one_conn.connid))) // not found
				conns_stored.push_back( one_conn);

			// store the sets the connections is member of
			// because these connection might get deleted
			// their membership needs storage
			collectMembershipOfConn( owner, one_conn);
		}
	}
}


// input: a connection in a base, which needs to be propagated down later into derd
// p_base: a fresh base which contains p_coreConn as a new connection
// p_derd: an object detached from a previous version of base
void RefreshManager::collectFreshConnection( const CoreObj& p_coreConn
                                           , const CoreObj& p_base, const CoreObj& p_derd)
{
	CComPtr<IMgaFCO> fco;
	auto c = ObjForCore( p_coreConn);
	if( c) c->getinterface( &fco);
	CComQIPtr<IMgaConnection> conn( fco);

	CComPtr<IMgaFCO> b_fco, d_fco;
	c = static_cast<FCO *>(ObjForCore( p_base));
	if( c) c->getinterface( &b_fco);
	c = static_cast<FCO *>(ObjForCore( p_derd));
	if( c) c->getinterface( &d_fco);
	CComQIPtr<IMgaModel> b_model( b_fco), d_model( d_fco);
	if( conn && b_model && d_model)
	{
		derConnEnds_struct derconn_details;
		derconn_details.base_parent = b_model;
		derconn_details.derd_parent = d_model;
		derconn_details.b_conn      = conn;

		COMTHROW(conn->get_MetaRole( &derconn_details.b_conn_role));

		getConnectionDetails( conn, derconn_details.b_conn_struct);
		
		conns_to_derive.push_back( derconn_details);

		
	}
	else ASSERT(0);
}

bool RefreshManager::buildRefChainColl( CMgaProject *mgaproject, CoreObj& folder, CHAIN_VEC& p_chain, CComPtr<IMgaFCOs>& p_sref_chain)
{
	bool not_found = false;
	for( unsigned int i = 0; !not_found && i < p_chain.size(); ++i)
	{
		CComPtr<IMgaFCO> ref_chain_elem = SearchTool::findObjWithDetails( mgaproject, folder, p_chain[i]);
		if( !ref_chain_elem) not_found = true;

		if( !p_sref_chain)
			COMTHROW( p_sref_chain.CoCreateInstance( L"Mga.MgaFCOs"));
		COMTHROW( p_sref_chain->Append( ref_chain_elem));
	}

	return !not_found;
}

void RefreshManager::collectMembershipOfConn( const CComPtr<IMgaConnection>& p_conn, const connEnds_struct& p_conn_info)
{
	CComPtr<IMgaFCOs> sets;
	COMTHROW( p_conn->get_MemberOfSets( &sets));
	long c = 0;
	if( sets) COMTHROW( sets->get_Count( &c));
	for( long i = 1; i <= c; ++i)
	{
		CComPtr<IMgaFCO> set;
		COMTHROW( sets->get_Item( i, &set));
		
		// getting project id of set
		PROJ_ID pid;
		COMTHROW( set->get_ID( &pid));

		// since usually sets survive the refresh
		// project id is a good identifier
		m_mapOfMemberships[p_conn_info.connid].push_back( pid);
	}
}

void RefreshManager::restoreMembership( const CComPtr<IMgaFCO>& p_conn, connEnds_struct& p_conn_info, const CComPtr<IMgaModel>& p_parent)
{
	MEMBERMAP::iterator it = m_mapOfMemberships.find( p_conn_info.connid);
	if( it != m_mapOfMemberships.end())
	{
		PIDVECTOR &pidvector = it->second;
		for( unsigned int i = 0; i < pidvector.size(); ++i)
		{
			CComPtr<IMgaFCO> res;
			findAmongContained( p_parent, pidvector[i], res);
			if( res)
			{
				CComQIPtr<IMgaSet> set( res);
				if( set)
				{
					VARIANT_BOOL it_is;
					COMTHROW( set->get_IsMember( p_conn, &it_is));
					if( it_is == VARIANT_FALSE)
						COMTHROW( set->AddMember( p_conn));
				}
			}
		}
	}
}

void RefreshManager::restoreMixedConns( CoreObj& folder)
{
	// restore deleted conns when ref redirect happened
	// those are saved already in conns_stored
	//restoreDeletedConns( folder);
	
	for( unsigned int i = 0; i < conns_stored.size(); ++i)
	{
		connEnds_struct &current_i = conns_stored[i];
		CComPtr<IMgaFCO> s = SearchTool::findObjWithDetails( m_mgaproject, folder, current_i.src.end);
		CComPtr<IMgaFCO> d = SearchTool::findObjWithDetails( m_mgaproject, folder, current_i.dst.end);
		CComPtr<IMgaFCO> p = SearchTool::findObjWithDetails( m_mgaproject, folder, current_i.parent_model);
		CComPtr<IMgaModel> pM;
		if( p) COMTHROW( p.QueryInterface( &pM));

		CComPtr<IMgaFCOs> sref_chain;
		CComPtr<IMgaFCOs> dref_chain;
		bool chain1_rb_ok( false), chain2_rb_ok( false); // rebuilding ok?
		bool chain1_vf_ok( false), chain2_vf_ok( false); // verification ok?
		bool connends_and_parent_ok = s && d && pM;

		if( connends_and_parent_ok)
		{
			chain1_rb_ok = buildRefChainColl( m_mgaproject, folder, current_i.src.chain, sref_chain);
			chain2_rb_ok = buildRefChainColl( m_mgaproject, folder, current_i.dst.chain, dref_chain);

			if( chain1_rb_ok && chain2_rb_ok)
			{
				chain1_vf_ok = verifyChain( pM, sref_chain, s);
				chain2_vf_ok = verifyChain( pM, dref_chain, d);
			}
		}

		if( connends_and_parent_ok
			&& chain1_rb_ok && chain2_rb_ok
			&& chain1_vf_ok && chain2_vf_ok)
		{ // OK
			CComPtr<IMgaMetaFCO> p_meta;

			COMTHROW( pM->get_Meta( &p_meta));
			CComPtr<IMgaMetaModel> p_metaM;
			COMTHROW( p_meta.QueryInterface( &p_metaM));
			
			CComPtr<IMgaMetaRole> metarole;
			COMTHROW( p_metaM->get_RoleByName( current_i.role, &metarole));

			try {
				CComPtr<IMgaFCO> new_conn;
				//COMTHROW( pM->CreateSimpleConn( metarole, s, d, sref_chain, dref_chain, &new_conn));
				COMTHROW( Creator::SimpleConn( pM, metarole, s, d, sref_chain, dref_chain, m_mgaproject, &new_conn));

				//COMTHROW( new_conn->put_Name( current_i.connname));
				COMTHROW( Worker::put_Name( m_mgaproject, new_conn, current_i.connname));
				//COMTHROW( new_conn->PutGuidDisp( current_i.guid));
				COMTHROW( Worker::PutGuidDisp( m_mgaproject, new_conn, current_i.guid));

				loadSavedStuff( new_conn, current_i);
				
				if( current_i.is_derived && current_i.is_sec_derived)
				{
					// We deleted and restored rigth now a connection.
					// We pretend that the object is the same
					// that is why we have changed the guid
					// we update the mapOfSecondaryDeriveds
					// because we must pretend the new connection
					// is a detached obj, formerly secderived.

					PROJ_ID new_conn_id;
					COMTHROW( new_conn->get_ID( &new_conn_id));
#if(DONT_OPTIM)
					mapOfSecondaryDeriveds[ new_conn_id] = current_i.baseConnId;
#endif
					napOfSecondaryDeriveds[ new_conn   ] = current_i.baseConnId;
				}

				MyCComBSTR msg;
				msg.appendLink( new_conn); 
#ifdef _DEBUG
				CComBSTR ncid; COMTHROW(new_conn->get_ID( &ncid));
				COMTHROW(msg.AppendBSTR( ncid));
				if( current_i.is_derived && current_i.is_sec_derived) 
					COMTHROW(msg.Append( L" napofsec "));
				else 
					COMTHROW(msg.Append( L" reg "));
#endif
				COMTHROW(msg.Append( L" reconstructed in "));
				msg.appendLink( pM);
				m_reporter.show( msg, false);

				// if the original connection was part of any set
				// we need to restore membership because the connection
				// has been deleted and replaced by a new one
				restoreMembership( new_conn, current_i, pM);

			} catch( hresult_exception& ) {

				// report error
				//MyCComBSTR msg( "Connection ");
				//msg.Append( current_i.connname);
				//msg.Append( L" could not be reconstructed in "); 
				//msg.appendLink( pM);
				//msg.Append( L"<br>");
				//prepareConnErrMsg( pM, s, d, sref_chain, dref_chain,  current_i, msg); // the pointers are valid
				MyCComBSTR msg;
				prepareConnErrMsg( pM, s, d, sref_chain, dref_chain,  current_i, msg); // the pointers are valid
				m_reporter.show( msg);
				ASSERT(0);
			}
		}
		else
		{
			// report error
			//MyCComBSTR msg( "Connection ");
			//msg.Append( current_i.connname);
			//msg.Append( L" could not be reconstructed in "); 
			//if( pM) msg.appendLink( pM);// pM might be 0
			//else    msg.appendLink( current_i.parent_model.id, "Parent");
			//msg.Append( L"<br>");
			//if( chain1_rb_ok && chain2_rb_ok) // buildRefChainColl succeeded, verifyChain failed
			//{
			//	if( !chain1_vf_ok) msg.Append( L"Reason: SrcChain verification failed.");
			//	if( !chain2_vf_ok) msg.Append( L"Reason: DstChain verification failed.");
			//}
			//else // buildRefChainColl failed
			//{
			//	if( !chain1_rb_ok) msg.Append( L"Reason: SrcChain rebuilding failed.");
			//	if( !chain2_rb_ok) msg.Append( L"Reason: DstChain rebuilding failed.");
			//}
			//if( connends_and_parent_ok)
			//{
			//	msg.Append( L"<br>");
			//	prepareConnErrMsg( pM, s, d, sref_chain, dref_chain, current_i, msg);
			//}
			MyCComBSTR msg;
			prepareConnErrMsg( pM, s, d, sref_chain, dref_chain, current_i, msg);
			if( chain1_rb_ok && chain2_rb_ok) // buildRefChainColl succeeded, verifyChain failed
			{
				COMTHROW(msg.Append( L"<br>"));
				if( !chain1_vf_ok)
					COMTHROW(msg.Append( L"Reason: SrcChain verification failed. "));
				if( !chain2_vf_ok)
					COMTHROW(msg.Append( L"Reason: DstChain verification failed. "));
			}
			else // buildRefChainColl failed
			{
				COMTHROW(msg.Append( L"<br>"));
				if( !chain1_rb_ok) 
					COMTHROW(msg.Append( L"Reason: SrcChain rebuilding failed. "));
				if( !chain2_rb_ok)
					COMTHROW(msg.Append( L"Reason: DstChain rebuilding failed. "));
			}
			m_reporter.show( msg);
		}
	}
	conns_stored.clear();
}

// looks recursively for p_id in d_par
void RefreshManager::findAmongContained( const CComPtr<IMgaModel>& d_par, const PROJ_ID& p_id, CComPtr<IMgaFCO>& res)
{
	CComPtr<IMgaFCOs> children;
	COMTHROW( d_par->get_ChildFCOs( &children));
	long c = 0;
	if( children) COMTHROW( children->get_Count( &c));
	for( long i = 1; !res && i <= c; ++i)
	{
		CComPtr<IMgaFCO> child;
		COMTHROW( children->get_Item( i, &child));

		PROJ_ID i_id;
		COMTHROW( child->get_ID( &i_id));

		if( i_id == p_id)
		{
			res = child;
		}
		else
		{
			CComQIPtr<IMgaModel> child_model( child);
			if( child_model)
				findAmongContained( child_model, p_id, res);
		}
	}
}

// searches for an element inside d_par, derived from the object with p_uid
void RefreshManager::findAmongSecDerd( UniqueId& p_uid, const CComPtr<IMgaModel>& d_par, CComPtr<IMgaFCO>& res)
{
#if(DONT_OPTIM)
	SEC_DER_MAP::const_iterator it = mapOfSecondaryDeriveds.begin();
	while( !res && it != mapOfSecondaryDeriveds.end())
	{
		if( it->second == p_uid) // an element (with id= it->first) is found as derived from p_uid
		{
			// it->first is the id of the derived element from p_uid
			// d_par contains it->first (either as child or grandchild [in case of port])
			findAmongContained( d_par, it->first, res);
		}
		++it;
	}
#endif
}

void RefreshManager::findAmongNecDerd( UniqueId& p_uid, const CComPtr<IMgaModel>& d_par, CComPtr<IMgaFCO>& res)
{
	SEC_DER_NAP::const_iterator it = napOfSecondaryDeriveds.begin();
	while( !res && it != napOfSecondaryDeriveds.end())
	{
		if( it->second == p_uid) // an element (with id= it->first) is found as derived from p_uid
		{
			// it->first is the once-derived element from libobj identified by p_uid
			// d_par contains it->first (either as child or grandchild [in case of port])
			if( isInside( d_par, it->first))
				res = it->first;
		}
		++it;
	}
}
// more precisely: is equal or is inside?
bool RefreshManager::isInside( const CComPtr<IMgaModel>& d_par, const CComPtr<IMgaFCO>& p_fco)
{
	CoreObj d_parent( d_par);
	CoreObj p( p_fco);
	bool found = false;
	while( !found && p && p.IsFCO())
	{
		found = COM_EQUAL( p, d_parent);
		p = p[ATTRID_FCOPARENT];
	}

	return found;
}

void RefreshManager::findAmongPriDerd( UniqueId& p_uid, const CComPtr<IMgaModel>& d_par, CComPtr<IMgaFCO>& res)
{
	SUB_PAR_MAP::const_iterator it = mapOfDeriveds.find( p_uid);
	if( it != mapOfDeriveds.end())
	{
		const SUB_PAR_VEC &pair_vec = it->second;
		for( SUB_PAR_VEC::const_iterator jt = pair_vec.begin();
			!res && jt != pair_vec.end(); ++jt)
		{
			const CComPtr<IMgaFCO> &prev_detached_obj = jt->subt_ptr;
			if( isInside( d_par, prev_detached_obj))
				res = prev_detached_obj;
		}
	}
}

void RefreshManager::askTheMatchMaker( const CComPtr<IMgaFCO>& end, const CComPtr<IMgaModel>& b_par, const CComPtr<IMgaModel>& d_par, CComPtr<IMgaFCO>& n_end, bool checkParentHood)
{
	CoreObj b_end( end), d_end;
	CORE_MATCH_MAP::iterator it = m_coreMatchMaker.begin();
	for( ; !d_end && it != m_coreMatchMaker.end(); ++it)
	{
		if( it->second == b_end)
		{
			if( checkParentHood)
			{
				CoreObj d_parent( d_par);
				CoreObj d_paren2( it->first[ATTRID_FCOPARENT]);
				CoreObj d_paren3;
				if( d_paren2.GetMetaID() != DTID_ROOT)
					d_paren3 = d_paren2[ATTRID_FCOPARENT];

				if( COM_EQUAL( d_parent, d_paren2)  // d_end child of d_parent
				|| COM_EQUAL( d_parent, d_paren3)) // d_end grandchild of d_parent (visible as a port)
					d_end = it->first;
			}
			else
				d_end = it->first;
		}
	}

	if( d_end)
	{
		ObjForCore(d_end)->getinterface( &n_end); 
	}
}

//
// something like GetDerivedEquivalent, based on our records
// if checkParentHood turned on, then n_end is supposed to be in d_par (for sets, connections)
// or to be a grandchild of d_par (if it is a port) for connections
// if checkParentHood is false no such test is performed on the found element
// ideally: end's relation to b_par (child, grandchild) should be the same as n_end's relation to d_par
void RefreshManager::getLibDerdEquiv( const CComPtr<IMgaFCO>& end, const CComPtr<IMgaModel>& b_par, const CComPtr<IMgaModel>& d_par, CComPtr<IMgaFCO>& n_end, bool checkParentHood /*=true*/)
{
	if( b_par == d_par || !end) 
	{
		n_end = end;
		return;
	}
	
	// first do a ptr based search among the derived objects
	askTheMatchMaker( end, b_par, d_par, n_end, checkParentHood);

	if( !n_end) // do a id-based search 
	{
		// end is supposed to be in the library
		UniqueId uid = Identifier::getDetailsOf( CoreObj( end));
		findAmongSecDerd( uid, d_par, n_end);
		
		CComPtr<IMgaFCO> nnend;
		findAmongNecDerd( uid, d_par, nnend);
		ASSERT( n_end == nnend);

		if( !n_end)
			findAmongPriDerd( uid, d_par, n_end);
	}
}

CComPtr<IMgaModel> RefreshManager::nextContainer( CComPtr<IMgaFCO>& p_ref)
{
	CComPtr<IMgaModel> retv;
	CComQIPtr<IMgaReference> ref( p_ref);
	if( !ref) return retv;

	CComPtr<IMgaFCO> tgt;
	COMTHROW( ref->get_Referred( &tgt));
	if( tgt)
	{
		CComPtr<IMgaModel> par;
		if( SUCCEEDED( tgt->get_ParentModel( &par)))
			retv = par;
	}

	return retv;
}

// passes one step in a reference chain
CComPtr<IMgaFCO> RefreshManager::nextElem( CComPtr<IMgaFCO>& p_ref)
{
	CComPtr<IMgaFCO> retv;
	CComQIPtr<IMgaReference> ref( p_ref);
	if( !ref) return retv;

	CComPtr<IMgaFCO> tgt;
	COMTHROW( ref->get_Referred( &tgt));
	if( tgt)
	{
		retv = tgt;
	}

	return retv;
}

// finds an element taking part in a connection
bool RefreshManager::findConnEndPeer( const derConnEnds_struct& c_info
		, const CComPtr<IMgaFCOs>& p_chainOrig, const CComPtr<IMgaFCO>& p_endOrig
		,       CComPtr<IMgaFCOs>& p_chainPeer,       CComPtr<IMgaFCO>& p_endPeer)
{
#ifdef _DEBUG
	bool details = false;
#endif
	bool so_far_ok( true);
	long len( 0);
	if( p_chainOrig) COMTHROW( p_chainOrig->get_Count( &len));
	if( len == 0)
	{
		getLibDerdEquiv( p_endOrig, c_info.base_parent, c_info.derd_parent, p_endPeer);
		so_far_ok = p_endPeer != 0;
	}
	else
	{
		// if p_chainOrig is a chain of lenght len
		// we must find a similar chain for p_chainPeer
		COMTHROW( p_chainPeer.CoCreateInstance( L"Mga.MgaFCOs"));

		CComPtr<IMgaModel> base_cur_parent( c_info.base_parent);
		CComPtr<IMgaModel> derd_cur_parent( c_info.derd_parent);
		CComPtr<IMgaFCO>   prev_elem;
		for( long i = 1; so_far_ok && i <= len; ++i)
		{
			CComPtr<IMgaFCO> chain_elem;   // orig chain elem
			CComPtr<IMgaFCO> n_chain_elem; // new chain elem

			COMTHROW( p_chainOrig->get_Item( i, &chain_elem));

			if( i == 1) // the first step is special
			{
				getLibDerdEquiv( chain_elem, base_cur_parent, derd_cur_parent, n_chain_elem);
			}
			else
			{
				n_chain_elem = nextElem( prev_elem);
			}
			
			if( n_chain_elem)
			{
				COMTHROW( p_chainPeer->Append( n_chain_elem));
				//base_cur_parent = nextContainer( chain_elem);
				//derd_cur_parent = nextContainer( n_chain_elem);

				prev_elem = n_chain_elem;

#ifdef _DEBUG
				MyCComBSTR msg;
				msg.appendLink( n_chain_elem);
				msg.Append( L" counterpart found for ");
				msg.appendLink( chain_elem);
				if( details) m_reporter.show( msg, false);
#endif
			}
			else
			{
				so_far_ok = false;
#ifdef _DEBUG
				MyCComBSTR msg;
				msg.Append( L" Not found");
				msg.Append( L" counterpart for ");
				msg.appendLink( chain_elem);
				if( details) m_reporter.show( msg, true);
#endif
			}
		}

		if( so_far_ok) //
		{
			CComPtr<IMgaModel> base_connend_parent; // the connend container in base (a model)
			CComPtr<IMgaModel> derd_connend_parent; // the connend container in derd (a model)
			bool b_ok = followRefChain( p_chainOrig, base_connend_parent);
			bool d_ok = followRefChain( p_chainPeer, derd_connend_parent);

			if( b_ok && d_ok && base_connend_parent && derd_connend_parent)
			{
#ifdef _DEBUG
				// reporting progress
				MyCComBSTR msg;
				msg.Append( L"Looking for counterpart of ");
				msg.appendLink( p_endOrig);
				msg.Append( L" ( ");
				msg.appendLink( base_connend_parent);
				msg.Append( L" -basemodel) in ");
				msg.appendLink( derd_connend_parent);
				msg.Append( L" -derdmodel");
				if( details) m_reporter.show( msg, false);
#endif
				getLibDerdEquiv( p_endOrig, base_connend_parent, derd_connend_parent, p_endPeer);
				so_far_ok = p_endPeer != 0;
			}
			else
			{
				so_far_ok = false;

#ifdef _DEBUG
				// reporting error
				MyCComBSTR msg;
				msg.Append( L"Invalid chain. Can't look for counterpart of ");
				msg.appendLink( p_endOrig);
				msg.Append( L" ( ");
				if( base_connend_parent) msg.appendLink( base_connend_parent);
				else                     msg.Append( L"empty basemodel");
				msg.Append( L" ) in ");
				if( derd_connend_parent) msg.appendLink( derd_connend_parent);
				else                     msg.Append( L"empty derdmodel");
				if( details) m_reporter.show( msg, false);

				ASSERT(0);
#endif
			}
		}
	}

	return so_far_ok;
}

bool RefreshManager::fillConnectionPtrs( const derConnEnds_struct& c_info
			, CComPtr<IMgaFCO>& p_newSrc,     CComPtr<IMgaFCO>& p_newDst
			, CComPtr<IMgaFCOs>& p_newSChain, CComPtr<IMgaFCOs>& p_newDChain)
{
	CComQIPtr<IMgaSimpleConnection> b_sconn( c_info.b_conn);
	ASSERT( b_sconn);
	if( !b_sconn) return false;

	bool rv = false;
	CComPtr<IMgaFCOs> s_chain, d_chain;
	COMTHROW( b_sconn->get_SrcReferences( &s_chain));
	COMTHROW( b_sconn->get_DstReferences( &d_chain));

	CComPtr<IMgaFCO> src, dst;
	COMTHROW( b_sconn->get_Src( &src));
	COMTHROW( b_sconn->get_Dst( &dst));

	bool v1 = findConnEndPeer( c_info, s_chain, src, p_newSChain, p_newSrc);
	bool v2 = findConnEndPeer( c_info, d_chain, dst, p_newDChain, p_newDst);

	rv = v1 && v2 && p_newSrc && p_newDst;

	return rv;
}

bool RefreshManager::followRefChain( const CComPtr<IMgaFCOs>& chain, CComPtr<IMgaModel>& final_m)
{
	bool retv = false;
	long c(0);
	if( chain) COMTHROW( chain->get_Count( &c));
	if( c >= 1)
	{
		CComPtr<IMgaFCO> it;
		COMTHROW( chain->get_Item( c, &it)); // get the last item in the chain
		CComQIPtr<IMgaReference> rit( it);
		if( rit) // it is a reference
		{
			CComPtr<IMgaFCO> tgt;
			COMTHROW( rit->get_Referred( &tgt)); 
			if( tgt)
			{
				CComQIPtr<IMgaModel> f_m( tgt);
				if( f_m) // it refers to a model
				{
					final_m = f_m;
					retv = true; // 
				}
			}
		}
	}
	return retv;
}

// a connection can be drawn to a
// 1. connend in the same parent as the connection
// 2. connend whose parent is sibling of the connection
//    because the parent show this connend as a ports
// 3. referenceports: at the end of the chain there is a model having the connend
bool RefreshManager::verifyChain( const CComPtr<IMgaModel>& pM, const CComPtr<IMgaFCOs>& sref_chain, const CComPtr<IMgaFCO>& s)
{
	ASSERT( s);
	ASSERT( pM);
	if( !s) return false;
	if( !pM) return false;

	bool ret = true;
	long c = 0;
	if( sref_chain)
		COMTHROW(sref_chain->get_Count( &c));
	if( c == 0)
	{
		// s must be pM's child, or its grandchild (but in this case must be a port)
		CComPtr<IMgaModel> par;
		HRESULT hr = s->get_ParentModel( &par);
		ret = SUCCEEDED( hr) && COM_EQUAL(par, pM);
		if( !ret && par) // test if grandchild
		{
			CComPtr<IMgaModel> par2;
			hr = par->get_ParentModel( &par2);
			ret = SUCCEEDED( hr) && COM_EQUAL( par2, pM); //is <s> a grandchild of pM?
		}
	}
	else if( c >= 1)
	{
		CComPtr<IMgaFCO> next;
		COMTHROW(sref_chain->get_Item( 1, &next));
		// next must be pM's child
		CComPtr<IMgaModel> par;
		HRESULT hr = E_FAIL;
		if( next) hr = next->get_ParentModel( &par);
		if( SUCCEEDED( hr) && COM_EQUAL( par, pM))
		{
			long i;
			for( i = 2; ret && i <= c; ++i)
			{
				CComPtr<IMgaFCO> chain_elem;
				COMTHROW(sref_chain->get_Item( i, &chain_elem));

				CComQIPtr<IMgaReference> next_as_ref( next);
				if( next_as_ref)
				{
					CComPtr<IMgaFCO> tgt;
					COMTHROW( next_as_ref->get_Referred( &tgt));
					
					// the 1st refs target must be the 2nd element in the array and so on
					ASSERT( COM_EQUAL( chain_elem, tgt));
					if( COM_EQUAL( chain_elem, tgt))
						next = tgt;
					else // chain element (i) not pointing to element (i+1)
						ret = false; // exit
				}
				else // chain_elem not a reference
					ret = false; // exit
			
			} // for

			if( i == c+1 && ret) // ret is still ok, went through the chain
			{
				CComPtr<IMgaFCO> last;
				COMTHROW(sref_chain->get_Item(c, &last));
				CComQIPtr<IMgaReference> last_ref( last);
				if( last_ref)
				{
					CComPtr<IMgaFCO> tgt;
					COMTHROW( last_ref->get_Referred( &tgt));
					CComQIPtr<IMgaModel> tgt_model( tgt);
					if( tgt_model)
					{
						// s must be a child of tgt_model
						CComPtr<IMgaModel> par;
						HRESULT hr = s->get_ParentModel( &par);
						
						ret = SUCCEEDED( hr) && COM_EQUAL( par, tgt_model);
					}
					else // last element's target must be a model
						ret = false;
				}
				else // last element is not a ref?
					ret = false;
			}
		}
		else ret = false;
	} // c == 0 || c >= 1

	return ret;
}
 

void RefreshManager::prepareConnErrMsg( 
			const CComPtr<IMgaModel>& pM
			, const CComPtr<IMgaFCO>& s, const CComPtr<IMgaFCO>& d
			, const CComPtr<IMgaFCOs>& sref_chain, const CComPtr<IMgaFCOs>& dref_chain
			, const connEnds_struct& current_i, MyCComBSTR& msg)
{
	//
	// prepare error message
	//msg.appendLink( current_i.connid, current_i.connname);
	COMTHROW(msg.Append( current_i.connname));
	COMTHROW(msg.Append( L" ["));
	COMTHROW(msg.Append( current_i.connid));
	COMTHROW(msg.Append( L"]"));
	COMTHROW(msg.Append( L" connection could not be reconstructed in "));
	if( pM)
		msg.appendLink( pM);
	else
		COMTHROW(msg.Append( L"Null"));

	COMTHROW(msg.Append( L"<br>\tSrc: "));
	if( s)
		msg.appendLink( s);
	else
		COMTHROW(msg.Append( L"Null"));

	COMTHROW(msg.Append( L" Dst: "));
	if( d)
		msg.appendLink( d);
	else
		COMTHROW(msg.Append( L"Null"));

	COMTHROW(msg.Append( L"<br>\tSrcRefs:"));
	long c_len = 0;
	if( sref_chain)
		COMTHROW(sref_chain->get_Count( &c_len));
	for( long i = 1; i <= c_len; ++i)
	{
		if( i != 1)
			COMTHROW(msg.Append( L","));
		COMTHROW(msg.Append( L" "));
		CComPtr<IMgaFCO> r_i;
		COMTHROW(sref_chain->get_Item( i, &r_i));
		msg.appendLink( r_i);
	}

	COMTHROW(msg.Append( L" DstRefs:"));
	c_len = 0;
	if( dref_chain) 
		COMTHROW(dref_chain->get_Count( &c_len));
	for( long i = 1; i <= c_len; ++i)
	{
		if( i != 1)
			COMTHROW(msg.Append( L","));
		COMTHROW(msg.Append( L" "));
		CComPtr<IMgaFCO> r_i;
		COMTHROW(dref_chain->get_Item( i, &r_i));
		msg.appendLink( r_i);
	}
}

// fresh connections are those which have appeared after our last refresh
// so these must be propagated down into the derived object (if derived 
// from a library object)
// most typically such an object is a secondary derived object
void RefreshManager::syncFreshConns()
{
	for( unsigned int i = 0; i < conns_to_derive.size(); ++i)
	{
		derConnEnds_struct &i_conn = conns_to_derive[i];

		CComPtr<IMgaFCO>  s, d;
		CComPtr<IMgaFCOs> sref_chain, dref_chain;
		CComPtr<IMgaModel> pM = i_conn.derd_parent;

		// we would have used GetDerivedEquivalent had the base-derive relation been re-established
		bool chain1_ok = false;
		bool chain2_ok = false;
		bool valid = fillConnectionPtrs( i_conn, s, d, sref_chain, dref_chain);
		if( valid)
		{
			// the pointers are found, but let's verify whether 
			// the chain is leading really towards s, d
			chain1_ok = verifyChain( pM, sref_chain, s);
			chain2_ok = verifyChain( pM, dref_chain, d);
		}


		// this connection needs to be derived
		// pointer in i_conn.b_conn
		// structure info in i_conn.b_conn_struct
		connEnds_struct &current_i = i_conn.b_conn_struct;

		// we check again if the connend can be reached from pM though the chain

		if( valid && chain1_ok && chain2_ok)
		{   // OK
			CComPtr<IMgaFCO> new_conn;
			try {
				//COMTHROW( pM->CreateSimpleConn( i_conn.b_conn_role, s, d, sref_chain, dref_chain, &new_conn));
				COMTHROW( Creator::SimpleConn( pM, i_conn.b_conn_role, s, d, sref_chain, dref_chain, m_mgaproject, &new_conn));
				//COMTHROW( new_conn->put_Name( current_i.connname));
				COMTHROW( Worker::put_Name( m_mgaproject, new_conn, current_i.connname));

				m_coreMatchMaker[ CoreObj(new_conn)] = CoreObj(i_conn.b_conn);
#if(0)
				//UniqueId base_conn_id = Identifier::getDetailsOf( CoreObj(i_conn.b_conn));
				//PROJ_ID new_conn_id;
				//COMTHROW( new_conn->get_ID( &new_conn_id));
				//mapOfSecondaryDeriveds[ new_conn_id] = base_conn_id;
#endif
				// since this connection is fresh (just added to the base), it
				// had no peer in the prev version of the project, we don't care
				// about replacing its guid with the old one, nor we care about
				// inserting it into mapOfSecondaryDeriveds
				
				MyCComBSTR msg;
				msg.appendLink( new_conn); 
				COMTHROW(msg.Append( L" reconstructed in "));
				msg.appendLink( pM);
#ifdef _DEBUG
				CComBSTR ncid, pmid;
				COMTHROW(new_conn->get_ID( &ncid));
				COMTHROW(msg.AppendBSTR( ncid));
				COMTHROW(msg.Append( " "));
				COMTHROW(pM->get_ID( &pmid));
				COMTHROW(msg.AppendBSTR( pmid));
#endif
				m_reporter.show( msg, false);

			} catch( hresult_exception& ) {

				// report error
				//MyCComBSTR msg( "Connection ");
				//msg.Append( current_i.connname);
				//msg.Append( L" could not be reconstructed in "); 
				//msg.appendLink( pM);
				//msg.Append( L"<br>");
				//prepareConnErrMsg( pM, s, d, sref_chain, dref_chain, current_i, msg);
				MyCComBSTR msg;
				prepareConnErrMsg( pM, s, d, sref_chain, dref_chain, current_i, msg);
				m_reporter.show( msg);

				ASSERT(0);

				if( new_conn && ObjForCore( CoreObj( new_conn)))
				{
					//new_conn->DestroyObject();
					ObjForCore( CoreObj(new_conn))->inDeleteObject();
				}
			}
		}
		else
		{
			// report error
			//MyCComBSTR msg( "Connection ");
			//msg.Append( current_i.connname);
			//msg.Append( L" could not be reconstructed in "); 
			//if( pM) msg.appendLink( pM);
			//else msg.appendLink( current_i.parent_model.id, "Parent");

			//if( valid)
			//{
			//	if( !chain1_ok) msg.Append( L"Reason: SrcChain verification failed.");
			//	if( !chain2_ok) msg.Append( L"Reason: DstChain verification failed.");
			//	msg.Append( L"<br>");
			//	prepareConnErrMsg( pM, s, d, sref_chain, dref_chain, current_i, msg);
			//	m_reporter.show( msg);
			//}
			//m_reporter.show( msg);
			MyCComBSTR msg;
			prepareConnErrMsg( pM, s, d, sref_chain, dref_chain, current_i, msg);
			if( valid)
			{
				COMTHROW(msg.Append( L"<br>"));
				if( !chain1_ok)
					COMTHROW(msg.Append( L"Reason: SrcChain verification failed. "));
				if( !chain2_ok)
					COMTHROW(msg.Append( L"Reason: DstChain verification failed. "));
			}
				
			m_reporter.show( msg);
		}
	}
	conns_to_derive.clear();
}

// obsolete, since any connection detected upon reference-redirection
// is considered previously as a mixed connection, thus important
// information is saved about it
void RefreshManager::saveConnection( const CComPtr<IMgaFCO>& p_cn)
{
#if(DONT_OPTIM)
	CComQIPtr<IMgaSimpleConnection> cn( p_cn);
	if( !cn) return;

	connEnds_struct2 conn_info;
	cn->get_ParentModel( &conn_info.par);
	cn->get_MetaRole( &conn_info.mrole);
	cn->get_Src( &conn_info.src);
	cn->get_Dst( &conn_info.dst);
	cn->get_SrcReferences( &conn_info.srcRefs);
	cn->get_DstReferences( &conn_info.dstRefs);
	cn->get_Name( &conn_info.nm);
	cn->GetGuidDisp( &conn_info.guid);
	//getConnectionDetails( cn_i, conn_details.b_conn_struct);
	conns_to_recreate.push_back( conn_info);
#endif
	CComQIPtr<IMgaConnection> owner( p_cn);
	if( !owner) return;

	connEnds_struct one_conn;
	getConnectionDetails( owner, one_conn);

	// check if connection is (or precisely: was) secondary derived
	SEC_DER_NAP::const_iterator it = napOfSecondaryDeriveds.find( p_cn);
	if( it != napOfSecondaryDeriveds.end()) // found
	{
		ASSERT( !one_conn.is_derived);
		// when this occurs relations are already cut
		// that is why getConnectionDetails reported
		// that one_conn is NOT derived (is_derived=false)
		// but based on the napOfSecondaryDeriveds we can
		// confirm that it is derived, and the connection
		// will be saved altered into conns_stored
		one_conn.is_derived = true;
		one_conn.is_sec_derived = true;
		one_conn.baseConnId = it->second;
		one_conn.baseConnInLib = true;
	}

	// if sec derived from a project conn (non library conn)
	// then no need to store it, since its base will be recreated
	// and then the change propagated down into this place
	//if( one_conn.is_derived && one_conn.is_sec_derived && !one_conn.baseConnInLib)
	//	return;

	// even a sec-derd connection from library needs to be stored
	// because it needs to be deleted while the reference involved
	// is redirected, thus later needs restoration

	// examine if this connection has already been inserted into the vector
	if( conns_stored.end() == std::find_if( conns_stored.begin(), conns_stored.end(), HasThisConn(one_conn.connid))) // not found
		conns_stored.push_back( one_conn);

}

// obsolete, like saveConnection
void RefreshManager::restoreDeletedConns( CoreObj& nfolder)
{
#if(DONT_OPTIM)
	for( unsigned int i = 0; i < conns_to_recreate.size(); ++i)
	{
		connEnds_struct2 &curr_i = conns_to_recreate[i];
		try {
			CComPtr<IMgaFCO> resu;
			long c( 0);
			MyCComBSTR ms1;
			ms1.appendLink( ObjForCore(CoreObj(curr_i.src)));
			ms1.Append( L" ChainS ");
			curr_i.srcRefs->get_Count( &c);
			for( long i = 1; i <= c; ++i)
			{
				CComPtr<IMgaFCO> it;
				curr_i.srcRefs->get_Item( i, &it);
				CComQIPtr<IMgaReference> rit( it);
				
				ms1.appendLink( rit);
				ms1.Append( L" ");
			}
			m_reporter.show( ms1, false);

			MyCComBSTR ms2;
			ms2.appendLink( ObjForCore(CoreObj(curr_i.dst)));
			ms2.Append( L" ChainD ");
			curr_i.dstRefs->get_Count( &c);
			for( long i = 1; i <= c; ++i)
			{
				CComPtr<IMgaFCO> it;
				curr_i.dstRefs->get_Item( i, &it);
				CComQIPtr<IMgaReference> rit( it);
				
				ms2.appendLink( rit);
				ms2.Append( L" ");
			}
			m_reporter.show( ms2, false);

			/*COMTHROW( curr_i.par->CreateSimpleConn( curr_i.mrole, 
				curr_i.src, curr_i.dst, 
				curr_i.srcRefs, curr_i.dstRefs, &resu));*/
			COMTHROW( Creator::SimpleConn( curr_i.par, curr_i.mrole, 
				curr_i.src, curr_i.dst, 
				curr_i.srcRefs, curr_i.dstRefs, m_mgaproject, &resu));

			//COMTHROW( resu->put_Name( curr_i.nm));
			COMTHROW( Worker::put_Name( m_mgaproject, resu, curr_i.nm));
			//COMTHROW( resu->PutGuidDisp( curr_i.guid));
			COMTHROW( Worker::PutGuidDisp( m_mgaproject, resu, curr_i.guid));

			MyCComBSTR msg( "Succesfully recreated (prev. deleted) ");
			msg.appendLink( resu);
			msg.Append( L" in ");
			msg.appendLink( ObjForCore(CoreObj( curr_i.par)));
			m_reporter.show( msg, false);

		} catch( hresult_exception &) {

			MyCComBSTR msg( "Could not recreate ");
			msg.Append( curr_i.nm);
			msg.Append( L" (a deleted connection) in ");
			msg.appendLink( ObjForCore(CoreObj( curr_i.par)));
			m_reporter.show( msg);
		}
	}
#endif
}

// it differs from the putRefTask().DoWithDeriveds that it disregards the MasterObj attribute
// and it sets the same target for all Deriveds. 
// Instead of ATTRID_MASTEROBJ it operates with ATTRID_DERIVED
// TODO: improve parameter passing
bool RefreshManager::ignoreFurtherRedir( CComPtr<IMgaReference>& orig_ref, CComPtr<IMgaFCO>& ref_tgt_newlib)
{
	CoreObj one_ref = CoreObj( orig_ref);

	// will by skipped by the mapOfDependentRefs restoration
	m_alreadySetRefs.insert( one_ref);

	CoreObjs deriveds = one_ref[ATTRID_DERIVED + ATTRID_COLLECTION];
	ITERATE_THROUGH(deriveds) {
		CComPtr<IMgaFCO> fco;
		ObjForCore( ITER)->getinterface( &fco);
		CComQIPtr<IMgaReference> derd_ref( fco);
		if( !derd_ref) continue;

		//redirectRefWithCare( derd_ref, ref_tgt_newlib);

		ignoreFurtherRedir( derd_ref, ref_tgt_newlib); // recursive
	}
	return true;
}

bool RefreshManager::redirectRefWithCare( CComPtr<IMgaReference>& orig, CComPtr<IMgaFCO>& ref_tgt_newlib)
{
	ASSERT( orig);
	if( !orig) return false;

	// first check if it plays a role in some connections (thru reference ports)
	// which involve orig as a reference in the chain
	// a reference in such a case can't simply be deleted because the
	// redirecting is blocked by the the hanging connection points 

	CComPtr<IMgaConnPoints> cps;
	//COMTHROW( orig->get_PartOfConns( &cps)); // in case of a reference this does not give you back the refport conns
	COMTHROW( orig->get_UsedByConns( &cps));

	long c(0);
	if( cps) COMTHROW( cps->get_Count( &c));

	if( c <= 0) // in case of a reference which is not involved in any refchains for Connections
	{
		try {
			// redirect it simply
			//COMTHROW( orig->put_Referred( ref_tgt_newlib));
			CoreObj corig( orig);
			corig[ATTRID_REFERENCE] = CoreObj( ref_tgt_newlib);

			// success
			MyCComBSTR msg;
			msg.appendLink( orig);
			COMTHROW(msg.Append( L" stand alone reference retargeted to "));
			if( ref_tgt_newlib)
				msg.appendLink( ref_tgt_newlib);
			else
				COMTHROW(msg.Append( L"Null"));
			m_reporter.show( msg, false);

		} catch( hresult_exception&) {

			// failure
			MyCComBSTR msg;
			msg.appendLink( orig);
			COMTHROW(msg.Append( L" stand alone reference could not be retargeted to "));
			if( ref_tgt_newlib)
				msg.appendLink( ref_tgt_newlib);
			else
				COMTHROW(msg.Append( L"Null"));
			m_reporter.show( msg);
		}
	}
	else
	{
		//
		// we need to remove all connpoints which are attached to refports or orig
		// because 
		// but first we store them into coll_of_conns
		CComPtr<IMgaFCOs> coll_of_conns;
		COMTHROW( coll_of_conns.CoCreateInstance( L"Mga.MgaFCOs"));

		for( long i = 1; i <= c; ++i) 
		{
			CComPtr<IMgaConnPoint> cp;
			COMTHROW( cps->get_Item( i, &cp));
			if( cp)
			{
				CComPtr<IMgaConnection> cn;
				COMTHROW( cp->get_Owner( &cn));

				// if there is a loop connection and we destroy it
				// the connpoint will die along with it, so when
				// encountering the next connpoint belonging to this
				// connection will raise null ptr exception, because that
				// connpoint is 0 already
				//COMTHROW( cn->DestroyObject());
				long pos = 0;
				coll_of_conns->Find( cn, 1, &pos); // ignore failure
				if( pos < 1)
				{
					COMTHROW(coll_of_conns->Append( cn));
					// save it
					saveConnection( CComPtr<IMgaFCO>(cn));
				}
			}
		}

		//
		// now really remove the collected connections
		// 
		long conns_len = 0;
		if( coll_of_conns) COMTHROW( coll_of_conns->get_Count( &conns_len));
		for( long i = 1; i <= conns_len; ++i)
		{
			CComPtr<IMgaFCO> cn_i;
			COMTHROW( coll_of_conns->get_Item( i, &cn_i));

			// delete the connection
			try {
#ifdef _DEBUG
				long ci = CoreObj(cn_i)[ATTRID_RELID];
#endif
				//COMTHROW( cn_i->DestroyObject());
				ObjForCore( CoreObj(cn_i))->inDeleteObject();


				MyCComBSTR msg;
				COMTHROW(msg.Append( L"Connection "));
				msg.appendLink( cn_i);
				COMTHROW(msg.Append( L" deleted while retargeting reference "));
				msg.appendLink( orig);
				m_reporter.show( msg, false);

			} catch(hresult_exception&) {

				MyCComBSTR msg;
				COMTHROW(msg.Append( L"Connection "));
				msg.appendLink( cn_i);
				COMTHROW(msg.Append( L" could not be deleted while retargeting reference "));
				msg.appendLink( orig);
				m_reporter.show( msg);
			}
		}
		
		//
		// retarget the orig reference into the newlibrary
		//
		try {
			//COMTHROW( orig->put_Referred( ref_tgt_newlib));
			CoreObj corig( orig);
			corig[ATTRID_REFERENCE] = CoreObj( ref_tgt_newlib);

			// success
			MyCComBSTR msg;
			msg.appendLink( orig);
			COMTHROW(msg.Append( L" retargeted to "));
			msg.appendLink( ref_tgt_newlib);
			m_reporter.show( msg, false);

		} catch( hresult_exception& ) {

			// failure
			MyCComBSTR msg;
			msg.appendLink( orig);
			COMTHROW(msg.Append( L" could not be retargeted to "));
			msg.appendLink( ref_tgt_newlib);
			m_reporter.show( msg);
		}
	}

	CoreObj corig( orig);
	// remember that we already did retarget this corig object
	//m_alreadySetRefs.insert( corig);

	// propagate the target onto the deriveds objects
	CoreObjs deriveds = corig[ATTRID_DERIVED + ATTRID_COLLECTION];
	ITERATE_THROUGH(deriveds) {
		CComPtr<IMgaFCO> fco;
		ObjForCore( ITER)->getinterface( &fco);
		CComQIPtr<IMgaReference> derd_ref( fco);
		if( !derd_ref) continue;

		redirectRefWithCare( derd_ref, ref_tgt_newlib);// recursive
	}

	return true;
}

void RefreshManager::restoreRefsToLib()
{
	//
	// first do the archetype references
	for( REFERENCES_MAP::iterator it = mapOfReferences.begin();
		it != mapOfReferences.end(); ++it)
	{
		const UniqueId &guid_of_target = it->first;
		CORE_REFERENCES_VEC &refs     = it->second;
		
		CComPtr<IMgaFCO> lib_target = SearchTool::findLibObj( m_newLib, guid_of_target);
		if( !lib_target)
		{
			// report error:
			// target element of ref not found in library
			lib_target = SearchTool::findLibObj( m_oldLib, guid_of_target);
			MyCComBSTR msg( "Reference target "); 
			if( lib_target)
				msg.appendLink( lib_target);
			else
				msg.appendGuid( guid_of_target);
			COMTHROW(msg.Append( L" not found in new library!"));
			m_reporter.show( msg);

			continue;
		}

		
		MyCComBSTR mm;
		COMTHROW(mm.Append( L"Redirect refs to target: "));
		mm.appendLink( lib_target);
		m_reporter.show( mm, false);
		for( CORE_REFERENCES_VEC::iterator jt = refs.begin();
			jt != refs.end(); ++jt)
		{
			CComPtr<IMgaFCO> elem_in_proj;
			ObjForCore( *jt)->getinterface( &elem_in_proj);
			CComPtr<IMgaReference> ref_in_proj;
			HRESULT res = E_FAIL;
			long l = OBJECT_DELETED;

			if( elem_in_proj) 
			{
				res = elem_in_proj.QueryInterface( &ref_in_proj);
				if( ref_in_proj) 
					res = ref_in_proj->get_Status( &l);
			}

			if( SUCCEEDED( res) && ref_in_proj && l == OBJECT_EXISTS) // if we have a valid reference
			{
				redirectRefWithCare( ref_in_proj, lib_target);
			}
			else
			{
				// report error
				// elem_in_proj is not a reference any more
				// or is not found any more in the project,
				// however before RefreshLibrary it was a reference
				
				//MyCComBSTR msg("Could not find "); msg.appendLink( id_of_proj_elem, "Reference"); msg.Append( L" in project!");
				ASSERT(0);
				MyCComBSTR msg( "Could not find reference "); 
				msg.appendLink( elem_in_proj);
				COMTHROW(msg.Append( L" in project!"));
				m_reporter.show( msg);
			}
		}
	}

	//
	// now do the derd references (for these order matters)
	for( DEP_REFERENCES_MAP::iterator it = mapOfDependentReferences.begin();
		it != mapOfDependentReferences.end(); ++it)
	{
		// the key pair consists of the distance of the target (from its archetype) and the target uid
		// so that those references are set first which have archetype targets, or
		// at least higher level targets in a derivation hierarchy
		//const short dist = it->first.first;
		const UniqueId &guid_of_target = it->first.second;
		CORE_REFERENCES_SET &refs     = it->second;
		
		CComPtr<IMgaFCO> lib_target = SearchTool::findLibObj( m_newLib, guid_of_target);
		if( !lib_target)
		{
			// report error:
			// element with guid not found in library
			lib_target = SearchTool::findLibObj( m_oldLib, guid_of_target);
			MyCComBSTR msg( "Reference target "); 
			if( lib_target) msg.appendLink( lib_target);
			else            msg.appendGuid( guid_of_target);
			COMTHROW(msg.Append( L" not found in new library!"));
			m_reporter.show( msg);

			continue;
		}

		// lib_target is the new intended target for refs
		for( CORE_REFERENCES_SET::iterator jt = refs.begin();
			jt != refs.end(); ++jt)
		{
			if( m_alreadySetRefs.find( jt->second) != m_alreadySetRefs.end()) // found means already redirected/set
				continue;

			CComPtr<IMgaFCO> elem_in_proj;
			ObjForCore( jt->second)->getinterface( &elem_in_proj);

			CComQIPtr<IMgaReference> ref_in_proj( elem_in_proj);
			
			if( ref_in_proj) // if we have a valid reference
			{
				// is ref derived from library
				// or hosting project
				redirectRefWithCare( ref_in_proj, lib_target);
			}
			else
			{
				// report error
				// elem_in_proj is not a reference any more
				// or is not found any more in the project,
				// however before RefreshLibrary it was a reference
				ASSERT(0);
				MyCComBSTR msg( "Could not find "); 
				msg.appendLink( elem_in_proj);
				COMTHROW(msg.Append( L" in project!"));
				m_reporter.show( msg);
			}
		}
	}

	mapOfReferences.clear();
}

//typedef std::pair< UniqueId, UniqueId >               LIBREF_AND_LIBTARGET;
//typedef std::pair< short, LIBREF_AND_LIBTARGET >      LIB_REFTARG_PAIR;
//// if target is in library then UniqueId identifies it
//// otherwise the CoreObj is the target
//typedef std::pair< UniqueId, CoreObj >                MIXED_TARGET;
//// CoreObj is the regular reference in the project, mixedtarget is its target
//typedef std::pair< CoreObj, MIXED_TARGET>             REGREF_AND_TARGET;
//typedef std::vector< REGREF_AND_TARGET >              CORE_DERD_REF_VEC;
//typedef std::map< LIB_REFTARG_PAIR, CORE_DERD_REF_VEC > ADAPTIVE_REFERENCES_MAP;
void RefreshManager::adaptDerRefs()
{
	for( SPECIAL_MAP::iterator it = mapOfDerdRefsFromAdaptiveRef.begin();
		it != mapOfDerdRefsFromAdaptiveRef.end(); ++it)
	{
		const DIST_AND_REFOBJ  &ref_info = it->first;
		MIXED_TARGET2          &tgt_info = it->second;
		
		const CoreObj          &ref_core = ref_info.second;
		const UniqueId         &tgt_guid = tgt_info.first;
		const CoreObj          &tgt_core = tgt_info.second;

		if( m_alreadySetRefs.find( ref_core) != m_alreadySetRefs.end()) // found means already redirected/set
			continue;

		// target
		CComPtr<IMgaFCO> target;

		if( tgt_guid == UniqueId()) // ptr tgt_core must be valid
		{
			ObjForCore( tgt_core)->getinterface( &target);
		}
		else
		{
			target = SearchTool::findLibObj( m_newLib, tgt_guid);
		}

		// ref
		CComPtr<IMgaFCO> t_ref;
		ObjForCore( ref_core)->getinterface( &t_ref);
		CComQIPtr<IMgaReference> ref( t_ref);

		if( ref)
			redirectRefWithCare( ref, target);
	}
}

void RefreshManager::adaptTopRefs()
{
	for( ADAPTIVE_REFERENCES_MAP::iterator it = mapOfAdaptiveReferences.begin();
		it != mapOfAdaptiveReferences.end(); ++it)
	{
		// the key pair consists of the distance of the target (from its archetype) and the target uid
		// so that those references are set first which have archetype targets, or
		// at least higher level targets in a derivation hierarchy
		const LIB_REFTARG_PAIR  &base = it->first;
		CORE_DERD_REF_VEC       &refs = it->second;
		//short dist = base.first;
		UniqueId base_id = base.second.first;
		UniqueId targ_id = base.second.second;
		
		// finding the base reference
		CComPtr<IMgaFCO> lib_refern = SearchTool::findLibObj( m_newLib, base_id);
		if( lib_refern) 
		{
			CComQIPtr<IMgaReference> lib_r( lib_refern);
			if( lib_r)
			{
				CComPtr<IMgaFCO> tgt;
				COMTHROW( lib_r->get_Referred( &tgt));
				UniqueId frsh_tgt;
				if( tgt) // fresh version of base_ref has a target
				{
					frsh_tgt = Identifier::getDetailsOf( CoreObj( tgt));
					if( frsh_tgt == targ_id) //base's referral did not change
					{
						// nop, we are satisfied with restoreRefsToLib's job
						// we need to redirect safely some of the derived refs
						// something similar to restoreRefsToLib
						for( CORE_DERD_REF_VEC::iterator jt = refs.begin();
							jt != refs.end(); ++jt)
						{
							CoreObj       &ref2        = jt->first;
							MIXED_TARGET  &target_info = jt->second;

							CComPtr<IMgaFCO> elem_in_proj;
							ObjForCore( ref2)->getinterface( &elem_in_proj);
							CComQIPtr<IMgaReference> ref_in_proj( elem_in_proj);

							UniqueId &derd_tgt_uid        = target_info.first;
							CoreObj  &derd_tgt_coreobj    = target_info.second;

							if( derd_tgt_uid.libId != BinGuid()) 
							{ // lookup the target in the new version of the library
								CComPtr<IMgaFCO> der_target = SearchTool::findLibObj( m_newLib, derd_tgt_uid);
								redirectRefWithCare( ref_in_proj, der_target);
							}
							else if( derd_tgt_coreobj) // the target was a project object
							{
								// although the target object survived the refresh we need to set 
								// back again for ref_in_proj because its ancestors have been
								// reset, thus its settings have been overwritten
								CComPtr<IMgaFCO> der_target;
								ObjForCore( derd_tgt_coreobj)->getinterface( &der_target);
								redirectRefWithCare( ref_in_proj, der_target);
							}
							else ASSERT(0);
							
						}
					}
					else // base redirected, dependents need redirection too
					{
						if( targ_id == UniqueId()) {} // it was a null ref, easier (set happens)
						else {} // old version of ref was referring somewhere (redirection happens)
						
						// same job:
						for( CORE_DERD_REF_VEC::iterator jt = refs.begin();
							jt != refs.end(); ++jt)
						{
							CoreObj &ref2 = jt->first;
							CComPtr<IMgaFCO> elem_in_proj;
							ObjForCore( ref2)->getinterface( &elem_in_proj);
							CComQIPtr<IMgaReference> ref_in_proj( elem_in_proj);

							redirectRefWithCare( ref_in_proj, tgt);
							// brutal redirect happeened, mark all dependends of ref_in_proj
							ignoreFurtherRedir( ref_in_proj, tgt);
						}
					}
				}
				else // null-ref
				{
					// propagate down the null-referenceness only if:
					if( !(targ_id == UniqueId())) // old version of ref was referring somewhere
					{
						for( CORE_DERD_REF_VEC::iterator jt = refs.begin();
							jt != refs.end(); ++jt)
						{
							CoreObj &ref2 = jt->first;
							CComPtr<IMgaFCO> elem_in_proj;
							ObjForCore( ref2)->getinterface( &elem_in_proj);
							CComQIPtr<IMgaReference> ref_in_proj( elem_in_proj);

							// redirect to 0
							redirectRefWithCare( ref_in_proj, CComPtr<IMgaFCO>());
							// brutal redirect happeened, mark all dependends of ref_in_proj
							ignoreFurtherRedir( ref_in_proj, CComPtr<IMgaFCO>());
						}
					}
					// else : not needed, neither old nor new version point anywhere
				}
			}
			else
			{
				ASSERT(0);
			} // non-reference object has the same uid?
		}
		else
		{
		} // not found base in newlib, so the deriveds already are(/will be) deleted
	}
}

void RefreshManager::updateMatchDb( coreobjpairhash& p_creaList)
{
	coreobjpairhash::iterator it  = p_creaList.begin();
	coreobjpairhash::iterator end = p_creaList.end();
	for( ; it != end; ++it)
	{
		const CoreObj& oi = it->first;
		const CoreObj& ni = it->second;

		// important
		PROJ_ID dr_id, bs_id;
		ObjForCore(ni)->get_ID( &dr_id);
		ObjForCore(oi)->get_ID( &bs_id);
#if(DONT_OPTIM)
		m_matchMaker[ dr_id ] = bs_id;
#endif
		m_coreMatchMaker[ ni ] = oi;

#if(0)
		//important, otherwise how the connections will be found
		//UniqueId bs = Identifier::getDetailsOf( oi);
		//mapOfSecondaryDeriveds[ dr_id ] = bs;
#endif
	}
}

void RefreshManager::copyNewArrivals( CoreObj& p_adaptiveObj, std::vector< CoreObj>& p_newComers
									 , SET_CREALIST& p_sets, REF_CREALIST& p_refs)
{
	int targetlevel = 0;
	unsigned int cnt = (unsigned int)p_newComers.size();
	CoreObj rootp;
	GetRootOfDeriv( p_adaptiveObj, rootp, &targetlevel);

	coreobjpairhash crealist;
	std::vector<CoreObj> nobjs( cnt);
	for( unsigned int i = 0; i < cnt; ++i)
	{
		CoreObj oldobj = p_newComers[i];
		int derdist = GetRealSubtypeDist( oldobj);
		ObjTreeCopy( m_mgaproject, oldobj, nobjs[i], crealist);  // copy
		if( derdist) ObjTreeDist( nobjs[i], derdist);
		// assigns a new relid to nobjs[i]
		ObjForCore( p_adaptiveObj)->assignnewchild( nobjs[i]);

		// collect the sets (on the topmost level only)
		if( oldobj.GetMetaID() == DTID_SET)
			p_sets.push_back( std::make_pair( nobjs[i], oldobj));
		else if( oldobj.GetMetaID() == DTID_REFERENCE)
			p_refs.push_back( std::make_pair( nobjs[i], oldobj));
	}

	for( unsigned int i = 0; i < cnt; ++i)
	{
		ObjTreeReconnect( nobjs[i], crealist);
	}

	// important!
	// update our matchinfo storage
	// (used later to recreate connections)
	updateMatchDb( crealist);

	coreobjhash newcrealist;
	shiftlist(crealist, newcrealist);

	for( unsigned int i = 0; i < cnt; ++i)
	{
		ObjTreeCheckRelations( m_mgaproject, nobjs[i], newcrealist);
	}

	if(targetlevel >= 0) ReDeriveNewObjs( m_mgaproject, nobjs, cnt, targetlevel+1);
}

// let's remove those children of adaptiveobj which were once derived
// from the master's children, but now we face that the master has been
// simplified (some of its children are not present any more in it)
// (Subtypes may contain additional children, those are not affected.)
// By using the mapOfSecondaryDeriveds we decide the was-it-once-derived? question
void RefreshManager::removeObsoleteChildren( const CoreObj& masterObj, CoreObj& adaptiveobj)
{
	CoreObjs children = adaptiveobj[ATTRID_FCOPARENT + ATTRID_COLLECTION];
	ITERATE_THROUGH(children) {
		PROJ_ID my_id;
		ObjForCore(ITER)->get_ID( &my_id);
		CComPtr<IMgaFCO> iter_fco;
		ObjForCore(ITER)->getinterface( &iter_fco);

		SEC_DER_NAP::iterator secder_jterator = napOfSecondaryDeriveds.find( iter_fco);
#if(DONT_OPTIM)
		SEC_DER_MAP::iterator secder_iterator = mapOfSecondaryDeriveds.find( my_id );
		if( secder_iterator != mapOfSecondaryDeriveds.end()) // it is (actually was) a sec derived
#endif
		if( secder_jterator != napOfSecondaryDeriveds.end()) // it is (actually was) a sec derived
		{
			ASSERT( secder_jterator != napOfSecondaryDeriveds.end());

			bool valid = true;
			if( m_coreMatchMaker.find( ITER) == m_coreMatchMaker.end())
			{
				// ITER seems to be an object whose base no longer exists in the fresh library
				valid = false;
			}
#if(DONT_OPTIM)
			if( m_matchMaker.find( my_id) == m_matchMaker.end())
			{
				// ITER seems to be an object whose base no longer exists in the fresh library
				if( valid) ASSERT(0); // TODO: is m_matchMaker needed at all? this line will detect it
				valid = false;
			}
#endif
			if( !valid) // need to delete it
			{
//#ifdef _DEBUG
				MyCComBSTR msg;
				COMTHROW(msg.Append( L"Orphan child "));
				msg.appendLink( ObjForCore(ITER));
				COMTHROW(msg.Append( L" deleted."));
				m_reporter.show( msg, false);
//#endif
				ObjForCore(ITER)->inDeleteObject();
			}
		}
		else
			ASSERT( secder_jterator == napOfSecondaryDeriveds.end());
	}
}

void RefreshManager::SyncDerSets( const CoreObj &p_frMasterSet, CoreObj &p_adaptiveSet
                                , const CoreObj &p_frMasterPar, CoreObj &p_adaptivePar)
{
	CComPtr<IMgaModel> masterParent, adaptvParent;
	ObjForCore( p_frMasterPar)->getinterface( &masterParent);
	ObjForCore( p_adaptivePar)->getinterface( &adaptvParent);
	CComPtr<IMgaSet> masterSet, adaptvSet;
	ObjForCore( p_frMasterSet)->getinterface( &masterSet);
	ObjForCore( p_adaptiveSet)->getinterface( &adaptvSet);

	CComPtr<IMgaFCOs> membs;
	COMTHROW( masterSet->get_Members( &membs));
	long c = 0;
	if( membs) COMTHROW( membs->get_Count( &c));
	for( long i = 1; i <= c; ++i)
	{
		CComPtr<IMgaFCO> b_memb, d_memb;
		COMTHROW( membs->get_Item( i, &b_memb));
		getLibDerdEquiv( b_memb, masterParent, adaptvParent, d_memb);
		
		if( d_memb  // if member found and it is a sybling of the set (must be)
			&& COM_EQUAL( CoreObj(p_adaptiveSet[ATTRID_FCOPARENT]), CoreObj(CoreObj(d_memb)[ATTRID_FCOPARENT])))
		{
			COMTHROW( adaptvSet->AddMember( d_memb));
		}
		else
		{
			MyCComBSTR msg;
			COMTHROW(msg.Append( L"Could not find derived peer of "));
			msg.appendLink( b_memb);
			COMTHROW(msg.Append( L" in order to add as a member in "));
			msg.appendLink( adaptvSet);
			m_reporter.show( msg);
		}
	}
}

void RefreshManager::SyncDerRefs( const CoreObj &p_frMasterRef, CoreObj &p_adaptiveRef
                                , const CoreObj &p_frPrimaryMasterPar, const CoreObj &p_primaryAdaptivePar)
{
	CComPtr<IMgaModel> masterParent, adaptvParent;
	ObjForCore( p_frPrimaryMasterPar)->getinterface( &masterParent);
	ObjForCore( p_primaryAdaptivePar)->getinterface( &adaptvParent);
	CComPtr<IMgaReference> masterRef, adaptvRef;
	ObjForCore( p_frMasterRef)->getinterface( &masterRef);
	ObjForCore( p_adaptiveRef)->getinterface( &adaptvRef);

	CComPtr<IMgaFCO> b_tgt, d_tgt;
	COMTHROW( masterRef->get_Referred( &b_tgt));
	if( b_tgt) 
	{
		getLibDerdEquiv( b_tgt, masterParent, adaptvParent, d_tgt, false); // no limitation if parent, grandparent ...
		if( d_tgt)
		{
			COMTHROW( adaptvRef->put_Referred( d_tgt));
		}
		else
		{
			MyCComBSTR msg;
			COMTHROW(msg.Append( L"Could not find derived peer of "));
			msg.appendLink( b_tgt);
			COMTHROW(msg.Append( L" in order to set as target of "));
			msg.appendLink( adaptvRef);
			m_reporter.show( msg);
		}
	}
}

void RefreshManager::SyncObjectName( const CoreObj& p_masterObj, CoreObj &p_adaptiveObj)
{
	// ITER must be a secondary derived, so propagate name change down
	CComBSTR masters_name = p_masterObj[ATTRID_NAME];
	CComBSTR adaptivs_name = p_adaptiveObj[ATTRID_NAME];
	if( adaptivs_name != masters_name) {
		//put_NameTask( adaptivs_name, masters_name).DoWithDeriveds( p_adaptiveObj);
		ObjForCore( p_adaptiveObj)->put_Name( masters_name);
	}
}
// masterobj is the fresh base
// adaptiveobj is the (previously detached) derived object
// these two must be synchronized
// since the method is used recursively we have 
// priMasterObj as the primary base
// priAdaptiveObj as the primary derived (previously detached)
void RefreshManager::SyncDerObjs( const CoreObj &masterobj, CoreObj &adaptiveobj
								 , long instance, bool prim
								 , const CoreObj &priMasterObj, const CoreObj &priAdaptiveObj)
{
	// this is a variable which can change the behaviour when
	// a subtype is attached to a changed (simplified) basetype:
	//  true: any secondary derived object which now due to 
	//        simplification remained without base (its base
	//        was deleted from the primary basetype) will be deleted
	//  false: the old secondary derived objects now without base
	//         will be altered such that they will become archetypes
	//         in the containing subtype
	// static bool basetype_simplification_propagated_down = true;

	// this will hold those objects which have appeared in the new version of the library
	std::vector< CoreObj > newcomers;

	CComPtr<ICoreAttributes> atts;
	COMTHROW(masterobj->get_Attributes(&atts));

	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));

			if(ai >= ATTRID_COLLECTION) {
				ai -= ATTRID_COLLECTION;
				if(LINKREF_ATTR(ai))  {
					if( ai == ATTRID_ATTRPARENT)       { } // don't care
					else if( ai == ATTRID_CONSTROWNER) { } // don't care
					else if( ai == ATTRID_CONNROLE)    { } // manually propagated, see collectFreshConnection below
					else if( ai == ATTRID_SETMEMBER)   { } // manually set, see SyncDerSets below
					else if( ai == ATTRID_FCOPARENT)
					{
						{CoreObjs new_base_children = masterobj[ai + ATTRID_COLLECTION];
						ITERATE_THROUGH(new_base_children) {
							metaid_type meta_id = ITER.GetMetaID();
	
							// acquire id and uid
							PROJ_ID bs_id;
							ObjForCore(ITER)->get_ID( &bs_id);
							UniqueId bs_uid = Identifier::getDetailsOf( ITER);

							// current original object held in outer_ITER
							// actually is an element of the fresh basetype
							// whose fresh inner elements need to be propagated 
							// into adaptiveobj (a previously detached derdtype)
							CoreObj outer_ITER = ITER;
							bool fnd = false;
							{CoreObjs detached_derd_children = adaptiveobj[ai + ATTRID_COLLECTION];
							ITERATE_THROUGH(detached_derd_children) {
								// acquire id
								PROJ_ID dr_id;
								ObjForCore(ITER)->get_ID( &dr_id);
								CComPtr<IMgaFCO> iter_fco;
								ObjForCore(ITER)->getinterface( &iter_fco);
								
								// check if ITER (adaptiveobj's child) was a previously
								// dependent object (a secondary derived) or just an object
								// inserted as an archetype (possible only in subtypes)
								SEC_DER_NAP::iterator secder_jterator = napOfSecondaryDeriveds.find( iter_fco);
#if(DONT_OPTIM)
								SEC_DER_MAP::iterator secder_iterator = mapOfSecondaryDeriveds.find( dr_id );
								if( secder_iterator != mapOfSecondaryDeriveds.end()) // yes, it was a sec derived
#endif
								if( secder_jterator != napOfSecondaryDeriveds.end()) // yes, it was a sec derived
								{
									ASSERT( secder_jterator != napOfSecondaryDeriveds.end());
									// uid of old-base stored in VALUE
									UniqueId mine_bs_uid;
#if(DONT_OPTIM)
									mine_bs_uid = secder_iterator->second;
#endif
									mine_bs_uid = secder_jterator->second;

									if( bs_uid == mine_bs_uid                             // ITER was derived from outer_ITER before detach
										&& outer_ITER[ATTRID_META] == ITER[ATTRID_META])  // in the same role
									{
#if(DONT_OPTIM)
										m_matchMaker[ dr_id ] = bs_id;
#endif
										m_coreMatchMaker[ ITER] = outer_ITER;
										
										// for sec derd objects names are synchronized
										SyncObjectName( outer_ITER, ITER);

										if( meta_id == DTID_MODEL)//!meta_id == DTID_CONNECTION
										{
											SyncDerObjs( outer_ITER, ITER, instance, false, priMasterObj, priAdaptiveObj);
										}   // nothing to do for connections since they have no inner objects

										fnd = true;
										break;
									}
								}
								else
									ASSERT( secder_jterator == napOfSecondaryDeriveds.end());
							}}
							
							if( !fnd) // fresh object in basetype
							{
								if( meta_id == DTID_CONNECTION)
									collectFreshConnection( ITER, masterobj, adaptiveobj);
								else
									newcomers.push_back( ITER);
							}
						}}

						//do it anyway
						//if( basetype_simplification_propagated_down)
						removeObsoleteChildren( masterobj, adaptiveobj);
					}
				}
			}

	} MGACOLL_ITERATE_END;

	if( newcomers.size() > 0)
	{
		// will contain the sets which are strictly children of adaptiveobj (excludes granchildren, ...)
		// the inner sets will be copied along with their members, thus they don't need special care
		SET_CREALIST set_pairs; REF_CREALIST ref_pairs;
		copyNewArrivals( adaptiveobj, newcomers, set_pairs, ref_pairs);
		
		// if any set copied restore their members right now
		for( unsigned int i = 0; i < set_pairs.size(); ++i)
		{
			//call SyncSets with (base_set  , derd_set          , base_parent, derd_parent)
			SyncDerSets( set_pairs[i].second, set_pairs[i].first, masterobj, adaptiveobj);
		}

		// if refs
		for( unsigned int i = 0; i < ref_pairs.size(); ++i)
		{
			SyncDerRefs( ref_pairs[i].second, ref_pairs[i].first, priMasterObj, priAdaptiveObj);
		}
	}
}

void RefreshManager::syncStructureFromLib()
{
	for( SUB_PAR_MAP::iterator it = mapOfDeriveds.begin();
		it != mapOfDeriveds.end(); ++it)
	{
		const UniqueId &guid_of_base   = it->first;
		SUB_PAR_VEC    &pair_vec       = it->second;

		CComPtr<IMgaFCO> lib_base = SearchTool::findLibObj( m_newLib, guid_of_base);
		if( lib_base) // new version of the object found in new library
		{
			for( SUB_PAR_VEC::iterator jt = pair_vec.begin();
				jt != pair_vec.end(); ++jt)
			{
				CComPtr<IMgaFCO> &prev_detached_obj = jt->subt_ptr;

				long st(-1);
				HRESULT hr = E_FAIL;
				if( prev_detached_obj) hr = prev_detached_obj->get_Status( &st);
				//CComPtr<IMgaFCO> prev_detached_obj = SearchTool::findRegularObj( m_mgaproject, nfolder, jt->subt_id);
				//if( prev_detached_obj)
				if( SUCCEEDED( hr) && st == OBJECT_EXISTS && prev_detached_obj) // a valid object ptr
				{
					CoreObj prev_detached( prev_detached_obj);
					CoreObj frsh_base( lib_base);
					
					// sync structure of fresh base with detached derive
					// the last two parameters helps in redirection of the internal references
					SyncDerObjs( frsh_base, prev_detached, 0, true, frsh_base, prev_detached);
					
					PROJ_ID b_id, d_id;
					COMTHROW( lib_base->get_ID( &b_id));
					COMTHROW( prev_detached_obj->get_ID( &d_id));
					// store the match info
					m_primMatchMaker[ prev_detached] = std::make_pair( frsh_base, jt->is_instance);
					m_coreMatchMaker[ prev_detached] = frsh_base;
#if(DONT_OPTIM)
					m_matchMaker[ d_id ] = b_id;
#endif

					MyCComBSTR msg;
					msg.appendLink( ObjForCore(frsh_base));
					COMTHROW(msg.Append( L" derived to "));
					msg.appendLink( ObjForCore(prev_detached));
					m_reporter.show( msg, false);
				}
				else
				{
					ASSERT(0); // error
					MyCComBSTR msg(" Could not find detached object "); msg.appendLink( jt->subt_ptr);
					COMTHROW(msg.Append( L" while trying to re-attach to base!"));
					m_reporter.show( msg);
				}
			}
		}
		else
		{
#ifdef _DEBUG
			// report library backroll: lib_base not found in new library
			CComPtr<IMgaFCO> old_lib_base = SearchTool::findLibObj( m_oldLib, guid_of_base);
			MyCComBSTR msg("Archetype "); 
			if( old_lib_base) msg.appendLink( old_lib_base);
			else              msg.appendGuid( guid_of_base);
			msg.Append( L" not found in refreshed library.");
			m_reporter.show( msg);
#endif
			// lib_base not found -> delete all existing primary (and secondary?) subtypes
			for( SUB_PAR_VEC::iterator jt = pair_vec.begin();
				jt != pair_vec.end(); ++jt)
			{
				//CComPtr<IMgaFCO> prev_detached_obj = SearchTool::findRegularObj( m_mgaproject, m_newLib, jt->subt_id);
				CComPtr<IMgaFCO> &prev_detached_obj = jt->subt_ptr;
				if( prev_detached_obj)
				{
//#ifdef _DEBUG
					MyCComBSTR msg;
					msg.appendLink( prev_detached_obj);
					COMTHROW(msg.Append( L" deleted as "));
					if( jt->is_instance)
						COMTHROW(msg.Append( L"an instance"));
					else
						COMTHROW(msg.Append( L"a subtype"));
					COMTHROW(msg.Append( L" of an obsolete library object."));
					m_reporter.show( msg, false);
//#endif
					ObjForCore(CoreObj(prev_detached_obj))->inDeleteObject();
				}
			}
		}
	}
}

void RefreshManager::reapplyLibFlagToChangedLibs()
{
	ASSERT( m_mgaproject);
	if( !m_mgaproject) return;

	CComBSTR id; // the refreshed library's id
	COMTHROW( ObjForCore( m_newLib)->get_ID( &id));

	CComBSTR idlist;
	Ozer::StorageMgr::getIncludedBy( m_oldLib, idlist);
	for( Ozer::DependentIterator it( idlist); !it.isDone(); it.next())
	{
		CoreObj libroot;
		CComPtr<IMgaFolders> coll;
		COMTHROW( m_mgaproject->GetTopLibraries( it.getCurrentBstr(), &coll));
		// this lib may have gotten new elements during refresh, that's why
		// for all such libs reapply the LIB flag onto the hierarchy
		long len( 0);
		if( coll) COMTHROW( coll->get_Count( &len));
		for( long i = 1; i <= len; ++i)
		{
			CComPtr<IMgaFolder> ele;
			COMTHROW( coll->get_Item( i, &ele));
			applyLibFlag( CoreObj( ele));
		}
	}
}

void RefreshManager::applyLibFlag( CoreObj& p_libObj)
{
	// for all children
	CoreObjs kids = p_libObj[ATTRID_FCOPARENT + ATTRID_COLLECTION];
	ITERATE_THROUGH(kids) {
		ITER[ATTRID_PERMISSIONS] = ITER[ATTRID_PERMISSIONS] | LIBRARY_FLAG;

		if( ITER.IsContainer()) applyLibFlag( ITER);
	}
}

void RefreshManager::propObjRelidIncV2( CoreObj& p_obj)
{
	long brelid = p_obj[ATTRID_RELID];

	// for all deriveds from p_obj
	CoreObjs deriveds = p_obj[ATTRID_DERIVED + ATTRID_COLLECTION];
	ITERATE_THROUGH(deriveds) {
		// increase depth
		ITER[ATTRID_RELID] = brelid + RELIDSPACE;

		propObjRelidIncV2( ITER);
	}
}

void RefreshManager::DetachObjFromLibBase( const CoreObj& p_baseObj, CoreObj& p_derdObj, unsigned long nextrelid, bool prim)
{
	bool has_last_relid_attr = false;
	long last_relid_set = 0;
	long max_relid_found= 0;
#ifdef _DEBUG
	char p[151];p[0] = 0;
#endif

	CComPtr<ICoreAttributes> atts;
	COMTHROW(p_baseObj->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));
			if(ai < ATTRID_COLLECTION) {
				switch(ai) {
					case ATTRID_GUID1:
					case ATTRID_GUID2:
					case ATTRID_GUID3:
					case ATTRID_GUID4: 
						break; // do not copy from base these values

					case ATTRID_DERIVED: 
					case ATTRID_MASTEROBJ:
					{
						p_derdObj[ai] = CComVariant( (IDispatch*) 0);//an empty value;
						break;
					}
					case ATTRID_PERMISSIONS:
					{
						//if( p_derdObj[ai] & INSTANCE_FLAG) //if INSTANCE_FLAG present
						//	p_derdObj[ai] = (p_baseObj[ai]) & ~INSTANCE_FLAG;//then INSTANCE_FLAG removed;
						
						//do not copy the permissions from the base
						p_derdObj[ai] = 0;
						break;
					}
					case ATTRID_RELID:
					{
						break;
					}
					case ATTRID_LASTRELID:
					{
						// old val contains the number of the plain objects only
						last_relid_set = p_derdObj[ai];
						has_last_relid_attr = true;
						break;
					}

					default:
						break; // no copy/change needed in other plain cases

				} // endswitch
			}
			else if( LINKREF_ATTR(ai-ATTRID_COLLECTION)){
				ai -= ATTRID_COLLECTION;

				switch( ai) {
					case ATTRID_CONSTROWNER:        // the subobjects which are owned by p_derdObj
					case ATTRID_ATTRPARENT:         // p_baseObj's subobjects will be forgotten
                            break;                  // and when the new version is reattached
                                                    // then those will be propagated down to p_derdObj
                                                    // automatically by the derivation relationship

					case ATTRID_FCOPARENT: // for all secondary derived fco children-> detach
					{
						// we will calc the max_relid of those child objects which are not secondary, tertiary, ... objects
						// because in case of subtypes not all children originate from the current base:
						// some children might be plain objects, and some others may be primary subtypes of other bases
						//long cur_max_relid = p_derdObj[ATTRID_LASTRELID];
						CoreObjs children = p_derdObj[ai + ATTRID_COLLECTION];
						ITERATE_THROUGH(children) {
							CoreObj base = (ITER)[ATTRID_DERIVED];
							if( base) // child is also derived
							{
								//MyCComBSTR m;
								//m.appendLink( ObjForCore( base));
								//m.Append( L" <--- ");
								//m.appendLink( ObjForCore( ITER));
								//m_reporter.show( m);

								VARIANT_BOOL primDer = VARIANT_FALSE;
								if( p_derdObj.IsFCO())
								{
									ObjForCore( ITER)->get_IsPrimaryDerived( &primDer);
									PROJ_ID my_id;
									ObjForCore( ITER)->get_ID( &my_id);
									UniqueId base_guid = Identifier::getDetailsOf( base);

									CComPtr<IMgaFCO> my_pt;
									ObjForCore(ITER)->getinterface( &my_pt);
#if(DONT_OPTIM)
									mapOfSecondaryDeriveds [ my_id ] = base_guid;
#endif
#ifdef _DEBUG
									//MyCComBSTR m;
									//m.Append( my_id);
									//m.Append( L" ----::---- ");
									//if( primDer != VARIANT_TRUE) 
									//	m.Append( L" napOfSec");
									//else
									//	m.Append( L" nop");
									//m_reporter.show( m);
#endif
									// check if its really a secondary derived object
									// before storing into map
									// FIX for bug submitted by Pramod
									// when A, B have subtypes AS and BS, and BS gets a new child
									// an instance of AS: ASI
									// upon refresh ASI was deleted as an orphan child
									// because it was derived and it seemed secondary derived
									// along with it parent, however it wasn't
									if( primDer != VARIANT_TRUE) 
										napOfSecondaryDeriveds [ my_pt ] = base_guid;
								}

								if( primDer != VARIANT_TRUE) // it is a child derived along with 'p_baseObj'
								{
									// detach it as well from its base
									DetachObjFromLibBase( base, ITER, 0, false);
								}
								else 
								{	// it might be a primary subtype/instance placed into this subtype
									// leave this object untouched
								}
							}
							else
							{	// additional child->no action needed
							}

							long i_relid = ITER[ATTRID_RELID];
							if( i_relid < RELIDSPACE)
								max_relid_found = std::max(max_relid_found, i_relid);
						}
						break;
					}
					default: // for all others (CONNROLE, SETMEMB, REF) -> detach, 
					{        // otherwise they will be killed along with their master
#ifdef _DEBUG
						char p2[150];
						sprintf( p2, "(%u) ", ai);
						strcat( p, p2);
#endif
						// don't let connections die
						CoreObjs collmembers = p_derdObj[ai + ATTRID_COLLECTION];
						ITERATE_THROUGH(collmembers) {
							// remove the ATTRID_DERIVED attrs if any?
							// NOTE: 1st parameter is a dummy one: ITER
							DetachObjFromLibBase( ITER, ITER, 0, false);
						}
					}
				}; // endswitch
			}
	} MGACOLL_ITERATE_END;

	if( has_last_relid_attr // if this CoreObj had RELID_LAST attribute
		&& max_relid_found > last_relid_set) // additional objects became plain
	{
		p_derdObj[ATTRID_LASTRELID] = max_relid_found;
	}
}

void RefreshManager::AttachDerObjs( const CoreObj &p_baseObj, CoreObj &p_derdObj, long instance, bool prim) 
{
	long nb_of_children = 0;
	long last_relid_set = 0;
	long max_relid_found= 0;
	bool has_last_relid_attr = false;
	bool instance_met = false;
	CComPtr<ICoreAttributes> atts;
	COMTHROW( p_baseObj->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));
			if(ai < ATTRID_COLLECTION) {
				switch(ai) {
					case ATTRID_GUID1:
					case ATTRID_GUID2:
					case ATTRID_GUID3:
					case ATTRID_GUID4: 
						break; // nop

					case ATTRID_DERIVED: 
					case ATTRID_MASTEROBJ:
					{
						p_derdObj[ai] = p_baseObj;
						break;
					}
					case ATTRID_PERMISSIONS:
					// if it was instance or p_baseObj is instance or it is supposed to be an instance:
					{
							p_derdObj[ai] = (p_derdObj[ai] | p_baseObj[ai] | long(instance)) & INSTANCE_FLAG;
							instance_met = p_derdObj[ai] & INSTANCE_FLAG;
							break;
					}
					case ATTRID_RELID: 
					{
						if(!prim)
						{	// Damage Control for GME-145 entry in JIRA
							// relids were messed up by the other PropObjRelidInc mechanism
							// because it was invoked to all children of the derd
							// and children are not always homogeneous
							unsigned long lb = p_baseObj[ai];
							unsigned long ld = p_derdObj[ai];
							if( ld <= lb) // FIXME: is this necessary?
							{	// Shift relid
								p_derdObj[ai] = lb + RELIDSPACE;
								propObjRelidIncV2( p_derdObj);
#ifdef _DEBUG
								CComBSTR msg = p_derdObj[ATTRID_NAME];
								msg.Append( " relid problem corrected");
								m_reporter.show( msg, false);
#endif
							}
						}
						break;
					}
					case ATTRID_LASTRELID: 
					{
						last_relid_set = p_derdObj[ai];
						has_last_relid_attr = true;
						break;
					}
					
					default:
						break;
				} // endswitch
			}
	} MGACOLL_ITERATE_END;

	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));

			if(ai >= ATTRID_COLLECTION) {
				ai -= ATTRID_COLLECTION;
				if(LINKREF_ATTR(ai))  {
					//if( ai == ATTRID_ATTRPARENT)       { } // no need to copy these since the
					//else if( ai == ATTRID_CONSTROWNER) { } // automatically
					if(ai == ATTRID_CONNROLE) {
						// ConnRoles also depend on their base with ATTRID_MASTEROBJ
						// that's how a derived connections will be compared by
						// ConnCompareToBase to its base.
						// Thus the connroles will not be dumped into XML for derived conns
						CoreObjs roles = p_derdObj[ai + ATTRID_COLLECTION];
						ITERATE_THROUGH( roles) {
							CComBSTR n = ITER[ATTRID_NAME];
							CoreObj outer_ITER = ITER;
							ITERATE_THROUGH( p_baseObj[ai + ATTRID_COLLECTION]) {
								CComBSTR in = ITER[ATTRID_NAME];
								if( n == in)
								{
									// almost similar to calling AttachDerObjs( ITER, outer_ITER, ...)
									// but simpler
									outer_ITER[ATTRID_MASTEROBJ] = ITER;
									break;
								}
							}
						}
					}
					if(ai == ATTRID_FCOPARENT) {

						CoreObjs children_derd = p_derdObj[ai + ATTRID_COLLECTION];
						ITERATE_THROUGH(children_derd) {
							++nb_of_children;
							if( m_coreMatchMaker.find( ITER) != m_coreMatchMaker.end())
							{
								CoreObj bs = m_coreMatchMaker[ ITER];
								bool found = false;
								{CoreObjs children_base = p_baseObj[ai + ATTRID_COLLECTION];
								ITERATE_THROUGH(children_base) {
									if( ITER == bs) found = true;
								}}

								if( found)
								{
									AttachDerObjs( bs, ITER, instance, false);
								}

							}
							// else: will become an archetype child in subtype. OK.
							//       and what about instances? NOK.
							else if( instance)
								ASSERT(0);
						
							long i_relid = ITER[ATTRID_RELID];
							if( i_relid < RELIDSPACE)
								max_relid_found = std::max(max_relid_found, i_relid);
						}
					}
				}
			}

	} MGACOLL_ITERATE_END;

	if( has_last_relid_attr // if this CoreObj has RELID_LAST attribute
	 && max_relid_found < last_relid_set) // prefer lower last relid
	{
		p_derdObj[ATTRID_LASTRELID] = max_relid_found;
	}

}

// called from collectDersFromLib, in order to find and store all library dependent references
// (aka adaptive refs) which must be conforming (as far as their target is concerned) to their
// library object base
// not only the immediate deriveds are considered to be adaptive, but their deriveds also
// (the whole ref hierarchy stemming from the library reference)
void RefreshManager::collectRefsDerdFromLib( CoreObj& one_ref, CoreObj& bas_ref)
{
	// one_ref is (in the project) a primary derived object from a library object
	if( one_ref.GetMetaID() != DTID_REFERENCE) return;

#if(0)
	CoreObj tgt = one_ref[ATTRID_REFERENCE];

	short dist_tgt = 0;
	elem_struct elem_tgt;
	if( tgt) // !null reference
	{
		CComPtr<IMgaFCO> tgt_ptr;
		ObjForCore(tgt)->getinterface( &tgt_ptr);
		getElemDetails( tgt_ptr, elem_tgt);

		dist_tgt = distance( tgt);
	}

	short dist_ref = distance( one_ref);

	short st = 0;
	ObjForCore( one_ref)->RefCompareToBase( &st);
	if( st == 0 && tgt) // 0 means equal, so it is non-redird
	{
		dist_ref = -1;
	}
#endif
	// updates mapOfAdaptiveReferences
	saveTopAdaptiveRef( one_ref, bas_ref);

	// updates mapOfDerdRefsFromAdaptiveRef
	saveDerAdaptiveRef( one_ref);
}

// 
// RL = ref in library       RL
// RPA= ref in project       / \ 
//                            |
// RPD1, RPD2 are            RPA
// dependents refs of       / \ 
// adaptive ref(RP)        /   \
//                       RPD1  RPD2
// 
// this method deals with objects like RPD1
void RefreshManager::saveDerAdaptiveRef( CoreObj& one_ref)
{
	// save dependents of one_ref
	CoreObjs deriveds = one_ref[ATTRID_DERIVED + ATTRID_COLLECTION];
	ITERATE_THROUGH(deriveds) {
		CoreObj tgt = ITER[ATTRID_REFERENCE];
		if( tgt)
		{
			CComPtr<IMgaFCO> tgt_ptr;
			ObjForCore( tgt)->getinterface( &tgt_ptr);
			elem_struct tgt_elem;
			getElemDetails( tgt_ptr, tgt_elem);
			// save it only if targeted toward a nonlibrary object
			//if( !tgt_elem.inlib)
			{
				short dist = distance( ITER);
				DIST_AND_REFOBJ key = std::make_pair( dist, ITER);

				MIXED_TARGET2   val;
				//if( tgt_elem.inlib) val = std::make_pair( tgt_elem.uid, CoreObj());
				if( tgt_elem.inverysamelib) val = std::make_pair( tgt_elem.uid, CoreObj());
				else                val = std::make_pair( UniqueId()  , tgt);

				mapOfDerdRefsFromAdaptiveRef[ key] = val;
			}
		}

		saveDerAdaptiveRef( ITER);
	}
}

// 
// RL = ref in library       RL
// RPA= ref in project       / \ 
//                            |
// RPD1, RPD2 are            RPA
// dependents refs of       / \ 
// adaptive ref(RP)        /   \
//                       RPD1  RPD2
// 
// this method deals with objects like RPA
void RefreshManager::saveTopAdaptiveRef( CoreObj& one_ref, CoreObj& bas_ref)
{
	// base ref's info
	UniqueId bas_id = Identifier::getDetailsOf( bas_ref);

	// its target 
	UniqueId bas_tgt_id;
	CoreObj bas_tgt = bas_ref[ATTRID_REFERENCE];
	if( bas_tgt)
		bas_tgt_id = Identifier::getDetailsOf( bas_tgt);

	LIBREF_AND_LIBTARGET base_ref_pair = std::make_pair( bas_id, bas_tgt_id );
	short dist = distance( one_ref);
	LIB_REFTARG_PAIR k = std::make_pair( dist, base_ref_pair);

	// derived ref's info
	// might be in project also
	UniqueId ref_id = Identifier::getDetailsOf( one_ref);

	// its target 
	CoreObj  ref_tgt_core;
	UniqueId ref_tgt_id;
	CoreObj ref_tgt = one_ref[ATTRID_REFERENCE];
	if( ref_tgt)
	{
		ref_tgt_id = Identifier::getDetailsOf( ref_tgt);
		if( ref_tgt_id.libId == BinGuid()) // regular target (is in project)
			ref_tgt_core = ref_tgt;
	}

	MIXED_TARGET      target_info   = std::make_pair( ref_tgt_id, ref_tgt_core);
	REGREF_AND_TARGET derd_ref_pair = std::make_pair( one_ref, target_info );
	//short t_dist = 0;//distance( );
	//REF_PAIR v = std::make_pair( t_dist, derd_ref_pair);

	// insert the record into the storage
	mapOfAdaptiveReferences[ k ].push_back( derd_ref_pair);
#if(0)
	// recursive
	CoreObjs deriveds = one_ref[ATTRID_DERIVED + ATTRID_COLLECTION];
	ITERATE_THROUGH(deriveds) {
		//saveTopAdaptiveRef( ITER, one_ref);
	}
#endif
}

// 
void RefreshManager::collectDersFromLib( CoreObj& one_fco)
{
	auto obj = ObjForCore(one_fco);
	if( obj)
	{
		elem_struct elem_self;

		CComPtr<IMgaFCO> obj_ptr;
		obj->getinterface( &obj_ptr);
		getElemDetails( obj_ptr, elem_self);

		CComPtr<IMgaFCOs> subs;
		COMTHROW( obj->get_DerivedObjects( &subs));
		long l;
		COMTHROW( subs->get_Count( &l));
		for( long i = 1; i <= l; ++i)
		{
			CComPtr<IMgaFCO> sub;
			COMTHROW( subs->get_Item( i, &sub));

			elem_struct elem_sub;
			getElemDetails( sub, elem_sub);

			/*if( elem_sub.inlib) // which lib? it matters!
			{
				if( elem_sub.uid.libId == Identifier::getLibRootOf( one_fco)
					|| elem_sub.uid.libId == Identifier::getTopLibRootOf( one_fco)
					)
					continue;
				else
					if( m_changedLibs.end() == std::find( m_changedLibs.begin(), m_changedLibs.end(), elem_sub.uid.libId))
						m_changedLibs.push_back( elem_sub.uid.libId);
			}*/
			/*if( elem_sub.inlib) // if subtype also in library, do not care
				continue;*/
			if( elem_sub.inverysamelib)
				continue;

			// collect references, both primary and secondary deriveds 
			collectRefsDerdFromLib( CoreObj( sub), one_fco);

			VARIANT_BOOL prim;
			COMTHROW( sub->get_IsPrimaryDerived( &prim));
			if( prim == VARIANT_FALSE) // secondary derived, will be handled by DetachObjFromLibBase
				continue;

			// sub is in the project, primary derived
			VARIANT_BOOL is_inst;
			COMTHROW( sub->get_IsInstance( &is_inst));

			/*CComPtr<IMgaModel> parentM;
			CComPtr<IMgaFolder> parentF;
			CComPtr<IMgaObject> parentMF;
			HRESULT res = sub->get_ParentModel( &parentM);
			if( FAILED( res) || !parentM)
			{
				res = sub->get_ParentFolder( &parentF);
			}*/

			if( 1)//SUCCEEDED( res) && (parentM || parentF))
			{
				SUB_PAR sp;
				// fill the known fields
				sp.subt_ptr = sub;
				sp.is_instance = is_inst == VARIANT_TRUE;
#if(0)
				sp.subt_id = elem_sub.id;
				if( parentF)
				{
					// fill up subt_parent in sp SUB_PAR
					COMTHROW( parentF->get_ID( &sp.subt_parent));
					sp.parentIsFolder = true;
				}
				else if( parentM)
				{
					// fill up subt_parent in sp SUB_PAR
					COMTHROW( parentM->get_ID( &sp.subt_parent));

					CComPtr<IMgaMetaRole> meta_role;
					COMTHROW( sub->get_MetaRole( &meta_role));
					// fill up subt_metarole in sp SUB_PAR
					COMTHROW( meta_role->get_Name( &sp.subt_metarole));
					sp.parentIsFolder = false;
				}
#endif				
				// add structure to the storage map
				mapOfDeriveds[ elem_self.uid].push_back( sp);
#ifdef _DEBUG
				//MyCComBSTR msg ("mapOfDeriveds ");
				//CComBSTR oid, sid; COMTHROW(ObjForCore( one_fco)->get_ID( &oid)); COMTHROW(sub->get_ID( &sid));
				//COMTHROW(msg.AppendBSTR( oid));COMTHROW(msg.Append( L" base of ")); COMTHROW(msg.AppendBSTR( sid));
				//COMTHROW(msg.appendLink( one_fco)); COMTHROW(msg.appendLink( sub));
				//m_reporter.show( msg);
#endif

			}
			else 
				ASSERT(0);
		}
	}
}

void RefreshManager::collectRefsToLib( CoreObj& one_fco)
{
	auto obj = ObjForCore(one_fco);
	if( obj)
	{
		short dist_self = distance( one_fco); // the distance to the archetype of target (one_fco)

		elem_struct elem_self;
		//getElemDetails( obj->getinterface(), elem_self);
		
		CComPtr<IMgaFCO> obj_ptr;
		obj->getinterface( &obj_ptr);
		getElemDetails( obj_ptr, elem_self);

		CComPtr<IMgaFCOs> refs;
		COMTHROW( obj->get_ReferencedBy( &refs));
		long l;
		COMTHROW( refs->get_Count( &l));
		for( long i = 1; i <= l; ++i)
		{
			CComPtr<IMgaFCO> ref;
			COMTHROW( refs->get_Item( i, &ref));

			elem_struct elem_ref;
			getElemDetails( ref, elem_ref);

			/*if( elem_ref.inlib) // which lib? it matters!
			{
				if( elem_ref.uid.libId == Identifier::getLibRootOf( one_fco)
					|| elem_ref.uid.libId == Identifier::getTopLibRootOf( one_fco)
					)
					continue;
				else
					if( m_changedLibs.end() == std::find( m_changedLibs.begin(), m_changedLibs.end(), elem_ref.uid.libId))
						m_changedLibs.push_back( elem_ref.uid.libId);
			}*/
			/*if( elem_ref.inlib) // ref also in library?
				continue;*/
			if( elem_ref.inverysamelib)
				continue;


			// So far we know that:
			// ref in project
			// target in libr

			bool base_in_lib    = false;	// more precisely, base is the SAME lib as target
											// and we are interested whether the object is participating in
											// a derivation chain which has library objects at its top
			bool is_redirected  = false;
			bool is_secderd     = false;
			bool isder = isDerivedRef( ref, obj_ptr, &base_in_lib, &is_redirected, &is_secderd);
			if( isder)
			{
				// ref's target is in libr, so we are sure
				// that base's target is also in libr
				// 
				// if base is in the same lib as target
				// then we get for granted that relationship (base-->target)
				// and must update the adaptives accordingly (ref-->target)

				short dist_ref = distance( CoreObj( ref));
				if( base_in_lib)
				{
					// base ---> target reference relationship may have changed 'overnight'
					// (during library project modification) so there exists a chance of
					// discrepancy upon update if we restore it to refer to its old target
					// this challenge is taken by the collectRefsDerdFromLib & mapOfAdaptiveReferences
					continue;
				}
				else
				{
					// base is not in the library (the same library where target is)
					// base's target is in the library because ref's target is in the library
					// if is not redirected specifically then we can rely on the default propagation
					// mechanism (collectRefsToLib, restoreRefsToLib will be invoked with base as well)
					if( is_redirected)
					{
						mapOfDependentReferences[ std::make_pair( dist_self, elem_self.uid)].insert( std::make_pair( dist_ref, CoreObj( ref)));
						continue;
					}
					else
						continue; // no need for handling, when its base is handled that's enough for us
				}
			}

			mapOfReferences[ elem_self.uid].push_back( CoreObj( ref));
		}
	}
}

// cuts dependencies of derived objects from one_fco (a lib object)
// thus when one_fco is destroyed it won't be followed by them
void RefreshManager::cutDersFromLib( CoreObj& one_fco)
{
	auto obj = ObjForCore(one_fco);
	if( obj)
	{
		CComPtr<IMgaFCOs> subs;
		COMTHROW( obj->get_DerivedObjects( &subs));
		long l;
		COMTHROW( subs->get_Count( &l));
		for( long i = 1; i <= l; ++i)
		{
			CComPtr<IMgaFCO> sub;
			COMTHROW( subs->get_Item( i, &sub));

			VARIANT_BOOL prim;
			COMTHROW( sub->get_IsPrimaryDerived( &prim));
			if( prim == VARIANT_FALSE) // secondary derived, do not care
				continue;


			elem_struct elem_sub;
			getElemDetails( sub, elem_sub);

			/*if( elem_sub.inlib) // which lib? it matters!
			{
				if( elem_sub.uid.libId == Identifier::getLibRootOf( one_fco)
					|| elem_sub.uid.libId == Identifier::getTopLibRootOf( one_fco)
					)
					continue; // subtype also in the same library
				else
					if( m_changedLibs.end() == std::find( m_changedLibs.begin(), m_changedLibs.end(), elem_sub.uid.libId))
						m_changedLibs.push_back( elem_sub.uid.libId);
			}*/
			/*if( elem_sub.inlib) // subtype also in library
				continue;*/
			if( elem_sub.inverysamelib)
				continue;

			// cut the relationship between derd and bs
			// by removing the dependency me help derd 
			// objects survive the death of bs object
			CoreObj derd( sub);
			DetachObjFromLibBase( one_fco, derd, 0, true);
		}
	}
}

void RefreshManager::cutRelations( CoreObj& p_cont)
{
	ITERATE_THROUGH( p_cont[ATTRID_FCOPARENT+ATTRID_COLLECTION]) 
	{
		if(ITER.IsFCO())
		{
			cutDersFromLib( ITER); // also detaches objects from library base objects
		}
		
		if( ITER.IsContainer()) cutRelations( ITER);
	}
}

void RefreshManager::reattachSubtypesInstances()
{
	PRI_MATCHMAKER_MAP::const_iterator i = m_primMatchMaker.begin();
	for( ; i != m_primMatchMaker.end(); ++i)
	{
		//i->first; // ptr of derd
		//i->second; // PAIR <ptr of base, is_instance>
		CoreObj prev_detached( i->first);
		CoreObj fresh_base   ( i->second.first);
		bool is_instance     ( i->second.second);
		
		AttachDerObjs( fresh_base, prev_detached, is_instance?VARIANT_TRUE:VARIANT_FALSE, true);
	}
}

void RefreshManager::clearDepStorage()
{
	lib_stack.clear();
	conns_stored.clear();
	conns_to_derive.clear();
	m_mapOfMemberships.clear();

	mapOfReferences.clear();
	mapOfDependentReferences.clear();
	mapOfAdaptiveReferences.clear();
	mapOfDerdRefsFromAdaptiveRef.clear();
	m_alreadySetRefs.clear();

	mapOfDeriveds.clear();
	napOfSecondaryDeriveds.clear();

	m_primMatchMaker.clear();
	m_coreMatchMaker.clear();


#if(DONT_OPTIM)
	conns_to_recreate.clear();
	mapOfSecondaryDeriveds.clear();
	m_matchMaker.clear();
#endif
}

void RefreshManager::collectDependencies( CoreObj& p_container)
{
	bool lib_root_found = false;

	long pm = p_container[ATTRID_PERMISSIONS];
	if( pm & LIBROOT_FLAG)
	{
		BinGuid libn = Identifier::getPersistentIdOf( p_container);
		lib_stack.push_back( libn);
		lib_root_found = true;
	}

	ITERATE_THROUGH(p_container[ATTRID_FCOPARENT+ATTRID_COLLECTION]) 
	{
		if(ITER.IsFCO())
		{
			collectRefsToLib( ITER);
			collectMixedConns( ITER);
			collectDersFromLib( ITER);
		}
		
		if( ITER.IsContainer()) collectDependencies( ITER);
	}

	ASSERT( !lib_stack.empty());
	if( lib_root_found && !lib_stack.empty())
	{
		lib_stack.pop_back();
	}
}

void RefreshManager::restoreDependencies()
{
	// step 1
	m_reporter.show( CComBSTR(L"[Step 1] Restoring references to the new library..."), false);
	
	adaptTopRefs();
	adaptDerRefs();
	restoreRefsToLib();

	// step 2
	m_reporter.show( CComBSTR(L"[Step 1] Done."), false);
	m_reporter.show( CComBSTR(L"[Step 2] Restoring connections to the new library..."), false);

	restoreMixedConns( m_newLib);

	// step 3
	m_reporter.show( CComBSTR(L"[Step 2] Done."), false);
	m_reporter.show( CComBSTR(L"[Step 3] Updating subtypes and instances derived from the library..."), false);

	syncStructureFromLib();

	// step 4
	m_reporter.show( CComBSTR(L"[Step 3] Done."), false);
	m_reporter.show( CComBSTR(L"[Step 4] Updating new connections in subtypes and instances..."), false);

	syncFreshConns();

	// step 5
	m_reporter.show( CComBSTR(L"[Step 4] Done."), false);
	m_reporter.show( CComBSTR(L"[Step 5] Reattaching subtypes and instances..."), false);
	
	reattachSubtypesInstances();

	// step 6?
	reapplyLibFlagToChangedLibs();

	m_reporter.show( CComBSTR(L"[Step 5] Refresh done."), false);
}

int RefreshManager::getNumOfErrors( MyCComBSTR& msg)
{
	int k = m_reporter.getErrors();
	TCHAR buf[32];
	_stprintf_s( buf, _T("%d"), k);
	COMTHROW(msg.Append( L"Warnings: "));
	COMTHROW(msg.Append( buf));

	return k;
}

/*static*/ const CComBSTR Creator::SrcName = L"src"; // keep in sync with FCO::SrcName in MgaConnection.cpp
/*static*/ const CComBSTR Creator::DstName = L"dst"; // keep in sync with FCO::DstName

/*static*/
HRESULT Creator::SimpleConn( CComPtr<IMgaModel>&      p_parent
                           , CComPtr<IMgaMetaRole>&   p_mrole
                           , CComPtr<IMgaFCO>&        p_src
                           , CComPtr<IMgaFCO>&        p_dst
                           , CComPtr<IMgaFCOs>&       p_srcRefs
                           , CComPtr<IMgaFCOs>&       p_dstRefs
                           , CMgaProject*             p_mgaProject
                           , IMgaFCO**                p_newConn
                           )
{	// copied from HRESULT FCO::CreateSimpleConn( ...)
	CMgaProject* &mgaproject = p_mgaProject;
	COMTRY_IN_TRANSACTION { // COMTRY_IN_TRANSACTION needs a variable named mgaproject
		CComPtr<IMgaFCO> f;
		{
			booltempflag fl( p_mgaProject->checkofftemporary);
			//COMTHROW(CreateChildObject(metar, &f));
			COMTHROW( Creator::Child( p_mgaProject, CComPtr<IMgaFCO>( p_parent), p_mrole, &f));
			objtype_enum ot;
			COMTHROW( f->get_ObjType( &ot));
			if( ot != OBJTYPE_CONNECTION) COMTHROW( E_MGA_INVALID_ROLE);
			COMTHROW( Creator::put_NamedRole( p_mgaProject, f, SrcName, p_srcRefs, p_src));
		}	
		COMTHROW( Creator::put_NamedRole( p_mgaProject, f, DstName, p_dstRefs, p_dst));
		*p_newConn = f.Detach();
	}
	COMCATCH_IN_TRANSACTION(;);	
}

/*static*/
HRESULT Creator::Child( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_parent, CComPtr<IMgaMetaRole>& p_metar, IMgaFCO **p_newobj)
{	// copied from HRESULT FCO::CreateChildObject(IMgaMetaRole *metar, IMgaFCO **newobj)
	CMgaProject* &mgaproject = p_mgaProject;
	COMTRY_IN_TRANSACTION { // COMTRY_IN_TRANSACTION needs a variable named mgaproject
		//CheckWrite();
		CHECK_INPTRPAR( p_metar);
		CHECK_OUTPTRPAR( p_newobj);
		//if((long)self[ATTRID_PERMISSIONS] & ~EXEMPT_FLAG) COMTHROW(E_MGA_NOT_CHANGEABLE);
		CoreObj  nobj;
		CComPtr<IMgaMetaFCO> meta;
		metaref_type rr;
		COMTHROW( p_metar->get_MetaRef( &rr));
		COMTHROW( p_metar->get_Kind( &meta));

		CoreObj a_self( p_parent);
		auto parent = ObjForCore(a_self);

		COMTHROW( parent->ContainerCreateFCO( meta, nobj));

		nobj[ATTRID_ROLEMETA]=rr;
		auto nfco = ObjForCore(nobj);
		nfco->initialname();

		setcheck( p_mgaProject, nobj, CHK_NEW);

		int targetlevel = 0;
		CoreObj rootp;

		// a_self is not a member!
		GetRootOfDeriv( a_self, rootp, &targetlevel);
		if( targetlevel >= 0) ReDeriveNewObj( p_mgaProject, nobj, targetlevel+1);
		docheck( mgaproject);

		nfco->SelfMark(OBJEVENT_CREATED);
		parent->SelfMark(OBJEVENT_NEWCHILD);
		nfco->getinterface( p_newobj);
	}
	COMCATCH_IN_TRANSACTION(;);	
}

/*static*/
HRESULT Creator::put_NamedRole( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_conn, BSTR p_rolename,  IMgaFCOs* p_refs, IMgaFCO* p_endfco)
{	// copied from FCO::put_NamedRole(...)
	CMgaProject* &mgaproject = p_mgaProject;
	COMTRY_IN_TRANSACTION { // COMTRY_IN_TRANSACTION needs a variable named mgaproject
		//CheckWrite();
		{
			booltempflag fl( p_mgaProject->checkofftemporary);

			CComPtr<IMgaConnPoints> p;
			CComQIPtr<IMgaConnection> a_conn( p_conn);
			COMTHROW( a_conn->get_ConnPoints(&p)); 
			MGACOLL_ITERATE(IMgaConnPoint, p) {
				CComBSTR rn;
				COMTHROW(MGACOLL_ITER->get_ConnRole(&rn));
				if(rn == p_rolename) {
					COMTHROW(MGACOLL_ITER->Remove());
					break;
				}
			} MGACOLL_ITERATE_END;
		}
		if( p_endfco) {
			CComPtr<IMgaConnPoint> tt;
			COMTHROW( Creator::AddConnPoint( p_mgaProject, p_conn, p_rolename, 1, p_endfco, p_refs, &tt));
		}
	} COMCATCH_IN_TRANSACTION(;)
}	

/*static*/
HRESULT Creator::AddConnPoint( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_conn, BSTR p_rolename, long p_maxinrole, IMgaFCO* p_target, IMgaFCOs* p_refs, IMgaConnPoint** pVal) 
{	// copied from FCO::AddConnPoint(...)
	CMgaProject* &mgaproject = p_mgaProject;
	COMTRY_IN_TRANSACTION { // COMTRY_IN_TRANSACTION needs a variable named mgaproject
		//CheckWrite();
		long c = 0;
		if( p_refs) COMTHROW( p_refs->get_Count(&c));
		std::vector<CoreObj> peers(c+2);
		CHECK_INSTRPAR( p_rolename);
		CHECK_MYINPTRPAR( p_target);
		CHECK_OUTPTRPAR( pVal);
		int pos = 0;
		if( p_refs) {
			CHECK_MYINPTRSPAR( p_refs);

			MGACOLL_ITERATE(IMgaFCO, p_refs) {
				CoreObj l(MGACOLL_ITER);
				if(l.GetMetaID() != DTID_REFERENCE) COMTHROW(E_MGA_REFERENCE_EXPECTED);
				peers[pos++] = l;
			} MGACOLL_ITERATE_END;
		}
		peers[pos++] = CoreObj( p_target);
		peers[pos] = NULLCOREOBJ;

		CoreObj a_self( p_conn);
		auto this_conn( ObjForCore( a_self));

		AddConnPTask( p_maxinrole, p_rolename).DoWithDeriveds( a_self, &peers);
		CMgaConnPoint::GetConnPoint( this_conn, peers[pos], pVal);
	} COMCATCH_IN_TRANSACTION(;);
}

/*static*/ 
HRESULT Worker::put_Name( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_elem, CComBSTR p_newVal)
{	// copied from FCO::put_Name
	CMgaProject* &mgaproject = p_mgaProject;
	COMTRY_IN_TRANSACTION { // COMTRY_IN_TRANSACTION needs a variable named mgaproject
		//CheckWrite();
		CHECK_INPAR( p_newVal);  
		CoreObj a_self( p_elem);
		//if(self[ATTRID_PERMISSIONS] & LIBROOT_FLAG) COMTHROW(E_MGA_LIBOBJECT);
		if(CComBSTR(a_self[ATTRID_NAME]) != p_newVal) {
			put_NameTask(CComBSTR(a_self[ATTRID_NAME]), p_newVal).DoWithDeriveds(a_self);;
		}
	}
	COMCATCH_IN_TRANSACTION(;);	
}

/*static*/ 
HRESULT Worker::PutGuidDisp( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_elem, CComBSTR p_guidStr)
{	// copied from FCO::PutGuidDisp
	CMgaProject* &mgaproject = p_mgaProject;
	COMTRY_IN_TRANSACTION { // COMTRY_IN_TRANSACTION needs a variable named mgaproject
		//CheckWrite();
		CHECK_INSTRPAR( p_guidStr);
		//self[ATTRID_GUID1] = newVal;
		GUID t_guid;
		CopyTo( p_guidStr, t_guid);

		long v1 = t_guid.Data1; // Data1: 32 b, Data2, Data 3: 16 b, Data4: 64 bit
		long v2 = (t_guid.Data2 << 16) + t_guid.Data3;
		long v3 = (((((t_guid.Data4[0] << 8) + t_guid.Data4[1]) << 8) + t_guid.Data4[2]) << 8) + t_guid.Data4[3];
		long v4 = (((((t_guid.Data4[4] << 8) + t_guid.Data4[5]) << 8) + t_guid.Data4[6]) << 8) + t_guid.Data4[7];

		Worker::PutGuid( p_mgaProject, p_elem, v1, v2, v3, v4);
	}  COMCATCH_IN_TRANSACTION(;)
}

HRESULT Worker::PutGuid( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_elem, long l1, long l2, long l3, long l4)
{	// copied from FCO::PutGuid
	CMgaProject* &mgaproject = p_mgaProject;
	COMTRY_IN_TRANSACTION { // COMTRY_IN_TRANSACTION needs a variable named mgaproject
		//CheckWrite();
		//ASSERT( self.IsFCO());
		//if( self.IsFCO()) {
		CoreObj a_self( p_elem);
		a_self[ATTRID_GUID1] = l1;
		a_self[ATTRID_GUID2] = l2;
		a_self[ATTRID_GUID3] = l3;
		a_self[ATTRID_GUID4] = l4;
	}  COMCATCH_IN_TRANSACTION(;)
}

