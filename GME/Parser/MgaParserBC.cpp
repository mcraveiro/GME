#include "stdafx.h"
#include "Parser.h"
#include "MgaParser.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <stdio.h>

#include "../Common/CommonCollection.h"
#include "../Common/CommonMgaTrukk.h"
#include <list>//slist
#include "helper.h"

// --------------------------- CMgaParser - basic copy closure dumped data parsing

STDMETHODIMP CMgaParser::ParseClos1(IMgaObject *here, BSTR filename)
{
	CHECK_IN(here);

	try
	{
		COMTHROW( progress.CoCreateInstance(L"Mga.MgaProgressDlg") );
		COMTHROW( progress->SetTitle(_bstr_t(L"Importing CopyClosured XML data...")) );
		COMTHROW( progress->StartProgressDialog(NULL) );

		CComObjPtr<IMgaProject> p;
		COMTHROW( here->get_Project(PutOut(p)) );
		ASSERT( p != NULL );
		COMTHROW( p->get_Preferences(&project_prefs_orig) );
		manual_relid_mode = project_prefs_orig & MGAPREF_MANUAL_RELIDS ? true : false;

		m_GME = get_GME( p); // by zolmol

		COMTHROW( p->Notify(APPEVENT_XML_IMPORT_SPECIAL_BEGIN));

		COMTHROW( p->CreateTerritory(NULL, PutOut(territory), NULL) );
		COMTHROW( p->BeginTransaction(territory, TRANSACTION_NON_NESTED) );

		CComObjPtr<IMgaObject> target;
		COMTHROW(territory->OpenObj(here, PutOut(target)));

		//by zolmol
		m_maintainGuids = false; // do not preserve guids found in data pasted
		m_target = target;
		m_resolveDerFuncPtr = &CMgaParser::ResolveClosure1Derivation;
		//m_clVer = 1;
		m_clVerStr = _T("1");//end

		if( m_target && m_GME) 
		{
			CComBSTR bstr, id, nm;
			COMTHROW( m_target->get_ID( &id));
			COMTHROW( m_target->get_Name( &nm));
			COMTHROW(bstr.Append(L"[Closure Parser] Inserting copy closured data into: <A HREF=\"mga:"));
			COMTHROW(bstr.AppendBSTR( id));
			COMTHROW(bstr.Append(L"\">"));
			COMTHROW(bstr.AppendBSTR( nm));
			COMTHROW(bstr.Append(L"</A>"));
			COMTHROW( m_GME->ConsoleMessage(bstr, MSG_INFO));
		}

		project_prefs = project_prefs_orig | MGAPREF_IGNORECONNCHECKS;
		COMTHROW( p->put_Preferences(project_prefs) );

		COMTHROW( resolver.CoCreateInstance(L"Mga.MgaResolver") );
		ASSERT( resolver != NULL );

		project = p;

		CopyTo(filename, xmlfile);

		XMLPlatformUtilsTerminate_RAII term;
		try
		{
			SAXParser parser;
			parser.setValidationScheme(SAXParser::Val_Always);
			parser.setDocumentHandler(this);
			parser.setErrorHandler(this);
			parser.setEntityResolver(this);

			//elementfuncs = elementfuncs_bcmga;
			funcTableState = BC_MGA;

			// manual first pass

			pass_count = 1;

			ranges.clear();
			ranges.push_front(range_type());
			ranges.front().begin = 1;
			ranges.front().end = (counter_type)-1;
			ranges.front().previous.name = _T("start");
			ranges.front().previous.object = target;
			skip_element_level = 0;
			
			parser.parse(xmlfile.c_str());

			ASSERT( ranges.front().begin == 1 );
			ranges.pop_front();
			elements.clear();

			max_counter = counter;

			// the other passes

			parser.setValidationScheme(SAXParser::Val_Never);

			while( !ranges.empty() && ranges.front().begin != (counter_type)-1 )
			{
				// FIXME: better algorithm for infinite loop
				if( ++pass_count >= 100 )
					HR_THROW(E_TOOMANYPASSES);

				parser.parse(xmlfile.c_str());
			}

			ASSERT( elements.empty() );
			ranges.clear();
		}
	    catch(const XMLException &e)
		{
			XmlStr desc(e.getMessage());

			ThrowXmlError(L"%s", desc.c_str());
		}
		COMTHROW( project->put_Preferences(project_prefs_orig) );

		for(librecords::reverse_iterator i = libstodo.rbegin(); i != libstodo.rend(); i++) { // copied from ParseProject in order to recognize libraries (zolmol)
			COMTHROW(i->f->put_LibraryName(CComBSTR(i->libname.c_str())));
			COMTHROW(i->f->put_Exempt(VARIANT_FALSE));
		}
		libstodo.clear();

		COMTHROW( project->CommitTransaction() );
		COMTHROW( project->Notify(APPEVENT_XML_IMPORT_SPECIAL_END));
		project = NULL;
		clear_GME( m_GME);

		CloseAll();
	}
	catch(hresult_exception &e)
	{
		CloseAll();
		// in case we rethrew the [probably MGA originated] exception 
		// we have set into errorinfo the location info
		if( m_GME)
			COMTHROW(m_GME->ConsoleMessage( errorinfo, MSG_ERROR));
		clear_GME( m_GME);

		ASSERT( FAILED(e.hr) );
		if( e.hr == E_XMLPARSER )
			SetErrorInfo(errorinfo);
		else
			SetErrorInfo2(e.hr);

		return e.hr;
	}
	return S_OK;
}

