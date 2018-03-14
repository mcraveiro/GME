// MgaLibOps.cpp : Implementation of Mga library operations
#include "stdafx.h"
#include "MgaFCO.h"
#include "MgaSet.h"
#include "MgaReference.h"
#include "MgaConnection.h"
#include "MgaLibOps.h" // by ZolMol
#include "MgaLibRefr.h"
#include "../interfaces/Parser.h"
#include <algorithm>

/*static*/ const wchar_t* Ozer::StorageMgr::INCLUDES_STR        = L"_includes";
/*static*/ const wchar_t* Ozer::StorageMgr::INCLUDED_BY_STR     = L"_includedBy";

std::wstring getConnStrForLibName(FCO* fco, const TCHAR* libname) {
	std::wstring connStr = libname;
	if (_tcsnicmp(connStr.c_str(), _T("MGA="), 4) != 0 && _tcsnicmp(connStr.c_str(), _T("MGX="), 4) != 0) {
		CComPtr<IMgaProject> project;
		COMTHROW(fco->get_Project(&project));
		_bstr_t bconnStr;
		COMTHROW(project->get_ProjectConnStr(bconnStr.GetAddress()));

		TCHAR currentMgaPath[MAX_PATH];
		TCHAR* tcfilename;
		if (!GetFullPathName(static_cast<const TCHAR*>(bconnStr)+4, MAX_PATH, currentMgaPath, &tcfilename) || tcfilename == 0) {
		} else {
			*(tcfilename-1) = '\0';
			TCHAR combined[MAX_PATH];
			PathCombine(combined, currentMgaPath, connStr.c_str());
			connStr = std::wstring(_T("MGA=")) + combined;
		}
	}
	return connStr;
}

HRESULT FCO::get_LibraryName(BSTR *libname) {
	COMTRY {
		CHECK_OUTSTRPAR(libname);
		CheckRead();
		if(self[ATTRID_PERMISSIONS] & LIBROOT_FLAG) {
			CComBSTR nn = self[ATTRID_NAME];
			*libname = nn.Detach();
		}
		else *libname = NULL;
	} COMCATCH(;);
}


void setlibpermall(CoreObj &c, long newmask) {
	ASSERT(newmask == LIBRARY_FLAG || newmask == 0);
	ITERATE_THROUGH(c[ATTRID_PARENT+ATTRID_COLLECTION]) {
		if( newmask == 0) // remove all library flags (LIBRARY_FLAG, LIBROOT_FLAG), preserve only INSTANCE
			ITER[ATTRID_PERMISSIONS] = ITER[ATTRID_PERMISSIONS] & INSTANCE_FLAG;
		else // if applying LIBRARY_FLAG, preserve the LIBROOT_FLAG if previously present
			ITER[ATTRID_PERMISSIONS] = (ITER[ATTRID_PERMISSIONS] & (INSTANCE_FLAG|LIBROOT_FLAG)) | newmask;
		if(ITER.IsContainer()) setlibpermall(ITER, newmask);
	}
}	

HRESULT FCO::put_LibraryName(BSTR libname) {
	COMTRY_IN_TRANSACTION {
		CHECK_INSTRPAR(libname);
		CheckWrite();
		if(CComBSTR(libname)) {
			self[ATTRID_PERMISSIONS] = LIBROOT_FLAG;
			self[ATTRID_NAME] = libname;
			setlibpermall(self, LIBRARY_FLAG);
			SelfMark(OBJEVENT_PROPERTIES);
		}
		else {
			self[ATTRID_PERMISSIONS] = 0;
			setlibpermall(self, 0);
		}
	} COMCATCH_IN_TRANSACTION(;);
}

void FCO::prepareDepInfo2Coll( CComBSTR& pInfo, IMgaFolders** pResColl)
{
	CREATECOLLECTION_FOR(IMgaFolder,q);	

	for( Ozer::DependentIterator it( pInfo); !it.isDone(); it.next())
	{
		CComBSTR current_val = it.getCurrentBstr();
		CoreObj lib;
		CComPtr<IMgaFolders> coll;
		if( SUCCEEDED( mgaproject->GetTopLibraries( current_val, &coll)))
		{
			// append coll to q
			long len = 0;
			if( coll) COMTHROW( coll->get_Count( &len));

			for( long i = 1; i <= len; ++i)
			{
				CComPtr<IMgaFolder> ele;
				COMTHROW( coll->get_Item( i, &ele));

				ASSERT( ele);
				if( ele)
					q->Add( ele);
			}
		}
		else
		{
			ASSERT( 0);
		}
	}
	
	*pResColl = 0;
	long res_length = 0;
	if( q) COMTHROW( q->get_Count( &res_length));
	if( res_length)
		*pResColl = q.Detach();
}


void FCO::prepareColl2DepInfo( IMgaFolders* pColl, CComBSTR& pResBstr)
{
	long len = 0;
	if( pColl) COMTHROW( pColl->get_Count( &len));
	for( long i = 1; i <= len; ++i)
	{
		CComPtr<IMgaFolder> ele;
		COMTHROW( pColl->get_Item( i, &ele));

		ASSERT( ele);
		if( ele)
		{
			CComBSTR gd;
			COMTHROW( ele->GetGuidDisp( &gd));
			
			// '\n' delimited list created (no '\n' at the end)
			if( pResBstr && pResBstr.Length() > 0)
				COMTHROW(pResBstr.Append( "\n"));
			COMTHROW(pResBstr.AppendBSTR( gd));
		}
	}
}

HRESULT FCO::GetVirtuallyIncludedBy( IMgaFolders** pDep)
{
	COMTRY {
		CHECK_OUTPAR( pDep);
		CheckRead();
		if(self[ATTRID_PERMISSIONS] & LIBROOT_FLAG) {

			CComBSTR info;	
			Ozer::StorageMgr::getIncludedBy( self, info);
			prepareDepInfo2Coll( info, pDep);
		}
		else *pDep = NULL;
	} COMCATCH(;);
}
HRESULT FCO::SetVirtuallyIncludedBy( IMgaFolders*  pDep)
{
	COMTRY_IN_TRANSACTION {
		CHECK_INPAR( pDep);
		CheckWrite();

		CComBSTR info;
		prepareColl2DepInfo( pDep, info);
		Ozer::StorageMgr::setIncludedBy( self, info);

		SelfMark(OBJEVENT_REGISTRY);

	} COMCATCH_IN_TRANSACTION(;);
}
HRESULT FCO::GetVirtuallyIncludes  ( IMgaFolders** pDep)
{
	COMTRY {
		CHECK_OUTPAR( pDep);
		CheckRead();
		if(self[ATTRID_PERMISSIONS] & LIBROOT_FLAG) {

			CComBSTR info;
			Ozer::StorageMgr::getIncludes( self, info);
			prepareDepInfo2Coll( info, pDep);
		}
		else *pDep = NULL;
	} COMCATCH(;);
}
HRESULT FCO::SetVirtuallyIncludes  ( IMgaFolders*  pDep)
{
	COMTRY_IN_TRANSACTION {
		CHECK_INPAR( pDep);
		CheckWrite();

		CComBSTR info;
		prepareColl2DepInfo( pDep, info);
		Ozer::StorageMgr::setIncludes( self, info);

		SelfMark(OBJEVENT_REGISTRY);

	} COMCATCH_IN_TRANSACTION(;);
}
/* *************************** Pointer Fixup ************************************** */
/*
by ZolMol
class PointerFixup
{
	coreobjpairhash identity_map;

	typedef struct
	{
		CoreObj target;
		CoreObj created;
		attrid_type attrid;
	} resolve_entry;

	std::vector<resolve_entry> resolve_entries;

public:
	inline void identify(const CoreObj &original, const CoreObj &created)
	{
		ASSERT( original != NULL && created != NULL );

		identity_map.insert(coreobjpairhash::value_type(original, created));
	}

	void resolve(const CoreObj &original, const CoreObj &created,
		attrid_type attrid)
	{
		if( LINKREF_ATTR(attrid) )
			return;

		CoreObj target = original[attrid]; 
		if( target == NULL )
			return;

		coreobjpairhash::iterator i = identity_map.find(target);
		if( i != identity_map.end() )
			created[attrid] = i->second;	// already created
		else								// resolve it later
		{
			resolve_entry entry;
			entry.target = target;
			entry.created = created;
			entry.attrid = attrid;
			resolve_entries.push_back(entry);
		}
	}

	inline void clear()
	{
		resolve_entries.clear();
		identity_map.clear();
	}

	void fixPointers()
	{
		std::vector<resolve_entry>::const_iterator i = resolve_entries.begin();
		while( i != resolve_entries.end() )
		{
			CoreObj& new_target = identity_map[i->target];
			ASSERT( new_target != NULL );

			(i->created)[i->attrid] = new_target;

			++i;
		}
		clear();
	}
};
*/
/* *************************** Attach ********************************************* */

