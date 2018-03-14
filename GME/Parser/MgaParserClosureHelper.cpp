#include "stdafx.h"
#include "Parser.h"
#include "MgaParser.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <stdio.h>

#include "../Common/CommonCollection.h"
#include "../Common/CommonMgaTrukk.h"
//#include <list>//slist
#include "helper.h"

// goes up in the object hierarchy as indicated by the number of '..'s in 'originalPath' and returns the new parent and remaining path info
void CMgaParser::stepUpInHierarchy( CComObjPtr<IMgaObject>& pCurrentObj, const std::tstring& originalPath, CComObjPtr<IMgaObject>& pNewParent, std::tstring& remainingPath)
{
	CComObjPtr<IMgaFCO> cur_fco;
	CComObjPtr<IMgaFolder> cur_fld;

	pCurrentObj.QueryInterface( cur_fco);
	pCurrentObj.QueryInterface( cur_fld);

	bool isfco = (cur_fco != NULL);

	std::tstring m = originalPath;
	
	while( m.substr(0,4) == _T("/@.."))
	{
		CComObjPtr<IMgaModel> mod;
		if( isfco) COMTHROW( cur_fco->get_ParentModel( PutOut( mod)) );

		if ( mod)	// has a valid model parent
		{
			cur_fco = mod;
		}
		else		// must have folder parent
		{
			CComObjPtr<IMgaFolder> fold;

			if( isfco)  COMTHROW( cur_fco->get_ParentFolder( PutOut( fold)) );
			else		COMTHROW( cur_fld->get_ParentFolder( PutOut( fold)) );
			
			if ( fold)
			{
				isfco = false;
				cur_fld = fold;
			}
			else // rootfolder reached: wrong path information
			{
				remainingPath = _T("");
				return; // pNewParent remains empty
			}
		}
		
		m = m.substr(4);
	}
	
	// no more '..' strings in the originalPath
	remainingPath = m;
	if (isfco && cur_fco) pNewParent = cur_fco;
	else if( cur_fld)     pNewParent = cur_fld;
}

void CMgaParser::findFCOWithRelPathAndGUID( CComObjPtr<IMgaObject> obj_rel_to, const std::tstring& relpath , const std::tstring& guid, CComObjPtr<IMgaFCO>& pFoundFCO)
{
	// rel_path gives an indication how much to step up in the hierarchy
	CComObjPtr<IMgaObject> new_parent;
	stepUpInHierarchy( obj_rel_to, relpath, new_parent, std::tstring());
	if( new_parent) // valid parent
		findFCOWithGUIDInTree( new_parent, guid, pFoundFCO);
}


void CMgaParser::findFCOWithGUID( CComObjPtr<IMgaObject> prev, const std::tstring& guid, CComObjPtr<IMgaFCO>& pFoundFco)
{
	CComObjPtr<IMgaModel> m;
	CComObjPtr<IMgaFolder> f;
	prev.QueryInterface( m);
	prev.QueryInterface( f);
	CComObjPtrVector<IMgaFCO> chld;

	if( m)
		COMTHROW( m->get_ChildFCOs( PutOut( chld)));
	else if (f)
		COMTHROW( f->get_ChildFCOs( PutOut( chld)));

	unsigned int i;
	for( i = 0; i < chld.size(); ++i)
	{
		CComBSTR bstr;
		COMTHROW( chld[i]->get_RegistryValue( CComBSTR( PREV_GLOBAL_ID_STR), &bstr));

		if( bstr != 0 && bstr.Length() == GLOBAL_ID_LEN) //valid
		{
			if( bstr == CComBSTR( guid.c_str()))
				break;
		}
	}

	if( chld.size() > 0 && i < chld.size()) // found
		pFoundFco = chld[i];

}

