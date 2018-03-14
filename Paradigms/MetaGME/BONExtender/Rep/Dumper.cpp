#include "stdafx.h"

#include "Dumper.h"
#include "MakeVisitor.h"

#include "algorithm"
#include <afxdlgs.h>

#include "globals.h"
extern Globals global_vars;

int ind; // indent

#include "SelConf.h"
#include "OptionsDlg.h"
#include "MakeVisitor.h"

const char* Dumper::m_strBonExtenderOptions           = "BonExtender_Options";
const char* Dumper::Yes_Str                           = "yes";
const char* Dumper::Only_Str                          = "only";
const char* Dumper::No_Str                            = "no";
const char* Dumper::m_strGenVisitor                   = "GenerateVisitor";
const char* Dumper::m_strVisitorSignature             = "VisitorSignature";
const char* Dumper::m_strGenInitMethod                = "GenerateInitMethod";
const char* Dumper::m_strGenFiniMethod                = "GenerateFiniMethod";
const char* Dumper::m_strGenAcceptWithTraversal       = "GenerateAcceptMethodWithTraversal";
const char* Dumper::m_strGenAcceptSpecialized         = "GenerateAcceptMethodSpecialized";
const char* Dumper::m_strSpecAcceptRetVal             = "SpecializedAcceptReturnValue";
const char* Dumper::m_strVCVersion6                   = "VCVersion6";
const char* Dumper::m_strTemplateGetter               = "TemplateGetter";
const char* Dumper::m_strOutputInSeparateFile         = "OutputInSeparateFile";
const char* Dumper::m_strTgtNamespace                 = "GenerateElementsIntoNamespace";

bool Dumper::m_bParsePrev = false;
bool Dumper::m_bGenInit = false;
bool Dumper::m_bGenFinalize = false;
bool Dumper::m_bGenAcceptTrave = false;
bool Dumper::m_bGenAcceptSpeci = false;
bool Dumper::m_bGenTemplates = false;
bool Dumper::m_bGenTemplVersion6 = true;
bool Dumper::m_bGenRegular = true;
bool Dumper::m_bGenVisitor = false;
int  Dumper::m_iVisitSign = 0;
int  Dumper::m_iSpecAcceptRetVal = 0;
bool Dumper::m_bSafetyBackup = true;



Sheet * Dumper::getInstance()
{ 
	if (!Sheet::m_theOnlyInstance) 
	{
		Sheet::m_theOnlyInstance = new Dumper();
		return Sheet::m_theOnlyInstance;
	}
	return Sheet::m_theOnlyInstance;
}


Dumper::Dumper(): Sheet() 
{ 
}


Dumper::~Dumper()
{ 
}


bool Dumper::findInFolders( FCO * fco)
{
	std::vector<FolderRep*>::iterator it( m_folderList.begin());
	while( it != m_folderList.end() && !(*it)->isFCOContained( fco))
		++it;
	return it != m_folderList.end();
}


void Dumper::inheritMoReSeCoFolAsp()
{
	// task 1
	ModelRep_Iterator mod_iter( m_modelList.begin());
	for( ; mod_iter != m_modelList.end(); ++mod_iter)
		(*mod_iter)->inherit();

	// task 2
	// the refs first because the connections need the refs !
	ReferenceRep_Iterator ref_iter( m_refList.begin());
	for( ; ref_iter != m_refList.end(); ++ref_iter)
		(*ref_iter)->inherit();

	ref_iter = m_refList.begin();
	for( ; ref_iter != m_refList.end(); ++ref_iter)
		if (!(*ref_iter)->finalize())
		{
			//global_vars.err << (*ref_iter)->getName() << " is referring to too many kinds\n";
		}

	// task 3
	ConnectionRep_Iterator conn_iter( m_connList.begin());
	for( ; conn_iter != m_connList.end(); ++conn_iter)
		(*conn_iter)->inherit();

	// task 4 - for all sets on the sheet
	SetRep_Iterator set_iter( m_setList.begin());
	for( ; set_iter != m_setList.end(); ++set_iter)
		(*set_iter)->extendMembership();

	// task 5 folder element hierarchies
	std::vector<FolderRep*>::iterator folder_iter( m_folderList.begin());
	for( ; folder_iter != m_folderList.end(); ++folder_iter)
		(*folder_iter)->extendMembership();
}


std::string Dumper::getNamespaceName()
{
	//std::string t = m_projName + "_BON";
	//Any::convertToValidName( t);
	return m_validProjName + "_BON";
}


