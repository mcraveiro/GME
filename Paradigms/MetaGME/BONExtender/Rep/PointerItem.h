#ifndef POINTERITEM_H
#define POINTERITEM_H

#include "FCO.h"

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
	FCO * m_fcoPtr;
public:
	PointerItem( std::string name, FCO* fco = 0);
	PointerItem( const PointerItem& peer);
	const PointerItem& operator=( const PointerItem& peer);
	bool operator ==( const PointerItem& peer) const;
	bool operator !=( const PointerItem& peer) const;
	const std::string& name() const;
	FCO * fcoPtr() const; //<!> is this needed?
};

#endif //POINTERITEM_H
