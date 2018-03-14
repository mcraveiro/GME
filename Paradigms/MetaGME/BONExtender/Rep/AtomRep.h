#ifndef ATOMREP_H
#define ATOMREP_H
#include "Any.h"
#include "RoleRep.h"
#include "FCO.h"

/** Represents an Atom kind in a model. */
class AtomRep : public FCO 
{
public:
	AtomRep( BON::FCO& ptr, BON::FCO& resp_ptr);
	inline Any::KIND_TYPE getMyKind() const { return Any::ATOM; }
	std::string doDump();

private: // forbiding copy
	AtomRep( const AtomRep&);
	const AtomRep& operator=( const AtomRep&);

};
#endif //ATOMREP_H
