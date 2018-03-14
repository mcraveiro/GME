#include "stdafx.h"
#include "MakeClosure.h"

#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
//
// C O M M E N T
// In this file the term "The RootFolder" means the default root folder any project is placed in
// Not to be confused with top folder which means the folder staying on the top of the selected
// object hierarchy
//
const std::string rootfolder_str = "RootFolder";
const wchar_t* CMakeClosure::m_markerNode = L"inClosure";

inline void nop( IMgaMetaFCO * f)
{
#ifdef _DEBUG
	CComObjPtr<IMgaMetaFCO> ff( f);
	CComBstrObj name; std::string s;
	COMTHROW( ff->get_Name( PutOut( name)));
	CopyTo( name, s);
	s = s + "|";
#endif
}

inline void nop( IMgaMetaFolder * f)
{
#ifdef _DEBUG
	CComObjPtr<IMgaMetaFolder> ff( f);
	CComBstrObj name; std::string s;
	COMTHROW( ff->get_Name( PutOut( name)));
	CopyTo( name, s);
	s = s + "|";
#endif
}

inline void nop( IMgaMetaModel * f)
{
#ifdef _DEBUG
	CComObjPtr<IMgaMetaModel> ff( f);
	CComBstrObj name; std::string s;
	COMTHROW( ff->get_Name( PutOut( name)));
	CopyTo( name, s);
	s = s + "|";
#endif
}

inline void nop( IMgaFCO * f)
{
#ifdef _DEBUG
	CComObjPtr<IMgaFCO> ff( f);
	CComBstrObj name; std::string s;
	COMTHROW( ff->get_Name( PutOut( name)));
	CopyTo( name, s);
	s = s + "|";
#endif
}

inline void nop( IMgaFolder * f)
{
#ifdef _DEBUG
	CComObjPtr<IMgaFolder> ff( f);
	CComBstrObj name; std::string s;
	COMTHROW( ff->get_Name( PutOut( name)));
	CopyTo( name, s);
	s = s + "|";
#endif
}

inline void nop( IMgaModel * f)
{
#ifdef _DEBUG
	CComObjPtr<IMgaModel> ff( f);
	CComBstrObj name; std::string s;
	COMTHROW( ff->get_Name( PutOut( name)));
	CopyTo( name, s);
	s = s + "|";
#endif
}

// -----------------------------------------------------------
// --------------------------- GmeEqual ----------------------
// -----------------------------------------------------------
GmeEqual::GmeEqual( CComObjPtr<IMgaObject> obj)
: m_objToFind( obj), m_which( 1)
{ }


GmeEqual::GmeEqual( CComObjPtr<IMgaFCO> fco)
: m_fcoToFind( fco), m_which( 2)
{ }


GmeEqual::GmeEqual( CComObjPtr<IMgaFolder> folder)
: m_folderToFind( folder), m_which( 3)
{ }


GmeEqual::GmeEqual( CComObjPtr<IMgaModel> model)
: m_modelToFind( model), m_which( 4)
{ }


bool GmeEqual::operator() ( CComObjPtr<IMgaObject>& op )
{
	ASSERT( m_which == 1); // which variable is set

	VARIANT_BOOL is_equal;
	COMTHROW( m_objToFind->get_IsEqual( op, &is_equal));

	if (is_equal != VARIANT_FALSE)
		return true;

	return false;
}

bool GmeEqual::operator() ( CComObjPtr<IMgaFCO>& op )
{
	ASSERT( m_which == 2); // which variable is set

	VARIANT_BOOL is_equal;
	COMTHROW( m_fcoToFind->get_IsEqual( op, &is_equal));

	if (is_equal != VARIANT_FALSE)
		return true;

	return false;
}

bool GmeEqual::operator() ( CComObjPtr<IMgaFolder>& op )
{
	ASSERT( m_which == 3); // which variable is set

	VARIANT_BOOL is_equal;
	COMTHROW( m_folderToFind->get_IsEqual( op, &is_equal));

	if (is_equal != VARIANT_FALSE)
		return true;

	return false;
}

bool GmeEqual::operator() ( CComObjPtr<IMgaModel>& op )
{
	ASSERT( m_which == 4); // which variable is set

	VARIANT_BOOL is_equal;
	COMTHROW( m_modelToFind->get_IsEqual( op, &is_equal));

	if (is_equal != VARIANT_FALSE)
		return true;

	return false;
}

// -----------------------------------------------------------
// --------------------------- CMakeClosure ------------------
// -----------------------------------------------------------
long CMakeClosure::process()
{
	CHECK_IN( m_project); // the constructor initialized it
	COMTRY
	{
		initTrans();
		
		if( isValidSelection()) doClosure();

		doneTrans( false);
	}
	COMCATCH( doneTrans(true) );
}

void CMakeClosure::doClosure()
{
	if ( m_bDirection) // down
	{
		preProcess();
		processFolders();
		processFCOs();
		postValidateSelection();
		postProcess();
	}
	else // up -> so first process the fcos
	{
		preProcess();
		processFCOs();
		processFolders();
		postValidateSelection();
		postProcess();
	}
}

void CMakeClosure::preProcess()
{
	send2Console("[Closure Copy] Working...");
}

void CMakeClosure::postProcess()
{
	// wrapping needed
	if ( m_iContainerOption == 0) // miniproject wrapping
	{
		selectTop();
		totalWrap();
		clearTopObjs(); // no need for the top objects, this is a sign to dump from the RootFolder

		send2Console("[Closure Copy] The miniproject can be either Imported or Pasted in a new/existing project.");
	}
	else if ( m_iContainerOption == 1) // automatic wrapping
	{
		selectTop();
		autoWrap(); // resets the Top top values
		checkMeta( m_topFolds, m_topFcos);

		std::string res_str;
		getAccKindsInString( res_str);
		if( !res_str.empty()) send2Console( "[Closure Copy] The following kinds may accept the copied data: " + res_str + ".");
		else send2Console( "[Closure Copy] Possible accepting kind not found based on the meta.");
	}
	else if ( m_iContainerOption == 2) // as is, no wrapping
	{
		selectTop();
		checkMeta( m_topFolds, m_topFcos);

		std::string res_str;
		getAccKindsInString( res_str);
		if( !res_str.empty()) send2Console( "[Closure Copy] The following kinds may accept the copied data: " + res_str + ".");
		else send2Console( "[Closure Copy] Possible accepting kind not found based on the meta.");
	}
}

