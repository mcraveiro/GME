#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "ReferenceRep.h"
#include "ModelRep.h"
#include "Dumper.h"
#include "Broker.h"

#include "logger.h"
#include "algorithm"

#include "globals.h"
extern Globals global_vars;

#include <iterator>

/*static*/ const std::string ModelRep::IsTypeInfoShown_str = "IsTypeInfoShown";

ModelRep::ModelRep( BON::FCO& ptr, BON::FCO& resp_ptr)
	: FCO( ptr, resp_ptr)
	, m_initialRoleMap()
	, m_finalRoleMap()
	, m_initialAspectList()
	, m_finalAspectList()
	, m_bAttrIsTypeInfoShown( true )
{
}


ModelRep::~ModelRep()
{
	m_initialRoleMap.clear();
	m_finalRoleMap.clear();
	m_initialAspectList.clear();
	m_finalAspectList.clear();
}

void ModelRep::initAttributes()
{
	
	FCO::initAttributes();

	bool istypeinfoshown_set = false;
	if( m_ptr->getAttribute( IsTypeInfoShown_str)->getStatus() >= BON::AS_Here)
	{
		m_bAttrIsTypeInfoShown		= m_ptr->getAttribute( IsTypeInfoShown_str)->getBooleanValue();		// def val: FALSE (dumped if TRUE)
		istypeinfoshown_set = true;
	}
	
	
	std::set< BON::FCO >::const_iterator it = m_equivs.begin();
	for ( ; it != m_equivs.end(); ++it)
	{
		if ( *it == m_ptr) continue;
		if ((*it)->getObjectMeta().name().find("Proxy") != std::string::npos) continue;

		if( !istypeinfoshown_set && (*it)->getAttribute( IsTypeInfoShown_str)->getStatus() >= BON::AS_Here)
		{
			m_bAttrIsTypeInfoShown		= m_bAttrIsTypeInfoShown || (*it)->getAttribute( IsTypeInfoShown_str)->getBooleanValue();
			istypeinfoshown_set = true;
		}
	}
}
	
void ModelRep::addRole( FCO* whose, RoleSeriesValue& role) 
{ 
	RoleMapValue &series = m_initialRoleMap[ whose ];

	// avoiding double insertion
	// searching for the role in the role register
	RoleSeries_Iterator it = std::find( series.begin(), series.end(), role);

	if ( it == series.end() ) // not found
		series.push_back( role);
	else 
		global_vars.err << MSG_ERROR << "Error: Found duplicate role \"" << role.getSmartRoleName() << "\" in model \"" << m_ptr << "\"\n";
}


void ModelRep::initRoles()
{
	RoleMap_Iterator it = m_initialRoleMap.begin();
	for( ; it != m_initialRoleMap.end(); ++it)
	{
		RoleMapKey fco = it->first; // FCO *
		std::string nn = fco->getName();

		std::vector<FCO*> children;
		fco->getIntDescendants( children);

		//global_vars.err << getName() << ":" << nn << " # of int desc " << children.size() << "\n";

		int how_many_non_abstract = 0;
		if ( !fco->isAbstract()) ++how_many_non_abstract;
		for( unsigned int i = 0; i < children.size(); ++i)
		{
			std::string cn = children[i]->getName();
			if (! children[i]->isAbstract()) 
				++how_many_non_abstract;
		}

		// at this point the how_many_non_abstract
		// value indicates how many non abstract instances may exist
		// and how many will have the use the same role names
		// so if this value is greater than 1 then long form, otherwise 
		// short form (default) should be used

		if ( how_many_non_abstract > 1 ) // long_form_flag needs to be true for this fco's roles
		{
			RoleMapValue &series = it->second;
			RoleSeries_Iterator role_it = series.begin();
			for(; role_it != series.end(); ++role_it)
			{
				role_it->setLongForm( true);
			}
		}
	}
}


bool ModelRep::getRoles( FCO * ptr, RoleMapValue& map_val) const
{
	RoleMap_ConstIterator it = m_initialRoleMap.find( ptr);
	if ( it == m_initialRoleMap.end())
		return false;

	map_val = it->second;
	return true;
}


