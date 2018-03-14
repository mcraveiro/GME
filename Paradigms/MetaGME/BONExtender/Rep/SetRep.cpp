#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "SetRep.h"
#include "CodeGen.h"
#include "Dumper.h"

#include "logger.h"

#include "algorithm"

#include "globals.h"
extern Globals global_vars;
extern int h_ind;

SetRep::SetRep( BON::FCO& ptr, BON::FCO& resp_ptr)
	: FCO( ptr, resp_ptr)
	, m_memberList()
	, m_finalMemberList()
	, m_setMethods()
{
}


SetRep::~SetRep()
{ 
	m_memberList.clear();
	m_finalMemberList.clear();
	m_setMethods.clear();
}

	
void SetRep::addMember( FCO * member) 
{ 
	SetMemberList_ConstIterator it = 
		std::find( m_memberList.begin(), m_memberList.end(), member);

	if (it == m_memberList.end()) // not found
		m_memberList.push_back( member); 
	else
		global_vars.err << "Set member " << member->getName() << " is twice in set " << getName() << "\n";
} 


void SetRep::extendMembership()
{
	// insert in the m_finalMemberList all non-abstract interface descendants of the original members
	SetMemberList_ConstIterator member_it = m_memberList.begin();
	for( ; member_it != m_memberList.end(); ++member_it)
	{
		FCO * member_ptr = *member_it;
		std::vector<FCO*> family;
		member_ptr->getIntDescendants( family);
		family.push_back( member_ptr);
		std::vector<FCO*>::iterator desc_it = family.begin();
		for( ; desc_it != family.end(); ++desc_it)
		{
			FCO * new_member = *desc_it;
			if ( !new_member->isAbstract() &&
					 m_finalMemberList.end() == std::find( m_finalMemberList.begin(), m_finalMemberList.end(), new_member)) // not found
				m_finalMemberList.push_back( new_member);
		}
	}
}


void SetRep::createMethods()
{
	// getter for each potential member
	SetMemberList_Iterator it = m_finalMemberList.begin();
	for( ; it != m_finalMemberList.end(); ++it)
	{
		if ( Dumper::m_bGenRegular)
			m_setMethods.push_back( CodeGen::dumpSetGetter( this, *it, "", false, false));

		if ( Dumper::m_bGenTemplates)
			m_setMethods.push_back( CodeGen::dumpSetGetterGeneric( this, *it, "", false, false));
	}
	
	// aggregated getter with common ancestor of all potential members
	std::vector<FCO*> common_anc = FCO::lcdIntersect( m_finalMemberList); // the set may contain all of these fcos
	if ( !common_anc.empty())
	{
		std::vector<FCO*>::iterator c_it = common_anc.begin();
		if ( c_it != common_anc.end())
		{
			if ( Dumper::m_bGenRegular)
				m_setMethods.push_back( CodeGen::dumpSetGetter( this, *c_it, "", true, true)); // generate method for only 1 common ancestor and use the dummy trick to avoid name-conflict with plain getters

			if ( Dumper::m_bGenTemplates)
				m_setMethods.push_back( CodeGen::dumpSetGetterGeneric( this, *c_it, "", true, true)); // generate method for only 1 common ancestor and use the dummy trick to avoid name-conflict with plain getters
		}
	}
	else if ( common_anc.empty()) // empty, use common_kind
	{
		std::string common_kind = FCO::lcdKindIntersect( m_finalMemberList);
		if ( !common_kind.empty())
		{
			if ( Dumper::m_bGenRegular)
				m_setMethods.push_back( CodeGen::dumpSetGetter( this, 0, common_kind, true, false));

			if ( Dumper::m_bGenTemplates)
				m_setMethods.push_back( CodeGen::dumpSetGetterGeneric( this, 0, common_kind, true, false));

		}
		//else one solution remained: BON::FCO this is not adding any service to the plain Bon2 getMembers() call, so omitted
		else
			global_vars.err << "Note: " << getName() << "Impl::getAllMembers() omitted since it can return only what the SetImpl::getMembers() does."<< "\n";
	}
}


std::string SetRep::doDump()
{
	std::string h_file, c_file;

	dumpPre( h_file, c_file);
	dumpFCO( h_file, c_file);

	if ( !m_setMethods.empty())
		h_file += CodeGen::indent(1) + "//\n" + CodeGen::indent(1) + "// set getters\n";

	MethodLexicographicSort lex;
	std::sort( m_setMethods.begin(), m_setMethods.end(), lex);
	
	std::vector<Method>::iterator i = m_setMethods.begin();
	for( ; i != m_setMethods.end(); ++i)
	{
		h_file += i->getHeader() + "\n";
		c_file += i->getSource() + "";
	}

	h_file += hideAndExpose();

	dumpPost( h_file, c_file);

	sendOutH( h_file);//DMP_H( h_file);
	sendOutS( c_file);//DMP_S( c_file);

	return "";
}


bool SetRep::checkSetElements()
{
	bool res = true;
	const int number_of_lists = 1;
	const SetMemberList* lists[ number_of_lists] = {
		&m_memberList
	};

	for( int i = 0; i < number_of_lists; ++i)
	{
		SetMemberList_ConstIterator member_it = lists[i]->begin();
		// for all members of the set
		for( ; member_it != lists[i]->end(); ++member_it)
		{
			FCO * member_ptr = *member_it;

			if ( !member_ptr->checkIsPartOfFinal() && !member_ptr->isAbstract())
			{
				global_vars.err << std::string("CHECK: set member " + member_ptr->getName() + " [in set " + getName() + "] has no parent.\n");
				res = false;
			}
		}
	}
	return res;
}


std::string SetRep::setGetterTemplate( const FCO * fco)
{
#if(LONG_NAMES)
	if (fco)
	{
		bool same_nmsp = fco->getValidNmspc() == getValidNmspc();
		return "get" + ( same_nmsp ? fco->getValidName() : fco->getValidNmspc() + fco->getValidName())) + "Members";
	}
	else
		return "getAllMembers";
#else
	if (fco)
	{
		bool same_nmsp = fco->getValidNmspc() == getValidNmspc(); 
		return "get" + ( same_nmsp ? fco->getValidName() : ( fco->getValidNmspc() + fco->getValidName())) + "s";
	}
	else
		return "getAllMembers";
#endif
}


std::string SetRep::expose( const std::string& repl_container)
{
	std::string h_file;
	h_file += FCO::expose( repl_container);

	if (!m_setMethods.empty())
		h_file += CodeGen::indent(h_ind) + "//\n" + CodeGen::indent(h_ind) + "// exposed set getters\n";
	std::vector<Method>::iterator i = m_setMethods.begin();
	for( ; i != m_setMethods.end(); ++i)
	{
		h_file += i->getExposed( repl_container) + "\n";
	}

	return h_file;
}


std::string SetRep::hide()
{
	std::string h_file;
	h_file += FCO::hide();

	if (!m_setMethods.empty())
		h_file += CodeGen::indent(h_ind) + "//\n" + CodeGen::indent(h_ind) + "// hidden set getters\n";
	std::vector<Method>::iterator i = m_setMethods.begin();
	for( ; i != m_setMethods.end(); ++i)
	{
		h_file += i->getHidden() + "\n";
	}

	return h_file;
}


