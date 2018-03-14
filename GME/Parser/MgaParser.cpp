
#include "stdafx.h"
#include "CommonMath.h"
#include "Parser.h"
#include "MgaParser.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <stdio.h>

#include "../Common/CommonCollection.h"
#include <list>//slist
#include <algorithm>

const TCHAR * magic_exit_str = _T("Analysis done.Quit parsing.");
// --------------------------- CMgaParser

STDMETHODIMP CMgaParser::ParseFCOs(IMgaObject *here, BSTR filename)
{
	return ParseFCOs2(here, filename, NULL);
}

STDMETHODIMP CMgaParser::ParseFCOs2(IMgaObject *here, BSTR filename, ULONGLONG hwndParent_)
{
	CHECK_IN(here);
	m_maintainGuids = false;

	CComObjPtr<IMgaProject> p;
	HRESULT hr = here->get_Project(PutOut(p));
	if (FAILED(hr))
		return hr;
	if ((p->ProjectStatus & 1) == 0)
	{
		clear_GME(m_GME);
		SetErrorInfo(L"Project is not open");

		return E_MGA_ZOMBIE_NOPROJECT;
	}

	try
	{
		HWND hwndParent = (HWND)hwndParent_;
		if (hwndParent != 0)
		{
			COMTHROW( progress.CoCreateInstance(L"Mga.MgaProgressDlg") );
			COMTHROW( progress->SetTitle(_bstr_t(L"Importing XML file...")) );
			COMTHROW( progress->StartProgressDialog(hwndParent) );
		}

		ASSERT( p != NULL );
		COMTHROW( p->get_Preferences(&project_prefs_orig) );
		manual_relid_mode = project_prefs_orig & MGAPREF_MANUAL_RELIDS ? true : false;

		m_GME = get_GME( p); // by zolmol

		COMTHROW( p->Notify(APPEVENT_XML_IMPORT_FCOS_BEGIN));

		COMTHROW( p->CreateTerritory(NULL, PutOut(territory), NULL) );
		COMTHROW( p->BeginTransaction(territory, TRANSACTION_NON_NESTED) );

		// PETER: put the 'here' object into the parser's territory
		CComObjPtr<IMgaObject> target;
		COMTHROW(territory->OpenObj(here, PutOut(target)));

		project_prefs = project_prefs_orig | MGAPREF_IGNORECONNCHECKS;
		COMTHROW( p->put_Preferences(project_prefs) );

		if (resolver == NULL)
		{
			COMTHROW( resolver.CoCreateInstance(L"Mga.MgaResolver") );
			ASSERT( resolver != NULL );
		}

		project = p;

		CopyTo(filename, xmlfile);

		m_resolveDerFuncPtr = &CMgaParser::ResolveDerivation;
		XMLPlatformUtilsTerminate_RAII term;
		try
		{
			SAXParser parser;
			parser.setValidationScheme(SAXParser::Val_Always);
			parser.setDocumentHandler(this);
			parser.setErrorHandler(this);
			parser.setEntityResolver(this);

			// Now it is delegated to fireStartFunction
			//elementfuncs = elementfuncs_mga;
			funcTableState = MGA;

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

		for(librecords::reverse_iterator i = libstodo.rbegin(); i != libstodo.rend(); ++i) { // copied from ParseProject in order to recognize libraries (zolmol)
			COMTHROW(i->f->put_LibraryName(CComBSTR(i->libname.c_str())));
			COMTHROW(i->f->put_Exempt(VARIANT_FALSE));
		}
		libstodo.clear();

		COMTHROW( project->CommitTransaction() );
		COMTHROW( project->Notify(APPEVENT_XML_IMPORT_FCOS_END));
		project = NULL;

		CloseAll();
		clear_GME( m_GME);
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
	catch(_com_error &e)
	{
		CloseAll();
		clear_GME( m_GME);

		ASSERT( FAILED(e.Error()) );
		if (e.Description() != _bstr_t())
			SetErrorInfo(e.Description());

		return e.Error();
	}
	return S_OK;
}



STDMETHODIMP CMgaParser::ParseProject(IMgaProject *p, BSTR filename)
{
	return ParseProject2(p, filename, 0);
}

STDMETHODIMP CMgaParser::ParseProject2(IMgaProject *p, BSTR filename, ULONGLONG hwndParent_)
{
	CHECK_IN(p);
	m_maintainGuids = true; //will be set to false if p is NOT empty

	if ((p->ProjectStatus & 1) == 0)
	{
		clear_GME(m_GME);
		SetErrorInfo(L"Project is not open");

		return E_MGA_ZOMBIE_NOPROJECT;
	}
	try
	{
		HWND hwndParent = (HWND)hwndParent_;
		if (hwndParent != 0)
		{
			COMTHROW( progress.CoCreateInstance(L"Mga.MgaProgressDlg") );
			COMTHROW( progress->SetTitle(_bstr_t(L"Importing XML file...")) );
			COMTHROW( progress->StartProgressDialog(hwndParent) );
		}


		project = p;
		COMTHROW( project->get_Preferences(&project_prefs_orig) );
		manual_relid_mode = project_prefs_orig & MGAPREF_MANUAL_RELIDS ? true : false;

		m_GME = get_GME( p); // by zolmol

		COMTHROW( project->Notify(APPEVENT_XML_IMPORT_BEGIN));

		COMTHROW( project->CreateTerritory(NULL, PutOut(territory), NULL) );
		COMTHROW( project->BeginTransaction(territory, TRANSACTION_NON_NESTED) );

		CComObjPtr<IMgaFolder> rfld;
		COMTHROW( project->get_RootFolder( PutOut(rfld)));
		if( rfld) 
		{
			CComObjPtrVector<IMgaFolder> fols;
			CComObjPtrVector<IMgaFCO> chld;
			COMTHROW( rfld->get_ChildFCOs( PutOut( chld)));
			COMTHROW( rfld->get_ChildFolders( PutOut( fols)));
			
			m_maintainGuids = fols.empty() && chld.empty(); // maintain guids if and only if project is empty
		}
		
		project_prefs = project_prefs_orig | MGAPREF_IGNORECONNCHECKS;
		COMTHROW( project->put_Preferences(project_prefs) );

		if (resolver == NULL)
		{
			COMTHROW( resolver.CoCreateInstance(L"Mga.MgaResolver") );
			ASSERT( resolver != NULL );
		}

		CopyTo(filename, xmlfile);

		m_resolveDerFuncPtr = &CMgaParser::ResolveDerivation;
		XMLPlatformUtilsTerminate_RAII term;
		try
		{
			SAXParser parser;
			parser.setValidationScheme(SAXParser::Val_Always);
			parser.setDocumentHandler(this);
			parser.setErrorHandler(this);
			parser.setEntityResolver(this);

			// Now it is fireStart/End Function
			//elementfuncs = elementfuncs_mga;

			funcTableState = MGA;

			// we do the first pass manually

			pass_count = 1;

			ranges.clear();
			ranges.push_front(range_type());
			ranges.front().begin = 1;
			ranges.front().end = (counter_type)-1;
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

		// readonly flag and library name can't be applied if done in wrong order
		// because of the readonly flag is applied all way down the hierarchy
		// (preventing put_LibraryName for inner libs to succeed)

		// the order of libraries in libstodo is the sequential order as they appear
		// in the xme file (depth first traversal), thus if the vector is traversed
		// in reverse order (children libraries are always handled before their parent)
		// the libroot_flag and library_flag flags can be applied with success
		for(librecords::reverse_iterator i = libstodo.rbegin(); i != libstodo.rend(); ++i) {
			COMTHROW(i->f->put_LibraryName(CComBSTR(i->libname.c_str())));
			COMTHROW(i->f->put_Exempt(VARIANT_FALSE));
		}
		libstodo.clear();
		
		COMTHROW( project->put_GUID(projectguid) );
		if (projectversion != _bstr_t("")) {
			COMTHROW( project->put_Version(projectversion) );
		}

		project->__CommitTransaction();
		project->__Notify(APPEVENT_XML_IMPORT_END);
		project = NULL;

		CloseAll();
		clear_GME( m_GME);
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
	catch(_com_error &e)
	{
		CloseAll();
		clear_GME( m_GME);

		ASSERT( FAILED(e.Error()) );
		if (e.Description() != _bstr_t())
			SetErrorInfo(e.Description());

		return e.Error();
	}
	return S_OK;
}

STDMETHODIMP CMgaParser::GetXMLParadigm(BSTR filename, BSTR *paradigm)
{
	return GetXMLInfo(filename, paradigm, _bstr_t().GetAddress(), _variant_t().GetAddress(), _bstr_t().GetAddress(), _bstr_t().GetAddress());
}

STDMETHODIMP CMgaParser::GetXMLInfo(BSTR filename, BSTR *paradigm, BSTR* parversion, VARIANT *parguid, BSTR* basename, BSTR* version) {

//	CHECK_IN(filename);
	CHECK_OUT(paradigm);
	CHECK_OUT(parversion);
	CHECK_OUT(parguid);
	CHECK_OUT(basename);
	CHECK_OUT(version);

	try
	{
		CopyTo(filename, xmlfile);

		XMLPlatformUtilsTerminate_RAII term;
		try
		{
			SAXParser parser;
			parser.setValidationScheme(SAXParser::Val_Never);
			parser.setDocumentHandler(this);
			parser.setErrorHandler(this);
			parser.setEntityResolver(this);

			//elementfuncs = elementfuncs_mgainfo;
			funcTableState = MGA_INFO;


			pass_count = 1;

			ranges.clear();
			ranges.push_front(range_type());
			ranges.front().begin = 1;
			ranges.front().end = (counter_type)-1;
			skip_element_level = 0;

			infoparname = paradigm;
			infoparversion = parversion;
			infoparguid = parguid;
			infoprojname = basename;
			infoversion = version;

			parser.parse(xmlfile.c_str());

		}
		catch(const SAXException &e)
		{
			XmlStr msg = e.getMessage();
			if (msg != magic_exit_str)
			{
				ThrowXmlError(L"%s", msg.c_str());
			}
			// else just ignore it, we threw an exception for a good purpose:
			// to quit the costly parsing operation
		}
		catch(const XMLException &e)
		{
			XmlStr desc(e.getMessage());

			ThrowXmlError(L"%s", desc.c_str());
		}

		CloseAll();
	}
	catch(hresult_exception &e)
	{
		CloseAll();

		ASSERT( FAILED(e.hr) );
		if( e.hr == E_XMLPARSER )
			SetErrorInfo(errorinfo);
		else
			SetErrorInfo2(e.hr);

		return e.hr;
	}
	return S_OK;
}

void CMgaParser::CloseAll()
{
	elements.clear();
	ranges.clear();

	if( progress != NULL )
	{
		COMTHROW(progress->StopProgressDialog());
		progress = NULL;
	}

	resolver.Release();

	if( project != NULL )
	{

		COMTHROW(project->put_Preferences(project_prefs_orig));
		if (project->ProjectStatus & 8)
		{
			COMTHROW(project->AbortTransaction());
		}
		COMTHROW(project->Notify(APPEVENT_XML_IMPORT_END));
	}

	if( territory != NULL )
		COMTHROW(territory->Destroy());

	territory = NULL;
	project = NULL;

	manual_relid_mode = false;
	relid = -2;
	
	GUID tmpguid;
	memset(&tmpguid, 0, sizeof(GUID));
	CopyTo(tmpguid, projectguid);
};

// ------- Passes

TCHAR progress_msg[512];

void CMgaParser::startElement(const XMLCh* const name, AttributeList& attributes)
{
	ASSERT( !ranges.empty() );

	++counter;

	if( counter % 1000 == 0 )
	{
		_stprintf_s(progress_msg, _T("Phase: %d, number of parsed objects: %ld"), pass_count, (long)counter/2);
		if (progress)
			COMTHROW( progress->SetLine(0, PutInBstr(progress_msg)) );

		if( pass_count == 1 )
			max_counter = counter + 10000;

		if (progress)
			COMTHROW( progress->SetProgress(counter, 0) );
	}

	if( skip_element_level > 0 )
	{
		ASSERT( ranges.front().begin != counter );
		++skip_element_level;
	}
	else if( ranges.front().begin <= counter && counter <= ranges.front().end )
	{
		if( ranges.front().begin == counter )
		{
			ASSERT( elements.empty() );
			elements.push_back(ranges.front().previous);
		}
		ASSERT( !elements.empty() );

		try
		{
			CGenParser::startElement(name, attributes);
		}
		catch(pass_exception &e)
		{
			ASSERT( skip_element_level == 0 );

			ranges.push_back(range_type());
			ranges.back().begin = elements.back().begin;
			elements.pop_back();
			ranges.back().previous = elements.back();
			skip_element_level = 1;
			if (pass_count == 99)
				if (m_GME)
					COMTHROW(m_GME->ConsoleMessage(_bstr_t(e.wwhat()), msgtype_enum::MSG_ERROR));
		}
	}
}

void CMgaParser::endElement(const XMLCh* const name)
{
	ASSERT( !ranges.empty() );

	++counter;

	if( skip_element_level > 0 )
	{
		--skip_element_level;

		if( skip_element_level == 0 )
		{
			ranges.back().end = counter;
		}
	}
	else if( ranges.front().begin <= counter && counter <= ranges.front().end )
	{
		try
		{
			CGenParser::endElement(name);
		}
		catch(pass_exception &)
		{
			ASSERT( skip_element_level == 0 );

			ranges.push_back(range_type());
			ranges.back().begin = elements.back().begin;
			ranges.back().end = elements.back().end;
			elements.pop_back();
			ranges.back().previous = elements.back();
		}
	}

	if( ranges.front().end == counter )
	{
		ASSERT( elements.size() == 1 );

		elements.clear();
		ranges.pop_front();

		// no more ranges, skip everything
		if( ranges.empty() )
		{
			ranges.push_back(range_type());
			ranges.back().begin = (counter_type)-1;
			ranges.back().end = (counter_type)-1;
		}
	}
}

// ------- Lookup

void CMgaParser::LookupByID(const std::tstring &id, CComObjPtr<IMgaObject> &ret)
{
	ASSERT( project != NULL );

	ret.Release();

	id_lookup_iterator i = id_lookup.find(id);
	if( i != id_lookup.end() )
	{
		HRESULT hr = project->GetObjectByID((*i).second, PutOut(ret));
		if (hr != E_MGA_BAD_ID)
			// FIXME: test and enable
			; // COMTHROW(hr);
	}
}

void CMgaParser::LookupByID(const std::tstring &id, CComObjPtr<IMgaFCO> &ret)
{
	ASSERT( project != NULL );

	ret.Release();

	id_lookup_iterator i = id_lookup.find(id);
	if( i != id_lookup.end() )
	{
		HRESULT hr = project->GetFCOByID((*i).second, PutOut(ret));
		if (hr != E_MGA_BAD_ID)
			// FIXME: test and enable
			; // COMTHROW(hr);
	}
}

bool CMgaParser::RegisterLookup(const std::tstring &id, IMgaObject *object)
{
	ASSERT( object != NULL );

	CComBstrObj &mgaid = id_lookup[id];
	ASSERT( mgaid.p == NULL );
	if (mgaid.p != NULL)
	{
		return false;
	}

	COMTHROW( object->get_ID(PutOut(mgaid)) );
	return true;
}

void CMgaParser::RegisterLookup(const attributes_type &attributes, IMgaObject *object)
{
	bool perm_present = false; // currently the perm attribute appears only if the object is readonly
	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		if( (*i).first == _T("id") )
		{
			if (RegisterLookup((*i).second, object) == false)
			{
				std::wstring err = L"Duplicate id '" + (*i).second + L"'";
				throw_com_error(E_MGA_INVALID_ARG, err.c_str());
			}

		}
		else if( (*i).first == _T("guid") )
		{
			bool dup = !RegisterLookup((*i).second, object);
			// KMS: due to a bug in MgaFolder::CopyFCOs (ObjTreeCopyFoldersToo), some mga files (and thus xme files) may have duplicate GUIDs
			// If we have a Mga.dtd-type file, re-assign the dup GUIDs (==don't PutGuidDisp the parsed GUID)
			// If we have a Mga2.dtd-type file, this is a hard error (consider: what if a connection, reference, or set is connected to a dup GUID?)
			if (dup)
			{
				// "id" not in attributes => Mga2.dtd-type file
				auto old_id = std::find_if(attributes.begin(), attributes.end(), [](const attributes_type::value_type& val) -> bool { return val.first == L"id"; });
				if (old_id == attributes.end())
				{
					std::wstring err = L"Duplicate id '" + (*i).second + L"'";
					throw_com_error(E_MGA_INVALID_ARG, err.c_str());
				}
			}
			// n.b. dup == true means (*i).second is not registered in lookup. This is ok, since we have a Mga.dtd-type file
			if (m_maintainGuids && !dup)
			{
				// when fco was created already got a fresh GUID, but we need to maintain
				// the old guid, thus we overwrite the old value with the parsed one
				COMTHROW(object->PutGuidDisp( CComBSTR( (*i).second.c_str())));
			}
		}
		else if( (*i).first == _T("perm") )
		{
			// the plain presence of the attribute indicates 'ro' flag
			// no need to parse the value: (*i).second
			perm_present = true;
		}

		++i;
	}
	
	readonly_stack.push_back( perm_present); // we insert a value anyway into the stack
}

template<typename F>
static void fire(const std::tstring& namestr, CMgaParser::elementfunc elementfuncs[], F& f)
{
	// TODO: replace with binary search or perfect hash
	for(unsigned int index = 0; !elementfuncs[index].name.empty(); index++)
	{
		if( namestr == elementfuncs[index].name )
		{
			f(elementfuncs[index]);
			break;
		}
	}
}

static void fireStart(CMgaParser* that, const std::tstring& namestr, const CMgaParser::attributes_type& attributes,
	CMgaParser::elementfunc elementfuncs[])
{
	fire(namestr, elementfuncs, [&](CMgaParser::elementfunc& f) { f.Start(that, attributes); });
}

static void fireEnd(CMgaParser* that, const std::tstring& namestr, CMgaParser::elementfunc elementfuncs[])
{
	fire(namestr, elementfuncs, [&](CMgaParser::elementfunc& f) { f.End(that); });
}

void CMgaParser::fireStartFunction(const std::tstring& namestr, const attributes_type& attributes)
{
	switch (funcTableState) {
	case MGA:
		fireStart(this, namestr, attributes, elementfuncs_mga);
		break;
	case MGA_INFO:
		fireStart(this, namestr, attributes, elementfuncs_mgainfo);
		break;
	case BC_MGA:
		fireStart(this, namestr, attributes, elementfuncs_bcmga);
		break;
	case SC_MGA:
		fireStart(this, namestr, attributes, elementfuncs_scmga);
		break;
	default: /* CLIP_MGA_INFO */
		fireStart(this, namestr, attributes, elementfuncs_clipmgainfo);
	}
}

void CMgaParser::fireEndFunction(const std::tstring& namestr)
{
	switch (funcTableState) {
	case MGA:
		fireEnd(this, namestr, elementfuncs_mga);
		break;
	case MGA_INFO:
		fireEnd(this, namestr, elementfuncs_mgainfo);
		break;
	case BC_MGA:
		fireEnd(this, namestr, elementfuncs_bcmga);
		break;
	case SC_MGA:
		fireEnd(this, namestr, elementfuncs_scmga);
		break;
	default: /* CLIP_MGA_INFO */
		fireEnd(this, namestr, elementfuncs_clipmgainfo);
	}
}

CMgaParser::elementfunc CMgaParser::elementfuncs_mga[] = 
{
	elementfunc(_T("project"), &StartProject, &EndNone),
	elementfunc(_T("name"), &StartNone, &EndName),
	elementfunc(_T("comment"), &StartNone, &EndComment),
	elementfunc(_T("author"), &StartNone, &EndAuthor),
	elementfunc(_T("value"), &StartNone, &EndValue),
	elementfunc(_T("folder"), &StartFolder, &EndObject),
	elementfunc(_T("model"), &StartModel, &EndObject),
	elementfunc(_T("atom"), &StartAtom, &EndObject),
	elementfunc(_T("regnode"), &StartRegNode, &EndNone),
	elementfunc(_T("attribute"), &StartAttribute, &EndNone),
	elementfunc(_T("connection"), &StartConnection, &EndObject),
	elementfunc(_T("connpoint"), &StartConnPoint, &EndNone),
	elementfunc(_T("constraint"), &StartNone, &EndConstraint),
	elementfunc(_T("reference"), &StartReference, &EndObject),
	elementfunc(_T("set"), &StartSet, &EndObject),
	elementfunc(_T("clipboard"), &StartClipboard, &EndNone),
	elementfunc(_T(""), NULL, NULL)
};

// ------- Element Handlers

void CMgaParser::StartProject(const attributes_type &attributes)
{
	ASSERT( project != NULL );

	GetCurrent().object = project;

	// get metaproject
	CComObjPtr<IMgaMetaProject> metaproject;
	if( project) COMTHROW( project->get_RootMeta(PutOut(metaproject)) );
	ASSERT( metaproject != NULL );
	
	// obtain the current paradigm name
	CComBSTR host_pn;
	if( metaproject) COMTHROW( metaproject->get_Name( &host_pn));

	// is the resolver interactive?
	VARIANT_BOOL int_mode = VARIANT_FALSE;
	COMTHROW( resolver->get_IsInteractive( &int_mode));

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		if( i->first == _T("guid") )
		{
			_bstr_t bstr = i->second.c_str();

			GUID guid;
			CopyTo(bstr, guid);

			CopyTo(guid, projectguid);
		}
		else if( i->first == _T("version") )
		{	
			CComBSTR currversion;
			COMTHROW( project->get_Version(&currversion) );
			if (currversion.Length() == 0) {
				projectversion = i->second.c_str();
			}
		}
		else if( i->first == _T("metaname"))
		{
			// if host paradigm != imported project's paradigm
			if( host_pn != CComBSTR( i->second.c_str()))
			{
				if( int_mode == VARIANT_TRUE) // if interactive
				{
					COMTHROW(resolver->getUserOptions());
				}
			}
		}

		++i;
	}
}

void CMgaParser::StartClipboard(const attributes_type &attributes)
{
	ASSERT( project != NULL );
	ASSERT( GetPrevName() == _T("start") );
	ASSERT( GetPrevious().object != NULL );
	if( !GetPrevious().object) HR_THROW(E_XMLPARSER);//by ZolMol

	const CComObjPtr<IUnknown> &obj = GetPrevious().object;
	GetCurrent().object = obj;

	// get metaproject
	CComObjPtr<IMgaMetaProject> metaproject;
	if( project) COMTHROW( project->get_RootMeta(PutOut(metaproject)) );
	ASSERT( metaproject != NULL );
	
	// obtain the current paradigm name
	CComBSTR host_pn;
	if( metaproject) COMTHROW( metaproject->get_Name( &host_pn));

	// is the resolver interactive?
	VARIANT_BOOL int_mode = VARIANT_FALSE;
	COMTHROW( resolver->get_IsInteractive( &int_mode));

	CComObjPtr<IMgaModel> model;
	CComObjPtr<IMgaFolder> folder;

	if( SUCCEEDED(obj.QueryInterface(model)) )
		GetCurrent().name = _T("model");
	else if( SUCCEEDED(obj.QueryInterface(folder)) )
		GetCurrent().name = _T("folder");
	// Commented, thus allowing clipboard snippets to be 
	// dropped onto atoms/sets/references/connections too
	//else
	//	HR_THROW(E_INVALID_FILENAME);

	const std::tstring *parname_hint = GetByNameX(attributes, _T("paradigmnamehint"));
	// importing from different paradigm
	if( parname_hint != NULL && host_pn != CComBSTR( parname_hint->c_str()))
	{
		if( int_mode == VARIANT_TRUE)
		{
			COMTHROW(resolver->getUserOptions());
		}
	}
}

void CMgaParser::EndName()
{
	if( GetPrevName() == _T("project") )
	{
            COMTHROW( project->put_Name(PutInBstr(GetCurrData())) );
	}
	else if( GetPrevName() == _T("constraint") )
	{
		constraint_name = GetCurrData();
	}
	else if( GetPrevious().object != NULL ) 
	{
		CComObjPtr<IMgaObject> prev;
		GetPrevObj(prev);
		COMTHROW( prev->put_Name(PutInBstr(GetCurrData())) );
	}
}

void CMgaParser::EndComment()
{
	if( GetPrevName() == _T("project") )
	{
		COMTHROW( project->put_Comment(PutInBstr(GetCurrData())) );
	}
	else
		HR_THROW(E_INVALID_DTD);
}

void CMgaParser::EndAuthor()
{
	if( GetPrevName() == _T("project") )
	{
            COMTHROW( project->put_Author(PutInBstr(GetCurrData())) );
	}
	else
		HR_THROW(E_INVALID_DTD);
}

void CMgaParser::EndValue()
{
	// skip if the object is ignored
	if( GetPrevious().object == NULL )
		return;

	if( GetPrevName() == _T("constraint") )
	{
		constraint_value = GetCurrData();
	}
	else if( GetPrevName() == _T("regnode") )
	{
		CComObjPtr<IMgaRegNode> regnode;
		GetPrevObj(regnode);

		long status;
		COMTHROW( regnode->get_Status(&status) );

		// when we create the registry node, 
		// we fill it by an empty string if not inherited or not undefined
		if( status == 0 )
			COMTHROW( regnode->put_Value(PutInBstr(GetCurrData())) );
	}
	else if( GetPrevName() == _T("attribute") ) 
	{
		CComObjPtr<IMgaAttribute> attr;
		GetPrevObj(attr);

		long status;
		COMTHROW( attr->get_Status(&status) );

		// if inherited, then do not modify the value
		if( status != 0 )
			return;

		CComObjPtr<IMgaMetaAttribute> metaattr;
		COMTHROW( attr->get_Meta(PutOut(metaattr)) );
		ASSERT( metaattr != NULL );

		attval_enum attval;
		COMTHROW( metaattr->get_ValueType(&attval) );

		CComVariant v;

		switch( attval )
		{
		case ATTVAL_STRING:
		case ATTVAL_ENUM:
		case ATTVAL_DYNAMIC:
			v = GetCurrData().c_str();
			break;

		case ATTVAL_INTEGER:
			v = GetCurrData().c_str();
			COMTHROW( v.ChangeType(VT_I4) );
			break;

		case ATTVAL_DOUBLE:
			{
			const wchar_t* val = GetCurrData().c_str();
			double special;
			if (ParseSpecialDouble(val, special))
			{
				v = special;
			}
			else
			{
				double doubleVal;
				_swscanf_s_l(val, L"%lg", c_locale, &doubleVal);
				v = doubleVal;
			}
			}
			break;

		case ATTVAL_BOOLEAN:
			{
				char c = tolower(*GetCurrData().c_str());
				CopyTo( (c == 't' || c == '1' || c == 'y') ? VARIANT_TRUE : VARIANT_FALSE, v);
				break;
			}

		case ATTVAL_REFERENCE:
			{
				CComObjPtr<IMgaFCO> object;
				LookupByID(GetCurrData(), object);

				if( object == NULL )
					throw pass_exception(std::wstring(L"Referenced FCO ") + GetCurrData() + L" not found");

				CopyTo(object, v);
			}

		default:
			HR_THROW(E_INVALID_META);

		};

		COMTHROW( attr->put_Value(v) );
	}
}




void CMgaParser::preparerelid(const attributes_type &attributes) {
	const std::tstring & relidattr = GetByName(attributes, _T("relid"));
	ASSERT(relid == -2);
	if(relidattr.size()) {
		relid = _tcstol(relidattr.c_str(), NULL, 0);
		if(!manual_relid_mode) {
			COMTHROW(project->put_Preferences(project_prefs | MGAPREF_MANUAL_RELIDS));
		}
		ASSERT(relid >= 0 && 
			   relid <= RELID_BASE_MAX && 
			   GetPrevious().exnuminfo >= relid);
	}	
	else {
		if(!manual_relid_mode) {
			COMTHROW(project->put_Preferences(project_prefs & ~MGAPREF_MANUAL_RELIDS));
		}
		relid = -1;
	}
}


void CMgaParser::assignrelid(IMgaObject *obj) {
	ASSERT(relid != -2);
	if(relid > 0) {
		COMTHROW(obj->put_RelID(relid));
	}
	relid = -2;
}

void CMgaParser::StartFolder(const attributes_type &attributes)
{
	CComObjPtr<IMgaFolder> folder;

	if( GetPrevName() == _T("project") )
	{
		COMTHROW( project->get_RootFolder(PutOut(folder)) );
	}
	else
	{
		CComObjPtr<IMgaFolder> prev;
		GetPrevObj(prev);

		CComObjPtr<IMgaMetaFolder> meta;
		_bstr_t fname(PutInBstrAttr(attributes, _T("kind")));
        const std::tstring &libn = GetByName(attributes, _T("libref"));

		preparerelid(attributes);

		VARIANT_BOOL previactmode;
		COMTHROW(resolver->get_IsInteractive(&previactmode));
		COMTHROW(resolver->put_IsInteractive(VARIANT_FALSE));
		HRESULT hr = resolver->get_FolderByStr(prev, fname, PutOut(meta));
		COMTHROW(resolver->put_IsInteractive(previactmode));

		bool cheat = false;
		if(FAILED(hr) && libn.size()) {
			CComPtr<IMgaMetaProject> mp;  // it is a library, maybe a rootfolder
			COMTHROW(project->get_RootMeta(&mp));
			COMTHROW(mp->get_RootFolder(PutOut(meta)));
			CComBSTR nn;
			COMTHROW(meta->get_Name(&nn));
			if(wcscmp(nn, fname)) {
				COMTHROW(hr);
			}
			COMTHROW(project->CheckSupress(VARIANT_TRUE));
			cheat = true;
		} else if (FAILED(hr)) {
			_bstr_t error;
			if (GetErrorInfo(error.GetAddress()))
			{
				throw_com_error(hr, error);
			}
			COMTHROW(hr);
		}
		ASSERT(meta);
	
		hr = prev->CreateFolder(meta, PutOut(folder));
		if(cheat) {
			if(folder) COMTHROW(folder->put_Exempt(VARIANT_TRUE));
			COMTHROW(project->CheckSupress(VARIANT_FALSE));
		}
		COMTHROW(hr);
		assignrelid(folder);
		if(libn.size()) {
			librecord lr;
			lr.f = folder;
			lr.libname = libn;
			libstodo.push_back(lr);
		}
	}
	ASSERT( folder != NULL );
	GetCurrent().object = folder;

	long crid = toLong(GetByName(attributes,_T("childrelidcntr")));
	GetCurrent().exnuminfo = crid;
	COMTHROW(folder->put_ChildRelIDCounter(crid));

	//RegisterReadOnlyStatus( attributes);
	RegisterLookup(attributes, folder);
}

void CMgaParser::ResolveDerivation(const attributes_type &attributes, deriv_type &deriv)
{
	const std::tstring *s = GetByNameX(attributes, _T("derivedfrom"));
	if( s == NULL )
	{
		deriv.from.Release();
		return;
	}

	LookupByID(*s, deriv.from);
	if( deriv.from == NULL )
	{
		const std::tstring* name = GetByNameX(attributes, _T("id"));
		if (name == NULL)
		{
			name = GetByNameX(attributes, _T("guid"));
		}
		throw pass_exception(std::wstring(L"Subtype/instance ") + *name + L" cannot find archetype " + *s);
	}

	s = GetByNameX(attributes, _T("isinstance"));
	deriv.isinstance = ( s != NULL && *s == _T("yes") ) ? VARIANT_TRUE : VARIANT_FALSE;

	s = GetByNameX(attributes, _T("isprimary"));
	deriv.isprimary = ( s != NULL && *s == _T("no") ) ? false : true;
}

void CMgaParser::StartModel(const attributes_type &attributes)
{
	CComObjPtr<IMgaFCO> model;

	deriv_type deriv;
	//ResolveDerivation(attributes, deriv);
	(*this.*m_resolveDerFuncPtr)(attributes, deriv);


	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaFolder> prev;
		GetPrevObj(prev);

		preparerelid(attributes);
		if( deriv.from != NULL )
		{
			COMTHROW( prev->DeriveRootObject(deriv.from, deriv.isinstance, PutOut(model)) );
		}
		else
		{
			CComObjPtr<IMgaMetaFCO> meta;
			meta = resolver->KindByStr[prev, PutInBstrAttr(attributes, _T("kind")), 
				OBJTYPE_MODEL];
			ASSERT( meta != NULL );

			COMTHROW( prev->CreateRootObject(meta, PutOut(model)) );
		}
		assignrelid(model);
	}
	else
	{
		ASSERT( GetPrevName() == _T("model") );

		CComObjPtr<IMgaModel> prev;
		GetPrevObj(prev);

		CComObjPtr<IMgaMetaRole> role;
		role = resolver->RoleByStr[prev.p,
			PutInBstrAttr(attributes, _T("kind")), OBJTYPE_MODEL,
			PutInBstrAttr(attributes, _T("role")), _bstr_t()];
		ASSERT( role != NULL );

		if( deriv.from != NULL )
		{
			CComObjPtr<IMgaModel> derivedfrom;
			COMTHROW( deriv.from.QueryInterface(derivedfrom) );

			if( deriv.isprimary )
			{
				preparerelid(attributes);
				COMTHROW( prev->DeriveChildObject(derivedfrom, role, deriv.isinstance, PutOut(model)) );
				assignrelid(model);
			}
			else
			{
				COMTHROW( prev->get_ChildDerivedFrom(derivedfrom, PutOut(model)) );
				// models cannot throw pass_exception (except for subtype inheritance), so this isn't needed
				// fco->RelID = (~RELID_BASE_MAX &fco->RelID) | (RELID_BASE_MAX & derivedfrom->RelID);
			}
		}
		else
		{
			preparerelid(attributes);
			COMTHROW( prev->CreateChildObject(role, PutOut(model)) );
			assignrelid(model);
		}
	}
	ASSERT( model != NULL );

	GetCurrent().object = model;
	long crid = toLong(GetByName(attributes,_T("childrelidcntr")));
	GetCurrent().exnuminfo = crid;
	COMTHROW(CComQIPtr<IMgaModel>(model)->put_ChildRelIDCounter(crid));

	//RegisterReadOnlyStatus( attributes);
	RegisterLookup(attributes, model);
}

void CMgaParser::StartAtom(const attributes_type &attributes)
{
	CComObjPtr<IMgaFCO> atom;

	deriv_type deriv;
	//ResolveDerivation(attributes, deriv);
	(*this.*m_resolveDerFuncPtr)(attributes, deriv);


	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaFolder> prev;
		GetPrevObj(prev);

		if( deriv.from != NULL )
		{
			COMTHROW( prev->DeriveRootObject(deriv.from, deriv.isinstance, PutOut(atom)) );
		}
		else
		{
			CComObjPtr<IMgaMetaFCO> meta;
			meta = resolver->KindByStr[prev, PutInBstrAttr(attributes, _T("kind")), OBJTYPE_ATOM];
			ASSERT( meta != NULL );

			preparerelid(attributes);
			COMTHROW( prev->CreateRootObject(meta, PutOut(atom)) );
			assignrelid(atom);
		}
	}
	else
	{
		ASSERT( GetPrevName() == _T("model") );

		CComObjPtr<IMgaModel> prev;
		GetPrevObj(prev);

		CComObjPtr<IMgaMetaRole> role;
		role = resolver->RoleByStr[prev, 
			PutInBstrAttr(attributes, _T("kind")), OBJTYPE_ATOM,
			PutInBstrAttr(attributes, _T("role")), _bstr_t()];
		ASSERT( role != NULL );

		if( deriv.from != NULL )
		{
			CComObjPtr<IMgaAtom> derivedfrom;
			COMTHROW( deriv.from.QueryInterface(derivedfrom) );

			if( deriv.isprimary )
			{
				preparerelid(attributes);
				COMTHROW( prev->DeriveChildObject(derivedfrom, role, deriv.isinstance, PutOut(atom)) );
				assignrelid(atom);
			}
			else
			{
				COMTHROW( prev->get_ChildDerivedFrom(derivedfrom, PutOut(atom)) );
				// models and atoms can't throw pass_exception (except for subtype inheritance), so this isn't needed
				// fco->RelID = (~RELID_BASE_MAX &fco->RelID) | (RELID_BASE_MAX & derivedfrom->RelID);
			}
		}
		else
		{
			preparerelid(attributes);
			COMTHROW( prev->CreateChildObject(role, PutOut(atom)) );
			assignrelid(atom);
		}
	}

	ASSERT( atom != NULL );
	GetCurrent().object = atom;

	//RegisterReadOnlyStatus( attributes);
	RegisterLookup(attributes, atom);
}