void CMgaParser::findFCOWithGUIDInTree( CComObjPtr<IMgaObject> pParent, const std::tstring& guid, CComObjPtr<IMgaFCO>& pFoundFCO)
{
#if(1)
	// breadth-first
	CComObjPtrVector<IMgaObject> queue;
	unsigned int curr_i = 0;
	queue.push_back( pParent);

	while( !pFoundFCO && curr_i != queue.size())
	{
		CComObjPtr<IMgaFolder> qF;
		CComObjPtr<IMgaModel>  qM;
		CComObjPtr<IMgaFCO>    qfco;
		queue[curr_i].QueryInterface( qF);
		queue[curr_i].QueryInterface( qM);
		queue[curr_i].QueryInterface( qfco);

		// check if the current object has the searched GUID
		CComBSTR bstr;

		// IMgaObject has no get_RegistryValue property
		if( qF) COMTHROW( qF->get_RegistryValue( CComBSTR( PREV_GLOBAL_ID_STR), &bstr));
		else if( qfco) COMTHROW( qfco->get_RegistryValue( CComBSTR( PREV_GLOBAL_ID_STR), &bstr));

		if( bstr != 0 && bstr.Length() == GLOBAL_ID_LEN && bstr == CComBSTR( guid.c_str())) //valid and equal
			queue[curr_i].QueryInterface( pFoundFCO); // found if succeeds

		CComObjPtrVector<IMgaFCO> chld;

		// in case Folder or Model put some new children into the queue
		if( qF) 
		{
			CComObjPtrVector<IMgaFolder> fols;
			COMTHROW( qF->get_ChildFolders( PutOut( fols)));
			// append the children folders to the end of queue
			for( CComObjPtrVector<IMgaFolder>::const_iterator it = fols.begin(); it != fols.end(); ++it)
				queue.insert( queue.end(), CComObjPtr<IMgaObject>( *it));
			// there was a migration issue with:
			//queue.insert( queue.end(), fols.begin(), fols.end());

			COMTHROW( qF->get_ChildFCOs( PutOut( chld)));
		}
		else if ( qM) COMTHROW( qM->get_ChildFCOs( PutOut( chld)));

		// append the children fcos to the end of queue
		for( CComObjPtrVector<IMgaFCO>::const_iterator it = chld.begin(); it != chld.end(); ++it)
			queue.insert( queue.end(), CComObjPtr<IMgaObject>( *it));
		// there was a migration issue with:
		//queue.insert( queue.end(), chld.begin(), chld.end());

		++curr_i;
	}

#else // depth first
	CComObjPtr<IMgaFolder> f;
	CComObjPtr<IMgaModel>  m;
	pParent.QueryInterface( m);
	pParent.QueryInterface( f);
	
	CComObjPtrVector<IMgaFCO> chld;
	CComObjPtrVector<IMgaFolder> fols;

	if( m) 
		COMTHROW( m->get_ChildFCOs( PutOut( chld)));
	else if (f)	
	{
		COMTHROW( f->get_ChildFCOs( PutOut( chld)));
		COMTHROW( f->get_ChildFolders( PutOut( fols)));

	}
	
	CComBSTR bstr;
	// perform a depth-first search
	for( unsigned int i = 0; !pFoundFCO && i < chld.size(); ++i)
	{
		bstr.Empty();
		COMTHROW( chld[i]->get_RegistryValue( CComBSTR( PREV_GLOBAL_ID_STR), &bstr));

		if( bstr != 0 && bstr.Length() == GLOBAL_ID_LEN && bstr == CComBSTR( guid.c_str())) //valid and equal
		{
			pFoundFCO = chld[i]; // found
		}
	}

	// if not found invoke search for children as well
	for( unsigned int i = 0; !pFoundFCO && i < chld.size(); ++i)
		findFCOWithGUIDInTree( CComObjPtr<IMgaObject>( chld[i]), guid, pFoundFCO);
	for( unsigned int i = 0; !pFoundFCO && i < fols.size(); ++i)
		findFCOWithGUIDInTree( CComObjPtr<IMgaObject>( fols[i]), guid, pFoundFCO);
#endif
}


