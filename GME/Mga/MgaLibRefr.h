#ifndef MGALIBREFRESH_H
#define MGALIBREFRESH_H

#define DONT_OPTIMIZE 0

#include <stdio.h>
#include <map>
#include <vector>
#include "Gme.h"

// ******************************************************************************
//                             C L A S S  BinGuid
// for representing the globally unique id assigned to each object
// NOTE:
// this class (BinGuid) represents a persistent ID of any MGA object (fco or folder)
// these persistent ids are not changing upon export/import/refresh/detach/reattach
// unlike mga ids which can change upon export-import
// beware that if you import twice the same xme file into separate projects
// then these persistent ids will remain equal in those separate projects
// so if these projects later are altered and thus considered different after a while
// they still might contain objects with the same persistent id (a GUID form id)
// in order to tackle this non-uniqueness the id assigned to the rootfolder (as a folder)
// plays an important role in identifying the scope (the realm where uniqueness applies) 
// of a persistent id
// 
class BinGuid
{
protected:
	long v1;
	long v2;
	long v3;
	long v4;

public:
	BinGuid()
		: v1( 0)
		, v2( 0)
		, v3( 0)
		, v4( 0)
	{ 
	}

	BinGuid( long p1, long p2, long p3, long p4)
		: v1( p1)
		, v2( p2)
		, v3( p3)
		, v4( p4) 
	{
	}

	void Set( long p1, long p2, long p3, long p4)
	{
		v1 = p1;
		v2 = p2;
		v3 = p3;
		v4 = p4;
	}

	bool operator == (const BinGuid& peer) const
	{
		if( v1 != peer.v1) return false; // speed-up

		return ( v1 == peer.v1
			&& v2 == peer.v2
			&& v3 == peer.v3
			&& v4 == peer.v4);
	}

	bool operator != ( const BinGuid& peer) const
	{
		return !( *this == peer);
	}

	bool operator < ( const BinGuid& peer) const 
	{
		if( v1 < peer.v1) return true; // speed-up

		return v1 < peer.v1
			|| v1 == peer.v1 && v2 < peer.v2 
			|| v1 == peer.v1 && v2 == peer.v2 && v3 < peer.v3
			|| v1 == peer.v1 && v2 == peer.v2 && v3 == peer.v3 && v4 < peer.v4;
	}

	void ConvertToStd( GUID& p_guid) const
	{
		const BinGuid &m_guid = *this;
		p_guid.Data1 = m_guid.v1;
		p_guid.Data2 = m_guid.v2 >> 16;
		p_guid.Data3 = m_guid.v2 & 0xFFFF;
		p_guid.Data4[0] = (m_guid.v3 >> 24);
		p_guid.Data4[1] = (m_guid.v3 >> 16) & 0xFF;
		p_guid.Data4[2] = (m_guid.v3 >> 8) & 0xFF;
		p_guid.Data4[3] = m_guid.v3 & 0xFF;

		p_guid.Data4[4] = (m_guid.v4 >> 24);
		p_guid.Data4[5] = (m_guid.v4 >> 16) & 0xFF;
		p_guid.Data4[6] = (m_guid.v4 >> 8) & 0xFF;
		p_guid.Data4[7] = m_guid.v4 & 0xFF;
	}
};

// ******************************************************************************
//                             C L A S S  UniqueId
// this class comprises the 
//      -- persistent id of an object and the 
//      -- persistent id of the library's rootfolder 
//
// IMPORTANT: the uniqueness of an fco guid is guarranteed within the scope
//            of a project/rootfolder
class UniqueId
{
public:
	BinGuid objectId;
	BinGuid libId;

	UniqueId()
	{
	}

	UniqueId( const BinGuid& po, const BinGuid& pl)
		: objectId( po)
		, libId( pl)
	{
	}

	void SetObj( const BinGuid& obj)
	{
		objectId = obj;
	}

	void SetLib( const BinGuid& lib)
	{
		libId = lib;
	}

	bool operator == (const UniqueId& peer) const
	{
		return ( libId == peer.libId
			&& objectId == peer.objectId);
	}

	bool operator< ( const UniqueId& peer) const 
	{
		if( objectId < peer.objectId) return true;

		return objectId < peer.objectId
			|| objectId == peer.objectId && libId < peer.libId;
	}
};

// regular project id of 0x0065-00000001 form 
// (it is a temporary id in fact, it might change after an export-import)
// used for identifying objects in the hosting project (where libraries are attached)
typedef CComBSTR PROJ_ID;

struct elem_struct
{
	//
	// an object's project id if the referred object is not in a library
	PROJ_ID    id;

	//
	// an object's persistent id if its a library element
	UniqueId   uid;

	// 
	// this variable indicates whether id or uid is filled
	// if inlib the uid, else id is filled
	// we could have used a union as well
	bool       inverysamelib;

	bool       inanotherlib;
};