CMgaParser::elementfunc CMgaParser::elementfuncs_bcmga[] = 
{
	//elementfunc(_T("project"), StartProject, EndNone),
	elementfunc(_T("name"), &StartNone, &EndName),
	elementfunc(_T("comment"), &StartNone, &EndComment),
	elementfunc(_T("author"), &StartNone, &EndAuthor),
	elementfunc(_T("value"), &StartNone, &EndValue),
	elementfunc(_T("regnode"), &StartRegNode, &EndNone),
	elementfunc(_T("attribute"), &StartAttribute, &EndNone),
	elementfunc(_T("constraint"), &StartNone, &EndConstraint),
	elementfunc(_T("folder"), &StartFolder, &EndNone),
	elementfunc(_T("model"), &StartModel, &EndNone),
	elementfunc(_T("atom"), &StartAtom, &EndNone),
	elementfunc(_T("connection"), &StartConnection, &EndNone),
	elementfunc(_T("connpoint"), &StartBCConnPoint, &EndNone),	// diff
	elementfunc(_T("reference"), &StartBCReference, &EndNone),	// diff
	elementfunc(_T("set"), &StartBCSet, &EndNone),				// diff
	elementfunc(_T("clipboard"), &StartBCClipboard, &EndNone),	// diff, common for closure clipboards
	elementfunc(_T(""), NULL, NULL)
};

// ------- Element Handlers
void CMgaParser::StartBCClipboard(const attributes_type &attributes)
{
	ASSERT( project != NULL );
	ASSERT( GetPrevName() == _T("start") );
	ASSERT( GetPrevious().object != NULL );

	const CComObjPtr<IUnknown> &obj = GetPrevious().object;
	GetCurrent().object = obj;

	CComObjPtr<IMgaModel> model;
	CComObjPtr<IMgaFolder> folder;

	if( SUCCEEDED(obj.QueryInterface(model)) )
		GetCurrent().name = _T("model");
	else if( SUCCEEDED(obj.QueryInterface(folder)) )
		GetCurrent().name = _T("folder");
	else
		HR_THROW(E_INVALID_FILENAME);

	const std::tstring *ver = GetByNameX(attributes, _T("closureversion"));

	ASSERT( ver);
	ASSERT( ver->compare(m_clVerStr) == 0);

	const std::tstring *acceptingkinds = GetByNameX(attributes, _T("acceptingkind"));
	if( acceptingkinds != NULL)
	{
		CComBstrObj kind_name;
		if( model)
		{
			CComObjPtr<IMgaMetaFCO> k_meta;
			COMTHROW( model->get_Meta( PutOut( k_meta)));
			COMTHROW( k_meta->get_Name( PutOut(kind_name)) );
		}
		else if( folder)
		{
			CComObjPtr<IMgaMetaFolder> f_meta;
			COMTHROW( folder->get_MetaFolder( PutOut( f_meta)));
			COMTHROW( f_meta->get_Name( PutOut(kind_name)) );
		}
			
		// check if the accepting kind exists among the accepting kinds
		std::tstring kind_nm;
		CopyTo( kind_name, kind_nm);

		if( acceptingkinds->empty()) // if _T("") token avoid check
		{
			CComBSTR bstr(L"[Parser] \"clipboard\" element has empty \"acceptingkind\" attribute.");
			if( m_GME) COMTHROW( m_GME->ConsoleMessage(bstr, MSG_WARNING));
		}
		else if( !findExact( *acceptingkinds, kind_nm))
		{
			CComBSTR bstr(L"[Parser] Target kind \"");
			COMTHROW(bstr.Append( kind_nm.c_str()));
			COMTHROW(bstr.Append(L"\" not found among accepting kinds: \""));
			COMTHROW(bstr.Append( acceptingkinds->c_str()));
			COMTHROW(bstr.Append(L"\". If you'd like to avoid this check remove or modify to \"\" the \"acceptingkind\" attribute of \"clipboard\" element in an editor."));
			if( m_GME)
				COMTHROW( m_GME->ConsoleMessage(bstr, MSG_ERROR));
			HR_THROW(E_INVALID_MGA);
		}

	}

}