bool CMgaParser::findObjectIn( const CComObjPtr<IMgaObject>& p_parent, const CComObjPtrVector<IMgaFCO>& p_vec, const attributes_type& p_attributes, CComObjPtr<IMgaFCO>& p_obj, const char p_typeRequested)
{
	// if the user invoked "paste closure/smart" then merge is not allowed, do not look for existing objects
	if( !m_mergeAllowed) return false; 

	// TODO : use role to narrow search
	//std::tstring role_in_clipdata = GetByName(p_attributes, "role");

	std::tstring id_in_clipdata = GetByName(p_attributes, _T("closureguid")); // guid
	ASSERT( id_in_clipdata.length() == GLOBAL_ID_LEN); 

	bool found = false;
	CComObjPtrVector<IMgaFCO>::const_iterator it = p_vec.begin();
	while( it != p_vec.end() && !found)
	{
		CComBSTR bstr;
		CComObjPtr<IMgaFCO> i = *it;
		COMTHROW( i->get_RegistryValue( CComBSTR(PREV_GLOBAL_ID_STR), &bstr));
		if( bstr == id_in_clipdata.c_str())
		{
			found = true;
			p_obj = i;
		}
		else
			++it;
	}
	if( found && p_obj)
	{
		// checking object type
		switch( p_typeRequested) {
			case 'M':
				{
					CComObjPtr<IMgaModel> _t_m;
					COMTHROW( p_obj.QueryInterface( _t_m));
					found = _t_m != 0;
					break;
				}
			case 'A':
				{
					CComObjPtr<IMgaAtom> _t_a;
					COMTHROW( p_obj.QueryInterface( _t_a));
					found = _t_a != 0;
					break;
				}
			case 'R':
				{
					CComObjPtr<IMgaReference> _t_r;
					COMTHROW( p_obj.QueryInterface( _t_r));
					found = _t_r != 0;
					break;
				}
			case 'C':
				{
					CComObjPtr<IMgaConnection> _t_c;
					COMTHROW( p_obj.QueryInterface( _t_c));
					found = _t_c != 0;
					break;
				}
			case 'S':
				{
					CComObjPtr<IMgaSet> _t_s;
					COMTHROW( p_obj.QueryInterface( _t_s));
					found = _t_s != 0;
					break;
				}
			case 'F':
				{
					CComObjPtr<IMgaFolder> _t_f;
					COMTHROW( p_obj.QueryInterface( _t_f));
					found = _t_f != 0;
					break;
				}
			case '*':  // means that we don't know the object's type a priori
			default :
				{
					found = p_obj != 0;
					break;
				}
		};
	}

	if( !found                    // id not stored in registry (or different ids found only)
	   && p_typeRequested != 'C') // but the names may allow us to merge
	{                             // except if Connection (usually having same names)
		const std::tstring* s = 0;
		s = GetByNameX(p_attributes, _T("closurename"));
		if( s != 0 && s->compare(_T("")) != 0)
		{
			CComObjPtr<IMgaObject> obj2;
			findObjOnRelPath( CComObjPtr<IMgaObject>(p_parent), *s, obj2, _T("mergable object"));
			if( obj2) COMTHROW( obj2.QueryInterface( p_obj));
			found = p_obj != 0;
		}
	}
	return found;
}

bool CMgaParser::findObject( const CComObjPtr<IMgaModel>& p_prev, const attributes_type& p_attributes, CComObjPtr<IMgaFCO>& p_obj, const char p_typeRequested)
{
	CComObjPtrVector<IMgaFCO> chld;
	if( GetByNameX( p_attributes, _T("kind")))
		COMTHROW( p_prev->GetChildrenOfKind( PutInBstrAttr(p_attributes, _T("kind")), PutOut( chld)));
	else
		COMTHROW(p_prev->get_ChildFCOs( PutOut( chld)));

	return findObjectIn( CComObjPtr<IMgaObject>(p_prev), chld, p_attributes, p_obj, p_typeRequested);
}

bool CMgaParser::findObject( const CComObjPtr<IMgaFolder>& p_prev, const attributes_type& p_attributes, CComObjPtr<IMgaFCO>& p_obj, const char p_typeRequested)
{
	CComObjPtrVector<IMgaFCO> chld;
	
	if( GetByNameX( p_attributes, _T("kind")))
		COMTHROW( p_prev->GetChildrenOfKind( PutInBstrAttr(p_attributes, _T("kind")), PutOut( chld)));
	else
		COMTHROW(p_prev->get_ChildFCOs( PutOut( chld)));

	return findObjectIn( CComObjPtr<IMgaObject>(p_prev), chld, p_attributes, p_obj, p_typeRequested);
}

