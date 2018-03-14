//###############################################################################################################################################
//
//	Meta and Builder Object Network V2.0 for GME
//	BON2Component.cpp
//
//###############################################################################################################################################

/*
	Copyright (c) Vanderbilt University, 2000-2004
	ALL RIGHTS RESERVED

	Vanderbilt University disclaims all warranties with regard to this
	software, including all implied warranties of merchantability
	and fitness.  In no event shall Vanderbilt University be liable for
	any special, indirect or consequential damages or any damages
	whatsoever resulting from loss of use, data or profits, whether
	in an action of contract, negligence or other tortious action,
	arising out of or in connection with the use or performance of
	this software.
*/
#include "StdAfx.h"
#include "BON2Component.h"

#include "logger.h"

#include "string"

#include "Dumper.h"
#include "NameSpecDlg.h"
#include "globals.h"
Globals global_vars;
NameSpecDlg * dlg;

namespace BON
{

//###############################################################################################################################################
//
// 	C L A S S : BON::Component
//
//###############################################################################################################################################

Component::Component()
	: m_bIsInteractive( false )
{
}

Component::~Component()
{
	if ( m_project ) {
		m_project->finalizeObjects();
		finalize( m_project );
		m_project = NULL;
	}
}

// ====================================================
// This method is called after all the generic initialization is done
// This should be empty unless application-specific initialization is needed

void Component::initialize( Project& project )
{
	// ======================
	// Insert application specific code here
}

// ====================================================
// This method is called before the whole BON2 project released and disposed
// This should be empty unless application-specific finalization is needed

void Component::finalize( Project& project )
{
	// ======================
	// Insert application specific code here
		m_entities.clear();
		m_relations.clear();
		m_equivRelations.clear();
		//m_toBeDeletedEntities.clear();
		m_equivBag.clear();
		m_realObj.clear();

		Sheet::m_BON_Project_Root_Folder = (Folder)NULL; // crucial!!!

		if ( m_sheet != 0)  
		{ 
			TO( "Internal error: Merged Paradigm Sheet object still exists."); 
			delete m_sheet; 
			m_sheet = 0; 
		}
}


void Component::scanSubModels( const Model& model)
{
	std::set<Model> models = model->getChildModels();
	std::set<Model>::iterator i = models.begin();
	for( ; i != models.end(); ++i)
		scanSubModels( *i);
}


void Component::scanModels( const Model& model, const Folder& parent)
{
  scanSubModels(model);
  entityBuilder(model, parent);
}


void Component::scanSubFolders( const std::set<Folder>& subfolders, std::set<Folder>& result)
{
	if (subfolders.empty()) return;

	result.insert( subfolders.begin(), subfolders.end());
	std::set<Folder>::const_iterator sub_it = subfolders.begin();
	for( ; sub_it != subfolders.end(); ++sub_it)
		scanSubFolders( (*sub_it)->getChildFolders(), result);
}


void Component::scanProject( Project& project)
{
	try
	{
		Folder rf = project->getRootFolder();
		Sheet::m_BON_Project_Root_Folder = rf;

		std::set<Folder> folders;
		folders.insert( rf); // including root folder
		std::set<Folder> subfolders = rf->getChildFolders(); // scan folders
		folders.insert( subfolders.begin(), subfolders.end());
		
		std::set<Folder>::iterator folder_it = folders.begin();
		for( ; folder_it != folders.end(); ++folder_it)
		{
			std::set<Model> objs = (*folder_it)->getChildModels();
			std::set<Model>::iterator i = objs.begin();
			for( ; i != objs.end(); ++i)
				scanModels( *i, *folder_it); // scan all root models
		}

		std::set<Folder> library_rf = project->getLibraries(); // get all libraries
		std::set<Folder>::iterator lib_rf_it = library_rf.begin();
		for( ; lib_rf_it != library_rf.end(); ++lib_rf_it)
		{
			folders.clear();
			scanSubFolders( (*lib_rf_it)->getChildFolders(), folders); // get all subfolders of a library root folder
			folders.insert( *lib_rf_it); // insert the library root folder too
			for( folder_it = folders.begin(); folder_it != folders.end(); ++folder_it)
			{
				std::set<Model> objs = (*folder_it)->getChildModels();
				std::set<Model>::iterator i = objs.begin();
				for( ; i != objs.end(); ++i)
					scanModels( *i, *lib_rf_it); // scan all library contained models, with the library rootfolder as its registry information holder
			}
		}
	}
	catch(...)
	{
		global_vars.err << MSG_ERROR << "Exception thrown during project scanning.\n";
	}
}


void Component::initMembers( Project& project)
{
	m_projectName = project->getName();

	CComPtr<IMgaProject> mgaProject = project->getProjectI().p;
	CComBSTR    connString;
	BONCOMTHROW(mgaProject->get_ProjectConnStr(&connString));
	std::string path = Util::Copy( connString);
	std::string::size_type e = path.rfind('\\');
	if( e != std::string::npos && path.substr(0, 4) == "MGA=") m_dir = path.substr(4, e-3); // cut leading "MGA=" , preserve tailing '\\'

	m_entities.clear();
	//m_toBeDeletedEntities.clear();
	m_relations.clear();
	m_equivRelations.clear();
	m_realObj.clear();

	m_sheet = 0;
}


void Component::createSheet()
{
	//
	//TODO : create your own Sheet instance (should be a derived class from the Sheet)
	//       store the pointer in the m_sheet member
	m_sheet = Dumper::getInstance();
}	


bool Component::populateSheet( Project& project )
{
	m_sheet->setProject( project );

	bool error = false;

	Entity_Iterator it_1( m_entities.begin());

	Any * new_elem = 0;

	for( ; it_1 != m_entities.end(); ++it_1 )
	{
		if ( it_1->isDeleted()) continue;

		FCO fco( it_1->getPointer());
		FCO resp( it_1->getRespPointer());
		Folder parent_folder( it_1->getParentFolder());
		Folder orig_parent_folder( parent_folder);

		if( parent_folder->isInLibrary()) // element's parent is in a library?
		{
			// This means we face a nested library, whose rootfolder is also
			// write protected, meaning that it's registry can't be written.
			// We will use the main rootfolder instead
			parent_folder = project->getRootFolder();
		}

		if ( !fco) global_vars.err << MSG_ERROR << "Populate sheet: tries to create Rep with empty FCO.\n";
		else
		{
			new_elem = 0;
			if ( fco->getObjectMeta().name() == "FCO" )
				new_elem = m_sheet->createFcoRep( fco, resp);
			else if ( fco->getObjectMeta().name() == "Atom" )
				new_elem = m_sheet->createAtomRep( fco, resp);
			else if ( fco->getObjectMeta().name() == "Model" )
				new_elem = m_sheet->createModelRep( fco, resp);
			else if ( fco->getObjectMeta().name() == "Connection" )
				new_elem = m_sheet->createConnectionRep( fco, resp);
			else if ( fco->getObjectMeta().name() == "Set" )
				new_elem = m_sheet->createSetRep( fco, resp);
			else if ( fco->getObjectMeta().name() == "Reference" )
				new_elem = m_sheet->createReferenceRep( fco, resp);
			else if ( fco->getObjectMeta().name() == "Folder" )
				new_elem = m_sheet->createFolderRep( fco, resp);//(resp != BON::FCO())?resp:fco);
			else if ( fco->getObjectMeta().name() == "Aspect" )
				new_elem = m_sheet->createAspectRep( fco, resp);//(resp != BON::FCO())?resp:fco);
			else if ( fco->getObjectMeta().name() == "Constraint" )
				new_elem = m_sheet->createConstraintRep( fco);
			else if ( fco->getObjectMeta().name() == "ConstraintFunc" )
				new_elem = m_sheet->createConstraintFuncRep( fco);
			else if ( fco->getObjectMeta().name() == "BooleanAttribute" )
				new_elem = m_sheet->createBoolAttributeRep( fco);
			else if ( fco->getObjectMeta().name() == "EnumAttribute" )
				new_elem = m_sheet->createEnumAttributeRep( fco);
			else if ( fco->getObjectMeta().name() == "FieldAttribute" )
				new_elem = m_sheet->createFieldAttributeRep( fco);
			else 
			{ 
				global_vars.err << MSG_ERROR << "Not inserted into the sheet: " << fco << " of kind: " << fco->getObjectMeta().name() << "\n";
				error = true;
			}
			if (new_elem)
			{
				new_elem->setParentFolder( parent_folder, orig_parent_folder); 
				if ( m_equivBag.find( fco) != m_equivBag.end())
				{
					new_elem->setEquivPeers( m_equivBag[ fco]);
					new_elem->setDisplayedName( it_1->getDispName());
				}
			}
		}
	} // for m_entities

	if (error) 
	{
		global_vars.err << MSG_ERROR << "Internal error during creation of entities. Exiting\n";
		return false;
	}

	Relation_Iterator rel_it = m_relations.begin();
	for( ; rel_it != m_relations.end(); ++rel_it)
	{
		Relation::OPER_TYPE oper = rel_it->getOperation();
		if ( oper == Relation::INHERITANCE_OP ||
				oper == Relation::INT_INHERITANCE_OP ||
				oper == Relation::IMP_INHERITANCE_OP)
			inheritancesManager( *rel_it);
		else if ( oper == Relation::ASSOCIATION_OP)
			associationClassManager( *rel_it);
		else if ( oper == Relation::REFER_TO_OP)
			refersToManager( *rel_it);
		else if ( oper == Relation::SET_MEMBER_OP)
			setMemberManager( *rel_it);
		else if ( oper == Relation::CONTAINMENT_OP)
			containmentManager( *rel_it);
		else if ( oper == Relation::FOLDER_CONTAINMENT_OP)
			folderContainmentManager( *rel_it);
		else if ( oper == Relation::HAS_ASPECT_OP)
			hasAspectManager( *rel_it);
		else if ( oper == Relation::ASPECT_MEMBER_OP)
			aspectMemberManager( *rel_it);
		else if ( oper == Relation::HAS_CONSTRAINT_OP)
			hasConstraintManager( *rel_it);
		else if ( oper == Relation::HAS_ATTRIBUTE_OP)
			hasAttributeManager( *rel_it);
	}

	if (error) 
	{
		global_vars.err << MSG_ERROR << "Internal error during the relation establishment. Exiting\n";
		return false;
	}

	return true;
}


bool Component::executeSheet()
{
	bool success = false;
	try 
	{
		//CRUCIAL step: the role names may be short or long form
		success = m_sheet->finalize();
		
		if (success) 
			success = m_sheet->build();
		else
			global_vars.err << MSG_ERROR << "Internal error: Sheet::finalize() returned with error\n";
	}
	catch( MON::Exception p) 
	{
		global_vars.err << MSG_ERROR << p.getKind() << " exception caught during executeSheet()\n";
		success = false;
	}
	catch( std::string exc_str) 
	{
		global_vars.err << MSG_ERROR << "Something went wrong in executeSheet. Msg: " << exc_str << "\n";
		success = false;
	}
	catch(...) 
	{
		global_vars.err << MSG_ERROR << "Something went wrong in executeSheet\n";
		success = false;
	}
	return success;
}


void Component::deleteSheet()
{
	if ( m_sheet) delete m_sheet;
	m_sheet = 0;
}


// ====================================================
// This is the main component method for Interpereters and Plugins.
// May also be used in case of invokeable Add-Ons
void Component::invokeEx( Project& project, FCO& currentFCO, const std::set<FCO>& setSelectedFCOs, long lParam )
{
	project->setAutoCommit(false);
	project->beginOnly(TRANSACTION_GENERAL);
	// ======================
	// Insert application specific code here
	global_vars.silent_mode = (lParam & GME_SILENT_MODE) != 0;
	global_vars.silent_mode |= (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	global_vars.skip_paradigm_register |= (lParam & GME_EMBEDDED_START) != 0;
	//global_vars.silent_mode = true;
	initMembers( project);

	std::string temp;
	if ( !Sheet::makeValidParadigmName( m_projectName, temp))
	{
		if( !global_vars.silent_mode)
		{
			project->consoleMsg("[MetaInterpreter] Invalid or empty paradigm name (Rootfolder name). Please remove spaces and the special characters, except '.' and '_'. Using: \"" + temp + "\".", MSG_ERROR);
			CString msg = "Invalid or empty paradigm name: \"";
			msg += (m_projectName + "\". Continue using: \"" + temp + "\"?").c_str();
			if( AfxMessageBox( (LPCTSTR)msg, MB_YESNO | MB_ICONWARNING) != IDYES)
			{
				project->consoleMsg("[MetaInterpreter] Intepretation stopped by the user.", MSG_NORMAL);
				project->abortOnly();
				return;
			}
		}
	}
	if ( Dumper::selectOutputFiles( project, m_projectName, m_dir) > 1)
	{
		if (!global_vars.silent_mode)
			project->consoleMsg("[MetaInterpreter] Output file name selection cancelled by the user or other file operation failed.", MSG_NORMAL);
		project->abortOnly();
		return;
	}

	Dumper::selectOptions( project);

	global_vars.err.m_proj = project;

	CWaitCursor wait;

	CTime theTime = CTime::GetCurrentTime();
	CString s = theTime.Format("%#c");
	s = "METAINTERPRETER LOG STARTED " + s;

	global_vars.err.open( global_vars.err_file_name.c_str(), std::ios_base::out);

	global_vars.err << MSG_NORMAL << std::string( s.GetLength(), '=') << "\n";
	global_vars.err << (LPCTSTR) s << "\n";
	global_vars.err << MSG_NORMAL << std::string( s.GetLength(), '=') << "\n";

	try {
		
		scanProject( project);

		CHECK();

		m_entities.sort();
		m_equivRelations.sort();			// tries to ensure the same order of handling the equivalences

		selectFromSameAspectsFolders();		// selects names for the equivalent objects (designates the nameresp pointers)
		proxyFinder();						// replaces proxies with the real objects
		equivalenceFinder();				// merges the equivalent objects (the relations they are part of)
		removeProxiesAndEquiv();
		bool cont = nameSelector();

		while ( !checkForProxies()) { }
		
		bool do_at_all = true;
		if ( do_at_all)
		{
			if ( Sheet::checkInstance())
				global_vars.err << MSG_ERROR << "Internal error: merged paradigm sheet already exists\n";

			global_vars.dmp.open( global_vars.xmp_file_name.c_str(), std::ios_base::out);

			createSheet();

			bool success = false;
			try 
			{

				success = populateSheet( project );
				if ( success)
					success = executeSheet();
				else
					global_vars.err << MSG_ERROR << "Internal error: PopulateSheet returned with error\n";

			}
			catch( MON::Exception p)
			{
				global_vars.err << MSG_ERROR << "Internal error: " << p.getKind() << " exception caught during populate/execute\n";
			}
			catch( std::string exc_str)
			{
				global_vars.err << MSG_ERROR << "Internal error: Exception caught during populate/execute. Msg: " << exc_str << "\n";
			}
			catch(...)
			{
				global_vars.err << MSG_ERROR << "Internal error: Exception caught during populate/execute.\n";
			}

			if ( !Sheet::checkInstance())
				global_vars.err << MSG_ERROR << "Internal error: Merged Sheet object doesn't exist\n";
			
			deleteSheet();
			
			global_vars.dmp.close();
			if (success)
			{
				project->commit();
				Dumper::registerIt( global_vars.xmp_file_name);
			}
		}
		else
			global_vars.err << MSG_ERROR << "Exited before populating the Merged Paradigm Sheet. Proxy or other problem may exist.\n";
		
	}	catch (...)	
	{
		global_vars.err << MSG_ERROR << "Internal error: Exception thrown by Component.\n";
		deleteSheet();
	}

	if ( Sheet::checkInstance())
		global_vars.err << MSG_ERROR << "Internal error: Merged Paradigm Sheet object still exists\n";

	global_vars.err << MSG_NORMAL << "\nEND OF LOG\n";
	global_vars.err.flushit();
	global_vars.err.close();
	global_vars.err.m_proj = (Project) 0;
	project->commit();
	CComPtr<IMgaProject> pproject = (IMgaProject*)project->getProjectI();
	project = NULL;
	pproject->CommitTransaction();
}

// ====================================================
// This is the obsolete component interface
// This present implementation either tries to call InvokeEx, or does nothing except of a notification

void Component::invoke( Project& project, const std::set<FCO>& setModels, long lParam )
{
	#ifdef SUPPORT_OLD_INVOKE
		Object focus;
		invokeEx( project, focus, setModels, lParam );
	#else
		if ( m_bIsInteractive )
			AfxMessageBox("This BON2 Component does not support the obsolete invoke mechanism!");
	#endif
}


// ====================================================
// GME currently does not use this function
// You only need to implement it if other invokation mechanisms are used
void Component::objectInvokeEx( Project& project, Object& currentObject, const std::set<Object>& setSelectedObjects, long lParam )
	{
		if ( m_bIsInteractive )
			AfxMessageBox("This BON2 Component does not support objectInvokeEx method!");
	}

// ====================================================
// Implement application specific parameter-mechanism in these functions

Util::Variant Component::getParameter( const std::string& strName )
{
	// ======================
	// Insert application specific code here

	return Util::Variant();
}

void Component::setParameter( const std::string& strName, const Util::Variant& varValue )
{
	if (strName == "SkipParadigmRegister")
	{
		if (varValue.type() == Util::Variant::VT_Boolean)
		{
			global_vars.skip_paradigm_register = static_cast<bool>(varValue);
		}
		else
		{
			global_vars.skip_paradigm_register = true;
		}
	}
}

#ifdef GME_ADDON

// ====================================================
// If the component is an Add-On, then this method is called for every Global Event

void Component::globalEventPerformed( globalevent_enum event )
{
	// ======================
	// Insert application specific code here
}

// ====================================================
// If the component is an Add-On, then this method is called for every Object Event

void Component::objectEventPerformed( Object& object, unsigned long event, VARIANT v )
{
	// ======================
	// Insert application specific code here
}

#endif // GME_ADDON


}; // namespace BON