// functor class for replacing ObjTreeCopyFromExt with its countless parameters
LibImgHelper::DoExtTreeCopy::DoExtTreeCopy( CMgaProject*            p_mgaproject
                                          , PointerFixup&           p_fixup
                                          , Typedefs::LIBPAIRVEC&   p_libPairs
                                          , Typedefs::LIBVEC&       p_superfluousLibs)
         : m_mgaproject( p_mgaproject)
         , m_fixup( p_fixup)                        // a reference!, will be altered
         , m_libPairs( p_libPairs)                  // a reference!, will be altered
         , m_superfluousLibs( p_superfluousLibs)    // a reference!, will be altered
{
}

void LibImgHelper::DoExtTreeCopy::operator ()( const CoreObj& orig, CoreObj& nobj, bool p_indupl)
{
	metaid_type s;
	COMTHROW( m_mgaproject->dataproject->CreateObject(s = orig.GetMetaID(), &nobj.ComPtr()));

	m_fixup.identify(orig, nobj);


	bool skip_this = false;
	if(s >= DTID_MODEL && s <= DTID_FOLDER) {
		setcheck( m_mgaproject, nobj, CHK_NEW);
		CoreObjMark(nobj, OBJEVENT_CREATED);

		long pm = orig[ATTRID_PERMISSIONS];
		if( pm & LIBROOT_FLAG)
		{
			BinGuid libn = Identifier::getPersistentIdOf( orig);
			for( Typedefs::LIBPAIRVEC_ITER it = m_libPairs.begin(), en = m_libPairs.end()
				; it != en
				; ++it)
			{
				//
				BinGuid lib1 = Identifier::getPersistentIdOf( it->first);
				if( libn == lib1)
				{
					// what about skipping to load this duplicate library?
					// we can't avoid yet loading such portions into memory

					skip_this = true;
					//ASSERT( libn == lib1 == lib2);
					ASSERT( libn == Identifier::getPersistentIdOf( it->second));
					m_superfluousLibs.push_back( nobj);
				}
			}
		}

	}

	CComPtr<ICoreAttributes> atts;
	COMTHROW(orig->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));
			if(ai < ATTRID_COLLECTION) {
				if (ai == ATTRID_LOCK)
					continue;
				valtype_type mvt;
				COMTHROW(mattr->get_ValueType(&mvt));
				if(mvt != VALTYPE_POINTER) {
					if(ai == ATTRID_PERMISSIONS) {
						//WAS: nobj[ai] = orig[ai] & INSTANCE_FLAG | LIBRARY_FLAG;
						//this way we preserve nested libraries:
						
						// we will delete this object later, that's why 
						// we don't apply a readonly flag to it now
						if( skip_this) 
							nobj[ai] = orig[ai]; 
						else
							nobj[ai] = orig[ai] & (INSTANCE_FLAG|LIBROOT_FLAG) | LIBRARY_FLAG;
					}
					else nobj[ai] = static_cast<CComVariant>(orig[ai]);
				}
				else {
					m_fixup.resolve(orig, nobj, ai, p_indupl || skip_this);
				}
			}
			else {
				ai -= ATTRID_COLLECTION;
				if(LINKREF_ATTR(ai)) {
					CoreObjs collmembers = orig[ai + ATTRID_COLLECTION];
					ITERATE_THROUGH(collmembers) {
						CoreObj nchild;
						//ObjTreeCopyFromExt( ITER, nchild, p_indupl || skip_this);
						(*this)( ITER, nchild, p_indupl || skip_this);
						nchild[ai] = nobj;
					}
				}
			}
	} MGACOLL_ITERATE_END;
}
									   
