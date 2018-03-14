#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"


#include "ConnJoint.h"
#include "ConnectionRep.h"
#include "logger.h"
#include "ModelRep.h"
#include "ReferenceRep.h"
#include "Broker.h"
#include "ConstraintRep.h"
#include "Dumper.h"

#include "algorithm"

#include "globals.h"
extern Globals global_vars;


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
{
}


ConnJoint::ConnJoint( const ConnJoint& peer)
	: m_connPtr( peer.m_connPtr)
	
	, m_oper1( peer.m_oper1)
	, m_oper2( peer.m_oper2)
	
	, m_oper1TargetMap( peer.m_oper1TargetMap)
	, m_oper2TargetMap( peer.m_oper2TargetMap)
	
	, m_bidirect( peer.m_bidirect)
	
	, m_oper1Card( peer.m_oper1Card)
	, m_oper2Card( peer.m_oper2Card)
{ 
}


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

	return *this;
}


ConnJoint::~ConnJoint() 
{
	m_connPtr = 0;
	
	m_oper1.clear();
	m_oper2.clear();

	m_oper1TargetMap.clear();
	m_oper2TargetMap.clear();
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
	PointerItem item = new_role.getSmartRoleName();
	addTargetItem( i, model, item);
}


void ConnJoint::addTargetItem( int i, const ModelRep * model, const RoleRep::RoleRepSeries & new_role_series)
{ 
	RoleRep::RoleRepSeries_ConstIterator r_it = new_role_series.begin();
	for( ; r_it != new_role_series.end(); ++r_it)
		addTargetItem( i, model, *r_it);
}


void ConnJoint::intInherit( ModelRep * mod_ptr)
{
	std::string con_name = m_connPtr->getName(); //t
	std::string mod_name = mod_ptr->getName(); //t

	const int number_of_endpoints = 2;
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
									const FCO::ReferenceRepList &ref_list = sub_model->getTransitiveReferencesToMe(); //if you allow conns for transitive references //references may point to submodel
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
											}
											/*PointerItem item = (*ref_it)->getName() + " " + sub_role.getSmartRoleName();
											this->addTargetItem( i, mod_ptr, item);*/
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
				global_vars.err << MSG_WARNING << "CHECK: \"" << member_ptr->getName() << "\" fco in connection \"" << connection_name << "\" is not contained by any model.\n";
				res = false;
			}
		}
	}
	return res;
}


std::string ConnJoint::dumpElements( FCO::ModelRepPtrList & model_list)
{
	std::string mmm;
	std::string mmm_src = "";
	std::string mmm_dst = "";

	PointerItemSeries src_dumper_list, dst_dumper_list;

	FCO::ModelRepPtrList_ConstIterator mod_it = model_list.begin();
	// for all models i am part of
	for( ; mod_it != model_list.end(); ++mod_it )
	{
		ModelRep * mod_ptr = *mod_it;
		PointerItemSeries_Iterator item_it = m_oper1TargetMap[ mod_ptr].begin();
		for( ; item_it != m_oper1TargetMap[ mod_ptr].end(); ++item_it)
		{
			if ( std::find( src_dumper_list.begin(), src_dumper_list.end(), *item_it) == 
				src_dumper_list.end()) // not found
				src_dumper_list.push_back( *item_it);
			else 
			{ /* <!> too many warnings global_vars.err << "Warning: \"" << item_it->name() << "\" present twice in connnection \"" << m_connPtr->getName() << "\" as src.\n";*/ }
		}
		
		item_it = m_oper2TargetMap[ mod_ptr].begin();
		for( ; item_it != m_oper2TargetMap[ mod_ptr].end(); ++item_it)
		{
			if ( std::find( dst_dumper_list.begin(), dst_dumper_list.end(), *item_it) ==
				dst_dumper_list.end()) // not found
				dst_dumper_list.push_back( *item_it);
			else 
			{ /* <!> too many warnings global_vars.err << "Warning: \"" << item_it->name() << "\" present twice in connnection \"" << m_connPtr->getName() << "\" as dst.\n";*/ }
		}
	}

	mmm += indStr() + "<connjoint>\n";
	++ind;

	mmm += indStr() + "<pointerspec name = \"src\">\n";
	++ind;

	PointerItemLex lex;
	std::sort( src_dumper_list.begin(), src_dumper_list.end(), lex);
	std::sort( dst_dumper_list.begin(), dst_dumper_list.end(), lex);

	PointerItemSeries_Iterator item_it = src_dumper_list.begin();
	for( ; item_it != src_dumper_list.end(); ++item_it)
		mmm_src += indStr() + "<pointeritem desc = \"" + item_it->name() + "\"></pointeritem>\n";

	--ind;
	mmm += mmm_src + indStr() + "</pointerspec>\n";
	mmm += indStr() + "<pointerspec name = \"dst\">\n";
	++ind;

	item_it = dst_dumper_list.begin();
	for( ; item_it != dst_dumper_list.end(); ++item_it)
		mmm_dst += indStr() + "<pointeritem desc = \"" + item_it->name() + "\"></pointeritem>\n";

	--ind;
	mmm += mmm_dst + indStr() + "</pointerspec>\n";

	--ind;
	mmm += indStr() + "</connjoint>\n";
	
	// if bidirect dump the other direction also
	if (m_bidirect) 
	{
		mmm += indStr() + "<connjoint>\n";
		++ind;

		mmm += indStr() + "<pointerspec name = \"src\">\n";

		mmm += mmm_dst + indStr() + "</pointerspec>\n";

		mmm += indStr() + "<pointerspec name = \"dst\">\n";

		mmm += mmm_src + indStr() + "</pointerspec>\n";

		--ind;
		mmm += indStr() + "</connjoint>\n";
	}
	return mmm;
}


