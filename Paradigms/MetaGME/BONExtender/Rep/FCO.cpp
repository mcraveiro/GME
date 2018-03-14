#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "FCO.h"
#include "ReferenceRep.h"
#include "CodeGen.h"
#include "logger.h"
#include "list"
#include "algorithm"

#include "globals.h"
extern Globals global_vars;

/*static*/ const std::string FCO::IsAbstract_str = "IsAbstract";

FCO::FCO( BON::FCO& ptr, BON::FCO& resp_ptr)
	: Any( ptr)
	, m_respPointer( resp_ptr)
	, m_isAbstract( false)
	, m_references()
	, m_finalReferences()
	, m_partOf()
	, m_partOfFinal()
	, m_initialAttributeList()
	, m_multipleBaseClasses()
	, m_virtualBaseClasses()
	, m_classGlobalPart()
	, m_connectionMethods()
	, m_attributeMethods()
	, m_extedAnc( 0)
	, m_nonExtedDescs()
{ 
	for ( int i = 0; i < NUMBER_OF_INHERITANCES; ++i)
	{
		m_childList[i].clear();
		m_parentList[i].clear();
		m_ancestors[i].clear();
		m_descendants[i].clear();
		m_level[i] = 0;
		m_clique[i] = 0;
	}
}


FCO::~FCO() 
{ 
	m_respPointer = BON::FCO();
	m_references.clear();
	m_finalReferences.clear();
	m_partOf.clear();
	m_partOfFinal.clear();
	m_initialAttributeList.clear();

	m_multipleBaseClasses.clear();
	m_virtualBaseClasses.clear();

	m_connectionMethods.clear();
	m_attributeMethods.clear();

	m_extedAnc = 0;
	m_nonExtedDescs.clear();

	for ( int i = 0; i < NUMBER_OF_INHERITANCES; ++i)
	{
		m_childList[i].clear();
		m_parentList[i].clear();
		m_ancestors[i].clear();
		m_descendants[i].clear();
		m_level[i] = 0;
		m_clique[i] = 0;
	}
}

std::string FCO::getName() const
{
	if( this->m_respPointer) // there are equivalents
	{
		std::string regname = getMIRegistry()->getValueByPath( "/" + Any::NameSelectorNode_str);
		if( !regname.empty()) return regname;
		else return m_respPointer->getName();
	}
	else if ( this->m_ptr)
	{
		return m_ptr->getName();
	}
	return "NullPtrError";
}


void FCO::initAttributes()
{
	m_isInRootFolder = m_isInRootFolder || m_ptr->getAttribute( InRootFolder_str)->getBooleanValue();

	// isAbstract: true iff all values (which are set) are true
	bool isabs = true; // if any of the set values is false it will change
	bool isabs_set = false;
	//m_isAbstract = m_isAbstract && m_ptr->getAttribute( IsAbstract_str)->getBooleanValue();
	if( m_ptr->getAttribute( IsAbstract_str)->getStatus() == BON::AS_Here)
	{
		isabs = isabs && m_ptr->getAttribute( IsAbstract_str)->getBooleanValue();
		isabs_set = true;
	}

	// abstract iff all equivalent classes are abstract
	std::set< BON::FCO >::const_iterator it = m_equivs.begin();
	for ( ; it != m_equivs.end(); ++it)
	{
		if ( *it == m_ptr) continue;
		
		// --the following 4 attributes are applicable to proxies as well--
		// InRootFolder: true if one is at least true
		m_isInRootFolder = m_isInRootFolder || (*it)->getAttribute( InRootFolder_str)->getBooleanValue();

		// isAbstract: true if all objects are true
		//m_isAbstract = m_isAbstract && (*it)->getAttribute( IsAbstract_str)->getBooleanValue();
		if( (*it)->getAttribute( IsAbstract_str)->getStatus() == BON::AS_Here) // if set by the user
		{
			isabs = isabs && (*it)->getAttribute( IsAbstract_str)->getBooleanValue();
			isabs_set = true;
		}
	}
	
	if( isabs_set) m_isAbstract = isabs;

}


bool FCO::isAbstract() const
{
	return m_isAbstract;
}


void FCO::abstract( bool is)
{
	m_isAbstract = is;
}


void FCO::iAmPartOf( ModelRep * mod_ptr)
{
	std::vector<ModelRep *>::iterator jt = 
		std::find( m_partOf.begin(), m_partOf.end(), mod_ptr);

	// not inserting two times
	if (jt == m_partOf.end())
		m_partOf.push_back( mod_ptr);
	else { }
}


void FCO::iAmPartOfFinal( ModelRep * mod_ptr)
{
	std::vector<ModelRep *>::iterator jt = 
		std::find( m_partOfFinal.begin(), m_partOfFinal.end(), mod_ptr);

	// not inserting two times
	if (jt == m_partOfFinal.end() )
		m_partOfFinal.push_back( mod_ptr);
	else { }
}


const FCO::ModelRepPtrList& FCO::modelsIAmPartOf()
{
	return m_partOf;
}


const FCO::ModelRepPtrList& FCO::modelsIAmPartOfFinal()
{
	return m_partOfFinal;
}


bool FCO::amIPartOf(const ModelRep * mod_ptr) const
{
	ModelRepPtrList_ConstIterator it = 
		std::find( m_partOf.begin(), m_partOf.end(), mod_ptr);
	return ( it != m_partOf.end());
}


bool FCO::amIPartOfFinal(const ModelRep * mod_ptr) const
{
	ModelRepPtrList_ConstIterator it = 
		std::find( m_partOfFinal.begin(), m_partOfFinal.end(), mod_ptr);
	
	return ( it != m_partOfFinal.end());
}


