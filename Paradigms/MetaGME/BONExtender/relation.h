#ifndef RELATION_H
#define RELATION_H

#include "string"
#include "BONImpl.h"


class Relation
{
public:
	// basic relations 
	static const std::string containment_str;//"Containment"
	static const std::string folder_containment_str;//"FolderContainment"
	static const std::string set_membership_str;//"SetMembership"
	static const std::string refer_to_str;//"ReferTo"
	static const std::string has_aspect_str;//"HasAspect"
	static const std::string aspect_member_str;//"AspectMember"
	static const std::string has_constraint_str;//"HasConstraint"
	static const std::string has_attribute_str;//"HasAttribute"

	// complex relations
	static const std::string connector_str;//("Connector");
	static const std::string connector_src;//("SourceToConnector");
	static const std::string connector_dst;//("ConnectorToDestination");
	static const std::string connector_descr;//("AssociationClass");

	static const std::string equivalence_str;//("Equivalence");
	static const std::string equivalence_right;//("EquivRight");
	static const std::string equivalence_left;//("EquivLeft");

	static const std::string inheritance_str;//("Inheritance");
	static const std::string inheritance_base;//("BaseInheritance");
	static const std::string inheritance_derived;//("DerivedInheritance");

	static const std::string int_inheritance_str;//("InterfaceInheritance");
	static const std::string int_inheritance_base;//("BaseIntInheritance");
	static const std::string int_inheritance_derived;//("DerivedIntInheritance");

	static const std::string imp_inheritance_str;//("ImplementationInheritance");
	static const std::string imp_inheritance_base;//("BaseImpInheritance");
	static const std::string imp_inheritance_derived;//("DerivedImpInheritance");

	static const std::string same_folder_str;//("SameFolder");
	static const std::string same_folder_right;//("SameFolderRight");
	static const std::string same_folder_left;//("SameFolderLeft");

	static const std::string same_aspect_str;//("SameAspect");
	static const std::string same_aspect_right;//("SameAspectRight");
	static const std::string same_aspect_left;//("SameAspectLeft");


	typedef enum
	{
		// connections which make direct relationship
		CONTAINMENT_OP,
		FOLDER_CONTAINMENT_OP,
		SET_MEMBER_OP,
		REFER_TO_OP,
		HAS_ASPECT_OP,
		ASPECT_MEMBER_OP,
		HAS_CONSTRAINT_OP,
		HAS_ATTRIBUTE_OP,
		// connections which establish indirect relationship
		ASSOCIATION_OP, // replaces CONNECTOR
		EQUIVALENCE_OP, // replaces EquivLeft, EquivRight // elim
		INHERITANCE_OP, 
		INT_INHERITANCE_OP,
		IMP_INHERITANCE_OP,
		SAME_FOLDER_OP,	// elim
		SAME_ASPECT_OP  // elim
	} OPER_TYPE;

	inline std::string getOperationStr() const {
		const std::string strings[] = {
			"CONTAINMENT_OP",
			"FOLDER_CONTAINMENT_OP", 
			"SET_MEMBER_OP",
			"REFER_TO_OP",
			"HAS_ASPECT_OP",
			"ASPECT_MEMBER_OP",
			"HAS_CONSTRAINT_OP",
			"HAS_ATTRIBUTE_OP",
			"ASSOCIATION_OP", 
			"EQUIVALENCE_OP", 
			"INHERITANCE_OP",
			"INT_INHERITANCE_OP",
			"IMP_INHERITANCE_OP",
			"SAME_FOLDER_OP",
			"SAME_ASPECT_OP"
		};
		return strings[m_operation]; 
	}

	inline OPER_TYPE getOperation() const { return m_operation; }
	inline const BON::FCO getOp1() const { return m_operand1; }
	inline const BON::FCO getOp2() const { return m_operand2; }
	inline const BON::FCO getOp3() const { return m_operand3; }
	inline const BON::FCO getOp4() const { return m_operand4; }
	inline const BON::FCO getOp5() const { return m_operand5; }

	inline void setOperation( OPER_TYPE oper_t) { m_operation = oper_t; }
	inline void setOp1(const BON::FCO op_1) { m_operand1 = op_1; }
	inline void setOp2(const BON::FCO op_2) { m_operand2 = op_2; }
	inline void setOp3(const BON::FCO op_3) { m_operand3 = op_3; }
	inline void setOp4(const BON::FCO op_4) { m_operand4 = op_4; }
	inline void setOp5(const BON::FCO op_5) { m_operand5 = op_5; }

	explicit Relation( 
		OPER_TYPE oper_t, 
		const BON::FCO operd1 = BON::FCO(),
		const BON::FCO operd2 = BON::FCO(),
		const BON::FCO operd3 = BON::FCO(),
		const BON::FCO operd4 = BON::FCO(),
		const BON::FCO operd5 = BON::FCO())
		:	m_operation( oper_t),
		m_operand1( operd1),
		m_operand2( operd2),
		m_operand3( operd3),
		m_operand4( operd4),
		m_operand5( operd5) { };

	~Relation() { }
	
	Relation( const Relation & operand):
		m_operation( operand.m_operation),
		m_operand1( operand.m_operand1),
		m_operand2( operand.m_operand2),
		m_operand3( operand.m_operand3),
		m_operand4( operand.m_operand4),
		m_operand5( operand.m_operand5) { };
	
	const Relation & operator=(const Relation & operand)
	{
		if ( this == &operand) return *this;
		m_operation = operand.m_operation;
		m_operand1 = operand.m_operand1;
		m_operand2 = operand.m_operand2;
		m_operand3 = operand.m_operand3;
		m_operand4 = operand.m_operand4;
		m_operand5 = operand.m_operand5;
		return *this;
	}

	bool operator==( const Relation& peer) 
	{
		return (m_operation==peer.m_operation) && 
			(m_operand1 == peer.m_operand1) && 
			(m_operand2 == peer.m_operand2) &&
			(m_operand3 == peer.m_operand3) && 
			(m_operand4 == peer.m_operand4) &&
			(m_operand5 == peer.m_operand5);
	}

	bool operator<( const Relation& peer) const
	{
		if ( m_operation != peer.m_operation) return m_operation < peer.m_operation;
		
		int k;
		k = m_operand1->getName().compare( peer.m_operand1->getName());
		if( k != 0) return k < 0;
		k = m_operand2->getName().compare( peer.m_operand2->getName());
		if( k != 0) return k < 0;
		k = m_operand3->getName().compare( peer.m_operand3->getName());
		return ( k < 0);
	}

private:
	OPER_TYPE m_operation;
	BON::FCO m_operand1;
	BON::FCO m_operand2;
	BON::FCO m_operand3;
	BON::FCO m_operand4;
	BON::FCO m_operand5;
};


#endif //RELATION_H