// ******************************************************************************
//                             C L A S S  MyCComBSTR
// for enhancing CComBSTR with easy output of anchors to objects
// 
class MyCComBSTR : public CComBSTR
{
public:
	MyCComBSTR() : CComBSTR() { }
	MyCComBSTR( const CComBSTR& p) : CComBSTR( p) {}
	
	template<class T>
	void appendLink( const T& ptr)
	{
		if( ptr)
			appendLink( ObjForCore( CoreObj(ptr)));
		else
			COMTHROW(Append( L"NullObject"));
	}

	template<>
	void appendLink<FCOPtr>( const FCOPtr& ptr)
	{
		appendLink(static_cast<FCO*>(ptr));
	}

	void appendLink( const CComBSTR& id, const CComBSTR& nm = L"NonameObject")
	{
		COMTHROW(Append(L"<A HREF=\"mga:"));
		COMTHROW(AppendBSTR( id));
		COMTHROW(Append(L"\">"));
		COMTHROW(AppendBSTR( nm));
		COMTHROW(Append(L"</A>"));
	}

	void appendLink( FCO* fco)
	{
		CComBSTR id, nm;
		if( fco)
		{
			COMTHROW(fco->get_ID( &id));
			COMTHROW(fco->get_Name( &nm));
			appendLink( id, nm);
		}
		else
			COMTHROW(Append(L"NullObject"));
	}

	void appendGuid( const GUID& t_guid)
	{
		wchar_t buff[39];
		wsprintf( buff, L"{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			t_guid.Data1, t_guid.Data2, t_guid.Data3,
			t_guid.Data4[0], t_guid.Data4[1], t_guid.Data4[2], t_guid.Data4[3],
			t_guid.Data4[4], t_guid.Data4[5], t_guid.Data4[6], t_guid.Data4[7]);

		COMTHROW(Append( buff));
	}

	void appendGuid( const BinGuid& p_guid)
	{
		GUID t_guid;
		p_guid.ConvertToStd( t_guid);
		
		appendGuid( t_guid);
	}

	void appendGuid( const UniqueId& p_uid)
	{
		appendGuid( p_uid.objectId);
		COMTHROW(Append( L" in library "));
		appendGuid( p_uid.libId);
	}
};

// ******************************************************************************
//                             C L A S S  Identifier
// for getting identifying properties about object
//
class Identifier
{
public:
	static BinGuid getPersistentIdOf( const CoreObj& i)
	{
		return BinGuid( i[ATTRID_GUID1], i[ATTRID_GUID2], i[ATTRID_GUID3], i[ATTRID_GUID4]);
	}

	static bool isRefreshableLibRoot( const CoreObj& i)
	{
		// only toplibs can be refreshed (the inner libraries can't)
		bool retv = false;
		retv = getTopLibRootOf( i) == getLibRootOf( i);

		return retv;
	}

	static bool isLibRoot( const CoreObj& i)
	{
		bool retv = false;
		long perm = i[ATTRID_PERMISSIONS];
		ASSERT( perm & LIBROOT_FLAG);
		retv = (perm & LIBROOT_FLAG) != 0;
		
		return retv;
	}

	// gives back the uid of the realm (territory)
	// where the object uid is supposed to be unique
	static BinGuid getLibRootOf( const CoreObj& i)
	{
		CoreObj cur = i;

		bool goon = true;
		while( goon)
		{
			//CComBSTR nm;
			//ObjForCore(cur)->get_Name( &nm);
			//nm.Append(";");

			long perm = cur[ATTRID_PERMISSIONS];
			if( perm & LIBROOT_FLAG)
				return getPersistentIdOf( cur);

			// commented because sometimes regular objects are
			// identified with these helper method:
			// [we must be in a library if LibRoot still not found]
			//ASSERT( perm & LIBRARY_FLAG);

			// if no library element, nor libroot element, must be a regular project
			// element, so we consider these having a BinGuid(0) as their realm uid
			if( (perm & LIBRARY_FLAG) == 0)
				return BinGuid();

			metaid_type mt = cur.GetMetaID();
			ASSERT( mt != DTID_ROOT);
			cur = cur[ATTRID_PARENT];
			goon = cur != 0;// && ( mt != DTID_ROOT);
		}

		return BinGuid();
	}

	// used for getting the uid of the topmost library (the one we 
	// are refreshing now) in case of cascading libraries
	static BinGuid getTopLibRootOf( const CoreObj& i)
	{
		CoreObj cur = i;
		BinGuid lastgood;

		bool goon = true;
		while( goon)
		{
			//CComBSTR nm;
			//ObjForCore(cur)->get_Name( &nm);
			//nm.Append(";");

			long perm = cur[ATTRID_PERMISSIONS];

			if( perm & LIBROOT_FLAG)
				lastgood = getPersistentIdOf( cur);

			cur = cur[ATTRID_PARENT];
			metaid_type mt = cur.GetMetaID();
			//ASSERT( mt != DTID_ROOT);
			goon = cur != 0 && mt != DTID_ROOT;
		}

		return lastgood;
	}