void CreateLibraryImage(CMgaProject *mgaproject, LibWorker& lw, CoreObj &libimgroot, bool p_tolerateOldMga, bool *p_ptrIsOldMga, Typedefs::LIBMAP& p_results, Typedefs::LIBVEC& p_dependentLibs, CComBSTR& p_infoMsg, const CComBSTR& libname) {
	CComBSTR connstr = lw.getExpandedConnectionStr();
	CComPtr<IMgaProject> p;
	PointerFixup fixup;

	Typedefs::LIBVEC         superfluous_libs; // those libs which can be deleted simply after the optimization 
	Typedefs::LIBPAIRVEC     matching_libs;    // lib pairs (or triplets) which have equal guid

	Reporter reporter( mgaproject);

	try {
		COMTHROW(p.CoCreateInstance(__uuidof(MgaProject)));
		
		CComBSTR paradigmname;
		CComVariant paradigmGUID;
		COMTHROW(mgaproject->get_MetaName(&paradigmname));
		COMTHROW(mgaproject->get_MetaGUID(&paradigmGUID));

		// ask some info about the project to be attached
		bool upgraded = false; CComBSTR connstr_upgraded( "MGA=");
		long mga_ver( 0);
		// checking project version, checking if the same paradigm or subparadigm, and convert thru XML in such cases
		{
			CComBSTR par_nm;
			CComBSTR par_vs;     // not used now
			CComVariant par_gd;  // not used now
			VARIANT_BOOL ro_mode;// not used now 

			COMTHROW(p->QueryProjectInfo( connstr, &mga_ver, &par_nm, &par_vs, &par_gd, &ro_mode));
			if( par_nm != paradigmname)
			{
				// must upgrade 'connstr' project into 'paradigmname' paradigm
				VARIANT_BOOL ro = VARIANT_TRUE;
				HRESULT hr = p->Open( connstr, &ro);
				if( FAILED( hr)) {
					reporter.show( CComBSTR( "Could not open the to-be-library project."));
					return;
				}

				// Find TEMP directory
				TCHAR szTempPath[MAX_PATH];
				if (::GetTempPath(MAX_PATH, szTempPath) == 0) {
					reporter.show( CComBSTR( "Could not find TEMP directory."));
					return;
				}

				// Create temporary filenames 
				TCHAR szTempXmeFileName[MAX_PATH];
				TCHAR szTempMgaFileName[MAX_PATH];
				if( ::GetTempFileName(szTempPath, _T("GME"), 0, szTempXmeFileName) == 0  // 0 means ERROR
				 || ::GetTempFileName(szTempPath, _T("GME"), 0, szTempMgaFileName) == 0) {
					ASSERT(("Unable to get temporary filename.", false));
					reporter.show( CComBSTR( "Could not get temporary filename."));
					return;
				}

				// Dump the to-be-library into XML
				CComPtr<IMgaDumper> dumper;
				COMTHROW( dumper.CoCreateInstance(L"Mga.MgaDumper") );
				ASSERT( dumper != NULL );

				COMTHROW(dumper->put_FormatVersion(0));
				COMTHROW(dumper->DumpProject( p, _bstr_t(szTempXmeFileName)) );

				COMTHROW(p->Close(VARIANT_FALSE));

				// Create a new 'paradigmname' project
				COMTHROW(connstr_upgraded.Append( szTempMgaFileName)); // connection string prepared
				hr = p->CreateEx(connstr_upgraded, paradigmname, paradigmGUID);
				if( SUCCEEDED( hr)) {
					CComPtr<IMgaParser> parser;
					COMTHROW( parser.CoCreateInstance(L"Mga.MgaParser") );
					ASSERT( parser != NULL );

					COMTHROW(parser->ParseProject( p, _bstr_t(szTempXmeFileName)) );
					COMTHROW(p->Close(VARIANT_FALSE));
					upgraded = true;
				}
				else if(hr == E_MGA_PARADIGM_NOTREG || hr == E_MGA_PARADIGM_INVALID) {
					ASSERT(0); // how come? these are similar to mgaproject's data
				}
			}
		}

		*p_ptrIsOldMga = mga_ver < 2;

		// check version of the mga file to be attached
		if( mga_ver < 2)
		{
			if( p_tolerateOldMga)
			{
				MyCComBSTR msg( L"Library copy is in old MGA format. Library Refresh feature ver.1 (old) can be used only!");
				COMTHROW(msg.Append( L"To benefit from the new Library Refresh ver.2 feature please open/save the library with GME, then reattach it to this project."));
				reporter.show( msg);
			}
			else
			{
				MyCComBSTR msg( L"Library is in old MGA format. To update please open it as a project, save it and only then can be attached!");
				reporter.show( msg);
				
				COMTHROW(E_MGA_NOT_SUPPORTED);
			}
		}

		try 
		{
			p->__OpenEx( _bstr_t(upgraded?connstr_upgraded:connstr), _bstr_t(paradigmname), _variant_t(paradigmGUID));

			p->__BeginTransaction( NULL, TRANSACTION_READ_ONLY);

			CComPtr<IMgaFolder> libroot;
			COMTHROW(p->get_RootFolder(&libroot));
			CoreObj ext_libroot( libroot);

			//***********************************************************
			// Loading info about the libraries in the (attachable) file
			//-----------------------------------------------------------
			Typedefs::LIBMAP lib_results;
			try
			{
				LibImgHelper::recordLibs( true, ext_libroot, lib_results); // record all libs in library
				// the rootfolder of library is not yet a LIBROOT
				// so consider it a libroot now by pushing to the end of vector explicitly:
				//BinGuid libn = Identifier::getPersistentIdOf( ext_libroot);
				//lib_results[ libn ].push_back( ext_libroot);
				// above: commented out because we can't optimize away the newly attached library

				LibImgHelper::matchLibs( lib_results, p_results, matching_libs, p_infoMsg);
				lib_results.clear();
			}
			catch( ... )
			{
				matching_libs.clear(); // reset collected data (integrity not assured)
				lib_results.clear();

				CComBSTR msg( "Exception while analyzing library ");
				COMTHROW(msg.Append( connstr));
				COMTHROW(msg.Append( " !"));
				
				reporter.show( msg);

				throw hresult_exception( -1); // fail
			}

			if( lw.isOptimized()) fixup.setLibPairs( matching_libs);
			else                  matching_libs.clear();

			LibImgHelper::DoExtTreeCopy( mgaproject, fixup, matching_libs, superfluous_libs)( ext_libroot, libimgroot, false);

			fixup.fixPointers();
		}	
		catch(_com_error&)
		{
			CComBSTR msg( "Exception while loading external library ");
			COMTHROW(msg.Append( connstr));
			COMTHROW(msg.Append( " ."));
			reporter.show( msg);

			throw;
		}
		catch(...)
		{
			CComBSTR msg( "Exception while loading external library ");
			COMTHROW(msg.Append( connstr));
			COMTHROW(msg.Append( " ."));
			reporter.show( msg);

			throw hresult_exception( -1); // fail
		}

		//*************************************************
		// deleting the superfluous libs
		//*************************************************
		if( lw.isOptimized())
		{
			LibImgHelper::deleteSuperfluousLibs( superfluous_libs, reporter);
		}

		libimgroot[ATTRID_NAME] = libname;
		libimgroot[ATTRID_PERMISSIONS] = LIBROOT_FLAG;
		
		//*************************************************
		// collect the dependent (included by me) libraries
		//*************************************************
		if( lw.isOptimized())
		{
			LibImgHelper::collectDep( matching_libs, reporter, p_dependentLibs);
		}
		//else {}// not optimized, is it needed?

		superfluous_libs.clear();
		matching_libs.clear();

		COMTHROW(p->AbortTransaction());
		COMTHROW(p->Close(VARIANT_FALSE));
	} catch(...) { 
		fixup.clear();
		superfluous_libs.clear();
		matching_libs.clear();
		if(p) {
			long st;
			COMTHROW(p->get_ProjectStatus(&st));
			if(st & 8)
				p->AbortTransaction();
			if(st & 1)
				p->Close(VARIANT_FALSE);
		};
		throw;
	}
}

HRESULT FCO::doAttach( BSTR libname, VARIANT_BOOL ungroup, IMgaFolder **f) {
	COMTRY_IN_TRANSACTION {
		CHECK_OUTPTRPARVALIDNULL(f);
			
		Reporter reporter( mgaproject);

		// examining toplevel libraries in project

		// 'self' here is the rootfolder of the project
		Typedefs::LIBMAP top_libs;

		LibWorker lw( mgaproject, CComBSTR(getConnStrForLibName(this, libname).c_str()), ungroup == VARIANT_TRUE);
		SearchTool::m_optimized = lw.isOptimized();
		LibImgHelper::recordLibs( false, self, top_libs); // consider existing toplibs only (self = project)
		
		// examining the attachable library
		CComBSTR msg( "--------------------- Creating ");
		if( lw.isOptimized())
			COMTHROW(msg.Append( "optimized "));
		COMTHROW(msg.Append("library image from ["));
		COMTHROW(msg.Append( lw.getExpandedConnectionStr()));
		COMTHROW(msg.Append("]--"));
		reporter.show( msg, false);

		CoreObj libimgroot;
		bool accept_old_lib( false); // don't tolerate old format MGA files (attachment of such a library fails)
		bool is_old_lib( false);
		Typedefs::LIBVEC dep_libs; // those libraries which are factored out
		CComBSTR infmsg; // informative message about the lib pairs found
		CreateLibraryImage(mgaproject, lw, libimgroot, accept_old_lib, &is_old_lib, top_libs, dep_libs, infmsg, CComBSTR(libname));
		if( lw.isOptimized() && infmsg && infmsg.Length() > 0) // informative message about possible guid duplications in the library
			reporter.show( infmsg, false);

		assignnewchild(libimgroot);

		if( lw.isOptimized())
		{
			// more libraries factored out
			lw.flatten( self, libimgroot, dep_libs);

			// store dependency info on factored out libs
			Ozer::createDeps( libimgroot, dep_libs);
		}
		else ASSERT( dep_libs.empty());

		docheck(mgaproject);

		if(f) ObjForCore(libimgroot)->getinterface(f);

		ObjTreeNotify(mgaproject, libimgroot);
		SelfMark(OBJEVENT_NEWCHILD);
	} COMCATCH_IN_TRANSACTION(;);
}


HRESULT FCO::AttachLibraryV3(BSTR libname, VARIANT_BOOL ungroup, IMgaFolder **f) {
	return doAttach( libname, ungroup, f);
}

HRESULT FCO::AttachLibrary(BSTR libname, IMgaFolder **f) {
	return doAttach( libname, VARIANT_FALSE, f);
}

/* *************************** Refresh ********************************************* */

void steal(CoreObj &o, CoreObj &n, attrid_type ai) {
	ITERATE_THROUGH(o[ai + ATTRID_COLLECTION]) {
		ITER[ai] = n;
	}
}

