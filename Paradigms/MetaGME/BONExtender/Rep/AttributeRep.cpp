#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "AttributeRep.h"
#include "FCO.h"
#include "CodeGen.h"

#include "cctype"
#include "algorithm"

#include "globals.h"
#include "Dumper.h"
extern Globals global_vars;
extern int ind;


AttributeRep::AttributeRep( BON::FCO& ptr)
: Any( ptr) 
{
}


AttributeRep::~AttributeRep()
{
}


bool AttributeRep::isGlobal()
{
	return m_ptr->getAttribute("GlobalScope")->getBooleanValue();
}


bool AttributeRep::isViewable()
{
	return m_ptr->getAttribute("IsViewable")->getBooleanValue();
}


std::string AttributeRep::getPrompt()
{
	return Dumper::xmlFilter( m_ptr->getAttribute("Prompt")->getStringValue());
}

std::string AttributeRep::getNameToUse()
{
	std::string p = getName();
	
	Any::convertToValidName(p);
	return p;
}


void AttributeRep::addOwner( FCO * owner)
{
	std::vector<FCO*>::iterator it =
		std::find( m_ownerList.begin(), m_ownerList.end(), owner);

	if ( it == m_ownerList.end()) // not found so insert it
		m_ownerList.push_back( owner);
	else
		global_vars.err << getName() << " attribute owned by " << owner->getName() << " twice\n";
}


int AttributeRep::hasAnyOwner() const
{
	return m_ownerList.size();
}


EnumAttributeRep::EnumAttributeRep( BON::FCO& ptr)
: AttributeRep( ptr), m_noOfItems(0), m_noOfDefault(0)
{
	m_items.clear();
	m_itemsVal.clear();
}


bool EnumAttributeRep::getMenuItems()
{
/*
this method is capable of parsing enumerations like
elem1, value1
elem2
elem3, value3
, value4			-error case
elem4,				-error case


using special characters like: & < > " ' is not an error any more: are replaced with the xml escape codes
*/

	std::string items = Dumper::xmlFilter( m_ptr->getAttribute("MenuItems")->getStringValue());

	items += '\n'; // like an EOF sign
	std::string::size_type len = items.length();

	std::string whitespace = "\r\n\t ";
	std::string::size_type start_i = 0;
	while (start_i < len && whitespace.find(items[start_i]) != std::string::npos )
		++start_i;//skip heading multiple newlines

	std::string::size_type i = start_i, comma_i = 0;
	while( i < len )
	{
		if ( items[i] == '\n' && i - start_i > 0) // pass till end of line
		{ 
			if ( comma_i != 0) // there is value also
			{
				std::string::size_type st,end;
				// from start_i, to comma_i
				st = start_i;
				while( st < comma_i && whitespace.find( items[st]) != std::string::npos) ++st; //leading whsp
				end = comma_i-1;
				while( end > start_i && whitespace.find( items[end]) != std::string::npos) --end; //tailing whsp

				if ( end>=st)
					m_items.push_back(items.substr( st, end - st + 1));
				else
				{
					m_items.push_back("Error");
					global_vars.err << MSG_ERROR << "Error: Enumitem #" << 1 + m_noOfItems << " of " << getPtr() << " has wrong format. (If comma is used a second value is required.)\n";
				}
				
				// from comma_i+1 to i
				st = comma_i+1;
				while( st < i && whitespace.find( items[st]) != std::string::npos) ++st; //leading whsp
				end = i;
				while( end > comma_i+1 && whitespace.find( items[end]) != std::string::npos) --end; //tailing whsp
				
				if ( end>=st && comma_i+1 < i)
					m_itemsVal.push_back(items.substr( st, end - st + 1));
				else
				{
					m_itemsVal.push_back("Error");
					global_vars.err << MSG_ERROR << "Error: Enumitem #" << 1 + m_noOfItems << " of " << getPtr() << " has wrong format. (If comma is used a second value is required.)\n";
				}

				comma_i = 0;
			}
			else // no comma means no value
			{
				std::string::size_type st,end;
				// from start_i to i
				st = start_i;
				while( st < i && whitespace.find( items[st]) != std::string::npos) ++st; //leading whsp
				end = i;
				while( end > start_i && whitespace.find( items[end]) != std::string::npos) --end; //tailing whsp

				if ( end>=st)
				{
					m_items.push_back(items.substr( st, end - st + 1));
					m_itemsVal.push_back(items.substr( st, end - st + 1));
				}
			}
			++m_noOfItems; 
			start_i = i+1;
			while (start_i < len && whitespace.find(items[start_i]) != std::string::npos) ++start_i;//skip multiple newlines, tabs, spaces
			i = start_i-1;
		}
		else if ( items[i] == ',') 
		{ 
			comma_i = i;
		}
		++i;
	}

	if( m_noOfItems == 0)
		global_vars.err << MSG_ERROR << "Error: No enumeration items found. Please check: " << getPtr() << ".\n";

	bool filled_def_item = m_ptr->getAttribute( "DefaultItem")->getStatus() >= BON::AS_Here;
	std::string def_item = Dumper::xmlFilter( m_ptr->getAttribute("DefaultItem")->getStringValue());

	bool found ( false);
	i = 0;
	while (i < m_noOfItems && !found)
	{
		if ( m_items[i] == def_item ) found = true;
		else ++i;
	}

	if (found)
		m_noOfDefault = i;
	else if( filled_def_item)
		global_vars.err << MSG_ERROR << "Error: Default enumitem not found among enumerated items at " << getPtr() << ".\n";

	return true;
}


