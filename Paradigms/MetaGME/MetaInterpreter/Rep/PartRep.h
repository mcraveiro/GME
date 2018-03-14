#ifndef PARTREP_H
#define PARTREP_H

#include "string"

#include "AspectRep.h"
#include "RoleRep.h"
class ModelRep;
class AspectRep;
class RoleRep;
class FCO;

/** The element which represent an aspect member. */
class PartRep 
{
public:
	PartRep( const RoleRep & role, 
		AspectRep * aspect_ptr)
		//AspectRep * kind_aspect_ptr = 0,
		//bool primary = true)
	:	m_rolePtr( &role),
		m_containerAspect( aspect_ptr),
		m_kindAspectPtr( 0), //m_kindAspectPtr( kind_aspect_ptr),
		m_primary( true) //m_primary( primary)
	{	}

	PartRep( const PartRep& peer): 
		m_rolePtr( peer.m_rolePtr), 
		m_containerAspect( peer.m_containerAspect),
		m_kindAspectPtr( peer.m_kindAspectPtr),
		m_primary( peer.m_primary)
	{ }

	const PartRep& operator=( const PartRep& peer)
	{
		if ( this == &peer) return *this;
		m_rolePtr = peer.m_rolePtr;
		m_containerAspect = peer.m_containerAspect;
		m_kindAspectPtr = peer.m_kindAspectPtr;
		m_primary = peer.m_primary;
		return *this;
	}

	bool operator!=( const PartRep& peer) const
	{
		return 
		( m_rolePtr != peer.m_rolePtr ||
		m_containerAspect != peer.m_containerAspect ||
		m_kindAspectPtr != peer.m_kindAspectPtr ||
		m_primary != peer.m_primary );
	}

	bool operator==( const PartRep& peer) const
	{ 
		return !this->operator!=(peer);
	}

public:
	inline const ModelRep* getParentPtr() const { return m_rolePtr->getModelRepPtr(); }
	inline const FCO* getFCOPtr() const { return m_rolePtr->getFCOPtr(); }

	inline const RoleRep* getRoleRepPtr() const { return m_rolePtr; }
	inline const AspectRep* getContainerAspectPtr() const { return m_containerAspect; }

	inline void setKindAspectPtr( const AspectRep* a_rep) { m_kindAspectPtr = a_rep; }
	inline const AspectRep* getKindAspectPtr() const { return m_kindAspectPtr; }

	inline void setPrimary( bool is_primary) { m_primary = is_primary; }
	inline bool isPrimary() const { return m_primary; }

protected:
	const RoleRep	* m_rolePtr;			// role the part belongs to
  const AspectRep * m_containerAspect;	// aspect the part belongs to
	const AspectRep * m_kindAspectPtr; // the selected aspect in case of mOdel part
  bool m_primary;
};
#endif //PARTREP_H