// returns if this is not part of any model
bool FCO::checkIsPartOf()
{
	return !m_partOf.empty();
}


bool FCO::checkIsPartOfFinal()
{
	return !m_partOfFinal.empty();
}


bool FCO::checkInheritance()
{
	bool same_kind = true;
	KIND_TYPE kind_type = getMyKind();

	INHERITANCE_TYPE type[ NUMBER_OF_INHERITANCES ] ={ REGULAR, INTERFACE, IMPLEMENTATION};
	for(int i = 0; i < NUMBER_OF_INHERITANCES && same_kind; ++i)
	{
		INHERITANCE_TYPE inh_type = type[i];
		std::vector<FCO*> * vectors[] = {
			&m_childList[inh_type], 
			&m_parentList[inh_type], 
			&m_ancestors[inh_type], 
			&m_descendants[inh_type] 
		};
		for(int k = 0; k < 4 && same_kind; ++k)
		{
			std::vector<FCO*>::iterator it = vectors[k]->begin();
			for( ; it != vectors[k]->end() && same_kind; ++it)
				if ((*it)->getMyKind() != FCO_REP)
					same_kind = same_kind && kind_type == (*it)->getMyKind();
		}
	}
	if (!same_kind) 
		global_vars.err << "ERROR: \"" << getName() << "\" fco has another kind of ancestor or descendant.\n";
	return same_kind;
}


// initial reference list
void FCO::addRefersToMe( ReferenceRep * ref_obj)
{
	m_references.push_back( ref_obj);
}


const FCO::ReferenceRepList& FCO::getReferences() const
{
	return m_references;
}

// final reference list
void FCO::addFinalRefersToMe( ReferenceRep * ref_obj)
{
	if( std::find( m_finalReferences.begin(), m_finalReferences.end(), ref_obj) == m_finalReferences.end())
		m_finalReferences.push_back( ref_obj);
}


const FCO::ReferenceRepList& FCO::getFinalReferences() const
{
	return m_finalReferences;
}


/*
Dealing with the case when R1->R2->R3->M. 
They all have similar aspects, and connecting is allowed through these.
Previously named getAllMyReferences
*/
FCO::ReferenceRepList FCO::getTransitiveReferencesToMe() const
{
	ReferenceRepList multiple_refs = this->getFinalReferences();
	std::list< ReferenceRep * > ref_list;
	ref_list.insert( ref_list.end(), multiple_refs.begin(), multiple_refs.end());

	while ( !ref_list.empty())
	{
		ReferenceRep * r = *ref_list.begin();
		ref_list.pop_front();
		std::vector< ReferenceRep *> temp = r->getFinalReferences();

		std::vector<ReferenceRep *>::iterator temp_it = temp.begin();
		for( ; temp_it != temp.end(); ++temp_it)
		{
			if( std::find( multiple_refs.begin(), multiple_refs.end(), *temp_it) == multiple_refs.end()) // not found
			{
				multiple_refs.push_back( *temp_it); // multiple_refs is growing
				ref_list.push_back( *temp_it); // ref_list contains the new elements
			}
		}
	}
	return multiple_refs;
}


//
// inheritance related methods
//
void FCO::addParent( INHERITANCE_TYPE type, FCO * ptr) 
{ 

	//checking for multiple instances of the same base class
	std::vector<FCO *>::iterator jt = 
		std::find( m_parentList[type].begin(), m_parentList[type].end(), ptr);

	// not inserting two times
	if (jt == m_parentList[type].end())
		m_parentList[type].push_back( ptr);
	else 
	{
		global_vars.err << "CHECK: " << (*jt)->getName() <<
			" base class is two times in direct inheritance " <<
			" relation with the derived class " <<
			getName() << "\n";
	}
}


void FCO::addChild( INHERITANCE_TYPE type,  FCO * ptr) 
{ 
	std::vector<FCO *>::iterator jt = 
		std::find( m_childList[type].begin(), m_childList[type].end(), ptr);
	
	// not inserting two times
	if (jt == m_childList[type].end())
		m_childList[type].push_back( ptr);
	else 
	{ }	// error already noticed by addParent
}


const std::vector<FCO *>&  FCO::getParents( INHERITANCE_TYPE type) const
{
	return m_parentList[type];
}


const std::vector<FCO *>& FCO::getChildren( INHERITANCE_TYPE type) const
{
	return m_childList[type];
}


bool FCO::hasParent( const FCO * par, INHERITANCE_TYPE type) const
{
	bool has = false;
	if ( type != REGULAR)
	{
		std::vector<FCO*>::const_iterator it = 
			std::find( m_parentList[REGULAR].begin(), m_parentList[REGULAR].end(), par);

		if ( it != m_parentList[REGULAR].end()) // found
			has = has || true;
	}
	if (has) return has;

	std::vector<FCO*>::const_iterator it = 
		std::find( m_parentList[type].begin(), m_parentList[type].end(), par);

	if ( it != m_parentList[type].end()) // found
		has = has || true;

	return has;
}


void FCO::setLevel( INHERITANCE_TYPE type, int level)
{
	ASSERT( type != REGULAR);
	m_level[type] = level;
}


int FCO::getLevel( INHERITANCE_TYPE type)
{
	ASSERT( type != REGULAR);
	return m_level[type];
}


void FCO::setCliqueId( INHERITANCE_TYPE type, int cl)
{
	ASSERT( type != REGULAR);
	m_clique[type] = cl;
}


int FCO::getCliqueId( INHERITANCE_TYPE type)
{
	ASSERT( type != REGULAR);
	return m_clique[type];
}


void FCO::setExtedAnc( FCO * ptr)
{
	m_extedAnc = ptr;
	if( ptr) ptr->addNonExtedDesc( this);
}


