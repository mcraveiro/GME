#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "FCO.h"
#include "ReferenceRep.h"
#include "logger.h"
#include "Token.h"
#include "list"
#include "algorithm"
#include <iterator>

#include "globals.h"
extern Globals global_vars;


/*static*/ const std::string FCO::IsAbstract_str = "IsAbstract";
/*static*/ const std::string FCO::Icon_str = "Icon";
/*static*/ const std::string FCO::PortIcon_str = "PortIcon";
/*static*/ const std::string FCO::Decorator_str = "Decorator";
/*static*/ const std::string FCO::IsHotspotEnabled_str = "IsHotspotEnabled";
/*static*/ const std::string FCO::IsTypeShown_str = "IsTypeShown";
/*static*/ const std::string FCO::NamePosition_str = "NamePosition";
/*static*/ const std::string FCO::SubTypeIcon_str = "SubTypeIcon";
/*static*/ const std::string FCO::InstanceIcon_str = "InstanceIcon";
/*static*/ const std::string FCO::NameWrapNum_str = "NameWrapNum";
/*static*/ const std::string FCO::IsNameEnabled_str = "IsNameEnabled";
/*static*/ const std::string FCO::IsResizable_str = "IsResizable";
/*static*/ const std::string FCO::AutoRouterPref_str = "AutoRouterPref";
/*static*/ const std::string FCO::HelpURL_str = "HelpURL";
/*static*/ const std::string FCO::ShowPorts_str = "ShowPorts";

/*static*/ const std::string FCO::IsGradientFillEnabled_str = "IsGradientFillEnabled";
/*static*/ const std::string FCO::GradientFillColor_str = "GradientFillColor";
/*static*/ const std::string FCO::GradientFillDirection_str = "GradientFillDirection";
/*static*/ const std::string FCO::IsShadowCastEnabled_str = "IsShadowCastEnabled";
/*static*/ const std::string FCO::ShadowColor_str = "ShadowColor";
/*static*/ const std::string FCO::ShadowThickness_str = "ShadowThickness";
/*static*/ const std::string FCO::ShadowDirection_str = "ShadowDirection";
/*static*/ const std::string FCO::IsRoundRectangleEnabled_str = "IsRoundRectangleEnabled";
/*static*/ const std::string FCO::RoundRectangleRadius_str = "RoundRectangleRadius";

FCO::FCO( BON::FCO& ptr, BON::FCO& resp_ptr)
	: Any( ptr)
	, m_respPointer( resp_ptr)
	, m_isAbstract( false)
	, m_bAttrIsHotspotEnabled( true)
	, m_bAttrIsTypeShown( false)
	, m_bAttrIsNameEnabled( true)
	, m_bAttrIsResizable( false)
	, m_iAttrNamePosition( 0)
	, m_iAttrNameWrapNum( 0)
	, m_bAttrIsGradientFillEnabled( false)
	, m_iAttrGradientFillColor( RGB(0xC0,0xC0,0xC0))
	, m_iAttrGradientFillDirection( 0)
	, m_bAttrIsShadowCastEnabled( false)
	, m_iAttrShadowColor( RGB(0xC0,0xC0,0xC0))
	, m_iAttrShadowThickness( 9)
	, m_iAttrShadowDirection( 45)
	, m_bAttrIsRoundRectangleEnabled( false)
	, m_iAttrRoundRectangleRadius( 9)
	, m_references()
	, m_partOf()
	, m_partOfFinal()
	, m_initialAttributeList()
	, m_finalAttributeList()
{ 
	for ( int i = 0; i < NUMBER_OF_INHERITANCES; ++i)
	{
		m_childList[i].clear();
		m_parentList[i].clear();
		m_ancestors[i].clear();
		m_descendants[i].clear();
	}
}


FCO::~FCO() 
{ 
	m_references.clear();
	m_partOf.clear();
	m_partOfFinal.clear();
	m_initialAttributeList.clear();
	m_finalAttributeList.clear();

	for ( int i = 0; i < NUMBER_OF_INHERITANCES; ++i)
	{
		m_childList[i].clear();
		m_parentList[i].clear();
		m_ancestors[i].clear();
		m_descendants[i].clear();
	}
}


std::string FCO::getName() const
{
	if( m_respPointer)
	{
		return m_namespace + (m_namespace.empty()?"":Any::NamespaceDelimiter_str) + m_respPointer->getName();
	}
	else
	{
		return m_namespace + (m_namespace.empty()?"":Any::NamespaceDelimiter_str) + m_ptr->getName();
	}
}


