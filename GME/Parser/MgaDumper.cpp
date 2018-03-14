
#include "stdafx.h"
#include "Parser.h"
#include "MgaDumper.h"
#include "Transcoder.h"
#include <algorithm>

#define FLUSH_LIMIT			1000

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


// --------------------------- CMgaDumper

void CMgaDumper::InitDump(IMgaProject *p, BSTR xmlfile, BSTR encoding)
{
	if( p == NULL )
		HR_THROW(E_INVALIDARG);

	std::tstring filename;
	CopyTo(xmlfile, filename);

	if( filename.empty() )
		HR_THROW(E_INVALIDARG);

	ofs.init( filename.c_str(), encoding);

	elems.clear();

	ASSERT( project == NULL );
	ASSERT( territory == NULL );

	territory = NULL;
	COMTHROW( p->CreateTerritory(NULL, PutOut(territory), NULL) );

	COMTHROW( p->BeginTransaction(territory, TRANSACTION_READ_ONLY) );
	project = p;

	fco_count = 0;
}

void CMgaDumper::DoneDump(bool abort)
{
	ofs.finalize();
	elems.clear();

	if( territory != NULL )
		COMTHROW(territory->Destroy());
	territory = NULL;

	if( project != NULL )
	{
		if( abort )
			COMTHROW(project->AbortTransaction());
		else
			COMTHROW(project->CommitTransaction());
	}
	
	m_selFcos.clear();
	m_selFolders.clear();
	project = NULL;
}

STDMETHODIMP CMgaDumper::DumpProject(IMgaProject *p, BSTR xmlfile)
{
	return DumpProject2(p, xmlfile, NULL);
}

STDMETHODIMP CMgaDumper::DumpProject2(IMgaProject *p, BSTR xmlfile, ULONGLONG hwndParent_)
{
	CHECK_IN(p);
	m_dumpGuids = true; // dump GUIDs with the whole project
	m_closureDump = false;
	m_v2          = false;
	m_strictDump = false;
	m_dumpRelids = true;
	m_dumpLibraryStubs = false; // make sure closure logic does not involve in it

	COMTRY
	{
		HWND hwndParent = (HWND)hwndParent_;
		if (hwndParent != 0)
		{
			COMTHROW( m_progress.CoCreateInstance(L"Mga.MgaProgressDlg") );
			COMTHROW( m_progress->SetTitle(_bstr_t(L"Exporting XME file...")) );
			COMTHROW( m_progress->StartProgressDialog(hwndParent) );
		}
		InitDump(p, xmlfile, _bstr_t(L"UTF-8"));

		ofs << L"<!DOCTYPE project SYSTEM \"mga.dtd\">\n\n";

		Dump(p);

		DoneDump(false);
		if (m_progress != NULL )
		{
			COMTHROW(m_progress->StopProgressDialog());
			m_progress = NULL;
		}
	}
	COMCATCH(
		DoneDump(true);
		if (m_progress != NULL )
		{
			COMTHROW(m_progress->StopProgressDialog());
			m_progress = NULL;
		}
	)
}

STDMETHODIMP CMgaDumper::DumpFCOs(IMgaProject *proj, IMgaFCOs *p, IMgaFolders *f, IMgaRegNodes *r, BSTR xmlfile)
{
	return DumpFCOs2(proj, p, f, r, xmlfile, NULL);
}

STDMETHODIMP CMgaDumper::DumpFCOs2(IMgaProject *proj, IMgaFCOs *p, IMgaFolders *f, IMgaRegNodes *r, BSTR xmlfile, ULONGLONG hwndParent)
{
	m_dumpGuids = false; // while dumping selected FCOs do NOT dump guids of the fco's
	m_closureDump = false;
	m_v2          = false;
	m_strictDump = false; // this method uses the permissive dump
	// meaning that if a model/folder is in the closure then all children
	// and grandchildren are dumped as well

	m_dumpRelids = false; //dumpversion = 2; // no relids dumped
	m_dumpLibraryStubs = false; // make sure closure logic does not involve in it

	//CHECK_IN( (int)p | (int)f);

	COMTRY
	{
		if ( p) CopyTo(p, m_selFcos);
		if ( f) CopyTo(f, m_selFolders);
		if ( r) CopyTo(r, m_selRegNodes);

		if( m_selFcos.empty() && m_selFolders.empty() && m_selRegNodes.empty())
			return S_OK;

		CComObjPtr<IMgaProject> project;

		if ( !m_selFcos.empty())
			COMTHROW( m_selFcos.front()->get_Project( PutOut( project)) );
		else if ( !m_selFolders.empty())
			COMTHROW( m_selFolders.front()->get_Project( PutOut( project)) );
		else if ( !m_selRegNodes.empty()) // newly added
			project = proj;
		else
			return S_OK;

		if( !project) return S_OK;

		InitDump(project, xmlfile, _bstr_t(L"UTF-16"));

		ofs << L"<!DOCTYPE clipboard SYSTEM \"mga.dtd\" [\r\n";
		ofs << L"\t<!ELEMENT clipboard (folder|model|atom|reference|set|connection|regnode)*>\r\n";
		//ofs << "\t<!ATTLIST clipboard acceptingkind CDATA	#IMPLIED>\n";
		//ofs << "]>\n\n";
		ofs << L"\t<!ATTLIST clipboard\r\n";
		ofs << L"\t\tacceptingkind CDATA #IMPLIED\r\n";
		ofs << L"\t\tparadigmnamehint CDATA #IMPLIED\r\n";//TODO: could be extended with targetnamespacehint, srcnamespacehint
		ofs << L"\t>\r\n";
		ofs << L"]>\r\n\r\n";



		StartElem(_T("clipboard"));

		// dumping originating paradigm name as a hint for the parser
		CComObjPtr<IMgaMetaProject> metaproject;
		COMTHROW( project->get_RootMeta(PutOut(metaproject)) );
		ASSERT( metaproject != NULL );
		if( metaproject) Attr(_T("paradigmnamehint"), metaproject, &IMgaMetaProject::get_Name);

		CComObjPtrVector<IMgaFolder>::iterator fi = m_selFolders.begin();
		while( fi != m_selFolders.end() )
		{
			CComObjPtr<IMgaObject> obj;
			COMTHROW(territory->OpenObj(*fi, PutOut(obj)));
			CComObjPtr<IMgaFolder> folder;
			COMTHROW(obj.QueryInterface( folder));
			*fi = folder;
			++fi;
		}

		CComObjPtrVector<IMgaFCO>::iterator i = m_selFcos.begin();
		while( i != m_selFcos.end() )
		{
			// PETER: put the object into the parser's territory
			CComObjPtr<IMgaObject> obj;
			COMTHROW(territory->OpenObj(*i, PutOut(obj)));
			CComObjPtr<IMgaFCO> fco;
			COMTHROW(obj.QueryInterface( fco));
			*i = fco;
			++i;
		}

		if( m_selFolders.size() + m_selFcos.size() > 0) removeInnerObjs();
		
		for( fi = m_selFolders.begin(); fi != m_selFolders.end(); ++fi )
			Dump( *fi);

		for( i = m_selFcos.begin(); i != m_selFcos.end(); ++i )
			Dump ( *i);

		if( !m_selRegNodes.empty())
		{
			StartElem(_T("regnode"));
			Attr(_T("name"), _T("annotations"));
			StartElem(_T("value"));
			EndElem(); //value

			for( CComObjPtrVector<IMgaRegNode>::iterator ri = m_selRegNodes.begin(); ri != m_selRegNodes.end(); ++ri)
				Dump( *ri);

			EndElem(); // regnode of annotations
		}

		EndElem();
		DoneDump(false);
	}
	COMCATCH( DoneDump(true); )
}

// ------- Low level stuff

inline void CMgaDumper::Indent(int i)
{
	for(; i > 0; --i)
		ofs << '\t';
}