std::vector< ModelRep* > ModelRep::getInnerModels() const
{
	std::vector< ModelRep* > models;

	RoleMap_ConstIterator it = m_initialRoleMap.begin();
	while ( it != m_initialRoleMap.end())
	{
		FCO * const ptr = it->first; // ModelRep *
		if ( ptr->getMyKind() == Any::MODEL)
		{
			ModelRep * mod_ptr = dynamic_cast< ModelRep *> ( ptr);
			models.push_back( mod_ptr);
		}
		++it;
	}
	return models;
}


void ModelRep::addAspect( AspectRep * asp_ptr)
{
	std::vector<AspectRep*>::iterator ii = 
		std::find( m_initialAspectList.begin(), m_initialAspectList.end(), asp_ptr);

	if (ii == m_initialAspectList.end()) // not found
		m_initialAspectList.push_back(asp_ptr);
	else
		global_vars.err << MSG_ERROR << "CHECK: Model \"" << m_ptr << "\" has aspect \"" << asp_ptr->getPtr() << "\" associated twice\n";
}


const ModelRep::AspectRepPtrList& ModelRep::getAspectRepPtrList() const
{
	return m_initialAspectList;
}


/*------------------------------------------------------------*/


void ModelRep::addFinalRole( FCO* whose, RoleSeriesValue& role) 
{ 
	RoleMapValue &series = m_finalRoleMap[ whose ];

	// avoiding double insertion
	// searching for the role in the role register
	RoleSeries_Iterator it = std::find( series.begin(), series.end(), role);

	if ( it == series.end() ) // not found
		series.push_back( role);
	else 
		global_vars.err << MSG_ERROR << "Error: Found duplicate final role \"" << role.getSmartRoleName() << "\" in model \"" << m_ptr << "\"\n";
}

bool ModelRep::getFinalRoles( const FCO * ptr, RoleMapValue& map_val) const
{
	RoleMap_ConstIterator it = m_finalRoleMap.find( const_cast<FCO *>( ptr));
	if ( it == m_finalRoleMap.end())
		return false;

	map_val = it->second;
	return true;
}


std::vector< ModelRep* > ModelRep::getInnerModelsFinal() const
{
	std::vector< ModelRep* > models;

	RoleMap_ConstIterator it = m_finalRoleMap.begin();
	while ( it != m_finalRoleMap.end())
	{
		FCO * const ptr = it->first; // ModelRep *
		if ( ptr->getMyKind() == Any::MODEL)
		{
			ModelRep * mod_ptr = dynamic_cast< ModelRep *> ( ptr);
			models.push_back( mod_ptr);
		}
		++it;
	}
	return models;
}


std::vector< ReferenceRep *> ModelRep::getInnerModelReferencesFinal() const
{
	std::vector< ReferenceRep *> refs;
	RoleMap_ConstIterator it = m_finalRoleMap.begin();
	while ( it != m_finalRoleMap.end())
	{
		FCO * const ptr = it->first;
		if ( ptr->getMyKind() == Any::REF)
		{
			ReferenceRep * ref_ptr = dynamic_cast< ReferenceRep *> ( ptr);
			if ( ref_ptr->pointsToModels())
				refs.push_back( ref_ptr);
		}
		++it;
	}
	return refs;
}


// adding new roles for descendants of current members
//
//    B----x-<>Model1
//    |        <>  |
//   /^\       |   |
// A1   A2     y   ^
//        \    |   |
//         A3  | Model2
//           \ |
//            A4
//              \
//               A5
// in this case the A1,A2 and A3 may have its own roles in M, called inherited roles
// but A4 and A5 will have two roles: x and y

