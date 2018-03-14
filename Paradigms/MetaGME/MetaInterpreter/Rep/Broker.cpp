#include "stdafx.h"

#include "logger.h"
#include "Broker.h"
#include "AspectRep.h"
#include "map"
#include "Exceptions.h"

/* static vars to be initialized */
int	Broker::m_constraintId = 1;
const int Broker::INVALID_METAREF = -1;
const int Broker::INITIAL_METAREF = 1001; // the next available metaref
const std::string Broker::ROOTFOLDER_METAREF_STR = "1000"; // used by the "RootFolder"
int	Broker::m_metaRefId = INITIAL_METAREF;
int Broker::m_firstFree = INITIAL_METAREF;
std::list<Broker::MetaRefNode> Broker::m_metaRefDB;


/*This method resets all static variables of Broker. Called by the Dumper::~Dumper*/
void Broker::reset()
{
	m_metaRefDB.clear();
	m_firstFree = INITIAL_METAREF;

	m_constraintId = 0;
	m_metaRefId = INITIAL_METAREF;
}


void Broker::init()
{
	// this is called after the actual project registry has been scanned through
	// and the values can be set based on the information found
	m_constraintId = 1;
	m_metaRefId = (m_firstFree > INITIAL_METAREF)?m_firstFree : INITIAL_METAREF;
	/*char d[10];
	sprintf(d, "%i", m_metaRefId);
	global_vars.err <<  std::string(d) << " used as max metaref\n";*/

	m_metaRefDB.clear(); // after we know the greatest metaref this can be deleted
}


int Broker::getNextConstraintId()
{
	return m_constraintId++;
}


int Broker::getNextMetaRefId()
{
	return m_metaRefId++;
}


std::string Broker::getRegistryTokenName( const BON::Object& obj)
{
	std::string kind = obj->getObjectMeta().name();
	std::string name = obj->getPath();

	BON::ObjectPtr p = obj->getObjectI();
	long relid = 0;
	BONCOMTHROW( p->get_RelID(&relid));
	char t[16];
	sprintf( t, "%x", relid);

	ASSERT( name.length() >= 1);
	std::string tmp = name.substr(1) + '-' + t + '-' + kind;
	//return kind + "s" + name + t;
	return tmp;
}


void Broker::initFromObj( BON::Object& obj, BON::Folder& folder, const std::string& kind)
{
	// any modification in the registry policy should be syched with
	// the Any::getMyPrefix() Broker::initFromObj method

	std::string token = Broker::getRegistryTokenName( obj) + "/MetaRef";
	initFromRegistry( obj, folder, folder->getRegistry()->getDescendantByPath( token));
}

// folder is the root folder
void Broker::initFromAspectObj( BON::Object& obj, const std::string& name, BON::Folder& folder)
{
	// any modification in the registry policy should be syched with
	// the Broker::initFromObj method
	//std::string token = obj->getID() + ":" + obj->getName() + "/MetaRef";
	std::string token = AspectRep::m_aspectRegistryRoot + '/' + AspectRep::m_aspectMetaRefsRoot + '/' + name + "/MetaRef";
	initFromRegistry( obj, folder, folder->getRegistry()->getDescendantByPath( token));
}

void Broker::initFromRegistry(  BON::Object& obj, BON::Folder& folder, const BON::RegistryNode& reg_node)
{
	if ( reg_node->getStatus() == BON::RNS_Here)
	{
		int k = reg_node->getIntegerValue();
		MetaRefNode node( obj, folder, reg_node->getPath(), reg_node->getIntegerValue());
		m_metaRefDB.push_back( node);
		m_firstFree = max( m_firstFree, node.metaref + 1);
	}

	std::set<BON::RegistryNode> children = reg_node->getChildren();
	std::set<BON::RegistryNode>::iterator c_it = children.begin();
	for(; c_it != children.end(); ++c_it)
		initFromRegistry( obj, folder, *c_it);
}


void Broker::checkDuplicates()
{
	// map of metaref_int, and metaRefDB::iterator
	// since the list allows removing elements without invalidating iterators
	std::map< int, MetaRefDB_Iterator > check_map;

	MetaRefDB_Iterator it = m_metaRefDB.begin();
	for( ; it != m_metaRefDB.end(); ++it)
	{
		MetaRefNode node = *it;

		std::map< int, MetaRefDB_Iterator >::iterator map_it = check_map.end();
		map_it = check_map.find( node.metaref); 

		if ( map_it != check_map.end()) // same metaref found in the check_map
		{
			MetaRefDB_Iterator jt = map_it->second;
			MetaRefNode otherNode = *jt;
			int t_1 = otherNode.metaref;
			bool flag = false;
			std::string m1 = node.obj->getID();
			std::string m2 = otherNode.obj->getID();
			//bool similar = (node.obj == otherNode.obj);//t
			std::string p = node.obj->getName();
			if ( m1 < m2)
			{
				m_metaRefDB.erase( jt);
				check_map[ node.metaref] = it;
				flag = true;
			}
			else
			{
				it = m_metaRefDB.erase(it); // "it" will point to the next element
				--it; // decrement in order to stay consistent within the for cycle
				flag = false;
			}
			std::string token = (flag ? otherNode : node).path.substr(1);
			try {
				//(flag ? otherNode : node).obj->getRegistry()->getDescendantByPath( dpath.substr(1))->clear(); // reset the respective node
				if ( flag)
					otherNode.folder->getRegistry()->getDescendantByPath( token)->clear(); //reset
				else
					node.folder->getRegistry()->getDescendantByPath( token)->clear(); //reset
#ifdef _DEBUG
				std::string fmtstr = std::string("Duplicate metaref found. Class ") + (flag ? otherNode : node).obj->getName() +
					" subnode: " + std::string((flag ? otherNode : node).path.empty() ? "n/a" : (flag ? otherNode : node).path + " has been reset to Undefined value");
				std::string n1str = "Name" + node.obj->getName() + " path: " + node.path;
				std::string n2str = "Name" + otherNode.obj->getName() + " path: " + otherNode.path;
				//if (t_1) global_vars.err << fmtstr << "\n" << n1str << "\n" << n2str << "\n";
#endif
			}
			catch(...) {
				// in case of library fco-s the operation above is causing an assertion/exception
			}
		}
		else 
		{
			check_map[node.metaref] = it;
		}
	}
}