// obsolete -> really? question asked on 2/2/2004
/*static*/ std::string EnumAttributeRep::enumTypeName( EnumAttributeRep * a)
{
	std::string type = a->getNameToUse();
	type += "_Type";
	return type;
}


/*virtual*/ std::string EnumAttributeRep::getMethodName()
{
	return "get" + getNameToUse();
}


/*virtual*/ std::string EnumAttributeRep::getSetMethodName()
{
	return "set" + getNameToUse();
}


Method EnumAttributeRep::createMethodForAttr( FCO * container)
{
	Method m2;
	if (m_noOfItems == 0) getMenuItems();
	if (m_noOfItems == 0) return m2;//throw("Enumtype is empty");

	std::string mmm = "";
	mmm = CodeGen::indent(1) + "typedef enum\n" + CodeGen::indent(1) + "{";
	for(int i = 0; i < m_noOfItems; ++i)
	{
		std::string item_i = m_itemsVal[i] + '_' + EnumAttributeRep::enumTypeName( this);
		if ( !Any::checkIfValidName( item_i))
		{
			global_vars.err << "Invalid enumeration value \"" << item_i << "\" defined in " << EnumAttributeRep::enumTypeName( this) << ". Non alphanumeric characters replaced with underscore.\n";
			Any::convertToValidName( item_i);
		}

		if ( i != 0) mmm += ",";
		mmm += "\n" + CodeGen::indent(2) + item_i;
	}
	mmm += "\n" + CodeGen::indent(1) + "} " + EnumAttributeRep::enumTypeName( this) + ";\n";

	container->addClassGlobal( mmm);

	std::string src;

	src = CodeGen::indent(0) + "{\n";
	src += CodeGen::indent(1) + "std::string val = FCOImpl::getAttribute(\"" + getName() + "\")->getStringValue();\n";
	for( int i = 0; i < m_noOfItems; ++i)
	{
		std::string item_i = m_itemsVal[i] + '_' + EnumAttributeRep::enumTypeName( this);
		if ( !Any::checkIfValidName( item_i))
			Any::convertToValidName( item_i);

		src += "\n" + CodeGen::indent(1);
		if ( i != 0) src += "else ";
		src += "if ( val == \"" + m_itemsVal[i] + "\") return " + item_i + ";";
	}
	src += "\n" + CodeGen::indent(1) + "else throw(\"None of the possible items\");\n}\n\n\n";

	m2.m_returnValue = container->getLValidNameImpl() + "::" + EnumAttributeRep::enumTypeName( this);
	m2.m_signature = getMethodName() + "()";
	m2.m_container = container;
	m2.m_implementation = src;
	m2.m_comment = "";

	return m2;
}