void redo_derivs(CMgaProject *mgaproject, CoreObj &oldnode, CoreObj &newnode, bool deriv_non_primary) {
	bool libnode = (newnode[ATTRID_PERMISSIONS] & LIBRARY_FLAG) ? true : false;
	if(long(oldnode[ATTRID_META]) != long(newnode[ATTRID_META])) {
		COMTHROW(E_MGA_LIB_DIFF);
	}
	if(oldnode.IsFCO()) {
		CoreObjs ders = oldnode[ATTRID_DERIVED+ATTRID_COLLECTION];
		ITERATE_THROUGH(ders) {
			long perm = ITER[ATTRID_PERMISSIONS];
			if(perm & LIBRARY_FLAG) continue;

			if(!deriv_non_primary && ITER[ATTRID_RELID] >= RELIDSPACE) continue;  // not primary derived
// 
			CoreObj newder;
			coreobjpairhash crealist;
			ObjTreeDerive(mgaproject, newnode, newder, crealist, perm & INSTANCE_FLAG);  
			newder[ATTRID_ROLEMETA] = ITER[ATTRID_ROLEMETA];  // maybe 0
			newder[ATTRID_PARENT] = ITER[ATTRID_PARENT];
			newder[ATTRID_RELID] = ITER[ATTRID_RELID];
							
			ObjTreeReconnect(newder, crealist); // Reroute references
			redo_derivs(mgaproject, ITER, newder, true);  // RECURSION!
		}
	}
	if(oldnode.IsContainer()) {
		CoreObjs oldchildren = oldnode[ATTRID_PARENT+ATTRID_COLLECTION];
		Sort<long>(oldchildren, ATTRID_RELID);
		CoreObjs newchildren = newnode[ATTRID_PARENT+ATTRID_COLLECTION];
		Sort<long>(newchildren, ATTRID_RELID);
		std::vector<CoreObj> newvect;
		GetAll2<ICoreObjects>(newchildren, newvect);
		std::vector<CoreObj>::iterator iter = newvect.begin(), iter_end = newvect.end();
		if(iter != iter_end) {
			long newrelid = (*iter)[ATTRID_RELID];
			ITERATE_THROUGH(oldchildren) {
				long oldrelid = ITER[ATTRID_RELID];
				while(newrelid < oldrelid && iter != iter_end )
				{
					if( ++iter != iter_end )
						newrelid = (*iter)[ATTRID_RELID];
				}
				// no refresh for objects with 0 relID
				if(oldrelid != 0 && oldrelid == newrelid) {
					redo_derivs(mgaproject, ITER, *iter, false);
				}
				else if(oldrelid < RELIDSPACE && !libnode) {
					ITER[ATTRID_PARENT] = newnode;
				}
			}
		}
	}

	// PETER : reroute references from the model
	if (libnode && oldnode.IsFCO()) {
		// Sets ? COnnections ? Other relations ?
		ITERATE_THROUGH(oldnode[ATTRID_REFERENCE + ATTRID_COLLECTION]) {
			if (ITER[ATTRID_PERMISSIONS] & LIBRARY_FLAG) continue;
			ITER[ATTRID_REFERENCE] = newnode;
		}
	}
	// PETER END

	if (libnode) 
		return;

	ASSERT(newnode[ATTRID_RELID] == oldnode[ATTRID_RELID]);
	ASSERT(newnode[ATTRID_META] == oldnode[ATTRID_META]);

	newnode[ATTRID_NAME] = oldnode[ATTRID_NAME];
	if(oldnode.IsContainer()) {
		newnode[ATTRID_LASTRELID] = oldnode[ATTRID_LASTRELID];
	}
	steal(oldnode, newnode, ATTRID_CONSTROWNER);

	if(oldnode.IsFCO()) {
		ASSERT(newnode[ATTRID_ROLEMETA] == oldnode[ATTRID_ROLEMETA]);
		steal(oldnode, newnode, ATTRID_ATTRPARENT);
		steal(oldnode, newnode, ATTRID_XREF);
		steal(oldnode, newnode, ATTRID_REFERENCE);
		switch(oldnode.GetMetaID()) {
		case DTID_REFERENCE:
			steal(oldnode, newnode, ATTRID_SEGREF);
			if(!CoreObj(oldnode[ATTRID_MASTEROBJ])) {
				std::vector<CoreObj> peer(1);
				peer[0] = oldnode[ATTRID_REFERENCE];   // set the old val, it will
													   // be updated through xref
				newnode[ATTRID_MASTEROBJ] = NULLCOREOBJ;
				putreftask(false).DoWithDeriveds(newnode, &peer);
			}
			break;
		case DTID_CONNECTION:
			{
				ITERATE_THROUGH(oldnode[ATTRID_CONNROLE + ATTRID_COLLECTION]) {
					if(!CoreObj(ITER[ATTRID_MASTEROBJ])) {
						CoreObj rr;
						if(ObjForCore(newnode)->findroleobj(CComBSTR(ITER[ATTRID_NAME]), rr)) {
							MgaConnPointDelete(rr);
						}
// todo: propagate
						ITER[ATTRID_CONNROLE] = newnode;
					}
				}
			}
			break;
		case DTID_SET:
			{
				ITERATE_THROUGH(oldnode[ATTRID_SETMEMBER + ATTRID_COLLECTION]) {
					if(!CoreObj(ITER[ATTRID_MASTEROBJ])) {
// todo: delete duplicate setnodes
// todo: propagate
						ITER[ATTRID_SETMEMBER] = newnode;
					}
				}
			}
			break;
		}
	}
}

