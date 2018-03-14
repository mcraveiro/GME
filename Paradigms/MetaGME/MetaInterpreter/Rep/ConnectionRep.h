#ifndef CONNECTIONREP_H
#define CONNECTIONREP_H

#include "FCO.h"
#include "ConnJoint.h"
#include "list"

/** Represents a Connection kind. */
class ConnectionRep : public FCO 
{
public: // constant strings
	static const std::string Color_str;//"IsAbstract"
	static const std::string ConnLineEnd_str;//"Icon"
	static const std::string ConnLineStart_str;//"PortIcon"
	static const std::string ConnLineType_str;//"Decorator"
	static const std::string LabelFormatStr_str;//"LabelFormatStr"
	static const std::string SrcAttrLabel1_str;//"SrcAttrLabel1"
	static const std::string SrcAttrLabel2_str;//"SrcAttrLabel2"
	static const std::string DstAttrLabel1_str;//"DstAttrLabel1"
	static const std::string DstAttrLabel2_str;//"DstAttrLabel2"

public:
	ConnectionRep( BON::FCO& ptr, BON::FCO& resp_ptr);
	~ConnectionRep();

	/*virtual*/ std::string doDump();

	inline Any::KIND_TYPE getMyKind() const { return Any::CONN; }

public:
	/*virtual*/ void initAttributes();
	void addJoint( ConnJoint & joint);
	void appendJointElements( const ConnJoint & joint);
	void inherit();
	bool checkConnectionTargets();
	void createConstraints( Sheet*);

	std::string dumpConnDetails();

protected:
	std::list<ConnJoint> m_jointList;

	std::string m_sAttrColor;
	std::string m_sAttrConnLineEnd;
	std::string m_sAttrConnLineStart;
	std::string m_sAttrConnLineType;
	std::string m_sAttrLabelFormatStr;
	std::string m_sAttrSrcAttrLabel1;
	std::string m_sAttrSrcAttrLabel2;
	std::string m_sAttrDstAttrLabel1;
	std::string m_sAttrDstAttrLabel2;

private: // forbiding copy
	ConnectionRep( const ConnectionRep&);
	const ConnectionRep& operator=( const ConnectionRep&);
};

#endif //CONNECTIONREP_H