void CMgaParser::StartRegNode(const attributes_type &attributes)
{
	CComObjPtr<IMgaRegNode> regnode;

	_bstr_t name = GetByName(attributes, _T("name")).c_str();

	if( GetPrevName() == _T("regnode") )
	{
		CComObjPtr<IMgaRegNode> prev;
		GetPrevObj(prev);

		COMTHROW( prev->get_SubNodeByName(name, PutOut(regnode)) );
	}
	else if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaFolder> prev;
		GetPrevObj(prev);

		COMTHROW( prev->get_RegistryNode(name, PutOut(regnode)) );
	}
	else if( GetPrevName()== _T("attribute") )
	{
		CComObjPtr<IMgaAttribute> prev;
		GetPrevObj(prev);

		COMTHROW( prev->get_RegistryNode(name, PutOut(regnode)) );
	}
	else
	{
		CComObjPtr<IMgaFCO> prev;
		GetPrevObj(prev);

		COMTHROW( prev->get_RegistryNode(name, PutOut(regnode)) );
	}
	ASSERT( regnode != NULL );

	if( GetByNameX(attributes, _T("status")) == NULL )
		COMTHROW( regnode->put_Value(NULL) );

	if( GetByName(attributes, _T("isopaque")) == _T("yes") )
		COMTHROW( regnode->put_Opacity(VARIANT_TRUE) );

	GetCurrent().object = regnode;
}