HRESULT FCO::doRefresh( BSTR libname, VARIANT_BOOL ungroup, long *ptrNumOfErrors) {
	long placeholder;
	if( !ptrNumOfErrors) // invalid ptr
		ptrNumOfErrors = &placeholder;
	*ptrNumOfErrors = 0;

	COMTRY_IN_TRANSACTION {
		Reporter reporter(mgaproject);
		if( !Identifier::isRefreshableLibRoot( self))
		{
			MyCComBSTR msg( "Inner library can't be refreshed in this project!");
			reporter.show( msg);
		} 
		else 
		{
			CheckWrite();
			if(!(self[ATTRID_PERMISSIONS] & LIBROOT_FLAG)) {
				COMTHROW(E_MGA_OP_REFUSED);
			}

			std::wstring connStr = getConnStrForLibName(this, libname).c_str();
			LibWorker lw( mgaproject, CComBSTR(connStr.c_str()), ungroup  == VARIANT_TRUE);
			SearchTool::m_optimized = lw.isOptimized();

			// examining toplevel libraries in project

			CComPtr<IMgaFolder> prroot;
			COMTHROW(mgaproject->get_RootFolder(&prroot));
			CoreObj corerf = CoreObj( prroot);
			//CoreObj crlibroot( self);
			// 'self' here is the refreshable library
			CComBSTR id_of_this_library;
			ObjForCore( self)->get_ID( &id_of_this_library);

			// in the project consider existing toplibs only, [and avoid the library we are going to refresh]
			Typedefs::LIBMAP lib_results;
			LibImgHelper::recordLibs( false, corerf, lib_results, id_of_this_library);

			// examining the refreshable library
			CComBSTR msg( "--------------------- Creating ");
			if( lw.isOptimized())
				COMTHROW(msg.Append( "optimized "));
			COMTHROW(msg.Append("library image from ["));
			COMTHROW(msg.Append( lw.getExpandedConnectionStr()));
			COMTHROW(msg.Append("]--"));
			reporter.show( msg, false);
//
//  If required another logic could be introduced later:
//  If two or more copies of the same lib (or GUID conlfict) found on toplevel 
//  then we could restrict optimization to not optimize (thus remove) such copies. 
//
//#if(0)
//			Typedefs::LIBMAP my_inner_lib_results;
//			// in the refreshable lib examine the whole lib
//			LibImgHelper::recordLibs( true, self, my_inner_lib_results);
//
//			for( Typedefs::LIBMAP_ITER it = my_inner_lib_results.begin(), endit = my_inner_lib_results.end(); it != endit; ++it)
//			{
//				for( Typedefs::LIBVEC_ITER jt = it->second.begin(), endjt = it->second.end(); jt != endjt; ++jt)
//				{
//					CComBSTR id;
//					LibImgHelper::GetItsGuid( *jt, &id);
//					Typedefs::LIBMAP_ITER kt = lib_results.find( it->first);
//					if( kt != lib_results.end()) // 
//					{
//						ASSERT( kt->first == it->first);
//						// look for 'id' in vector kt->second
//						for( std::vector< CoreObj >::iterator lt = kt->second.begin()
//							; lt != kt->second.end()
//							; ++lt)
//						{
//							CComBSTR id2;
//							LibImgHelper::GetItsGuid( *lt, &id2);
//							if( id == id2) // found
//							{
//								ASSERT( 0 );
//								// recordlibs should have ignored the copies which are refershed
//								kt->second.erase( lt);
//								CComBSTR nsg;
//								nsg.Append( id);
//								nsg.Append( L" has been deleted from common map.");
//								reporter.show( nsg, false);
//								break;
//							}
//						}
//					}
//				}
//			}
//#endif

			CComBSTR ln(libname);
			if(!ln) COMTHROW(get_LibraryName(&ln));
			CoreObj libimgroot;
			bool accept_old_lib( true); // refreshment of an old format MGA file is allowed
			bool is_old_lib_copy( false);
			Typedefs::LIBVEC dep_libs; // those libraries which are factored out
			CComBSTR infmsg;
			CreateLibraryImage(mgaproject, lw, libimgroot, accept_old_lib, &is_old_lib_copy, lib_results, dep_libs, infmsg, CComBSTR(libname));
			if( lw.isOptimized() && infmsg && infmsg.Length() > 0) // informative message about possible guid duplications in the library
				reporter.show( infmsg, false);
	 
			libimgroot[ATTRID_PARENT] = self[ATTRID_PARENT];
			libimgroot[ATTRID_RELID] = self[ATTRID_RELID];

			if( is_old_lib_copy) // do a plain old refresh
			{
				MyCComBSTR msg( "Old version of Library refresh started [");
				COMTHROW(msg.AppendBSTR( libname));
				COMTHROW(msg.Append( "] --"));
				reporter.show( msg, false);

				redo_derivs(mgaproject, self, libimgroot, false);
				inDeleteObject();
				docheck(mgaproject);
			}
			else // do a uid based refresh
			{
				MyCComBSTR msg( "---------------------Library refresh v2 started [");
				COMTHROW(msg.AppendBSTR( libname));
				COMTHROW(msg.Append( "] --"));
				reporter.show( msg, false);
				
				RefreshManager rm( mgaproject, self, libimgroot, Ozer::isIncluded( self));
				ASSERT( Identifier::isLibRoot( self));

				rm.collectDependencies( self);
				rm.cutRelations( self);
				//redo_derivs(mgaproject, self, libimgroot, false);
				rm.restoreDependencies();
				rm.clearDepStorage();

				// remove from all included libraries the 'dependent on self' info
				Ozer::cutInclusions( corerf, self);
				// including libraries of self will continue to include the newself (=libimgroot)
				// but we need to copy the includedness of self into newself
				Ozer::copyIncludedBy( self, libimgroot);

				inDeleteObject();

				if( lw.isOptimized())
				{
					// more libraries factored out
					lw.flatten( corerf, libimgroot, dep_libs);

					// store dependency info on factored out libs
					Ozer::createDeps( libimgroot, dep_libs);
				} 
				else ASSERT( dep_libs.empty());

				try {
					docheck(mgaproject);

					MyCComBSTR msg;
					COMTHROW(msg.Append( "----------------------Library refresh done ["));
					COMTHROW(msg.AppendBSTR( libname));
					COMTHROW(msg.Append("]--------"));
					int num = rm.getNumOfErrors( msg);
					*ptrNumOfErrors = num;
					COMTHROW(msg.Append( "--"));
					reporter.show( msg, false);

				} catch(hresult_exception& ) {
					MyCComBSTR msg( "Check failed after refresh!");
					reporter.show( msg);
					int num = rm.getNumOfErrors( msg);
					*ptrNumOfErrors = num + 1;
					throw;
				}
			}
		} // else
	} COMCATCH_IN_TRANSACTION( if( *ptrNumOfErrors == 0) *ptrNumOfErrors = 1; Reporter(mgaproject).show( MyCComBSTR( "----------------------Library refresh failed----------------------")));
}

HRESULT FCO::RefreshLibraryV3(BSTR libname, VARIANT_BOOL ungroup, long *ptrNumOfErrors) {
	return doRefresh( libname, ungroup, ptrNumOfErrors);
}

HRESULT FCO::RefreshLibrary(BSTR libname) {
	long numOfErrs( 0);
	return doRefresh( libname, VARIANT_FALSE, &numOfErrs);
}

PointerFixup::PointerFixup()
	: m_isOptimizing( false)
{
}

CoreObj PointerFixup::findLibRoot( const CoreObj& p_elem)
{   // copied from Identifier::getLibRootOf
	CoreObj cur = p_elem;

	bool goon = cur != 0;
	while( goon)
	{
#ifdef _DEBUG
		CComBSTR nm;
		try {
			auto f = ObjForCore(cur);
			if( f)
				f->get_Name( &nm);
		} catch( hresult_exception& ) {
			ASSERT(0);
		}
		nm.Append(";");
#endif
		long perm = cur[ATTRID_PERMISSIONS];
		if( perm & LIBROOT_FLAG)
			return cur;

		// if no library element, nor libroot element, must be a regular project
		// element, so we consider these having a BinGuid(0) as their realm uid
		if( (perm & LIBRARY_FLAG) == 0)
			return cur;

		metaid_type mt = cur.GetMetaID();
		ASSERT( mt != DTID_ROOT);
		cur = cur[ATTRID_PARENT];
		goon = cur != 0;
	}

	return CoreObj();
}

CoreObj PointerFixup::findCounterpart( const CoreObj& p_libElem)
{
	CoreObj lib = findLibRoot( p_libElem); // the library the element is part of
	//ASSERT( lib);

	CoreObj cntpart_lib; // the libroot's counterpart (equivalent peer)
	if( lib) 
		cntpart_lib = cntPartLib( lib);

	if( cntpart_lib)
	{
		//UniqueId u = Identifier::getDetailsOf( p_libElem);
		CComPtr<IMgaFCO> fco = SearchTool::findLibObj( cntpart_lib, Identifier::getDetailsOf( p_libElem));
		if( fco)
		{
			//CComBSTR nm; fco->get_Name( &nm);
			//nm.Append( ":");
			return CoreObj( fco); // this is the counterpart elem
		}
	}

	return CoreObj();
}

CoreObj PointerFixup::cntPartLib( const CoreObj& lib2)
{
	BinGuid g0 = Identifier::getPersistentIdOf( lib2);
	for( Typedefs::LIBPAIRVEC_ITER it = m_libPairs.begin(), en = m_libPairs.end(); it != en; ++it)
	{
		BinGuid g1 = Identifier::getPersistentIdOf( it->first);
		if( g0 == g1)
		{
			ASSERT( g1 == Identifier::getPersistentIdOf( it->second));
			return it->second;
		}
	}

	return CoreObj();
}

void PointerFixup::setLibPairs( Typedefs::LIBPAIRVEC& p_libPairs)
{
	m_libPairs = p_libPairs;
	m_isOptimizing = !p_libPairs.empty();
}

