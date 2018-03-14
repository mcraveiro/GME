#ifndef ENTITY_H
#define ENTITY_H

#include "BON.h"

class Entity
{

public:
	explicit Entity( const BON::Folder& parent, const BON::FCO& p, const BON::FCO& p2 = BON::FCO())
		: m_pointer( p), m_respPointer( p2), m_parentFolder( parent), m_deleted( false) { }

	Entity( const Entity & operand )
		: m_pointer(operand.m_pointer)
		, m_respPointer( operand.m_respPointer)
		, m_parentFolder( operand.m_parentFolder)
		, m_deleted( operand.m_deleted)
	{ }

	const Entity& operator=(const Entity& operand) 
	{ 
		if (&operand == this) return *this;

		m_pointer = operand.m_pointer;
		m_respPointer = operand.m_respPointer;
		m_parentFolder = operand.m_parentFolder;
		m_deleted = operand.m_deleted;
		
		return *this;
	}

	~Entity() { }

	bool operator==( const Entity& peer) { return (m_pointer == peer.m_pointer) && (m_respPointer == peer.m_respPointer) && (m_parentFolder == peer.m_parentFolder) && (m_deleted == peer.m_deleted); }
	bool operator!=( const Entity& peer) { return !(*this==peer); }
	bool operator<( const Entity& peer) const
	{
		BON::FCO f1 ( getPointer());
		BON::FCO f2 ( peer.getPointer());
		return ( f1->getName().compare( f2->getName()) < 0);
	}

	BON::FCO getPointer() const { return m_pointer; }
	BON::FCO getRespPointer() const { return m_respPointer; }
	BON::Folder getParentFolder() const { return m_parentFolder; }
	void setPointer( const BON::FCO& p) { m_pointer = p; }
	void setRespPointer( const BON::FCO& r_p) { m_respPointer = r_p; }
	void setParentFolder( const BON::Folder& pf) { m_parentFolder = pf; }
	bool isDeleted() const { return m_deleted; }
	void deleted( const bool val) { m_deleted = val; }

private:
	BON::FCO m_pointer;
	BON::FCO m_respPointer;
	BON::Folder m_parentFolder;
	bool m_deleted;
};


#endif // ENTITY_H