inline void CMgaDumper::StartElem(const TCHAR *name)
{
	ASSERT( name != NULL );

	if( !elems.empty() && !elems.back().inbody )
	{
		ASSERT( !elems.back().indata );

		ofs << L">\n";
		elems.back().inbody = true;
	}

	elems.push_back(elem());
	elems.back().name = name;
	elems.back().inbody = false;
	elems.back().indata = false;

	Indent((int)elems.size()-1);
	ofs << L'<' << name;
}

inline void CMgaDumper::Attr(const TCHAR *name, const TCHAR *value)
{
	ASSERT( name != NULL );
	ASSERT( value != NULL );

	ASSERT( !elems.empty() && !elems.back().inbody );
	
	ofs << L' ' << name << L"=\"" << Transcoder::StdEscape << value << Transcoder::NoEscape << L'"';
}

inline void CMgaDumper::Attr(const TCHAR *name, const TCHAR *value, int len)
{
	ASSERT( name != NULL );
	ASSERT( value != NULL );
	ASSERT( len >= 0 );

	ASSERT( !elems.empty() && !elems.back().inbody && !elems.back().indata );

	ofs << L' ' << name << L"=\"" << Transcoder::StdEscape << std::tstring( value, len) << Transcoder::NoEscape << L'"';
}

bool CMgaDumper::HasMarkup(const TCHAR *value, int len)
{
	while( --len >= 0 )
	{
		TCHAR c = *(value++);
		if( c == '<' || c == '>' || c == '&' || c == '\'' || c == '\"' )
			return true;
	}

	return false;
}

inline void CMgaDumper::Data(const TCHAR *value, int len)
{
	ASSERT( value != NULL );
	ASSERT( len >= 0 );
	ASSERT( !elems.empty() );

	if( !elems.back().inbody )
	{
		ofs << L">";
		elems.back().inbody = true;
	}

	ofs << Transcoder::StdEscape << std::tstring( value, len) << Transcoder::NoEscape;

	elems.back().indata = true;

}

inline void CMgaDumper::EndElem()
{
	ASSERT( !elems.empty() );

	if( elems.back().inbody )
	{
		if( !elems.back().indata )
			Indent((int)elems.size()-1);

		ofs << L"</" << elems.back().name << L">\n";
	}
	else
	{
		ofs << L"/>\n";
	}

	elems.pop_back();
}

// ------- CheckInClosure
// there are two ways:
//  strict: those objects are dumped which are selected ( can be found in m_selFCO/Folder)
//          this method is used by the copy closure methods
//  permissive: hierarchical selection, meaning that if a folder/model is selected then
//              its children/grandchildren need not to be selected (need not be in m_selFCO/Folder)
//              this is used by regular clipboard methods
//
inline bool CMgaDumper::CheckInClosure(CComObjPtr<IMgaAttribute> ) { return true;}
inline bool CMgaDumper::CheckInClosure(CComObjPtr<IMgaConnPoint> ) { return true;}

bool CMgaDumper::CheckInClosure(CComObjPtr<IMgaFolder> folder)
{
	if ( m_selFolders.empty() && m_selFcos.empty()) {
		// if the selected FCOs and selected Folders values are empty
		// then everything is in the closure 
		return true;
	}

	bool in_closure = false;

	if (m_strictDump)
	{
		in_closure = std::find_if( m_selFolders.begin(), m_selFolders.end(), GmeEqual( folder)) != m_selFolders.end();
	}
	else
	{
		CComObjPtr<IMgaFolder> curr_folder = folder;
		while ( curr_folder != NULL && !in_closure)
		{
			in_closure = std::find_if( m_selFolders.begin(), m_selFolders.end(), GmeEqual( curr_folder)) != m_selFolders.end();

			CComObjPtr<IMgaFolder> parent;
			COMTHROW(curr_folder->get_ParentFolder(PutOut( parent)));

			curr_folder = NULL;
			
			if (parent != NULL) {
				curr_folder = parent;
			}
		}
	}
	return in_closure;
}

bool CMgaDumper::CheckInClosure(CComObjPtr<IMgaFCO> fco) 
{
	if ( m_selFolders.empty() && m_selFcos.empty()) {
		// if the selected FCOs and selected Folders values are empty
		// then everything is in the closure 
		return true;
	}

	bool in_closure = false;

	if (m_strictDump)
	{
		in_closure = std::find_if( m_selFcos.begin(), m_selFcos.end(), GmeEqual( fco)) != m_selFcos.end();
	}
	else
	{

		CComObjPtr<IMgaFCO> curr_fco = fco;
		CComObjPtr<IMgaFCO> last_fco = fco;
		while ( curr_fco != NULL && !in_closure) {
			
			in_closure = std::find_if( m_selFcos.begin(), m_selFcos.end(), GmeEqual( curr_fco)) != m_selFcos.end();

			CComObjPtr<IMgaModel> parent;
			COMTHROW(curr_fco->get_ParentModel(PutOut(parent)));
			last_fco = curr_fco;
			curr_fco = NULL;
			if (parent != NULL) {
				COMTHROW( ::QueryInterface(parent, curr_fco) );
			}
		}

		if ( !in_closure) // no parent yet (model) in closure, need to check folders
		{
			CComObjPtr<IMgaFolder> par_folder;
			COMTHROW( last_fco->get_ParentFolder(PutOut( par_folder)));

			while ( par_folder != NULL && !in_closure)
			{
				in_closure = std::find_if( m_selFolders.begin(), m_selFolders.end(), GmeEqual( par_folder)) != m_selFolders.end();

				CComObjPtr<IMgaFolder> parent;
				COMTHROW(par_folder->get_ParentFolder(PutOut( parent)));

				par_folder = NULL;
				
				if (parent != NULL)
					par_folder = parent;
			}
		}
	}

	return in_closure;
}

// ------- Dumpers

void CMgaDumper::Dump(IMgaProject *project)
{
	ASSERT( project != NULL );

	CComVariant variant;
	CComBstrObj bstr;
	GUID guid;

	StartElem(_T("project"));

	COMTHROW( project->get_GUID(PutOut(variant)) );
	CopyTo(variant, guid);
	CopyTo(guid, PutOut(bstr));
	Attr(_T("guid"), bstr);

	Attr(_T("cdate"), project, &IMgaProject::get_CreateTime);
	Attr(_T("mdate"), project, &IMgaProject::get_ChangeTime);
	Attr(_T("version"), project, &IMgaProject::get_Version);

	CComObjPtr<IMgaMetaProject> metaproject;
	COMTHROW( project->get_RootMeta(PutOut(metaproject)) );
	ASSERT( metaproject != NULL );

	variant.Clear();
	bstr.Empty();
	COMTHROW( metaproject->get_GUID(PutOut(variant)) );
	CopyTo(variant, guid);
	CopyTo(guid, PutOut(bstr));
	Attr(_T("metaguid"), bstr);
	Attr(_T("metaversion"), metaproject, &IMgaMetaProject::get_Version);
	Attr(_T("metaname"), metaproject, &IMgaMetaProject::get_Name);
	


	StartElem(_T("name"));
	Data(project, &IMgaProject::get_Name);
	EndElem();

	StartElem(_T("comment"));
	Data(project, &IMgaProject::get_Comment);
	EndElem();

	StartElem(_T("author"));
	Data(project, &IMgaProject::get_Author);
	EndElem();

	CComObjPtr<IMgaFolder> folder;
	COMTHROW( project->get_RootFolder(PutOut(folder)) );
	Dump(folder);

	EndElem();
}