LibWorker::LibWorker( CMgaProject* p_mgaProject
                    , CComBSTR     p_libName
                    , bool         p_optimize)
    : m_mgaProject( p_mgaProject)
    , m_libName( p_libName)
    , m_optimized( p_optimize)
{
}

bool LibWorker::isOptimized() const
{
	return m_optimized;
}

CComBSTR LibWorker::getConnectionStr() const
{
	return m_libName;
}

CComBSTR LibWorker::getExpandedConnectionStr() const
{	// the environment variable (if found) is replaced with its value
	CComBSTR r;
	std::string v;
	CopyTo( m_libName, v);

	// replacing the found environment variable, %Name% syntax used
	size_t pos = v.find( '%');
	if( pos != std::string::npos) // found
	{
		size_t npos = v.find( '%', pos + 1); //next occurence of '%'
		if( npos != -1 && npos > pos + 1)
		{
			// get the value of the environment variable between the two %'s
			char *value = getenv( v.substr( pos + 1, npos - pos - 1).c_str());
			v.replace( pos, npos - pos + 1, value);
		}
	}

	CopyTo( v, &r);
	return r;
}

void LibWorker::showDetails( CoreObj&  p_container, std::map< BinGuid, std::vector< CoreObj > >&  p_results)
{
	CComBSTR oid;
	LibImgHelper::GetItsGuid( p_container, &oid);
	COMTHROW(oid.Append( L" lib recorded"));
	if( m_mgaProject) Reporter::showIt( m_mgaProject, oid, false);

	for( std::map< BinGuid, std::vector< CoreObj > >::iterator it = p_results.begin()
		; it != p_results.end()
		; ++it)
	{
		if( it->second.size() > 1) // duplicates found
		{
			BinGuid bf = it->first;
			MyCComBSTR m;
			m.appendGuid( bf);
			COMTHROW(m.Append( L" duplicate lib"));
			if( m_mgaProject) Reporter::showIt( m_mgaProject, m, false);
		}
	}
}


// static
void LibImgHelper::discover( int                                             p_recDepth
                           , CoreObj&                                        p_container
                           , const CComBSTR&                                 p_avoidThisLib
                           , Typedefs::LIBMAP&                               p_results)
{
	// recursive method
	// unlimited depth allowed if p_recDepth == -1
	// limited depth allowed if positive p_recDepth provided

	if( p_avoidThisLib.Length() > 0) 
	{
		// the caller is trying the discover the whole project EXCEPT 
		// the library (and its internals) specified by this id
		CComBSTR cont_id;
		ObjForCore( p_container)->get_ID( &cont_id);
		if( p_avoidThisLib == cont_id)
			return; // avoid going into this library
	}

	long pm = p_container[ATTRID_PERMISSIONS];
	if( pm & LIBROOT_FLAG)
	{
		BinGuid libn = Identifier::getPersistentIdOf( p_container);
		p_results[ libn ].push_back( p_container);
	}

	if( p_recDepth) // specifically: -1 or > 0
	{
		ITERATE_THROUGH( p_container[ ATTRID_FCOPARENT + ATTRID_COLLECTION]) 
		{
			metaid_type mtyp = ITER.GetMetaID(); 
			// in case p_recDepth > 0 the method will be called with decreased value
			if( mtyp == DTID_FOLDER) discover( p_recDepth > 0 ? p_recDepth - 1:-1, ITER, p_avoidThisLib, p_results);
		}
	}
}


// static
void LibImgHelper::recordLibs( bool                 p_recordAllLibs
                             , CoreObj&             p_container
                             , Typedefs::LIBMAP&    p_results
                             , const CComBSTR&      p_avoidThisLibrary)
{
	// if all libs must be recorded then allow unlimited depth: -1
	// otherwise (only toplevel libs need to be recorded): depth 1 is sufficient

	LibImgHelper::discover( p_recordAllLibs ? -1:1, p_container, p_avoidThisLibrary, p_results);
}

// static
void LibImgHelper::ungroupLibs( CoreObj&              p_container
                              , Typedefs::LIBMAP&     p_results
                              , Typedefs::LIBVEC&     p_depLibs)
{
	for( Typedefs::LIBMAP_ITER it = p_results.begin(), endit = p_results.end(); it != endit; ++it)
	{
		for( Typedefs::LIBVEC_ITER jt = it->second.begin(), endjt = it->second.end(); jt != endjt; ++jt)
		{
			CoreObj librf = *jt;
			CoreObj librfparent = librf[ATTRID_PARENT];
			CComBSTR id1, id2;
			LibImgHelper::GetItsGuid( p_container, &id1);
			LibImgHelper::GetItsGuid( librfparent, &id2);
			if( id1 != id2)
			{
				// move out to the root level
				librf[ATTRID_FCOPARENT] = p_container;

				// take off the Library (ro) flag!
				librf[ATTRID_PERMISSIONS] = librf[ATTRID_PERMISSIONS] & ~LIBRARY_FLAG;

				// collect more dependency info
				// this librf will depend on 'self'
				// = will be marked 'included by me'
				if( p_depLibs.end() == std::find( p_depLibs.begin(), p_depLibs.end(), librf))
					p_depLibs.push_back( librf);
			}
		}
	}
}

void LibWorker::flatten( CoreObj&     p_rootfolder
                       , CoreObj&     p_libimgroot
                       , Typedefs::LIBVEC& p_depLibs)
{
	if( m_optimized)
	{
		Typedefs::LIBMAP results;
		LibImgHelper::recordLibs( true, p_libimgroot, results);
		LibImgHelper::ungroupLibs( p_rootfolder, results, p_depLibs);
	}
}

//static 
void LibImgHelper::logCreator( CComBSTR& p_log
                             , const Typedefs::LIBVEC& p_libsToBe
                             , const Typedefs::LIBVEC& p_libsHosted)
{
	if( !p_log || !p_log.Length())
	{
		COMTHROW(p_log.Append( "Opportunities for optimization found."));
	}
	
	COMTHROW(p_log.Append( "<br> ...(External, Hosted) library pairs found: "));

	if( p_libsToBe.size() > 1)
	{
		COMTHROW(p_log.Append( "<br>Warning: attached library contains two or more (sub)libraries with similar guids!"));
	}

	for( Typedefs::LIBVEC_CITER it = p_libsToBe.begin(), endit = p_libsToBe.end(); it != endit; ++it)
	{
		COMTHROW(p_log.Append( "<br>External library in attachment:"));
		//p_log.Append( "<br>External library being attached");
		//p_log.Append( "<br>External library:");
		auto it_lib = ObjForCore( *it);
		CComBSTR nm, id, gd;
		if( it_lib) 
		{
			COMTHROW(it_lib->get_Name( &nm));//nm = (*it)[ATTRID_NAME];
			COMTHROW(it_lib->get_ID( &id));
			COMTHROW(it_lib->GetGuidDisp( &gd));
			COMTHROW(p_log.Append( L" ["));
			COMTHROW(p_log.Append( id));
			COMTHROW(p_log.Append( L"] GUID="));
			COMTHROW(p_log.Append( gd));
			COMTHROW(p_log.Append( L" "));
			COMTHROW(p_log.Append( nm));
		}
		else
		{
			ASSERT( 0);
			COMTHROW(p_log.Append( L"libNull"));
		}
	}

	for( Typedefs::LIBVEC_CITER jt = p_libsHosted.begin(), endjt = p_libsHosted.end(); jt != endjt; ++jt)
	{
		COMTHROW(p_log.Append( "<br>Hosted library already present:"));
		//p_log.Append( "<br>A hosted library:");

		auto jt_lib = ObjForCore( *jt);
		CComBSTR nm, id, gd;
		if( jt_lib) 
		{
			COMTHROW(jt_lib->get_Name( &nm));//nm = (*it)[ATTRID_NAME];
			COMTHROW(jt_lib->get_ID( &id));
			COMTHROW(jt_lib->GetGuidDisp( &gd));
			COMTHROW(p_log.Append( " ["));
			COMTHROW(p_log.Append( id));
			COMTHROW(p_log.Append( "] GUID="));
			COMTHROW(p_log.Append( gd));
			COMTHROW(p_log.Append( " "));
			COMTHROW(p_log.Append( nm));
		}
		else 
		{
			ASSERT( 0);
			COMTHROW(p_log.Append(L"libNull"));
		}
	}
}

