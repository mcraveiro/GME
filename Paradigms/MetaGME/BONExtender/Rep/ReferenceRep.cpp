#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "ReferenceRep.h"
#include "ModelRep.h"
#include "CodeGen.h"


#include "algorithm"

#include "globals.h"
extern Globals global_vars;
extern int h_ind;

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
		global_vars.err << "Warning: Reference \"" << getName() << "\" referring twice to fco: \"" << refd->getName() << "\". Disregarded.\n";
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

/*
Return true if the reference refers to a Model (a non abstract model) beside other kinds like FCO or Reference
-----------------------------------------------------------------------------------------------
Since the inheritance is allowed to be homogenous the result is not depending 
if we checked the m_initialRefereeList or m_finalRefereeList
*/
bool ReferenceRep::pointsToModels() const
{
	int how_many_models = 0;
	std::string which_models;
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

				if (kind == Any::MODEL)
				{
					how_many_models += 1;
					which_models += m_allReferees[i]->getName() + " ";
				}
			}
		}
	}

	if (value_set) return to_models;
	else return false;
}


/*
Should return vector<ModelRep *> if pointToModels is true
Returns non abstract models only !!! (see ESML paradigm)
*/
std::vector<const ModelRep *> ReferenceRep::getModelRefVector() const
{
	std::vector<const ModelRep *> models;
	unsigned int i = 0;
	while( i < m_allReferees.size()) 
	{
		if ( m_allReferees[i]->getMyKind() == Any::MODEL && !m_allReferees[i]->isAbstract())
		{
			const ModelRep * c_m = dynamic_cast<const ModelRep*>( m_allReferees[i]);
			if ( std::find( models.begin(), models.end(), c_m) == models.end())
				models.push_back( c_m);
		}
		++i;
	}

	return models;
}


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
	this->getImpDescendants( refnce_descendants); //implementation inheritance needed because the common ancestor of all referees is to be returned
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
				global_vars.err << "Non-Reference descendant: " << (*refnce_it)->getName() <<" of reference: " << getName() <<"\n";
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


void ReferenceRep::createMethods() 
{ 
	if ( m_initialReferees.empty()) return; // in such cases no specific getter is added, its base (if any) provides anyway the needed methods

	std::string getter_src, getter_hdr;
	std::vector<FCO*> common_anc = FCO::lcdIntersect( m_finalReferees); // the reference may point to all of these fcos
	if ( !common_anc.empty())
	{
		std::vector<FCO*>::iterator c_it = common_anc.begin();
		for( ; c_it != common_anc.end(); ++c_it)
			m_refGetterMethods.push_back( CodeGen::dumpRefGetter( this, *c_it, ""));
	}
	else if ( common_anc.empty()) // empty, use BON::FCO or common_kind
	{
		std::string common_kind = FCO::lcdKindIntersect( m_finalReferees);
  		m_refGetterMethods.push_back( CodeGen::dumpRefGetter( this, 0, common_kind));
	}
	else
	{
		// the reference doesn't have any referee so it doesn't deserve any getter
		// if inherits something from its base class (if any) the OK
		// otherwise ?
	}

	/*RefereeList_Iterator it = m_allReferees.begin();//m_finalReferees.begin();//m_initialReferees.begin();
	for( ; it != m_allReferees.end(); ++it )
	{
		m_refGetterMethods.push_back( ReferenceRep::dumpRefGetter( this, *it, ""));
	}*/
}


std::string ReferenceRep::doDump() 
{ 
	std::string h_file, c_file;

	dumpPre( h_file, c_file);
	dumpFCO( h_file, c_file);

	if ( !m_refGetterMethods.empty())
		h_file += CodeGen::indent(1) + "//\n" + CodeGen::indent(1) + "// ref getters\n";

	MethodLexicographicSort lex;
	std::sort( m_refGetterMethods.begin(), m_refGetterMethods.end(), lex);

	std::vector<Method>::iterator i = m_refGetterMethods.begin();
	for( ; i != m_refGetterMethods.end(); ++i)
	{
		h_file += i->getHeader() + "\n";
		c_file += i->getSource() + "";
	}

	h_file += hideAndExpose();

	dumpPost( h_file, c_file);

	sendOutH( h_file);//DMP_H( h_file);
	sendOutS( c_file);//DMP_S( c_file);

	return "";
}


/*
It checks the m_initialReferees since the descendants of m_initialReferees will be exactly 
as diverse as the m_initialReferees
*/
bool ReferenceRep::checkNotEmpty() const
{
	return !m_initialReferees.empty();
}

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
					// removed on 2/25/2004
					/*global_vars.err << "CHECK: Reference \"" << getName() << "\" refers to too many kinds: " <<
					Any::KIND_TYPE_STR[kind] << " and " << Any::KIND_TYPE_STR[kind_of_i] << "\n";*/
				}
			}
		}
	}
	if (value_set) return same_kind;
	else return false;
}


std::string ReferenceRep::refGetterTemplate( FCO * fco)
{
#if(LONG_NAMES)
	if (fco)
		return "get" + fco->getValidName() + "Referred";
	else
		return "getReferred";
#else
	if (fco)
	{
		bool same_nmsp = fco->getValidNmspc() == getValidNmspc();
		return "get" + ( same_nmsp ? fco->getValidName() : ( fco->getValidNmspc() + fco->getValidName()));
	}
	else
		return "getReferred";
#endif
}


std::string ReferenceRep::expose( const std::string& repl_container)
{ 
	std::string h_file;
	h_file += FCO::expose( repl_container);

	if (!m_refGetterMethods.empty())
		h_file += CodeGen::indent(h_ind) + "//\n" + CodeGen::indent(h_ind) + "// exposed ref getters\n";
	std::vector<Method>::iterator i = m_refGetterMethods.begin();
	for( ; i != m_refGetterMethods.end(); ++i)
	{
		h_file += i->getExposed( repl_container) + "\n";
	}

	return h_file;
}


std::string ReferenceRep::hide() 
{ 
	std::string h_file;
	h_file += FCO::hide();
	
	if (!m_refGetterMethods.empty())
		h_file += CodeGen::indent(h_ind) + "//\n" + CodeGen::indent(h_ind) + "// hidden ref getters\n";
	std::vector<Method>::iterator i = m_refGetterMethods.begin();
	for( ; i != m_refGetterMethods.end(); ++i)
	{
		h_file += i->getHidden() + "\n";
	}

	return h_file;
}