void CMgaDumper::Dump(IMgaFolder *folder)
{
	ASSERT( folder != NULL );

	StartElem(_T("folder"));

	Attr(_T("id"), folder, &IMgaFolder::get_ID);

	if( m_closureDump && m_v2)
	{
		// dump	closureguid attribute
		CComObjPtr<IMgaRegNode> regnode;
		COMTHROW( folder->get_RegistryNode( CComBSTR( GLOBAL_ID_STR), PutOut( regnode)));
		long status;
		COMTHROW( regnode->get_Status( &status));
		if( status == ATTSTATUS_HERE)
		{
			CComBstrObj guid_str;
			COMTHROW( regnode->get_Value( PutOut(guid_str)));
			if( guid_str.Length() == GLOBAL_ID_LEN) // using { 8-4-4-4-12} form
				Attr( _T("closureguid"), guid_str);
		}


		Attr( _T("closurename"), folder, &IMgaFolder::get_Name);

	}

	if(dumpversion >= 1) {
		if( m_dumpRelids)
		{
			LAttr(_T("relid"), folder, &IMgaFolder::get_RelID);
			LAttr(_T("childrelidcntr"), folder, &IMgaFolder::get_ChildRelIDCounter);
		}

		CComBstrObj libname;
		COMTHROW( folder->get_LibraryName(PutOut(libname)) );
		if(libname) Attr(_T("libref"),libname);

		VARIANT_BOOL readonly;
		COMTHROW( folder->HasReadOnlyAccess( &readonly));
		if( readonly) Attr( _T("perm"), _T("1"));
	}

	CComObjPtr<IMgaMetaFolder> metafolder;
	COMTHROW( folder->get_MetaFolder(PutOut(metafolder)) );
	Attr(_T("kind"), metafolder, &IMgaMetaFolder::get_Name);

	if( m_dumpGuids) // this is true if Project is dumped and mgaversion of project is 2
	{
		try 
		{
			CComBstrObj bs;
			HRESULT hr = folder->GetGuidDisp( PutOut( bs));
			if( SUCCEEDED( hr) && bs && bs.Length() == 38) // {%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X} <-- dumping that form (38 chars long including braces)
				Attr( _T("guid"), bs);//Attr( _T("guid"), IMgaObject::GetGuidDisp);

		} catch( hresult_exception& )
		{
		}
	}

	StartElem(_T("name"));
	Data(folder, &IMgaFolder::get_Name);
	EndElem();

	DumpConstraints(folder);

	Dump(folder, &IMgaFolder::get_ChildFolders);

	// dumping the regnodes
	CComObjPtrVector<IMgaRegNode> v;
	COMTHROW( folder->get_Registry(VARIANT_FALSE, PutOut(v)) );
	CComObjPtrVector<IMgaRegNode>::iterator i = v.begin();
	CComObjPtrVector<IMgaRegNode>::iterator e = v.end();
	
	Sort(&v);
	
	while( i != e )
	{
		Dump(*i);

		++i;
	}

	Dump(folder, &IMgaFolder::get_ChildFCOs);

	EndElem();
}

void CMgaDumper::Dump(IMgaRegNode *regnode)
{
	ASSERT( regnode != NULL );

	StartElem(_T("regnode"));

	Attr(_T("name"), regnode, &IMgaRegNode::get_Name);

	VARIANT_BOOL b;
	COMTHROW( regnode->get_Opacity(&b) );

	if( b != VARIANT_FALSE )
		Attr(_T("isopaque"), _T("yes"));

	long status;
	COMTHROW( regnode->get_Status( &status ) );
	if( status == -2) // ATTSTATUS_UNDEFINED
		Attr(_T("status"), _T("undefined"));

	StartElem(_T("value"));
	Data(regnode, &IMgaRegNode::get_Value);
	EndElem();

	// FIXME: MetaGME uses many regnodes on rootfolder, which makes progress bar freeze

	// dumping the subnodes
	CComObjPtrVector<IMgaRegNode> v;
	COMTHROW( regnode->get_SubNodes(VARIANT_FALSE, PutOut(v)) );
	Sort(&v);

	CComObjPtrVector<IMgaRegNode>::iterator i = v.begin();
	CComObjPtrVector<IMgaRegNode>::iterator e = v.end();
	while( i != e )
	{
		Dump(*i);

		++i;
	}

	EndElem();
}

void CMgaDumper::Dump(IMgaObject *obj)
{
	ASSERT( obj != NULL );

	objtype_enum objtype;
	COMTHROW( obj->get_ObjType(&objtype) );

	switch(objtype)
	{
	case OBJTYPE_MODEL:
		{
			CComObjPtr<IMgaModel> p;
			COMTHROW( ::QueryInterface(obj, p) );
			Dump(p);
			break;
		}

	case OBJTYPE_ATOM:
		{
			CComObjPtr<IMgaAtom> p;
			COMTHROW( ::QueryInterface(obj, p) );
			Dump(p);
			break;
		}

	case OBJTYPE_REFERENCE:
		{
			CComObjPtr<IMgaReference> p;
			COMTHROW( ::QueryInterface(obj, p) );
			Dump(p);
			break;
		}

	case OBJTYPE_CONNECTION:
		{
			CComObjPtr<IMgaConnection> p;
			COMTHROW( ::QueryInterface(obj, p) );
			Dump(p);
			break;
		}

	case OBJTYPE_SET:
		{
			CComObjPtr<IMgaSet> p;
			COMTHROW( ::QueryInterface(obj, p) );
			Dump(p);
			break;
		}

	case OBJTYPE_FOLDER:
		{
			CComObjPtr<IMgaFolder> p;
			COMTHROW( ::QueryInterface(obj, p) );
			Dump(p);
			break;
		}

	default:
		HR_THROW(E_INVALID_MGA);
	};
}

