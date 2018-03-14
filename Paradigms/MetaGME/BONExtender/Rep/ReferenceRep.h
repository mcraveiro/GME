#ifndef REFERENCEREP_H
#define REFERENCEREP_H

#include "Any.h"
#include "FCO.h"
#include "vector"

class ReferenceRep : public FCO 
{
public: // types
	typedef std::vector<FCO *> RefereeList;
	typedef std::vector<FCO *>::iterator RefereeList_Iterator;
	typedef std::vector<FCO *>::const_iterator RefereeList_ConstIterator;

public:
	ReferenceRep( BON::FCO& ptr, BON::FCO& resp_ptr);
	~ReferenceRep(); 
	/*virtual*/ Any::KIND_TYPE getMyKind() const { return Any::REF; }
	/*virtual*/ std::string doDump();

	void addInitialReferee( FCO * refd );
	FCO * getInitialReferee() const;
	const RefereeList& getInitialReferees() const;
	
	void addFinalReferees( FCO * referee);
	void addFinalReferees( RefereeList & referees);
	const RefereeList& getFinalReferees() const;

	bool finalize();
	void inherit();

	bool pointsToModels() const; // true if points to models only
	std::vector<const ModelRep *> getModelRefVector() const;

	bool checkNotEmpty() const;
	bool checkAllTheSameKind() const; 

	std::string refGetterTemplate(FCO *);

	std::string dumpUsing();
	std::string dumpUsingLine( FCO *);
	void createMethods();
	std::string expose( const std::string& repl_container);
	std::string hide();

protected:
	std::vector<Method> m_refGetterMethods;
	// initialy "this" refers to
	RefereeList m_initialReferees;
	// finally "this" refers to
	RefereeList m_finalReferees;
	// member that contains all the referees (including multiple steps by referring to a reference)
	RefereeList m_allReferees;

private: // forbiding copy
	ReferenceRep( const ReferenceRep&);
	const ReferenceRep& operator=( const ReferenceRep&);

};

#endif //REFERENCEREP_H