FCO * FCO::getExtedAnc() const
{
	return m_extedAnc;
}


void FCO::addNonExtedDesc( FCO * ptr)
{
	ASSERT( ptr);
	if( m_nonExtedDescs.end() == std::find( m_nonExtedDescs.begin(), m_nonExtedDescs.end(), ptr))
		m_nonExtedDescs.push_back( ptr);
}


const std::vector<FCO*>& FCO::getNonExtedDescVector() const
{
	return m_nonExtedDescs;
}


std::string FCO::dumpNonExtedDescKinds() const
{
	if ( m_nonExtedDescs.empty()) return "";
	std::string res;
	std::vector<FCO*>::const_iterator it = m_nonExtedDescs.begin();
	for( ; it != m_nonExtedDescs.end(); ++it)
		if ( !(*it)->isAbstract())
			res += ' ' + (*it)->getLStrictNmspc() + (*it)->getName();

	return res;
}

FCO* FCO::findRspPtr( const std::string& responsible)
{
	FCO* found_anc = 0;
	std::vector<FCO *> ancestor;
	getIntAncestors( ancestor);
	
	for( unsigned int i = 0; !found_anc && i < ancestor.size(); ++i)
	{
		if( getMyKind() == ancestor[i]->getMyKind() && ancestor[i]->getName() == responsible) // it is the same kind that fco_ptr
			found_anc = ancestor[i];
	}
	
	return found_anc; // it is a valid ancestor or 0
}

void FCO::setAncestors( INHERITANCE_TYPE type, const std::vector<FCO*> &anc_list)
{
	ASSERT( type != REGULAR);
	//global_vars.err << getName() << " . # of ancest: " << anc_list.size() << " with type: "<< type << "\n";

	m_ancestors[type].clear();
	m_ancestors[type].insert( m_ancestors[type].end(), anc_list.begin(), anc_list.end());
}


void FCO::setDescendants( INHERITANCE_TYPE type, const std::vector<FCO*> &desc_list)
{
	ASSERT( type != REGULAR);
	//global_vars.err << getName() << " . # of desc: " << desc_list.size() << " with type: "<< type << "\n";

	m_descendants[type].clear();
	m_descendants[type].insert( m_descendants[type].end(), desc_list.begin(), desc_list.end());
}


void FCO::getIntAncestors( std::vector<FCO*> & ancestors) const
{
	ancestors.insert( ancestors.end(), m_ancestors[INTERFACE].begin(), m_ancestors[INTERFACE].end());	
}


void FCO::getIntDescendants( std::vector<FCO*> & descendants) const
{
	descendants.insert( descendants.end(), m_descendants[INTERFACE].begin(), m_descendants[INTERFACE].end());	
}


void FCO::getImpAncestors( std::vector<FCO*> & ancestors) const
{
	ancestors.insert( ancestors.end(), m_ancestors[IMPLEMENTATION].begin(), m_ancestors[IMPLEMENTATION].end());	
}


void FCO::getImpDescendants( std::vector<FCO*> & descendants) const
{
	descendants.insert( descendants.end(), m_descendants[IMPLEMENTATION].begin(), m_descendants[IMPLEMENTATION].end());	
}


std::vector<FCO *> FCO::getAllChildren() const
{
	std::vector<FCO*> rr = getChildren( FCO::REGULAR);
	rr.insert( rr.end(), getChildren( FCO::INTERFACE).begin(), getChildren( FCO::INTERFACE).end());
	rr.insert( rr.end(), getChildren( FCO::IMPLEMENTATION).begin(), getChildren( FCO::IMPLEMENTATION).end());
	return rr;
}

std::vector<FCO *> FCO::getAllParents() const
{
	std::vector<FCO*> rr = getParents( FCO::REGULAR);
	rr.insert( rr.end(), getParents( FCO::INTERFACE).begin(), getParents( FCO::INTERFACE).end());
	rr.insert( rr.end(), getParents( FCO::IMPLEMENTATION).begin(), getParents( FCO::IMPLEMENTATION).end());
	return rr;
}

std::vector<FCO *> FCO::getAllAncestors() const
{
	std::vector<FCO *> impl, intf;
	getImpAncestors( impl);
	getIntAncestors( intf);

	std::vector<FCO *>::iterator it = intf.begin();
	for( ; it != intf.end(); ++it) // check all int
	{
		if ( impl.end() == std::find( impl.begin(), impl.end(), *it)) // not found
			impl.push_back( *it);
	}
	return impl;	
}

std::vector<FCO *> FCO::getAllDescendants() const
{
	std::vector<FCO *> impl, intf;
	getImpDescendants( impl);
	getIntDescendants( intf);

	std::vector<FCO *>::iterator it = intf.begin();
	for( ; it != intf.end(); ++it) // check all int
	{
		if ( impl.end() == std::find( impl.begin(), impl.end(), *it)) // not found
			impl.push_back( *it);
	}
	return impl;	
}


