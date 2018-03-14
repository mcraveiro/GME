#ifndef CONNECTIONREP_H
#define CONNECTIONREP_H

#include "FCO.h"
#include "ConnJoint.h"
#include "list"

/** Represents a Connection kind. */
class ConnectionRep : public FCO 
{
public:
	ConnectionRep( BON::FCO& ptr, BON::FCO& resp_ptr);
	/*virtual*/ ~ConnectionRep();

public:
	inline Any::KIND_TYPE getMyKind() const { return Any::CONN; }
	void addJoint( ConnJoint & joint);
	void appendJointElements( const ConnJoint & joint);
	
	void inherit();

	bool checkConnectionTargets();

	std::string doDump();

	bool calcLCD();
	bool createEndGetters();
	void addMethod( Method& m) { m_connMethods.push_back( m); }

protected:
	std::list<ConnJoint> m_jointList;
	std::vector< Method> m_connMethods;

private: // forbiding copy
	ConnectionRep( const ConnectionRep&);
	const ConnectionRep& operator=( const ConnectionRep&);
};

#endif //CONNECTIONREP_H