void CMgaParser::StartAttribute(const attributes_type &attributes)
{
	CComObjPtr<IMgaAttribute> attr;

	CComObjPtr<IMgaFCO> fco;
	GetPrevObj(fco);

	CComObjPtr<IMgaMetaAttribute> metaattr;
	HRESULT hr;
	const std::tstring* status = GetByNameX(attributes, _T("status"));
	if (status && *status == L"meta")
		;
	else
		hr = resolver->get_AttrByStr(fco, PutInBstrAttr(attributes, _T("kind")), PutOut(metaattr));

	if (metaattr == NULL || FAILED(hr))
	{
		GetCurrent().object = NULL;
		return;
	}

	COMTHROW( fco->get_Attribute(metaattr, PutOut(attr)) );

	ASSERT( attr != NULL );
	GetCurrent().object = attr;

	if(status == NULL )
	{
		// we set some value, and from the _T("value") element we set the actual value

		CComVariant v;
		COMTHROW( attr->get_Value(PutOut(v)) );
		COMTHROW( attr->put_Value(v) );
	}
}

void CMgaParser::StartConnection(const attributes_type &attributes)
{
	CComObjPtr<IMgaFCO> conn;

	deriv_type deriv;
	//ResolveDerivation(attributes, deriv);
	(*this.*m_resolveDerFuncPtr)(attributes, deriv);

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaFolder> prev;
		GetPrevObj(prev);

		preparerelid(attributes);
		if( deriv.from != NULL )
		{
			COMTHROW( prev->DeriveRootObject(deriv.from, deriv.isinstance, PutOut(conn)) );
		}
		else
		{
			CComObjPtr<IMgaMetaFCO> meta;
			meta = resolver->KindByStr[prev, PutInBstrAttr(attributes, _T("kind")), OBJTYPE_CONNECTION];
			ASSERT( meta != NULL );

			COMTHROW( prev->CreateRootObject(meta, PutOut(conn)) );
		}
		assignrelid(conn);
	}
	else
	{
		ASSERT( GetPrevName() == _T("model") );
		CComObjPtr<IMgaModel> prev;
		GetPrevObj(prev);

		CComObjPtr<IMgaMetaRole> role;
		role = resolver->RoleByStr[prev, 
			PutInBstrAttr(attributes, _T("kind")), OBJTYPE_CONNECTION,
			PutInBstrAttr(attributes, _T("role")), _bstr_t()];
		ASSERT( role != NULL );

		if( deriv.from != NULL )
		{
			CComObjPtr<IMgaConnection> derivedfrom;
			COMTHROW( deriv.from.QueryInterface(derivedfrom) );

			if( deriv.isprimary )
			{
				preparerelid(attributes);
				COMTHROW( prev->DeriveChildObject(derivedfrom, role, deriv.isinstance, PutOut(conn)) );
				assignrelid(conn);
			}
			else
			{
				COMTHROW( prev->get_ChildDerivedFrom(derivedfrom, PutOut(conn)) );
				// If conn's parent (or primary derived ancestor) was being parsed before derivedfrom,
				// then derivedfrom won't have had the correct RelID when Derive{Root,Child}Object was called.
				// This can happen when parsing derivedfrom throws pass_exception.
				// Set it now, retaining the high bits that indicate non-primary derivation.
				// (We know derivedfrom is parsed at this point, since ResolveDerivation would throw otherwise)
				conn->RelID = (~RELID_BASE_MAX &conn->RelID) | (RELID_BASE_MAX & derivedfrom->RelID);
			}
		}
		else
		{
			preparerelid(attributes);
			COMTHROW( prev->CreateChildObject(role, PutOut(conn)) );
			assignrelid(conn);
		}
	}
	ASSERT( conn != NULL );
	GetCurrent().object = conn;

	if( GetByName(attributes, _T("isbound")) == _T("yes") ) GetCurrent().exstrinfo = _T("skip");

	//RegisterReadOnlyStatus( attributes);
	RegisterLookup(attributes, conn);
}