void CMgaParser::ResolveClosure1Derivation(const attributes_type &attributes, deriv_type &deriv)
{
	const std::tstring *s = GetByNameX(attributes, _T("derivedfrom"));
	if( s == NULL )
	{
		s = GetByNameX(attributes, _T("closurelibderivedfrom"));
		if( s == NULL)
		{
			deriv.from.Release();
			return;
		}

		CComObjPtr<IMgaObject> obj;
            COMTHROW( project->get_ObjectByPath( PutInBstr( *s), PutOut( obj)) );
		if ( obj)
			COMTHROW( obj.QueryInterface( deriv.from));
		else // if nonunique is allowed
		{
			COMTHROW( project->get_NthObjectByPath( 0, PutInBstr( *s), PutOut( obj)) );
			if( obj)
				COMTHROW( obj.QueryInterface( deriv.from));

			//<!> warning about non uniqueness
			if( obj && m_GME) 
			{
				CComBSTR bstr, id, nm;
				COMTHROW( obj->get_ID( &id));
				COMTHROW( obj->get_Name( &nm));
				COMTHROW(bstr.Append(L"[Closure Parser] Name ambiguity found. Selected: <A HREF=\"mga:"));
				COMTHROW(bstr.AppendBSTR( id));
				COMTHROW(bstr.Append(L"\">"));
				COMTHROW(bstr.AppendBSTR( nm));
				COMTHROW(bstr.Append(L"</A> as basetype. Search path used: "));
				COMTHROW(bstr.Append( makeViewable(*s).c_str()));

				COMTHROW( m_GME->ConsoleMessage(bstr, MSG_ERROR));
			}
		}

		if ( deriv.from == NULL) // do not throw exception in case of closurelibderivedfrom
		{
			deriv.from.Release();
			return;
		}
	}
	else
	{
		LookupByID(*s, deriv.from);

		if( deriv.from == NULL )
			throw pass_exception();
	}

	s = GetByNameX(attributes, _T("isinstance"));
	deriv.isinstance = ( s != NULL && *s == _T("yes") ) ? VARIANT_TRUE : VARIANT_FALSE;

	s = GetByNameX(attributes, _T("isprimary"));
	deriv.isprimary = ( s != NULL && *s == _T("no") ) ? false : true;
}


void CMgaParser::StartBCConnPoint(const attributes_type &attributes)
{
	ASSERT( GetPrevName() == _T("connection") );
	CComObjPtr<IMgaConnection> conn;
	GetPrevObj(conn);

	if( GetPrevious().exstrinfo == _T("skip") || GetByName(attributes, _T("isbound")) == _T("yes") ) return;

	CComObjPtr<IMgaFCO> target;

	// prefer the closurelibtarget over the target:
	const std::tstring *libtg = GetByNameX(attributes, _T("closurelibtarget"));
	if ( libtg != NULL)
	{
		CComObjPtr<IMgaObject> obj;
		COMTHROW( project->get_ObjectByPath( PutInBstr( *libtg), PutOut( obj)) );
		if ( obj)
			COMTHROW( obj.QueryInterface( target));
		else // if nonunique is allowed
		{
                  COMTHROW( project->get_NthObjectByPath( 0, PutInBstr( *libtg), PutOut( obj)) );
			if( obj)
				COMTHROW( obj.QueryInterface( target));

			//<!> warning about non uniqueness
			if( obj && m_GME) 
			{
				CComBSTR bstr, id, nm;
				COMTHROW( obj->get_ID( &id));
				COMTHROW( obj->get_Name( &nm));
				COMTHROW(bstr.Append(L"[Closure Parser] Name ambiguity found. Selected: <A HREF=\"mga:"));
				COMTHROW(bstr.AppendBSTR( id));
				COMTHROW(bstr.Append(L"\">"));
				COMTHROW(bstr.AppendBSTR( nm));
				COMTHROW(bstr.Append(L"</A> as connection target. Search path used: "));
				COMTHROW(bstr.Append( makeViewable(*libtg).c_str()));

				COMTHROW( m_GME->ConsoleMessage(bstr, MSG_ERROR));
			}
		}
	}

	const std::tstring *tg = GetByNameX(attributes, _T("target"));
	if( tg != NULL && libtg == NULL) // valid info in target iff no closurelibtarget token
	{
		LookupByID( *tg, target);
	}

	// throw even if closurelibtarget present but not found the needed object
	if ( target == NULL)
		throw pass_exception();

	CComObjPtr<IMgaFCOs> coll;
	COMTHROW(coll.CoCreateInstance(L"Mga.MgaFCOs"));

	const std::tstring *s = GetByNameX(attributes, _T("refs"));
	if( s != NULL )
	{
		size_t pos = s->find_first_not_of(' ', 0);
		ASSERT(pos != -1);
		while (pos < s->length())
		{
			size_t pos2 = s->find_first_of(' ', pos);
			if (pos2 == -1)
				pos2 = s->length();
			ASSERT(pos2 > pos);

			CComObjPtr<IMgaFCO> ref;
			LookupByID(std::tstring(*s, pos, pos2-pos), ref);

			if( ref == NULL )
				throw pass_exception();

			COMTHROW(coll->Append(ref));

			pos = pos2+1;
		}
	}

	CComObjPtr<IMgaConnPoint> connpoint;

	COMTHROW( conn->AddConnPoint(PutInBstr(GetByName(attributes, _T("role"))), 0,	// FIXME: multiplicity
		target, coll, PutOut(connpoint)) );
}