	static UniqueId getDetailsOf( const CoreObj& i)
	{
		UniqueId uid;

		uid.SetObj( getPersistentIdOf( i));
		uid.SetLib( getLibRootOf( i));

		return uid;
	}

	static UniqueId getDetailsOf( const CoreObj& i, BinGuid& pLibCursor)
	{
		UniqueId uid;

		uid.SetObj( getPersistentIdOf( i));
		ASSERT( pLibCursor != BinGuid());
		uid.SetLib( pLibCursor);

		return uid;
	}
};

// ******************************************************************************
//                             C L A S S  Reporter
// sends info and error messages to the console if feature run with GUI
//
class Reporter
{
protected:
	int                 m_counter;
	CComPtr<IGMEOLEApp> m_gme;

	static CComPtr<IGMEOLEApp> get_GME( CMgaProject *p_mgaproject)
	{
		CComPtr<IGMEOLEApp> gme;
		if( p_mgaproject) {		
			CComBSTR bstrName("GME.Application");
			CComPtr<IMgaClient> pClient;
			HRESULT hr = p_mgaproject->GetClientByName(bstrName, &pClient);
			if (SUCCEEDED(hr) && pClient) {
				CComPtr<IDispatch> pDispatch;
				hr = pClient->get_OLEServer(&pDispatch);
				if (SUCCEEDED(hr) && pDispatch) {
					hr = pDispatch.QueryInterface(&gme);
					if (FAILED(hr)) {
						gme = NULL;
					}
				}
			}
		}
		return gme;
	} 

public:

	Reporter( CMgaProject *p_mgaproject) 
		: m_counter( 0)
	{
		m_gme = get_GME( p_mgaproject);
	}

	void show( CComBSTR msg, bool error = true )
	{
		if( error) ++m_counter;
		if( m_gme)
		{
			COMTHROW(m_gme->ConsoleMessage( msg, error?MSG_ERROR:MSG_INFO));
		}
	}

	int getErrors() const
	{
		return m_counter;
	}

	static void showIt( CMgaProject *p_mgaproject, CComBSTR msg, bool error = true )
	{
		CComPtr<IGMEOLEApp> t_gme = get_GME( p_mgaproject);

		if( t_gme)
		{
			COMTHROW(t_gme->ConsoleMessage( msg, error?MSG_ERROR:MSG_INFO));
		}
	}
};

// ******************************************************************************
//                             C L A S S  SearchTool
// for searching project objects based on project id
// and library objects based on uid
//
class SearchTool
{
public:
	//static std::list<BinGuid> m_sLibIdStack; // the search position is inside the topmost library right now
	static bool                 m_optimized;   // if the current operation in progress is optimized?

	static CComPtr<IMgaFCO> findLibObj(CoreObj& folder, const UniqueId& p_toFind, const BinGuid& p_inLib = BinGuid())
	{
		BinGuid act_lib_cursor = p_inLib;
		long pm = folder[ATTRID_PERMISSIONS];
		if( pm & LIBROOT_FLAG)
		{
			BinGuid act_lib_id = Identifier::getPersistentIdOf( folder);
			//m_sLibIdStack.push_back( act_lib_id);

			act_lib_cursor = act_lib_id;
		}

		//BinGuid lib_cursor;
		//ASSERT( !m_sLibIdStack.empty());
		//if( !m_sLibIdStack.empty()) lib_cursor = m_sLibIdStack.back();

		ITERATE_THROUGH(folder[ATTRID_FCOPARENT+ATTRID_COLLECTION]) 
		{
			if(ITER.IsFCO())
			{
				//UniqueId iters_id = Identifier::getDetailsOf( ITER, lib_cursor);
				UniqueId iters_id = Identifier::getDetailsOf( ITER, act_lib_cursor);
				if( iters_id == p_toFind)
				//if( iters_guid == p_toFind.objectId)
				//if( ITER[ATTRID_GUID1] == p_toFind.v1
				//	&& ITER[ATTRID_GUID2] == p_toFind.v2
				//	&& ITER[ATTRID_GUID3] == p_toFind.v3
				//	&& ITER[ATTRID_GUID4] == p_toFind.v4)
				{
					CComPtr<IMgaFCO> fco;
					ObjForCore(ITER)->getinterface(&fco);
					return fco; // found
				}
			}
			
			if( ITER.IsContainer())
			{
				CComPtr<IMgaFCO> fco = findLibObj(ITER, p_toFind, act_lib_cursor);
				if( fco) return fco; // found
			}
		}
		if( pm & LIBROOT_FLAG)
		{
			//m_sLibIdStack.pop_back(); // pop the library id when finished with depth first search of this branch
		}

		if( folder && m_optimized) // current operation in progress is optimized
		{
			CoreObj parnt = folder[ATTRID_FCOPARENT];
			if( parnt && parnt.GetMetaID() == DTID_FOLDER)
			{
				// process further down if really the rootfolder is met
				CoreObj grandparnt = parnt[ ATTRID_FCOPARENT];
				if( grandparnt && grandparnt.GetMetaID() == DTID_ROOT) 
				{
					// check other toplevel libraries (some of them might be the virtually included one
					ITERATE_THROUGH( parnt[ ATTRID_FCOPARENT + ATTRID_COLLECTION]) 
					{
						long prm = ITER[ATTRID_PERMISSIONS];
						if( prm & LIBROOT_FLAG)
						{
							BinGuid act_lib_id = Identifier::getPersistentIdOf( ITER);
							if( p_toFind.libId == act_lib_id     // proper toplevel library found
								&& act_lib_id != act_lib_cursor) // is not the very same the search already has analyzed
							{
								CComPtr<IMgaFCO> fco = findLibObj( ITER, p_toFind);
								if( fco) return fco; // found
							}
						}

					}
				}
				//findLibObj( parnt, p_toFind, 
			}
		}
		return 0;
	}