void CMgaParser::StartConnPoint(const attributes_type &attributes)
{
	ASSERT( GetPrevName() == _T("connection") );
	CComObjPtr<IMgaConnection> conn;
	GetPrevObj(conn);

	if( GetPrevious().exstrinfo == _T("skip") || GetByName(attributes, _T("isbound")) == _T("yes") ) return;

	CComObjPtr<IMgaFCO> target;
	LookupByID(GetByName(attributes, _T("target")), target);
	if( target == NULL )
		throw pass_exception(std::wstring(L"Connection end ") + GetByName(attributes, _T("target")) + L" not found");


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

	const std::tstring& role = GetByName(attributes, _T("role"));
	_bstr_t brole = role.c_str();

	// Since the Refresh operation created some derived connections
	// in its own way, some derived connections might have been
	// dumped with connroles, so that when the import happened
	// the number of connroles went up to 4 or 6 or ...
	// So to mitigate this issue the parser has been enhanced
	// to eliminate superfuous connroles in case of derived conns
	CComPtr<IMgaConnPoints> connpoints;
	COMTHROW( conn->get_ConnPoints( &connpoints));
	long c = 0;
	if( connpoints) COMTHROW( connpoints->get_Count( &c));
	for( long i = 1; i <= c; ++i)
	{
		CComPtr<IMgaConnPoint> old_cp;
		COMTHROW( connpoints->get_Item( i, &old_cp));
		if( old_cp)
		{
			CComBSTR old_role;
			COMTHROW( old_cp->get_ConnRole( &old_role));
			if( old_role == static_cast<LPCOLESTR>(brole)) // check if targets are equal
			{
				CComPtr<IMgaFCO> old_tgt;
				COMTHROW( old_cp->get_Target( &old_tgt));
				if( old_tgt == target)
				{
					// same role not inserted twice
					CComBSTR msg( L"Warning: Superfluous connection role ignored!");
					if( m_GME) COMTHROW( m_GME->ConsoleMessage( msg, MSG_WARNING));
					return;
				}
			}
		}
	}

	COMTHROW( conn->AddConnPoint(PutInBstr(GetByName(attributes, _T("role"))), 0,	// FIXME: multiplicity
		target, coll, PutOut(connpoint)) );
}

