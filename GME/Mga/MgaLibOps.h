#pragma once

#include <map>
#include <vector>
class BinGuid;
class Reporter;

class Typedefs
{
public: // typedefs
	typedef std::vector< CoreObj >                                    LIBVEC;
	typedef LIBVEC::iterator                                          LIBVEC_ITER;
	typedef LIBVEC::const_iterator                                    LIBVEC_CITER;

	typedef std::map< BinGuid, LIBVEC >                               LIBMAP;
	typedef LIBMAP::iterator                                          LIBMAP_ITER;
	typedef LIBMAP::const_iterator                                    LIBMAP_CITER;

	typedef std::vector< std::pair< CoreObj, CoreObj > >              LIBPAIRVEC;
	typedef LIBPAIRVEC::iterator                                      LIBPAIRVEC_ITER;
};

class PointerFixup
{
	coreobjpairhash identity_map;

	typedef struct
	{
		CoreObj target;
		CoreObj created;
		attrid_type attrid;
		bool        redirectable;
	} resolve_entry;

	std::vector<resolve_entry> resolve_entries;

	bool                       m_isOptimizing;
	Typedefs::LIBPAIRVEC       m_libPairs;

public:
	PointerFixup();
	void setLibPairs( Typedefs::LIBPAIRVEC& p_libPairs);

	CoreObj findLibRoot( const CoreObj& p_elem);
	CoreObj findCounterpart( const CoreObj& p_libElem);
	CoreObj cntPartLib( const CoreObj& p_lib2);

	inline void identify(const CoreObj &original, const CoreObj &created)
	{
		ASSERT( original != NULL && created != NULL );

		identity_map.insert(coreobjpairhash::value_type(original, created));
	}

	void resolve(const CoreObj &original, const CoreObj &created,
		attrid_type attrid, bool p_skipThis)
	{
		if( LINKREF_ATTR(attrid) )
			return;

		CoreObj target = original[attrid]; 
		if( target == NULL )
			return;

		// if lib1 to be imported contains a lib2
		// while the project contains already
		// a copy of lib2 (as a standalone lib)
		// there is a possibility for optimization
		//
		// if pointer is in lib1 and tgt in lib2, then
		// pointer of lib1 might be redirected to lib2's 
		// existing counterpart in project (lib2copy)
		coreobjpairhash::iterator i = identity_map.find(target);
		if( i != identity_map.end() )
		{
			if( p_skipThis)
			{
#ifdef _DEBUG
				//CComBSTR m;
				//if( original.IsFCO())
				//{
				//	m.Append( original[ATTRID_NAME]);m.Append( "=");
				//	m.Append( created[ATTRID_NAME]);m.Append( ".");
				//	m.Append( target[ATTRID_NAME]); m.Append( " found in lib: ");
				//	CoreObj n = findLibRoot( original);
				//	if( n)
				//		m.Append( n[ATTRID_NAME]); m.Append( ";");
				//}
#endif

				created[attrid] = i->second;// already created
				return;
			}

			CoreObj iseccnpart;
			
			if( m_isOptimizing)
			{
				// i->second might be replaced by its counterpart 
				// in lib2copy
				if( i->second.IsFCO() && i->second.GetMetaID() != DTID_CONNECTION)
					iseccnpart = findCounterpart( i->second);
				// attempts to postpone the decison about these:
				else
				{
#ifdef _DEBUG
					CComBSTR m;
					if( i->second.IsFCO())
					{
						m.Append( original[ATTRID_NAME]);m.Append( "=");
						m.Append( created[ATTRID_NAME]);m.Append( ".");
						m.Append( target[ATTRID_NAME]); m.Append( ";");
					}
					else
					{
						CComBSTR m;
						m.Append( CoreObj(original).GetMgaObj()[ATTRID_NAME]);
						m.Append( ".");
					}
#endif
					resolve_entry entry;
					entry.target = target;
					entry.created = created;
					entry.attrid = attrid;
					entry.redirectable = true;
					resolve_entries.push_back(entry);
					return;
				}
			}

			if( iseccnpart)
			{
				created[attrid] = iseccnpart;
			}
			else
			{
				created[attrid] = i->second;// already created
#ifdef _DEBUG
				//CoreObj tgt = i->second;
				//CComBSTR m;
				//if( created.IsFCO())
				//{
				//	ObjForCore( created)->get_ID( &m);
				//	m.Append( ".");
				//	m.Append( created[ATTRID_NAME]);
				//	if( tgt.IsFCO())
				//		m.Append( tgt[ATTRID_NAME]);
				//	else 
				//		m.Append( tgt.GetMgaObj()[ATTRID_NAME]);
				//	m.Append( "/");
				//}
				//else
				//{
				//	/*CoreObj& p = created.GetMgaObj();
				//	ObjForCore( p)->get_ID( &m);
				//	m.Append( "{");
				//	m.Append( p[ATTRID_NAME]);*/
				//	if( tgt.IsFCO())
				//		m.Append( tgt[ATTRID_NAME]);
				//	else 
				//		m.Append( tgt.GetMgaObj()[ATTRID_NAME]);
				//	m.Append( "}");
				//}
#endif
			}
		}
		else								// resolve it later
		{
			resolve_entry entry;
			entry.target = target;
			entry.created = created;
			entry.attrid = attrid;
			entry.redirectable = !p_skipThis;
			resolve_entries.push_back(entry);
		}
	}

