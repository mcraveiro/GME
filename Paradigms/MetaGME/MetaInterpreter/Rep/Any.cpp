#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "Any.h"
#include "ConstraintRep.h"
#include "Broker.h"
#include "Dumper.h"

#include "algorithm"

#include "globals.h"
extern Globals global_vars;

bool AnyLexicographicSort::operator()( Any * op1, Any * op2) const
{
	std::string s1 = op1->getName();
	std::string s2 = op2->getName();
	int k = s1.compare(s2);

	return ( k < 0);
}

/*static*/ const std::string Any::NamespaceDelimiter_str = "::";
/*static*/ const std::string Any::InRootFolder_str = "InRootFolder";
/*static*/ const std::string Any::DisplayedName_str = "DisplayedName";
/*static*/ const std::string Any::NameSelectorNode_str = "myNameIs";
/*static*/ const std::string Any::DisplayedNameSelectorNode_str = "myDisplayedNameIs";
/*static*/ const std::string Any::GeneralPreferences_str = "GeneralPreferences";

/*static*/ const std::string Any::KIND_TYPE_STR[] =
{
	"ATOM",
	"MODEL",
	"CONN",
	"SET",
	"REF",
	"FCO",
	"ASPECT",
	"FOLDER",
	"CONSTRAINT",
	"CONSTRAINFUNC",
	"ATTRIBUTE"
};


Any::Any( BON::FCO& ptr)
	: m_ptr( ptr)
	, m_isInRootFolder( false)
	, m_equivs()
	, m_initialConstraintList()
	, m_finalConstraintList()
{ 
}


Any::~Any() 
{ 
	m_equivs.clear();
	m_initialConstraintList.clear();
	m_finalConstraintList.clear();
}

void Any::initNamespace()
{
	// decide which namespace the object belongs to:
	if( m_ptr->isInLibrary())
	{
		bool all_equivs_in_lib = true;
		for ( std::set< BON::FCO >::const_iterator it = m_equivs.begin()
			; all_equivs_in_lib && it != m_equivs.end()
			; ++it)
		{
			if ( *it == m_ptr) continue;
			if ((*it)->getObjectMeta().name().find("Proxy") != std::string::npos) continue;//ignore proxies (from m_equivs) when determining namespace
			all_equivs_in_lib = (*it)->isInLibrary();
		}

		if( !all_equivs_in_lib) // it has equivs in the host project
			resetNamespace();   // so needs to be in the main namespace
		else
		{
			// find the Namespace value from the namespace holding rootfolder
			// in case of nested libs m_libRootFolder and m_nmspRootFolder are !equal
			BON::RegistryNode rn = m_nmspRootFolder->getRegistry()->getChild( "Namespace");

			if( rn)	m_namespace = rn->getValue();
			else    m_namespace = "";
			//m_respPointer could be in another library then m_ptr, so the namespace could be different
		}
	}
	else // regular object, init its namespace value from the project's value
	{
		resetNamespace();
	}
}

void Any::resetNamespace()
{
	m_namespace = Dumper::getInstance()->getNamespace();
}

std::string Any::getNamespace() const
{
	return m_namespace;
}

void FolderRep::initAttributes()
{
	// for folders the default value (from the meta) true
	// for fcos    the default value (from the meta) false

	m_isInRootFolder = m_ptr->getAttribute( InRootFolder_str)->getBooleanValue();
	// try to find one among the equivalent classes which is in the root folder
	std::set< BON::FCO >::const_iterator it = m_equivs.begin();
	for ( ; !m_isInRootFolder && it != m_equivs.end(); ++it)
	{
		if ( *it == m_ptr) continue;
		if ((*it)->getObjectMeta().name().find("Proxy") != std::string::npos) continue; // folderproxies don't have attributes
		m_isInRootFolder = m_isInRootFolder || (*it)->getAttribute( InRootFolder_str)->getBooleanValue();
	}
	auto attr = m_ptr->getAttribute(GeneralPreferences_str);
	if (attr) // old MetaGME versions don't GeneralPreferences for Folders
	{
		m_sAttrGenPref = attr->getStringValue();
	}
}


bool Any::isInRootFolder()
{
	return m_isInRootFolder;
}


bool Any::isFCO() const
{
	bool fco = false;
	fco = (getMyKind() == ATOM ||
		getMyKind() == MODEL ||
		getMyKind() == CONN ||
		getMyKind() == SET ||
		getMyKind() == REF ||
		getMyKind() == FCO_REP);
	return fco;
}


std::string Any::getName() const
{
	if ( this->m_ptr)
	{
		return m_namespace + (m_namespace.empty()?"":Any::NamespaceDelimiter_str) + m_ptr->getName();
	}
	return "NullPtrError";
}


std::string Any::getDispName() const
{
	if ( this->m_ptr)
	{
		return m_ptr->getAttribute( DisplayedName_str)->getStringValue();
	}
	return "NullPtrError";
}


std::string Any::dumpDispName() const
{
	std::string mmm;
	std::string disp = getDispName();
	if ( !disp.empty())
		mmm += indStr() + "<dispname>" + Dumper::xmlFilter(disp) +"</dispname>\n";
	return mmm;
}