bool CMgaParser::findFolderIn( const CComObjPtr<IMgaFolder>& p_prev, const attributes_type& p_attributes, CComObjPtr<IMgaFolder>& p_folder)
{
	// if the user invoked ''paste closure/smart'' then merge is not allowed, do not look for existing objects
	if( !m_mergeAllowed) return false; 

	CComObjPtrVector<IMgaFolder> chld;
	COMTHROW(p_prev->get_ChildFolders( PutOut( chld)));

	std::tstring req_kind = GetByName( p_attributes, _T("kind"));

	std::tstring id_in_clipdata = GetByName(p_attributes, _T("closureguid"));
	ASSERT( id_in_clipdata.length() == GLOBAL_ID_LEN);

	bool found = false;
	CComObjPtrVector<IMgaFolder>::const_iterator it = chld.begin();
	while( it != chld.end() && !found)
	{
		CComBSTR bstr;
		CComObjPtr<IMgaFolder> i = *it;
		COMTHROW( i->get_RegistryValue( CComBSTR(PREV_GLOBAL_ID_STR), &bstr)); 
		CComObjPtr<IMgaMetaFolder> i_metaf;
		COMTHROW( i->get_MetaFolder( PutOut( i_metaf)));
		CComBSTR i_kind;
		COMTHROW( i_metaf->get_Name( &i_kind));

		if( bstr == id_in_clipdata.c_str() && i_kind == req_kind.c_str())
		{
			found = true;
			p_folder = i;
		}
		else
			++it;
	}
	if( !found)  // id not stored in registry (or different ids found only)
	{           // but the names may allow us to merge
		const std::tstring* s = 0;
		s = GetByNameX(p_attributes, _T("closurename"));
		if( s != 0 && s->compare(_T("")) != 0)
		{
			CComObjPtr<IMgaObject> obj2;
			findObjOnRelPath( CComObjPtr<IMgaObject>(p_prev), *s, obj2, _T("mergable object"));
			if( obj2) COMTHROW( obj2.QueryInterface( p_folder));
			found = p_folder != 0;
		}
	}
	return found;
}
void CMgaParser::findObjOnAbsPath( CComObjPtr<IMgaProject> p_project, const std::tstring& p_absPath , CComObjPtr<IMgaObject>& p_obj, const std::tstring& text)
{
	COMTHROW( p_project->get_ObjectByPath( _bstr_t(p_absPath.c_str()), PutOut( p_obj)) );
	if( !p_obj) // if nonunique is allowed
	{
		COMTHROW( p_project->get_NthObjectByPath( 0, _bstr_t(p_absPath.c_str()), PutOut( p_obj)) );

		if( p_obj)
		{
			// name ambiguity exists for sure
			//<!> warning about non uniqueness
			if( m_GME) 
			{
				CComBSTR bstr;
				COMTHROW(bstr.Append(_T("Name ambiguity, selected: ")));
				COMTHROW(bstr.AppendBSTR( makeLink( p_obj)));
				
				if( !text.empty())
				{
					COMTHROW(bstr.Append(L" as "));
					COMTHROW(bstr.Append( text.c_str()));
				}

				COMTHROW(bstr.Append(L". Search path used: "));
				COMTHROW(bstr.Append( makeViewable(p_absPath).c_str()));
				msgSC( bstr, MSG_WARNING);
			}
		}
	}
}

void CMgaParser::findObjOnRelPath( CComObjPtr<IMgaObject> obj_rel_to, const std::tstring& relpath , CComObjPtr<IMgaObject>& obj, const std::tstring& text)
{
	ASSERT( !relpath.empty());
	ASSERT( obj_rel_to);

	CComObjPtr<IMgaFCO> cur_fco;
	CComObjPtr<IMgaFolder> cur_fld;

	obj_rel_to.QueryInterface( cur_fco);
	obj_rel_to.QueryInterface( cur_fld);

	bool isfco = (cur_fco != NULL);

	std::tstring m = relpath;
	
	while( m.substr(0,4) == _T("/@.."))
	{
		CComObjPtr<IMgaModel> mod;
		if( isfco) COMTHROW( cur_fco->get_ParentModel( PutOut( mod)) );

		if ( mod)	// has a valid model parent
		{
			cur_fco = mod;
		}
		else		// must have folder parent
		{
			CComObjPtr<IMgaFolder> fold;

			if( isfco)  COMTHROW( cur_fco->get_ParentFolder( PutOut( fold)) );
			else		COMTHROW( cur_fld->get_ParentFolder( PutOut( fold)) );
			
			if ( fold)
			{
				isfco = false;
				cur_fld = fold;
			}
			// else rootfolder reached: wrong path information
		}
		
		m = m.substr(4);
	}
	
	if( m.empty())
	{
		if (isfco)	obj = cur_fco;
		else 		obj = cur_fld;
	}
	else if( isfco)
		COMTHROW( cur_fco->get_ObjectByPath( PutInBstr( m), PutOut( obj)) );
	else
		COMTHROW( cur_fld->get_ObjectByPath( PutInBstr( m), PutOut( obj)) );

	if( !obj) // not found , name ambiguity may exist, take the 1st
	{
		if( isfco)
			COMTHROW( cur_fco->get_NthObjectByPath( 0, PutInBstr( m), PutOut( obj)) );
		else
			COMTHROW( cur_fld->get_NthObjectByPath( 0, PutInBstr( m), PutOut( obj)) );

		if( obj) // name ambiguity exists for sure
		{
			if( m_GME) 
			{
				CComBSTR bstr;
				COMTHROW(bstr.Append(L"Name ambiguity, selected: "));
				COMTHROW(bstr.AppendBSTR( makeLink( obj)));
				if( !text.empty())
				{
					COMTHROW(bstr.Append(L" as "));
					COMTHROW(bstr.Append( text.c_str()));
				}

				COMTHROW(bstr.Append(L". Search path used: "));
				COMTHROW(bstr.Append( makeViewable(relpath).c_str()));
				msgSC( bstr, MSG_WARNING);
			}
		}
	}
}

