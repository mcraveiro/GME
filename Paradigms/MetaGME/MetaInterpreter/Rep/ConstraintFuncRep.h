#ifndef CONSTRAINTFUNC_H
#define CONSTRAINTFUNC_H

#include "Any.h"
#include "string"

class ConstraintFuncRep : public Any
{
public:
	ConstraintFuncRep( BON::FCO& ptr);
	std::string doDump();
	/*virtual*/ std::string getName() const;
	Any::KIND_TYPE getMyKind() const { return Any::CONSTRAINTFUNC; }

protected:
	void fetch();
	std::string m_context;
	std::string m_returntype;
	std::string m_stereotype;
	std::string m_definition;
	std::string m_parameterlist;
	std::string m_defdForNamesp;

private: // forbiding copy
	ConstraintFuncRep( const ConstraintFuncRep &);
	const ConstraintFuncRep& operator=( const ConstraintFuncRep&);

};
#endif //CONSTRAINTFUNC_H