void ModelRep::inherit()
{
	std::vector<FCO*> model_descendants;
	this->getImpDescendants( model_descendants);
	model_descendants.push_back( this);

	// for each model implementation descendant
	std::vector<FCO*>::reverse_iterator model_it = model_descendants.rbegin();
	for( ; model_it != model_descendants.rend(); ++model_it)
	{
		if ( (*model_it)->getMyKind() != Any::MODEL)
			throw std::string("Error: nonmodel kind ") + (*model_it)->getPtr()->getName() + " has model descendant " + this->getName();
		ModelRep * mod_desc_ptr = dynamic_cast<ModelRep*>(*model_it);
		if (!mod_desc_ptr) global_vars.err << MSG_ERROR << "Error: model descendant badly casted to model\n";
					
		RoleMap_Iterator fco_it( m_initialRoleMap.begin());
		// for each FCO
		for( ; fco_it != m_initialRoleMap.end(); ++fco_it)
		{ 
			RoleMapKey fco_ptr = fco_it->first;
			std::vector<FCO*> children;
			
			fco_ptr->getIntDescendants( children);
			children.push_back( fco_ptr);

			// take its roleList
			RoleSeries &roles = fco_it->second; // RoleMapValue

			RoleSeries_Iterator role_it = roles.begin();
			// for each role
			for( ; role_it != roles.end(); ++role_it)
			{
				std::vector<FCO*>::iterator desc_it = children.begin();
				// for each interface descendant
				for( ; desc_it != children.end(); ++desc_it)
				{
					// the descendants must have a separate role for each initial role
					// this role is inherited and has long form depending on the hierarchy_flag
					RoleRep r( 
						role_it->getOnlyRoleName(),
						*desc_it,
						mod_desc_ptr,//role_it->getModelRepPtr(), // = this
						role_it->isPort(),
						role_it->getCardinality(),
						mod_desc_ptr != this,//true, // inherited role
						role_it->isLongForm());
					
					mod_desc_ptr->addFinalRole( *desc_it, r);
					(*desc_it)->iAmPartOfFinal( mod_desc_ptr);//(*desc_it)->iAmPartOf( this);
				}
			}
		}
	}
}


void ModelRep::sortMyAspects()
{
	// sort my aspects based on lessThan
	AspectCompare asp_less_than;
	std::sort( m_finalAspectList.begin(), m_finalAspectList.end(), asp_less_than);
}


void ModelRep::createPartsInModelAspects()
{
	RoleMap_Iterator role_it = m_finalRoleMap.begin();
	// for all roles in this model
	for( ; role_it != m_finalRoleMap.end(); ++role_it)
	{
		RoleStringLex lex;
		std::sort( role_it->second.begin(), role_it->second.end(), lex);
		std::vector<AspectRep*>::iterator asp_it = m_finalAspectList.begin();
		// for all my aspects 
		for( ; asp_it != m_finalAspectList.end(); ++asp_it)
		{
			// the fco which owns the roles
			RoleMapKey fco_ptr = role_it->first;
			// if part (fco) is intended to be in the aspect
			if ( !fco_ptr->isAbstract())
			{
				if ( (*asp_it)->findFinalFCO( fco_ptr) || (*asp_it)->isDummy())
				{
					// each role of fco is member of aspect
					RoleSeries_Iterator jt = role_it->second.begin();
					for( ; jt != role_it->second.end(); ++jt)
					{
						PartRep pr( *jt, *asp_it);
						(*asp_it)->addPart2Map( this, pr); // add parts to the aspect
					}
				}
				else
				{
					// some roles are going to become part of aspect
					RoleSeries_Iterator jt = role_it->second.begin();
					for( ; jt != role_it->second.end(); ++jt)
					{
						if ( (*asp_it)->findFinalRole( *jt)) // this role is intended to be in the aspect
						{
							PartRep pr( *jt, *asp_it);
							(*asp_it)->addPart2Map( this, pr); // add parts to the aspect
						}
					}
				}
			}
		}
	}
}


const ModelRep::AspectRepPtrList& ModelRep::getFinalAspectRepPtrList() const
{
	return m_finalAspectList;
}


void ModelRep::addFinalAspect( AspectRep * asp_ptr)
{
	std::vector<AspectRep*>::iterator ii = 
		std::find( m_finalAspectList.begin(), m_finalAspectList.end(), asp_ptr);

	if (ii == m_finalAspectList.end()) // not found
		m_finalAspectList.push_back(asp_ptr);
	/*else Not an error because of multiple inheritance
		global_vars.err << getName() << " has aspect " << asp_ptr->getName() << " associated twice\n";*/
}


void ModelRep::addFinalAspectList(const AspectRepPtrList& l)
{
	std::vector<AspectRep*>::const_iterator ii = l.begin();
	for( ; ii != l.end(); ++ii)
		addFinalAspect(*ii);
}


