#include "stdafx.h"

#include "FolderRep.h"
#include "Dumper.h"
#include "Broker.h"

#include "algorithm"
#include "fstream"

#include "globals.h"
extern Globals global_vars;


FolderRep::FolderRep( BON::FCO& ptr, BON::FCO& resp_ptr) 
	: Any( ptr)
	, m_fcoList()
	, m_fcoCardList()
	, m_subFolderList()
	, m_subCardList()
	, m_respPointer( resp_ptr)
{ 
}


FolderRep::~FolderRep() 
{ 
	m_fcoList.clear();
	m_fcoCardList.clear();
	m_subFolderList.clear();
	m_subCardList.clear();
	m_respPointer = BON::FCO();
}

	
std::string FolderRep::getName() const
{
#if(0)
	if (m_respPointer == BON::FCO())
	{
		global_vars.err << MSG_ERROR << "Null pointer error in getFolderName\n";
		return std::string("Null pointer error in getFolderName");
	}
	return m_respPointer->getName();
#else // new way
	if( m_respPointer)
	{
		return m_namespace + (m_namespace.empty()?"":Any::NamespaceDelimiter_str) + m_respPointer->getName();
	}
	else
	{
		return m_namespace + (m_namespace.empty()?"":Any::NamespaceDelimiter_str) + m_ptr->getName();
	}
#endif
}

std::string FolderRep::getDispName() const
{
#if(0)
	//what about a reform such that dispname is allowed only for lonely aspects (which are not equivalent with any other)
	//<!> 
	if (m_respPointer == BON::FCO())
	{
		global_vars.err << MSG_ERROR << "Null pointer error in getDispName for aspect \"" + getName() + "\"\n";
		return std::string("Null pointer error in getDispName for folder \"") + getName() + "\"";
	}
	else
	{
		return m_ptr->getAttribute( Any::DisplayedName_str)->getStringValue();//<!> modified from m_respPointer
	}
#else // new way
	if( m_respPointer) // not a plain folder, it has its resppointer set
	{
		//return "";
		return m_userSelectedDisplayName;
	}
	else
	{
		return m_ptr->getAttribute( Any::DisplayedName_str)->getStringValue();
	}
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
		global_vars.err << MSG_ERROR << "CHECK: "<< ptr->getPtr() << " contained by folder " << m_ptr << " twice. Disregarded.\n";
}


bool FolderRep::isFCOContained( FCO * ptr)
{
	return m_fcoList.end() != std::find( m_fcoList.begin(), m_fcoList.end(), ptr);
}


bool FolderRep::isEmpty() const
{
	//return m_fcoList.isEmpty();
	bool fco_list_empty = true;
	FCO_ConstIterator it = m_fcoList.begin();
	while ( it != m_fcoList.end())
	{
		if (!(*it)->isAbstract())
			fco_list_empty = false;
		++it;
	}
	return m_subFolderList.empty() && fco_list_empty;
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
		global_vars.err << MSG_ERROR << "CHECK: Folder " << ptr->getPtr() << " contained by folder "  << m_ptr << " twice. Disregarded.\n";
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


std::string FolderRep::doDump() 
{ 
	std::string m_ref = askMetaRef();

	// header
	std::string mmm = indStr() + "<folder name = \"" + getName() + "\" metaref = \"" + m_ref + "\" ";

	AnyLexicographicSort lex;
	std::sort( m_subFolderList.begin(), m_subFolderList.end(), lex);

	SubFolder_ConstIterator it_f = m_subFolderList.begin();
	if  ( it_f != m_subFolderList.end())
	{
		mmm += " subfolders = \"" + (*it_f)->getName();
		for( ++it_f; it_f != m_subFolderList.end(); ++it_f)
		{
			mmm += " " + (*it_f)->getName();
		}
		mmm += "\"";
	}

	std::sort( m_fcoList.begin(), m_fcoList.end(), lex);

	if ( !m_fcoList.empty())
	{
		std::string nnn = "";
		bool first = true;
		FCO_Iterator it = m_fcoList.begin();
		while ( it != m_fcoList.end())
		{
			if (!(*it)->isAbstract()) // check for abstract
			{
				if ( !first) nnn += " ";
				nnn += (*it)->getName();
				first = false;
			}
			++it;
		}
		if ( !nnn.empty())
			mmm += " rootobjects = \"" + nnn + "\"";
	}
	mmm += " >\n";
	
	++ind;

	// properties
	std::string f_disp = getDispName();
	if (!f_disp.empty() && f_disp != getName()) // if not empty and not the same
		mmm += dumpDispName();

	mmm += dumpGeneralPref();
	mmm += dumpConstraints();
	--ind;
	// footer
	mmm += indStr() + "</folder>\n";

	return mmm;
}


/*
This function must be called before the extend(), because the cardinalities 
are consistent till that point

Presumption: m_fcoList[i] has m_fcoCardList[i] cardinality
*/
void FolderRep::createConstraints( Sheet * s)
{
	std::vector<FCO *>::iterator fco_it = m_fcoList.begin();
	SubFolder_Iterator sub_it = m_subFolderList.begin();

	for ( unsigned int i = 0; i < m_fcoList.size(); ++i)
	{
		FCO * ptr = m_fcoList[i];
		std::string card = "";
		if (i < m_fcoCardList.size())
			card = m_fcoCardList[i];
		else
			global_vars.err << MSG_ERROR << "Error in folder \"" << m_ptr << "\" element constraint generation: not enough cardList members\n";

		std::string str_expr_end;
		std::string str_card_context;
		str_card_context = "[containment] In folder: " + getName() + ", FCO " + ptr->getName();
		bool valid_constr = ! Dumper::doParseCardinality( card, "rootObjCount", str_card_context, str_expr_end);
		if ( ! valid_constr )
		{
			global_vars.err << MSG_ERROR << "Ignoring invalid cardinality string in folder part: " << ptr->getPtr() + ". String: " << card << "\n";
		}

		std::string str_expr_begin = 
			"let rootObjCount = rootChildren()->select( child | child.oclIsTypeOf( " + ptr->getName() + " )";

		std::vector<FCO*> descendants;
		ptr->getIntDescendants( descendants);
		std::vector<FCO*>::iterator desc_it = descendants.begin();
		for( ; desc_it != descendants.end(); ++desc_it)
			str_expr_begin += " or child.oclIsTypeOf( " + (*desc_it)->getName() + ")";

		str_expr_begin += " )->size in\n                     ";
		// If Cardinality was appropriate

		if ( valid_constr && ! str_expr_end.empty() ) {

			// Build Name, EventMask, Description

			int id = Broker::getNextConstraintId();
			char str_id[64];
			sprintf( str_id, "%d", id);

			std::string str_cons_name;
			str_cons_name = "Valid" + getName() + "PartCardinality" + std::string(str_id);
			std::string::size_type pos = str_cons_name.find( "::");
			if( pos != std::string::npos) 
				str_cons_name.replace( pos, 2, 2, '_');

			int iEventMask = 0;
			char chMask[64];
			sprintf( chMask, "%x", iEventMask );

			std::string str_desc;
			str_desc = "Multiplicity of parts, which are contained by folder " 
				+ getName() +	", has to match "+ card + ".";

			ConstraintRep * cr = s->createConstraintRep( BON::FCO());
			std::string s_b = str_expr_begin + str_expr_end;
			cr->init( str_cons_name, /*mask:*/global_vars.genConstr.fol_cont_mask, "1", global_vars.genConstr.priority, s_b, str_desc);

			this->addInitialConstraintRep( cr); // <!> to be decided whether initial or final
			cr->attachedTo();
		}
	}
}