#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "ReferenceRep.h"
#include "ModelRep.h"

#include "algorithm"

#include "globals.h"
extern Globals global_vars;


ReferenceRep::ReferenceRep( BON::FCO& ptr, BON::FCO& resp_ptr)
	: FCO( ptr, resp_ptr)
	, m_initialReferees()
	, m_finalReferees()
	, m_allReferees()
{
}


ReferenceRep::~ReferenceRep() 
{ 
	m_initialReferees.clear();
	m_finalReferees.clear();
	m_allReferees.clear();
}


void ReferenceRep::addInitialReferee( FCO * refd ) 
{	
	if ( std::find( m_initialReferees.begin(), m_initialReferees.end(), refd) 
		== m_initialReferees.end()) // not found
		m_initialReferees.push_back( refd );
	else
		global_vars.err << MSG_WARNING << "Warning: Reference \"" << m_ptr << "\" referring twice to fco: \"" << refd->getPtr() << "\". Disregarded.\n";
}


FCO * ReferenceRep::getInitialReferee() const
{
	if (checkNotEmpty()) return m_initialReferees[0];
	else return 0; // or an exception
}


const ReferenceRep::RefereeList& ReferenceRep::getInitialReferees() const
{
	return m_initialReferees;
}


const ReferenceRep::RefereeList& ReferenceRep::getFinalReferees() const
{
	return m_finalReferees;
}

/**
 * Returns true if the reference refers to a Model (a non abstract model) beside all other kinds like Atom, FCO, Reference, Set
 * The permissive version
 */
bool ReferenceRep::pointsToModels() const
{
	bool to_models = false;

	for( unsigned int i = 0; !to_models && i < m_allReferees.size(); ++i)
	{
		// consider the non-abstracts models only (if a reference points to an abstract model, that doesn't mean too much)
		if ( !m_allReferees[i]->isAbstract())
		{
			to_models = to_models || (m_allReferees[i]->getMyKind() == Any::MODEL);
		}
	}

	return to_models;
}

/*****    deprecated version, see Emre's bug, non-homogeneous references  ************
// The strict version
//Return true if the reference refers to a Model (a non abstract model) beside other kinds like FCO or Reference
//-----------------------------------------------------------------------------------------------
//Since the inheritance is allowed to be homogenous the result is not depending 
//if we checked the m_initialRefereeList or m_finalRefereeList
bool ReferenceRep::pointsToModels() const
{
	int how_many_models = 0;//t
	std::string which_models;//t
	bool value_set = false;
	bool to_models = true;
	KIND_TYPE kind;

	for( unsigned int i = 0; i < m_allReferees.size(); ++i)
	{
		kind = m_allReferees[i]->getMyKind();
		if( kind != Any::FCO_REP && kind != Any::REF)
		{
			// consider the non-abstracts models only
			if ( !m_allReferees[i]->isAbstract())
			{
				to_models = to_models && (kind == Any::MODEL);

				value_set = true;

				if (kind == Any::MODEL)//t
				{
					how_many_models += 1;
					which_models += m_allReferees[i]->getName() + " ";
				}
			}
		}
	}

	if (value_set) return to_models;
	else return false;
}*/



/*
Should return vector<ModelRep *> if pointsToModels is true
Returns non abstract models only !!! (see ESML paradigm)
*/
std::vector<const ModelRep *> ReferenceRep::getModelRefVector() const
{
	std::vector<const ModelRep *> models;
	
	for( unsigned int i = 0; i < m_allReferees.size(); ++i) 
	{
		if ( m_allReferees[i]->getMyKind() == Any::MODEL && !m_allReferees[i]->isAbstract())
		{
			const ModelRep * c_m = dynamic_cast<const ModelRep*>( m_allReferees[i]);
			if ( std::find( models.begin(), models.end(), c_m) == models.end())
				models.push_back( c_m);
		}
	}

	return models;
}


/* -- obsolete
Should return a ModelRep * if pointsToModels is true
Returns non abstract models only !!! (see ESML paradigm)
*/
// see getModelRefVector() too
/*const ModelRep * ReferenceRep::getModelRef() const
{
	unsigned int i = 0;
	bool conti = true;
	while( i < m_allReferees.size() && conti) 
	{
		if ( m_allReferees[i]->getMyKind() == Any::MODEL && !m_allReferees[i]->isAbstract())
			conti = false;
		else
			++i;
	}

	if ( i == m_allReferees.size()) // not found any model, how come?
		return 0;
	else 
	{
		unsigned int j = i+1; // look for other models 
		while( j < m_allReferees.size() && m_allReferees[j]->getMyKind() != Any::MODEL)
			++j;
		if ( j != m_allReferees.size()) // found another one
		{
			//global_vars.err << "Notice: reference \"" << getName() << "\" referring to many models. Considering only the first: " << m_allReferees[i]->getName() << "\n";
			//<!> this would have strange effects in case of aspect mapping -> gathered all
			// aspects of the different models ...
		}

		return dynamic_cast<const ModelRep*>( m_allReferees[i]);
	}
}*/