void CMgaDumper::DumpFCO(IMgaFCO *fco, bool dump_attrs, bool dump_name, bool dump_elems)
{
	ASSERT( fco != NULL );

	if (fco_count++ % FLUSH_LIMIT == 0)
	{
		ASSERT( territory != NULL );

		if (fco_count >= FLUSH_LIMIT)
		{
			COMTHROW( territory->Flush() );
			COMTHROW( project->CommitTransaction() );
			COMTHROW( project->BeginTransaction(territory, TRANSACTION_READ_ONLY) );
		}

		if (m_progress)
		{
			static TCHAR progress_msg[512];
			_stprintf_s(progress_msg, _T("Number of exported FCOs: %ld"), (long)fco_count);
			COMTHROW(m_progress->SetLine(0, PutInBstr(progress_msg)));
			COMTHROW(m_progress->SetProgress(fco_count, 0));
		}
	}

	if( dump_attrs )
	{
		Attr(_T("id"), fco, &IMgaFCO::get_ID);

		CComObjPtr<IMgaMetaFCO> metafco;
		COMTHROW( fco->get_Meta(PutOut(metafco)) );
		Attr(_T("kind"), metafco, &IMgaMetaFCO::get_Name);

		CComObjPtr<IMgaMetaRole> role;
		COMTHROW( fco->get_MetaRole(PutOut(role)) );
		if( role != NULL )
			Attr(_T("role"), role, &IMgaMetaRole::get_Name);

		if( m_dumpGuids) // this is true if Project is dumped and mgaversion of project is 2
		{
			try 
			{
				CComBstrObj bs;
				HRESULT hr = fco->GetGuidDisp( PutOut( bs));
				if( SUCCEEDED( hr) && bs && bs.Length() == 38) // {%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X} <-- dumping that form (38 chars long including braces)
					Attr( _T("guid"), bs);//Attr( _T("guid"), IMgaFCO::GetGuidDisp);

			} catch( hresult_exception& )
			{
			}
		}

		if( m_closureDump && m_v2)
		{
			// dump	closureguid attribute
			CComBstrObj guid_str; // this form will not create the node if is not present
			COMTHROW( fco->get_RegistryValue( CComBSTR( GLOBAL_ID_STR), PutOut( guid_str)));
			if( guid_str != 0 && guid_str.Length() == GLOBAL_ID_LEN) // using { 8-4-4-4-12} form
				Attr( _T("closureguid"), guid_str);

			Attr( _T("closurename"), fco, &IMgaFCO::get_Name);

		}

		VARIANT_BOOL prim = VARIANT_TRUE;
		bool lost_basetype = false;
		CComObjPtr<IMgaFCO> derivedfrom;
		COMTHROW( fco->get_DerivedFrom(PutOut(derivedfrom)) );
		if(derivedfrom != NULL )
		{
			if (CheckInClosure(derivedfrom)) {
				Attr(_T("derivedfrom"), derivedfrom, &IMgaFCO::get_ID);

				VARIANT_BOOL b;

				COMTHROW( fco->get_IsInstance(&b) );
				Attr(_T("isinstance"), b != VARIANT_FALSE ? _T("yes") : _T("no"));

				COMTHROW( fco->get_IsPrimaryDerived(&prim) );
				Attr(_T("isprimary"), prim != VARIANT_FALSE ? _T("yes") : _T("no"));
			}
			else {
				lost_basetype = true;
			}

			if( m_closureDump && !CheckInClosure(derivedfrom))//!CIN() introduced fresly //<!>
			{
				if ( m_dumpLibraryStubs && !CheckInClosure(derivedfrom) && !isInLibrary( fco) && isInLibrary( derivedfrom)) { //derived from a type defined in a library
					CComBSTR name;
					COMTHROW( derivedfrom->get_AbsPath( &name));
					std::tstring nm;
					CopyTo( name, nm);
					
					Attr(_T("closurelibderivedfrom"), nm);

					VARIANT_BOOL b;

					COMTHROW( fco->get_IsInstance(&b) );
					Attr(_T("isinstance"), b != VARIANT_FALSE ? _T("yes") : _T("no"));

					COMTHROW( fco->get_IsPrimaryDerived(&prim) );
					Attr(_T("isprimary"), prim != VARIANT_FALSE ? _T("yes") : _T("no"));
				}
				else if( m_v2)
				{
					CComBSTR name;

					COMTHROW( derivedfrom->get_AbsPath( &name));
					std::tstring nm;
					CopyTo( name, nm);
					
					COMTHROW( fco->get_AbsPath( &name));
					std::tstring nm2;
					CopyTo( name, nm2);

					Attr(_T("closure2derivedfrom"), makeRel( nm, nm2));

					VARIANT_BOOL b;

					COMTHROW( fco->get_IsInstance(&b) );
					Attr(_T("isinstance"), b != VARIANT_FALSE ? _T("yes") : _T("no"));

					COMTHROW( fco->get_IsPrimaryDerived(&prim) );
					Attr(_T("isprimary"), prim != VARIANT_FALSE ? _T("yes") : _T("no"));
				}
			}

			if( m_closureDump && m_v2)
			{
				CComBstrObj guid;
				COMTHROW( derivedfrom->get_RegistryValue( CComBSTR( GLOBAL_ID_STR), PutOut( guid)));
				if( guid != 0 && guid.Length() == GLOBAL_ID_LEN)
					Attr( _T("smartDerivedFromGUID"), guid);
			}
		}
		if(prim && (dumpversion >= 1) && (!lost_basetype) && m_dumpRelids)
			LAttr(_T("relid"), fco, &IMgaFCO::get_RelID);

		VARIANT_BOOL readonly;
		COMTHROW( fco->HasReadOnlyAccess( &readonly));
		if( readonly) Attr( _T("perm"), _T("1"));
	}

	if( dump_name )
	{
		StartElem(_T("name"));
		Data(fco, &IMgaFCO::get_Name);
		EndElem();
	}

	if( dump_elems )
	{
		DumpConstraints(fco);

		// dumping the regnodes
		CComObjPtrVector<IMgaRegNode> v;
		COMTHROW( fco->get_Registry(VARIANT_FALSE, PutOut(v)) );
		CComObjPtrVector<IMgaRegNode>::iterator i = v.begin();
		CComObjPtrVector<IMgaRegNode>::iterator e = v.end();
	
		Sort(&v);

		while( i != e )
		{
			Dump(*i);

			++i;
		}

		Dump(fco, &IMgaFCO::get_Attributes);
	}
}

void CMgaDumper::Dump(IMgaModel *model)
{
	ASSERT( model != NULL );

	StartElem(_T("model"));

	DumpFCO(model, true, false, false);	
	if(dumpversion >= 1 && m_dumpRelids) LAttr(_T("childrelidcntr"), model, &IMgaModel::get_ChildRelIDCounter);
	DumpFCO(model, false, true, true);	

	Dump(model, &IMgaModel::get_ChildFCOs);

	EndElem();
}

void CMgaDumper::DumpConstraints(IMgaObject *object)
{
	ASSERT( object != NULL );

	CComObjPtrVector<IMgaConstraint> v;

	CComObjPtr<IMgaFCO> fco;
	if( SUCCEEDED(::QueryInterface(object, fco)) )
	{
		COMTHROW( fco->get_Constraints(VARIANT_FALSE, PutOut(v)) );
	}
	else
	{
		CComObjPtr<IMgaFolder> folder;
		COMTHROW( ::QueryInterface(object, folder) );
		COMTHROW( folder->get_Constraints(VARIANT_FALSE, PutOut(v)) );
	}

	CComObjPtrVector<IMgaConstraint>::iterator i = v.begin();
	CComObjPtrVector<IMgaConstraint>::iterator e = v.end();
	while( i != e )
	{
		Dump(*i);

		++i;
	}
}

void CMgaDumper::Dump(IMgaConstraint *constraint)
{
	ASSERT( constraint != NULL );

	StartElem(_T("constraint"));

	StartElem(_T("name"));
	Data(constraint, &IMgaConstraint::get_Name);
	EndElem();

	StartElem(_T("value"));
	Data(constraint, &IMgaConstraint::get_Expression);
	EndElem();

	EndElem();
}

void CMgaDumper::Dump(IMgaAttribute *attribute)
{
	ASSERT( attribute != NULL );

	StartElem(_T("attribute"));
	
	CComObjPtr<IMgaMetaAttribute> metaattr;
	COMTHROW( attribute->get_Meta(PutOut(metaattr)) );
	Attr(_T("kind"), metaattr, &IMgaMetaAttribute::get_Name);

	long status;
	COMTHROW( attribute->get_Status(&status) );
	if( status < 0 )
		Attr(_T("status"), _T("meta"));
	else if( status > 0 )
		Attr(_T("status"), _T("inherited"));

	StartElem(_T("value"));

	attval_enum attval;
	COMTHROW( metaattr->get_ValueType(&attval) );

	std::tstring data;

	switch(attval)
	{
	case ATTVAL_STRING:
	case ATTVAL_ENUM:
	case ATTVAL_DYNAMIC:
		{
			CComBstrObj bstr;
			COMTHROW( attribute->get_StringValue(PutOut(bstr)) );
			CopyTo(bstr, data);
			break;
		}

	case ATTVAL_INTEGER:
		{
			long l;
			COMTHROW( attribute->get_IntValue(&l) );
			Format(data, _T("%ld"), l);
			break;
		}

	case ATTVAL_DOUBLE:
		{
			double d;
			COMTHROW( attribute->get_FloatValue(&d) );
			wchar_t dblbuf[40];
			_swprintf_s_l(dblbuf, sizeof(dblbuf) / sizeof(dblbuf[0]), L"%.17g", c_locale, d);
			data = dblbuf;
			break;
		}

	case ATTVAL_BOOLEAN:
		{
			VARIANT_BOOL b;
			COMTHROW( attribute->get_BoolValue(&b) );

			if( b != VARIANT_FALSE )
				data = _T("true");
			else
				data = _T("false");

			break;
		}

	case ATTVAL_REFERENCE:
		{
			CComObjPtr<IMgaFCO> fco;
			COMTHROW( attribute->get_FCOValue(PutOut(fco)) );

			if( (fco != NULL) && (CheckInClosure(fco)) )
			{
				CComBstrObj bstr;
				COMTHROW( fco->get_ID(PutOut(bstr)) );
				CopyTo(bstr, data);
			}
			break;
		}

	default:
		HR_THROW(E_INVALID_META);
	};
		
	Data(data);
	EndElem();

//	Dump(attribute, IMgaAttribute::get_Registry);

	EndElem();
}

void CMgaDumper::Dump(IMgaAtom *atom)
{
	ASSERT( atom != NULL );

	StartElem(_T("atom"));

	DumpFCO(atom);

	EndElem();
}