Method EnumAttributeRep::createSetMethodForAttr( FCO * container)
{
	// we presume that the getter generator is called before this setter generator
	// for getMenuItems() 's sake
	Method m2;
	if (m_noOfItems == 0) return m2;//throw("Enumtype is empty");

	std::string src;

	src  = CodeGen::indent(0) + "{\n";
	src += CodeGen::indent(1) + "std::string str_val = \"\";\n\n";

	for( int i = 0; i < m_noOfItems; ++i)
	{
		std::string item_i = m_itemsVal[i] + '_' + EnumAttributeRep::enumTypeName( this);
		if ( !Any::checkIfValidName( item_i))
			Any::convertToValidName( item_i);

		src += CodeGen::indent(1);
		if ( i != 0) src += "else ";
		src += "if ( val == " + item_i + ") str_val = \"" + m_itemsVal[i] + "\";\n";
	}
	src += CodeGen::indent(1) + "else throw(\"None of the possible items\");\n";
	src += "\n";
	src += CodeGen::indent(1) + "FCOImpl::getAttribute(\"" + getName() + "\")->setStringValue( str_val);\n";
	src += "}\n\n\n";

	m2.m_returnValue = "void";
	m2.m_signature = getSetMethodName() + "( " + container->getValidNameImpl() + "::" + EnumAttributeRep::enumTypeName( this) + " val)";
	m2.m_container = container;
	m2.m_implementation = src;
	m2.m_comment = "";

	return m2;
}


/*virtual*/ std::string EnumAttributeRep::doDumpErroneousAttrHdr()
{
	std::string hdr;
	hdr = CodeGen::indent(1) + "/*virtual*/ inline std::string " + getMethodName() + "() ";
	hdr += "{ ";
	hdr += "throw std::string(\"Interface inherited kind. \" + getName() + \" doesn't have " + getNameToUse() + " attribute\");";
	hdr += " }\n";

	// the signature has to match the parent's getter's in order to override it
	hdr += "\n";
	//hdr += CodeGen::indent(1) + "/*virtual*/ inline void " + getSetMethodName() + "(" + container->getValidNameImpl() + "::" + EnumAttributeRep::enumTypeName( this) + " val)";
	// but the container ptr is not available here, we presume that the current object has the enum values
	// inherited in its own scope
	hdr += CodeGen::indent(1) + "/*virtual*/ inline void " + getSetMethodName() + "(" + EnumAttributeRep::enumTypeName( this) + " val)";
	hdr += "{ ";
	hdr += "throw std::string(\"Interface inherited kind. \" + getName() + \" doesn't have " + getNameToUse() + " attribute\");";
	hdr += " }\n";

	return hdr;
}


/*virtual*/ std::string EnumAttributeRep::doDumpErroneousAttrSrc( FCO*)
{
	return "";
}


/*virtual*/ std::string BoolAttributeRep::getMethodName()
{
	return "is" + getNameToUse();
}


/*virtual*/ std::string BoolAttributeRep::getSetMethodName()
{
	return "set" + getNameToUse();
}


Method BoolAttributeRep::createMethodForAttr( FCO * container)
{
	Method m;
	std::string src;
	src  = CodeGen::indent(0) + "{\n";
	src += CodeGen::indent(1) +   "return FCOImpl::getAttribute(\"" + getName() + "\")->getBooleanValue();\n";
	src += CodeGen::indent(0) + "}\n\n\n";

	m.m_returnValue = "bool";
	m.m_signature = getMethodName() + "() ";
	m.m_implementation = src;
	m.m_container = container;
	m.m_comment = "";
	return m;
}


Method BoolAttributeRep::createSetMethodForAttr( FCO * container)
{
	Method m;
	std::string src;
	src  = CodeGen::indent(0) + "{\n";
	src += CodeGen::indent(1) +   "FCOImpl::getAttribute(\"" + getName() + "\")->setBooleanValue( val);\n";
	src += CodeGen::indent(0) + "}\n\n\n";

	m.m_returnValue = "void";
	m.m_signature = getSetMethodName() + "( bool val)";
	m.m_implementation = src;
	m.m_container = container;
	m.m_comment = "";
	return m;
}