	static CComPtr<IMgaFCO> findObjWithDetails( CMgaProject *mgaproject, CoreObj& folder, elem_struct& elem)
	{
		if( elem.inverysamelib)//if( elem.inlib)
			return findLibObj( folder, elem.uid);
		else
			return findRegularObj( mgaproject, folder, elem.id);
	}


	static CComPtr<IMgaFCO> findRegularObj(CMgaProject * mgaproject, CoreObj& folder, const PROJ_ID& p_toFind)
	{
		CComPtr<IMgaFCO> res;
		try {
			COMTHROW( mgaproject->GetFCOByID( p_toFind, &res));
		} catch( hresult_exception&) {
			res = CComPtr<IMgaFCO>(0);
		}
		return res;
	}

	static CComPtr<IMgaFolder> findFolInHostProjectWithProjId(CMgaProject * mgaproject, CoreObj& folder, const PROJ_ID& p_toFind)
	{
		CComPtr<IMgaFolder> res;
		try {
			CComPtr<IMgaObject> ro;
			COMTHROW( mgaproject->GetObjectByID( p_toFind, &ro));
			if( ro) COMTHROW( ro.QueryInterface( &res));
		} catch( hresult_exception&) {
			res = CComPtr<IMgaFolder>(0);
		}
		return res;
	}

	static CoreObj findLibrary( CoreObj& p_folder, BinGuid& p_libId)
	{
		long pm = p_folder[ATTRID_PERMISSIONS];
		if( pm & LIBROOT_FLAG)
		{
			BinGuid act_lib_id = Identifier::getPersistentIdOf( p_folder);
			if( act_lib_id == p_libId)
				return p_folder;
		}

		ITERATE_THROUGH( p_folder[ATTRID_FCOPARENT+ATTRID_COLLECTION]) 
		{
			metaid_type mi = ITER.GetMetaID();
			if( mi == DTID_FOLDER)
			{
				CoreObj lib = findLibrary( ITER, p_libId);
				if( lib) return lib; // found
			}
		}

		return CoreObj();
	}

	static CoreObj findTopLibrary( CoreObj& p_folder, CComBSTR& p_libId)
	{
		ITERATE_THROUGH( p_folder[ATTRID_FCOPARENT+ATTRID_COLLECTION]) 
		{
			metaid_type mi = ITER.GetMetaID();
			long pm = ITER[ATTRID_PERMISSIONS];
			if( mi == DTID_FOLDER && (pm & LIBROOT_FLAG))
			{
				//BinGuid lib_id = Identifier::getPersistentIdOf( ITER);
				CComBSTR lib_id;
				ObjForCore( ITER)->GetGuidDisp( &lib_id);
				if( lib_id == p_libId)
					return ITER;
			}
		}

		return CoreObj();
	}

	static bool findTopLibInProj(       CMgaProject*       p_mgaProject
	                            , const PROJ_ID&           p_idOfLib
	                            ,       CoreObj&           p_libObjCore)
	{
		bool all_right = false;

		ASSERT( p_idOfLib.Length() == 16); // id like: "id-006a-00000002"

		try 
		{
			CComPtr<IMgaFolder> rf;
			p_mgaProject->get_RootFolder( &rf);
			CoreObj crf = CoreObj( rf);

			ITERATE_THROUGH( crf[ ATTRID_FCOPARENT + ATTRID_COLLECTION]) 
			{
				metaid_type mtyp = ITER.GetMetaID(); 
				if( mtyp == DTID_FOLDER) 
				{
					auto kid = ObjForCore( ITER);
					if( !kid) continue;

					CComBSTR id;
					COMTHROW( kid->get_ID( &id));

					if( id == p_idOfLib                                  // equal id
						&& !ITER.IsDeleted()                             // not a zombie
						&& ( ITER[ATTRID_PERMISSIONS] & LIBROOT_FLAG))   // a library root
					{
						p_libObjCore = ITER;
						all_right = true;
						break;
					}
				}
			}
		} 
		catch( hresult_exception& ) 
		{
			ASSERT(0);
		}

		ASSERT( all_right);
		return all_right;
	}


};