int ModelRep::howManyAspects() const
{
	return m_finalAspectList.size();
}


AspectRep * ModelRep::getFirstAspect() const
{
	// FIXME: use std::min_element instead?
	unsigned int i;
	unsigned sel_i = -1;
	for( i = 0; i < m_finalAspectList.size(); ++i)
	{
		if (sel_i == -1)
			sel_i = 0;
		else 
		{
			AspectRep * curr_best = m_finalAspectList[sel_i];
			if ( !curr_best->lessThan( *m_finalAspectList[i]))
			sel_i = i;
		}
	}
	if ( sel_i != -1) return m_finalAspectList[sel_i];
	else return 0;
}


bool ModelRep::findAspect( const AspectRep * one_asp) const
{
	std::vector<AspectRep*>::const_iterator ii = 
		std::find( m_finalAspectList.begin(), m_finalAspectList.end(), one_asp);

	return ii != m_finalAspectList.end(); // found
}


/*
The aspect_set contains aspects with exactly the same name!
This method gives back an aspect pointer with the following features:
-the model owns this aspect
-the aspect is in the incoming set

In other words the first aspect from the intersection of the incoming aspect_set and
the m_finalAspectList
*/
AspectRep* ModelRep::getMyFirstAspectFromSet( const std::vector<AspectRep *> & aspect_set) const
{
	bool found = false;
	
	std::vector<AspectRep *>::const_iterator aspects_it = aspect_set.begin();
	while( aspects_it != aspect_set.end() && !found)
	{
		std::vector<AspectRep*>::const_iterator ii = 
			std::find( m_finalAspectList.begin(), m_finalAspectList.end(), *aspects_it);

		found = ii != m_finalAspectList.end();
		if (!found) ++aspects_it;
	}

	if (found)
		return *aspects_it;
	else 
		return 0;
}


/* 
how many of my aspects contain part/FCO as an aspect member
*/
int ModelRep::searchMyAspectsForPart( PartRep& part) const
{
	const FCO * asp_element = part.getFCOPtr();
	unsigned int i = 0;
	int count = 0;
	while ( i < m_finalAspectList.size())
	{
		if ( m_finalAspectList[i]->findFinalFCO( asp_element) ||
			m_finalAspectList[i]->findFinalRole( *part.getRoleRepPtr()) )
			++count;
		++i;
	}
	return count;
}


void ModelRep::getAspectNames(CStringList &list) const
{
	unsigned int i = 0;
	while ( i < m_finalAspectList.size())
	{
		list.AddTail(m_finalAspectList[i]->getName().c_str());
		++i;
	}
}


std::string ModelRep::dumpTypeInfoShown()
{
	std::string mmm;
	std::vector<FCO*> ancestors;
	bool typeInfoShown = m_bAttrIsTypeInfoShown;

	/*
	// Cannot implement this properly (inheritance). 
	// Not enough information here to decide if this property is inherited or not
	getImpAncestors( ancestors);
	std::vector<FCO*>::iterator it = ancestors.begin();
	for( ; it != ancestors.end(); ++it)
	{
		ModelRep * mod_ptr = dynamic_cast< ModelRep *> (*it);
		if (mod_ptr) {
			typeInfoShown |= mod_ptr->m_bAttrIsTypeInfoShown;
		}
	}
	*/

	if( typeInfoShown) {
		mmm += indStr() + "<regnode name = \"isTypeInfoShown\" value =\"true\"></regnode>\n";
	}
	return mmm;
}


