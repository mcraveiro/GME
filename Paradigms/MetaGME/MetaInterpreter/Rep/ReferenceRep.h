#ifndef REFERENCEREP_H
#define REFERENCEREP_H

#include "Any.h"
#include "FCO.h"
#include "vector"
#include "AspectRep.h"

class ReferenceRep : public FCO 
{
public: // types
	typedef std::vector<FCO *> RefereeList;
	typedef std::vector<FCO *>::iterator RefereeList_Iterator;
	typedef std::vector<FCO *>::const_iterator RefereeList_ConstIterator;

public:
	ReferenceRep( BON::FCO& ptr, BON::FCO& resp_ptr);
	~ReferenceRep(); 
	
	/*virtual*/ std::string doDump();
	std::string dumpShowPorts() const;

	inline Any::KIND_TYPE getMyKind() const { return Any::REF; }

	void addInitialReferee( FCO * refd );
	FCO * getInitialReferee() const;
	const RefereeList& getInitialReferees() const;
	
	void addFinalReferees( FCO * referee);
	void addFinalReferees( RefereeList & referees);
	const RefereeList& getFinalReferees() const;

	void setShowPorts(bool set) { m_showPorts = set; }

	bool finalize();
	void inherit();

	bool pointsToModels() const; // true if points to models as well
	std::vector<const ModelRep *> getModelRefVector() const;

	bool checkNotEmpty() const;

	// aspect related
	int howManyAspectsAmongModelRefs() const;
	std::vector<AspectRep *> getAspectsIntersection() const;
	AspectRep * getFirstAspectFromIntersection() const;

protected:
	// initialy "this" refers to
	RefereeList m_initialReferees;
	// finally "this" refers to
	RefereeList m_finalReferees;
	// member that contains all the referees (including multiple steps by referring to a reference)
	// this reference may look like all of m_allReferees 
	// i.e. ( ref1 -> ref2 -> model) ref1 looks like model
	// plays role at connection dump
	// it should be named m_transitiveReferences
	RefereeList m_allReferees;
	bool m_showPorts;

private: // forbiding copy
	ReferenceRep( const ReferenceRep&);
	const ReferenceRep& operator=( const ReferenceRep&);

};

#endif //REFERENCEREP_H