// ******************************************************************************
//                             C L A S S  RefreshManager
// for managing the refresh procedure
//
class RefreshManager
{
public:
	RefreshManager( CMgaProject *p_mgaproject, CoreObj& p_oldRoot, CoreObj& p_newRoot, bool p_isIncluded)
		: m_mgaproject( p_mgaproject)
		, m_newLib( p_newRoot)
		, m_oldLib( p_oldRoot)
		, m_reporter( p_mgaproject)
		, m_isVirtuallyIncluded( p_isIncluded)
	{
	}

protected:
	CMgaProject * m_mgaproject;
	CoreObj       m_newLib;
	CoreObj       m_oldLib;
	Reporter      m_reporter;
	bool          m_isVirtuallyIncluded;
	// for reference recreating
	//typedef std::vector< PROJ_ID > REFERENCES_VEC;
	//typedef std::map< UniqueId, REFERENCES_VEC > REFERENCES_MAP; // KEY: GUID of LibTarget, VALUE: vector of Reference ids, which need to be redirected to LibTarget
	typedef std::vector< CoreObj > CORE_REFERENCES_VEC;
	typedef std::map< UniqueId, CORE_REFERENCES_VEC > REFERENCES_MAP; // KEY: GUID of LibTarget, VALUE: vector of Reference ids, which need to be redirected to LibTarget
	REFERENCES_MAP mapOfReferences;

	typedef std::pair< short, CoreObj>                SORTED_REF_ELEM;

	class RefComp
	{
	public:
		bool operator () ( const std::pair< short, CoreObj>& p1
			            , const std::pair< short, CoreObj>& p2) const
		//bool operator < ( const SORTED_REF_ELEM& p1, const SORTED_REF_ELEM& p2)
		{
			if( p1.first < p2.first) return true;
			else if( p1.first > p2.first) return false;
			else return p1.second < p2.second;
		}
	};

	typedef std::set< SORTED_REF_ELEM, RefComp >      CORE_REFERENCES_SET; // short: the distance from the topmost base
	// by using a pair as the key of the map we make sure that hierarchy
	// of reference is observed, the ones at the top will be set before those
	// at the bottom

	typedef std::map< std::pair<short,UniqueId>, CORE_REFERENCES_SET > DEP_REFERENCES_MAP;
	DEP_REFERENCES_MAP mapOfDependentReferences;

	typedef std::pair< UniqueId, UniqueId >               LIBREF_AND_LIBTARGET;
	typedef std::pair< short, LIBREF_AND_LIBTARGET >      LIB_REFTARG_PAIR;
	// if target is in library then UniqueId identifies it
	// otherwise the CoreObj is the target
	typedef std::pair< UniqueId, CoreObj >                MIXED_TARGET;
	// CoreObj is the regular reference in the project, mixedtarget is its target
	typedef std::pair< CoreObj, MIXED_TARGET>             REGREF_AND_TARGET;
	typedef std::vector< REGREF_AND_TARGET >              CORE_DERD_REF_VEC;
	typedef std::map< LIB_REFTARG_PAIR, CORE_DERD_REF_VEC > ADAPTIVE_REFERENCES_MAP;

	// will store adaptive refs (they must be knowledgable of the fact whether their base 
	// was redirected or not, while they are not attached to them yet)
	ADAPTIVE_REFERENCES_MAP          mapOfAdaptiveReferences;
	
	typedef std::pair< short, CoreObj >                 DIST_AND_REFOBJ;
	typedef std::pair< UniqueId, CoreObj >              MIXED_TARGET2; // these refs might point to the library or to the project
	typedef std::map< DIST_AND_REFOBJ, MIXED_TARGET2>   SPECIAL_MAP; // Key: <levelOfRef, Ref>, Value: <TgtId, TgtObj>

	// will store dependents of adaptive references (they must conform to the adaptive refs
	// while they ARE attached to them)
	SPECIAL_MAP                      mapOfDerdRefsFromAdaptiveRef;

	// will prevent dependents of changed (redirected in library) refs
	// to be restored according to the pre-refresh database
	std::set< CoreObj> m_alreadySetRefs;

	// for subtype recreating 
	struct SUB_PAR
	{
		CComPtr<IMgaFCO> subt_ptr;
#if(0)
		PROJ_ID subt_id; // is this needed at all? YES ! if Detach/Attach procedure is used definitely
		PROJ_ID subt_parent;    //obsolete: // these are NOT needed since Detach/Attach  //CComBSTR subt_parent; // where to recreate the subtype?
		CComBSTR subt_metarole; //obsolete: // these are NOT needed since Detach/Attach 
		bool     parentIsFolder;//obsolete: // these are NOT needed since Detach/Attach 
#endif
		bool     is_instance;
	};