void CMgaDumper::Dump(IMgaReference *reference)
{
	ASSERT( reference != NULL );

	StartElem(_T("reference"));

	DumpFCO(reference, true, false, false);

	CComObjPtr<IMgaFCO> fco;
	COMTHROW( reference->get_Referred(PutOut(fco)) );
	
	if( fco != NULL)
	{
		if( CheckInClosure( fco)) // regular dump or the fco is really in the closure
			Attr(_T("referred"), fco, &IMgaFCO::get_ID);
		
		if( m_closureDump)
		{
			if ( m_dumpLibraryStubs && !CheckInClosure( fco) && !isInLibrary( reference) && isInLibrary( fco)) //reference pointing out to a library
			{
				CComBSTR name;
				COMTHROW( fco->get_AbsPath( &name));
				std::tstring nm;
				CopyTo( name, nm);
				
				Attr(_T("closurelibreferred"), nm);
			}

			// dump the relativepath of the referred object if v2
			else if( m_v2) // _T("else") introd freshly, not to dump to paths, the parser will check first for closurelibreferred
			{
				CComBSTR name;
				COMTHROW( fco->get_AbsPath( &name));
				std::tstring nm;
				CopyTo( name, nm);

				COMTHROW( reference->get_AbsPath( &name));
				std::tstring nm2;
				CopyTo( name, nm2);

				
				Attr(_T("closure2referred"), makeRel( nm, nm2));
			}

			// dump guid of fco if m_v2
			if( m_v2)
			{
				CComBstrObj ref_guid;
				COMTHROW( fco->get_RegistryValue( CComBSTR( GLOBAL_ID_STR), PutOut( ref_guid)));
				if( ref_guid != 0 && ref_guid.Length() == GLOBAL_ID_LEN) // valid guid
					Attr( _T("smartReferredGUID"), ref_guid);
			}
		}
	}


	CComObjPtr<IMgaFCO> base;
	COMTHROW( reference->get_DerivedFrom(PutOut(base)));
	if(base && CheckInClosure(base)
		|| base && m_closureDump && m_v2) {
		short stat;
		COMTHROW( reference->CompareToBase(&stat));
		if(!stat) {
			Attr(_T("isbound"), _T("yes") );
		}
	}

	DumpFCO(reference, false, true, true);

	EndElem();
}

void CMgaDumper::Dump(IMgaConnection *connection)
{
	ASSERT( connection != NULL );

	StartElem(_T("connection"));

	bool skipdump = false;

	CComObjPtr<IMgaFCO> base;
	COMTHROW( connection->get_DerivedFrom(PutOut(base)));
	if(base && CheckInClosure(base)
		|| base && m_closureDump && m_v2) {
		short stat;
		COMTHROW( connection->CompareToBase(NULL, &stat));
		if(!stat) {
			Attr(_T("isbound"), _T("yes") );
			skipdump = true;
		}
	}

	if( m_closureDump && m_v2)
		DumpConnDetails( CComObjPtr<IMgaConnection>( connection));

	DumpFCO(connection);

	if(!skipdump)
		Dump(connection, &IMgaConnection::get_ConnPoints);

	EndElem();
}

void CMgaDumper::Dump(IMgaSet *set)
{
	ASSERT( set != NULL );

	StartElem(_T("set"));

	DumpFCO(set, true, false, false);
	bool skipdump = false;
	CComObjPtr<IMgaFCO> base;
	COMTHROW( set->get_DerivedFrom(PutOut(base)));
	if(base && CheckInClosure(base)
		|| base && m_closureDump && m_v2) {
		short stat;
		COMTHROW( set->CompareToBase(&stat));
		if(!stat) {
			Attr(_T("isbound"), _T("yes") );
			skipdump = true;
		}
	}


	if(!skipdump) {
		CComObjPtrVector<IMgaFCO> members;
		COMTHROW( set->get_Members(PutOut(members)) );
		Sort( &members); // we sort the set members by ID
		
		if( m_closureDump)
		{
			CComBSTR name;
			COMTHROW( set->get_AbsPath( &name));
			CopyTo( name, m_currAbsPath); // will be used by the DumpIDRefs

			if( m_v2)
			{
				std::tstring memberguids = DumpMixedGUIDRefs( members); // uses m_currAbsPath
				if( !memberguids.empty()) Attr( _T("smartMemberGUIDs"), memberguids);
			}
		}

		DumpIDRefs(_T("members"), members); // might use m_currAbsPath
	}

	DumpFCO(set, false, true, true);

	EndElem();
}

void CMgaDumper::Dump(IMgaConnPoint *connpoint)
{
	ASSERT( connpoint != NULL );

	StartElem(_T("connpoint"));

#ifdef _DEBUG
	CComBSTR rolename;
	connpoint->get_ConnRole(&rolename);
	ASSERT(rolename.Length() != 0);
#endif

	Attr(_T("role"), connpoint, &IMgaConnPoint::get_ConnRole);
	
	CComObjPtr<IMgaFCO> target;
	COMTHROW( connpoint->get_Target(PutOut(target)) );

	if( target == NULL )
		HR_THROW(E_INVALID_MGA);


	CComPtr<IMgaConnection> conn;
	COMTHROW( connpoint->get_Owner(&conn));

	CComObjPtr<IMgaFCO> base;
	COMTHROW( conn->get_DerivedFrom(PutOut(base)));
	if(base && CheckInClosure(base)
		|| base && m_closureDump && m_v2) {
		short stat;
		COMTHROW( conn->CompareToBase(connpoint, &stat));
		if(!stat) {
			Attr(_T("isbound"), _T("yes") );
		}
	}

	Attr(_T("target"), target, &IMgaFCO::get_ID);

	if( m_closureDump)
	{
		CComBSTR name;
		COMTHROW( conn->get_AbsPath( &name));
		CopyTo( name, m_currAbsPath); // used by DumpIDRefs

		// m_dumpLibraryStubs is true, so the connection to a library element will be dumped with "closurelibtarget" attribute and a dummy! "target" attribute
		if ( m_dumpLibraryStubs && !isInLibrary( conn) && isInLibrary( target))
		{
			CComBSTR name;
			COMTHROW( target->get_AbsPath( &name));
			std::tstring nm;
			CopyTo( name, nm);
			
			Attr(_T("closurelibtarget"), nm);
		}
		else if( m_v2) // closure v2
		{
			CComBSTR name;
			COMTHROW( target->get_AbsPath( &name));
			std::tstring nm;
			CopyTo( name, nm);

			Attr(_T("closure2target"), makeRel( nm, m_currAbsPath));
		} 
	}


	CComObjPtrVector<IMgaFCO> refs;
	COMTHROW( connpoint->get_References(PutOut(refs)) );
	// m_currAbsPath is set above, at the "closure2target" dump
	DumpIDRefs(_T("refs"), refs);

	EndElem();
}

void CMgaDumper::DumpIDRefs(const TCHAR *name, CComObjPtrVector<IMgaFCO> &fcos)
{
	if( !fcos.empty() )
	{
		std::tstring idrefs, clos_idrefs;

		CComObjPtrVector<IMgaFCO>::iterator i = fcos.begin();
		CComObjPtrVector<IMgaFCO>::iterator e = fcos.end();
		while( i != e )
		{
			if( m_closureDump)
			{
				CComBSTR name;
				COMTHROW( (*i)->get_AbsPath( &name));
				std::tstring nm;
				CopyTo( name, nm);

				if( !clos_idrefs.empty() )
					clos_idrefs += ' ';

				clos_idrefs += makeRel( nm, m_currAbsPath);
			}

			if (!CheckInClosure(*i)) {
				++i; // inserted by ZolMol
				continue;
			}
			CComBstrObj bstr;
			COMTHROW( (*i)->get_ID(PutOut(bstr)) );

			std::tstring id;
			CopyTo(bstr, id);

			if( !idrefs.empty() )
				idrefs += ' ';

			idrefs += id;

			++i;
		}

		if ( !idrefs.empty())
			Attr(name, idrefs);

		if( m_closureDump && m_v2 && !clos_idrefs.empty()) // closure v2
		{
			std::tstring clos_name(_T("closure2")); clos_name += name;
			Attr( clos_name.c_str(), clos_idrefs);
		}
	}
}
std::tstring CMgaDumper::DumpGUIDRefs( CComObjPtrVector<IMgaFCO>& fcos)
{
	std::tstring guidrefs;

	for( CComObjPtrVector<IMgaFCO>::iterator i = fcos.begin(); i != fcos.end(); ++i)
	{
		CComBSTR bstr;
		COMTHROW( (*i)->get_RegistryValue( CComBSTR( GLOBAL_ID_STR), &bstr));

		if( bstr != 0 && bstr.Length() == GLOBAL_ID_LEN)
		{
			std::tstring guid;
			CopyTo(bstr, guid);

			if( !guidrefs.empty() )
				guidrefs += ' ';

			guidrefs += guid;
		}
		else
			return _T(""); // if one guid not found, then all are disregarded
	}

	return guidrefs;
}