std::string ModelRep::doDump()
{
	bool error = false;
	std::string m_ref = askMetaRef();

	std::string model_name = getName();
	std::string mmm = indStr() + "<model name = \"" + model_name + "\" metaref = \"" + m_ref + "\"";

	std::string dumped_attr_list = dumpAttributeList();
	mmm += dumped_attr_list;
	mmm +=" >\n";
	dumped_attr_list = dumpAttributeList( true); // check viewable ->for aspects
	++ind;
	mmm += dumpDispName();
	++ind;
	mmm += dumpNamePosition();
	mmm += dumpGeneralPref();

	mmm += dumpIcon();
	mmm += dumpPortIcon();
	mmm += dumpDecorator();
	mmm += dumpHotspotEnabled();
	mmm += dumpTypeShown();
	mmm += dumpTypeInfoShown();
	mmm += dumpSubTypeIcon();
	mmm += dumpInstanceIcon();
	mmm += dumpNameWrap();
	mmm += dumpNameEnabled();
	mmm += dumpResizable();
	mmm += dumpAutoRouterPref();
	mmm += dumpHelpURL();

	mmm += dumpGradientFillEnabled();
	mmm += dumpGradientFillColor();
	mmm += dumpGradientFillDirection();
	mmm += dumpShadowCastEnabled();
	mmm += dumpShadowColor();
	mmm += dumpShadowThickness();
	mmm += dumpShadowDirection();
	mmm += dumpRoundRectangleEnabled();
	mmm += dumpRoundRectangleRadius();
	--ind;
	mmm += dumpConstraints();
	mmm += dumpAttributes();

	std::vector<std::string> role_lines_to_dump;
	std::vector< RoleMapKey> key_list;
	// all roles (final)
	for( RoleMap_Iterator it = m_finalRoleMap.begin();
		 it != m_finalRoleMap.end(); 
		 ++it) {
		key_list.push_back( it->first);
	}

	AnyLexicographicSort any_lex;
	std::sort( key_list.begin(), key_list.end(), any_lex);
	
	for( std::vector< RoleMapKey>::iterator key_it = key_list.begin(); 
		 key_it != key_list.end(); 
		 ++key_it )	{
		//RoleMapKey ptr = it->first;
		//RoleMapValue &roles = it->second;
		RoleMapKey ptr = *key_it;
		RoleMapValue &roles = m_finalRoleMap[ ptr];
		RoleSeries_Iterator jt = roles.begin();
		for( ; jt != roles.end(); ++jt)
			if (!jt->getFCOPtr()->isAbstract())
			{
				const std::string &role_name = jt->getSmartRoleName();
				const std::string &kind_name = ptr->getName();

				std::string m_ref = jt->getFCOPtr()->askMetaRef("/" + model_name + "/" + role_name);
				//std::string role_line = indStr() + "<role name = \"" + role_name + "\" metaref = \"" + m_ref +"\" kind = \"" + kind_name + "\"><dispname>" + role_name + "</dispname></role>\n";
				std::string role_line = indStr() + "<role name = \"" + role_name + "\" metaref = \"" + m_ref +"\" kind = \"" + kind_name + "\"></role>\n";
				
				if ( std::find(role_lines_to_dump.begin(), 
					role_lines_to_dump.end(), role_line) 
					!= role_lines_to_dump.end()) // found
				{
					RoleRep &r = *jt;
					global_vars.err << MSG_ERROR << "Duplicate role line found: " << role_line << "\n" 
						<< MSG_NORMAL
						<< role_name << " of kind " << kind_name
						<< " in model " << m_ptr << ". Details of the 2nd role following: "
						<< (r.isInheritedRole()?"":"Not ") << "Inherited Role, "
						<< (r.isLongForm()?"":"Not ") << "Long form, "
						<< "Is " << (r.isPort()?"":"Not ") << "Port, "
						<< "Cardinality: " << r.getCardinality() << " Not dumping twice.\n";
					if (!error) 
					{
						TO("Please check the metamodel for duplicate roles.");
						error = true;
					}
				}
				else
					role_lines_to_dump.push_back( role_line);
			}
	}
	StringLex lex;
	std::sort( role_lines_to_dump.begin(), role_lines_to_dump.end(), lex);

	std::vector<std::string>::iterator sorted_list_it = role_lines_to_dump.begin();
	for( ; sorted_list_it != role_lines_to_dump.end(); ++sorted_list_it)
		mmm += *sorted_list_it;

	role_lines_to_dump.clear();

	// aspects
	{
		AspectCompare asp_less_than;
		std::sort( m_finalAspectList.begin(), m_finalAspectList.end(), asp_less_than);
		std::vector<AspectRep *>::iterator asp_it = m_finalAspectList.begin();
		for( ; asp_it != m_finalAspectList.end(); ++asp_it)
		{
			AspectRep * asp_ptr = *asp_it;
			std::string asp_name = asp_ptr->getName();
			std::string m_ref = asp_ptr->askMetaRef("/" + model_name);

			mmm += indStr() + "<aspect name = \"" + asp_name + "\" metaref = \"" + m_ref + "\"";
			mmm += dumped_attr_list;
			mmm +=" >\n";

			++ind;

			std::string asp_disp = asp_ptr->getDispName();
			if (!asp_disp.empty() && asp_disp != asp_name) // if not empty and not the same
				mmm += asp_ptr->dumpDispName();

			std::vector< std::string> part_lines_to_dump;

			const AspectRep::PartRepSeries parts = (*asp_it)->getPartSeries( this);
			AspectRep::PartRepSeries_ConstIterator part_it = parts.begin();
			for( ; part_it != parts.end(); ++part_it)
			{
				std::string r_name = part_it->getRoleRepPtr()->getSmartRoleName();

				// get values from the registry
				std::string regPath, regVal1, regVal2;

				// inquiring the registry for the primary asp value
				regPath = "/PrimaryAspects/" + model_name + ":" + r_name + "/" + asp_name;
				//BON::FCO bon_ptr = part_it->getFCOPtr()->getPtr();
				//regVal1 = bon_ptr->getRegistry()->getValueByPath(regPath);
				regVal1 = part_it->getFCOPtr()->getMyRegistry()->getValueByPath(regPath);
				
				std::string m_ref = part_it->getFCOPtr()->askMetaRef("/" + model_name + '/' + r_name + '/' + asp_name);

				//if ( res ) part_it->setPrimary( regVal1 == "yes");
				
				bool is_model = ( part_it->getFCOPtr()->getMyKind() == Any::MODEL);

				bool is_ref_to_model = false;
				if ( part_it->getFCOPtr()->getMyKind() == Any::REF)
				{
					const ReferenceRep * ref_ptr = dynamic_cast<const ReferenceRep *>( part_it->getFCOPtr());
					is_ref_to_model = ref_ptr->pointsToModels();
				}

				std::string is_port_str = part_it->getRoleRepPtr()->isPort()?"yes":"no";
				std::string primary_str = (!regVal1.empty() && regVal1=="no")?"no":"yes";
				std::string kind_aspect_name = "";

				if ( is_model || is_ref_to_model)
				{
					// inquiring the registry for the kind asp value
					regPath = "/KindAspects/" + model_name + ":" + r_name + "/" + asp_name;
					//regVal2 = bon_ptr->getRegistry()->getValueByPath(regPath);
					regVal2 = part_it->getFCOPtr()->getMyRegistry()->getValueByPath(regPath);
					
					if ( regVal2 != "__GME_default_mechanism" && !regVal2.empty() && regVal2.find_first_not_of(' ') != std::string::npos) // trim it
						kind_aspect_name = std::string("\" kindaspect = \"") + regVal2;
#if( _DEBUG)
					else
						global_vars.err << "Empty kindaspect value left out. Model: " << model_name << " Aspect: " << asp_name << " Role: " << r_name << "\n";
#endif
				}
				std::string part_line = "<part metaref = \"" + m_ref + "\" role = \"" + r_name + "\" primary = \"" + primary_str + kind_aspect_name +"\" linked = \"" + is_port_str + "\"></part>";
				
				if ( std::find( part_lines_to_dump.begin(), part_lines_to_dump.end(), part_line) !=
					part_lines_to_dump.end()) // found
				{
					global_vars.err << MSG_ERROR << "Duplicate part found. Not dumping twice: " << part_line << "\n";
					if (!error) 
					{
						TO("Please check the metamodel for duplicate parts.");
						error = true;
					}
				}
				else
					part_lines_to_dump.push_back( part_line);
			}
			
			PartStringLex lex;
			std::sort( part_lines_to_dump.begin(), part_lines_to_dump.end(), lex);
			std::vector<std::string>::iterator part_lines_it = part_lines_to_dump.begin();
			for( ; part_lines_it != part_lines_to_dump.end(); ++part_lines_it)
				mmm += indStr() + *part_lines_it + "\n";

			part_lines_to_dump.clear();
			--ind;
			mmm += indStr() + "</aspect>\n";
		}
	}

	--ind;
	mmm += indStr() + "</model>\n";
	return mmm;
}