void Dumper::createMethods()
{
	//std::string t = m_projName;
	//Any::convertToValidName( t);

	MakeVisitor m_visitor( m_validProjName);

	// connections
	{std::vector<ConnectionRep*>::const_iterator it_cn = m_connList.begin();
	if ( it_cn != m_connList.end())
	{
		for( ; it_cn != m_connList.end(); ++it_cn)
			(*it_cn)->calcLCD();
		for( it_cn = m_connList.begin() ; it_cn != m_connList.end(); ++it_cn)
		{
			if ( (*it_cn)->isToBeEx())
			{
				(*it_cn)->createEndGetters();
				(*it_cn)->prepare();

				//if ( !(*it_cn)->isAbstract())
				m_visitor.addEntity( Any::CONN, ClassAndNamespace::makeIt(*it_cn));
			}
		}
	}}

	// folders
	{
	std::vector<FolderRep*>::const_iterator it_b = m_folderList.begin();
	if ( it_b != m_folderList.end())
		for( ; it_b != m_folderList.end(); ++it_b)
		{
			if ( (*it_b)->isToBeEx())
			{
				(*it_b)->prepare();
				(*it_b)->createMethods();
				
				m_visitor.addEntity( Any::FOLDER, ClassAndNamespace::makeIt(*it_b));
			}
		}
	}
	// fcos
	{
	std::vector<FcoRep*>::const_iterator it_b = m_fcoRepList.begin();
	if ( it_b != m_fcoRepList.end())
		for( ; it_b != m_fcoRepList.end(); ++it_b)
		{
			if ( (*it_b)->isToBeEx())
			{
				(*it_b)->prepare();

				//if ( !(*it_b)->isAbstract())
					m_visitor.addEntity( Any::FCO_REP, ClassAndNamespace::makeIt(*it_b));
			}
		}
	}

	// atoms
	{
	std::vector<AtomRep*>::const_iterator it_b = m_atomList.begin();
	if ( it_b != m_atomList.end())
		for( ; it_b != m_atomList.end(); ++it_b)
		{
			if ( (*it_b)->isToBeEx())
			{

				(*it_b)->prepare();

				//if ( !(*it_b)->isAbstract())
					m_visitor.addEntity( Any::ATOM, ClassAndNamespace::makeIt(*it_b));
			}
		}
	}

	// sets
	{
	std::vector<SetRep*>::const_iterator it_c = m_setList.begin();
	if ( it_c != m_setList.end())
		for( ; it_c != m_setList.end(); ++it_c)
		{
			if ( (*it_c)->isToBeEx())
			{
				(*it_c)->prepare();
				(*it_c)->createMethods();

				//if ( !(*it_c)->isAbstract())
					m_visitor.addEntity( Any::SET, ClassAndNamespace::makeIt(*it_c));
			}
		}
	}

	// references
	{
	std::vector<ReferenceRep*>::const_iterator it_ref = m_refList.begin();
	if ( it_ref != m_refList.end())
		for( ; it_ref != m_refList.end(); ++it_ref)
		{
			if ( (*it_ref)->isToBeEx())
			{
				(*it_ref)->prepare();
				(*it_ref)->createMethods();

				//if ( !(*it_ref)->isAbstract())
					m_visitor.addEntity( Any::REF, ClassAndNamespace::makeIt(*it_ref));
			}
		}
	}

	// models
	{
	std::vector<ModelRep*>::const_iterator it_m = m_modelList.begin();
	if ( it_m != m_modelList.end())
		for( ; it_m != m_modelList.end(); ++it_m)
		{
			if ( (*it_m)->isToBeEx())
			{
				(*it_m)->prepare();
				(*it_m)->createMethods();

				//if ( !(*it_m)->isAbstract())
					m_visitor.addEntity( Any::MODEL, ClassAndNamespace::makeIt(*it_m));
			}
		}
	}

	if ( m_bGenVisitor)
	{
		std::ofstream visitorh( global_vars.m_visitorHeaderFileName.c_str());
		std::ofstream visitors( global_vars.m_visitorSourceFileName.c_str());

		std::string mmm = m_visitor.dumpVisitorHeader();
		visitorh.write( mmm.c_str(), mmm.length());

		std::string nnn = m_visitor.dumpVisitorSource();
		visitors.write( nnn.c_str(), nnn.length());

		visitorh.close();
		visitors.close();
	}
}


