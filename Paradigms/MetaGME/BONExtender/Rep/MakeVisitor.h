#ifndef MAKEVISITOR_H
#define MAKEVISITOR_H

#include "string"
#include "vector"
#include "Any.h"

#define HOW_MANY_ITEMS 8

class ClassAndNamespace
{
public:
	static ClassAndNamespace makeIt( Any *);

	ClassAndNamespace( const std::string& pName, const std::string& pSpace, const std::string& pStrictSpace);
	std::string name() const;
	std::string space() const;
	std::string strictSpace() const;
	std::string infoName() const; // gives back a NMSPKIND form (informative name)
	std::string exactType() const;// gives back a type NMPS::KIND form (exact C++ type name)
private:
	std::string m_name;
	std::string m_space;
	std::string m_strictSpace;
};

class MakeVisitor
{
public:
	static std::string getVisitorName( const std::string& name) { return name + "Visitor"; }
	MakeVisitor( const std::string& name) { m_className = getVisitorName( name); }
	const std::string& getClassName() { return m_className; }

	void addEntity( Any::KIND_TYPE , const ClassAndNamespace&);
	std::string dumpVisitorHeader();
	std::string dumpVisitorSource();
	std::string dumpSpecificMethods( const ClassAndNamespace& name);
	std::string dumpGenericMethods();
protected:
	std::vector< ClassAndNamespace > m_entities[HOW_MANY_ITEMS];
	std::string m_className;
};

#endif // MAKEVISITOR_H