std::tstring CMgaDumper::DumpMixedGUIDRefs( CComObjPtrVector<IMgaFCO>& fcos)
{
	// this method produces a sequence of the {guid}=path forms
	//{E200BEEB-34BC-4271-A134-AA5728C38124}\\/@../@module_ref1|kind=module_ref|relpos=0
	std::tstring guidrefs;

	for( CComObjPtrVector<IMgaFCO>::iterator i = fcos.begin(); i != fcos.end(); ++i)
	{

		CComBSTR name;
		COMTHROW( (*i)->get_AbsPath( &name));
		std::tstring nm;
		CopyTo( name, nm);

		if( !guidrefs.empty() )
			guidrefs += ' ';

		CComBSTR bstr;
		COMTHROW( (*i)->get_RegistryValue( CComBSTR( GLOBAL_ID_STR), &bstr));

		if( bstr != 0 && bstr.Length() == GLOBAL_ID_LEN)
		{
			std::tstring guid;
			CopyTo(bstr, guid);


			guidrefs += guid;
			guidrefs += '\\';
		}
		//else
		//	return ""; // if one guid not found, then all are disregarded
		
		guidrefs += makeRel( nm, m_currAbsPath);
	}

	return guidrefs;
}

//sort the RegNode Vector in place by Name
void CMgaDumper::Sort(CComObjPtrVector<IMgaRegNode> *v)
{
	int n = v->size();

	//small speed up, build a vector of strings first, then sort the strings
	//while doing the sort operations on the original vector, so we don't have
	//to call the get_Name function each time we look at a RegNode
	std::vector<std::tstring> vPrime;
	for (int i=0; i<n; i++)
	{
		CComBSTR bstr;
		COMTHROW((*v)[i]->get_Name(&bstr));
		std::tstring s;
		CopyTo(bstr,s);
		vPrime.push_back(s);
	}
	
	//in place insertion sort
	for (int x=1; x<n; x++) 
	{
		std::tstring index = vPrime[x];
		CComObjPtr<IMgaRegNode> indexRegNode = (*v)[x];
		int y = x;

		while ((y>0)&&(vPrime[y-1].compare(index)>0))
		{
				vPrime[y]=vPrime[y-1];

				(*v)[y]=(*v)[y-1];

				--y;
		}

		vPrime[y] = index;
		(*v)[y] = indexRegNode;
	}		

}

//Sort the Attribute Vector by Kind
void CMgaDumper::Sort(CComObjPtrVector<IMgaAttribute> *v)
{
	int n = v->size();

	//small speed up, build a vector of strings first, then sort the strings
	//while doing the sort operations on the original vector, so we don't have
	//to get the Kind Name every time
	std::vector<std::tstring> vPrime;
	for (int i=0; i<n; i++)
	{
		CComBSTR bstr;
		CComPtr<IMgaMetaAttribute> meta;
		COMTHROW((*v)[i]->get_Meta(&meta));
		COMTHROW(meta->get_Name(&bstr));
		std::tstring s;
		CopyTo(bstr,s);
		vPrime.push_back(s);
	}
	
	//in place insertion sort
	for (int x=1; x<n; x++) 
	{
		std::tstring index = vPrime[x];
		CComObjPtr<IMgaAttribute> indexAttr = (*v)[x];
		int y = x;

		while ((y>0)&&(vPrime[y-1].compare(index)>0))
		{
				vPrime[y]=vPrime[y-1];

				(*v)[y]=(*v)[y-1];

				--y;
		}

		vPrime[y] = index;
		(*v)[y] = indexAttr;
	}	
}

//Sort the Connection Point Vector by Target ID
void CMgaDumper::Sort(CComObjPtrVector<IMgaConnPoint> *v)
{
	int n = v->size();

	//small speed up, build a vector of strings first, then sort the strings
	//while doing the sort operations on the original vector, so we don't have
	//to get the Target ID every time
	std::vector<std::tstring> vPrime;
	for (int i=0; i<n; i++)
	{
		CComBSTR bstr;
		CComPtr<IMgaFCO> fco;
		COMTHROW((*v)[i]->get_Target(&fco));
		COMTHROW(fco->get_ID(&bstr));
		std::tstring s;
		CopyTo(bstr,s);
		// Tie-break sort on role, for self-connections
		CComBSTR role;
		COMTHROW((*v)[i]->get_ConnRole(&role));
		s += role;
		vPrime.push_back(s);
	}
	
	//in place insertion sort
	for (int x=1; x<n; x++) 
	{
		std::tstring index = vPrime[x];
		CComObjPtr<IMgaConnPoint> indexConnPoint = (*v)[x];
		int y = x;

		while ((y>0)&&(vPrime[y-1].compare(index)>0))
		{
				vPrime[y]=vPrime[y-1];

				(*v)[y]=(*v)[y-1];

				--y;
		}

		vPrime[y] = index;
		(*v)[y] = indexConnPoint;
	}	
}

//sort the Folder Vector by ID
void CMgaDumper::Sort(CComObjPtrVector<IMgaFolder> *v)
{
	int n = v->size();
	
	//small speed up, build a vector of strings first, then sort the strings
	//while doing the sort operations on the original vector, so we don't have
	//to call the get_ID function each time we look at a Folder
	std::vector<std::tstring> vPrime;
	for (int i=0; i<n; i++)
	{
		CComBSTR bstr;
		COMTHROW((*v)[i]->get_ID(&bstr));
		std::tstring s;
		CopyTo(bstr,s);
		vPrime.push_back(s);
	}	
	
	//in place insertion sort
	for (int x=1; x<n; x++) 
	{
		std::tstring index = vPrime[x];
		CComObjPtr<IMgaFolder> indexFolder = (*v)[x];
		int y = x;

		while ((y>0)&&(vPrime[y-1].compare(index)>0))
		{
				vPrime[y]=vPrime[y-1];

				(*v)[y]=(*v)[y-1];

				--y;
		}

		vPrime[y] = index;
		(*v)[y] = indexFolder;
	}	
	
}

//Sort the FCO vector in place by ID
void CMgaDumper::Sort(CComObjPtrVector<IMgaFCO> *v)
{
	int n = v->size();
	
	//small speed up, build a vector of strings first, then sort the strings
	//while doing the sort operations on the original vector, so we don't have
	//to call the get_ID function each time we look at an FCO
	std::vector<std::tstring> vPrime;
	for (int i=0; i<n; i++)
	{
		CComBSTR bstr;
		COMTHROW((*v)[i]->get_ID(&bstr));
		std::tstring s;
		CopyTo(bstr,s);
		vPrime.push_back(s);
	}

	//in place insertion sort
	for (int x=1; x<n; x++) 
	{
		std::tstring index = vPrime[x];
		CComObjPtr<IMgaFCO> indexFCO = (*v)[x];
		int y = x;

		while ((y>0)&&(vPrime[y-1].compare(index)>0))
		{
				vPrime[y]=vPrime[y-1];

				(*v)[y]=(*v)[y-1];

				--y;
		}

		vPrime[y] = index;
		(*v)[y] = indexFCO;
	}	

}