/*virtual*/ std::string BoolAttributeRep::doDumpErroneousAttrHdr()
{
	std::string hdr;
	hdr  = CodeGen::indent(1) + "/*virtual*/ inline bool " + getMethodName() + "() ";
	hdr += "{";
	hdr += "throw std::string(\"Interface inherited kind. \" + getName() + \" doesn't have " + getNameToUse() + " attribute\");";
	hdr += "}\n";

	// the signature has to match the parent's getter's in order to override it
	hdr += "\n";
	hdr += CodeGen::indent(1) + "/*virtual*/ inline void " + getSetMethodName() + "( bool) ";
	hdr += "{";
	hdr += "throw std::string(\"Interface inherited kind. \" + getName() + \" doesn't have " + getNameToUse() + " attribute\");";
	hdr += "}\n";


	return hdr;
}


/*virtual*/ std::string BoolAttributeRep::doDumpErroneousAttrSrc( FCO*)
{
	return "";
}


/*virtual*/ std::string FieldAttributeRep::getMethodName()
{
	return "get" + getNameToUse();
}


/*virtual*/ std::string FieldAttributeRep::getSetMethodName()
{
	return "set" + getNameToUse();
}


std::string FieldAttributeRep::getTypeStr()
{
	std::string val_type = m_ptr->getAttribute("DataType")->getStringValue();
	if ( val_type == "integer")
		return "long";
	else if ( val_type == "double")
		return "double";
	else if ( val_type == "string")
		return "std::string";

	return "Error";
}


std::string FieldAttributeRep::getSetTypeStr()
{
	std::string val_type = m_ptr->getAttribute("DataType")->getStringValue();
	if ( val_type == "integer")
		return "const long";
	else if ( val_type == "double")
		return "const double";
	else if ( val_type == "string")
		return "const std::string&";

	return "Error";
}


std::string FieldAttributeRep::getMethodStr()
{
	std::string val_type = m_ptr->getAttribute("DataType")->getStringValue();
	if ( val_type == "integer")
		return "Integer";
	else if ( val_type == "double")
		return "Real";
	else if ( val_type == "string")
		return "String";

	return "Error";
}


Method FieldAttributeRep::createMethodForAttr( FCO * container)
{
	Method m;
	std::string src;
	src  = CodeGen::indent(0) + "{\n";
	src += CodeGen::indent(1) +   "return FCOImpl::getAttribute(\"" + getName() + "\")->get" + getMethodStr() + "Value();\n";
	src += CodeGen::indent(0) + "}\n\n\n";

	m.m_returnValue = getTypeStr();
	m.m_signature = getMethodName() + "() ";
	m.m_implementation = src;
	m.m_container = container;
	m.m_comment = "";

	return m;
}


Method FieldAttributeRep::createSetMethodForAttr( FCO * container)
{
	Method m;
	std::string src;
	src  = CodeGen::indent(0) + "{\n";
	src += CodeGen::indent(1) +   "FCOImpl::getAttribute(\"" + getName() + "\")->set" + getMethodStr() + "Value( val);\n";
	src += CodeGen::indent(0) + "}\n\n\n";

	m.m_returnValue = "void";
	m.m_signature = getSetMethodName() + "( " + getSetTypeStr() + " val)";
	m.m_implementation = src;
	m.m_container = container;
	m.m_comment = "";

	return m;
}


/*virtual*/ std::string FieldAttributeRep::doDumpErroneousAttrHdr()
{
	std::string hdr;
	hdr  = CodeGen::indent(1) + "/*virtual*/ inline " + getTypeStr() + " " + getMethodName() + "() ";
	hdr += "{";
	hdr += "throw std::string(\"Interface inherited kind. \" + getName() + \" doesn't have " + getNameToUse() + " attribute\");";
	hdr += "}\n";

	// the signature has to match the parent's getter's in order to override it
	hdr += "\n";
	hdr += CodeGen::indent(1) + "/*virtual*/ inline void " + getSetMethodName() + "( " + getSetTypeStr() + ") ";
	hdr += "{";
	hdr += "throw std::string(\"Interface inherited kind. \" + getName() + \" doesn't have " + getNameToUse() + " attribute\");";
	hdr += "}\n";

	return hdr;
}


/*virtual*/ std::string FieldAttributeRep::doDumpErroneousAttrSrc( FCO*)
{
	return "";
}