void CMgaParser::EndConstraint()
{
	CComObjPtr<IMgaConstraint> constraint;

	if( GetPrevName() == _T("folder") )
	{
		CComObjPtr<IMgaFolder> folder;
		GetPrevObj(folder);

		COMTHROW( folder->DefineConstraint(PutInBstr(constraint_name), 0, // FIXME: the mask
			PutInBstr(constraint_value), PutOut(constraint)) );
	}
	else
	{
		CComObjPtr<IMgaFCO> fco;
		GetPrevObj(fco);

		COMTHROW( fco->DefineConstraint(PutInBstr(constraint_name), 0, // FIXME: the mask
			PutInBstr(constraint_value), PutOut(constraint)) );
	}
}

void CMgaParser::StartReference(const attributes_type &attributes)
{
	CComObjPtr<IMgaFCO> fco;

	deriv_type deriv;
	//ResolveDerivation(attributes, deriv);
	(*this.*m_resolveDerFuncPtr)(attributes, deriv);

	CComObjPtr<IMgaFCO> referred;

	const std::tstring *s = GetByNameX(attributes, _T("referred"));
	if( s != NULL )
	{
		LookupByID(*s, referred);

		if( referred == NULL )
			throw pass_exception(std::wstring(L"Referenced FCO ") + GetCurrData() + L" not found");
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
			meta = resolver->KindByStr[prev, PutInBstrAttr(attributes, _T("kind")), OBJTYPE_REFERENCE];
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
		role = resolver->RoleByStr[prev, 
			PutInBstrAttr(attributes, _T("kind")), OBJTYPE_REFERENCE,
			PutInBstrAttr(attributes, _T("role")), _bstr_t()];
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
				fco->RelID = (~RELID_BASE_MAX &fco->RelID) | (RELID_BASE_MAX & derivedfrom->RelID);
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

		// Derived References may have incorrect refport connections from initial creation (DeriveRootObject)
		if (GetByNameX(attributes, _T("derivedfrom")) != nullptr) {
			auto connPoints = ref->UsedByConns;
			for (int i = 1; i <= connPoints->Count; i++) {
				auto connPoint = connPoints->GetItem(i);
				// TODO?? if (connPoint->References->GetItem(1) == fco
				connPoint->Remove(); // these will be set when <connpoint> is parsed
			}
		}

		COMTHROW( ref->put_Referred(referred) );
	}

	GetCurrent().object = fco;

	//RegisterReadOnlyStatus( attributes);
	RegisterLookup(attributes, fco);
}