void CMakeClosure::selectTop()
{
	CComObjPtrVector<IMgaFCO> &parentless = m_topFcos;
	CComObjPtrVector<IMgaFolder> &parentless_folders = m_topFolds;

	// selecting parentless ( = top) fcos	
	CComObjPtrVector<IMgaFCO>::iterator i = m_selFcos.begin();
	for( ; i != m_selFcos.end(); ++i)
	{
		CComObjPtr<IMgaModel> parent;
		COMTHROW( (*i)->get_ParentModel( PutOut( parent)));
		if ( parent) // not a root object
		{
			CComObjPtr<IMgaFCO> fco;
			COMTHROW( ::QueryInterface( parent, fco));
			
			ASSERT( fco);

			// check if model parent is in the selected set of objects
			// and if *i is already in the parentless
			if ( std::find_if( m_selFcos.begin(), m_selFcos.end(), GmeEqual( fco)) == m_selFcos.end() &&
				 std::find_if( parentless.begin(), parentless.end(), GmeEqual( *i)) == parentless.end())
			{
				parentless.push_back( *i);
			}
		}
		else // it may be contained by a folder
		{
			CComObjPtr<IMgaFolder> parent;
			COMTHROW( (*i)->get_ParentFolder( PutOut( parent)));
			if ( parent)
			{
				// check if folder parent is in the selected set of objects
				// and if *i is already in the parentless
				if ( std::find_if( m_selFolds.begin(), m_selFolds.end(), GmeEqual( parent)) == m_selFolds.end() &&
					 std::find_if( parentless.begin(), parentless.end(), GmeEqual( *i)) == parentless.end())
				{
					parentless.push_back( *i);	
				}
			}
			else 
			{ 
				ASSERT( 0);
			} // what the heck: *i is an fco and does not have a valid model or a valid folder parent?
		}
	}

	// selecting parentless folders	
	CComObjPtrVector<IMgaFolder>::iterator k = m_selFolds.begin();
	for( ; k != m_selFolds.end(); ++k)
	{
		CComObjPtr<IMgaFolder> parent;
		COMTHROW( (*k)->get_ParentFolder( PutOut( parent)));
		if ( parent) // is not a root object
		{
			// check if parent is in the selected set of objects
			if ( std::find_if( m_selFolds.begin(), m_selFolds.end(), GmeEqual( parent)) == m_selFolds.end() &&
				 std::find_if( parentless_folders.begin(), parentless_folders.end(), GmeEqual( *k)) == parentless_folders.end())
			{
				parentless_folders.push_back( *k);
			}
		}
		//else root: The RootFolder is not considered anymore a top object, see noTopObjs() method
	}
}

void CMakeClosure::processFolders()
{
	unsigned int curr_start = 0;
	bool continju = true;
	while (continju)
	{
		continju = false;
		CComObjPtrVector<IMgaFolder>::iterator i = m_selFolds.begin();
		for( unsigned int j = curr_start; j < m_selFolds.size() && i != m_selFolds.end(); ++j, ++i)
		{
			procObj( m_selFolds[j]);
		}

		continju = !m_newlySelFolds.empty() || !m_newlySelFcos.empty();
		if ( continju)
		{
			curr_start = m_selFolds.size();
			m_selFolds.insert( m_selFolds.end(), m_newlySelFolds.begin(), m_newlySelFolds.end());
			m_selFcos.insert( m_selFcos.end(), m_newlySelFcos.begin(), m_newlySelFcos.end());
		}
		
		m_newlySelFcos.clear();
		m_newlySelFolds.clear();
	}
}

void CMakeClosure::processFCOs()
{
	unsigned int curr_start = 0;
	bool continju = true;
	while (continju)
	{
		continju = false;
		CComObjPtrVector<IMgaFCO>::iterator i = m_selFcos.begin();
		for( unsigned int j = curr_start; j < m_selFcos.size() && i != m_selFcos.end(); ++j, ++i)
		{
			procObj( m_selFcos[j]);
		}

		continju = !m_newlySelFolds.empty() || !m_newlySelFcos.empty();
		if ( continju)
		{
			curr_start = m_selFcos.size();
			m_selFolds.insert( m_selFolds.end(), m_newlySelFolds.begin(), m_newlySelFolds.end());
			m_selFcos.insert( m_selFcos.end(), m_newlySelFcos.begin(), m_newlySelFcos.end());
		}
		m_newlySelFcos.clear();
		m_newlySelFolds.clear();
	}
}

//---------------------------------------------------------------------------------------
//-------------------------------------------------processor methods---------------------
//---------------------------------------------------------------------------------------
void CMakeClosure::procFolder( IMgaFolder *folder)
{
	ASSERT( folder != NULL );
	insertNew( folder);

	if ( m_bFolderContainment)
	{
		insertNew( folder, &IMgaFolder::get_ChildFolders);
		insertNew( folder, &IMgaFolder::get_ChildFCOs);
	}
}

void CMakeClosure::procModel( IMgaModel *model)
{
	ASSERT( model != NULL );
	insertNew( CComObjPtr<IMgaModel>( model));

	if ( m_bContainment)
	{
		insertNew( model, &IMgaModel::get_ChildFCOs);
	}
}

void CMakeClosure::procReference( IMgaReference *reference)
{
	ASSERT( reference != NULL );
	insertNew( CComObjPtr<IMgaFCO>( reference));

	CComObjPtr<IMgaFCO> fco;
	COMTHROW( reference->get_Referred( PutOut( fco)) );

	if ( m_bRefersTo && fco) // test if null reference
	{
		bool inlib = isInLibrary( fco); // a referred fco may be in a library
		if ( inlib && m_iIntoLibraries == LIB_CONT || !inlib)
			insertNew( fco);
	}
}


void CMakeClosure::procSet( IMgaSet *one_set)
{
	ASSERT( one_set != NULL );
	insertNew( one_set);

	CComObjPtrVector<IMgaFCO> members;
	COMTHROW( one_set->get_Members( PutOut( members)) );//zz1
	
	if ( m_bSetMember)
	{
		insertNew( members);
	}
}

void CMakeClosure::procAtom( IMgaAtom *atom)
{
	ASSERT( atom != NULL );
	insertNew( atom);
}


bool CMakeClosure::isValidSelection()
{
	// check each object for validity
	CComObjPtrVector<IMgaFCO>::iterator fco_it = m_selFcos.begin();
	while( fco_it != m_selFcos.end())
	{
		if( !isValidFCO( *fco_it))
			fco_it = m_selFcos.erase( fco_it);
		else
			++fco_it;
	}

	return m_selFcos.size() + m_selFolds.size() > 0;
}

bool CMakeClosure::isValidFCO( IMgaFCO * fco)
{
	objtype_enum objtype;
	COMTHROW( fco->get_ObjType( &objtype) );
	if( objtype == OBJTYPE_CONNECTION)
	{
		CComObjPtr<IMgaConnection> c;
		COMTHROW( ::QueryInterface( fco, c) );
		return isValidConnection( c);
	}

	return true;
}