std::string FCO::getDispName() const
{
	if( m_respPointer) // not a plain fco, it has its resppointer set
	{
		// previously return "";
		return m_userSelectedDisplayName;
	}
	else
	{
		return m_sAttrDispName;
	}
}


void FCO::initAttributes()
{
	m_isInRootFolder = m_isInRootFolder || m_ptr->getAttribute( InRootFolder_str)->getBooleanValue();

	// isAbstract: true iff all values (which are set) are true
	bool isabs = true; // if any of the set values is false it will change
	bool isabs_set = false;
	//m_isAbstract = m_isAbstract && m_ptr->getAttribute( IsAbstract_str)->getBooleanValue();
	if( m_ptr->getAttribute( IsAbstract_str)->getStatus() >= BON::AS_Here)
	{
		isabs = isabs && m_ptr->getAttribute( IsAbstract_str)->getBooleanValue();
		isabs_set = true;
	}

	// general pref
	bool isgenpref_set = false;
	if( m_ptr->getAttribute( GeneralPreferences_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrGenPref = m_ptr->getAttribute( GeneralPreferences_str)->getStringValue();
		isgenpref_set = true;
	}

	// displayed name
	bool isdispname_set = false;
	if( m_ptr->getAttribute( DisplayedName_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrDispName = m_ptr->getAttribute( DisplayedName_str)->getStringValue();
		isdispname_set = true;
	}

	// --applicable to non proxies only--

	bool ishotspotenabled_set = false;
	if( m_ptr->getAttribute( IsHotspotEnabled_str)->getStatus() >= BON::AS_Here)
	{
		m_bAttrIsHotspotEnabled	= m_ptr->getAttribute( IsHotspotEnabled_str)->getBooleanValue();// def val: TRUE (dumped if FALSE)
		ishotspotenabled_set = true;

	}
	
	bool istypeshown_set = false;
	if( m_ptr->getAttribute( IsTypeShown_str)->getStatus() >= BON::AS_Here)
	{
		m_bAttrIsTypeShown		= m_ptr->getAttribute( IsTypeShown_str)->getBooleanValue();		// def val: FALSE (dumped if TRUE)
		istypeshown_set = true;
	}

	
	bool isnameenabled_set = false;
	if( m_ptr->getAttribute( IsNameEnabled_str)->getStatus() >= BON::AS_Here)
	{
		m_bAttrIsNameEnabled	= m_ptr->getAttribute( IsNameEnabled_str)->getBooleanValue();	// def val: TRUE (dumped if FALSE)
		isnameenabled_set = true;
	}

	bool isnamepos_set = false;
	//if( m_ptr->getAttribute( NamePosition_str)->getStatus() >= BON::AS_Here)
	{
		m_iAttrNamePosition	= m_ptr->getAttribute( NamePosition_str)->getIntegerValue();// dumped anyway
		isnamepos_set = true;
	}

	bool isnamewrapnum_set = false;
	if( m_ptr->getAttribute( NameWrapNum_str)->getStatus() >= BON::AS_Here)
	{
		m_iAttrNameWrapNum	= m_ptr->getAttribute( NameWrapNum_str)->getIntegerValue(); // dumped if not 0
		isnamewrapnum_set = true;
	}

	bool icon_set = false;
	if( m_ptr->getAttribute( Icon_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrIcon = m_ptr->getAttribute( Icon_str)->getStringValue(); // dumped if not empty
		icon_set = true;
	}

	bool porticon_set = false;
	if( m_ptr->getAttribute( PortIcon_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrPortIcon = m_ptr->getAttribute( PortIcon_str)->getStringValue();
		porticon_set = true;
	}

	bool subtypeicon_set = false;
	if( m_ptr->getAttribute( SubTypeIcon_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrSubTypeIcon = m_ptr->getAttribute( SubTypeIcon_str)->getStringValue();
		subtypeicon_set = true;
	}

	bool instanceicon_set = false;
	if( m_ptr->getAttribute( InstanceIcon_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrInstanceIcon = m_ptr->getAttribute( InstanceIcon_str)->getStringValue();
		instanceicon_set = true;
	}

	bool decorator_set = false;
	if( m_ptr->getAttribute( Decorator_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrDecorator = m_ptr->getAttribute( Decorator_str)->getStringValue();
		decorator_set = true;
	}

	bool isresizable_set = false;
	if( m_ptr->getAttribute( IsResizable_str)->getStatus() >= BON::AS_Here)
	{
		m_bAttrIsResizable = m_ptr->getAttribute( IsResizable_str)->getBooleanValue();
		isresizable_set = true;
	}

	bool autorouterpref_set = false;
	if( m_ptr->getAttribute( AutoRouterPref_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrAutoRouterPref = m_ptr->getAttribute( AutoRouterPref_str)->getStringValue();
		autorouterpref_set = true;
	}

	bool helpurl_set = false;
	if( m_ptr->getAttribute( HelpURL_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrHelpURL = m_ptr->getAttribute( HelpURL_str)->getStringValue();
		helpurl_set = true;
	}

	bool isgradientfillenabled_set = false;
	if( m_ptr->getAttribute( IsGradientFillEnabled_str)->getStatus() >= BON::AS_Here)
	{
		m_bAttrIsGradientFillEnabled = m_ptr->getAttribute( IsGradientFillEnabled_str)->getBooleanValue();
		isgradientfillenabled_set = true;
	}

	bool gradientfillcolor_set = false;
	if( m_ptr->getAttribute( GradientFillColor_str)->getStatus() >= BON::AS_Here)
	{
		std::string tmpStr = m_ptr->getAttribute( GradientFillColor_str)->getStringValue();
		m_iAttrGradientFillColor = (COLORREF)strtoul(tmpStr.c_str(), NULL, 16);
		gradientfillcolor_set = true;
	}

	bool gradientfilldirection_set = false;
	if( m_ptr->getAttribute( GradientFillDirection_str)->getStatus() >= BON::AS_Here)
	{
		m_iAttrGradientFillDirection = m_ptr->getAttribute( GradientFillDirection_str)->getIntegerValue();
		gradientfilldirection_set = true;
	}

	bool isshadowcastenabled_set = false;
	if( m_ptr->getAttribute( IsShadowCastEnabled_str)->getStatus() >= BON::AS_Here)
	{
		m_bAttrIsShadowCastEnabled = m_ptr->getAttribute( IsShadowCastEnabled_str)->getBooleanValue();
		isshadowcastenabled_set = true;
	}

	bool shadowcolor_set = false;
	if( m_ptr->getAttribute( ShadowColor_str)->getStatus() >= BON::AS_Here)
	{
		std::string tmpStr = m_ptr->getAttribute( ShadowColor_str)->getStringValue();
		m_iAttrShadowColor = (COLORREF)strtoul(tmpStr.c_str(), NULL, 16);
		shadowcolor_set = true;
	}

	bool shadowthickness_set = false;
	if( m_ptr->getAttribute( ShadowThickness_str)->getStatus() >= BON::AS_Here)
	{
		m_iAttrShadowThickness = m_ptr->getAttribute( ShadowThickness_str)->getIntegerValue();
		shadowthickness_set = true;
	}

	bool shadowdirection_set = false;
	if( m_ptr->getAttribute( ShadowDirection_str)->getStatus() >= BON::AS_Here)
	{
		m_iAttrShadowDirection = m_ptr->getAttribute( ShadowDirection_str)->getIntegerValue();
		shadowdirection_set = true;
	}

	bool isroundrectangleenabled_set = false;
	if( m_ptr->getAttribute( IsRoundRectangleEnabled_str)->getStatus() >= BON::AS_Here)
	{
		m_bAttrIsRoundRectangleEnabled = m_ptr->getAttribute( IsRoundRectangleEnabled_str)->getBooleanValue();
		isroundrectangleenabled_set = true;
	}

	bool roundrectangleradius_set = false;
	if( m_ptr->getAttribute( RoundRectangleRadius_str)->getStatus() >= BON::AS_Here)
	{
		m_iAttrRoundRectangleRadius = m_ptr->getAttribute( RoundRectangleRadius_str)->getIntegerValue();
		roundrectangleradius_set = true;
	}


	// proxies have only the following attributes: abstract, inrootfolder, displayedname, generalpref
	// real objects have beside these other attributes

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
		if( (*it)->getAttribute( IsAbstract_str)->getStatus() >= BON::AS_Here) // if set by the user
		{
			isabs = isabs && (*it)->getAttribute( IsAbstract_str)->getBooleanValue();
			isabs_set = true;
		}
#if(1)
		// general pref
		if ( !isgenpref_set && (*it)->getAttribute( GeneralPreferences_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrGenPref = (*it)->getAttribute( GeneralPreferences_str)->getStringValue();
			isgenpref_set = true;
		}

		// displayed name
		if ( !isdispname_set && (*it)->getAttribute( DisplayedName_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrDispName = (*it)->getAttribute( DisplayedName_str)->getStringValue();
			isdispname_set = true;
		}

		// --applicable to non proxies only--
		if ((*it)->getObjectMeta().name().find("Proxy") != std::string::npos) continue;

		if( !ishotspotenabled_set && (*it)->getAttribute( IsHotspotEnabled_str)->getStatus() >= BON::AS_Here)
		{
			m_bAttrIsHotspotEnabled	= m_bAttrIsHotspotEnabled || (*it)->getAttribute( IsHotspotEnabled_str)->getBooleanValue();
			ishotspotenabled_set = true;
		}
		
		if( !istypeshown_set && (*it)->getAttribute( IsTypeShown_str)->getStatus() >= BON::AS_Here)
		{
			m_bAttrIsTypeShown		= m_bAttrIsTypeShown || (*it)->getAttribute( IsTypeShown_str)->getBooleanValue();
			istypeshown_set = true;
		}
		
		if( !isnameenabled_set && (*it)->getAttribute( IsNameEnabled_str)->getStatus() >= BON::AS_Here)
		{
			m_bAttrIsNameEnabled	= m_bAttrIsNameEnabled || (*it)->getAttribute( IsNameEnabled_str)->getBooleanValue();
			isnameenabled_set = true;
		}


		if ( !isnamepos_set && (*it)->getAttribute( NamePosition_str)->getStatus() >= BON::AS_Here)
		{
			m_iAttrNamePosition	= (*it)->getAttribute( NamePosition_str)->getIntegerValue();
			isnamepos_set = true;
		}
		if ( !isnamewrapnum_set && (*it)->getAttribute( NameWrapNum_str)->getStatus() >= BON::AS_Here)
		{
			m_iAttrNameWrapNum	= (*it)->getAttribute( NameWrapNum_str)->getIntegerValue();
			isnamewrapnum_set = true;
		}


		if ( !icon_set && (*it)->getAttribute( Icon_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrIcon = (*it)->getAttribute( Icon_str)->getStringValue();
			icon_set = true;
		}
		if ( !porticon_set && (*it)->getAttribute( PortIcon_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrPortIcon = (*it)->getAttribute( PortIcon_str)->getStringValue();
			porticon_set = true;
		}
		if( !subtypeicon_set && (*it)->getAttribute( SubTypeIcon_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrSubTypeIcon = (*it)->getAttribute( SubTypeIcon_str)->getStringValue();
			subtypeicon_set = true;
		}
		if ( !instanceicon_set && (*it)->getAttribute( InstanceIcon_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrInstanceIcon = (*it)->getAttribute( InstanceIcon_str)->getStringValue();
			instanceicon_set = true;
		}
		if ( !decorator_set && (*it)->getAttribute( Decorator_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrDecorator = (*it)->getAttribute( Decorator_str)->getStringValue();
			decorator_set = true;
		}

		if ( !isresizable_set && (*it)->getAttribute( IsResizable_str)->getStatus() >= BON::AS_Here)
		{
			m_bAttrIsResizable = (*it)->getAttribute( IsResizable_str)->getBooleanValue();
			isresizable_set = true;
		}

		if ( !autorouterpref_set && (*it)->getAttribute( AutoRouterPref_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrAutoRouterPref = (*it)->getAttribute( AutoRouterPref_str)->getStringValue();
			autorouterpref_set = true;
		}

		if ( !helpurl_set && (*it)->getAttribute( HelpURL_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrHelpURL = (*it)->getAttribute( HelpURL_str)->getStringValue();
			helpurl_set = true;
		}

		if ( !isgradientfillenabled_set && (*it)->getAttribute( IsGradientFillEnabled_str)->getStatus() >= BON::AS_Here)
		{
			m_bAttrIsGradientFillEnabled = (*it)->getAttribute( IsGradientFillEnabled_str)->getBooleanValue();
			isgradientfillenabled_set = true;
		}

		if ( !gradientfillcolor_set && (*it)->getAttribute( GradientFillColor_str)->getStatus() >= BON::AS_Here)
		{
			std::string tmpStr = (*it)->getAttribute( GradientFillColor_str)->getStringValue();
			m_iAttrGradientFillColor = (COLORREF)strtoul(tmpStr.c_str(), NULL, 16);
			gradientfillcolor_set = true;
		}

		if ( !gradientfilldirection_set && (*it)->getAttribute( GradientFillDirection_str)->getStatus() >= BON::AS_Here)
		{
			m_iAttrGradientFillDirection = (*it)->getAttribute( GradientFillDirection_str)->getIntegerValue();
			gradientfilldirection_set = true;
		}

		if ( !isshadowcastenabled_set && (*it)->getAttribute( IsShadowCastEnabled_str)->getStatus() >= BON::AS_Here)
		{
			m_bAttrIsShadowCastEnabled = (*it)->getAttribute( IsShadowCastEnabled_str)->getBooleanValue();
			isshadowcastenabled_set = true;
		}

		if ( !shadowcolor_set && (*it)->getAttribute( ShadowColor_str)->getStatus() >= BON::AS_Here)
		{
			std::string tmpStr = (*it)->getAttribute( ShadowColor_str)->getStringValue();
			m_iAttrShadowColor = (COLORREF)strtoul(tmpStr.c_str(), NULL, 16);
			shadowcolor_set = true;
		}

		if ( !shadowthickness_set && (*it)->getAttribute( ShadowThickness_str)->getStatus() >= BON::AS_Here)
		{
			m_iAttrShadowThickness = (*it)->getAttribute( ShadowThickness_str)->getIntegerValue();
			shadowthickness_set = true;
		}

		if ( !shadowdirection_set && (*it)->getAttribute( ShadowDirection_str)->getStatus() >= BON::AS_Here)
		{
			m_iAttrShadowDirection = (*it)->getAttribute( ShadowDirection_str)->getIntegerValue();
			shadowdirection_set = true;
		}

		if ( !isroundrectangleenabled_set && (*it)->getAttribute( IsRoundRectangleEnabled_str)->getStatus() >= BON::AS_Here)
		{
			m_bAttrIsRoundRectangleEnabled = (*it)->getAttribute( IsRoundRectangleEnabled_str)->getBooleanValue();
			isroundrectangleenabled_set = true;
		}

		if ( !roundrectangleradius_set && (*it)->getAttribute( RoundRectangleRadius_str)->getStatus() >= BON::AS_Here)
		{
			m_iAttrRoundRectangleRadius = (*it)->getAttribute( RoundRectangleRadius_str)->getIntegerValue();
			roundrectangleradius_set = true;
		}
#endif
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
		global_vars.err << MSG_ERROR << m_ptr << " fco has another kind of ancestor or descendant.\n";
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
		ReferenceRep * r = *ref_list.begin(); // take a ref from the final references to me
		ref_list.pop_front();
		std::vector< ReferenceRep *> temp = r->getFinalReferences(); // take its references

		std::vector<ReferenceRep *>::iterator temp_it = temp.begin();
		for( ; temp_it != temp.end(); ++temp_it)
		{
			if( std::find( multiple_refs.begin(), multiple_refs.end(), *temp_it) == multiple_refs.end()) // not found a ref in the current transitive ref list
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
		global_vars.err << MSG_WARNING << "CHECK: " << (*jt)->getPtr() <<
			" base class is two times in direct inheritance " <<
			" relation with the derived class " <<
			m_ptr << "\n";
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


const std::vector<FCO *>&  FCO::getParents( INHERITANCE_TYPE type)
{
	return m_parentList[type];
}


const std::vector<FCO *>& FCO::getChildren( INHERITANCE_TYPE type)
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
		global_vars.err << MSG_ERROR << attr->getPtr() << " attribute owned by " << m_ptr << " twice\n";
}


void FCO::addFinalAttribute( AttributeRep * attr)
{
	AttributeRepPtrList_ConstIterator it = 
		std::find( m_finalAttributeList.begin(), m_finalAttributeList.end(), attr);

	if ( it == m_finalAttributeList.end()) // not found so insert
		m_finalAttributeList.push_back( attr);
	/*else not an error because of multiple inheritance
		global_vars.err << attr->getName() << " attribute owned by " << getName() << " twice\n";*/
}


void FCO::addFinalAttributeList(const AttributeRepPtrList& t_list)
{
	AttributeRepPtrList_ConstIterator it_pos = t_list.begin();
	for( ; it_pos != t_list.end(); ++it_pos)
		addFinalAttribute( *it_pos);
}

bool FCO::findFinalAttributeBasedOnName( const std::string & name)
{
	AttributeRepPtrList_Iterator it;
	it = m_finalAttributeList.begin();
	while( it != m_finalAttributeList.end() && !((*it)->getName() == name /*&& (*it)->isViewable()*/))
		++it;
	return ( it != m_finalAttributeList.end()); // if found
}


// dumps out all owned (non-global) attributes
std::string FCO::dumpAttributes()
{
	std::string mmm = "";
	AttributeRepPtrList_Iterator it;
	
	AnyLexicographicSort lex;
	std::string last_name = "";
	std::sort( m_finalAttributeList.begin(), m_finalAttributeList.end(), lex );

	it = m_finalAttributeList.begin();
	for( ; it != m_finalAttributeList.end(); ++it)
		if ( !(*it)->isGlobal())
		{
			//if ( (*it)->isViewable())
			mmm += (*it)->doDumpAttr( getName());
			if ( last_name != "" && last_name == (*it)->getName())
				global_vars.err << MSG_ERROR << "Duplicate attribute name " << (*it)->getPtr() << " found for " << m_ptr << "\n";
			last_name = (*it)->getName();
		}

	return mmm;
}


// dumps out sorted attribute name list 
std::string FCO::dumpAttributeList( bool check_viewable /* = false */ )
{
	std::string mmm = "";

	std::vector<AttributeRep*> temp_list;
	std::copy_if(m_finalAttributeList.begin(), m_finalAttributeList.end(),
		std::back_inserter(temp_list), [&](AttributeRep* p) { return !check_viewable || p->isViewable(); });
	std::sort(temp_list.begin(), temp_list.end(), [](AttributeRep* a, AttributeRep* b) { return a->lessThan(b); });

	if ( !temp_list.empty())
	{
		mmm = " attributes = \"";
		std::vector<AttributeRep*>::iterator t_it;
		for( t_it = temp_list.begin(); t_it != temp_list.end(); ++t_it)
		{
			if (t_it != temp_list.begin()) mmm += " ";
			mmm += (*t_it)->getName();
		}
		mmm += "\"";
	}
	return mmm;
}


std::string FCO::dumpIcon()
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpIcon();
	}

	//if ( this->m_ptr)
	{
		std::string &icon = m_sAttrIcon;//m_ptr->getAttribute( Icon_str)->getStringValue();

		if( !icon.empty())
			mmm += indStr() + "<regnode name = \"icon\" value =\"" + icon + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpPortIcon()
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpPortIcon();
	}

	//if ( this->m_ptr)
	{
		std::string &icon = m_sAttrPortIcon;//m_ptr->getAttribute( PortIcon_str)->getStringValue();

		if( !icon.empty())
			mmm += indStr() + "<regnode name = \"porticon\" value =\"" + icon + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpDecorator()
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpDecorator();
	}

	//if ( this->m_ptr)
	{
		std::string &icon = m_sAttrDecorator;//m_ptr->getAttribute( Decorator_str)->getStringValue();

		if( !icon.empty())
			mmm += indStr() + "<regnode name = \"decorator\" value =\"" + icon + "\"></regnode>\n";
	}
	return mmm;

}


std::string FCO::dumpHotspotEnabled()
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpHotspotEnabled();
	}

	//if ( this->m_ptr)
	{
		bool &icon = m_bAttrIsHotspotEnabled;//m_ptr->getAttribute( IsHotspotEnabled_str)->getBooleanValue();

		if( !icon)
			mmm += indStr() + "<regnode name = \"isHotspotEnabled\" value =\"false\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpTypeShown()
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpTypeShown();
	}

	//if ( this->m_ptr)
	{
		bool &icon = m_bAttrIsTypeShown;//m_ptr->getAttribute( IsTypeShown_str)->getBooleanValue();

		if( icon)
			mmm += indStr() + "<regnode name = \"isTypeShown\" value =\"true\"></regnode>\n";
	}
	return mmm;
}



std::string Any::dumpGeneralPref()
{
	std::string mmm;
	std::vector<Any*> ancestors = getGeneralPrefAncestors();
	std::vector<Any*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpGeneralPref();
	}
	
	//if ( this->m_ptr)
	{
		std::string &prefs_1 = m_sAttrGenPref;//m_ptr->getAttribute( GeneralPreferences_str)->getStringValue();

		if (prefs_1.empty()) return mmm;

		bool berr = false;
		//CString prefs = prefs_1.c_str();
		std::string prefs = prefs_1;
		//CStringArray prefArr;
		std::vector< std::string > prefArr;
		//CTokenEx	 tok;
		Tokenizer tok;
		//tok.Split(prefs, _T("\n"), prefArr);
		tok.split( prefs, _T("\n"), prefArr);
		//for (int i = 0; i < prefArr.GetSize(); i++) {
		for (int i = 0; i < prefArr.size(); i++) {
			//CStringArray prefPair;
			std::vector< std::string > prefPair;
			//tok.Split(prefArr[i], _T("="), prefPair);
			tok.split( prefArr[i], _T("="), prefPair);
			// Compatibility with the older format
			//if (prefPair.GetSize() != 2) {
			if (prefPair.size() != 2) {
				//prefPair.RemoveAll();
				prefPair.clear();
				//tok.Split(prefArr[i], _T(","), prefPair);
				tok.split( prefArr[i], _T(","), prefPair);
				//if (prefPair.GetSize() == 2) {
				if (prefPair.size() == 2) {
					global_vars.err << MSG_WARNING << "Warning: Deprecated general preferences format for " << m_ptr << ".\n" << MSG_NORMAL << "Valid format:\n\t<prefname1> = <value1>\n\t<prefname2> = <value2>\n\t...\n";
				}
			}
			//if (prefPair.GetSize() == 2) {
			if (prefPair.size() == 2) {
				//prefPair[0].TrimLeft();
				tok.trimLeft( prefPair[0]);
				//prefPair[0].TrimRight();
				tok.trimRight( prefPair[0]);
				//prefPair[1].TrimLeft();
				tok.trimLeft( prefPair[1]);
				//prefPair[1].TrimRight();
				tok.trimRight( prefPair[1]);
				//mmm += indStr() + "<regnode name = \"" + (LPCTSTR)prefPair[0] + "\" value =\"" + (LPCTSTR)prefPair[1] + "\"></regnode>\n";
				if( prefPair[0].empty())
					global_vars.err << MSG_ERROR << "Empty General Preference token found at " << m_ptr << " with \"" << prefPair[1] << "\" as value.\n";
				else
					mmm += indStr() + "<regnode name = \"" + prefPair[0] + "\" value =\"" + prefPair[1] + "\"></regnode>\n";
			}
			else {
				berr = true;
				break;
			}
		}
		if(berr) {
			global_vars.err << MSG_ERROR << "Invalid general preferences specification for " << m_ptr << ".\n"
				<< MSG_NORMAL << "Valid format:\n\t<prefname1> = <value1>\n\t<prefname2> = <value2>\n\t...\n";
		}
	}
	return mmm;
}
 

std::string FCO::dumpNamePosition() const
{
	CString p;
	//if ( this->m_ptr)
	{
		int name_pos = min(8,max(0,m_iAttrNamePosition));//m_ptr->getAttribute( NamePosition_str)->getIntegerValue();
		p.Format("%ld", name_pos);
	}
	//else mmm = "NullPtrError";
	return indStr() + "<regnode name = \"namePosition\" value =\"" + (const char*)p +"\">" + "</regnode>\n";
}


std::string FCO::dumpSubTypeIcon() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpSubTypeIcon();
	}

	//if ( this->m_ptr)
	{
		const std::string &icon = m_sAttrSubTypeIcon;//m_ptr->getAttribute( SubTypeIcon_str)->getStringValue();

		if( !icon.empty())
			mmm += indStr() + "<regnode name = \"subTypeIcon\" value =\"" + icon + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpInstanceIcon() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpInstanceIcon();
	}

	//if ( this->m_ptr)
	{
		const std::string &icon = m_sAttrInstanceIcon;//m_ptr->getAttribute( InstanceIcon_str)->getStringValue();

		if( !icon.empty())
			mmm += indStr() + "<regnode name = \"instanceIcon\" value =\"" + icon + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpNameWrap() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpNameWrap();
	}

	//if ( this->m_ptr)
	{
		const int &icon = m_iAttrNameWrapNum;//m_ptr->getAttribute( NameWrapNum_str)->getIntegerValue();
		CString p;
		p.Format("%ld", icon);
		if( icon != 0)
			mmm += indStr() + "<regnode name = \"nameWrap\" value =\"" + (const char*)p + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpNameEnabled() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpNameEnabled();
	}

	//if ( this->m_ptr)
	{
		const bool &icon = m_bAttrIsNameEnabled;//m_ptr->getAttribute( IsNameEnabled_str)->getBooleanValue();

		if( !icon)
			mmm += indStr() + "<regnode name = \"isNameEnabled\" value =\"false\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpResizable() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpResizable();
	}

	//if ( this->m_ptr)
	{
		bool enabled = m_bAttrIsResizable;//m_ptr->getAttribute( IsResizable_str)->getBooleanValue();

		if( enabled)
			mmm += indStr() + "<regnode name = \"itemResizable\" value =\"true\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpAutoRouterPref() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpAutoRouterPref();
	}

	//if ( this->m_ptr)
	{
		const std::string &formatStr = m_sAttrAutoRouterPref;//m_ptr->getAttribute( AutoRouterPref_str)->getStringValue();

		if( !formatStr.empty())
			mmm += indStr() + "<regnode name = \"autorouterPref\" value =\"" + formatStr + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpHelpURL() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpHelpURL();
	}

	//if ( this->m_ptr)
	{
		const std::string &helpURL = m_sAttrHelpURL;//m_ptr->getAttribute( HelpURL_str)->getStringValue();

		if( !helpURL.empty())
			mmm += indStr() + "<regnode name = \"help\" value =\"" + helpURL + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpGradientFillEnabled() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpGradientFillEnabled();
	}

	//if ( this->m_ptr)
	{
		bool enabled = m_bAttrIsGradientFillEnabled;//m_ptr->getAttribute( IsGradientFillEnabled_str)->getBooleanValue();

		if( enabled)
			mmm += indStr() + "<regnode name = \"gradientFill\" value =\"true\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpGradientFillColor() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpGradientFillColor();
	}

	//if ( this->m_ptr)
	{
		const unsigned int color = (unsigned int)m_iAttrGradientFillColor;//m_ptr->getAttribute( GradientFillColor_str)->getIntegerValue();
		CString p;
		p.Format("0x%06x", color);
		if( m_iAttrGradientFillColor != RGB(0xC0,0xC0,0xC0))
			mmm += indStr() + "<regnode name = \"gradientColor\" value =\"" + (const char*)p + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpGradientFillDirection() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpGradientFillDirection();
	}

	//if ( this->m_ptr)
	{
		const int direction = m_iAttrGradientFillDirection;//m_ptr->getAttribute( GradientFillDirection_str)->getIntegerValue();
		CString p;
		p.Format("%ld", direction);
		if( direction != 0)
			mmm += indStr() + "<regnode name = \"gradientDirection\" value =\"" + (const char*)p + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpShadowCastEnabled() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpShadowCastEnabled();
	}

	//if ( this->m_ptr)
	{
		bool enabled = m_bAttrIsShadowCastEnabled;//m_ptr->getAttribute( IsShadowCastEnabled_str)->getBooleanValue();

		if( enabled)
			mmm += indStr() + "<regnode name = \"itemShadowCast\" value =\"true\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpShadowColor() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpShadowColor();
	}

	//if ( this->m_ptr)
	{
		const unsigned int color = (unsigned int)m_iAttrShadowColor;//m_ptr->getAttribute( ShadowColor_str)->getIntegerValue();
		CString p;
		p.Format("0x%06x", color);
		if( m_iAttrShadowColor != RGB(0xC0,0xC0,0xC0))
			mmm += indStr() + "<regnode name = \"shadowColor\" value =\"" + (const char*)p + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpShadowThickness() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpShadowThickness();
	}

	//if ( this->m_ptr)
	{
		const int thickness = m_iAttrShadowThickness;//m_ptr->getAttribute( ShadowThickness_str)->getIntegerValue();
		CString p;
		p.Format("%ld", thickness);
		if( thickness != 9)
			mmm += indStr() + "<regnode name = \"shadowThickness\" value =\"" + (const char*)p + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpShadowDirection() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpShadowDirection();
	}

	//if ( this->m_ptr)
	{
		const int direction = m_iAttrShadowDirection;//m_ptr->getAttribute( ShadowDirection_str)->getIntegerValue();
		CString p;
		p.Format("%ld", direction);
		if( direction != 45)
			mmm += indStr() + "<regnode name = \"shadowDirection\" value =\"" + (const char*)p + "\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpRoundRectangleEnabled() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpRoundRectangleEnabled();
	}

	//if ( this->m_ptr)
	{
		bool enabled = m_bAttrIsRoundRectangleEnabled;//m_ptr->getAttribute( IsRoundRectangleEnabled_str)->getBooleanValue();

		if( enabled)
			mmm += indStr() + "<regnode name = \"roundCornerRect\" value =\"true\"></regnode>\n";
	}
	return mmm;
}


std::string FCO::dumpRoundRectangleRadius() const
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		mmm += (*it)->dumpRoundRectangleRadius();
	}

	//if ( this->m_ptr)
	{
		const int radius = m_iAttrRoundRectangleRadius;//m_ptr->getAttribute( RoundRectangleRadius_str)->getIntegerValue();
		CString p;
		p.Format("%ld", radius);
		if( radius != 9)
			mmm += indStr() + "<regnode name = \"roundCornerRadius\" value =\"" + (const char*)p + "\"></regnode>\n";
	}
	return mmm;
}

