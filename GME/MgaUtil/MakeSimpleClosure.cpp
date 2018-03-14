#include "stdafx.h"
#include "MakeSimpleClosure.h"
#include "GUIDCreate.h"
#include <algorithm>

//---------------------------------------------------------------------------
//  C  L  A  S  S   CSimpleSelectClosure
//                  enabler of SmartCopy
//---------------------------------------------------------------------------
#define GLOBAL_ID_STR   "guid"

bool CSimpleSelectClosure::isValidSelection()
{
	// be more permissive than the base class
	// allows connections thru refport also
	return true;
}

void CSimpleSelectClosure::manageGuid( CComObjPtr<IMgaObject> object)
{
	if( object)
	{
		CComObjPtr<IMgaRegNode> regnode;

		CComObjPtr<IMgaFCO> fco;
		object.QueryInterface( fco);
		if( fco) COMTHROW( fco->get_RegistryNode( CComBSTR( GLOBAL_ID_STR), PutOut( regnode)));
		else
		{
			CComObjPtr<IMgaFolder> folder;
			object.QueryInterface( folder);
			if( folder) COMTHROW( folder->get_RegistryNode( CComBSTR( GLOBAL_ID_STR), PutOut( regnode)));
			else ASSERT(0);
		}

		// in case object is in Library, do not write anything into registry
		VARIANT_BOOL vbLibrary;
		COMTHROW( object->get_IsLibObject( &vbLibrary ) );

		if( regnode && !vbLibrary)
		{
			long status;
			COMTHROW( regnode->get_Status(&status) );
			if( status != ATTSTATUS_HERE) //UNDEFINED, METADEF, INVALID, ARCHETYPE1,2,...
				COMTHROW( regnode->put_Value( GuidCreate::newGuid()));
		}
	}
}

// there is not much magic: process down the hierarchy (folders and models)
void CSimpleSelectClosure::procObj( IMgaObject* obj)
{
	ASSERT( obj != NULL );
	//!m_bIntoLibraries <=> m_iIntoLibraries != 2
	//if ( m_iIntoLibraries != LIB_CONT && isInLibrary( obj)) // if no intention to step into libraries and the object is in a library then return
	//	return;
	// library elements are handled the same way as the usual objects, 
	// except no guids are stored in their registry

	objtype_enum objtype;
	COMTHROW( obj->get_ObjType( &objtype) );

	// now mark the object itself (with a guid), later mark its dependents (ref, mem)
	manageGuid( obj);

	switch( objtype)
	{
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

	case OBJTYPE_ATOM: break;
	case OBJTYPE_REFERENCE:
		{
			CComObjPtr<IMgaReference> r;
			COMTHROW( ::QueryInterface( obj, r) );

			// mark its referred object (if any)
			CComObjPtr<IMgaFCO> refd;
			COMTHROW( r->get_Referred( PutOut( refd)));
			manageGuid( CComObjPtr<IMgaObject>( refd));
			break;
		}
	case OBJTYPE_CONNECTION:
		{
			CComObjPtr<IMgaConnection> c;
			COMTHROW( ::QueryInterface( obj, c) );
			
			CComObjPtrVector<IMgaConnPoint> cps;
			COMTHROW( c->get_ConnPoints( PutOut( cps)));
			for( unsigned int i = 0; i < cps.size(); ++i)
			{
				// mark the endpoint
				CComObjPtr<IMgaFCO> target;
				COMTHROW( cps[i]->get_Target(PutOut(target)) );
				manageGuid( CComObjPtr<IMgaObject>( target));

				// mark the ref chain
				CComObjPtrVector<IMgaFCO> refs;
				COMTHROW( cps[i]->get_References(PutOut(refs)) );
				for( unsigned int i = 0; i < refs.size(); ++i)
					manageGuid( CComObjPtr<IMgaObject>( refs[i]));
			}

			break;
		}
	case OBJTYPE_SET:
		{
			CComObjPtr<IMgaSet> s;
			COMTHROW( ::QueryInterface( obj, s) );
			CComObjPtrVector<IMgaFCO> mems;
			COMTHROW( s->get_Members( PutOut( mems)));
			for( unsigned int i = 0; i < mems.size(); ++i)
				manageGuid( CComObjPtr<IMgaObject>( mems[i]));
			break;
		}
	};
}