bool ModelRep::checkMyAspects( Sheet * s)
{
	bool no = (howManyAspects() >= 1);
	if (!no) 
	{
		if (m_finalRoleMap.size() > 0) {
			std::string name = m_ptr->getName();
			std::vector<std::string> rolenames;
			std::transform(m_finalRoleMap.begin(), m_finalRoleMap.end(), std::back_inserter(rolenames),
				[](const RoleMap::value_type& p){ return p.first->getNamespace() + "::" + p.first->getName(); });
			std::string children;
			std::for_each(m_finalRoleMap.begin(), m_finalRoleMap.end(),
				[&](const RoleMap::value_type& p) { children += p.first->getName() + ", ";  });
			children = children.substr(0, children.size() - 2);
			global_vars.err << MSG_WARNING << "Warning: Model \"" << m_ptr << "\" has no aspect defined and may contain: " + children;
		}
		AspectRep * asp = s->createAspectRep( BON::FCO(), BON::FCO());
		this->addFinalAspect( asp);
		no = true;
	}
	return no;
}


void ModelRep::createConstraints( Sheet * s)
{
	RoleMap_ConstIterator it = m_initialRoleMap.begin();
	for ( ; it != m_initialRoleMap.end(); ++it)
	{
		FCO * const ptr = it->first;
		
		std::vector<FCO*> descendants;
		ptr->getIntDescendants( descendants);
		
		RoleSeries_ConstIterator jt = it->second.begin();
		for( ; jt != it->second.end(); ++jt)
		//if (!jt->getFCOPtr()->isAbstract())
		{
			const RoleRep & r = *jt;
			std::string card = jt->getCardinality();
			{
				std::string str_expr_end;
				std::string str_card_context;
				str_card_context = "[containment] In model: " + r.getModelRepPtr()->getName() + ", Role: " + r.getSmartRoleName();
				bool valid_constr = ! Dumper::doParseCardinality( card, "partCount", str_card_context, str_expr_end);
				if ( ! valid_constr )
				{
					global_vars.err << MSG_ERROR << "Ignoring invalid cardinality string in role: " << r.getSmartRoleName() << ". String: " << card << ".\n";
				}

				//CBuilderConnection* pContainment = pRole->conn;

				// Build the Beginning of the Expression

				std::string str_expr_begin = 
					"let partCount = self.parts( \"" + r.getSmartRoleName() + "\" ) -> size ";

				std::vector<FCO*>::iterator desc_it = descendants.begin();
				for( ; desc_it != descendants.end(); ++desc_it)
					if ( !(*desc_it)->isAbstract())
						str_expr_begin += "+ self.parts( \"" + (*desc_it)->getName() + r.getOnlyRoleName() + "\" ) -> size ";

				str_expr_begin += "in\n                     ";

				// If Cardinality was appropriate

				if ( valid_constr && ! str_expr_end.empty() ) {

					// Build Name, EventMask, Description

					int id = Broker::getNextConstraintId();
					char str_id[64];
					sprintf( str_id, "%d", id);

					std::string str_cons_name;
					str_cons_name = "Valid" + getName() + "PartCardinality" + std::string(str_id);
					std::string::size_type pos = str_cons_name.find( "::");
					if( pos != std::string::npos) 
						str_cons_name.replace( pos, 2, 2, '_');

					int iEventMask = 0;
					char chMask[64];
					sprintf( chMask, "%x", iEventMask );

					std::string str_desc;
					str_desc = "Multiplicity of parts, which are contained by " 
						+ getName() +	", has to match "+ card + ".";

					ConstraintRep * cr = s->createConstraintRep( BON::FCO());
					std::string s_b = str_expr_begin + str_expr_end;
					cr->init( str_cons_name, /*mask:*/global_vars.genConstr.reg_cont_mask, "1", global_vars.genConstr.priority, s_b, str_desc);

					this->addInitialConstraintRep( cr); // <!> to be dec'd whether initial or final
					cr->attachedTo();
				}
			}
		}
	}
}
