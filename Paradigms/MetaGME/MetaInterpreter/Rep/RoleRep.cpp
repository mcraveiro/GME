#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "RoleRep.h"
#include "FCO.h"

#include "fstream"




// finds and returns the value after the "token" token
std::string get_name( const std::string& line, const std::string & token = "name")
{
	std::string::size_type start_of_token = line.find(token);
	if (start_of_token == std::string::npos) return "";
	
	std::string::size_type first_quote = line.substr( start_of_token).find( "\"");
	if (first_quote == std::string::npos) return "";
	// find the second '"'
	std::string::size_type len_of_name = line.substr(start_of_token + first_quote + 1).find("\"");
	if ( len_of_name == std::string::npos) return "";

	std::string pp = line.substr( start_of_token + first_quote + 1, len_of_name);
	return line.substr( start_of_token + first_quote + 1, len_of_name);
}


bool PartStringLex::operator()( const std::string& peer1, const std::string& peer2) const
{ 
	std::string part1_name = ::get_name( peer1, "role");
	std::string part2_name = ::get_name( peer2, "role");
	int k = part1_name.compare( part2_name);

	return (k < 0);
}


bool StringLex::operator()( const std::string& peer1, const std::string& peer2) const
{ 
	int k = peer1.compare(peer2);

	return (k < 0);
}


bool RoleStringLex::operator()( const RoleRep& peer1, const RoleRep& peer2) const
{
	std::string n1 = peer1.getSmartRoleName();
	std::string n2 = peer2.getSmartRoleName();
	int k = n1.compare( n2);

	return ( k < 0);
}


RoleRep::RoleRep( 
	const std::string &role_name,
	FCO* ptr, 
	ModelRep * model_ptr,
	bool is_port,
	const std::string &cardinality,
	bool inh_role,
	bool long_form)
	:	m_roleName( role_name),
	m_ptr( ptr),
	m_model( model_ptr),
	m_isPort( is_port),
	m_cardinality( cardinality),
	m_inheritedRole( inh_role),
	m_longForm( long_form)
{ 
}

RoleRep::RoleRep( const RoleRep & peer) 
	:
	m_roleName( peer.m_roleName),
	m_ptr( peer.m_ptr), 
	m_model( peer.m_model),
	m_isPort( peer.m_isPort),
	m_cardinality( peer.m_cardinality),
	m_inheritedRole( peer.m_inheritedRole),
	m_longForm( peer.m_longForm)
{
}


const RoleRep& RoleRep::operator=( const RoleRep & peer) 
{ 
	if (this == &peer) return *this;
	m_roleName = peer.m_roleName;
	m_ptr = peer.m_ptr;
	m_model = peer.m_model;
	m_isPort = peer.m_isPort;
	m_cardinality = peer.m_cardinality; 
	m_inheritedRole = peer.m_inheritedRole;
	m_longForm = peer.m_longForm;
	return *this;
}


bool RoleRep::operator==( const RoleRep& peer) const 
{ 
	return
		m_roleName == peer.m_roleName &&
		m_ptr == peer.m_ptr &&
		m_model == peer.m_model &&
		m_isPort == peer.m_isPort &&
		m_cardinality == peer.m_cardinality &&
		m_inheritedRole == peer.m_inheritedRole &&
		m_longForm == peer.m_longForm;
}


bool RoleRep::operator!=( const RoleRep& peer) const 
{ 
	return !(*this == peer); 
}


/*bool RoleRep::operator<( const RoleRep& peer) const 
{ 
	std::string m1 = this->getSmartRoleName() + m_ptr->getName();
	std::string m2 = peer.getSmartRoleName() + peer.getFCOPtr()->getName();
	err << m1 << " < " << m2 << "\n";
	return (m1.compare(m2) < 0);
}*/


const std::string& RoleRep::getOnlyRoleName() const 
{ 
	return m_roleName; 
}


std::string RoleRep::getSmartRoleName() const 
{ 
	if ( m_longForm) return m_ptr->getName() + m_roleName;
	else if (m_roleName != "") return (m_ptr->getNamespace().empty() ? "" : ( m_ptr->getNamespace() + Any::NamespaceDelimiter_str)) + m_roleName;
	else return m_ptr->getName();
}


FCO * RoleRep::getFCOPtr() const 
{ return m_ptr; }


ModelRep * RoleRep::getModelRepPtr() const
{ return m_model; }


bool RoleRep::isPort() const 
{ 
	return m_isPort; 
}


const std::string& RoleRep::getCardinality() const 
{ 
	return m_cardinality; 
}


void RoleRep::setInheritedRole( bool val) 
{ 
	m_inheritedRole = val; 
}


bool RoleRep::isInheritedRole() const
{ 
	return m_inheritedRole; 
}


void RoleRep::setLongForm( bool val)
{
	m_longForm = val;
}


bool RoleRep::isLongForm() const
{
	return m_longForm;
}