void Dumper::dumpGlobals( std::vector<FCO*>& s)
{
	{
	std::vector<FolderRep*>::const_iterator it_b = m_folderList.begin();
	for( ; it_b != m_folderList.end(); ++it_b)
		if ( (*it_b)->isToBeEx())
			(*it_b)->dumpGlobals();
	}

	std::vector<FCO*>::iterator it = s.begin();
	for( ; it != s.end(); ++it)
	{
		if ( (*it)->isToBeEx())
			(*it)->dumpGlobals();
	}
}


void Dumper::dumpMain( std::vector<FCO*>& s)
{
	// folders
	{
	std::vector<FolderRep*>::const_iterator it_b = m_folderList.begin();
	if ( it_b != m_folderList.end())
		for( ; it_b != m_folderList.end(); ++it_b)
			if ( (*it_b)->isToBeEx())
				(*it_b)->doDump();
	}
	// all other
	std::vector<FCO*>::iterator it = s.begin();
	for( ; it != s.end(); ++it)
	{
		if ( (*it)->isToBeEx())
			(*it)->doDump();
	}
}

void Dumper::dumpGlobalMOF()
{
	std::string res;

	std::set<BON::Model>::iterator it = m_setOfParadigmSheets2.begin();
	for( ; it != m_setOfParadigmSheets2.end(); ++it)
	{
		BON::RegistryNode rn = (*it)->getRegistry();
		if( rn)
			res += Any::processMOFNode( rn, 1);

	}

	if( !res.empty())
	{
		DMP_H( "// Global " + Any::MOFStart_str + "\n\n");
		DMP_H( res);
		DMP_H( "// Global " + Any::MOFEnd_str + "\n\n\n");
	}
}

void Dumper::initOutFiles( std::vector<FCO*>& s, std::string& resu)
{
	if( global_vars.m_outputMethod == Globals::PERCLASS)
	{ 
		// first make a backup of every file ( move )
		
		std::vector<FolderRep*>::const_iterator it_b = m_folderList.begin();
		for( ; it_b != m_folderList.end(); ++it_b)
			if ( (*it_b)->isToBeEx())
			{
				(*it_b)->makeBackup();
			}
		
		std::vector<FCO*>::iterator it = s.begin();
		for( ; it != s.end(); ++it)
			if ( (*it)->isToBeEx())
			{
				(*it)->makeBackup();
			}

		{ // then open files for append
		std::vector<FolderRep*>::const_iterator it_b = m_folderList.begin();
		for( ; it_b != m_folderList.end(); ++it_b)
			if ( (*it_b)->isToBeEx())
			{
				(*it_b)->initOutH( resu);
				(*it_b)->initOutS();
			}
		}

		{
		std::vector<FCO*>::iterator it = s.begin();
		for( ; it != s.end(); ++it)
			if ( (*it)->isToBeEx())
			{
				(*it)->initOutH( resu);
				(*it)->initOutS();
			}
		}
	}
}

void Dumper::finiOutFiles( std::vector<FCO*>& s)
{
	{
	std::vector<FolderRep*>::const_iterator it_b = m_folderList.begin();
	for( ; it_b != m_folderList.end(); ++it_b)
		if ( (*it_b)->isToBeEx())
		{
			(*it_b)->finiOutH();
			(*it_b)->finiOutS();
		}
	}

	{
	std::vector<FCO*>::iterator it = s.begin();
	for( ; it != s.end(); ++it)
		if ( (*it)->isToBeEx())
		{
			(*it)->finiOutH();
			(*it)->finiOutS();
		}
	}
}


