#ifndef FCOREP_H
#define FCOREP_H
#include "Any.h"
#include "RoleRep.h"
#include "FCO.h"

/** Represents an FCO kind in a model. */
class FcoRep : public FCO 
{
public:

	FcoRep( BON::FCO& ptr, BON::FCO& resp_ptr) 
		: FCO( ptr, resp_ptr) 
	{ }

	/*virtual*/ std::string doDump() { return ""; }
	inline Any::KIND_TYPE getMyKind() const { return Any::FCO_REP; }

private: // forbiding copy
	FcoRep( const FcoRep&);
	const FcoRep& operator=( const FcoRep&);
};
#endif //FCOREP_H