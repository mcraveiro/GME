#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"


#include "ConnJoint.h"
#include "ConnectionRep.h"
#include "logger.h"
#include "ModelRep.h"
#include "ReferenceRep.h"
#include "Dumper.h"
#include "CodeGen.h"

#include "algorithm"

#include "globals.h"
extern Globals global_vars;
extern int ind;

std::string ConnJoint::m_srcLabel = "src";
std::string ConnJoint::m_dstLabel = "dst";


ConnJoint::ConnJoint
	(	ConnectionRep * ptr,
		const SDList& op1,
		const SDList& op2,
		bool bidirect,
		std::string card1 /*= ""*/, 
		std::string card2 /*= ""*/
	)
	: m_connPtr( ptr)

	, m_oper1( op1)
	, m_oper2( op2)
	
	, m_oper1TargetMap()
	, m_oper2TargetMap()
	
	, m_bidirect( bidirect)
	
	, m_oper1Card( card1)
	, m_oper2Card( card2)
	
	, m_srcLCD()
	, m_dstLCD()
	
	, m_oper1IsAnyReferencePort( false)
	, m_oper2IsAnyReferencePort( false)
{ }


ConnJoint::ConnJoint( const ConnJoint& peer)
	: m_connPtr( peer.m_connPtr)
	
	, m_oper1( peer.m_oper1)
	, m_oper2( peer.m_oper2)
	
	, m_oper1TargetMap( peer.m_oper1TargetMap)
	, m_oper2TargetMap( peer.m_oper2TargetMap)
	
	, m_bidirect( peer.m_bidirect)
	
	, m_oper1Card( peer.m_oper1Card)
	, m_oper2Card( peer.m_oper2Card)
	
	, m_srcLCD( peer.m_srcLCD)
	, m_dstLCD( peer.m_dstLCD)
	
	, m_oper1IsAnyReferencePort( peer.m_oper1IsAnyReferencePort)
	, m_oper2IsAnyReferencePort( peer.m_oper2IsAnyReferencePort)
{ }


const ConnJoint& ConnJoint::operator=( const ConnJoint& peer)
{
	if ( this == &peer ) return *this;
	m_connPtr = peer.m_connPtr;

	m_oper1 = peer.m_oper1;
	m_oper2 = peer.m_oper2;

	m_oper1TargetMap = peer.m_oper1TargetMap;
	m_oper2TargetMap = peer.m_oper2TargetMap;

	m_bidirect = peer.m_bidirect;

	m_oper1Card = peer.m_oper1Card;
	m_oper2Card = peer.m_oper2Card;

	m_srcLCD = peer.m_srcLCD;
	m_dstLCD = peer.m_dstLCD;

	m_oper1IsAnyReferencePort = peer.m_oper1IsAnyReferencePort;
	m_oper2IsAnyReferencePort = peer.m_oper2IsAnyReferencePort;

	return *this;
}


ConnJoint::~ConnJoint() 
{
	m_connPtr = 0;

	m_oper1.clear();
	m_oper2.clear();

	m_oper1TargetMap.clear();
	m_oper2TargetMap.clear();

	m_srcLCD.clear();
	m_dstLCD.clear();
}


// to be used upon copying
void ConnJoint::setConnectionPtr( ConnectionRep * conn_ptr)
{
	m_connPtr = conn_ptr;
}


const ConnJoint::SDList& ConnJoint::getOp1() const { return m_oper1; }
const ConnJoint::SDList& ConnJoint::getOp2() const { return m_oper2; }


bool ConnJoint::isBidirect() const { return m_bidirect; }


void ConnJoint::addTargetItem( int i, const ModelRep * model, const PointerItem & item)
{
	PointerItemSeries * series;
	if (i==0) series = &m_oper1TargetMap[model];
	else series = &m_oper2TargetMap[model];

	if ( std::find( series->begin(), series->end(), item)
		== series->end()) // not found
		series->push_back( item);
	/*else this may happen because of mutiple inheritance
		global_vars.err << "Connection target : " << item.name() << " added twice to connection.\n";*/
}


void ConnJoint::addTargetItem( int i, const ModelRep * model, const RoleRep & new_role)
{ 
	PointerItem item (new_role.getSmartRoleName(), new_role.getFCOPtr());
	addTargetItem( i, model, item);
}