void CMgaParser::StartBCReference(const attributes_type &attributes)
{
	CComObjPtr<IMgaFCO> fco;

	deriv_type deriv;
	(*this.*m_resolveDerFuncPtr)(attributes, deriv);

	CComObjPtr<IMgaFCO> referred;

	const std::tstring *s = GetByNameX(attributes, _T("referred"));
	if( s != NULL )
	{
		LookupByID(*s, referred);

		if( referred == NULL )
			throw pass_exception();
	}
	else // by ZolMol
	{
		s = GetByNameX(attributes, _T("closurelibreferred"));
		if( s != NULL)
		{
			CComObjPtr<IMgaObject> obj;
			COMTHROW( project->get_ObjectByPath( PutInBstr( *s), PutOut( obj)) );
			if ( obj)
				COMTHROW( obj.QueryInterface( referred));
			else // if nonunique is allowed
			{
				COMTHROW( project->get_NthObjectByPath( 0, PutInBstr( *s), PutOut( obj)) );
				if( obj)
					COMTHROW( obj.QueryInterface( referred));

				//<!> warning about non uniqueness
				if( obj && m_GME) 
				{
					CComBSTR bstr, id, nm;
					COMTHROW( obj->get_ID( &id));
					COMTHROW( obj->get_Name( &nm));
					COMTHROW(bstr.Append(L"[Closure Parser] Name ambiguity found. Selected: <A HREF=\"mga:"));
					COMTHROW(bstr.AppendBSTR( id));
					COMTHROW(bstr.Append(L"\">"));
					COMTHROW(bstr.AppendBSTR( nm));
					COMTHROW(bstr.Append(L"</A> as library reference. Search path used: "));
					COMTHROW(bstr.Append( makeViewable(*s).c_str()));

					COMTHROW( m_GME->ConsoleMessage(bstr, MSG_ERROR));
				}
			}
		}
	}

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaFolder> prev;
		GetPrevObj(prev);

		preparerelid(attributes);
		if( deriv.from != NULL )
		{
			COMTHROW( prev->DeriveRootObject(deriv.from, deriv.isinstance, PutOut(fco)) );
		}
		else
		{
			CComObjPtr<IMgaMetaFCO> meta;
			COMTHROW( resolver->get_KindByStr(prev, PutInBstrAttr(attributes, _T("kind")), 
				OBJTYPE_REFERENCE, PutOut(meta)) );
			ASSERT( meta != NULL );

			COMTHROW( prev->CreateRootObject(meta, PutOut(fco)) );
		}
		assignrelid(fco);
	}
	else
	{
		ASSERT( GetPrevName() == _T("model") );
		CComObjPtr<IMgaModel> prev;
		GetPrevObj(prev);

		CComObjPtr<IMgaMetaRole> role;
		COMTHROW( resolver->get_RoleByStr(prev, 
			PutInBstrAttr(attributes, _T("kind")), OBJTYPE_REFERENCE,
			PutInBstrAttr(attributes, _T("role")), NULL, PutOut(role)) );
		ASSERT( role != NULL );

		if( deriv.from != NULL )
		{
			CComObjPtr<IMgaReference> derivedfrom;
			COMTHROW( deriv.from.QueryInterface(derivedfrom) );

			if( deriv.isprimary )
			{
				preparerelid(attributes);
				COMTHROW( prev->DeriveChildObject(derivedfrom, role, deriv.isinstance, PutOut(fco)) );
				assignrelid(fco);
			}
			else
			{
				COMTHROW( prev->get_ChildDerivedFrom(derivedfrom, PutOut(fco)) );
			}
		}
		else
		{
			preparerelid(attributes);
			COMTHROW( prev->CreateChildObject(role, PutOut(fco)) );
			assignrelid(fco);
		}
	}
	ASSERT( fco != NULL );

	if( !(GetByName(attributes, _T("isbound")) == _T("yes")) && referred != NULL )
	{
		CComObjPtr<IMgaReference> ref;
		COMTHROW( fco.QueryInterface(ref) );

		COMTHROW( ref->put_Referred(referred) );
	}

	GetCurrent().object = fco;

	RegisterLookup(attributes, fco);
}