//static 
void LibImgHelper::collectDep( Typedefs::LIBPAIRVEC&         p_matchingLibs
                             , Reporter&                     p_reporter
                             , Typedefs::LIBVEC&             p_dependentLibs)
{
	for( Typedefs::LIBPAIRVEC::iterator iiit = p_matchingLibs.begin(), eeen = p_matchingLibs.end(); iiit != eeen; ++iiit)
	{
		try
		{
			// collect second parameters of the matching map into the library vector
			if( p_dependentLibs.end() == std::find( p_dependentLibs.begin(), p_dependentLibs.end(), iiit->second))
				p_dependentLibs.push_back( iiit->second);

		} 
		catch( ... ) 
		{
			CComBSTR msg( "Exception while applying dependency info to ");
			try {
				CComBSTR nm, id;
				nm = (iiit->second)[ATTRID_NAME];
				auto lib = ObjForCore(iiit->second);
				if( lib)
					COMTHROW(lib->get_ID( &id));
				
				COMTHROW(msg.AppendBSTR( nm));
				COMTHROW(msg.Append( " [ "));
				COMTHROW(msg.AppendBSTR( id));
				COMTHROW(msg.Append( " ] !"));
			} 
			catch( ... ) 
			{ 
				COMTHROW(msg.Append( "!"));
			}
			p_reporter.show( msg);
		}
	}
}

//static 
void LibImgHelper::deleteSuperfluousLibs( Typedefs::LIBVEC& p_superfluousLibs, Reporter& reporter)
{
	for( Typedefs::LIBVEC_ITER it = p_superfluousLibs.begin(), en = p_superfluousLibs.end(); it != en; ++it)
	{
		try {
			auto lib = ObjForCore(*it);
			if( lib) lib->inDeleteObject();
			else     throw hresult_exception( -1);
		} 
		catch( ... ) 
		{
			CComBSTR msg( "Exception while deleting superfluous library ");
			try {
				CComBSTR nm, id;
				nm = (*it)[ATTRID_NAME];
				auto lib = ObjForCore(*it);
				if( lib) lib->get_ID( &id);
				
				COMTHROW(msg.AppendBSTR( nm));
				COMTHROW(msg.Append( " [ "));
				COMTHROW(msg.AppendBSTR( id));
				COMTHROW(msg.Append( " ] !"));
			} 
			catch( ... ) 
			{ 
				COMTHROW(msg.Append( "!"));
			}

			reporter.show( msg);
		}
	}
}

//static 
void LibImgHelper::matchLibs( const Typedefs::LIBMAP&          p_libMap
                            , const Typedefs::LIBMAP&          p_projMap
                            , Typedefs::LIBPAIRVEC&            p_matchingPairs
                            , CComBSTR&                        p_infoMsg)
{
	for( Typedefs::LIBMAP_CITER it = p_libMap.begin(), en = p_libMap.end(); it != en; ++it)
	{
		CComBSTR nm, pm;
		BinGuid bg = it->first;
		ASSERT( !it->second.empty());
		Typedefs::LIBMAP_CITER jt = p_projMap.find( bg);
		if( jt != p_projMap.end()) // found
		{
			//if( jt->second.size() >= 2) // more than one copy exists already
			//{
			//}

			// duplicate lib
			if( jt->second.empty())
				ASSERT(0); 
			else // make a pair from to-be-lib libroot and project-hosted libroot
			{
				//nm = it->second.front()[ATTRID_NAME];
				//nm.Append( ":");
				//pm = jt->second.front()[ATTRID_NAME];
				//pm.Append( ":");
				CoreObj frntLibToBe = it->second.front();
				CoreObj frntHosted  = jt->second.front();
				p_matchingPairs.push_back( std::make_pair( frntLibToBe, frntHosted));
				
				LibImgHelper::logCreator( p_infoMsg, it->second, jt->second);
			}
		}
	}
}

//static 
void LibImgHelper::GetItsGuid( CoreObj&           p_coreObj
                             , BSTR*              p_guidDisp)
{
	//
	// better be in sync with FCO::GetGuidDisp() in MgaFCO.cpp
	//
	// in constrast to MgaFCO::GetGuidDisp this method does not 
	// require the CheckRead() test, thus external, zombie or
	// not-yet-attached objects can be provided as well in its
	// coreObj parameter
	long v1(0), v2(0), v3(0), v4(0);
	v1 = p_coreObj[ATTRID_GUID1];
	v2 = p_coreObj[ATTRID_GUID2];
	v3 = p_coreObj[ATTRID_GUID3];
	v4 = p_coreObj[ATTRID_GUID4];
	
	GUID t_guid;
	t_guid.Data1 = v1;
	t_guid.Data2 = v2 >> 16;
	t_guid.Data3 = v2 & 0xFFFF;
	t_guid.Data4[0] = (v3 >> 24);
	t_guid.Data4[1] = (v3 >> 16) & 0xFF;
	t_guid.Data4[2] = (v3 >> 8) & 0xFF;
	t_guid.Data4[3] = v3 & 0xFF;

	t_guid.Data4[4] = (v4 >> 24);
	t_guid.Data4[5] = (v4 >> 16) & 0xFF;
	t_guid.Data4[6] = (v4 >> 8) & 0xFF;
	t_guid.Data4[7] = v4 & 0xFF;

	char buff[39];
	sprintf( buff, "{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		t_guid.Data1, t_guid.Data2, t_guid.Data3,
		t_guid.Data4[0], t_guid.Data4[1], t_guid.Data4[2], t_guid.Data4[3],
		t_guid.Data4[4], t_guid.Data4[5], t_guid.Data4[6], t_guid.Data4[7]);

	*p_guidDisp = CComBSTR( buff).Detach();
}

bool Ozer::isIncluded( CoreObj&  p_fldCore)
{
	CComBSTR idlist;
	StorageMgr::getIncludedBy( p_fldCore, idlist);
	return idlist && idlist.Length() > 0; // if included by somebody
}

bool Ozer::loseRelation  ( CoreObj&          p_fldCore
                         , const CComBSTR&   p_idToRemoveFromReg
                         , bool              p_incByOrInc
                         , bool&             p_libraryBecameFree)
{
	bool all_right     = false;
	//long prm           = p_fldCore[ATTRID_PERMISSIONS];
	CComBSTR valb;

	//p_fldCore[ATTRID_PERMISSIONS] = prm & ~READONLY_FLAG;
	if( p_incByOrInc) Ozer::StorageMgr::getIncludedBy( p_fldCore, valb);
	else              Ozer::StorageMgr::getIncludes( p_fldCore, valb);
	//p_fldCore[ATTRID_PERMISSIONS] = prm;

	if( valb && valb.Length() > 0)
	{
		if( removeFromList( p_idToRemoveFromReg, valb))
		{
			// save modified value back
			//p_fldCore[ATTRID_PERMISSIONS] = prm & ~READONLY_FLAG;
			if( p_incByOrInc) Ozer::StorageMgr::setIncludedBy( p_fldCore, valb);
			else              Ozer::StorageMgr::setIncludes( p_fldCore, valb);
			//p_fldCore[ATTRID_PERMISSIONS] = prm;

			if( valb.Length() == 0) // library became free
			{
				// let's take off the read-only flag
				//p_fldCore[ATTRID_PERMISSIONS] = prm & ~READONLY_FLAG;
				p_libraryBecameFree = true;
			}

			all_right = true;
		}
		else { ASSERT( 0); }
	}
	else { ASSERT( 0); }// empty registry value

	ASSERT( all_right);
	return all_right;
}

