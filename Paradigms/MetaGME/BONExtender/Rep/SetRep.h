#ifndef SETREP_H
#define SETREP_H

#include "BON.h"
#include "BONImpl.h"

#include "Any.h"
#include "FCO.h"
#include "PointerItem.h"
#include "ModelRep.h"
#include "logger.h"

class SetRep : public FCO 
{
public: // types
	typedef std::vector<FCO*> SetMemberList;
	typedef std::vector<FCO*>::iterator SetMemberList_Iterator;
	typedef std::vector<FCO*>::const_iterator SetMemberList_ConstIterator;

	typedef std::vector< PointerItem> PointerItemSeries;
	typedef std::vector< PointerItem>::iterator PointerItemSeries_Iterator;
	typedef std::vector< PointerItem>::const_iterator PointerItemSeries_ConstIterator;
	typedef const ModelRep* Key;
	typedef std::map< Key, PointerItemSeries> SetMembersMap;
	typedef SetMembersMap::iterator SetMembersMap_Iterator;

public: // types
	SetRep( BON::FCO& ptr, BON::FCO& resp_ptr);
	~SetRep();

	inline Any::KIND_TYPE getMyKind() const { return Any::SET; }

	// adds new elements to the m_memberList vector
	void addMember( FCO * member);
	void extendMembership();

	std::string doDump();

	bool checkSetElements();

	std::string setGetterTemplate( const FCO * fco);
	void createMethods();
	std::string expose( const std::string& repl_container);
	std::string hide();
protected:
	// stores those FCO-s which are in SET_MEMBER relationship with (*this)
	// coresponds to "initial"
	SetMemberList m_memberList;
	SetMemberList m_finalMemberList;

	std::vector<Method> m_setMethods;

private: // forbiding copy
	SetRep( const SetRep&);
	const SetRep& operator=( const SetRep&);

};

#endif //SETREP_H
