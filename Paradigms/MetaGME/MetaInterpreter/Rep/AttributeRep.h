#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "Any.h"
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
	/*virtual*/ std::string getName() const;
	/*virtual*/ std::string doDump() { return ""; }

	virtual std::string doDumpAttr(const std::string& mm = "") = 0;
	virtual ATTR_TYPE getType() = 0;

	bool isGlobal();
	bool isViewable();
	std::string getPrompt();
	std::string getMetaRef(const std::string & owner);
	std::string dumpHelp();
	void getXY( unsigned int * x, unsigned int *y) const;
	bool lessThan( const AttributeRep * rep) const;

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
	/*virtual*/ std::string doDumpAttr(const std::string& mm = "");

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
	/*virtual*/ std::string doDumpAttr(const std::string& mm = "");
};

class FieldAttributeRep : public AttributeRep
{
public:
	FieldAttributeRep( BON::FCO& ptr): AttributeRep( ptr) { }
	virtual ~FieldAttributeRep() { }
	ATTR_TYPE getType() { return FIELD; }
	/*virtual*/ std::string doDumpAttr(const std::string& mm = "");
};

#endif // ATTRIBUTE_H