void CMgaParser::StartSet(const attributes_type &attributes)
{
	CComObjPtr<IMgaFCO> fco;

	deriv_type deriv;
	//ResolveDerivation(attributes, deriv);
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
				throw pass_exception(std::wstring(L"Set member ") + std::tstring(*s, pos, pos2-pos) + L" not found");

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
			meta = resolver->KindByStr[prev, PutInBstrAttr(attributes, _T("kind")), OBJTYPE_SET];
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
		role = resolver->RoleByStr[prev, 
			PutInBstrAttr(attributes, _T("kind")), OBJTYPE_SET,
			PutInBstrAttr(attributes, _T("role")), _bstr_t()];
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
				COMTHROW( prev->get_ChildDerivedFrom(derivedfrom, PutOut(fco)) );
				fco->RelID = (~RELID_BASE_MAX &fco->RelID) | (RELID_BASE_MAX & derivedfrom->RelID);
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

	//RegisterReadOnlyStatus( attributes);
	RegisterLookup(attributes, fco);
}

// ------- ElementFuncs -- info

CMgaParser::elementfunc CMgaParser::elementfuncs_mgainfo[] = 
{
	elementfunc(_T("project"), &StartProjectInfo, &EndNone),
	elementfunc(_T("name"), &StartNone, &EndNameInfo),
	elementfunc(_T(""), NULL, NULL)
};