void CMgaDumper::putInTerritory( CComObjPtrVector<IMgaFCO>& fco_vec)
{		
	CComObjPtrVector<IMgaFCO>::iterator i = fco_vec.begin();
	for( ; i != fco_vec.end(); ++i )
	{
		CComObjPtr<IMgaObject> obj;
		COMTHROW(territory->OpenObj(*i, PutOut(obj)));
		CComObjPtr<IMgaFCO> fco;
		COMTHROW(obj.QueryInterface(fco));
		*i = fco;
	}
}

void CMgaDumper::putInTerritory( CComObjPtrVector<IMgaFolder>& fold_vec)
{
	CComObjPtrVector<IMgaFolder>::iterator j = fold_vec.begin();
	for( ; j != fold_vec.end(); ++j )
	{
		CComObjPtr<IMgaObject> obj;
		COMTHROW(territory->OpenObj(*j, PutOut(obj)));
		CComObjPtr<IMgaFolder> fold;
		COMTHROW(obj.QueryInterface(fold));
		*j = fold;
	}
}

//
// method called from the DumpFCO
// it is zombie-aware since it may happen that an object is placed to the clipboard
// then deleted, then the clipboard may be dumped when the project is closed
void CMgaDumper::removeInnerObjs()
{
	bool zombies_found = false;
	// try to remove those objects which are part of (child, grandchild) of another selected object
	CComObjPtrVector<IMgaFCO>::iterator i = m_selFcos.begin();
	while( i != m_selFcos.end())
	{
		bool zombie = false;
		long status;
		COMTHROW( (*i)->get_Status(&status));
		if (status != OBJECT_EXISTS) 
		{
			zombies_found = zombie = true;
		}

		bool found_parent = false;
		if( !zombie)
		{
			CComObjPtr<IMgaFCO> last_fco = *i, curr_fco = *i;
			do
			{
				CComObjPtr<IMgaModel> parent;
				COMTHROW( curr_fco->get_ParentModel(PutOut(parent)));
				last_fco = curr_fco;
				curr_fco = NULL;
				if (parent != NULL) {
					COMTHROW( ::QueryInterface(parent, curr_fco) );
					found_parent = std::find_if( m_selFcos.begin(), m_selFcos.end(), GmeEqual( curr_fco)) != m_selFcos.end();
				}
			} while ( curr_fco != NULL && !found_parent);

			if ( !found_parent) // no parent found in the set of fcos
			{
				CComObjPtr<IMgaFolder> par_folder;
				COMTHROW( last_fco->get_ParentFolder(PutOut( par_folder)));

				while ( par_folder != NULL && !found_parent)
				{
					found_parent = std::find_if( m_selFolders.begin(), m_selFolders.end(), GmeEqual( par_folder)) != m_selFolders.end();

					CComObjPtr<IMgaFolder> parent;
					COMTHROW( par_folder->get_ParentFolder(PutOut( parent)));

					par_folder = parent;
				}
			}
		}

		if ( found_parent || zombie)
			i = m_selFcos.erase( i);
		else
			++i;
	}	


	// try to remove those folders which are part of (child, grandchild) of another selected folder
	CComObjPtrVector<IMgaFolder>::iterator k = m_selFolders.begin();
	while( k != m_selFolders.end())
	{
		bool zombie = false;
		long status;
		COMTHROW( (*k)->get_Status(&status));
		if (status != OBJECT_EXISTS)
		{
			zombies_found = zombie = true;
		}

		bool found_parent = false;
		if( !zombie)
		{
			CComObjPtr<IMgaFolder> par_folder;
			COMTHROW( (*k)->get_ParentFolder(PutOut( par_folder)));
			
			while ( par_folder != NULL && !found_parent)
			{
				found_parent = std::find_if( m_selFolders.begin(), m_selFolders.end(), GmeEqual( par_folder)) != m_selFolders.end();

				CComObjPtr<IMgaFolder> parent;
				COMTHROW(par_folder->get_ParentFolder(PutOut( parent)));
				par_folder = parent;
			}
		}
		
		if ( found_parent || zombie)
			k = m_selFolders.erase( k);
		else
			++k;
	}

	ASSERT( zombies_found || m_selFolders.size() + m_selFcos.size() > 0);
}

STDMETHODIMP CMgaDumper::DumpClos( IMgaFCOs *p_sel_fcos, IMgaFolders *p_sel_folds,  BSTR xmlfile, int dump_options)
{
	m_closureDump = true;
	m_strictDump = true; // strict dump meaning that all dumped objects must be strictly in the closure
	
	m_dumpRelids = false; // dumpversion = 2; // no relids dumped
	m_dumpGuids  = true;  // dump these, as they are needed by new features

	m_dumpLibraryStubs =	(dump_options & 0x1) == 0x1; // dump library stubs ( absolute path used as libreferred or libderivedfrom attribute)
	m_v2 =					(dump_options & 0x2) == 0x2;
	
	COMTRY
	{
		if ( p_sel_fcos )
			CopyTo( p_sel_fcos, m_selFcos);

		if ( p_sel_folds) 
			CopyTo( p_sel_folds, m_selFolders);

		CComObjPtr<IMgaProject> project;

		if ( !m_selFcos.empty())
			COMTHROW( m_selFcos.front()->get_Project( PutOut( project)) );
		else if ( !m_selFolders.empty())
			COMTHROW( m_selFolders.front()->get_Project( PutOut( project)) );
		else
			return S_OK;


		InitDump( project, xmlfile, _bstr_t(L"UTF-16"));

		putInTerritory( m_selFcos);
		putInTerritory( m_selFolders);
		
		if ( false) // clipboard format
		{
			ofs << L"<!DOCTYPE clipboard SYSTEM \"mgaclosure.dtd\" [\n";
			ofs << L"\t<!ELEMENT clipboard (folder|model|atom|reference|set|connection|regnode)*>\n";
			ofs << L"\t<!ATTLIST clipboard\n";
			ofs << L"\t\tclosureversion CDATA #IMPLIED\n";
			ofs << L"\t\tacceptingkind CDATA #IMPLIED\n";
			ofs << L"\t\tparadigmnamehint CDATA #IMPLIED\n"; // just for compatibility with raw copied data
			ofs << L"\t>\n";
			ofs << L"]>\n\n";

			StartElem(_T("clipboard"));
			Attr(_T("closureversion"), _T("1"));
			Dump( project); 
			EndElem();
		}
		else
		{
			ofs << L"<!DOCTYPE project SYSTEM \"mgaclosure.dtd\">\n\n";

			Dump( project); 
		}
		DoneDump(false);
	}
	COMCATCH( DoneDump(true); )

	m_selFcos.clear();
	m_selFolders.clear();
}