bool CMgaParser::findPlaceForElem( 
							     const attributes_type &attributes
							   , deriv_type& deriv
							   , CComObjPtr<IMgaObject>& place
							)
{
	const std::tstring *nm = GetByNameX(attributes, _T("closurepath"));
	if( nm != NULL)
	{
		if( nm->compare(_T("")) == 0)
		{
			place = m_target;
			return true;
		}
		else
		{
			findObjOnRelPath( m_target, *nm, place, _T("place"));
			if( place)
				return true;
			else
				ASSERT(0);
		}
	}
	return false;
}

//<!> check if findObjOnRelPath is given "" string

bool CMgaParser::findPlaceForElem( 
							     const attributes_type &attributes
							   , deriv_type& deriv
							   , CComObjPtr<IMgaFolder>& place_folder
							   , CComObjPtr<IMgaModel>& place_model
							)
{
	
	CComObjPtr<IMgaObject> place;
	if( findPlaceForElem( attributes, deriv, place))
	{
		place.QueryInterface( place_folder);
		place.QueryInterface( place_model);
		return (place_folder || place_model);
	}
	else
	{
		place = m_target;//<!> let's try this
		
		CComBSTR bstr("Correct place not found for object: ");
		COMTHROW(bstr.Append( makeNameViewable( GetByName( attributes, _T("closurename"))).c_str()));
		COMTHROW(bstr.Append(". Search path used: "));
		COMTHROW(bstr.Append( makeViewable( GetByName(attributes, _T("closurepath"))).c_str()));
		COMTHROW(bstr.Append(". Trying to insert into the target object: "));
		COMTHROW(bstr.AppendBSTR( makeLink( place)));
		COMTHROW(bstr.Append("."));
		msgSC(bstr, MSG_ERROR);

		place.QueryInterface( place_folder);
		place.QueryInterface( place_model);
		return (place_folder || place_model);
	}
	return false;
}

