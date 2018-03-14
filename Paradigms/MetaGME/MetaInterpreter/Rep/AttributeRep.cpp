#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "AttributeRep.h"
#include "FCO.h"

#include <algorithm>
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

/*virtual*/ std::string AttributeRep::getName() const
{
	if ( this->m_ptr)
	{
		return m_ptr->getName();
	}
	return "NullPtrError";
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


std::string AttributeRep::dumpHelp()
{
	std::string mmm;
	std::string help = Dumper::xmlFilter( m_ptr->getAttribute("Help")->getStringValue());
	if( !help.empty())
		mmm += indStr() + "<regnode name=\"help\" value = \"" + help + "\"></regnode>\n";

	return mmm;
}


std::string AttributeRep::getMetaRef(const std::string & owner)
{
	std::string m_ref;
	std::string token = "";
	if ( !owner.empty()) token = "/" + owner;
	m_ref = askMetaRef(token);

	return m_ref;
}


void AttributeRep::getXY( unsigned int * x, unsigned int * y) const
{
	BON::Point p = BON::Atom( m_ptr)->getRegistry()->getLocation("Attributes");
	*x = p.first;
	*y = p.second;
}


bool AttributeRep::lessThan( const AttributeRep * rep) const
{
	std::string m = getName();
	unsigned int x, y;
	unsigned int rep_x, rep_y;
	getXY( &x, &y);
	rep->getXY( &rep_x, &rep_y);
	return ( y < rep_y || (y == rep_y && x < rep_x ));
}


void AttributeRep::addOwner( FCO * owner)
{
	std::vector<FCO*>::iterator it =
		std::find( m_ownerList.begin(), m_ownerList.end(), owner);

	if ( it == m_ownerList.end()) // not found so insert it
		m_ownerList.push_back( owner);
	else
		global_vars.err << MSG_ERROR << m_ptr << " attribute owned by " << owner->getPtr() << " twice\n";
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


std::string EnumAttributeRep::doDumpAttr(const std::string& owner)
{
	if (m_noOfItems == 0) getMenuItems();
	std::string mmm = "";

	std::string m_ref = getMetaRef( owner);
	std::string viewable = isViewable()?"": " viewable = \"no\"";
	mmm += indStr() + "<attrdef name=\"" + getName() + "\" metaref = \"" + m_ref + "\"" + viewable + " valuetype = \"enum\" defvalue = \"";
	if( m_noOfDefault < m_itemsVal.size())
		mmm += m_itemsVal[m_noOfDefault];
	else
		global_vars.err << MSG_ERROR << "Error: Default enumitem not found in vector of enumerated items. Please check: " << getPtr() << ".\n";
	
	mmm += "\">\n";
	
	++ind;

	std::string prompt = getPrompt();
	if ( !prompt.empty())
		mmm += indStr() + "<dispname>" + prompt + "</dispname>\n";

	for(int i = 0; i < m_noOfItems; ++i)
	{
		mmm+= indStr() + "<enumitem dispname = \"" + m_items[i] + "\" value = \"" + m_itemsVal[i] + "\"></enumitem>\n";
	}

	mmm += dumpHelp();

	--ind;
	mmm += indStr() + "</attrdef>\n";
	return mmm;
}


std::string BoolAttributeRep::doDumpAttr(const std::string& owner)
{
	std::string mmm = "";
	bool on;
	on = m_ptr->getAttribute("BooleanDefault")->getBooleanValue();

	std::string m_ref = getMetaRef( owner);
	std::string viewable = isViewable()?"": " viewable = \"no\"";


	mmm += indStr() + "<attrdef name=\"" + getName() + "\" metaref = \"" + m_ref + "\"" + viewable + " valuetype = \"boolean\" defvalue = \"";
	if (on) mmm += "true";
	else mmm +="false";
	mmm += "\">\n";
	
	++ind;

	std::string prompt = getPrompt();
	if ( !prompt.empty())
		mmm += indStr() + "<dispname>" + prompt + "</dispname>\n";

	mmm += dumpHelp();

	--ind;
	mmm += indStr() + "</attrdef>\n";
	return mmm;
}


std::string FieldAttributeRep::doDumpAttr(const std::string& owner)
{
	std::string mmm = "";
	std::string val_type = m_ptr->getAttribute("DataType")->getStringValue();
	std::string def_val = Dumper::xmlFilter( m_ptr->getAttribute("FieldDefault")->getStringValue());
	std::string content_type = Dumper::xmlFilter( m_ptr->getAttribute("ContentType")->getStringValue());
	int multiline = m_ptr->getAttribute("Multiline")->getIntegerValue();

	std::string m_ref = getMetaRef( owner);
	std::string viewable = isViewable()?"": " viewable = \"no\"";


	mmm += indStr() + "<attrdef name=\"" + getName() + "\" metaref = \"" + m_ref + "\"" + viewable + " valuetype = \"" + val_type + "\" defvalue = \"" + def_val + "\">\n";
	
	++ind;

	std::string prompt = getPrompt();
	if ( !prompt.empty())
		mmm += indStr() + "<dispname>" + prompt + "</dispname>\n";

	if( multiline > 1)
	{
		char mul[10];
		sprintf(mul, "%i", multiline);
		mmm += indStr() + "<regnode name=\"multiLine\" value = \"" + mul + "\"></regnode>\n";
	}

	if( !content_type.empty())
		mmm += indStr() + "<regnode name=\"content-type\" value = \"" + content_type + "\"></regnode>\n";

	mmm += dumpHelp();

	--ind;
	mmm += indStr() + "</attrdef>\n";
	return mmm;
}
