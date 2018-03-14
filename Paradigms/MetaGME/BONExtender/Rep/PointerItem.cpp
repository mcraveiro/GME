#include "stdafx.h"
#include "PointerItem.h"

bool PointerItemLex::operator()( const PointerItem& p1, const PointerItem& p2) const
{
	std::string p1_n = p1.name();
	return (p1_n.compare( p2.name()) < 0 );
}


PointerItem::PointerItem( std::string name, FCO * ptr)
 : m_name( name), m_fcoPtr( ptr)
{ }

PointerItem::PointerItem( const PointerItem& peer)
 : m_name( peer.m_name), m_fcoPtr( peer.m_fcoPtr)
{ }

const PointerItem& PointerItem::operator=( const PointerItem& peer)
{
	if (&peer == this) return *this;
	m_name = peer.m_name;
	m_fcoPtr = peer.m_fcoPtr;
	return *this;
}

bool PointerItem::operator==( const PointerItem& peer) const
{
	return m_name == peer.m_name && m_fcoPtr == peer.m_fcoPtr;
}

bool PointerItem::operator!=( const PointerItem& peer) const
{
	return !operator==(peer);
}

const std::string& PointerItem::name() const
{
	return m_name;
}

FCO * PointerItem::fcoPtr() const
{
	return m_fcoPtr;
}