void CMgaParser::StartProjectInfo(const attributes_type &attributes)
{

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		if( i->first == _T("metaguid") )
		{
			_bstr_t bstr = i->second.c_str();

			GUID guid;
			CopyTo(bstr, guid);

			CComVariant v;
			CopyTo(guid, v);

			v.Detach(infoparguid);

		}

		if( i->first == _T("metaname") )
		{
			*infoparname = _bstr_t(i->second.c_str()).Detach();
		}

		if( i->first == _T("metaversion") )
		{
			*infoparversion = _bstr_t(i->second.c_str()).Detach();
		}

		if( i->first == _T("version") )
		{
			*infoversion = _bstr_t(i->second.c_str()).Detach();
		}


		++i;
	}
}

void CMgaParser::EndNameInfo()
{
	if( GetPrevName() == _T("project") )
	{
		*infoprojname = _bstr_t(GetCurrData().c_str()).Detach();
		// analysis done, why waste time by processing further the xme file?
		throw SAXException( magic_exit_str); // trick used & handled only by GetXMLInfo()
	}
}

CMgaParser::elementfunc CMgaParser::elementfuncs_clipmgainfo[] = 
{
	elementfunc(_T("clipboard"), &StartClipboardInfo, &EndNone),
	elementfunc(_T(""), NULL, NULL)
};