void Dumper::doDump()
{
	AnyLexicographicSort lex;
	std::sort( m_folderList.begin(),  m_folderList.end(), lex );
	std::sort( m_fcoRepList.begin(),  m_fcoRepList.end(), lex );
	std::sort( m_atomList.begin(), m_atomList.end(), lex );
	std::sort( m_refList.begin(), m_refList.end(), lex );
	std::sort( m_connList.begin(), m_connList.end(), lex );
	std::sort( m_setList.begin(), m_setList.end(), lex );
	std::sort( m_modelList.begin(), m_modelList.end(), lex );
	// attributes sort by FCO::prepare

	std::vector<FCO*> sorted = sortBasedOnLevels();

	selConf( sorted); // select configuration ->set the toBeDumped member to T/F

	CWaitCursor w;

	std::string short_name_header_file = shortFileName( global_vars.header_file_name);
	std::string capitalized = capitalizeString( cutExtension( short_name_header_file));

	std::string global_user_part1, global_user_part2;
	Dumper::getGlobalUserParts( global_user_part1, global_user_part2);
	
	std::string global_includes;
	initOutFiles( sorted, global_includes);

	DMP_S("#include \"stdafx.h\"\n");
	DMP_S("#include \"" + short_name_header_file + "\"\n");
	if( Dumper::m_bGenAcceptSpeci)
		DMP_S("#include \"" + shortFileName( global_vars.m_visitorHeaderFileName) + "\"\n");
	DMP_S("\n");

	//DMP_S("using namespace BON;\n\n");
	
	DMP_H("#ifndef " + capitalized + "_H\n");
	DMP_H("#define " + capitalized + "_H\n\n");
	DMP_H("#include \"BON.h\"\n#include \"BONImpl.h\"\n#include \"Extensions.h\"\n\n");

	if ( m_bGenTemplates)
	{
		DMP_H("//\n");
		DMP_H("// this dummy template parameter is needed by VC6 to deduct the return type\n");
		DMP_H("// VC7 users can modify this macro to a value like \"int _dummy = 0\"\n");
		DMP_H("// TDP standing for TEMPLATE DUMMY PAR\n");

		if ( m_bGenTemplVersion6)
		{
			DMP_H("//#define TDP int _dummy = 0			// recommended for VC7 users\n");
			DMP_H("#define TDP const T & _dummyComp		// recommended for VC6 users\n");
		}
		else
		{
			DMP_H("#define TDP int _dummy = 0			// recommended for VC7 users\n");
			DMP_H("//#define TDP const T & _dummyComp		// recommended for VC6 users\n");
		}
		DMP_H("\n// sample less-than comparator (functor)\n");
		DMP_H("// class Lt { public: bool operator()( const BON::Object& x, const BON::Object& y) { return x->getName().compare( y->getName()) < 0; } };\n");
		DMP_H("// can be used as follows\n");
		DMP_H("// std::set< MyObj, Lt > myObjs = c->getMyObj( Lt()); // in VC6\n");
		DMP_H("// std::set< MyObj, Lt > myObjs = c->getMyObj();      // in VC7.1 and above\n");
		DMP_H("// beware: must be a partial ordering (implemented like a 'less than') having the following three properties: \n");
		DMP_H("// A) x < y => !(y < x), B) x < x must be false C) x < y && y < z => x < z\n");


		DMP_H("\n");
	}

	DMP_H( Any::m_startUPToken + "\n" + global_user_part1 + Any::m_endUPToken + "\n\n");

	//global_vars.m_namespace_name = getNamespaceName();

	//DMP_H("namespace " + global_vars.m_namespace_name + "\n{\n\n");
	if( Dumper::m_bGenAcceptSpeci)
		DMP_H("namespace " + global_vars.m_namespace_name + " { class " + MakeVisitor::getVisitorName( m_validProjName) + "; }\n");

	DMP_S("\nnamespace BON\n{\n\n");

	//selConf( sorted); // select configuration ->set the toBeDumped member to T/F

	//CWaitCursor w;

	createMethods();
	dumpGlobals( sorted);

	DMP_H("\n\n");

	if( !global_includes.empty())
	{
		DMP_H( global_includes);
		DMP_H( "\n");
	}

	if( Dumper::m_bGenAcceptSpeci)
		DMP_H( "#include \"" +  shortFileName( global_vars.m_visitorHeaderFileName) + "\"\n\n");
	//	DMP_H("namespace " + global_vars.m_namespace_name + " { class " + MakeVisitor::getVisitorName( m_validProjName) + "; }\n");
	//if( Dumper::m_bGenAcceptSpeci && 0) // if special accept is generated then special visitor header included
	//	m_sStream << "#include \"" << shortFileName( global_vars.m_visitorHeaderFileName) << "\"" << std::endl;
	

	DMP_S("\n\n} // namespace BON\n\n");
	//DMP_H("\n}; // namespace " + global_vars.m_namespace_name + "\n\n"); // freshly introd'd
	// commented out
	//DMP_S("namespace " + global_vars.m_namespace_name + "\n{\n");
	
	dumpGlobalMOF();

	dumpMain( sorted);

	// commented out
	//DMP_H("\n}; // namespace " + global_vars.m_namespace_name + "\n\n");

	DMP_H( Any::m_startUPToken + "\n" + global_user_part2 + Any::m_endUPToken + "\n\n");

	DMP_H("#endif // " + capitalized + "_H\n");

	// commented out
	//DMP_S("\n}; // namespace " + global_vars.m_namespace_name + "\n\n");
	finiOutFiles( sorted);
}