	inline void clear()
	{
		resolve_entries.clear();
		identity_map.clear();
		m_libPairs.clear();
	}

	void fixPointers()
	{
		std::vector<resolve_entry>::const_iterator i = resolve_entries.begin();
		while( i != resolve_entries.end() )
		{
			CoreObj& new_target = identity_map[i->target];

			try {
			ASSERT( new_target != NULL );

			CoreObj iseccnpart;
			if( m_isOptimizing)
			{
				// look for the element's peer in the peer library
				if( new_target.IsFCO() && new_target.GetMetaID() != DTID_CONNECTION)
				{
					iseccnpart = findCounterpart( new_target);
				}
				if( new_target.IsFCO() && new_target.GetMetaID() == DTID_CONNECTION)
				{
					iseccnpart = findCounterpart( new_target);
				}
			}

			if( m_isOptimizing && i->redirectable)
			{
				if( iseccnpart)
					(i->created)[i->attrid] = iseccnpart;
				else
					(i->created)[i->attrid] = new_target;

				// diagnostics
#ifdef _DEBUG
				//if( !iseccnpart && new_target) // iseccnpart = 0, new_target != 0
				//{
				//	CComBSTR nm, id;
				//	CoreObj mp;
				//	if( new_target.IsFCO())
				//	{
				//		nm = new_target[ATTRID_NAME];
				//		ObjForCore( new_target)->get_ID( &id);
				//		
				//	}
				//	else if( mp = new_target.GetMgaObj())
				//	{
				//		nm = mp[ATTRID_NAME];
				//		ObjForCore( mp)->get_ID( &id);
				//	}
				//	else
				//	{
				//		nm = "Null";
				//	}
				//	CComBSTR msg( "Resolution error at ");
				//	msg.Append( nm);
				//	msg.Append( " with ");
				//	msg.Append( id);
				//}
#endif
			}
			else
				(i->created)[i->attrid] = new_target;
			} catch(...) 
			{
#ifdef _DEBUG
				//CComBSTR nm, id;
				//CoreObj mp;
				//if( new_target.IsFCO())
				//{
				//	nm = new_target[ATTRID_NAME];
				//	ObjForCore( new_target)->get_ID( &id);
				//	
				//}
				//else if( mp = new_target.GetMgaObj())
				//{
				//	nm = mp[ATTRID_NAME];
				//	ObjForCore( mp)->get_ID( &id);
				//}
				//else
				//{
				//	nm = "Null";
				//}
				//CComBSTR msg( "Resolution error at ");
				//msg.Append( nm);
				//msg.Append( " with ");
				//msg.Append( id);
#endif
			}

			++i;
		}

		// deal with connections now
		i = resolve_entries.begin();
		while( i != resolve_entries.end() )
		{
			CoreObj& new_target = identity_map[i->target];
			ASSERT( new_target != NULL );

			CoreObj iseccnpart;
			if( m_isOptimizing && i->redirectable)
			{
				try
				{
					if( !new_target.IsFCO() && new_target.GetMgaObj().GetMetaID() == DTID_CONNECTION)
					{
						// needs to be dealt with:
						// in case of DTID_CONNROLE: XREF and MASTEROBJ
						// 
						int mid = new_target.GetMetaID();
						int nid = i->created.GetMetaID();
						if( DTID_CONNROLE == nid) // 107
						{
							ASSERT( mid == nid);
							if( ATTRID_MASTEROBJ == i->attrid) // 517
							{
								CoreObj g = i->created;
								CoreObj derd_conn = g.GetMgaObj();
								CoreObj base_con1 = new_target.GetMgaObj();
								CoreObj base_con2 = findCounterpart( base_con1);
#ifdef _DEBUG
								CComBSTR idofcon2, nmofcon2;
								if( base_con2) 
								{
									nmofcon2 = base_con2[ATTRID_NAME];
									ObjForCore( base_con2)->get_ID( &idofcon2);
								}

								COMTHROW(idofcon2.Append( ";"));
								COMTHROW(idofcon2.AppendBSTR( nmofcon2));
								COMTHROW(idofcon2.Append( "."));
#endif
								CoreObj role_cntpt;
								if( base_con2)
									findConnRoleCounterpart( new_target, base_con1, base_con2, role_cntpt);

								if( role_cntpt)
								{
									// 
									(i->created)[i->attrid] = role_cntpt;
								}
								else
								{
									// what to do with unconnectible connections?
#ifdef _DEBUG
									CComBSTR nm, id;
									CoreObj mp;
									if( new_target.IsFCO())
									{
										nm = new_target[ATTRID_NAME];
										ObjForCore( new_target)->get_ID( &id);
										
									}
									else if( mp = new_target.GetMgaObj())
									{
										nm = mp[ATTRID_NAME];
										ObjForCore( mp)->get_ID( &id);
									}
									else
									{
										nm = "Null";
									}
									CComBSTR msg( "RoleResolution error at ");
									msg.Append( nm);
									msg.Append( " with ");
									msg.Append( id);
#endif
								}
							}
						}
					}
			
				}
				catch (...) {
#ifdef _DEBUG
					CComBSTR nm, id;
					CoreObj mp;
					if( new_target.IsFCO())
					{
						nm = new_target[ATTRID_NAME];
						ObjForCore( new_target)->get_ID( &id);
						
					}
					else if( mp = new_target.GetMgaObj())
					{
						nm = mp[ATTRID_NAME];
						ObjForCore( mp)->get_ID( &id);
					}
					else
					{
						nm = "Null";
					}
					CComBSTR msg( "RoleResolution error at ");
					msg.Append( nm);
					msg.Append( " with ");
					msg.Append( id);
#endif
				}
			}


			++i;
		}

		clear();
	}