void FCO::getRootDescsUntilNonFcoRep( std::vector< FCO *> & family) // here the family contains only the ancestors, when the method exits it will contain some descendants too
{
	// *this is not necessarily and fco_rep
	std::vector<FCO *> roots;
	for( std::vector< FCO *>::const_iterator root_it = family.begin(); root_it != family.end(); ++root_it)
	{
		if ( (*root_it)->m_ancestors[ INTERFACE].empty() && (*root_it)->m_ancestors[ IMPLEMENTATION].empty()) // root obj
			roots.push_back( *root_it);
	}

	if ( roots.empty())
		roots.push_back( this); // *this is itself a root

	std::vector< FCO*> res;
	std::vector< FCO*> fco_reps;
	std::vector< FCO*> nodes = roots;

	bool has_fco_rep( true);
	while ( has_fco_rep)
	{
		has_fco_rep = false;
		std::vector< FCO *>::iterator it( nodes.begin());
		for( ; it != nodes.end(); ++it)
		{
			if ( std::find( family.begin(), family.end(), *it) == family.end()) //not found
				family.push_back( *it);

			if ( (*it)->getMyKind() == FCO_REP)
			{
				has_fco_rep = has_fco_rep || true;
				fco_reps.push_back( *it);
			}
		}
		nodes.clear();
		if ( has_fco_rep)
		{
			std::vector< FCO *>::iterator it1 = fco_reps.begin();
			for( ; it1 != fco_reps.end(); ++it1)
			{
				std::vector< FCO*> children_of_fco_rep = (*it1)->getAllChildren();
				std::vector< FCO*>::iterator it2( children_of_fco_rep.begin());
				for( ; it2 != children_of_fco_rep.end(); ++it2)
				{
					if ( std::find( nodes.begin(), nodes.end(), *it2) == nodes.end()) //not found
						nodes.push_back( *it2);
				}

			}
			fco_reps.clear(); // nodes has the union of fcoreps' children
		}
	}
}


bool FCO::hasParentOfSameKind()
{
	KIND_TYPE mine = getMyKind();
	bool has = false;
	std::vector< FCO *> pars = getAllParents();
	std::vector< FCO *>::iterator it = pars.begin();
	for( ; it != pars.end(); ++it)
	{
		if ( mine == (*it)->getMyKind())
			has = true;
	}
	return has;
}


void FCO::getAllInMyHierarchy( std::vector< FCO *>& family)
{
	std::vector<FCO *> roots, ancests = getAllAncestors();
	std::vector< FCO *>::const_iterator itc = ancests.begin();
	for( ; itc != ancests.end(); ++itc)
		if ( (*itc)->m_ancestors[ INTERFACE].empty() && (*itc)->m_ancestors[ IMPLEMENTATION].empty()) // root obj
			roots.push_back( *itc);

	if ( roots.empty())	roots.push_back( this); // *this is a root itself

	std::vector<FCO *>::iterator it = roots.begin();
	for( ; it != roots.end(); ++it) // these are roots
	{
		std::vector<FCO *> descs = (*it)->getAllDescendants();
		descs.push_back( *it); // insert the root too
		std::vector<FCO *>::iterator it_d = descs.begin();
		for( ; it_d != descs.end(); ++it_d)
			if ( std::find( family.begin(), family.end(), *it_d) == family.end()) // not found
				family.push_back( *it_d);

	}
}


bool FCO::hasExactParent( const FCO * par, INHERITANCE_TYPE type) const
{
	std::vector<FCO*>::const_iterator it = 
		std::find( m_parentList[type].begin(), m_parentList[type].end(), par);

	return ( it != m_parentList[type].end()); // found
}


const FCO::AttributeRepPtrList& FCO::getInitialAttributeRepPtrList() const
{
	return m_initialAttributeList;
}


void FCO::addInitialAttribute( AttributeRep * attr)
{
	AttributeRepPtrList_ConstIterator it = 
		std::find( m_initialAttributeList.begin(), m_initialAttributeList.end(), attr);

	if ( it == m_initialAttributeList.end()) // not found so insert
		m_initialAttributeList.push_back( attr);
	else 
		global_vars.err << attr->getName() << " attribute owned by " << getName() << " twice\n";
}

 
/*
This method puts all base classes into m_multipleBaseClasses vector
which are inherited from multiple times:
if two of my parents have some common ancestor then this ancestor is 
to be put into the vector

To prevent that case when one of my parents is parent of another parent of mine
the parents are inserted into their ancestor vector
*/
bool FCO::multipleInheritanceStep1()
{
	std::vector<FCO*> parents = getAllParents();
	std::vector<FCO*>::iterator it1 = parents.begin();
	for( ; it1 != parents.end(); ++it1)
	{
		std::vector<FCO*>::iterator it2 = it1;
		for( ++it2 ; it2 != parents.end(); ++it2)
		{
			// compare the ancestors of *it1 and *it2 (including themselves)
			std::vector<FCO*> anc1 = (*it1)->getAllAncestors(); anc1.push_back( *it1);
			std::vector<FCO*> anc2 = (*it2)->getAllAncestors(); anc2.push_back( *it2);

			std::vector<FCO*>::iterator anc_it_1 = anc1.begin();
			for( ; anc_it_1 != anc1.end(); ++anc_it_1)
			{
				std::vector<FCO*>::iterator anc_it_2 = anc2.begin();
				for( ; anc_it_2 != anc2.end(); ++anc_it_2)
				{
					if ( *anc_it_1 == *anc_it_2) // two different base classes ( *it1, *it2) have common base class *anc_it_1
					{
						if ( m_multipleBaseClasses.end() == std::find( m_multipleBaseClasses.begin(), 
							m_multipleBaseClasses.end(), *anc_it_1)) // if not found then
						m_multipleBaseClasses.push_back( *anc_it_1);
					}
				}
			}
		}
	}
	/*std::vector<FCO*> desc = getAllDescendants(); //Not necessary
	std::vector<FCO*>::iterator desc_it = desc.begin();
	for( ; desc_it != desc.end(); ++desc_it)
	{
		(*desc_it)->addMultipleBaseClasses( m_multipleBaseClasses);
	}*/
	return true;
}