/*static*/ CComPtr<IGMEOLEApp> CMgaParser::get_GME(CComObjPtr<IMgaProject> project)
{
	CComPtr<IGMEOLEApp> gme;
	if ( (project != NULL)) {		
		CComBSTR bstrName(L"GME.Application");
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

/*static*/ void CMgaParser::clear_GME( CComPtr<IGMEOLEApp>& p_GME)
{
	p_GME = CComPtr<IGMEOLEApp>();
}


bool CMgaParser::isNeedFor2ndStep()
{
	return !m_notFoundReferredObject.empty() || !m_notFoundSetMembers.empty();
}

void CMgaParser::tryToFindMissedReferreds()
{
	std::map< CComObjPtr<IMgaFCO>, std::tstring, CompareCComObj >::iterator it = m_notFoundReferredObject.begin();
	std::map< CComObjPtr<IMgaFCO>, std::tstring, CompareCComObj >::iterator itend = m_notFoundReferredObject.end();
	for( ; it != itend; ++it)
	{
		if( !it->first) continue;
		bool error = false;

		CComObjPtr<IMgaObject> target;
		findObjOnRelPath( CComObjPtr<IMgaObject>( it->first), it->second, target, _T("referred object"));
		if( target)
		{
			CComObjPtr<IMgaReference> ref;
			COMTHROW( it->first.QueryInterface( ref));
			CComObjPtr<IMgaFCO> fco_target;
			COMTHROW( target.QueryInterface( fco_target));

			if( ref && fco_target)
			{
				COMTHROW( ref->put_Referred( fco_target));

				CComBSTR bstr( "Reference ");
				COMTHROW(bstr.AppendBSTR( makeLink( ref)));
				COMTHROW(bstr.Append( " set to refer to "));
				COMTHROW(bstr.AppendBSTR( makeLink( fco_target)));
				COMTHROW(bstr.Append(" in 2nd step successfully."));
				msgSC(bstr, MSG_INFO);
			}
			else
				error = true;
		}
		else 
			error = true;

		if( error)
		{
			CComBSTR bstr( "Reference ");
			COMTHROW(bstr.AppendBSTR( makeLink( it->first )));
			COMTHROW(bstr.Append( ": target not found in 2nd step. "));
			COMTHROW(bstr.Append("Search path used: "));
			COMTHROW(bstr.Append( makeViewable( it->second).c_str()));
			msgSC(bstr, MSG_ERROR);
		}
	}
}

void CMgaParser::tryToFindMissedSetMembers()
{
	std::map< CComObjPtr<IMgaFCO>, std::vector< std::tstring >, CompareCComObj >::iterator it = m_notFoundSetMembers.begin();
	std::map< CComObjPtr<IMgaFCO>, std::vector< std::tstring >, CompareCComObj >::iterator itend = m_notFoundSetMembers.end();
	for( ; it != itend; ++it)
	{
		if( !it->first || it->second.empty()) continue;

		CComObjPtr<IMgaSet> set;
		COMTHROW( it->first.QueryInterface( set));

		if( !set) continue;

		std::vector< std::tstring >::iterator member_it = it->second.begin();
		std::vector< std::tstring >::iterator member_it_end = it->second.end();
		for( ; member_it != member_it_end; ++member_it)
		{
			if( member_it->empty()) continue;

			bool error = false;
			CComObjPtr<IMgaObject> member;
			findObjOnRelPath( CComObjPtr<IMgaObject>( it->first), *member_it, member, _T("set member"));
			if( member)
			{
				CComObjPtr<IMgaFCO> fco_member;
				COMTHROW( member.QueryInterface( fco_member));

				if( fco_member)
				{
					COMTHROW( set->AddMember( fco_member));

					CComBSTR bstr( "Member");
					COMTHROW(bstr.AppendBSTR( makeLink( fco_member)));
					COMTHROW(bstr.Append( " added to set "));
					COMTHROW(bstr.AppendBSTR( makeLink( set)));
					COMTHROW(bstr.Append(" in 2nd step successfully."));
					msgSC(bstr, MSG_INFO);
				}
				else
					error = true;
			}
			else
				error = true;

			if( error)
			{
				CComBSTR bstr( "Set ");
				COMTHROW(bstr.AppendBSTR( makeLink( set)));
				COMTHROW(bstr.Append(": member not found in 2nd step. "));
				COMTHROW(bstr.Append("Search path used: "));
				COMTHROW(bstr.Append( makeViewable( *member_it).c_str()));
				msgSC(bstr, MSG_ERROR);
			}
		}
	}
}

/*static*/ bool CMgaParser::isEmptySet( CComObjPtr<IMgaFCO>& fco)
{
	bool empty = false;
	CComObjPtr<IMgaSet> t_set;
	if( fco) 
	{
		COMTHROW( fco.QueryInterface( t_set));
		if( t_set) 
		{
			CComObjPtrVector<IMgaFCO> mems;
			COMTHROW( t_set->get_Members( PutOut( mems)));
			if( mems.size() == 0) empty = true;
		}
	}
	return empty;
}

/*static*/ bool CMgaParser::isNullRef( CComObjPtr<IMgaFCO>& fco)
{
	bool null = false;
	CComObjPtr<IMgaReference> t_ref;
	ASSERT( fco);
	if( fco)
	{
		COMTHROW( fco.QueryInterface( t_ref));
		if( t_ref) 
		{
			CComObjPtr<IMgaFCO> referred;
			COMTHROW( t_ref->get_Referred( PutOut(referred)));
			if( !referred) null = true;
		}
	}
	return null;
}

void CMgaParser::msgSC( CComBSTR& msg, msgtype_enum type)
{
	static const TCHAR * sc_text = _T("[Smartcopy Parser] ");
	CComBSTR m2;
	std::tstring t;
	CopyTo( msg, t);
	if( t.substr( 0, _tcslen( sc_text)) != sc_text)
	{
		COMTHROW(m2.Append( sc_text));
		COMTHROW(m2.AppendBSTR( msg));
	}
	else
		m2 = msg;

	if( m_GME) COMTHROW( m_GME->ConsoleMessage( m2, type));
}