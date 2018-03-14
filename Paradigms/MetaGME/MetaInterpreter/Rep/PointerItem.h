#ifndef POINTERITEM_H
#define POINTERITEM_H

#include "string"
class PointerItem;

class PointerItemLex
{
public:
	bool operator()( const PointerItem& p1, const PointerItem& p2) const;
};


class PointerItem
{
private:
	std::string m_name;
public:
	PointerItem( std::string name);
	PointerItem( const PointerItem& peer);
	const PointerItem& operator=( const PointerItem& peer);
	bool operator ==( const PointerItem& peer) const;
	bool operator !=( const PointerItem& peer) const;
	const std::string& name() const;
};

#endif //POINTERITEM_H