/*
This method puts into m_virtualBaseClasses those direct Base Classes of "this"
which must be inherited virtually.
Its input is: m_multipleBaseClasses. Elements of this vector are those classes which
are inherited multiple times.
Checks how many children of "it" (alleged multiple base) are in the set of ancestors of "this"
If only one child of "it" is ancestor of "this" then there is no need for that child to be 
inherited virtually from "it"
If more than one children are ancestors of "this" then these classes must be virtually derived

"this" is inserted into the ancestor list to prepare for the case when the alleged multiple 
base is parent of "this"
*/
bool FCO::multipleInheritanceStep2()
{
	std::vector<FCO*> ancestors = getAllAncestors(); ancestors.push_back( this);
	std::vector<FCO*>::iterator it = m_multipleBaseClasses.begin();
	for( ; it != m_multipleBaseClasses.end(); ++it) // it is a multiple base class of "this"
	{
		std::vector<FCO*> ch = (*it)->getAllChildren();
		int how_many_derived_from_it = 0; // how many of my ancestors derive from it
		std::vector<FCO*>::iterator ch_it_2 = ch.begin();
		for( ; ch_it_2 != ch.end(); ++ ch_it_2)
		{
			if ( ancestors.end() != std::find( ancestors.begin(),
			ancestors.end(), *ch_it_2)) // ch_it_2 is an ancestor of "this"
			++how_many_derived_from_it; // count that how many of my ancestors derive from "it"
		}
		if ( how_many_derived_from_it>1) // more than one of my ancestors is derived from "it"
		{
			std::vector<FCO*>::iterator ch_it = ch.begin();
			for( ; ch_it != ch.end(); ++ch_it) // ch_it is child of it (an alleged multipleBaseClass of "this")
			{
				if ( ancestors.end() != std::find( ancestors.begin(),
					ancestors.end(), *ch_it)) // ch_it is an ancestor of "this"
					(*ch_it)->addVirtualBaseClasses( *it);
			}
		}
		else
		{
#ifdef _DEBUG
			global_vars.err << std::string( getName() + " has caught one tricky multiple base: " + (*it)->getName() + "\n");
#endif
		}
	}
	return true;
}


void FCO::showMultipleInheritance()
{
	std::string names1, names2;
	std::vector<FCO*>::const_iterator it = m_virtualBaseClasses.begin();
	for( ; it != m_virtualBaseClasses.end(); ++it)
	{
		names1 += (*it)->getName() + "\n";
	}

	it = m_multipleBaseClasses.begin();
	for( ; it != m_multipleBaseClasses.end(); ++it)
		names2 += (*it)->getName() + "\n";

	if ( !names1.empty() || !names2.empty())
		global_vars.err << std::string( getName() + " VirtualBase Classes\n" + names1 + "\n" + getName() + " Multipl Base Classes\n" + names2 + "\n");
}


const std::vector<FCO *>& FCO::getMultipleBaseClasses() const
{
	return m_multipleBaseClasses;
}


void FCO::addMultipleBaseClasses( const std::vector<FCO *>& classes)
{
	std::vector<FCO*>::const_iterator it = classes.begin();
	for( ; it != classes.end(); ++it)
	{
		if ( m_multipleBaseClasses.end() == std::find( m_multipleBaseClasses.begin(),
			m_multipleBaseClasses.end(), *it)) 
			m_multipleBaseClasses.push_back( *it);
	}
}


const std::vector<FCO *>& FCO::getVirtualBaseClasses() const
{
	return m_virtualBaseClasses;
}


void FCO::addVirtualBaseClasses( FCO * one_class)
{
	if ( m_virtualBaseClasses.end() == std::find( m_virtualBaseClasses.begin(),
		m_virtualBaseClasses.end(), one_class)) 
		m_virtualBaseClasses.push_back( one_class);
}


void FCO::addVirtualBaseClasses( const std::vector<FCO *>& classes)
{
	std::vector<FCO*>::const_iterator it = classes.begin();
	for( ; it != classes.end(); ++it)
		addVirtualBaseClasses( *it);
}


/*virtual*/ void FCO::prepare()
{
	prepareMacros();
	prepareIniFin();
	prepareMOF();
	prepareAttributes();
}