	void findConnRoleCounterpart( const CoreObj& p_connRole1, const CoreObj& p_baseConn1, const CoreObj& p_baseConn2, CoreObj& p_connRoleResult) 
	{
		if( !p_connRole1) return;
		if( !p_baseConn1) return;
		if( !p_baseConn2) return;

		CComBSTR cr_rname = p_connRole1[ATTRID_NAME];
		ITERATE_THROUGH( p_baseConn2[ ATTRID_CONNROLE + ATTRID_COLLECTION]) {
			CComBSTR it_rname = ITER[ATTRID_NAME];
			if( it_rname == cr_rname)
			{
				// found:
				if( !p_connRoleResult)
					p_connRoleResult = ITER;
				else
					ASSERT(0); // found twice?
			}
		}

	}
};

class ReplaceDependency // function object
	: public std::unary_function<CComBSTR, void>
{
	      CMgaProject*         m_mgaproject;
	const CComBSTR&            m_lib_id_to_remove;
	const CComBSTR&            m_lib_id_to_add;
	const bool                 m_incOrIncBy;

public:
	ReplaceDependency( CMgaProject*               p_mgaproject
	                 , const CComBSTR&            p_lib_id_to_remove
	                 , const CComBSTR&            p_lib_id_to_add
	                 , bool                       p_incOrIncBy);

	ReplaceDependency::result_type operator()( ReplaceDependency::argument_type p_arg);
};

class Ozer //Optimizer
{
public:

	class StorageMgr // responsible for Storing and Retrieving the information
	{
		static void writer( CoreObj&                p_fldCore
		                  , const CComBSTR&         p_node
		                  , const CComBSTR&         p_value);
		static void reader( CoreObj&                p_fldCore
		                  , const CComBSTR&         p_node
		                  , CComBSTR&               p_value);
	public:
		static const wchar_t* INCLUDED_BY_STR;
		static const wchar_t* INCLUDES_STR;

		static void getIncludes( CoreObj&           p_fldCore
		                       , CComBSTR&          p_idList);
		static void setIncludes( CoreObj&           p_fldCore
		                       , const CComBSTR&    p_idList);