std::string Any::getMyKindStr() const
{
	return KIND_TYPE_STR[ getMyKind()];
}


std::string Any::getMyPrefix() const
{
	// any modification in the registry policy should be syched with
	// the Broker::initFromObj method
	if ( this->m_ptr)
		return Broker::getRegistryTokenName( m_ptr);

	throw "Error: inquiring prefix for a null object\n";
}


BON::RegistryNode Any::getMyRegistry() const
{
	// any modification in the registry policy should be synched with
	// the Broker::initFromObj method
	if ( this->m_ptr && this->m_parentFolder)
		return m_parentFolder->getRegistry()->getChild( getMyPrefix());

	throw 1;
}


BON::RegistryNode Any::getMyRegistryOld() const
{
	if ( this->m_ptr && this->m_parentFolder)
	{
		std::string kind = m_ptr->getObjectMeta().name();
		std::string name = m_ptr->getPath();

		BON::ObjectPtr p = m_ptr->getObjectI();
		long relid = 0;
		BONCOMTHROW( p->get_RelID(&relid));
		char t[16];
		sprintf( t, "%x", relid);

		std::string child_name = kind + "s" + name + t;
		return m_parentFolder->getRegistry()->getChild( child_name);
	}

	throw 1;
}

/*static*/ std::string Any::getMyPrefix( BON::FCO& fco, BON::Folder& f)
{
	ASSERT( fco);
	ASSERT( f);
	return Broker::getRegistryTokenName( fco);
}


/*static*/ BON::RegistryNode Any::getMyRegistry( BON::FCO& fco, BON::Folder& f)
{
	ASSERT( fco);
	ASSERT( f);
	return f->getRegistry()->getChild( Any::getMyPrefix(fco, f));
}


std::string Any::askMetaRef(const std::string& in_token /* = "" */) const
{
	// if called with non empty in_token, this has to have a leading slash

	// getValueByPath needs leading '/'
	// getChild doesn't need leading '/'
	// in_token may look like: "", "/Role"
	std::string token = "/MetaRef" + in_token;
	std::string meta_ref = "";
	if ( this->m_ptr && this->m_parentFolder)
	{
		//meta_ref = m_ptr->getRegistry()->getValueByPath(token);
		meta_ref = getMyRegistry()->getValueByPath( token);
			
		if (meta_ref == "") 
		{ 
			int meta_ref_int = Broker::getNextMetaRefId();
			char n_meta_ref[64];
			sprintf( n_meta_ref, "%d", meta_ref_int);
			//m_ptr->getRegistry()->setValueByPath( token, n_meta_ref);
			getMyRegistry()->setValueByPath( token, n_meta_ref);
			meta_ref = n_meta_ref;
		}
		return meta_ref;
	}
	else // added for dummy aspects
	{
		int meta_ref_int = Broker::getNextMetaRefId();
		char n_meta_ref[64];
		sprintf( n_meta_ref, "%d", meta_ref_int);
		//getMyRegistry()->setValueByPath( token, n_meta_ref);
		meta_ref = n_meta_ref;
		return meta_ref;
	}
	return "NullPtrError";
}


void Any::addInitialConstraintRep( ConstraintRep * constraint)
{
	ConstraintRepPtrList_ConstIterator it = 
		std::find( m_initialConstraintList.begin(), m_initialConstraintList.end(), constraint);

	if ( it == m_initialConstraintList.end())
		m_initialConstraintList.push_back( constraint);
	else
		global_vars.err << MSG_ERROR << constraint->getName() << " constraint owned by " << m_ptr << " twice\n";
}


const Any::ConstraintRepPtrList& Any::getInitialConstraintRepPtrList() const
{
	return m_initialConstraintList;
}


void Any::addFinalConstraint( ConstraintRep * constraint)
{
	ConstraintRepPtrList_ConstIterator c_it = 
		std::find( m_finalConstraintList.begin(), m_finalConstraintList.end(), constraint);

	if ( c_it == m_finalConstraintList.end())
		m_finalConstraintList.push_back( constraint);
	/*else - because of multiple inheritance this can happen easily
		global_vars.err << constraint->getName() << " inherited constraint owned by " << getName() << " twice\n";*/
}


void Any::addFinalConstraintList( const ConstraintRepPtrList & list)
{
	ConstraintRepPtrList_ConstIterator it = list.begin();
	for( ; it != list.end(); ++it)
		addFinalConstraint( *it);
}


std::string Any::dumpConstraints()
{
	std::string mmm = "";

	AnyLexicographicSort lex;
	std::string last_name = "";
	std::sort( m_finalConstraintList.begin(), m_finalConstraintList.end(), lex );

	ConstraintRepPtrList_ConstIterator c_i = m_finalConstraintList.begin();
	for( ; c_i != m_finalConstraintList.end(); ++c_i)
	{
		mmm += ( *c_i)->doDump();
		if ( last_name != "" && last_name == (*c_i)->getName())
			global_vars.err << MSG_ERROR << "Duplicate constraint name " << (*c_i)->getName() << " found for " << m_ptr << "\n";
		last_name = (*c_i)->getName();
	}
	return mmm;
}