STDMETHODIMP CMgaDumper::DumpClosR(
			IMgaFCOs *p_sel_fcos, IMgaFolders *p_sel_folds, BSTR xmlfile, 
			IMgaFCOs *p_top_fcos, IMgaFolders *p_top_folds, int dump_options, BSTR abspath, BSTR acceptingkinds)
{
	m_closureDump = true;
	m_strictDump = true; // strict dump meaning that all dumped objects must be strictly in the closure
	m_dumpRelids = false; //dumpversion = 2; // no relids dumped
	m_dumpGuids  = true; // dump these, as they are needed by new features

	m_dumpLibraryStubs	= (dump_options & 0x1) == 0x1; // dump library stubs ( absolute path used as closurelibreferred, closurelibderivedfrom, closurelibtarget attribute)
	m_v2				= (dump_options & 0x2) == 0x2; // dump the closure2members, closure2refs like attributes

	std::tstring version_string;
	if( dump_options & 0x4)
		version_string = _T("4");
	else
		version_string = _T("1");
	
	//CopyTo( abspath, m_currParAbsPath);

	COMTRY
	{
		if ( p_sel_fcos )
			CopyTo( p_sel_fcos, m_selFcos);

		if ( p_sel_folds) 
			CopyTo( p_sel_folds, m_selFolders);

		CComObjPtr<IMgaProject> project;

		if ( !m_selFcos.empty())
			COMTHROW( m_selFcos.front()->get_Project( PutOut( project)) );
		else if ( !m_selFolders.empty())
			COMTHROW( m_selFolders.front()->get_Project( PutOut( project)) );
		else
			return S_OK;

		CComObjPtrVector<IMgaFCO>	parentless_fcos; // parentless fcos from the selected set
		CComObjPtrVector<IMgaFolder> parentless_folders; // parentless folders from the selected set

		if ( p_top_fcos )
			CopyTo( p_top_fcos, parentless_fcos);

		if ( p_top_folds) 
			CopyTo( p_top_folds, parentless_folders);

		if ( parentless_fcos.empty() && parentless_folders.empty())
		{
			// this case should be handled by the DumpClos()
			ASSERT( 0);
			return DumpClos( p_sel_fcos, p_sel_folds, xmlfile, dump_options);
		}

		InitDump( project, xmlfile, _bstr_t(L"UTF-16"));

		putInTerritory( m_selFcos);
		putInTerritory( parentless_fcos);
		putInTerritory( m_selFolders);
		putInTerritory( parentless_folders);

		ofs << L"<!DOCTYPE clipboard SYSTEM \"mgaclosure.dtd\" [\n";
		ofs << L"\t<!ELEMENT clipboard (folder|model|atom|reference|set|connection|regnode)*>\n";
		ofs << L"\t<!ATTLIST clipboard\n";
		ofs << L"\t\tclosureversion CDATA #IMPLIED\n";
		ofs << L"\t\tacceptingkind CDATA #IMPLIED\n";
		ofs << L"\t\tparadigmnamehint CDATA #IMPLIED\n"; // just for compatibility with raw copied data
		ofs << L"\t>\n";
		ofs << L"]>\n\n";

		StartElem(_T("clipboard"));
		Attr(_T("closureversion"), version_string);
		Attr(_T("acceptingkind"), CComBstrObj( acceptingkinds));

		for( CComObjPtrVector<IMgaFolder>::iterator j = parentless_folders.begin(); j != parentless_folders.end(); ++j )
		{
			CComBSTR path;

			CComObjPtr<IMgaFolder> parent;
			COMTHROW( (*j)->get_ParentFolder( PutOut( parent)));
			if ( parent) // it has a folder parent
				COMTHROW( parent->get_AbsPath( &path));
			// else: a parentless_folder is the rootfolder, so the path = "";
			
			//CopyTo( path, m_curTopPath);// m_curTopPath will be used to dump the relative path in "closurename" attr

			Dump( *j);
		}

		for( CComObjPtrVector<IMgaFCO>::iterator i = parentless_fcos.begin() ; i != parentless_fcos.end(); ++i )
		{
			CComBSTR path;

			CComObjPtr<IMgaModel> parent;
			COMTHROW( (*i)->get_ParentModel( PutOut( parent)));
			if ( parent) // it has a model parent
				COMTHROW( parent->get_AbsPath( &path));
			else // it may be contained by a folder
			{
				CComObjPtr<IMgaFolder> parent;
				COMTHROW( (*i)->get_ParentFolder( PutOut( parent)));
				if ( parent) // it has a folder parent
					COMTHROW( parent->get_AbsPath( &path));
				else // what the heck! (a parentless fco cannot be the rootfolder)
					ASSERT(0);
			}


			//CopyTo( path, m_curTopPath);// m_curTopPath will be used to dump the relative path in "closurename" attr

			Dump( *i); 
		}
		
		EndElem();

		DoneDump(false);
	}
	COMCATCH( DoneDump(true); )

	m_selFcos.clear();
	m_selFolders.clear();
}

void CMgaDumper::DumpConnDetails( CComObjPtr<IMgaConnection> connection)
{
	const TCHAR * role_attr []       = { _T("smart0Role=")         , _T("smart1Role=")         };
	const TCHAR * targetGUID_attr [] = { _T("smart0TargetGUID=")   , _T("smart1TargetGUID=")   };
	const TCHAR * target_attr []     = { _T("smart0Target=")       , _T("smart1Target=")       };
	const TCHAR * refchainGUID_attr[]= { _T("smart0RefChainGUID=") , _T("smart1RefChainGUID=") };
	const TCHAR * refchain_attr []   = { _T("smart0RefChain=")     , _T("smart1RefChain=")     };
	const TCHAR * isbound_attr []    = { _T("smart0IsBound=")      , _T("smart1IsBound=")      };

	std::tstring array[2]; // { source_info, destin_info };
	CComObjPtrVector<IMgaConnPoint> cps;
	COMTHROW( connection->get_ConnPoints( PutOut( cps)));

	if( 2 == cps.size())
	{
		for( unsigned int i = 0; i < cps.size(); ++i)
		{
			CComBSTR role_bstr;std::tstring role_str;
			COMTHROW( cps[i]->get_ConnRole( &role_bstr));
			int w = 0; // which?
			if( role_bstr == CComBSTR(L"dst")) w = 1; // otherwise will fill arr[0]
			CopyTo( role_bstr, role_str);
			
			array[w] += std::tstring( role_attr[w]) + _T("\"") + role_str + _T("\" ");

			CComObjPtr<IMgaFCO> target;
			COMTHROW( cps[i]->get_Target(PutOut(target)) );

			if( target == NULL )
				HR_THROW(E_INVALID_MGA);

			CComBSTR guid;
			COMTHROW( target->get_RegistryValue( CComBSTR(GLOBAL_ID_STR), &guid));
			if( guid != 0 && guid.Length() == GLOBAL_ID_LEN) // valid
			{
				std::tstring guid_str; CopyTo( guid, guid_str);
				array[w] += std::tstring( targetGUID_attr[w]) + _T("\"") + guid_str + _T("\" ");
			}

			CComBSTR nameBstr;
			COMTHROW( connection->get_AbsPath( &nameBstr));
			CopyTo( nameBstr, m_currAbsPath); // will be used by the DumpConnPoint also
				
			nameBstr.Empty(); 
			std::tstring t_name;
			COMTHROW( target->get_AbsPath( &nameBstr));
			CopyTo( nameBstr, t_name);

			//Attr("closure2target", makeRel( t_name, m_currAbsPath));
			std::tstring relpath_to_end = makeRel( t_name, m_currAbsPath);
			ASSERT( relpath_to_end.substr(0, 4) == _T("/@.."));
			// the path to connection end is calculated relative to the connection
			// so if we cut off the first node, then it will be relative to the container
			array[w] += std::tstring( target_attr[w]) + _T("\"") + relpath_to_end.substr(4) + _T("\" ");

			CComObjPtrVector<IMgaFCO> refs;
			COMTHROW( cps[i]->get_References(PutOut(refs)) );

			std::tstring guidrefs = DumpGUIDRefs( refs);
			if( !guidrefs.empty()) array[w] += std::tstring( refchainGUID_attr[w]) + _T("\"") + guidrefs + _T("\" ");

			std::tstring chain;
			for( CComObjPtrVector<IMgaFCO>::iterator j = refs.begin(); j != refs.end(); ++j)
			{
				CComBSTR name;
				COMTHROW( (*j)->get_AbsPath( &name));
				std::tstring nm;
				CopyTo( name, nm);

				if( !chain.empty() )
					chain += ' ';

				std::tstring rel_path_to = makeRel( nm, m_currAbsPath);
				ASSERT( rel_path_to.substr( 0, 4) == _T("/@.."));
				// rel path converted from relative to the m_currAbsPath to relative to the container
				chain += rel_path_to.substr(4);
			}

			array[w] += std::tstring( refchain_attr[w]) + _T("\"") + chain + _T("\" ");
			
			CComObjPtr<IMgaFCO> base;
			COMTHROW( connection->get_DerivedFrom(PutOut(base)));
			if(base) {
				short stat;
				COMTHROW( connection->CompareToBase( cps[i], &stat));
				if(!stat) {
					//Attr("isbound", "yes" );
					array[w] += std::tstring( isbound_attr[w]) + _T("\"yes\" ");
				}
			}
		}

		if( !array[0].empty()) ofs << " " << array[0] << " "; //the attr names are already in the string
		if( !array[1].empty()) ofs << " " << array[1] << " ";
	}
}
