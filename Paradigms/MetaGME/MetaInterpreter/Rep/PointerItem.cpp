#include "stdafx.h"
#include "PointerItem.h"

bool PointerItemLex::operator()( const PointerItem& p1, const PointerItem& p2) const
{
	std::string p1_n = p1.name();
	std::string p2_n = p2.name();
	int k = p1_n.compare( p2_n);

	return (k < 0 );
}


PointerItem::PointerItem( std::string name): m_name( name)
{ }

PointerItem::PointerItem( const PointerItem& peer): m_name( peer.m_name)
{ }

const PointerItem& PointerItem::operator=( const PointerItem& peer)
{
	if (&peer == this) return *this;
	m_name = peer.m_name;
	return *this;
}

bool PointerItem::operator==( const PointerItem& peer) const
{
	return m_name == peer.m_name;
}

bool PointerItem::operator!=( const PointerItem& peer) const
{
	return !operator==(peer);
}

const std::string& PointerItem::name() const
{
	return m_name;
}