void ReferenceRep::addFinalReferees( FCO * referee)
{
	// insert into m_finalReferees if not present already
	RefereeList_Iterator l_it = std::find( m_finalReferees.begin(), m_finalReferees.end(), referee);

	if ( l_it == m_finalReferees.end()) // not found
		m_finalReferees.push_back( referee);
	else { } // can happen if multiple inheritance is used

	referee->addFinalRefersToMe( this);
}


void ReferenceRep::addFinalReferees( RefereeList & referees )
{
	RefereeList_Iterator referee_it = referees.begin();
	for( ; referee_it != referees.end(); ++referee_it)
	{
		addFinalReferees( *referee_it);
		(*referee_it)->addFinalRefersToMe( this);
	}
}


void ReferenceRep::inherit()
{
	std::vector<FCO*> refnce_descendants;
	this->getImpDescendants( refnce_descendants);
	refnce_descendants.push_back( this);

	RefereeList_Iterator it = m_initialReferees.begin();
	for( ; it != m_initialReferees.end(); ++it)
	{
		FCO * target_ptr = *it;

		std::vector<FCO*> refree_descendants;

		// inquiring the refree_descendants who have the same Interface
		target_ptr->getIntDescendants( refree_descendants);
		//refree_descendants.push_back( target_ptr); see below
		std::vector<FCO*>::reverse_iterator refnce_it = refnce_descendants.rbegin();
		for( ; refnce_it != refnce_descendants.rend(); ++refnce_it) // for all impl desc of "this"
		{
			if ( (*refnce_it)->getMyKind() != Any::REF)
				global_vars.err << "Non-Reference descendant: " << (*refnce_it)->getPtr() <<" of reference: " << m_ptr <<"\n";
			else
			{
				ReferenceRep* one_refnce = dynamic_cast<ReferenceRep *>(*refnce_it);
				one_refnce->addFinalReferees( target_ptr); // put at first place the original referee
				one_refnce->addFinalReferees( refree_descendants); // adding all the interface desc of target_ptr
			}
		}
	}
}


bool ReferenceRep::finalize()
{
	bool value_set = false;
	bool same_kind = true;
	const ModelRep * mod_ptr = 0;
	KIND_TYPE kind;

	std::vector<FCO*> all_referees( m_finalReferees);

	unsigned int current = 0;
	while( !all_referees.empty() && current < all_referees.size())
	{
		if ( all_referees[current]->getMyKind() != Any::FCO_REP && 
			all_referees[current]->getMyKind() != Any::REF)
		{
			if (!value_set) 
			{
				kind = all_referees[current]->getMyKind();
				value_set = true;
			}
			else // value set already
				same_kind = same_kind && (all_referees[current]->getMyKind() == kind);
		}
		else if ( all_referees[current]->getMyKind() == Any::REF)
		{
			ReferenceRep * r = dynamic_cast<ReferenceRep *>( all_referees[current]);
			// get all referees
			const RefereeList &list = r->getFinalReferees();
			// copy if not present already into all_referees
			RefereeList_ConstIterator list_iter = list.begin();
			for( ; list_iter != list.end(); ++list_iter)
				if (std::find( all_referees.begin(), all_referees.end(), *list_iter) == all_referees.end())
					all_referees.push_back( *list_iter);
		}
		++current;
	}
	
	m_allReferees = all_referees;
	
	return same_kind;
}