void CSimpleSelectClosure::preProcess()
{
	send2Console("[Smart Copy] Working...");
}

void CSimpleSelectClosure::postProcess()
{
	//remove specified unneeded kinds 
	if( removeExcludedKinds()) 
	{   // all objects were removed by the filter
		send2Console( "[Smart Copy] No object in selection.");
		return;
	}

	// sort the elements based on: basetypes first, connections last rule
	std::sort( m_selFcos.begin(), m_selFcos.end(), ConnsAndDerivsLast());

	selectTop(); // calculate the parentless objects
	autoWrap();  // wrapping (resets the Top values)
	             // normally if objects selected from the View
	             // would not wrap much
	             // if initial objects selected from the activebrowser
	             // it might have to do something
	
	checkMeta( m_topFolds, m_topFcos); // calculate the accepting kind

	std::string res_str;
	getAccKindsInString( res_str);
	if( !res_str.empty()) send2Console( "[Smart Copy] The following kinds may accept the copied data: " + res_str + ".");
	else send2Console( "[Smart Copy] Possible accepting kind not found based on the meta.");
}

bool CSimpleSelectClosure::removeExcludedKinds()
{
	// we filter out the excluded kinds even from the initially selected objects
	// that is why we don't use the base's kindfilter mechanism (that filters out
	// only children of models, folders)
	// worth to mention: connections are initially selected by the GUI if both of
	// their ends is selected also, thus it may happen that the user would like
	// connections excluded, but if they are initially selected by the mechanism
	// outlined above then the base's filter would not catch that
	CComObjPtrVector<IMgaFCO>::iterator it = m_selFcos.begin();
	while( it != m_selFcos.end())
	{
		CComQIPtr<IMgaAtom> a = *it;
		CComQIPtr<IMgaConnection> c = *it;
		CComQIPtr<IMgaReference> r = *it;
		CComQIPtr<IMgaSet> s = *it;

		if( m_bExclAtom && a // if atom and atoms need to be excluded OR
		 || m_bExclConn && c // ...
		 || m_bExclRef  && r
		 || m_bExclSet  && s)
			it = m_selFcos.erase( it); // erase will step forward the iterator
		else
			++it;
	}
	
	return m_selFcos.size() + m_selFolds.size() == 0; // all objects were removed by the filter
}

bool ConnsAndDerivsLast::operator()( const CComObjPtr<IMgaFCO>& p1, const CComObjPtr<IMgaFCO>& p2) const
{
	// used for sort. 
	// 0th rule: derived types will be put after the archetypes
	// 1st rule: Connections will be put in the back of the vector
	// 2nd rule: if the two elements are both connections or none of them is connection then sort on ID
	// returns true if p1 precedes p2

	objtype_enum objtype1, objtype2;
	CComObjPtr<IMgaFCO> bas1, bas2;
	COMTHROW( p1->get_ArcheType( PutOut(bas1))); // returns 0 for archetypes
	COMTHROW( p2->get_ArcheType( PutOut(bas2)));
	bool p1_is_der = bas1 != 0;
	bool p2_is_der = bas2 != 0;
	COMTHROW( p1->get_ObjType( &objtype1));
	COMTHROW( p2->get_ObjType( &objtype2));

	// compare on deriv or not?
	bool sim = objtype1 == objtype2; 
	if( sim && p1_is_der != p2_is_der)
		return p2_is_der;//IF p1 is F, p2 is T ->ret true, IF p1 is T, p2 is F ->ret false

	// sim is F or p1_is_der == p2_is_der

	// compare on conn or not?
	bool c1 = objtype1 == OBJTYPE_CONNECTION;
	bool c2 = objtype2 == OBJTYPE_CONNECTION;

	if( c1 == c2)
	{
		//return true;// the predicate must be a strict weak ordering, so let's compare the IDs
		CComBstrObj bstr1, bstr2;
		COMTHROW( p1->get_ID( PutOut( bstr1)));
		COMTHROW( p2->get_ID( PutOut( bstr2)));
		return bstr1.Compare( bstr2) < 0;
	}
	else 
		return c2; // IF c2 is T, c1 is F -> ret true, IF c2 is F, c1 is T ->ret false
}