	typedef std::vector< SUB_PAR > SUB_PAR_VEC;
	typedef std::map< UniqueId, SUB_PAR_VEC > SUB_PAR_MAP; // KEY: GUID of LibBase, VALUE: vector of subtype related info, which need to be recreated/rederived from LibBase
	SUB_PAR_MAP mapOfDeriveds;
#if(DONT_OPTIMIZE)
	typedef std::map< PROJ_ID, UniqueId > SEC_DER_MAP; // KEY: ID in secondary derived object, VALUE: GUID of base in library
	SEC_DER_MAP mapOfSecondaryDeriveds;
#endif
	typedef std::map< CComPtr<IMgaFCO>, UniqueId > SEC_DER_NAP;
	SEC_DER_NAP napOfSecondaryDeriveds;

	
	typedef std::map< CoreObj, std::pair< CoreObj, bool > > PRI_MATCHMAKER_MAP; // KEY: DERD Obj, VALUE: pair of <BASE, is_instance>
	PRI_MATCHMAKER_MAP                    m_primMatchMaker; // holds only the primary deriveds

	typedef std::map< CoreObj, CoreObj > CORE_MATCH_MAP;
	CORE_MATCH_MAP                       m_coreMatchMaker;  // holds both primary and secondary deriveds
#if(DONT_OPTIMIZE)
	// OBSOLETE:
	std::map< PROJ_ID, PROJ_ID>          m_matchMaker; // map of ids already matched objects
						                               // prevents assigning the same
						                               // base to two different objects
#endif						

	// 
	// for connection recreating
	typedef std::vector< elem_struct > CHAIN_VEC;
	struct conndetails_struct
	{
		elem_struct end;
		CHAIN_VEC chain;
	};

	struct connEnds_struct
	{
		conndetails_struct src;
		conndetails_struct dst;
		elem_struct        parent_model;
		CComBSTR           role;
		PROJ_ID            connid;//will prevent loopnode duplication
		CComBSTR           connname;
		bool               is_derived;
		int                is_sec_derived;
		bool               is_instance;
		bool               baseConnInLib;
		UniqueId           baseConnId;
		CComBSTR           guid;
		CoreObj            saver; // will save owned Attributes, Regnodes & Constraints
	};

	typedef std::vector< connEnds_struct > CONN_STORAGE;
	CONN_STORAGE conns_stored;


#if(DONT_OPTIMIZE)
	struct connEnds_struct2
	{
		CComPtr<IMgaFCO>        src;
		CComPtr<IMgaFCO>        dst;
		CComPtr<IMgaFCOs>       srcRefs;
		CComPtr<IMgaFCOs>       dstRefs;
		CComPtr<IMgaModel>      par;
		CComPtr<IMgaMetaRole>   mrole;
		CComBSTR                nm;
		CComBSTR                guid;
	};
	typedef std::vector< connEnds_struct2 > CONN_STORAGE2;
	CONN_STORAGE2 conns_to_recreate;
#endif
	struct derConnEnds_struct
	{
		CComPtr<IMgaModel>  base_parent;
		CComPtr<IMgaModel>  derd_parent;
		CComPtr<IMgaConnection>  b_conn; // the base connection (new connection found in library)
		CComPtr<IMgaMetaRole>    b_conn_role;
		connEnds_struct          b_conn_struct;

		CComPtr<IMgaFCO>        src;
		CComPtr<IMgaFCO>        dst;
		CComPtr<IMgaFCOs>       srcRefs;
		CComPtr<IMgaFCOs>       dstRefs;
		CComBSTR                nm;
	};

	typedef std::vector< derConnEnds_struct > DERCONN_STORAGE;
	DERCONN_STORAGE conns_to_derive; // new connections found in base which must be derived into derd

	struct HasThisConn // unary predicate, to search for a connection in the vector based on ID
	{
		HasThisConn( const PROJ_ID& id_to_store) : m_id( id_to_store) { }
		bool operator () (connEnds_struct& peer) { return peer.connid == m_id; }
	private:
		PROJ_ID m_id;
	};

	struct OldConn // unary predicate, to decide whether existed connections that were derived from p_uid
	{
		OldConn( const UniqueId& p_uid) : m_uid( p_uid) { }
		bool operator () ( connEnds_struct& peer) { return peer.baseConnId == m_uid; }
	private:
		UniqueId m_uid;
	};

	// 
	// for recreating connection memberships in sets
	typedef std::vector< PROJ_ID>          PIDVECTOR;
	typedef std::map< PROJ_ID, PIDVECTOR > MEMBERMAP;
	MEMBERMAP m_mapOfMemberships;


	std::list<BinGuid> lib_stack;// this stack maintains the library root's guid 
							// imagine that a library may contain inner libraries
							// and during dependency storage (collectDependencies)
							// we have to know which library contains the element
							// whose guid we are storing
							//
							// element guids are not unique globally
							// <project_guid, element_guid> pair is unique
							// TODO: this project guid changes every time the project
							// is modified (?)
	std::vector<PROJ_ID> m_changedLibs;
	void addChangedLib( elem_struct& elem);
	// -----------------------------COLLECTORS & RESTORERS----------------------------
	void reapplyLibFlagToChangedLibs();
	void applyLibFlag( CoreObj& p_libObj);

	void DetachObjFromLibBase( const CoreObj& baseObj, CoreObj& derdObj, unsigned long nextrelid, bool prim);
	void AttachDerObjs( const CoreObj& baseObj, CoreObj& derdObj, long instance, bool prim);