bool Ozer::addRelation( CoreObj&           p_fldCore
                      , const CComBSTR&    p_idToAddToReg
                      , bool               p_incByOrInc)
{
	// take off the Library (ro) flag
	//long prm        = p_fldCore[ATTRID_PERMISSIONS];
	//p_fldCore[ATTRID_PERMISSIONS] = prm & ~LIBRARY_FLAG & ~READONLY_FLAG;

	// registry asked about 'IncludedBy/s' info
	CComBSTR valu;
	if( p_incByOrInc) Ozer::StorageMgr::getIncludedBy( p_fldCore, valu);
	else              Ozer::StorageMgr::getIncludes( p_fldCore, valu);
	
	// value appended
	if( valu && valu.Length() > 0)
		COMTHROW(valu.Append( L"\n"));
	COMTHROW(valu.AppendBSTR( p_idToAddToReg)); // p_idToAddToReg might be a list of ids !!!

	// save value
	if( p_incByOrInc) Ozer::StorageMgr::setIncludedBy( p_fldCore, valu);
	else              Ozer::StorageMgr::setIncludes( p_fldCore, valu);

	// set saved perm values back
	//p_fldCore[ATTRID_PERMISSIONS] = prm;

	return true;
}

//static 
bool Ozer::removeFromList( const CComBSTR&     p_erasableVal
                         ,       CComBSTR&     p_valueList)
{
	bool all_right = false;

	DependentIterator it( p_valueList);
	p_valueList.Empty(); // the iterator made a copy of the list

	for( ; !it.isDone(); it.next())
	{
		if( it.getCurrentBstr() == p_erasableVal)
			all_right = true; // erasable value found
		else
		{
			if( p_valueList && p_valueList.Length() > 0)  // if not empty
				COMTHROW(p_valueList.Append( L"\n"));               // use separator 

			COMTHROW(p_valueList.Append( it.getCurrentBstr()));
		}
	}

	ASSERT( all_right);
	return all_right;
}


// copy dependencies from the old lib to the newer libs:
// if libX included oldlib, then libX will include newlib too
// that is why IncludedBy of newlib must be copied from oldlib
void Ozer::copyIncludedBy( CoreObj&     p_oldLib
                         , CoreObj&     p_newLib)
{
	CComBSTR list;
	Ozer::StorageMgr::getIncludedBy( p_oldLib, list);
	if( list && list.Length() > 0) // save info if not empty
	{
		CComBSTR val;
		Ozer::StorageMgr::getIncludedBy( p_newLib, val);
		if( val && val.Length() > 0)
		{
			COMTHROW(list.Append( L"\n"));
			COMTHROW(list.AppendBSTR( val));
		}
		Ozer::StorageMgr::setIncludedBy( p_newLib, list);
	}
}

// remove IncludedBy dependency reference from included 
// libraries of oldlib, the new lib will add again
// dependency reference for its own included libraries
// (i.e. some libraries may be deleted from a new 
// version of a library, in such cases some of the old
// included ones become 'free' or 'freelancers')
void Ozer::cutInclusions( CoreObj&     p_rootfolder
                        , CoreObj&     p_oldLib)
{
	CComBSTR list;
	Ozer::StorageMgr::getIncludes( p_oldLib, list);
	if( list && list.Length() > 0) // save info if not empty
	{
		CComBSTR oldlib_guid;
		LibImgHelper::GetItsGuid( p_oldLib, &oldlib_guid);

		for( DependentIterator it( list); !it.isDone(); it.next())
		{
			CoreObj lib_i;
			if( lib_i = SearchTool::findTopLibrary( p_rootfolder, it.getCurrentBstr()))
			{
				// remove lib_i's IncludedBy reference to oldlib
				bool free;
				Ozer::loseRelation( lib_i, oldlib_guid, true, free);
			}
		}
	}
}

// recreate the dependencies based on the vector of 
// factored-out ( = dependent = included) libraries
void Ozer::createDeps( CoreObj&          p_mainLib
                     , Typedefs::LIBVEC& p_depnLibs)
{
	CComBSTR m_gd;
	LibImgHelper::GetItsGuid( p_mainLib, &m_gd);

	for( Typedefs::LIBVEC_ITER i = p_depnLibs.begin(), e = p_depnLibs.end(); i != e; ++i)
	{
		CComBSTR i_gd;
		LibImgHelper::GetItsGuid( *i, &i_gd);

		Ozer::addRelation( p_mainLib, i_gd, false); // includes, p_mainLib includes a lib with i_gd
		Ozer::addRelation( *i,        m_gd, true);  // includedBy, *i is included by a lib with m_gd
	}
}


void Ozer::StorageMgr::writer( CoreObj&                p_fldCore
                             , const CComBSTR&         p_node
                             , const CComBSTR&         p_value)
{
	ObjForCore( p_fldCore)->put_RegistryValue( p_node, p_value);
}

void Ozer::StorageMgr::reader( CoreObj&                p_fldCore
                             , const CComBSTR&         p_node
                             , CComBSTR&               p_value)
{
	ObjForCore( p_fldCore)->get_RegistryValue( p_node, &p_value);
}

void Ozer::StorageMgr::getIncludes( CoreObj&             p_fldCore
                                  , CComBSTR&            p_idList)
{
	// get_RegistryValue complains if its first parameter
	// is a literal, that's why we pass a variable: path
	CComBSTR path( INCLUDES_STR);
	reader( p_fldCore, path, p_idList);
}

void Ozer::StorageMgr::setIncludes( CoreObj&             p_fldCore
                                  , const CComBSTR&      p_idList)
{
	// put_RegistryValue complains if its first parameter
	// is a literal, that's why we pass a variable: path
	CComBSTR path( INCLUDES_STR);
	writer( p_fldCore, path, p_idList);
}

void Ozer::StorageMgr::getIncludedBy( CoreObj&           p_fldCore
                                    , CComBSTR&          p_idList)
{
	CComBSTR path( INCLUDED_BY_STR);
	reader( p_fldCore, path, p_idList);
}

void Ozer::StorageMgr::setIncludedBy( CoreObj&           p_fldCore
                                    , const CComBSTR&    p_idList)
{
	CComBSTR path( INCLUDED_BY_STR);
	writer( p_fldCore, path, p_idList);
}


Ozer::DependentIterator::DependentIterator( const CComBSTR& p_tokenizable)
{
	CopyTo( p_tokenizable, m_tokenizable);

	m_pos = m_tokenizable.empty()? std::string::npos : 0;
	m_nextPos = m_tokenizable.find('\n', m_pos + 1);
}

Ozer::DependentIterator::DependentIterator( const std::string& p_tokenizable)
	: m_tokenizable( p_tokenizable)
{
	m_pos = m_tokenizable.empty()? std::string::npos : 0;
	m_nextPos = m_tokenizable.find('\n', m_pos + 1);
}

bool Ozer::DependentIterator::isDone()
{
	return m_pos == std::string::npos;
}

void Ozer::DependentIterator::next()
{
	m_pos = m_nextPos;
	if( !isDone())
		m_nextPos = m_tokenizable.find('\n', m_pos + 1);
}

std::string Ozer::DependentIterator::getCurrentStr()
{
	if( m_nextPos == std::string::npos)
		return m_tokenizable.substr( m_pos + ( m_pos == 0?0:1));
	else
		return m_tokenizable.substr( m_pos + ( m_pos == 0?0:1), m_nextPos - m_pos - ( m_pos == 0?0:1));
}

CComBSTR Ozer::DependentIterator::getCurrentBstr()
{
	CComBSTR retv;
	CopyTo( getCurrentStr(), &retv);
	return retv;
}