std::string ReferenceRep::doDump() 
{ 
	std::string m_ref = askMetaRef();	

	std::string mmm = indStr() + "<reference name = \"" + getName()  + "\" metaref = \"" + m_ref + "\"";

	mmm += dumpAttributeList();
	mmm +=">\n";
	++ind;
	mmm += dumpDispName();
	++ind;
	mmm += dumpNamePosition();
	mmm += dumpShowPorts();
	mmm += dumpGeneralPref();
	--ind;
	mmm += dumpConstraints();
	mmm += dumpAttributes();

	mmm += dumpIcon();
	mmm += dumpPortIcon();
	mmm += dumpDecorator();
	mmm += dumpHotspotEnabled();
	mmm += dumpTypeShown(); // previously didn't needed, but let's try <!>
	mmm += dumpSubTypeIcon();
	mmm += dumpInstanceIcon();
	mmm += dumpNameWrap();
	mmm += dumpNameEnabled();
	mmm += dumpResizable();
	mmm += dumpAutoRouterPref();
	mmm += dumpHelpURL();
	// 	mmm += dumpTypeDisplayed() == dumpTypeShown() not needed

	mmm += dumpGradientFillEnabled();
	mmm += dumpGradientFillColor();
	mmm += dumpGradientFillDirection();
	mmm += dumpShadowCastEnabled();
	mmm += dumpShadowColor();
	mmm += dumpShadowThickness();
	mmm += dumpShadowDirection();
	mmm += dumpRoundRectangleEnabled();
	mmm += dumpRoundRectangleRadius();

	mmm += indStr() + "<pointerspec name = \"ref\">\n";
	++ind;
	
	// sort them lexicographically
	AnyLexicographicSort lex;
	std::sort( m_finalReferees.begin(), m_finalReferees.end(), lex);
	RefereeList_Iterator it = m_finalReferees.begin();
	for( ; it != m_finalReferees.end(); ++it )
	{
		if (!(*it)->isAbstract())
			mmm += indStr() + "<pointeritem desc = \"" + (*it)->getName() + "\"></pointeritem>\n";
	}

	// end for
	--ind;
	mmm += indStr() + "</pointerspec>\n";
	--ind;
	mmm += indStr() + "</reference>\n";
	
	return mmm;
}

std::string ReferenceRep::dumpShowPorts() const
{
	std::string mmm = "";
	if(!m_showPorts)
			mmm += indStr() + "<regnode name = \"showPorts\" value =\"false\"></regnode>\n";
	return mmm;
}

bool ReferenceRep::checkNotEmpty() const
{
	return !m_allReferees.empty();
}

/*
// obsolete // removed upon user request 2/25/2004
bool ReferenceRep::checkAllTheSameKind() const
{
	if ( !checkNotEmpty()) return false;
	if ( m_finalReferees.empty()) return false;
	if ( m_allReferees.empty()) return false;
	
	bool all_the_same_kind = true;

	bool value_set = false;
	bool same_kind = true;
	KIND_TYPE kind;
	for( unsigned int i = 0; i < m_allReferees.size(); ++i)
	{
		KIND_TYPE kind_of_i = m_allReferees[i]->getMyKind();
		if( kind_of_i != Any::FCO_REP && kind_of_i != Any::REF)
		{
			if (!value_set) 
			{
				kind = kind_of_i;
				value_set = true;
			}
			else // value set already
			{
				same_kind = same_kind && ( kind_of_i == kind);
				if (kind_of_i != kind)
				{
					// removed upon user request 2/25/2004
					global_vars.err << "CHECK: Reference \"" << getName() << "\" refers to too many kinds: " <<
					Any::KIND_TYPE_STR[kind] << " and " << Any::KIND_TYPE_STR[kind_of_i] << "\n";
				}
			}
		}
	}
	if (value_set) return same_kind;
	else return false;
}*/


int ReferenceRep::howManyAspectsAmongModelRefs() const
{
	ModelRep::AspectRepPtrList intersection_of_asps = getAspectsIntersection();
	return (int) intersection_of_asps.size(); 
}


AspectRep * ReferenceRep::getFirstAspectFromIntersection() const
{
	ModelRep::AspectRepPtrList intersection_of_asps = getAspectsIntersection();

	AspectRep * lowest_asp = 0;

	ModelRep::AspectRepPtrList::iterator asp_it = intersection_of_asps.begin();
	for( ; asp_it != intersection_of_asps.end(); ++asp_it)
	{
		if ( !lowest_asp) // is lowest_asp set?
			lowest_asp = *asp_it;
		else if ( !lowest_asp->lessThan( **asp_it)) // compare
			lowest_asp = *asp_it;
	}

	return lowest_asp;
}


std::vector<AspectRep *> ReferenceRep::getAspectsIntersection() const
{
	ModelRep::AspectRepPtrList intersection_of_asps;
	bool inited = false;
	std::vector<const ModelRep *> models = getModelRefVector();
	std::vector<const ModelRep *>::iterator it = models.begin();
	for( ; it != models.end(); ++it)
	{
		const ModelRep::AspectRepPtrList &asplist = (*it)->getFinalAspectRepPtrList();
		if ( !inited)
		{
			intersection_of_asps = asplist; // set the first vector as an initial value of the union
			inited = true;
		}
		else
		{
			ModelRep::AspectRepPtrList::iterator isect_it = intersection_of_asps.begin();
			while( isect_it != intersection_of_asps.end())
			{
				if ( std::find( asplist.begin(), asplist.end(), *isect_it) == asplist.end()) // element of union not found in asp set, so it should be deleted from the union
					isect_it = intersection_of_asps.erase( isect_it); // this moves the iterator ahead
				else
				 ++isect_it;
			}
		}
	}
	return intersection_of_asps;
}



