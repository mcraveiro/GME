#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "ReferenceRep.h"
#include "ModelRep.h"
#include "Dumper.h"
#include "CodeGen.h"

#include "logger.h"
#include "algorithm"

#include "globals.h"
extern Globals global_vars;
extern int h_ind;

ModelRep::ModelRep( BON::FCO& ptr, BON::FCO& resp_ptr)
	: FCO( ptr, resp_ptr)
	, m_initialRoleMap()
	, m_finalRoleMap()
	, m_methods()
{ 
}


ModelRep::~ModelRep()
{
	m_initialRoleMap.clear();
	m_finalRoleMap.clear();
	m_methods.clear();
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
		global_vars.err << "Error: Found duplicate role \"" << role.getSmartRoleName() << "\" in model \"" << getName() << "\"\n";
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
		global_vars.err << "Error: Found duplicate final role \"" << role.getSmartRoleName() << "\" in model \"" << getName() << "\"\n";
}

bool ModelRep::getFinalRoles( const FCO * ptr, RoleMapValue& map_val) const
{
	RoleMap_ConstIterator it = m_finalRoleMap.begin();
	while ( it != m_finalRoleMap.end() && it->first != ptr)
		++it;
	if ( it == m_finalRoleMap.end())
		return false;
	map_val = it->second;
	return true;
	/*RoleMap_ConstIterator it = m_finalRoleMap.find( ptr);
	if ( it == m_finalRoleMap.end())
		return false;

	map_val = it->second;
	return true;*/
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
	//temp
	std::string m_name;
	std::string f_name;
	//temp
	
	RoleMap temp_map;
	std::vector<FCO*> model_descendants;
	this->getImpDescendants( model_descendants); // needed because of complexity
	model_descendants.push_back( this);

	// for each model implementation descendant
	std::vector<FCO*>::reverse_iterator model_it = model_descendants.rbegin();
	for( ; model_it != model_descendants.rend(); ++model_it)
	{
		if ( (*model_it)->getMyKind() != Any::MODEL)
			global_vars.err << std::string("ERROR: model inherit\n");
		ModelRep * mod_desc_ptr = dynamic_cast<ModelRep*>(*model_it);
		if (!mod_desc_ptr) global_vars.err << std::string("NULL PTR ERROR: model inherit\n");
		m_name = mod_desc_ptr->getName();
					
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
					f_name = (*desc_it)->getName();

					// the descendants must have a separate role for each initial role
					// this role is inherited and has long form depending on the hierarchy_flag
					//TO( role_it->getOnlyRoleName());
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
					//TO( bon_ptr->GetName() + " < " + r.getRoleName().c_str() + " > " + bon_m_ptr->GetName());
				}
			}
		}
	}
}


bool ModelRep::check()
{
	return true;
}


/*
used when the long form is NOT needed for role
*/
/*static*/ std::string ModelRep::roleGetterMethodName2(FCO * fco, RoleRep* role, bool use_fco_name, const std::string& diff_nmsp)
{
#if(LONG_NAMES)
	return "get_Role_" + (role->getOnlyRoleName().empty()?fco->getValidName():role->getOnlyRoleName());
#else
	return "get" + diff_nmsp + (role->getOnlyRoleName().empty()?fco->getValidName():role->getOnlyRoleName());
#endif
}


/*
used when the long form is needed for role
*/
/*static*/ std::string ModelRep::roleGetterMethodName3(FCO * fco, RoleRep* role, bool use_fco_name, const std::string& diff_nmsp)
{
#if(LONG_NAMES)
	return "get_Role_" + (role->getOnlyRoleName().empty()?fco->getValidName():fco->getValidName() + role->getOnlyRoleName());
#else
	return "get" + diff_nmsp + (role->getOnlyRoleName().empty()?fco->getValidName():fco->getValidName() + role->getOnlyRoleName());
#endif
}


void ModelRep::createMethods()
{
	RoleMap_Iterator r_it = m_initialRoleMap.begin();
	while ( r_it != m_initialRoleMap.end())
	{
		RoleMapKey ptr = r_it->first;
		RoleMapValue &roles = r_it->second;
		RoleSeries_Iterator jt = roles.begin();
		for( ; jt != roles.end() ; ++jt)
		{
			CodeGen::dumpRoleGetter( ptr, &*jt, this);
		}
		++r_it;
	}
}


std::string ModelRep::doDump()
{
	std::string h_file, c_file;

	dumpPre( h_file, c_file);
	dumpFCO( h_file, c_file);

	if ( !m_methods.empty())
		h_file += CodeGen::indent(1) + "//\n" + CodeGen::indent(1) + "// kind and role getters\n";

	MethodLexicographicSort lex;
	std::sort( m_methods.begin(), m_methods.end(), lex);
	
	std::vector<Method>::iterator i = m_methods.begin();
	for( ; i != m_methods.end(); ++i)
	{
		if ( !i->m_template)
		{
			h_file += i->getHeader() + "\n";
			c_file += i->getSource() + "";
		}
		else
			h_file += i->getHeader() + "\n";
	}

	h_file += hideAndExpose();

	dumpPost( h_file, c_file);

	sendOutH( h_file);//DMP_H( h_file);
	sendOutS( c_file);//DMP_S( c_file);

	return "";
}

std::string ModelRep::expose( const std::string& repl_container)
{
	std::string h_file;
	h_file += FCO::expose( repl_container);

	if (!m_methods.empty())
		h_file += CodeGen::indent(h_ind) + "//\n" + CodeGen::indent(h_ind) + "// exposed kind and role getters\n";
	std::vector<Method>::iterator i = m_methods.begin();
	for( ; i != m_methods.end(); ++i)
		h_file += i->getExposed( repl_container) + "\n";

	return h_file;
}


std::string ModelRep::hide()
{
	std::string h_file;
	h_file += FCO::hide();

	if (!m_methods.empty())
		h_file += CodeGen::indent(h_ind) + "//\n" + CodeGen::indent(h_ind) + "// hidden kind and role getters\n";
	std::vector<Method>::iterator i = m_methods.begin();
	for( ; i != m_methods.end(); ++i)
		h_file += i->getHidden() + "\n";

	return h_file;
}

