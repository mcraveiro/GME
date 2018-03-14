#ifndef CONNJOINT_H
#define CONNJOINT_H

#include "Any.h"
#include "RoleRep.h"
#include "PointerItem.h"
#include "FCO.h"
#include "Method.h"

class Sheet;
class ConnectionRep;

#include "list"

class ConnJoint 
{
public: // types
	typedef std::vector< FCO *> SDList;
	typedef std::vector< FCO *>::iterator SDList_Iterator;
	typedef std::vector< FCO *>::const_iterator SDList_ConstIterator;

	typedef std::vector< PointerItem> PointerItemSeries;
	typedef std::vector< PointerItem>::iterator PointerItemSeries_Iterator;
	typedef std::vector< PointerItem>::const_iterator PointerItemSeries_ConstIterator;

	typedef const ModelRep* Key;
	typedef std::map< Key, PointerItemSeries> TargetMap;
	typedef TargetMap::iterator TargetMap_Iterator;

public:
	static std::string m_srcLabel;
	static std::string m_dstLabel;

	ConnJoint( 
		ConnectionRep *, const SDList& op1, const SDList& op2, bool bidirect,
		std::string card1 = "", std::string card2 = ""
		);
	ConnJoint( const ConnJoint& peer);
	const ConnJoint& operator=( const ConnJoint& peer);
	~ConnJoint();

	void setConnectionPtr( ConnectionRep * conn_ptr);

	const SDList& getOp1() const;
	const SDList& getOp2() const;
	bool isBidirect() const;

	void intInherit( ModelRep * mod_ptr);

	// appends new role(s)
	void addTargetItem( int i, const ModelRep * model, const PointerItem& item);
	void addTargetItem( int i, const ModelRep * model, const RoleRep & new_role);
	void addTargetItem( int i, const ModelRep * model, const RoleRep::RoleRepSeries & new_role_series);

	bool checkElements( std::string connection_name);

	bool calcLCD();

	bool createLinkGetters();
	bool createSrcDstGetters();
	bool createEndGetters();

protected:

	// pointer to the container Connection, this ConnJoint is part of
	// beware: when copied during implementation inheritance the owner may change
	ConnectionRep * m_connPtr;

	// initial 
	SDList m_oper1;
	SDList m_oper2; 
	
	// final
	TargetMap m_oper1TargetMap;
	TargetMap m_oper2TargetMap;

	// if label1 == label2
	bool m_bidirect;

	// cardinality
	std::string m_oper1Card;
	std::string m_oper2Card;

	// least common denominators for SRC and DST
	SDList m_srcLCD;
	SDList m_dstLCD;
	//bool  m_same;

	// indicates whether the src or dst might be reference ports or not
	bool m_oper1IsAnyReferencePort;
	bool m_oper2IsAnyReferencePort;
};

#endif //CONNJOINT_H
