#include "stdafx.h"

#include "FolderRep.h"
#include "Dumper.h"
#include "CodeGen.h"
#include "algorithm"

#include "globals.h"
extern Globals global_vars;


FolderRep::FolderRep( BON::FCO& ptr, BON::FCO& resp_ptr) 
	: Any( ptr)
	, m_fcoList()
	, m_fcoCardList()
	, m_subFolderList()
	, m_subCardList()
	, m_respPointer( resp_ptr)
	, m_methods()
{ 
}

FolderRep::~FolderRep() 
{ 
	m_fcoList.clear();
	m_fcoCardList.clear();
	m_subFolderList.clear();
	m_subCardList.clear();
	m_respPointer = BON::FCO();
	m_methods.clear();
}

	
std::string FolderRep::getName() const
{
#if(0)
	if (m_respPointer == BON::FCO())
	{
		global_vars.err << "Null pointer error in getFolderName\n";
		return std::string("Null pointer error in getFolderName");
	}
	return m_respPointer->getName();
#else
	if( this->m_respPointer) // there are equivalents
	{
		std::string regname = getMIRegistry()->getValueByPath( "/" + Any::NameSelectorNode_str);
		if( !regname.empty()) return regname;
		else return m_respPointer->getName();
	}
	else if ( this->m_ptr)
	{
		return m_ptr->getName();
	}
	return "NullPtrError";
#endif
}


void FolderRep::addFCO( FCO * ptr, const std::string& card)
{
	if ( std::find( m_fcoList.begin(), m_fcoList.end(), ptr) == m_fcoList.end()) // not found
	{
		m_fcoList.push_back( ptr);
		m_fcoCardList.push_back( card);
	}
	else
		global_vars.err << "CHECK: "<< ptr->getName() << " contained by folder " << getName() << " twice. Disregarded.\n";
}


bool FolderRep::isFCOContained( FCO * ptr)
{
	return m_fcoList.end() != std::find( m_fcoList.begin(), m_fcoList.end(), ptr);
}


void FolderRep::addSubFolderRep( FolderRep *ptr, const std::string& card)
{
	if ( std::find( m_subFolderList.begin(), m_subFolderList.end(), ptr) 
		== m_subFolderList.end()) // not found
	{
		m_subFolderList.push_back( ptr);
		m_subCardList.push_back( card);
	}
	else
		global_vars.err << "CHECK: Folder " << ptr->getName() << " contained by folder "  << getName() << " twice. Disregarded.\n";
}


void FolderRep::extendMembership()
{
	std::vector<FCO *> temp_list;
	FCO_Iterator fco_it = m_fcoList.begin();
	for( ; fco_it != m_fcoList.end(); ++fco_it)
	{
		FCO * fco_ptr = *fco_it;
		std::vector<FCO*> descendants;
		fco_ptr->getIntDescendants( descendants);
		std::vector<FCO*>::iterator desc_it = descendants.begin();
		for( ; desc_it != descendants.end(); ++desc_it)
		{
			FCO * new_member = *desc_it;
			if ( temp_list.end() == 
				std::find( temp_list.begin(), temp_list.end(), new_member)) // not found
				temp_list.push_back( new_member);
		}
	}
	// append the temp_list to the end of the fco_list
	m_fcoList.insert( m_fcoList.end(), temp_list.begin(), temp_list.end());
}


void FolderRep::createMethods()
{
	std::vector<FCO *>::iterator fco_it = m_fcoList.begin();
	for( ; fco_it != m_fcoList.end(); ++fco_it)
	{
		CodeGen::dumpKindGetter( *fco_it, this);
	}

	std::vector<FolderRep*>::iterator fold_it = m_subFolderList.begin();
	for( ; fold_it != m_subFolderList.end(); ++fold_it)
	{
		CodeGen::dumpFoldGetter( *fold_it, this);
	}
}


std::string FolderRep::doDump()
{
	std::string h_file, c_file;

	dumpPre( h_file, c_file);

	if ( !m_methods.empty())
		h_file += CodeGen::indent(1) + "//\n" + CodeGen::indent(1) + "// kind and subfolder getters\n";

	MethodLexicographicSort lex;
	std::sort( m_methods.begin(), m_methods.end(), lex);
	
	std::vector<Method>::iterator i = m_methods.begin();
	for( ; i != m_methods.end(); ++i)
	{
		h_file += i->getHeader() + "\n";
		c_file += i->getSource() + "";
	}

	dumpPost( h_file, c_file);

	sendOutH( h_file);//DMP_H( h_file);
	sendOutS( c_file);//DMP_S( c_file);

	return "";
}

/*static*/ std::string FolderRep::subFolderGetterMethodName(FolderRep * fold, const std::string& diff_nmsp)
{
#if(LONG_NAMES)
	return "get_Sub_" + fold->getValidName();
#else
	return "get" + diff_nmsp + fold->getValidName();
#endif
}


/*static*/ std::string FolderRep::kindGetterMethodName(FCO * fco, const std::string& diff_nmsp)
{
#if(LONG_NAMES)
	return "get_Kind_" + fco->getValidName();
#else
	return "get" + diff_nmsp + fco->getValidName();
#endif
}