void Dumper::showMultipleInheritance()
{
	FcoRep_Iterator it0( m_fcoRepList.begin());
	for( ; it0 != m_fcoRepList.end(); ++it0 )
	{
		(*it0)->showMultipleInheritance();
	}

	AtomRep_Iterator it1 = m_atomList.begin();
	for( ; it1 != m_atomList.end(); ++it1 )
	{
		(*it1)->showMultipleInheritance();
	}

	ModelRep_Iterator it2 = m_modelList.begin();
	for( ; it2 != m_modelList.end(); ++it2 )
	{
		(*it2)->showMultipleInheritance();
	}

	ConnectionRep_Iterator it3 = m_connList.begin();
	for( ; it3 != m_connList.end(); ++it3 )
	{
		(*it3)->showMultipleInheritance();
	}

	SetRep_Iterator it4 = m_setList.begin();
	for( ; it4 != m_setList.end(); ++it4 )
	{
		(*it4)->showMultipleInheritance();
	}

	ReferenceRep_Iterator it5 = m_refList.begin();
	for( ; it5 != m_refList.end(); ++it5 )
	{
		(*it5)->showMultipleInheritance();
	}
}


void Dumper::fetchConfigurationNames( std::vector< std::string>& res)
{
	BON::RegistryNode rn = m_BON_Project_Root_Folder2->getRegistry()->getChild("BonExtender/SavedConfigurations");
	if ( rn)
	{
		std::set<BON::RegistryNode> ch = rn->getChildren();
		std::set<BON::RegistryNode>::iterator i = ch.begin();
		for( ; i != ch.end(); ++i)
		{
			std::string one_config( (*i)->getName()); // previously getPath used

			if ( (*i)->getIntegerValue() == 1
				&& std::find( res.begin(), res.end(), one_config) == res.end()) // not found
				res.push_back( one_config);
		}
	}
}


void Dumper::selConf( std::vector<FCO*>& s)
{
	CWaitCursor wait;	
	std::vector< std::string > tmp,vec;
	fetchConfigurationNames( tmp);
	if ( std::find( tmp.begin(), tmp.end(), "Default") == tmp.end())
		vec.push_back( "Default");
	vec.insert( vec.end(), tmp.begin(), tmp.end()); // copy tmp to the end of vec

	SelConf sc( vec.size());
	sc.addConfigs( vec);

	int last_id;
	for( std::vector<FolderRep*>::const_iterator it_b = m_folderList.begin(); it_b != m_folderList.end(); ++it_b)
	{
		CString name = (*it_b)->getLStrictName().c_str();
		std::string regVal = (*it_b)->getMyRegistry()->getValueByPath( "/Configs/Default");

		if( global_vars.silent_mode)
			(*it_b)->toBeEx( regVal != "false");
		else
			last_id = sc.addEntry( name, 'f', 0, regVal != "false", "", *it_b);
	}
	
	for( std::vector< FCO *>::iterator it = s.begin(); it != s.end(); ++it)
	{
		CString name = (*it)->getLStrictName().c_str();
		const char kinds[] = "AMCSRF f";
		char kind = ' ';
		if( (int) (*it)->getMyKind() < sizeof(kinds)/sizeof(char))
			kind = kinds[(int) (*it)->getMyKind()];
		int clique = (*it)->getCliqueId( FCO::INTERFACE);
		std::string regVal = (*it)->getMyRegistry()->getValueByPath( "/Configs/Default");
		std::string rspVal = (*it)->getMyRegistry()->getValueByPath( "/Configs/Default/Resp");
		FCO* resp_ptr = (*it)->findRspPtr( rspVal);
		if( !resp_ptr) // valid info in Responsible registry?
		{
			rspVal = "";
			(*it)->getMyRegistry()->setValueByPath( "/Configs/Default/Resp", rspVal);
		}
		
		if( global_vars.silent_mode)
		{
			(*it)->toBeEx( regVal != "false");
			(*it)->setExtedAnc( resp_ptr);
		}
		else
		{
			last_id = sc.addEntry( name, kind, clique, regVal != "false", rspVal.c_str(), *it);
		}
	}

	if ( !global_vars.silent_mode && sc.DoModal() == IDOK)
	{
		wait.Restore();
		if ( sc.m_currSelConfigStr != "Default")
		{
			std::string path = "/BonExtender/SavedConfigurations/" + sc.m_currSelConfigStr;
			m_BON_Project_Root_Folder2->getRegistry()->setValueByPath( path, "1");
		}
	}	
}