void CMgaParser::StartBCSet(const attributes_type &attributes)
{
	CComObjPtr<IMgaFCO> fco;

	deriv_type deriv;
	(*this.*m_resolveDerFuncPtr)(attributes, deriv);

	std::list< CComObjPtr<IMgaFCO> > members;//slist

	const std::tstring *s = GetByNameX(attributes, _T("members"));
	if( s != NULL )
	{
		size_t pos = s->find_first_not_of(' ', 0);
		ASSERT(pos != -1);
		while (pos < s->length())
		{
			size_t pos2 = s->find_first_of(' ', pos);
			if (pos2 == -1)
				pos2 = s->length();
			ASSERT(pos2 > pos);

			CComObjPtr<IMgaFCO> member;
			LookupByID(std::tstring(*s, pos, pos2-pos), member);

			if( member == NULL )
				throw pass_exception();

			members.push_front(member);

			pos = pos2+1;
		}
	}

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaFolder> prev;
		GetPrevObj(prev);

		preparerelid(attributes);
		if( deriv.from != NULL )
		{
			COMTHROW( prev->DeriveRootObject(deriv.from, deriv.isinstance, PutOut(fco)) );
		}
		else
		{
			CComObjPtr<IMgaMetaFCO> meta;
			COMTHROW( resolver->get_KindByStr(prev, PutInBstrAttr(attributes, _T("kind")), 
				OBJTYPE_SET, PutOut(meta)) );
			ASSERT( meta != NULL );

			COMTHROW( prev->CreateRootObject(meta, PutOut(fco)) );
		}
		assignrelid(fco);
	}
	else
	{
		ASSERT( GetPrevName() == _T("model") );
		CComObjPtr<IMgaModel> prev;
		GetPrevObj(prev);

		CComObjPtr<IMgaMetaRole> role;
		COMTHROW( resolver->get_RoleByStr(prev, 
			PutInBstrAttr(attributes, _T("kind")), OBJTYPE_SET,
			PutInBstrAttr(attributes, _T("role")), NULL, PutOut(role)) );
		ASSERT( role != NULL );

		if( deriv.from != NULL )
		{
			CComObjPtr<IMgaSet> derivedfrom;
			COMTHROW( deriv.from.QueryInterface(derivedfrom) );

			if( deriv.isprimary )
			{
				preparerelid(attributes);
				COMTHROW( prev->DeriveChildObject(derivedfrom, role, deriv.isinstance, PutOut(fco)) );
				assignrelid(fco);
			}
			else
			{
				preparerelid(attributes);
				COMTHROW( prev->get_ChildDerivedFrom(derivedfrom, PutOut(fco)) );
				assignrelid(fco);
			}
		}
		else
		{
			preparerelid(attributes);
			COMTHROW( prev->CreateChildObject(role, PutOut(fco)) );
			assignrelid(fco);
		}
	}
	ASSERT( fco != NULL );

	CComObjPtr<IMgaSet> mgaset;
	COMTHROW( fco.QueryInterface(mgaset) );

	if( !(GetByName(attributes, _T("isbound")) == _T("yes")) ) {
		COMTHROW( mgaset->RemoveAll() ); //by ZolMol: if not bound then the members are different, remove the inherited members
		std::list< CComObjPtr<IMgaFCO> >::iterator i = members.begin();//slist
		while( i != members.end() )
		{
			COMTHROW( mgaset->AddMember(*i) );
			++i;
		}
	}
	GetCurrent().object = fco;

	RegisterLookup(attributes, fco);
}

