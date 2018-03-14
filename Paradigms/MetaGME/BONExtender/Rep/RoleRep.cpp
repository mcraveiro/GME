#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "RoleRep.h"
#include "FCO.h"

#include "fstream"
extern std::ofstream err;


bool StringLex::operator()( const std::string& peer1, const std::string& peer2) const
{ 
	return (peer1.compare(peer2) < 0);
}


RoleRep::RoleRep( 
	const std::string &role_name,
	FCO* ptr, 
	ModelRep * model_ptr,
	//ModelRep * owner_model_ptr,
	bool is_port,
	const std::string &cardinality,
	bool inh_role,
	bool long_form)
	:	m_roleName( role_name),
	m_ptr( ptr),
	m_model( model_ptr),
	//m_ownerModel( owner_model_ptr),
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
	//m_ownerModel( peer.m_ownerModel),
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
	//m_ownerModel = peer.m_ownerModel;
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
		//m_ownerModel == peer.m_ownerModel &&
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
	else if (m_roleName != "") return m_roleName;
	else return m_ptr->getName();
}


FCO * RoleRep::getFCOPtr() const 
{ return m_ptr; }


ModelRep * RoleRep::getModelRepPtr() const
{ return m_model; }


/*ModelRep * RoleRep::getOwnerModelRepPtr() const
{ return m_ownerModel; }*/


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