void ConnJoint::addTargetItem( int i, const ModelRep * model, const RoleRep::RoleRepSeries & new_role_series)
{ 
	RoleRep::RoleRepSeries_ConstIterator r_it = new_role_series.begin();
	for( ; r_it != new_role_series.end(); ++r_it)
		addTargetItem( i, model, *r_it);
}

/*    rationale:
                 ___________________________
                |                           |
                <>                          |
     /-------<> M1<>--p-A1---isPort=true--<>M3
    C -------<> M1<>--r-A1---isPort=true--<>M3
		|\-------<> M2<>--x-A1---isPort=true--<>M3
     \-------<> M2<>--z-A1---isPort=false--<>M3
where C connects A1 kind of atoms
and M3 is contained by M1 and M2

this means that p,r,x,z can be extended with M3 p, M3 r, M3 x


	or another example:


    C -------<> M1<>----M4
		|\-------<> M2<>----M5
     \-------<> M3<>----M6
		            <>      <>
								 |     /
								 |   /
								 | /
	               A1
where C connects A1 kind of atoms
if (A1 containment M4..6) relation has the isPort attribute set to true
then the C has to be extended with the M4 A1, M5 A1, M6 A1 targets
*/
void ConnJoint::intInherit( ModelRep * mod_ptr)
{
	std::string con_name = m_connPtr->getName(); //t
	std::string mod_name = mod_ptr->getName(); //t

	const int number_of_endpoints = 2;
	bool reference_ports_also[ number_of_endpoints] = { false, false }; 

	TargetMap * target_map[ number_of_endpoints] = { &m_oper1TargetMap, &m_oper2TargetMap };

	std::vector<ModelRep*> inner_models1 = mod_ptr->getInnerModelsFinal();
	
	std::vector< const ModelRep*> inner_models( inner_models1.size());
	std::copy( inner_models1.begin(), inner_models1.end(), inner_models.begin());
	
	std::vector<ReferenceRep*> inner_modelreferences = mod_ptr->getInnerModelReferencesFinal();
	std::vector<ReferenceRep*>::iterator model_ref_it = inner_modelreferences.begin();
	for( ; model_ref_it != inner_modelreferences.end(); ++model_ref_it)
	{
		std::string m_r_n = (*model_ref_it)->getName();//t

		std::vector<const ModelRep *> models_ref_refers = (*model_ref_it)->getModelRefVector(); // all models the reference refers to
		std::vector<const ModelRep *>::iterator ref_model_it = models_ref_refers.begin();
		for( ; ref_model_it != models_ref_refers.end(); ++ref_model_it)
		{
			std::string r_m_n = (*ref_model_it)->getName();
			if ( std::find( inner_models.begin(), inner_models.end(), *ref_model_it) == inner_models.end())
				inner_models.push_back( *ref_model_it);
		}
	}
	// inner_models now contains all inner models and all models the inner references refer to
	// why? because the inner references may look like the model they refer to, so if a port is
	// visible on a model then that port is visible on all references (to that model)

	SDList* targets[ number_of_endpoints] = { &m_oper1, &m_oper2 };
	// for each target (src and dst)
	for( int i = 0; i < number_of_endpoints; ++i)
	{
		SDList_Iterator op_it = targets[i]->begin();
		for( ; op_it != targets[i]->end(); ++op_it)
		{
			FCO* target_ptr = *op_it;

			std::vector<FCO*> descendants;

			// inquiring the descendants who have the same Interface
			target_ptr->getIntDescendants( descendants);
			descendants.push_back( target_ptr);

			std::vector<FCO*>::reverse_iterator desc_it = descendants.rbegin();
			// for each desc
			for( ; desc_it != descendants.rend(); ++desc_it)
			{
				FCO * fco = *desc_it;
				std::string fco_name = fco->getName();

				//*desc_it -> is a potential target -> 
				if ( !fco->isAbstract() ) 
				{
					//inquire the roles this FCO has in this model
					//const RoleRepSeries & series = fco_ptr->getMyRoles( mod_ptr);
					RoleRep::RoleRepSeries series;
					bool has_some = mod_ptr->getFinalRoles( fco, series);
					if ( has_some && !series.empty()) 
					{
						// add the roles to the member list of the set ( in that model) 
						this->addTargetItem( i, mod_ptr, series);
					}

					std::vector<const ModelRep*>::iterator sub_mod_it = inner_models.begin();
					for(; sub_mod_it != inner_models.end(); ++sub_mod_it)
					{
						const ModelRep * sub_model = *sub_mod_it;
						std::string sm_name = sub_model->getName();
						// inquire the roles of fco in the sub_model
						RoleRep::RoleRepSeries roles_in_sub_model;
						bool has_some = sub_model->getFinalRoles( fco, roles_in_sub_model);
						RoleRep::RoleRepSeries_ConstIterator r_it = roles_in_sub_model.begin();
						for( ; has_some && r_it != roles_in_sub_model.end(); ++r_it) // if (!roles_in_sub_model.empty())
						{
							RoleRep sub_role = *r_it;	//TOF("    pe sub_role: " + sub_role.getSmartRoleName() + " in " + sub_model->getName());
							//check if any role is port
							if ( sub_role.isPort()) // this role may appear as a port of sub_model (and/or may appear as a port of a reference to this submodel too)
							{
								if ( sub_model->getMyKind() == Any::MODEL && !sub_model->isAbstract())
								{
									// fetch all roles of sub_model_desc in model
									RoleRep::RoleRepSeries sub_model_roles_in_model;
									bool has_some_subm = mod_ptr->getFinalRoles( sub_model, sub_model_roles_in_model ); // if the model contains sub_model with some roles
									RoleRep::RoleRepSeries_ConstIterator r_it_sm = sub_model_roles_in_model.begin();
									for( ; has_some_subm && r_it_sm != sub_model_roles_in_model.end(); ++r_it_sm )
									{
										std::string desc_sub_model_role_name = r_it_sm->getSmartRoleName();
										PointerItem item = desc_sub_model_role_name + " " + sub_role.getSmartRoleName();
										this->addTargetItem( i, mod_ptr, item);
									}
									// take a look on the references to sub_model
									const FCO::ReferenceRepList &ref_list = sub_model->getTransitiveReferencesToMe(); //references may point to submodel
									FCO::ReferenceRepList_ConstIterator ref_it = ref_list.begin();
									for( ; ref_it != ref_list.end(); ++ref_it)
									{
										if( !(*ref_it)->isAbstract() && (*ref_it)->amIPartOfFinal( mod_ptr) ) // if model may contain the reference
										{
											RoleRep::RoleRepSeries sub_ref_roles_in_model;
											bool ref_has_roles = mod_ptr->getFinalRoles( *ref_it, sub_ref_roles_in_model ); // if the model contains reference with some roles
											RoleRep::RoleRepSeries_ConstIterator r_it_ref = sub_ref_roles_in_model.begin();
											for( ; ref_has_roles && r_it_ref != sub_ref_roles_in_model.end(); ++r_it_ref )
											{
												std::string desc_sub_ref_role_name = r_it_ref->getSmartRoleName();
												PointerItem item = desc_sub_ref_role_name + " " + sub_role.getSmartRoleName();
												this->addTargetItem( i, mod_ptr, item);
												reference_ports_also[i] = true; // this will indicate
											}
											/* changed on 3/9/2004
											PointerItem item = (*ref_it)->getName() + " " + sub_role.getSmartRoleName();
											this->addTargetItem( i, mod_ptr, item);
											reference_ports_also[i] = true; // this will indicate
											*/
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	// these values may be set after several calls to intInherit, that is why
	m_oper1IsAnyReferencePort = m_oper1IsAnyReferencePort || reference_ports_also[0];
	m_oper2IsAnyReferencePort = m_oper2IsAnyReferencePort || reference_ports_also[1];
}


bool ConnJoint::checkElements( std::string connection_name)
{
	bool res = true;
	const int number_of_lists = 2;
	const int number_of_endpoints = 2;

	const SDList* lists [ number_of_endpoints]= 
	{
		&m_oper1, &m_oper2
	};

	for( int i = 0; i < number_of_endpoints; ++i)
	{
		SDList_ConstIterator fco_it = lists[i]->begin();
		// for all fcos of the src or dst lists
		for( ; fco_it != lists[i]->end(); ++fco_it)
		{
			FCO * member_ptr = *fco_it;

			if ( !member_ptr->checkIsPartOfFinal() && !member_ptr->isAbstract())
			{
				global_vars.err << "CHECK: \"" << member_ptr->getName() << "\" fco in connection \"" << connection_name << "\" is not contained by any model.\n";
				res = false;
			}
		}
	}
	return res;
}


bool ConnJoint::calcLCD()
{
	if (!m_bidirect && !m_oper1.empty())
	{
		m_srcLCD = FCO::lcdIntersect( m_oper1);
	}
	if (!m_bidirect && !m_oper2.empty())
	{
		m_dstLCD = FCO::lcdIntersect( m_oper2);
	}
	if (!m_bidirect)
	{
		std::string mmm = "Src lcd: ";
		std::vector<FCO*>::iterator it = m_srcLCD.begin();
		for(; it != m_srcLCD.end(); ++it)
		{
			mmm += (*it)->getName() + "\t";
		}
		mmm += "\nDst lcd: ";
		it = m_dstLCD.begin();
		for(; it != m_dstLCD.end(); ++it)
		{
			mmm += (*it)->getName() + "\t";
		}
		//TO( m_connPtr->getName() + " has\n" + mmm);
	}
	if (m_bidirect)
	{
		SDList union_list = m_oper1;
		union_list.insert( union_list.end(), m_oper2.begin(), m_oper2.end());
		m_srcLCD = FCO::lcdIntersect( union_list);

		std::string mmm = "Src/dst lcd: ";
		SDList_Iterator it = m_srcLCD.begin();
		for(; it != m_srcLCD.end(); ++it)
		{
			mmm += (*it)->getName() + "\t";
		}
		//TO( m_connPtr->getName() + " has\n" + mmm);
	}
	return true;
}


bool ConnJoint::createLinkGetters()
{
	//
	// 1st task
	// create methods like InPort::getInConnLinks() ; owned by the ConnEnds
	if ( !m_bidirect)
	{
		if ( FCO::equal( m_oper1, m_oper2))
		{
			SDList_Iterator op1_it = m_oper1.begin();
			for( ; op1_it != m_oper1.end(); ++op1_it)
			{
				if ( (*op1_it)->isToBeEx())
				{
					if ( Dumper::m_bGenTemplates)
					{
						(*op1_it)->addConnMethod( CodeGen::dumpGetInConnectionLinksGeneric( *op1_it, m_connPtr));
						(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionLinksGeneric( *op1_it, m_connPtr));
						(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionLinksGeneric( *op1_it, m_connPtr));
					}

					if ( Dumper::m_bGenRegular)
					{
						(*op1_it)->addConnMethod( CodeGen::dumpGetInConnectionLinks( *op1_it, m_connPtr));
						(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionLinks( *op1_it, m_connPtr));
						(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionLinks( *op1_it, m_connPtr));
					}
				}
			}
		}
		else // not equal m_oper1 and m_oper2
		{
			SDList_Iterator op1_it = m_oper1.begin(); // oper1s have outgoing conns
			for( ; op1_it != m_oper1.end(); ++op1_it)
			{
				if ( (*op1_it)->isToBeEx())
				{
					if ( Dumper::m_bGenTemplates) 
						(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionLinksGeneric( *op1_it, m_connPtr));

					if ( Dumper::m_bGenRegular) 
						(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionLinks( *op1_it, m_connPtr));
				}
			}
			SDList_Iterator op2_it = m_oper2.begin(); // oper2s have incoming conns
			for( ; op2_it != m_oper2.end(); ++op2_it)
			{
				if ( (*op2_it)->isToBeEx())
				{
					if ( Dumper::m_bGenTemplates) 
						(*op2_it)->addConnMethod( CodeGen::dumpGetInConnectionLinksGeneric( *op2_it, m_connPtr));

					if ( Dumper::m_bGenRegular) 
						(*op2_it)->addConnMethod( CodeGen::dumpGetInConnectionLinks( *op2_it, m_connPtr));
				}
			}
		}
	}
	else // if (m_bidirect)
	{
		if ( FCO::equal( m_oper1, m_oper2))
		{
			SDList_Iterator op1_it = m_oper1.begin();
			for( ; op1_it != m_oper1.end(); ++op1_it)
				if ( (*op1_it)->isToBeEx())
				{
					if ( Dumper::m_bGenTemplates) 
						(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionLinksGeneric( *op1_it, m_connPtr));

					if ( Dumper::m_bGenRegular) 
						(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionLinks( *op1_it, m_connPtr));
				}
		}
		else // though bidirect, different src and dest
		{
			SDList_Iterator op1_it = m_oper1.begin();
			for( ; op1_it != m_oper1.end(); ++op1_it)
				if ( (*op1_it)->isToBeEx())
				{
					if ( Dumper::m_bGenTemplates)
						(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionLinksGeneric( *op1_it, m_connPtr));

					if ( Dumper::m_bGenRegular)
						(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionLinks( *op1_it, m_connPtr));
				}

			SDList_Iterator op2_it = m_oper2.begin();
			for( ; op2_it != m_oper2.end(); ++op2_it)
				if ( (*op2_it)->isToBeEx())
				{
					if ( Dumper::m_bGenTemplates)
						(*op2_it)->addConnMethod( CodeGen::dumpGetBothConnectionLinksGeneric( *op2_it, m_connPtr));

					if ( Dumper::m_bGenRegular)
						(*op2_it)->addConnMethod( CodeGen::dumpGetBothConnectionLinks( *op2_it, m_connPtr));
				}
		}
	}
	return true;
}


bool ConnJoint::createSrcDstGetters()
{	
	//
	// 2nd task
	// creating methods like Connection::getSrc() ; owned by the connection itself
	// the m_connPtr is to be extended if reached this point
	if ( !m_bidirect)
	{
		SDList_Iterator op1_it = m_oper1.begin();
		for( ; op1_it != m_oper1.end(); ++op1_it)
			m_connPtr->addMethod( CodeGen::dumpGetSrc( *op1_it, m_connPtr, m_oper1IsAnyReferencePort));

		SDList_Iterator op2_it = m_oper2.begin();
		for( ; op2_it != m_oper2.end(); ++op2_it)
			m_connPtr->addMethod( CodeGen::dumpGetDst( *op2_it, m_connPtr, m_oper2IsAnyReferencePort));
	}
	else // bidirect, but has some lcd
	{
		if ( !m_srcLCD.empty())
		{
			std::vector<FCO*>::iterator return_value_it = m_srcLCD.begin();
			for( ; return_value_it != m_srcLCD.end(); ++return_value_it)
			{ // if multiple inheritance is present the for cycle performed more than once
				m_connPtr->addMethod( CodeGen::dumpGetSrc( *return_value_it, m_connPtr, m_oper1IsAnyReferencePort));
				m_connPtr->addMethod( CodeGen::dumpGetDst( *return_value_it, m_connPtr, m_oper2IsAnyReferencePort));
			}
		}
		else // m_srcLCD.empty = true
		{
			m_connPtr->addMethod( CodeGen::dumpGetSrc( 0, m_connPtr, m_oper1IsAnyReferencePort));
			m_connPtr->addMethod( CodeGen::dumpGetDst( 0, m_connPtr, m_oper2IsAnyReferencePort));
		}
	}
	return true;
}


bool ConnJoint::createEndGetters()
{
	//
	// 3rd task
	// create methods like InPort::getInConnEnds() ; owned by the connEnds
	//                     InPort::getParamConnSrcs()
	if ( !m_bidirect)
	{
		// obsolete !m_srcLCD.empty() && !m_dstLCD.empty())
		SDList_Iterator op1_it = m_oper1.begin();
		for( ; op1_it != m_oper1.end(); ++op1_it) // for all src labeled FCOs
		{
			if ( (*op1_it)->isToBeEx())
			{
				if ( !m_dstLCD.empty())
				{
					SDList_Iterator peer_it = m_dstLCD.begin();
					for( ; peer_it != m_dstLCD.end(); ++peer_it) // separate getters for all the peers' lcd
					{
						FCO* peer_ptr = 0;
						if ( (*peer_it)->isToBeEx()) // use peer name only if is to extended
							peer_ptr = *peer_it;
						else if ( (*peer_it)->getExtedAnc()) // if it has an extended ancestor
							peer_ptr = (*peer_it)->getExtedAnc();

						if ( Dumper::m_bGenTemplates)
							(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionEndGeneric( *op1_it, peer_ptr, m_connPtr, m_oper2IsAnyReferencePort));

						if ( Dumper::m_bGenRegular)
							(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionEnd( *op1_it, peer_ptr, m_connPtr, m_oper2IsAnyReferencePort));
					}
				}
				else // no common denom (lcd) of dst nodes
				{
					if ( Dumper::m_bGenTemplates)
						(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionEndGeneric( *op1_it, 0, m_connPtr, m_oper2IsAnyReferencePort));

					if ( Dumper::m_bGenRegular)
						(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionEnd( *op1_it, 0, m_connPtr, m_oper2IsAnyReferencePort));
				}
			}
		}

		SDList_Iterator op2_it = m_oper2.begin();
		for( ; op2_it != m_oper2.end(); ++op2_it)
		{
			if ( (*op2_it)->isToBeEx())
			{
				if ( !m_srcLCD.empty())
				{
					std::vector<FCO*>::iterator peer_it = m_srcLCD.begin();
					for( ; peer_it != m_srcLCD.end(); ++peer_it)
					{
						FCO* peer_ptr = 0;
						if ( (*peer_it)->isToBeEx()) // use peer name only if is to extended
							peer_ptr = *peer_it;
						else if ( (*peer_it)->getExtedAnc()) // if it has an extended ancestor
							peer_ptr = (*peer_it)->getExtedAnc();

						if ( Dumper::m_bGenTemplates)
							(*op2_it)->addConnMethod( CodeGen::dumpGetInConnectionEndGeneric( *op2_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));

						if ( Dumper::m_bGenRegular)
							(*op2_it)->addConnMethod( CodeGen::dumpGetInConnectionEnd( *op2_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));
					}
				}
				else // no common denom (lcd) of src nodes
				{
					if ( Dumper::m_bGenTemplates)
						(*op2_it)->addConnMethod( CodeGen::dumpGetInConnectionEndGeneric( *op2_it, 0, m_connPtr, m_oper1IsAnyReferencePort));

					if ( Dumper::m_bGenRegular)
						(*op2_it)->addConnMethod( CodeGen::dumpGetInConnectionEnd( *op2_it, 0, m_connPtr, m_oper1IsAnyReferencePort));
				}
			}
		}
	}
	else if ( m_bidirect) // if bidirect then the srcLCD contains the lcds of m_oper1 and m_oper2 too
	{
		if ( !m_srcLCD.empty()) // this means that the lcd of union(oper1, oper2) is not FCO = the worst
		{
			if ( FCO::equal( m_oper1, m_oper2))
			{ // bidirect connection with same dst and src
				std::vector<FCO*>::iterator it = m_srcLCD.begin();
				for( ; it != m_srcLCD.end(); ++it)
				{
					FCO * peer_ptr = 0;
					if ( (*it)->isToBeEx()) // use peer name only if is to extended
						peer_ptr = *it;
					else if ( (*it)->getExtedAnc()) // if it has an extended ancestor
						peer_ptr = (*it)->getExtedAnc();

					SDList_Iterator op1_it = m_oper1.begin();
					for( ; op1_it != m_oper1.end(); ++op1_it) // though bidirect, some elements are still source
					{
						if ( (*op1_it)->isToBeEx())
						{
							if ( Dumper::m_bGenTemplates)
							{
								(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionEndGeneric( *op1_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));

								(*op1_it)->addConnMethod( CodeGen::dumpGetInConnectionEndGeneric( *op1_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));
								(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionEndGeneric( *op1_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));
							}

							if ( Dumper::m_bGenRegular)
							{
								(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionEnd( *op1_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));

								(*op1_it)->addConnMethod( CodeGen::dumpGetInConnectionEnd( *op1_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));
								(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionEnd( *op1_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));
							}
						}
					}
				}
			}
			else
			{ // bidirect connection, where the src and dst are different
				std::vector<FCO*>::iterator it = m_srcLCD.begin();
				for( ; it != m_srcLCD.end(); ++it)
				{
					FCO * peer_ptr = 0;
					if ( (*it)->isToBeEx()) // use peer name only if is to extended
						peer_ptr = *it;
					else if ( (*it)->getExtedAnc()) // if it has an extended ancestor
						peer_ptr = (*it)->getExtedAnc();

					SDList_Iterator op1_it = m_oper1.begin();
					for( ; op1_it != m_oper1.end(); ++op1_it) // though bidirect, some elements are still source
					{
						if ( (*op1_it)->isToBeEx())
						{
							if ( Dumper::m_bGenTemplates)
							{
								(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionEndGeneric( *op1_it, peer_ptr, m_connPtr, m_oper2IsAnyReferencePort));

								(*op1_it)->addConnMethod( CodeGen::dumpGetInConnectionEndGeneric( *op1_it, peer_ptr, m_connPtr, m_oper2IsAnyReferencePort));
								(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionEndGeneric( *op1_it, peer_ptr, m_connPtr, m_oper2IsAnyReferencePort));
							}

							if ( Dumper::m_bGenRegular)
							{
								(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionEnd( *op1_it, peer_ptr, m_connPtr, m_oper2IsAnyReferencePort));

								(*op1_it)->addConnMethod( CodeGen::dumpGetInConnectionEnd( *op1_it, peer_ptr, m_connPtr, m_oper2IsAnyReferencePort));
								(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionEnd( *op1_it, peer_ptr, m_connPtr, m_oper2IsAnyReferencePort));
							}
						}
					}

					SDList_Iterator op2_it = m_oper2.begin();
					for( ; op2_it != m_oper2.end(); ++op2_it) // though bidirect, some elements are still destination
					{
						if ( (*op2_it)->isToBeEx())
						{
							if ( Dumper::m_bGenTemplates)
							{
								(*op2_it)->addConnMethod( CodeGen::dumpGetBothConnectionEndGeneric( *op2_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));

								(*op2_it)->addConnMethod( CodeGen::dumpGetInConnectionEndGeneric( *op2_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));
								(*op2_it)->addConnMethod( CodeGen::dumpGetOutConnectionEndGeneric( *op2_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));
							}

							if ( Dumper::m_bGenRegular)
							{
								(*op2_it)->addConnMethod( CodeGen::dumpGetBothConnectionEnd( *op2_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));

								(*op2_it)->addConnMethod( CodeGen::dumpGetInConnectionEnd( *op2_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));
								(*op2_it)->addConnMethod( CodeGen::dumpGetOutConnectionEnd( *op2_it, peer_ptr, m_connPtr, m_oper1IsAnyReferencePort));
							}
						}
					}
				}
			}
		}
		else // if bidirect, but the two ends don't have anything in common lcd = FCO
		{
			SDList_Iterator op1_it = m_oper1.begin();
			for( ; op1_it != m_oper1.end(); ++op1_it)
			{
				if ( (*op1_it)->isToBeEx())
				{
					if ( Dumper::m_bGenTemplates)
					{
						(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionEndGeneric( *op1_it, 0, m_connPtr, m_oper2IsAnyReferencePort));
						(*op1_it)->addConnMethod( CodeGen::dumpGetInConnectionEndGeneric( *op1_it, 0, m_connPtr, m_oper2IsAnyReferencePort));
						(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionEndGeneric( *op1_it, 0, m_connPtr, m_oper2IsAnyReferencePort));
					}
					
					if ( Dumper::m_bGenRegular)
					{
						(*op1_it)->addConnMethod( CodeGen::dumpGetBothConnectionEnd( *op1_it, 0, m_connPtr, m_oper2IsAnyReferencePort));
						(*op1_it)->addConnMethod( CodeGen::dumpGetInConnectionEnd( *op1_it, 0, m_connPtr, m_oper2IsAnyReferencePort));
						(*op1_it)->addConnMethod( CodeGen::dumpGetOutConnectionEnd( *op1_it, 0, m_connPtr, m_oper2IsAnyReferencePort));
					}
				}
			}

			SDList_Iterator op2_it = m_oper2.begin();
			for( ; op2_it != m_oper2.end(); ++op2_it)
			{
				if ( (*op2_it)->isToBeEx())
				{
					if ( Dumper::m_bGenTemplates)
					{
						(*op2_it)->addConnMethod( CodeGen::dumpGetBothConnectionEndGeneric( *op2_it, 0, m_connPtr, m_oper1IsAnyReferencePort));
						(*op2_it)->addConnMethod( CodeGen::dumpGetInConnectionEndGeneric( *op2_it, 0, m_connPtr, m_oper1IsAnyReferencePort));
						(*op2_it)->addConnMethod( CodeGen::dumpGetOutConnectionEndGeneric( *op2_it, 0, m_connPtr, m_oper1IsAnyReferencePort));
					}

					if ( Dumper::m_bGenRegular)
					{
						(*op2_it)->addConnMethod( CodeGen::dumpGetBothConnectionEnd( *op2_it, 0, m_connPtr, m_oper1IsAnyReferencePort));
						(*op2_it)->addConnMethod( CodeGen::dumpGetInConnectionEnd( *op2_it, 0, m_connPtr, m_oper1IsAnyReferencePort));
						(*op2_it)->addConnMethod( CodeGen::dumpGetOutConnectionEnd( *op2_it, 0, m_connPtr, m_oper1IsAnyReferencePort));
					}
				}
			}
		}
	}
	return true;
}