bool Dumper::build()
{
	inheritMoReSeCoFolAsp();		// models, references, set, connections, folder and aspect membership
	//showMultipleInheritance();

	doDump();

	return true;
}


/*static*/ int Dumper::backup( const std::string& file_name)
{
	if ( existsFile( file_name.c_str()))
	{
		std::string backup = file_name + ".bak";
		int res = makeFileCopy( file_name.c_str(), backup.c_str());
		if ( res == 2)
		{
			TO("Could not read file during backup process: " + file_name);
			return 2;
		}
		else if ( res == 3)
		{
			TO("Cannot create backup file: " + backup);
			return 3;
		}
	}
	return 1;
}


/*static*/ int Dumper::selectOutputFiles( const std::string& proj_name_to_be_checked, BON::RegistryNode& optRn)
{
	std::string proj_name = proj_name_to_be_checked;
	Any::convertToValidName( proj_name);

	if( global_vars.m_namespace_name.empty())
		global_vars.m_namespace_name = proj_name + "_BON"; // default namespace provided first time


	global_vars.header_file_name            = proj_name + "BonX.h";;
	global_vars.source_file_name            = proj_name + "BonX.cpp";
	global_vars.m_visitorHeaderFileName     = proj_name + "Visitor.h";
	global_vars.m_visitorSourceFileName     = proj_name + "Visitor.cpp";
	global_vars.header_backup_name          = ""; //global_vars.header_file_name + ".bak";


	// load stored options from registry
	std::string _visitor = optRn->getChild( Dumper::m_strGenVisitor)->getValue();
	std::string _vissign = optRn->getChild( Dumper::m_strVisitorSignature)->getValue();
	std::string _initmtd = optRn->getChild( Dumper::m_strGenInitMethod)->getValue();
	std::string _finimtd = optRn->getChild( Dumper::m_strGenFiniMethod)->getValue();
	std::string _acceptT = optRn->getChild( Dumper::m_strGenAcceptWithTraversal)->getValue();
	std::string _acceptS = optRn->getChild( Dumper::m_strGenAcceptSpecialized)->getValue();
	std::string _acSRetV = optRn->getChild( Dumper::m_strSpecAcceptRetVal)->getValue();
	std::string _vcVers6 = optRn->getChild( Dumper::m_strVCVersion6)->getValue();
	std::string _templGt = optRn->getChild( Dumper::m_strTemplateGetter)->getValue();
	std::string _outSepF = optRn->getChild( Dumper::m_strOutputInSeparateFile)->getValue();
	std::string _tgtNmsp = optRn->getChild( Dumper::m_strTgtNamespace)->getValue();

	// set the values of the variables based on registry values, defaulting to the obvious/most common ones in some cases
	m_bGenVisitor       = _visitor == Yes_Str;
	m_iVisitSign        = _vissign == "1"? 1 : 0;
	m_bGenInit          = _initmtd == Yes_Str;
	m_bGenFinalize      = _finimtd == Yes_Str;
	m_bGenAcceptTrave   = _acceptT == Yes_Str;
	m_bGenAcceptSpeci   = _acceptS == Yes_Str;
	m_iSpecAcceptRetVal = _acSRetV == "1"? 1 : 0;
	m_bGenTemplVersion6 = _vcVers6 == Yes_Str;
	if( _templGt == Yes_Str)            { m_bGenTemplates = TRUE;       m_bGenRegular = TRUE;  }
	else if( _templGt == Only_Str)      { m_bGenTemplates = TRUE;       m_bGenRegular = FALSE; }
	else                                { m_bGenTemplates = FALSE;      m_bGenRegular = TRUE;  }
	global_vars.m_outputMethod     = _outSepF == Yes_Str ? Globals::PERCLASS : Globals::ALLTOGETHER;
	global_vars.m_namespace_name   = _tgtNmsp.empty() ? global_vars.m_namespace_name : _tgtNmsp; // overwrite the project namespace setting if option found
	
	if( !global_vars.silent_mode) // show dialog only in verbose mode
	{
		OptionsDlg opt;

		opt.m_headerName             = global_vars.header_file_name.c_str();
		opt.m_sourceName             = global_vars.source_file_name.c_str();
		opt.m_visitorHeaderName      = global_vars.m_visitorHeaderFileName.c_str();
		opt.m_visitorSourceName      = global_vars.m_visitorSourceFileName.c_str();
		opt.m_prevHeaderName         = opt.m_headerName + ".bak";

		// set the values of the variables based on registry values, defaulting to the obvious/most common ones in some cases
		opt.m_bVisitor          = m_bGenVisitor;
		opt.m_visitorSignature  = m_iVisitSign;
		opt.m_bInit             = m_bGenInit;
		opt.m_bFinalize         = m_bGenFinalize;
		opt.m_bAcceptTrave      = m_bGenAcceptTrave;
		opt.m_bAcceptSpeci      = m_bGenAcceptSpeci;
		opt.m_specAcceptRetVal  = m_iSpecAcceptRetVal;
		opt.m_version60         = m_bGenTemplVersion6;
		opt.m_whichStyle        = m_bGenRegular && m_bGenTemplates ? 2 : m_bGenTemplates?1:0;
		opt.m_methodOfOutput    = global_vars.m_outputMethod == Globals::PERCLASS ? 1 : 0;
		opt.m_namespaceName     = global_vars.m_namespace_name.c_str();

		if ( opt.DoModal() != IDOK) return 2;

		m_bParsePrev          = opt.m_bParse == TRUE;
		m_bGenVisitor         = opt.m_bVisitor == TRUE;
		m_iVisitSign          = opt.m_visitorSignature;
		m_bGenInit            = opt.m_bInit == TRUE;
		m_bGenFinalize        = opt.m_bFinalize == TRUE;
		m_bGenAcceptTrave     = opt.m_bAcceptTrave == TRUE;
		m_bGenAcceptSpeci     = opt.m_bAcceptSpeci == TRUE;
		m_iSpecAcceptRetVal   = opt.m_specAcceptRetVal;
		m_bGenTemplVersion6   = opt.m_version60 == TRUE;
		m_bGenRegular         = opt.m_whichStyle != 1; // 0 or 2
		m_bGenTemplates       = opt.m_whichStyle > 0;  // 1 or 2 ( = both)

		global_vars.m_outputMethod   = opt.m_methodOfOutput == 0 ? Globals::ALLTOGETHER: Globals::PERCLASS;
		global_vars.m_namespace_name = opt.m_namespaceName.IsEmpty() ? "std" : (LPCTSTR) opt.m_namespaceName;

		// save options to registry
		optRn->getChild( Dumper::m_strGenVisitor            )->setValue( m_bGenVisitor        ? Yes_Str : No_Str);
		optRn->getChild( Dumper::m_strVisitorSignature      )->setValue( m_iVisitSign == 1    ? "1" : "0");
		optRn->getChild( Dumper::m_strGenInitMethod         )->setValue( m_bGenInit           ? Yes_Str : No_Str);
		optRn->getChild( Dumper::m_strGenFiniMethod         )->setValue( m_bGenFinalize       ? Yes_Str : No_Str);
		optRn->getChild( Dumper::m_strGenAcceptWithTraversal)->setValue( m_bGenAcceptTrave    ? Yes_Str : No_Str);
		optRn->getChild( Dumper::m_strGenAcceptSpecialized  )->setValue( m_bGenAcceptSpeci    ? Yes_Str : No_Str);
		optRn->getChild( Dumper::m_strSpecAcceptRetVal      )->setValue( m_iSpecAcceptRetVal == 1 ? "1" : "0");
		optRn->getChild( Dumper::m_strVCVersion6            )->setValue( m_bGenTemplVersion6  ? Yes_Str : No_Str);
		optRn->getChild( Dumper::m_strTemplateGetter        )->setValue( m_bGenTemplates && m_bGenRegular ? Yes_Str : m_bGenTemplates ? Only_Str : No_Str);
		optRn->getChild( Dumper::m_strOutputInSeparateFile  )->setValue( global_vars.m_outputMethod == Globals::PERCLASS ? Yes_Str : No_Str);
		optRn->getChild( Dumper::m_strTgtNamespace          )->setValue( global_vars.m_namespace_name);

		global_vars.output_directory_name = "";
		int i = opt.m_headerName.ReverseFind('\\');
		if ( i != -1)
		{
			global_vars.output_directory_name = opt.m_headerName.Left( i+1);
		}

		global_vars.header_file_name = opt.m_headerName;
		global_vars.source_file_name = opt.m_sourceName;
		global_vars.err_file_name = global_vars.output_directory_name + proj_name + "BONExt.log";

		if (m_bGenVisitor)
		{
			global_vars.m_visitorHeaderFileName = opt.m_visitorHeaderName;
			global_vars.m_visitorSourceFileName = opt.m_visitorSourceName;
		}

		global_vars.header_backup_name = ""; // initial value, notifies the dumper NOT to search for UP (user part) declarations
		if ( m_bParsePrev)
		{
			if ( existsFile( (LPCTSTR) opt.m_prevHeaderName))
				global_vars.header_backup_name = opt.m_prevHeaderName;
			else
				TO( opt.m_prevHeaderName + " previously generated header file not found.");
		}

	}

	m_bSafetyBackup = true;
	if ( m_bSafetyBackup)
	{
		int res1( 1), res2( 1), res3( 1), res4( 1);
		res1 = backup( global_vars.header_file_name);
		res2 = backup( global_vars.source_file_name);

		if ( m_bGenVisitor)
		{
			res3 = backup( global_vars.m_visitorHeaderFileName);
			res4 = backup( global_vars.m_visitorSourceFileName);
		}
		if ( res1 != 1 || res2 != 1 || res3 != 1 || res4 != 1)
			return 2; // abort execution
	}

	return 1;
}