	// cuts or detaches libraryderived objects
	void cutDersFromLib( CoreObj& one_fco);
	// does the final reattachment of derived objects
	// to the refreshed base objects (found in the library)
	void reattachSubtypesInstances();// CoreObj& ofolder, CoreObj& nfolder);
	
	// -after the cut we decrease relids to seem consistent
	// (to be consistent with the changed hierarchy)
	// -after the reattach we increase relids to be consistent
	// with the actual restored hierarchy
	void propObjRelidIncV2( CoreObj& orig);

	// aggregates objects in the project which are 
	// derived from library objects
	void saveTopAdaptiveRef( CoreObj& one_ref, CoreObj& bas_ref);
	void saveDerAdaptiveRef( CoreObj& one_ref);

	void collectRefsDerdFromLib( CoreObj& one_ref, CoreObj& bas_ref);
	void collectDersFromLib  ( CoreObj& one_fco);
	void syncStructureFromLib();

	// propagates any change in the base object
	// into subtypes aggregated by collectDersFromLib
	void SyncDerObjs( const CoreObj &freshMasterObj, CoreObj &adaptiveObj, long instance, bool prim, const CoreObj &priMasterObj, const CoreObj &priAdaptiveObj);
	void SyncObjectName( const CoreObj& p_masterObj, CoreObj &p_adaptiveObj);
	void SyncDerSets( const CoreObj &freshMasterSet, CoreObj &adaptiveSet, const CoreObj &freshMasterPar, CoreObj &adaptivePar);
	void SyncDerRefs( const CoreObj &freshMasterRef, CoreObj &adaptiveRef, const CoreObj &priFreshMasterPar, const CoreObj &priAdaptivePar);


	// aggregates references which are in the project
	// and are directed towards the library
	void collectRefsToLib( CoreObj& one_fco);
	void restoreRefsToLib();
	
	// adapts library derived refs to the new situation
	void adaptTopRefs();
	void adaptDerRefs();

	// a mixed connection is an archetype connection in the project
	// (it is not a secondary derived connection)
	void collectMixedConns( CoreObj& one_fco);
	void restoreMixedConns( CoreObj& folder);

	// a fresh connection is a newly appeared connection in a library base object
	// which needs to be propagated down into the derived objects in the project
	// so it will become secondary derived
	void collectFreshConnection( const CoreObj& p_coreConn, const CoreObj& p_base, const CoreObj& p_derd);
	void syncFreshConns();// CoreObj& ofolder, CoreObj& nfolder);

	void collectMembershipOfConn( const CComPtr<IMgaConnection>& p_conn, const connEnds_struct& conn_info);
	void restoreMembership( const CComPtr<IMgaFCO>& p_conn, connEnds_struct& conn_info, const CComPtr<IMgaModel>& parent);


	// ----------------------------------HELPERS---------------------------------------

	void getElemDetails( CComPtr<IMgaFCO>& peer, elem_struct& elem);
	void getElemDetails( CoreObj& peer, elem_struct& elem);

	short distance( CoreObj& derd);
	bool isDerivedRef( CComPtr<IMgaFCO>& p_ref, CComPtr<IMgaFCO>& p_tgt, bool *p_fromLib, bool *p_isRedirected, bool *p_isSecondaryDerd);
	bool isSecondaryDerivedRef( CComPtr<IMgaFCO>& p_ref, CComPtr<IMgaFCO>& p_tgt, bool* p_fromLib);
	bool isSecondaryDerivedRefFromLibraryObj( CComPtr<IMgaFCO>& ref, bool *p_pbIsRedirected);

	bool ignoreFurtherRedir ( CComPtr<IMgaReference>& orig, CComPtr<IMgaFCO>& ref_tgt_newlib);
	// todo: convert to CoreObj parameters
	bool redirectRefWithCare( CComPtr<IMgaReference>& orig, CComPtr<IMgaFCO>& ref_tgt_newlib);

	void saveConnection( const CComPtr<IMgaFCO>& p_cn);
	void restoreDeletedConns( CoreObj& nfolder);
	void getConnPointDetails( CComPtr<IMgaConnPoint>& cp, conndetails_struct& det, bool* p_ptrIsDestination);
	void getConnectionDetails       ( CComPtr<IMgaConnection>& owner, connEnds_struct& one_conn);

	void findAmongNecDerd( UniqueId& p_uid, const CComPtr<IMgaModel>& d_par, CComPtr<IMgaFCO>& res);
	void findAmongSecDerd( UniqueId& p_uid, const CComPtr<IMgaModel>& d_par, CComPtr<IMgaFCO>& res);
	void findAmongPriDerd( UniqueId& p_uid, const CComPtr<IMgaModel>& d_par, CComPtr<IMgaFCO>& res);
	void findAmongContained( const CComPtr<IMgaModel>& d_par, const PROJ_ID& p_id, CComPtr<IMgaFCO>& res);
	bool isInside( const CComPtr<IMgaModel>& d_par, const CComPtr<IMgaFCO>& res);