/*virtual*/ void FCO::prepareMacros()
{
	std::string h, s, n = getValidNmspc();
	int c = (n.length() < 15) ? 15-n.length():(
	        (n.length() < 30) ? 30-n.length():(
	        (n.length() < 45) ? 45-n.length():(
			0
	)));

	std::string base_list;
	bool has_same_kind_parent = false;
	std::string abstract_str = "";
	std::vector<FCO*> bases = getAllParents();
	std::vector<FCO*>::iterator it = bases.begin();
	for( ; it != bases.end(); ++it)
	{
		has_same_kind_parent = has_same_kind_parent || ( (*it)->getMyKind() == getMyKind());
		if( (*it)->getValidNmspc() == n)
			base_list += (*it)->getValidName() + ", ";
		else
			base_list += (*it)->getLValidName() + ", ";
	}

	std::string dump_Non_Exted_Desc_Kinds = dumpNonExtedDescKinds(); // this class extends some other classes (descendants)
	if ( m_isAbstract && dump_Non_Exted_Desc_Kinds.empty()) 
		abstract_str = "_ABSTRACT"; //only if it is not intended to be representant of other kinds
	
	h  = "namespace " + n + " { " + std::string( c, ' ');

	if ( bases.empty())
	{
		//h = "class " + getValidNameImpl() + ";\n"; commented out on 5/6/2004 not needed
		h += "DECLARE" + abstract_str + "_BONEXTENSION( BON::";
		h += getMyKindStr() + ", " + getValidNameImpl() + ", " + getValidName()	+ " ); }\n";
	}
	else
	{
		std::string no_of_bases_str = "";
		if ( has_same_kind_parent)
		{
			unsigned int no_of_bases = bases.size();
			if ( no_of_bases >= 2 && no_of_bases <= 6)
			{
				char base_ptr[3]; sprintf( base_ptr, "%i", no_of_bases); 
				no_of_bases_str = base_ptr;
			}
			else if ( no_of_bases > 6)
			{
				TO("Currently macros do not support cases of more than 6 base classes");
				global_vars.err << "Serious problem: Currently macros do not support cases of more than 6 base classes. Class : " << getValidName() << "\n";
			}
		}
		else //if ( has_same_kind_parent)
		{
			unsigned int no_of_bases = bases.size() + 1;
			char base_ptr[3]; sprintf( base_ptr, "%i", no_of_bases); no_of_bases_str = base_ptr;
			base_list = "BON::" + getMyKindStr() + ", " + base_list;
		}
		//h = "class " + getValidNameImpl() + ";\n"; commented out on 5/6/2004 not needed
		h += "DECLARE" + abstract_str + "_BONEXTENSION" + no_of_bases_str + "( " + base_list + getValidNameImpl() + ", " + getValidName()	+ " ); }\n";
	}
	if ( m_isAbstract && !dump_Non_Exted_Desc_Kinds.empty()) // if abstract and responsible for its descendants
	{
		// notify the user
		global_vars.err << "Note: " << getValidName() << " is originally an abstract class, but some descendants have it as their only extended ancestor. Abstractness disregarded.\n";

		s = "IMPLEMENT_BONEXTENSION( " + getValidNmspc() + Any::NamespaceDelimiter_str + getValidName();
		s += ", \"" + dump_Non_Exted_Desc_Kinds + "\"";
		s += " );\n";
	}
	else if ( m_isAbstract) // is not responsible for its descendants
	{
		s = "IMPLEMENT" + abstract_str + "_BONEXTENSION( " + getValidNmspc() + Any::NamespaceDelimiter_str + getValidName();
		s += " );\n";
	}
	else // not abstract
	{
		s = "IMPLEMENT_BONEXTENSION( " + getValidNmspc() + Any::NamespaceDelimiter_str + getValidName();
		s += ", \"" + getLStrictNmspc() + getName() + dump_Non_Exted_Desc_Kinds + "\"";
		s += " );\n";
	}

	m_globalSource += s;
	m_globalHeader += h;
}


void FCO::prepareAttributes()
{
	AnyLexicographicSort lex;
	std::sort( m_initialAttributeList.begin(), m_initialAttributeList.end(), lex);

	AttributeRepPtrList_ConstIterator it = m_initialAttributeList.begin();
	for( ; it != m_initialAttributeList.end(); ++it)
	{
		m_attributeMethods.push_back( (*it)->createMethodForAttr( this));
		m_attributeMethods.push_back( (*it)->createSetMethodForAttr( this));
	}
}


std::string FCO::dumpClassHeader()
{
	// this bool will sign if this is a child of an fco , thus needs to have 
	// a 'virtual' parent of his kind, to access all features which are needed
	//
	//         A<<fco>>
	//        /|\
	//         |
	//         B<<model>>
	//   in this case A is derived from BON::FCO
	//   B is derived from A and BON::Model
	bool has_ancestor_of_the_same_kind = false;

	// if has private base class (used in case of implementation inheritance in order to prevent casting)
	// then the object must be derived publicly and virtually from its kind again! (thus the cast to BON::Object will succeed)
	// i.e: 
	//         B<<fco>>
	//        /.\
	//         |
	//         C (implementation inheritance between B and C)
	//
	//  in such cases C inherits privately from B, thus upcasting an C* pointer is not allowed
	//  neither to B*, nor to BON::FCO*, nor to BON::Object*
	//  but then the CImpl extension class would not compile with BON::Extension and BON::ExtensionFactory
	//  thus C is derived once more (virtually!) from BON::FCO
	bool has_private_base_class = false;
	bool same_nmsp_base = true;
	std::string my_nmsp = getValidNmspc();

	std::string mmm;
	std::vector< std::string> list;

	std::vector<FCO *>::iterator it = m_parentList[ REGULAR ].begin();
	for( ; it != m_parentList[ REGULAR ].end(); ++it)
	{
		same_nmsp_base = (*it)->getValidNmspc() == my_nmsp;
		has_ancestor_of_the_same_kind = has_ancestor_of_the_same_kind || ( (*it)->getMyKind() == getMyKind());
		if ( std::find( m_virtualBaseClasses.begin(), m_virtualBaseClasses.end(), *it) ==
			m_virtualBaseClasses.end()) // not a virtual base class
			list.push_back( "public " + (same_nmsp_base?(*it)->getValidNameImpl():(*it)->getLValidNameImpl()));
		else 
			list.push_back( "virtual public " + (same_nmsp_base?(*it)->getValidNameImpl():(*it)->getLValidNameImpl()));
	}

	it = m_parentList[ IMPLEMENTATION ].begin();
	for( ; it != m_parentList[ IMPLEMENTATION ].end(); ++it)
	{
		same_nmsp_base = (*it)->getValidNmspc() == my_nmsp;
		has_private_base_class = true;
		has_ancestor_of_the_same_kind = has_ancestor_of_the_same_kind || ( (*it)->getMyKind() == getMyKind());
		if ( std::find( m_virtualBaseClasses.begin(), m_virtualBaseClasses.end(), *it) ==
			m_virtualBaseClasses.end()) // not a virtual base class
			list.push_back( "private " + (same_nmsp_base?(*it)->getValidNameImpl():(*it)->getLValidNameImpl()));
		else 
			list.push_back( "virtual private " + (same_nmsp_base?(*it)->getValidNameImpl():(*it)->getLValidNameImpl()));
	}

	it = m_parentList[ INTERFACE ].begin();
	for( ; it != m_parentList[ INTERFACE ].end(); ++it)
	{
		same_nmsp_base = (*it)->getValidNmspc() == my_nmsp;
		has_ancestor_of_the_same_kind = has_ancestor_of_the_same_kind || ( (*it)->getMyKind() == getMyKind());
		if ( std::find( m_virtualBaseClasses.begin(), m_virtualBaseClasses.end(), *it) ==
			m_virtualBaseClasses.end()) // not a virtual base class
			list.push_back( "public " + (same_nmsp_base?(*it)->getValidNameImpl():(*it)->getLValidNameImpl()));
		else 
			list.push_back( "virtual public " + (same_nmsp_base?(*it)->getValidNameImpl():(*it)->getLValidNameImpl()));
	}

	// the place of ":" plays important role in identifying where class definitions start really
	// to prevent confusing with predefinitions like "class X;"
	// the canonical form is "class X :"

	if ( !has_ancestor_of_the_same_kind || has_private_base_class)
	{
		mmm = " :\n\t  virtual public BON::" + getMyKindStr() + "Impl";
		for( unsigned int k = 0; k < list.size(); ++k)
			mmm += "\n\t, " + list[k];
	}
	else
		for( unsigned int k = 0; k < list.size(); ++k)
			mmm += (k == 0?" :\n\t  ":"\n\t, ") + list[k];

	return mmm + "\n";
}