		static void getIncludedBy( CoreObj&           p_fldCore
		                         , CComBSTR&          p_idList);
		static void setIncludedBy( CoreObj&           p_fldCore
		                         , const CComBSTR&    p_idList);
	};

protected:

	static bool removeFromList( const CComBSTR&     p_erasableVal
	                          ,       CComBSTR&     p_valueList);

	static bool addRelation( CoreObj&             p_fldCore
	                       , const CComBSTR&      p_idToAddToReg
	                       , bool                 p_incByOrInc);

	static bool loseRelation  ( CoreObj&          p_fldCore
	                          , const CComBSTR&   p_idToRemoveFromReg
	                          , bool              p_incByOrInc
	                          , bool&             p_libraryBecameFree);

public:

	class DependentIterator  // a dependent library iterator
	{
	public:
		// based on p_incbyOrInc the included or the including libraries are given back
		DependentIterator( const CComBSTR& p_list);
		DependentIterator( const std::string& p_list);

		// traversal methods
		bool        isDone();
		void        next();
		CComBSTR    getCurrentBstr();
		std::string getCurrentStr();

	protected:
		size_t          m_pos;
		size_t          m_nextPos;
		std::string           m_tokenizable;
	};

	static bool isIncluded( CoreObj&  p_fldCore);


	static void cutInclusions( CoreObj&     p_rootfld
	                         , CoreObj&     p_oldLib);

	static void copyIncludedBy( CoreObj&     p_oldLib
	                          , CoreObj&     p_newLib);

	static void createDeps( CoreObj&            p_lib
	                      , Typedefs::LIBVEC&   p_deps);

};

class LibWorker
{
protected: // members

	CMgaProject* m_mgaProject;
	CComBSTR     m_libName;
	bool         m_optimized;

	void showDetails( CoreObj& p_container, Typedefs::LIBMAP& p_results);

public:

	LibWorker( CMgaProject* p_mgaProject
	         , CComBSTR     p_libName
	         , bool         p_optimize);

	bool          isOptimized() const;
	CComBSTR      getConnectionStr() const;
	CComBSTR      getExpandedConnectionStr() const;

	void flatten( CoreObj&     p_rootfolder
	            , CoreObj&     p_libimgroot
	            , Typedefs::LIBVEC& p_depLibs);
};


class LibImgHelper
{
	static void discover( int                       p_recDepth
	                    , CoreObj&                  p_container
	                    , const CComBSTR&           p_avoidThisLib
	                    , Typedefs::LIBMAP&          p_results);


public:
	// functor class for replacing ObjTreeCopyFromExt with its countless parameters
	class DoExtTreeCopy
	{
		CMgaProject*                     m_mgaproject;
		PointerFixup&                    m_fixup;            // a reference!
		Typedefs::LIBPAIRVEC&            m_libPairs;         // a reference!
		Typedefs::LIBVEC&                m_superfluousLibs;  // a reference!
	public:
		DoExtTreeCopy( CMgaProject*            p_mgaproject
		             , PointerFixup&           p_fixup
		             , Typedefs::LIBPAIRVEC&   p_libPairs
		             , Typedefs::LIBVEC&       p_superfluousLibs);

		void operator() ( const CoreObj& p_orig, CoreObj& p_nobj, bool p_indupl);
	};


	static void logCreator( CComBSTR& p_log
	                      , const Typedefs::LIBVEC& p_libsToBe
	                      , const Typedefs::LIBVEC& p_libsHosted);

	static void matchLibs( const Typedefs::LIBMAP&          p_libMap
	                     , const Typedefs::LIBMAP&          p_projMap
	                     , Typedefs::LIBPAIRVEC&            p_machingPairs
	                     , CComBSTR&                        p_msgInfo);

	static void deleteSuperfluousLibs( Typedefs::LIBVEC&        p_superfluousLibs
	                                 , Reporter&                p_reporter);

	static void collectDep( Typedefs::LIBPAIRVEC&         p_matchingInfo
	                      , Reporter&                     p_reporter
	                      , Typedefs::LIBVEC&             p_depLibs);

	static void ungroupLibs( CoreObj&              p_container
	                       , Typedefs::LIBMAP&     p_results
	                       , Typedefs::LIBVEC&     p_depLibs);

	static void recordLibs( bool               p_recordAllLibs
	                      , CoreObj&           p_container
	                      , Typedefs::LIBMAP&  p_results
	                      , const CComBSTR&    p_avoidThisLibrary = "");

	static void GetItsGuid( CoreObj&           p_coreObj
	                      , BSTR*              p_guidDisp);
};