/*static*/ void Dumper::getGlobalUserParts( std::string& up1, std::string& up2)
{
	std::ifstream old_xmp_file;
	std::string old_f_name;
	old_f_name = global_vars.header_backup_name;
	if ( old_f_name.empty()) // means that no previous version existed or the user selected not to parse the old file
	{
		up1 = "// add your include files/class definitions here\n";
		up2 = "// add your additional class definitions here\n";
		return;
	}

	old_xmp_file.open( old_f_name.c_str() ,std::ios_base::in);
	if ( !old_xmp_file.is_open())
	{
		up1 = "// ERROR accessing file\n";
		up2 = "// ERROR accessing file\n";
		return;
	}

	std::string line;
	while( !old_xmp_file.eof() && line.substr(0, Any::m_startUPToken.length()) != Any::m_startUPToken) 
	{
		std::getline( old_xmp_file, line);
	}
	if ( !old_xmp_file.eof()) // found the start token starting at 1st character
	{
		std::getline( old_xmp_file, line);
		while( !old_xmp_file.eof() && line.substr( 0, Any::m_endUPToken.length()) != Any::m_endUPToken) 
		{
			up1 += line + "\n";
			std::getline( old_xmp_file, line);
		}
		if ( !old_xmp_file.eof()) // found the end token
		{
		}
		else
		{
			old_xmp_file.close();
			up1 = "// Ending token (" + Any::m_endUPToken + ") for first global user part not found\n";
			up2 = "// Error in first global user part\n";
			return;
		}
	}
	else
	{
		old_xmp_file.close();
		up1 = "// Starting token (" + Any::m_startUPToken + ") for first global user part not found\n";
		up2 = "// Error in first global user part\n";
		return; 
	}

	while( !old_xmp_file.eof() && line.substr(0, Any::m_startUPToken.length()) != Any::m_startUPToken) 
	{
		std::getline( old_xmp_file, line);
	}
	if ( !old_xmp_file.eof()) // found the start token starting at 1st character
	{
		std::getline( old_xmp_file, line);
		while( !old_xmp_file.eof() && line.substr( 0, Any::m_endUPToken.length()) != Any::m_endUPToken) 
		{
			up2 += line + "\n";
			std::getline( old_xmp_file, line);
		}
		if ( !old_xmp_file.eof()) // found the end token
		{
		}
		else
		{
			old_xmp_file.close();
			up2 = "// Ending token (" + Any::m_endUPToken + ") for second global user part not found\n";
			return;
		}
	}
	else
	{
		old_xmp_file.close();
		up2 = "// Starting token (" + Any::m_startUPToken + ") for second global user part not found\n";
		return; 
	}

	old_xmp_file.close();
	return;
}


/*static*/ std::string Dumper::xmlFilter( const std::string& in)
{
	std::map<char, std::string> m;
	m['&'] = "&amp;";
	m['>'] = "&gt;";
	m['<'] = "&lt;";
	m['"'] = "&quot;";
	m['\''] = "&apos;";

	std::string ret;
	for( std::string::const_iterator i = in.begin(); i != in.end(); ++i)
		if ( m.find( *i) != m.end())
			ret += m[*i];
		else
			ret += *i;

	return ret;
}