void FCO::dumpConnMethods( std::string & h_file, std::string & c_file)
{
	if (!m_connectionMethods.empty())
		h_file += CodeGen::indent(1) +"//\n" + CodeGen::indent(1) + "// connectionEnd getters\n";

	MethodLexicographicSort lex;
	std::sort( m_connectionMethods.begin(), m_connectionMethods.end(), lex);

	std::vector<Method>::iterator i = m_connectionMethods.begin();
	for( ; i != m_connectionMethods.end(); ++i)
	{
		if (*i) // if valid method
		{
			h_file += i->getHeader() + "\n";
			c_file += i->getSource() + "";
		}
	}
}


void FCO::dumpAttrMethods( std::string & h_file, std::string & c_file)
{
	if (!m_attributeMethods.empty())
		h_file += CodeGen::indent(1) + "//\n" + CodeGen::indent(1) + "// attribute getters and setters\n";
	
	MethodLexicographicSort lex;
	std::sort( m_attributeMethods.begin(), m_attributeMethods.end(), lex);

	std::vector<Method>::iterator i = m_attributeMethods.begin();
	for( ; i != m_attributeMethods.end(); ++i)
	{
		if (*i) // if valid method
		{
			h_file += i->getHeader() + "\n";
			c_file += i->getSource() + "";
		}
	}
}


void FCO::dumpFCO( std::string & h_file, std::string & c_file)
{
	dumpConnMethods( h_file, c_file);
	dumpAttrMethods( h_file, c_file);
}


void FCO::dumpPre( std::string & h_file, std::string & c_file)
{
	// namespace opened in h and cpp files
	h_file += "namespace " + getValidNmspc() + "\n{\n";
	//c_file += "namespace " + getValidNmspc() + "\n{\n";

	h_file += dumpOrnament( isAbstract());
	h_file += "class " + getValidNameImpl();
	h_file += dumpClassHeader() + "{\npublic:\n";
	/*if ( isAbstract())
		h_file += CodeGen::indent(1) + "virtual bool isAbstract() const { return true; } // it is an abstract class, it will not be instantiated\n";
	else
		h_file += CodeGen::indent(1) + "virtual bool isAbstract() const { return false; } // it is not an abstract class\n";*/

	for( unsigned int k = 0; k < m_inifinMethods.size(); ++k)
	{
		h_file += m_inifinMethods[k].getHeader() + "\n";
		c_file += m_inifinMethods[k].getSource();
	}

	h_file += m_classGlobalPart + '\n';
}


std::string FCO::hide()
{
	// no need to hide the conn methods because the kind of inheritance (int or imp)
	// does the right thing from the connections point of view
	return /*hideConnMethods() +*/ hideAttrMethods();
}


std::string FCO::expose( const std::string& repl)
{ 
	// no need to expose the conn methods because the kind of inheritance (int or imp)
	// does the right thing from the connections point of view
	return /*exposeConnMethods() +*/ exposeAttrMethods( repl);
}


std::string FCO::hideAndExpose()
{
	std::string hdr, public_part, private_part;

	// handling methods inherited from the parents
	std::vector<FCO *>::iterator imp_it = m_parentList[ IMPLEMENTATION ].begin();
	for( ; imp_it != m_parentList[ IMPLEMENTATION ].end(); ++imp_it)
		public_part += (*imp_it)->expose(""); // expose methods of the imp parent

	std::vector<FCO *>::iterator int_it = m_parentList[ INTERFACE ].begin();
	for( ; int_it != m_parentList[ INTERFACE ].end(); ++int_it)
		private_part += (*int_it)->hide(); // hide methods of the int parent

	if ( !private_part.empty())
		hdr += "\nprivate: // hidden members (interface inherited)\n" + private_part;
	if ( !public_part.empty())
		hdr += "\npublic: // exposed members (implementation inherited)\n" + public_part;

	// Handling methods inherited from the ancestors (excluding parents).
	// Those methods are needed to be exposed/hidden which are inherited from 
	// the IMPLEMENTATION and REGULAR parents, grandparents, ... of my
	// IMPLEMENTATION and INTERFACE parents
	// The INTERFACE parents of my parents do not cause problems since my parent
	// has already hidden such methods (are overwritten throwing exceptions)
	// so this behaviour will result if one of my users will call such inherited methods
	
	// on the IMPLEMENTATION parent's way up we need to expose
	imp_it = m_parentList[ IMPLEMENTATION ].begin();
	for( ; imp_it != m_parentList[ IMPLEMENTATION ].end(); ++imp_it)
	{
		std::vector<FCO *>::iterator imp2_it = (*imp_it)->m_ancestors[ IMPLEMENTATION].begin();
		for( ; imp2_it != (*imp_it)->m_ancestors[ IMPLEMENTATION].end(); ++imp2_it)
			hdr += (*imp2_it)->expose( (*imp_it)->getValidNameImpl()); // instead of "using GrandParent::get..." the "using Parent::get" form has to be used
	}

	
	// on the INTERFACE parent's way up we need to hide
	int_it = m_parentList[ INTERFACE ].begin();
	for( ; int_it != m_parentList[ INTERFACE ].end(); ++int_it)
	{
		std::vector<FCO *>::iterator int2_it = (*int_it)->m_ancestors[ IMPLEMENTATION].begin();
		for( ; int2_it != (*int_it)->m_ancestors[ IMPLEMENTATION].end(); ++int2_it)
			hdr += (*int2_it)->hide();
	}

	return hdr;
}


