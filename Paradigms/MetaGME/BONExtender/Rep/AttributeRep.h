#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "Any.h"
#include "Method.h"
#include "logger.h"
class FCO;

class AttributeRep : public Any
{
public:
	typedef enum {
		ENUM,
		BOOL,
		FIELD
	} ATTR_TYPE;

	AttributeRep( BON::FCO& ptr);
	virtual ~AttributeRep();
	/*virtual*/ inline Any::KIND_TYPE  getMyKind() const { return Any::ATTRIBUTE; }
	/*virtual*/ std::string doDump() { return ""; }
	virtual Method createMethodForAttr( FCO* ) = 0;
	virtual Method createSetMethodForAttr( FCO* ) = 0;
	virtual std::string doDumpErroneousAttrHdr() = 0;
	virtual std::string doDumpErroneousAttrSrc( FCO*) = 0;

	virtual ATTR_TYPE getType() = 0;

	bool isGlobal();
	bool isViewable();
	std::string getPrompt();
	std::string getNameToUse();
	virtual std::string getMethodName() = 0;
	virtual std::string getSetMethodName() = 0;

	void addOwner( FCO * owner_fco);
	int hasAnyOwner() const;

protected:
	ATTR_TYPE attr;
	std::vector<FCO*> m_ownerList;

private:
	AttributeRep( const AttributeRep&);
	const AttributeRep& operator=( const AttributeRep &);
};


class EnumAttributeRep : public AttributeRep
{
public:
	EnumAttributeRep( BON::FCO& ptr);
	virtual ~EnumAttributeRep() { }
	ATTR_TYPE getType() { return ENUM; }
	bool getMenuItems();
	/*virtual*/ std::string doDumpErroneousAttrHdr();
	/*virtual*/ std::string doDumpErroneousAttrSrc( FCO*);
	/*virtual*/ Method createMethodForAttr( FCO* );
	/*virtual*/ Method createSetMethodForAttr( FCO* );
	/*virtual*/ std::string getMethodName();
	/*virtual*/ std::string getSetMethodName();
	/*static*/ std::string enumTypeName( EnumAttributeRep * a);
protected:
	std::vector<std::string> m_items;
	std::vector<std::string> m_itemsVal;
	int m_noOfItems;
	int m_noOfDefault;
};

class BoolAttributeRep : public AttributeRep
{
public:
	BoolAttributeRep( BON::FCO& ptr): AttributeRep( ptr) { }
	virtual ~BoolAttributeRep() { }
	ATTR_TYPE getType() { return BOOL; }
	/*virtual*/ std::string doDumpErroneousAttrHdr();
	/*virtual*/ std::string doDumpErroneousAttrSrc( FCO*);
	/*virtual*/ Method createMethodForAttr( FCO* );
	/*virtual*/ Method createSetMethodForAttr( FCO* );
	/*virtual*/ std::string getMethodName();
	/*virtual*/ std::string getSetMethodName();
};

class FieldAttributeRep : public AttributeRep
{
public:
	FieldAttributeRep( BON::FCO& ptr): AttributeRep( ptr) { }
	virtual ~FieldAttributeRep() { }
	ATTR_TYPE getType() { return FIELD; }
	/*virtual*/ std::string doDumpErroneousAttrHdr();
	/*virtual*/ std::string doDumpErroneousAttrSrc( FCO*);
	/*virtual*/ Method createMethodForAttr( FCO* );
	/*virtual*/ Method createSetMethodForAttr( FCO* );
	std::string getTypeStr();
	std::string getSetTypeStr();
	std::string getMethodStr();
	/*virtual*/ std::string getMethodName();
	/*virtual*/ std::string getSetMethodName();
};

#endif // ATTRIBUTE_H