/*static*/ CComPtr<IGMEOLEApp> CMakeClosure::get_GME(CComObjPtr<IMgaProject> project)
{
	CComPtr<IGMEOLEApp> gme;
	if (/*mb_is_interactive &&*/ (project != NULL)) {		
		CComBSTR bstrName("GME.Application");
		CComPtr<IMgaClient> pClient;
		HRESULT hr = project->GetClientByName(bstrName, &pClient);
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

bool CMakeClosure::isSimpleConnection( IMgaConnection * connection)
{
	bool res = true;
	CComObjPtrVector<IMgaConnPoint> v;
	COMTHROW( connection->get_ConnPoints( PutOut( v)) );//zz1

	for( CComObjPtrVector<IMgaConnPoint>::iterator i = v.begin(); res && i != v.end(); ++i)
	{
		CComObjPtr<IMgaConnPoint> connpoint = *i;
		
		CComObjPtrVector<IMgaFCO> refs;
		COMTHROW( connpoint->get_References(PutOut( refs)) );//zz1
		if( !refs.empty()) // it is a connection thru reference ports
			res = false;
	}

	return res;
}


bool CMakeClosure::isValidConnection( IMgaConnection * connection)
{
	bool res = true;

	CComObjPtrVector<IMgaConnPoint> v;
	COMTHROW( connection->get_ConnPoints( PutOut( v)) );//zz1

	for( CComObjPtrVector<IMgaConnPoint>::iterator i = v.begin(); res && i != v.end(); ++i)
	{
		CComObjPtr<IMgaConnPoint> connpoint = *i;
		
		// general constraints (which apply for each case: LIB_*
		CComObjPtrVector<IMgaFCO> refs;
		COMTHROW( connpoint->get_References(PutOut( refs)) );//zz1
		for( CComObjPtrVector<IMgaFCO>::iterator ref_i = refs.begin(); res && ref_i != refs.end(); ++ref_i )
		{
			if( !m_bRefPortConnection)
			{
				res = false;//this is a connection between at least one reference port, so if the corresponding option is not set (m_bRefPortConnection) then we consider it invalid
				
				if( m_GME) 
				{
					CComBSTR bstr, id, nm;
					COMTHROW( connection->get_ID( &id));
					COMTHROW( connection->get_Name( &nm));
					COMTHROW(bstr.Append("Closure: <A HREF=\"mga:"));
					COMTHROW(bstr.AppendBSTR( id));
					COMTHROW(bstr.Append("\">"));
					COMTHROW(bstr.AppendBSTR( nm));
					COMTHROW(bstr.Append("</A> disregarded. Connection involving reference ports. Use the \"Connection thru refport\" option to include it."));

					COMTHROW(m_GME->ConsoleMessage(bstr, MSG_ERROR));
				}

			}
		}

		// constraints for lib_stop case
		if( m_iIntoLibraries == LIB_STOP)
		{
			CComObjPtr<IMgaFCO> target; // the src or dst of a connection
			COMTHROW( connpoint->get_Target( PutOut( target)) );
			if( target && isInLibrary( target))
				res = false; // this is a connection with one end in a library which in lib_stop case is not good
		}
	}

	return res;
}

bool CMakeClosure::postValidateSelection()
{
	// check each object for validity...
	return true;
}

bool CMakeClosure::postValidateFCO( IMgaFCO * fco)
{
	return true;
}

bool CMakeClosure::postValidateConnection( IMgaConnection * connection)
{
	return true;
}

void CMakeClosure::procConnection( IMgaConnection *connection)
{
	ASSERT( connection != NULL );

	bool simple = isSimpleConnection( connection);
	if ( simple && m_bConnection || !simple && m_bRefPortConnection)
	{
		CComObjPtrVector<IMgaConnPoint> v;
		COMTHROW( connection->get_ConnPoints( PutOut( v)) );//zz1
		
		insertNew( CComObjPtr<IMgaFCO>( connection));
		for( CComObjPtrVector<IMgaConnPoint>::iterator i = v.begin(); i != v.end(); ++i)
		{
			procConnPoint( *i);
		}
	}
}

// if called from procConnection then the validity is checked
// if called from procMyConnections then it is checked as well
void CMakeClosure::procConnPoint( IMgaConnPoint *connpoint)
{
	ASSERT( connpoint != NULL );

	CComObjPtr<IMgaFCO> target; // the src or dst of a connection
	COMTHROW( connpoint->get_Target( PutOut( target)) );

	CComObjPtr<IMgaConnection> conn; // the connection itself
	COMTHROW( connpoint->get_Owner( PutOut( conn)) );

	CComObjPtr<IMgaFCO> conn_fco;
	COMTHROW( conn.QueryInterface( conn_fco));

	CComObjPtrVector<IMgaFCO> refs; // the reference chain the connection is drawn
	COMTHROW( connpoint->get_References( PutOut( refs)) );//zz1

	if ( refs.empty()) // simple connection, no reference ports involved
	{
		insertNew( target);
		insertNew( conn_fco);
	}
	else // refport conn, so insert everything 
	{
		bool inlib;
		inlib = isInLibrary( target);
		if ( inlib && m_iIntoLibraries == LIB_CONT || !inlib) insertNew( target);
		
		inlib = isInLibrary( conn_fco); // the connection might be in the library (=both endtargets are as well in the library)
		if ( inlib && m_iIntoLibraries == LIB_CONT || !inlib) 
		{
			insertNew( conn_fco);
			insertNew( refs);
		}
	}
}

// this method is needed after an fco is processed to insert the attaching connections as well
void CMakeClosure::procMyConnections( IMgaFCO * fco)
{
	if ( m_bConnection || m_bRefPortConnection)
	{
		CComObjPtrVector<IMgaConnPoint> my_conns;	
		COMTHROW( fco->get_PartOfConns( PutOut( my_conns)) );//zz1
		for( CComObjPtrVector<IMgaConnPoint>::iterator i = my_conns.begin(); i != my_conns.end(); ++i)
		{
			CComObjPtr<IMgaConnection> conn; // the connection itself
			COMTHROW( (*i)->get_Owner( PutOut( conn)) );
			bool simple = isSimpleConnection( conn);
			if( simple && m_bConnection || !simple && m_bRefPortConnection)
				procConnPoint( *i);//the validity of each connection is tested
		}
	}
}

void CMakeClosure::procMyContainerSets( IMgaFCO * fco)
{
	if ( m_bMemberOfSets)
	{
		CComObjPtrVector<IMgaFCO> my_sets;
		COMTHROW( fco->get_MemberOfSets( PutOut( my_sets)) );//zz1
		insertNew( my_sets);
	}
}

void CMakeClosure::procMyReferences( IMgaFCO * fco)
{
	if ( m_bReferredBy)
	{
		CComObjPtrVector<IMgaFCO> my_references;
		COMTHROW( fco->get_ReferencedBy( PutOut( my_references)) );//zz1
		insertNew( my_references);
	}
}

void CMakeClosure::procMyParent( IMgaFCO * fco)
{
	CComObjPtr<IMgaModel> mod;
	COMTHROW( fco->get_ParentModel( PutOut( mod)) );
	if ( mod)
	{
		if ( m_bPartOfModels)
		{
			insertNew( mod);
		}
	}
	else
	{
		CComObjPtr<IMgaFolder> fold;
		COMTHROW( fco->get_ParentFolder( PutOut( fold)) );
		if ( fold)
		{
			if ( m_bPartOfFolders)
			{
				insertNew( fold);
			}
		}
		// else rootfolder
	}
}

void CMakeClosure::procMyParent( IMgaFolder * folder)
{
	CComObjPtr<IMgaFolder> parent;
	COMTHROW( folder->get_ParentFolder( PutOut( parent)) );
	if ( parent)
	{
		if ( m_bPartOfFolders)
		{
			insertNew( parent);
		}
	}
	// else rootfolder
}

template<class T>
void CMakeClosure::markObj( CComObjPtr<T> obj)
{
	if( m_markWith != -1)
	{
		CComBSTR buf0;
		COMTHROW( obj->get_RegistryValue( CComBSTR( m_markerNode), &buf0));

		// check if the new marker appears already in the marker list
		std::wstring str_buf = buf0.Length() ? buf0 : L"";

		TCHAR buf1[12];
		swprintf_s( buf1, L",%i", m_markWith);
		std::wstring new_mrk( buf1);

		str_buf += L","; // will ease the eos comparison
		new_mrk += L","; // and the exact match is guaranteed
		
		if( str_buf.find( new_mrk) == std::string::npos) // if marker not present
		{
			COMTHROW(buf0.Append( buf1)); // insert new marker into list & update
			COMTHROW( obj->put_RegistryValue( CComBSTR( m_markerNode), buf0));
		}
	}
}

void CMakeClosure::procObj( IMgaObject* obj)
{
	ASSERT( obj != NULL );
	//!m_bIntoLibraries <=> m_iIntoLibraries != 2
	if ( m_iIntoLibraries != LIB_CONT && isInLibrary( obj)) // if no intention to step into libraries and the object is in a library then return
		return;

	objtype_enum objtype;
	COMTHROW( obj->get_ObjType( &objtype) );

	switch( objtype)
	{
	case OBJTYPE_MODEL:
	case OBJTYPE_ATOM:
	case OBJTYPE_REFERENCE:
	case OBJTYPE_CONNECTION:
	case OBJTYPE_SET:
		{
			CComObjPtr<IMgaFCO> p;
			COMTHROW( ::QueryInterface( obj, p) );

			markObj( p);

			if ( m_bConnection) 		procMyConnections( p);
			if ( m_bMemberOfSets)		procMyContainerSets( p);
			if ( m_bReferredBy)			procMyReferences( p);

			procMyParent( p);			// we don't know what kind of parent the fco has: Model or Folder

			if ( m_bBaseTypes)			procMyBaseTypes( p);
			if ( m_bDerivedTypes)		procMyDerivedTypes( p);
			break;
		}

	case OBJTYPE_FOLDER:
		{
			CComObjPtr<IMgaFolder> p;
			COMTHROW( ::QueryInterface( obj, p) );
			
			markObj( p);

			if ( m_bPartOfFolders)		procMyParent( p);
			break;
		}

	default:
		HR_THROW(E_INVALID_MGA);
	};


	switch( objtype)
	{
	case OBJTYPE_MODEL:
		{
			if ( m_bContainment)
			{
				CComObjPtr<IMgaModel> m;
				COMTHROW( ::QueryInterface( obj, m) );

				procModel( m);
			}
			break;
		}

	case OBJTYPE_ATOM:
		{
			CComObjPtr<IMgaAtom> a;
			COMTHROW( ::QueryInterface( obj, a) );
			procAtom( a);
			break;
		}

	case OBJTYPE_REFERENCE:
		{
			if ( m_bRefersTo)
			{
				CComObjPtr<IMgaReference> r;
				COMTHROW( ::QueryInterface( obj, r) );
				procReference( r);
			}
			break;
		}

	case OBJTYPE_CONNECTION:
		{
			if ( m_bConnection)
			{
				CComObjPtr<IMgaConnection> c;
				COMTHROW( ::QueryInterface( obj, c) );
				procConnection( c);
			}
			break;
		}

	case OBJTYPE_SET:
		{
			if ( m_bSetMember)
			{
				CComObjPtr<IMgaSet> s;
				COMTHROW( ::QueryInterface( obj, s) );
				procSet( s);
			}
			break;
		}

	case OBJTYPE_FOLDER:
		{
			if ( m_bFolderContainment)
			{
				CComObjPtr<IMgaFolder> f;
				COMTHROW( ::QueryInterface( obj, f) );
				procFolder( f);
			}
			break;
		}

	default:
		HR_THROW(E_INVALID_MGA);
	};
}

void CMakeClosure::procMyBaseTypes( IMgaFCO * fco)
{
	if ( !m_bBaseTypes)
		return;

	CComObjPtr<IMgaFCO> base;
	COMTHROW( fco->get_DerivedFrom( PutOut( base)));
	if( base)
	{
		bool inlib = isInLibrary( base); // base types may be in a library
		if ( inlib && m_iIntoLibraries == LIB_CONT || !inlib)
			insertNew( base); 
	}

}

void CMakeClosure::procMyDerivedTypes( IMgaFCO * fco)
{
	if ( !m_bDerivedTypes) 
		return;

	CComObjPtrVector<IMgaFCO> der_fcos;
	COMTHROW( fco->get_DerivedObjects( PutOut( der_fcos)) );//zz1
	insertNew( der_fcos);
}

void CMakeClosure::insertNew( const std::string& id, bool is_top /* = false */)
{
	CComObjPtr<IMgaObject> obj;
	COMTHROW( m_project->GetObjectByID( PutInBstr( id), PutOut( obj)) );
	if ( obj)
	{
		objtype_enum objtype;
		COMTHROW( obj->get_ObjType(&objtype) );

		switch( objtype)
		{
		case OBJTYPE_MODEL:
		case OBJTYPE_SET:
		case OBJTYPE_REFERENCE:
		case OBJTYPE_CONNECTION:
		case OBJTYPE_ATOM:
			{
				CComObjPtr<IMgaFCO> model;
				COMTHROW( ::QueryInterface(obj, model) );
				ASSERT( model);
				
				insertNew( model);
				if ( is_top)
				{
					nop( model);
					if ( std::find_if( m_topFcos.begin(), m_topFcos.end(), GmeEqual( model)) == m_topFcos.end())
						m_topFcos.push_back( model);
				}

				break;
			}
		case OBJTYPE_FOLDER:
			{
				CComObjPtr<IMgaFolder> folder;
				COMTHROW( ::QueryInterface( obj, folder));
				ASSERT( folder);
				
				insertNew( folder);
				if ( is_top)
				{
					nop( folder);
					if ( std::find_if( m_topFolds.begin(), m_topFolds.end(), GmeEqual( folder)) == m_topFolds.end())
						m_topFolds.push_back( folder);
				}

				break;
			}
		default:
			ASSERT( 0);
		} // switch
	} // if
}

void CMakeClosure::insertNew( const CComObjPtr<IMgaFCO>& in)
{
	nop( in);

	if ( in != NULL
		&& !findAmongNews( in)
		&& !findAmongSelected( in)
		) 
		m_newlySelFcos.push_back( in);
}

void CMakeClosure::insertNew( const CComObjPtr<IMgaFolder>& in)
{
	nop( in);

	if ( in != NULL
		&& !findAmongNews( in)
		&& !findAmongSelected( in)
		) 
		m_newlySelFolds.push_back( in);
}

void CMakeClosure::insertNew( const CComObjPtr<IMgaModel>& in)
{
	if ( in != NULL)
	{
		CComObjPtr<IMgaFCO> fco;
		COMTHROW( ::QueryInterface( in, fco));

		insertNew( fco);
	}
}

void CMakeClosure::insertNew( const CComObjPtrVector<IMgaFCO>& in_v)
{
	CComObjPtrVector<IMgaFCO>::const_iterator it = in_v.begin();
	for( ; it != in_v.end(); ++it)
		insertNew( *it);
}

void CMakeClosure::insertNew( const CComObjPtrVector<IMgaFolder>& in_v)
{
	CComObjPtrVector<IMgaFolder>::const_iterator it = in_v.begin();
	for( ; it != in_v.end(); ++it)
		insertNew( *it);
}

bool CMakeClosure::findAmongSelected(const CComObjPtr<IMgaFCO>& in)
{
	return std::find_if( m_selFcos.begin(), m_selFcos.end(), GmeEqual( in)) != m_selFcos.end();
}

bool CMakeClosure::findAmongSelected(const CComObjPtr<IMgaFolder>& in)
{
	return std::find_if( m_selFolds.begin(), m_selFolds.end(), GmeEqual( in)) != m_selFolds.end();
}

bool CMakeClosure::findAmongNews(const CComObjPtr<IMgaFCO>& in)
{
	return std::find_if( m_newlySelFcos.begin(), m_newlySelFcos.end(), GmeEqual( in)) != m_newlySelFcos.end();
}

bool CMakeClosure::findAmongNews(const CComObjPtr<IMgaFolder>& in)
{
	return std::find_if( m_newlySelFolds.begin(), m_newlySelFolds.end(), GmeEqual( in)) != m_newlySelFolds.end();
}

bool CMakeClosure::isInLibrary( IMgaObject * obj)
{
	VARIANT_BOOL vbLibrary;
	COMTHROW( obj->get_IsLibObject( &vbLibrary ) );
	return ( vbLibrary ) ? true : false;
}

// --------------------------------------------------------- wrappers
void CMakeClosure::totalWrap()
{
	CComObjPtrVector<IMgaFolder> fo_parents; // will collect the parents of all objects
	CComObjPtrVector<IMgaModel> mo_parents;

	CComObjPtrVector<IMgaFCO>::iterator fco_it = m_topFcos.begin();
	for( ; fco_it != m_topFcos.end(); ++fco_it)
	{
		processUpward( *fco_it, fo_parents, mo_parents);
	}
	//int sz = m_selFcos.size();

	CComObjPtrVector<IMgaFolder>::iterator fold_it = m_topFolds.begin();
	for( ; fold_it != m_topFolds.end(); ++fold_it)
	{
		processUpward( *fold_it, fo_parents, mo_parents);
	}
	//sz = m_selFolds.size();

	//append the folder parents to selected folders
	//m_selFolds.insert( m_selFolds.end(), fo_parents.begin(), fo_parents.end());
	CComObjPtrVector<IMgaFolder>::iterator fol_it = fo_parents.begin();
	for( ; fol_it != fo_parents.end(); ++fol_it)
	{
		if ( std::find_if( m_selFolds.begin(), m_selFolds.end(), GmeEqual( *fol_it)) == m_selFolds.end())
			m_selFolds.push_back( *fol_it);
		else
			ASSERT( 0);
	}
	
	//append the model parents to selected fcos
	CComObjPtrVector<IMgaModel>::iterator mod_it = mo_parents.begin();
	for( ; mod_it != mo_parents.end(); ++mod_it)
	{
		CComObjPtr<IMgaFCO> fco;
		COMTHROW( ::QueryInterface( *mod_it, fco));
		if ( fco && std::find_if( m_selFcos.begin(), m_selFcos.end(), GmeEqual( fco)) == m_selFcos.end())
			m_selFcos.push_back( fco);
		else
			ASSERT( 0);
	}
}

std::string CMakeClosure::processUpward( IMgaObject *obj, CComObjPtrVector<IMgaFolder>& f_parents, CComObjPtrVector<IMgaModel>& m_parents)
{
	ASSERT( obj != NULL );

	objtype_enum objtype;
	COMTHROW( obj->get_ObjType(&objtype) );

	switch(objtype) {
	case OBJTYPE_MODEL:
	case OBJTYPE_ATOM:
	case OBJTYPE_REFERENCE:
	case OBJTYPE_CONNECTION:
	case OBJTYPE_SET:
		{
			CComObjPtr<IMgaFCO> fco;
			COMTHROW( ::QueryInterface( obj, fco) );
			ASSERT( fco);

			bool topmost_m_set( false);
			CComObjPtr<IMgaModel> m, topmost_m;
			COMTHROW( fco->get_ParentModel( PutOut( m)));
			while ( m)
			{
				if ( std::find_if( m_parents.begin(), m_parents.end(), GmeEqual( m)) == m_parents.end())
					m_parents.push_back( m);

				topmost_m = m; topmost_m_set = true;
				CComObjPtr<IMgaModel> m_out;
				COMTHROW( m->get_ParentModel( PutOut( m_out)));
				m = m_out;
			}

			CComObjPtr<IMgaFolder> f;
			if ( topmost_m_set)
				COMTHROW( topmost_m->get_ParentFolder( PutOut( f)));
			else
				COMTHROW( fco->get_ParentFolder( PutOut( f)));

			while( f)
			{
				if ( std::find_if( f_parents.begin(), f_parents.end(), GmeEqual( f)) == f_parents.end())
					f_parents.push_back( f);
				
				CComObjPtr<IMgaFolder> f_out;
				COMTHROW( f->get_ParentFolder( PutOut( f_out)));
				f = f_out;
			}
			break;
		}
	case OBJTYPE_FOLDER:
		{
			CComObjPtr<IMgaFolder> folder, f;
			COMTHROW( ::QueryInterface( obj, folder));
			ASSERT( folder);

			COMTHROW( folder->get_ParentFolder( PutOut( f)));

			while( f)
			{
				if ( std::find_if( f_parents.begin(), f_parents.end(), GmeEqual( f)) == f_parents.end())
					f_parents.push_back( f);

				CComObjPtr<IMgaFolder> f_out;
				COMTHROW( f->get_ParentFolder( PutOut( f_out)));
				f = f_out;
			}
			break;
		}
	default:
		HR_THROW(E_INVALID_MGA);
	}; // endswitch

	return "";
}

void CMakeClosure::autoWrap()
{
	if( m_topFcos.empty() && m_topFolds.empty()) // no top objects == root folder selected
		return;

	CComObjPtrVector<IMgaFolder> f_parents;
	CComObjPtrVector<IMgaModel> m_parents;

	std::map< int, std::string> path_map;
	std::map< int, std::string> name_map;

	unsigned int k = 0;
	CComObjPtrVector<IMgaFCO>::iterator i = m_topFcos.begin();
	for( ; i != m_topFcos.end(); ++i, ++k)
	{
		std::string res, resid;

		// initialize the values
		CComBstrObj name, id;
		COMTHROW( (*i)->get_Name( PutOut(name)) );
		COMTHROW( (*i)->get_ID( PutOut( id)) );
		CopyTo( name, res);
		CopyTo( id, resid);
		res += '/';
		resid += '/';

		
		bool topmost_m_set = false;
		CComObjPtr<IMgaModel> m, topmost_m;
		COMTHROW( (*i)->get_ParentModel( PutOut( m)));
		while ( m)
		{
			CComBstrObj name, id;
			std::string s, ids;
			COMTHROW( m->get_Name( PutOut(name)) );
			COMTHROW( m->get_ID( PutOut( id)) );
			CopyTo( name, s);
			CopyTo( id, ids);
			res = s + '/' + res;
			resid = ids + '/' + resid;

			
			if ( std::find_if( m_parents.begin(), m_parents.end(), GmeEqual( m)) == m_parents.end())
				m_parents.push_back( m);

			topmost_m = m; topmost_m_set = true;
			CComObjPtr<IMgaModel> m_out;
			COMTHROW( m->get_ParentModel( PutOut( m_out)));
			m = m_out;
		}

		CComObjPtr<IMgaFolder> f;
		if ( topmost_m_set)
			COMTHROW( topmost_m->get_ParentFolder( PutOut( f)));
		else
			COMTHROW( (*i)->get_ParentFolder( PutOut( f)));

		while( f)
		{
			CComBstrObj name, id;
			std::string s, ids;
			COMTHROW( f->get_Name( PutOut( name)) );
			COMTHROW( f->get_ID( PutOut( id)) );
			CopyTo( name, s);
			CopyTo( id, ids);
			res = s + '/' + res;
			resid = ids + '/' + resid;

			if ( std::find_if( f_parents.begin(), f_parents.end(), GmeEqual( f)) == f_parents.end())
				f_parents.push_back( f);
			
			CComObjPtr<IMgaFolder> f_out;
			COMTHROW( f->get_ParentFolder( PutOut( f_out)));
			f = f_out;
		}

		path_map[ k] = resid;
		name_map[ k] = res;
	}

	CComObjPtrVector<IMgaFolder>::iterator j = m_topFolds.begin();
	for(  ; j != m_topFolds.end(); ++j, ++k)
	{
		std::string res, resid;

		// initialize the values
		CComBstrObj name, id;
		COMTHROW( (*j)->get_Name( PutOut(name)) );
		COMTHROW( (*j)->get_ID( PutOut( id)) );
		CopyTo( name, res);
		CopyTo( id, resid);
		res += '/';
		resid += '/';


		CComObjPtr<IMgaFolder> f;
		COMTHROW( (*j)->get_ParentFolder( PutOut( f)));

		while( f)
		{
			CComBstrObj name, id;
			std::string s, ids;
			COMTHROW( f->get_Name( PutOut(name)) );
			COMTHROW( f->get_ID( PutOut( id)) );
			CopyTo( name, s);
			CopyTo( id, ids);
			res = s + '/' + res;
			resid = ids + '/' + resid;

			if ( std::find_if( f_parents.begin(), f_parents.end(), GmeEqual( f)) == f_parents.end())
				f_parents.push_back( f);

			CComObjPtr<IMgaFolder> f_out;
			COMTHROW( f->get_ParentFolder( PutOut( f_out)));
			f = f_out;
		}

		path_map[ k] = resid;
		name_map[ k] = res;
	}

	ASSERT( k > 0);
	if ( k == 0) // no top objects == root folder selected
		return;

	std::string common_path = path_map[ 0];
	std::string common_name = name_map[ 0];
	for( unsigned int l = 1; l < k; ++l)
	{
		std::string next_path = path_map[ l];
		unsigned int minlen = std::min( next_path.length(), common_path.length());

		// the strings may look like
		// id-0065-001/id-0065-002/id-0065-003/
		// id-0065-001/id-0065-002/id-0065-004/
		// thus the common part of all may look like
		// id-0065-001/id-0065-002/id-0065-00
		// for this we use the position of the last slash
		unsigned int pos_of_last_slash = 0;
		unsigned int newlen = 0;

		while( newlen < minlen && next_path[ newlen] == common_path[ newlen])
		{
			if ( common_path[ newlen] == '/') 
				pos_of_last_slash = newlen;

			++newlen;
		}
		common_path = common_path.substr( 0, pos_of_last_slash + 1); // tailing '/' needed

		std::string next_name = name_map[ l];
		minlen = std::min( next_name.length(), common_name.length());

		pos_of_last_slash = 0;
		newlen = 0;
		while( newlen < minlen && next_name[ newlen] == common_name[ newlen])
		{
			if ( common_name[ newlen] == '/') 
				pos_of_last_slash = newlen;

			++newlen;
		}

		common_name = common_name.substr( 0, pos_of_last_slash + 1); // tailing '/' needed
	}

	common_name = common_name.substr( 0, common_name.length() - 1);
	common_path = common_path.substr( 0, common_path.length() - 1);

	std::string common_root_name; // the common model's name
	int pos = common_name.rfind( "/"); // tailing '/' removed previously
	common_root_name = common_name.substr( pos + 1, common_name.length() - pos - 1);

	std::string common_root_id; // the common model's id
	pos = common_path.rfind( "/"); // tailing '/' removed previously
	common_root_id = common_path.substr( pos + 1, common_path.length() - pos - 1);
	
	//AfxMessageBox( CString( "The common part of the paths is: ") + common_name.c_str() + CString( "\nThe last common part of the paths is: ") + common_root_name.c_str() );

	m_topFcos.clear(); // these two will be reset soon
	m_topFolds.clear();
	m_newlySelFcos.clear();
	m_newlySelFolds.clear();

	// are all selected objects children (not equal) of the common root?
	// checks if the common root is among the selected objects
	// ?indicates if the common root of the objects is a selected object itself
	bool are_all_seld_children_of_common_root = true; 
	for( unsigned int l = 0; l < k; ++l)
	{
		// this happens when one of the sel objs is the grandparent/parent of all other sel objs
		// in this case the common_root is a selected object
		// and only this needs to be inserted in the m_topFCOs/Folders
		if ( path_map[ l].length() == common_path.length() + 1 )
			are_all_seld_children_of_common_root = false;
	}

	if ( !are_all_seld_children_of_common_root) // there is one root/top obj; otherwise the for cycle belwo will do the job
		insertNew( common_root_id, true); // inserting the common root/top object


	// inserting the objects which stay between the common root and the top objects in the containment hierarchy
	for( unsigned int l = 0; l < k; ++l)
	{
		std::string rest_of_path = path_map[ l];
		rest_of_path = rest_of_path.substr( common_path.length() + 1, rest_of_path.length() - common_path.length() - 1); 

		bool first_after_the_common_root = true;
		while( !rest_of_path.empty())
		{
			std::string next_id;
			pos = rest_of_path.find( '/'); // no leading '/' but a tailing '/' is there anyway
			ASSERT( pos); // pos != 0

			if ( pos != -1)
			{
				next_id = rest_of_path.substr( 0, pos);
				rest_of_path = rest_of_path.substr( pos + 1, rest_of_path.length() - pos - 1);
			}
			else
			{
				ASSERT( 0);
				next_id = rest_of_path;
				rest_of_path = ""; // to end the loop
			}

			insertNew( next_id, are_all_seld_children_of_common_root && first_after_the_common_root);
			first_after_the_common_root = false;
		}

		// rest_of_path cannot be empty ( <=> first_after_the_common_root cannot be true) 
		// with 'are_all_seld_children_of_common_root' being true
		// p: all selected are children (not equal) of the common root
		// q: the remaining path is not empty <==> first after the common root is false
		// ASSERT( p -> q)  <=> ASSERT( !p || q)

		ASSERT( !are_all_seld_children_of_common_root || !first_after_the_common_root);
	}


	// inserting into the list of selected objects the newly selected ones
	m_selFolds.insert( m_selFolds.end(), m_newlySelFolds.begin(), m_newlySelFolds.end());
	m_selFcos.insert( m_selFcos.end(), m_newlySelFcos.begin(), m_newlySelFcos.end());

	m_newlySelFolds.clear();
	m_newlySelFcos.clear();
}

void CMakeClosure::checkMeta( const CComObjPtrVector<IMgaFolder>& p_folds, const CComObjPtrVector<IMgaFCO>& p_fcos)
{
	if ( p_folds.empty() && p_fcos.empty())
	{
		m_acceptingKindsAndFolders.push_back( rootfolder_str);
		return;
	}

	//
	// looking for potential folder containers
	unsigned int how_many_libraries = 0;
	std::set < std::string > goody_folds;
	bool goody_folds_init_done = false;
	for( CComObjPtrVector<IMgaFolder>::const_iterator fold_it = p_folds.begin(); fold_it != p_folds.end(); ++fold_it)
	{
		// obtaining its meta
		CComObjPtr<IMgaMetaFolder> f_meta;
		COMTHROW( (*fold_it)->get_MetaFolder( PutOut( f_meta)));
		
		// check if exists library among the top folders
		CComBstrObj kind_name;
		COMTHROW( f_meta->get_Name( PutOut(kind_name)) );
		std::string kind_nm;
		CopyTo( kind_name, kind_nm);
		if ( kind_nm == rootfolder_str)
		{
			++how_many_libraries;
			continue;
		}

		// will store the folder parents this folder may be part of
		std::set < std::string > actual;
		CComObjPtrVector<IMgaMetaFolder> act_vec;
		f_meta->get_UsedInFolders( PutOut( act_vec) );//zz1
		for( CComObjPtrVector<IMgaMetaFolder>::iterator a_i = act_vec.begin(); a_i != act_vec.end(); ++a_i)
		{
			CComBstrObj name;
			COMTHROW( (*a_i)->get_Name( PutOut(name)) );
			std::string s;
			CopyTo( name, s);

			actual.insert( s);
		}

		// calculate the intersection of the possible parent folders
		if ( goody_folds_init_done)
		{
			std::set < std::string > res;
			std::set_intersection( goody_folds.begin(), goody_folds.end(), actual.begin(), actual.end(), std::inserter( res, res.begin()));
			goody_folds = res;
		}
		else
		{
			goody_folds = actual;
			goody_folds_init_done = true;
		}
	}
	
	for( CComObjPtrVector<IMgaFCO>::const_iterator kind_it = p_fcos.begin(); kind_it != p_fcos.end(); ++kind_it)
	{
		// obtaining its meta
		CComObjPtr<IMgaMetaFCO> k_meta;
		COMTHROW( (*kind_it)->get_Meta( PutOut( k_meta)));

		// will store the folder parents this fco may be part of
		std::set < std::string > actual;
		CComObjPtrVector<IMgaMetaFolder> act_vec;
		k_meta->get_UsedInFolders( PutOut( act_vec) );//zz1
		for( CComObjPtrVector<IMgaMetaFolder>::iterator a_i = act_vec.begin(); a_i != act_vec.end(); ++a_i)
		{
			CComBstrObj name;
			COMTHROW( (*a_i)->get_Name( PutOut(name)) );
			std::string s;
			CopyTo( name, s);

			actual.insert( s);
		}

		// calculate the intersection of the possible parent folders ( goody_folds might be already filled from the cycle before)
		if ( goody_folds_init_done) // if goody_folds inited already
		{
			std::set < std::string > res;
			std::set_intersection( goody_folds.begin(), goody_folds.end(), actual.begin(), actual.end(), std::inserter( res, res.begin()));
			goody_folds = res;
		}
		else
		{
			goody_folds = actual;
			goody_folds_init_done = true;
		}
	}

	if( how_many_libraries == 0) //regular case
	{
		// these folders may contain all objects ( topFCOs and topFolders)
		for( std::set< std::string > :: iterator git = goody_folds.begin(); git != goody_folds.end(); ++git)
			m_acceptingKindsAndFolders.push_back( *git);
	}
	else // there are some libraries as top folders, the only possible target to paste in is the roofolder
	{
		if( p_folds.size() == how_many_libraries || // there are no other folders OR
			goody_folds.end() != goody_folds.find( rootfolder_str))  // the RootFolder is a good target for the regular folders and kinds
			m_acceptingKindsAndFolders.push_back( rootfolder_str);
	}

	
	//
	// looking for potential model containers
	
	if ( p_folds.empty()) // if folds needed to be inserted then models positively are not possible containers
	{	
		// will store the intersection of accepting models
		std::set < std::string > goody_models;
		bool goody_models_init_done = false;
		CComObjPtrVector<IMgaFCO>::const_iterator fco_it = p_fcos.begin();
		for( ; fco_it != p_fcos.end(); ++fco_it)
		{
			// will store the names of those models which may contain this kind (*fco_it)
			std::set < std::string > actual_models;
			CComObjPtr<IMgaMetaFCO> k_meta;
			COMTHROW( (*fco_it)->get_Meta( PutOut( k_meta)));
			
			// when checking the possible container kinds it is enough if we check the kind
			// name matching, since an acceptable kind (even with different role) can be inserted 
			// into a model (if ambiguities exist the resolver is invoked)
			// that is why the role the topFCOs have in the containing models is ignored

			CComPtr<IMgaMetaRoles> roles;
			k_meta->get_UsedInRoles( &roles); // the fco may be used in these roles in the models (in this paradigm)
			MGACOLL_ITERATE(IMgaMetaRole, roles) {
				// the parent model may contain the kind
				CComObjPtr<IMgaMetaModel> m;
				COMTHROW( MGACOLL_ITER->get_ParentModel( PutOut( m)));

				// take its name
				CComBstrObj name;
				COMTHROW( m->get_Name( PutOut(name)) );
				std::string s;
				CopyTo( name, s); // we have the name of a kind that may appear in the model

				actual_models.insert( s);
			}
			MGACOLL_ITERATE_END; 

			// calculate the intersection of the possible parent models
			if ( goody_models_init_done) // if set already
			{
				std::set < std::string > res;
				std::set_intersection( goody_models.begin(), goody_models.end(), actual_models.begin(), actual_models.end(), std::inserter( res, res.begin()));
				goody_models = res;
			}
			else
			{
				goody_models = actual_models;
				goody_models_init_done = true;
			}
		}

		// these models may contain all objects ( topFCOs only since topFolders is empty)
		for( std::set < std::string > :: iterator gid = goody_models.begin(); gid != goody_models.end(); ++gid)
		{
			m_acceptingKindsAndFolders.push_back( *gid);
		}
	}
}

// --------------------------------------------------------- transactions
void CMakeClosure::initTrans()
{
	//ASSERT( m_project == NULL ); // the constructor initialized it
	ASSERT( m_territory == NULL );

	m_territory = NULL;
	COMTHROW( m_project->CreateTerritory(NULL, PutOut(m_territory), NULL) );

	// needs write access, when guids are created for all objects in the closure
	COMTHROW( m_project->BeginTransaction(m_territory, TRANSACTION_GENERAL) );

	m_GME = get_GME(m_project);

	// put the objects into the new territory
	CComObjPtrVector<IMgaFCO>::iterator fco_it = m_selFcos.begin();
	for( ; fco_it != m_selFcos.end(); ++fco_it )
	{
		CComObjPtr<IMgaObject> obj;
		COMTHROW(m_territory->OpenObj(*fco_it, PutOut(obj)));
		CComObjPtr<IMgaFCO> fco;
		COMTHROW(obj.QueryInterface(fco));
		*fco_it = fco;
	}

	CComObjPtrVector<IMgaFolder>::iterator fold_it = m_selFolds.begin();
	for( ; fold_it != m_selFolds.end(); ++fold_it )
	{
		CComObjPtr<IMgaObject> obj;
		COMTHROW(m_territory->OpenObj(*fold_it, PutOut(obj)));
		CComObjPtr<IMgaFolder> fold;
		COMTHROW(obj.QueryInterface(fold));
		*fold_it = fold;
	}
}

void CMakeClosure::doneTrans(bool abort)
{
	if( m_territory != NULL )
		m_territory->Destroy();
	m_territory = NULL;

	if( m_project != NULL )
	{
		if( abort )
			m_project->AbortTransaction();
		else
			m_project->CommitTransaction();
	}
	m_project = NULL;
}

// --------------------------------------------------------- getters
void CMakeClosure::getFCOs( IMgaFCOs **res_fcos)
{
	CComPtr<IMgaFCOs> coll;
	COMTHROW(coll.CoCreateInstance(OLESTR("Mga.MgaFCOs")));
	
	CComObjPtrVector<IMgaFCO>::iterator i = m_selFcos.begin();
	for( ; i != m_selFcos.end(); ++i)
		COMTHROW(coll->Append( *i));

	*res_fcos = coll.Detach();
}

void CMakeClosure::getFolders( IMgaFolders **res_folds)
{
	CComPtr<IMgaFolders> coll;
	COMTHROW(coll.CoCreateInstance(OLESTR("Mga.MgaFolders")));
	
	CComObjPtrVector<IMgaFolder>::iterator i = m_selFolds.begin();
	for( ; i != m_selFolds.end(); ++i)
		COMTHROW(coll->Append( *i));

	*res_folds = coll.Detach();
}

bool CMakeClosure::noTopObjs()
{
	// this means that either the variables are not calculated
	// or The RootFolder is the common root object of all selected objects
	//	in the latter case for dumping and parsing reasons (is not nice if
	// the clipboard contains a project dumped?) another dumper is used
	return m_topFcos.empty() && m_topFolds.empty();
}

void CMakeClosure::clearTopObjs()
{
	m_topFcos.clear();
	m_topFolds.clear();
}

void CMakeClosure::getTopFCOs( IMgaFCOs **res_fcos)
{
	CComPtr<IMgaFCOs> coll;
	COMTHROW(coll.CoCreateInstance(OLESTR("Mga.MgaFCOs")));
	
	CComObjPtrVector<IMgaFCO>::iterator i = m_topFcos.begin();
	for( ; i != m_topFcos.end(); ++i)
		COMTHROW(coll->Append( *i));

	*res_fcos = coll.Detach();
}

void CMakeClosure::getTopFolders( IMgaFolders **res_folds)
{
	CComPtr<IMgaFolders> coll;
	COMTHROW(coll.CoCreateInstance(OLESTR("Mga.MgaFolders")));
	
	CComObjPtrVector<IMgaFolder>::iterator i = m_topFolds.begin();
	for( ; i != m_topFolds.end(); ++i)
		COMTHROW(coll->Append( *i));

	*res_folds = coll.Detach();
}

bool CMakeClosure::isAnyAccepting() const
{
	return m_acceptingKindsAndFolders.size() > 0;
}

void CMakeClosure::getAccKindsInString( std::string& p_resStr) const
{
	unsigned int i = 0;
	for( i = 0; i < m_acceptingKindsAndFolders.size(); ++i)
	{
		if(i) p_resStr += " ";

		p_resStr += m_acceptingKindsAndFolders[i];
	}
}

const std::vector< std::string >& CMakeClosure::getAccKindsVector() const
{
	return m_acceptingKindsAndFolders;
}

void CMakeClosure::send2Console( const std::string& msg)
{
	CComBSTR bstr( msg.c_str());
	if( m_GME) m_GME->ConsoleMessage( bstr, MSG_INFO);
	else ASSERT(0);// m_GME is not valid
}
