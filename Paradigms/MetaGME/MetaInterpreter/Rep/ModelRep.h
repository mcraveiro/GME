/* Generated by Together */

#ifndef MODELREP_H
#define MODELREP_H
#include "Any.h"
#include "AspectRep.h"
#include "RoleRep.h"
#include "FCO.h"

class Sheet;
/**
 * Represents a Model kind. Since the containment relationship always refers to a model, this class has to have at least
 * references to the Roles. Has to contain the Aspect related information also.
 */
#include "list"
#include "map"
#include "vector"

class ModelRep : public FCO 
{
public: // constant strings
	static const std::string IsTypeInfoShown_str;//"IsTypeInfoShown"
	// types
	// roles
	typedef RoleRep RoleSeriesValue;
	typedef std::vector<RoleSeriesValue> RoleSeries;
	typedef RoleSeries::iterator RoleSeries_Iterator;
	typedef RoleSeries::const_iterator RoleSeries_ConstIterator;
	typedef FCO * RoleMapKey;
	typedef RoleSeries RoleMapValue;
	typedef std::map< RoleMapKey, RoleMapValue> RoleMap;
	typedef RoleMap::iterator RoleMap_Iterator;
	typedef RoleMap::const_iterator RoleMap_ConstIterator;
	// aspects
	typedef std::vector<AspectRep*> AspectRepPtrList;

public:
	ModelRep( BON::FCO& ptr, BON::FCO& resp_ptr);
	~ModelRep();

	/*virtual*/ std::string doDump();

	inline Any::KIND_TYPE getMyKind() const { return Any::MODEL; }

	/*virtual*/ void initAttributes();

	// Roles
	void addRole( RoleMapKey whose, RoleRep& role);
	void initRoles();
	bool getRoles( FCO * ptr, RoleMapValue& ) const;

	// Final roles
	void addFinalRole( RoleMapKey whose, RoleRep& role);
	bool getFinalRoles( const FCO * ptr, RoleMapValue& ) const;

	// inner models
	std::vector< ModelRep *> getInnerModels() const;
	std::vector< ModelRep *> getInnerModelsFinal() const;
	std::vector< ReferenceRep *> getInnerModelReferencesFinal() const;

	// aspects
	void addAspect( AspectRep * asp);
	const AspectRepPtrList& getAspectRepPtrList() const;
	// final
	const AspectRepPtrList& getFinalAspectRepPtrList() const;
	void addFinalAspect( AspectRep *);
	void addFinalAspectList(const AspectRepPtrList& l);

	void getAspectNames( CStringList &list) const;

	AspectRep * getFirstAspect() const;
	AspectRep * getMyFirstAspectFromSet( const std::vector<AspectRep *> & aspect_set) const;
	bool findAspect( const AspectRep * one_asp) const;

	std::string dumpTypeInfoShown();

	// search
public:
	int searchMyAspectsForPart( PartRep& part) const; // how many of my aspects contain part as an aspect member
	int howManyAspects() const; // how many aspects has this model
	
	void createConstraints( Sheet *);
	bool checkMyAspects( Sheet *);
	void inherit();
	
	void sortMyAspects();
	void createPartsInModelAspects();

protected:
	RoleMap m_initialRoleMap;
	RoleMap m_finalRoleMap;

	AspectRepPtrList m_initialAspectList;
	AspectRepPtrList m_finalAspectList;

	bool m_bAttrIsTypeInfoShown;

private: // forbid copying
	ModelRep( const ModelRep&);
	const ModelRep& operator=( const ModelRep&);

};
#endif //MODELREP_H