#ifndef ROLEREP_H
#define ROLEREP_H

class ConnectionRep;
class ConnJoint;
class ModelRep;
class SetRep;
class AtomRep;
class ReferenceRep;
class FCO;

#include "string"
#include "list"
#include "vector"

class StringLex
{
public:
	bool operator()( const std::string& peer1, const std::string& peer2) const;
};


class RoleRep {

public:

	typedef std::vector<RoleRep> RoleRepSeries;
	typedef RoleRepSeries::iterator RoleRepSeries_Iterator;
	typedef RoleRepSeries::const_iterator RoleRepSeries_ConstIterator;

	RoleRep( 
		const std::string &role_name,
		FCO* ptr, 
		ModelRep* model_ptr,
		bool is_port,
		const std::string &cardinality,
		bool inh_role,
		bool long_form);

	RoleRep( const RoleRep & peer) ;
	const RoleRep& operator=( const RoleRep & peer); 
	bool operator==( const RoleRep& peer) const;
	bool operator!=( const RoleRep& peer) const;
	
	friend bool operator<(const RoleRep&, const RoleRep&);
	//bool operator<(const RoleRep& peer) const;

	const std::string& getOnlyRoleName() const;
	std::string getSmartRoleName() const;

	FCO * getFCOPtr() const;
	ModelRep * getModelRepPtr() const;
	bool isPort() const;
	const std::string& getCardinality() const;

	void setInheritedRole( bool val);
	bool isInheritedRole() const;

	void setLongForm( bool val);
	bool isLongForm() const;

private:
	std::string m_roleName;
	FCO* m_ptr;
	ModelRep * m_model;
	bool m_isPort;
	std::string m_cardinality;

	// shows if a role is inherited from an ancestor
	// M<>----r-B
	//          |
	//          D
	bool m_inheritedRole;

	// in the case above for B the short role name form of "r" cannot be used
	// because it can be confusing. It refers to a B or a D?
	// so in such cases the long form is to be used: Br, Dr
	bool m_longForm;

};
#endif //ROLEREP_H