	void askTheMatchMaker( const CComPtr<IMgaFCO>& end, const CComPtr<IMgaModel>& b_par, const CComPtr<IMgaModel>& d_par, CComPtr<IMgaFCO>& n_end, bool checkParentHood);
	void getLibDerdEquiv( const CComPtr<IMgaFCO>& end, const CComPtr<IMgaModel>& b_par, const CComPtr<IMgaModel>& d_par, CComPtr<IMgaFCO>& n_end, bool checkParentHood = true);
	
	void removeObsoleteChildren( const CoreObj& masterObj, CoreObj& adaptiveobj);
	void updateMatchDb( coreobjpairhash& p_creaList);
	// copy a fresh container into the whole subtype tree
	typedef std::vector< std::pair< CoreObj, CoreObj> > SET_CREALIST;
	typedef std::vector< std::pair< CoreObj, CoreObj> > REF_CREALIST;
	void copyNewArrivals( CoreObj& adaptiveobj, std::vector< CoreObj> & newcomers
		, SET_CREALIST& set_crealist, REF_CREALIST& ref_crealist);

	// for mixed connection recreation
	bool buildRefChainColl( CMgaProject *mgaproject, CoreObj& folder, CHAIN_VEC& p_chain, CComPtr<IMgaFCOs>& p_sref_chain);
	bool followRefChain( const CComPtr<IMgaFCOs>& chain, CComPtr<IMgaModel>& final_m);

	// for storing previously owned properties of mixed connections
	void loadSavedStuff( CComPtr<IMgaFCO>& p_nConn, connEnds_struct& p_one_conn);
	void saveOwnedStuff( CComPtr<IMgaConnection>& owner, connEnds_struct& one_conn);

	// for fresh connection recreation
	CComPtr<IMgaModel> nextContainer( CComPtr<IMgaFCO>& ref);
	CComPtr<IMgaFCO>   nextElem( CComPtr<IMgaFCO>& p_ref);

	bool verifyChain( const CComPtr<IMgaModel>& pM
		, const CComPtr<IMgaFCOs>& sref_chain, const CComPtr<IMgaFCO>& s);

	bool findConnEndPeer( const derConnEnds_struct& c_info
		, const CComPtr<IMgaFCOs>& p_origChain, const CComPtr<IMgaFCO>& p_origConnEnd
		,       CComPtr<IMgaFCOs>& p_peerChain,       CComPtr<IMgaFCO>& p_peerConnEnd);

	bool fillConnectionPtrs( const derConnEnds_struct& c_info
		, CComPtr<IMgaFCO>& s, CComPtr<IMgaFCO>& d
		, CComPtr<IMgaFCOs>& s_chain, CComPtr<IMgaFCOs>& d_chain);

	// for both
	void prepareConnErrMsg( const CComPtr<IMgaModel>& pM
			, const CComPtr<IMgaFCO>& s, const CComPtr<IMgaFCO>& d
			, const CComPtr<IMgaFCOs>& sref_chain, const CComPtr<IMgaFCOs>& dref_chain
			, const connEnds_struct& current_i, MyCComBSTR& msg);

public:
	void collectDependencies( CoreObj& p_container);
	void cutRelations( CoreObj& container);
	void restoreDependencies();
	void clearDepStorage();
	int  getNumOfErrors( MyCComBSTR& msg);
};

class Creator
{
public:
	static const CComBSTR SrcName; // keep in sync with FCO::SrcName in MgaConnection.cpp
	static const CComBSTR DstName; // keep in sync with FCO::DstName

	static HRESULT SimpleConn( CComPtr<IMgaModel>&      p_parent
	                         , CComPtr<IMgaMetaRole>&   p_mrole
	                         , CComPtr<IMgaFCO>&        p_src
	                         , CComPtr<IMgaFCO>&        p_dst
	                         , CComPtr<IMgaFCOs>&       p_srcRefs
	                         , CComPtr<IMgaFCOs>&       p_dstRefs
	                         , CMgaProject*             p_mgaProject
	                         , IMgaFCO**                p_newConn);

private:
	static HRESULT Child          ( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_parent, CComPtr<IMgaMetaRole>& metar, IMgaFCO **newobj);

	static HRESULT put_NamedRole  ( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_conn, BSTR p_rolename, IMgaFCOs* p_refs, IMgaFCO* p_endfco);
	static HRESULT AddConnPoint   ( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_conn, BSTR p_rolename, long p_maxinrole, IMgaFCO* p_target, IMgaFCOs* p_refs, IMgaConnPoint** pVal);
};

class Worker
{
public:
	static HRESULT put_Name     ( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_elem, CComBSTR p_newVal);
	static HRESULT PutGuidDisp  ( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_elem, CComBSTR p_guidStr);
private:
	static HRESULT PutGuid      ( CMgaProject* p_mgaProject, CComPtr<IMgaFCO>& p_elem, long l1, long l2, long l3, long l4);
};

#endif
