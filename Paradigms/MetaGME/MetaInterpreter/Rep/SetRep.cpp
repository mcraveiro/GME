#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "SetRep.h"
#include "logger.h"

#include "algorithm"

#include "globals.h"
extern Globals global_vars;

SetRep::SetRep( BON::FCO& ptr, BON::FCO& resp_ptr)
	: FCO( ptr, resp_ptr)
{ 
	m_memberList.clear();
	m_memberMap.clear();
}


SetRep::~SetRep()
{ 
	m_memberList.clear();
	m_memberMap.clear();
}


void SetRep::addMember( FCO * member) 
{ 
	SetMemberList_ConstIterator it = 
		std::find( m_memberList.begin(), m_memberList.end(), member);

	if (it == m_memberList.end()) // not found
		m_memberList.push_back( member); 
	else
		global_vars.err << MSG_ERROR << "Set member " << member->getPtr() << " is twice in set " << m_ptr << "\n";
} 


// adds new role to the potential lists of set elements in model!!!
void SetRep::addSetMemberRole( const ModelRep * model, const RoleRep& role)
{ 
	PointerItemSeries & series = m_memberMap[model];
	PointerItem item = role.getSmartRoleName();
	
	PointerItemSeries_Iterator it = std::find( series.begin(), series.end() ,item);

	//global_vars.err << "Role \"" << role.getSmartRoleName() << "\" in set \"" << getName() << ":" << model->getName() <<"\"\n";
	if ( it == series.end() ) // not found
		series.push_back( item);
	else 
		{ /*global_vars.err << "Notice: found duplicate role \"" << role.getSmartRoleName() << "\" in set \"" << getName() << ":" << model->getName() << "\"\n";*/ }
}


// adds new roles to the potential lists of set elements in model!!!
void SetRep::addSetMemberRole( const ModelRep * model, const RoleRep::RoleRepSeries & role_series)
{ 
	RoleRep::RoleRepSeries_ConstIterator role_it = role_series.begin();
	for( ; role_it != role_series.end(); ++role_it)
		addSetMemberRole( model, *role_it);
}


//
// SetRep::createElements
//
// expands the set and connection targets with roleNames like below as (elementNames)
//
// i.e.:      S
//          / | \
//        /   |   \
//      /    < >    \
//    M1-r1---A---r2-M2
//    M1-R1--/ \--R2-M2
//
// in this case S has at first A as a member, but after the completion of this task
// S will have elements like r1, R1 (in M1), r2, R2 ( in M2)
void SetRep::inherit()
{
	std::vector<FCO*> set_descendants;
	this->getImpDescendants( set_descendants);
	set_descendants.push_back( this);
	//global_vars.err << "Inheriting from set: "<< getName() << "\n";

	// this set is contained by the following models
	ModelRepPtrList models = this->modelsIAmPartOfFinal();
	ModelRepPtrList_Iterator mod_it = models.begin();
	// for all models that contain this set
	for( ; mod_it != models.end(); ++mod_it )
	{
		ModelRep* mod_ptr = *mod_it; 
		// we don't have to care with the impl inh between models since all model related
		// inheritances are taken into account
		//global_vars.err << "In model " << mod_ptr->getName() << "\n";

		// for all members of the set
		SetMemberList_ConstIterator member_it = m_memberList.begin();
		for( ; member_it != m_memberList.end(); ++member_it)
		{
			FCO * member_ptr = *member_it;
			std::vector<FCO*> children;
		
			//global_vars.err << "For all desc's of " << member_ptr->getName() << "\n";

			member_ptr->getIntDescendants( children);
			children.push_back( member_ptr);

			// for all int desc of current set member
			std::vector<FCO*>::reverse_iterator child_it = children.rbegin();
			for( ; child_it != children.rend(); ++child_it)
			{
				FCO * child_member_ptr = * child_it;
				if ( !child_member_ptr->isAbstract())
				{
					RoleRep::RoleRepSeries series;
					bool has_some = mod_ptr->getFinalRoles( child_member_ptr, series);

					if ( has_some && !series.empty())
					{
						std::vector<FCO*>::reverse_iterator sets_iterator = set_descendants.rbegin();
						for( ; sets_iterator != set_descendants.rend(); ++sets_iterator )
						{
							if ( (*sets_iterator)->getMyKind() != Any::SET) 
								global_vars.err << MSG_ERROR << "Non-set descendant: " << (*sets_iterator)->getPtr() <<" of set: " << m_ptr <<"\n";
							else
							{
								SetRep* one_set = dynamic_cast<SetRep *>(*sets_iterator);
								std::string set_nm = one_set->getName();
								// add the roles to the member list of the set ( in that model) 
								one_set->addSetMemberRole( mod_ptr, series);
							}
						}
					}
				}
			}
		}
	}
}


std::string SetRep::doDump()
{
	std::string m_ref = askMetaRef();

	std::string mmm = indStr() + "<set name = \"" + getName()  + "\" metaref = \"" + m_ref + "\"";

	mmm += dumpAttributeList();

	mmm +=" >\n";
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
	mmm += indStr() + "<pointerspec name = \"set\">\n";
	++ind;
	
	
	PointerItemSeries dumper_list;
	
	ModelRepPtrList models = this->modelsIAmPartOfFinal();
	ModelRepPtrList_ConstIterator mod_it = models.begin();

	// for all models that contain this set
	for( ; mod_it != models.end(); ++mod_it )
	{
		ModelRep * mod_ptr = *mod_it;

		// for all members in this model
		PointerItemSeries_Iterator part_it = m_memberMap[ mod_ptr].begin();
		for( ; part_it != m_memberMap[ mod_ptr].end(); ++part_it)
		{
			// insert into dumper_list if not present already
			if ( std::find( dumper_list.begin(), dumper_list.end(), *part_it) ==
				dumper_list.end()) // not found
				dumper_list.push_back( *part_it);
			else
			{	/*global_vars.err << "Warning: set element \"" << part_it->name() << "\" present twice in set \"" << getName() << "\"\n";*/ }
		}
	}	// end for

	PointerItemLex lex;
	std::sort( dumper_list.begin(), dumper_list.end(), lex);
	PointerItemSeries_Iterator dump_it = dumper_list.begin();
	for( ; dump_it != dumper_list.end(); ++dump_it)
		mmm += indStr() + "<pointeritem desc = \"" + dump_it->name() + "\"></pointeritem>\n";

	--ind;
	mmm += indStr() + "</pointerspec>\n";
	--ind;
	mmm += indStr() + "</set>\n";
	
	return mmm;
}


bool SetRep::checkSetElements()
{
	bool res = true;
	const int number_of_lists = 1;
	const SetMemberList* lists[ number_of_lists] = {
		&m_memberList
	};

	for( int i = 0; i < number_of_lists; ++i)
	{
		SetMemberList_ConstIterator member_it = lists[i]->begin();
		// for all members of the set
		for( ; member_it != lists[i]->end(); ++member_it)
		{
			FCO * member_ptr = *member_it;

			if ( !member_ptr->checkIsPartOfFinal() && !member_ptr->isAbstract())
			{
				global_vars.err << MSG_ERROR << "CHECK: set member " << member_ptr->getPtr() << " in set \"" << m_ptr << "\" is not contained by any model.\n";
				res = false;
			}
		}
	}
	return res;
}