std::string FCO::exposeConnMethods()
{
	// not used currently
	std::string h_file;
	if (!m_connectionMethods.empty())
		h_file += CodeGen::indent(1) + "//\n" + CodeGen::indent(1) + "// exposed connectionEnd getters\n";
	std::vector<Method>::iterator i = m_connectionMethods.begin();
	for( ; i != m_connectionMethods.end(); ++i)
		h_file += i->getExposed("blabla") + "\n";
	return h_file;
}


std::string FCO::exposeAttrMethods( const std::string& repl_container)
{
	std::string h_file;
	if (!m_attributeMethods.empty())
		h_file += CodeGen::indent(1) + "//\n" + CodeGen::indent(1) + "// exposed attribute getters and setters\n";
	std::vector<Method>::iterator i = m_attributeMethods.begin();
	for( ; i != m_attributeMethods.end(); ++i)
		h_file += i->getExposed( repl_container) + "\n";
	return h_file;
}


std::string FCO::hideConnMethods()
{
	std::string h_file;
	if (!m_connectionMethods.empty())
		h_file += CodeGen::indent(1) + "//\n" + CodeGen::indent(1) + "// hidden connectionEnd getters\n";
	std::vector<Method>::iterator i = m_connectionMethods.begin();
	for( ; i != m_connectionMethods.end(); ++i)
		h_file += i->getHidden() + "\n";
	return h_file;
}


std::string FCO::hideAttrMethods()
{
	std::string h_file;
	if (!m_attributeMethods.empty())
		h_file += CodeGen::indent(1) + "//\n" + CodeGen::indent(1) + "// hidden attribute getters and setters\n";
	std::vector<Method>::iterator i = m_attributeMethods.begin();
	for( ; i != m_attributeMethods.end(); ++i)
		h_file += i->getHidden() + "\n";
	return h_file;
}


/**********STATICS**************/

/*static*/ std::vector<FCO *> FCO::intersect( const std::vector<FCO*>& in1, const std::vector<FCO*>& in2)
{
	std::vector<FCO*> out;
	std::vector<FCO*>::const_iterator i1 = in1.begin();
	for( ; i1 != in1.end(); ++i1)
		if ( std::find( in2.begin(), in2.end(), *i1) != in2.end()) 
			out.push_back( *i1);

	return out;
}


/*static*/ std::vector<FCO*> FCO::lcdIntersect( const std::vector<FCO*>& elems)
{
	std::vector<FCO*> common_anc, lowest_common_anc;
	if ( elems.empty()) 
		return lowest_common_anc;

	std::vector<FCO*>::const_iterator it = elems.begin();
	common_anc = (*it)->getAllAncestors();
	common_anc.push_back( *it);

	for(++it ; it != elems.end(); ++it)
	{
		std::vector<FCO*> r = (*it)->getAllAncestors();
		r.push_back( *it);
		common_anc = FCO::intersect( common_anc, r);
	}

	// common_anc has now all the common ancestors of the conn ends
	// if these are fco-s are related to each other then select the youngest one
	// if not then further investigation is needed
	std::vector<FCO*>::iterator jt = common_anc.begin();
	for( ; jt != common_anc.end(); ++jt)
	{
		// check if children of *jt are also in common_anc
		std::vector<FCO*> ints = FCO::intersect( common_anc, (*jt)->getAllChildren());
		if ( !ints.empty()) // child of *jt is also a common_anc so *jt is not the lowest common anc
		{ }
		else // it is the lowest (or one of the lowest ones) <!>
		{
			lowest_common_anc.push_back( *jt);
		}
	}

	return lowest_common_anc;
}


/*static*/ bool FCO::equal( std::vector<FCO*>& op1, std::vector<FCO*>& op2)
{
	if ( op1.size() != op2.size()) return false;
	std::vector<FCO*>::iterator it;
	
	for( it = op1.begin(); it != op1.end(); ++it)
		if ( op2.end() == std::find( op2.begin(), op2.end(), *it))
			return false;
	
	for( it = op2.begin(); it != op2.end(); ++it)
		if ( op1.end() == std::find( op1.begin(), op1.end(), *it))
			return false;
	
	return true;
}


/*used at least by the ReferenceRep::createMethods */
/*static*/ std::string FCO::lcdKindIntersect( const std::vector<FCO*>& elems)
{
	if ( elems.empty()) return "";


	std::vector<FCO*>::const_iterator it = elems.begin();
	bool same_kind( true);
	KIND_TYPE kt = (*it)->getMyKind();

	for( ++it; it != elems.end() && same_kind ; ++it)
		same_kind = same_kind && ( kt == (*it)->getMyKind());

	if (same_kind) 
		return "BON::" + Any::KIND_TYPE_STR[ kt];
	return "";
}