void ConnJoint::createConstraints( Sheet* s, const std::string& conn_name)
{
	int cons_id1 = Broker::getNextConstraintId();
	int cons_id2 = Broker::getNextConstraintId();

	char cons_id1_str[64], cons_id2_str[64];

	sprintf( cons_id1_str, "%d", cons_id1);
	sprintf( cons_id2_str, "%d", cons_id2);

	std::string src_cons_name, dst_cons_name;
	src_cons_name = "Valid" + conn_name + m_srcLabel + "Cardinality" + cons_id1_str;
	dst_cons_name = "Valid" + conn_name + m_dstLabel + "Cardinality" + cons_id2_str;
	std::string::size_type pos = 0;
	pos = src_cons_name.find( "::");
	if( pos != std::string::npos) 
		src_cons_name.replace( pos, 2, 2, '_');
	pos = dst_cons_name.find( "::");
	if( pos != std::string::npos)
		dst_cons_name.replace( pos, 2, 2, '_');

	int iEventMask = 0;
	char chMask[64];
	sprintf( chMask, "%x", iEventMask );
	std::string mask = chMask;

	// Parse Cardinality, build Expression

	std::string src_expr_begin, dst_expr_begin;
	src_expr_begin = "let srcCount = self.attachingConnections( \"" + m_dstLabel + "\", meta::" + conn_name + " ) -> size in\n                     ";
	dst_expr_begin = "let dstCount = self.attachingConnections( \"" + m_srcLabel + "\", meta::" + conn_name + " ) -> size in\n                     "; 
	
	std::string src_expr_end, dst_expr_end;
	std::string src_card_context, dst_card_context;

	src_card_context = "[connection] Connection: " + conn_name + ", Conn.Role: " + m_srcLabel;
	dst_card_context = "[connection] Connection: " + conn_name + ", Conn.Role: " + m_dstLabel;
	
	if ( Dumper::doParseCardinality( this->m_oper1Card, "srcCount", src_card_context, src_expr_end))
	{
		global_vars.err << MSG_WARNING << "Ignoring invalid cardinality string in connection: " << m_connPtr->getPtr() << ". String: " << this->m_oper1Card << "\n";
	}
	if ( ! src_expr_end.empty() ) 
	{
		// assign the source cardinality constraints to DST-s
		SDList_Iterator it = m_oper2.begin();
		while ( it != m_oper2.end())
		{
			std::string src_desc;
			src_desc = "Multiplicity of objects, which are associated to " + (*it)->getName() + 
			" as \"" + m_srcLabel + "\" over " + conn_name + ", has to match " + m_oper1Card + ".";

			ConstraintRep * cr = s->createConstraintRep( BON::FCO());
			std::string s_b = src_expr_begin + src_expr_end;
			cr->init( src_cons_name, /*mask:*/ global_vars.genConstr.connect_mask, "1", global_vars.genConstr.priority, s_b, src_desc);

			(*it)->addInitialConstraintRep( cr);
			cr->attachedTo();
			++it;
		}
	}
	if ( Dumper::doParseCardinality( this->m_oper2Card, "dstCount", dst_card_context, dst_expr_end))
	{
		global_vars.err << MSG_WARNING << "Ignoring invalid cardinality string in connection: " << m_connPtr->getPtr() << ". String: " << this->m_oper2Card << "\n";
	}
	if ( ! dst_expr_end.empty() ) 
	{
		// assign the dst cardinality constraints to SRC-s
		SDList_Iterator it = m_oper1.begin();
		while ( it != m_oper1.end())
		{
			std::string dst_desc;
			dst_desc = "Multiplicity of objects, which are associated to " + (*it)->getName() + 
			" as \"" + m_dstLabel + "\" over " + conn_name + ", has to match " + m_oper2Card + ".";

			ConstraintRep * cr = s->createConstraintRep( BON::FCO());
			std::string d_b = dst_expr_begin + dst_expr_end;
			cr->init( dst_cons_name, /*mask:*/ global_vars.genConstr.connect_mask, "1", global_vars.genConstr.priority, d_b, dst_desc);

			(*it)->addInitialConstraintRep( cr);
			cr->attachedTo();
			++it;
		}
		//mm.push_back(cr);
	}
}


/*
the added connjoint (since it comes from above) is supposed to have an m_oper1 and m_oper2 
values that are ancestors of the m_oper1 and m_oper2 lists of *this 
*/
bool ConnJoint::descendantsOf( const ConnJoint& peer) const
{
	bool res = true;
	
	const SDList* mylists[] = { &m_oper1, &m_oper2 };
	const SDList* peerlists[] = { &peer.m_oper1, &peer.m_oper2 };

	for( int i = 0; res && i < 2; ++i)
	{
		SDList_ConstIterator it = mylists[i]->begin();
		for( ; res && it != mylists[i]->end(); ++it)
		{
			bool current_found = false;
			
			SDList_ConstIterator itp = peerlists[i]->begin();
			for( ; !current_found && itp != peerlists[i]->end(); ++itp)
			{
				std::vector<FCO*> descs;
				(*itp)->getIntDescendants( descs);

				if ( std::find( descs.begin(), descs.end(), *it) != descs.end()) // found
					current_found = true;
			}
			res = res && current_found;
		}
	}
	return res;
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