void CMgaParser::StartClipboardInfo(const attributes_type &attributes)
{
	bool closure_version_found = false;
	*closversion = 0;
	*closacckind = 0;
	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		if( i->first == _T("closureversion") )
		{
			// if the value is _T("") then the bstr will be null, so bool introduced to correctly check the presence of "closureversion" token
			closure_version_found = true;

			*closversion = _bstr_t(i->second.c_str()).Detach();
		}
		else if( i->first == _T("acceptingkind") )
		{
			*closacckind = _bstr_t(i->second.c_str()).Detach();
		}
		++i;
	}
	if( !closure_version_found && *closversion == 0) // not found such token
	{
		CComBSTR bstr(L"0");
		*closversion = bstr.Detach();
	}
}

STDMETHODIMP CMgaParser::GetClipXMLInfo(BSTR filename, IMgaObject *target, VARIANT_BOOL *is_acceptable, BSTR* p_acckind, BSTR* p_version) 
{
	try
	{
		CopyTo(filename, xmlfile);

		XMLPlatformUtilsTerminate_RAII term;
		try
		{
			SAXParser parser;
			parser.setValidationScheme(SAXParser::Val_Never);
			parser.setDocumentHandler(this);
			parser.setErrorHandler(this);
			parser.setEntityResolver(this);

			//elementfuncs = elementfuncs_clipmgainfo;
			funcTableState = CLIP_MGA_INFO;

			pass_count = 1;

			ranges.clear();
			ranges.push_front(range_type());
			ranges.front().begin = 1;
			ranges.front().end = (counter_type)-1;
			skip_element_level = 0;

			closversion = p_version; // the result stored in these returning parameters p_...
			closacckind = p_acckind;

			parser.parse(xmlfile.c_str());

		}
		catch(const XMLException &e)
		{
			XmlStr desc(e.getMessage());

			ThrowXmlError(L"%s", desc.c_str());
		}

		CloseAll();
	}
	catch(hresult_exception &e)
	{
		CloseAll();

		ASSERT( FAILED(e.hr) );
		if( e.hr == E_XMLPARSER )
			SetErrorInfo(errorinfo);
		else
			SetErrorInfo2(e.hr);

		return e.hr;
	}

	return S_OK;
}

//void CMgaParser::RegisterReadOnlyStatus( const attributes_type &attributes)
//{
//	readonly_stack.push_back( GetByNameX( attributes, "perm") != 0);
//}

bool CMgaParser::GetIntendedReadOnlyFlag( bool *p_isReadOnly)
{
	ASSERT( !readonly_stack.empty());
	if( readonly_stack.empty()) return false;

	ASSERT( p_isReadOnly);
	*p_isReadOnly = readonly_stack.back();
	readonly_stack.pop_back();

	return true;
}

void CMgaParser::EndObject()
{
	bool ro = false; // should be made read only?
	bool re = GetIntendedReadOnlyFlag( &ro); // re: is stack ok?
	if( ro && re)
	{
		CComQIPtr<IMgaObject> obj( GetCurrent().object);
		ASSERT( obj);
		if( obj) COMTHROW( obj->PutReadOnlyAccess( VARIANT_TRUE));
	} // else if( re) stack error
}
