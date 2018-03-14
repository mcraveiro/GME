#include "StdAfx.h"
#include "BON2Component.h"

#include "logger.h"

#include "string"

#include "Dumper.h"
#include "globals.h"

extern Globals global_vars;

namespace BON
{

void Component::entityBuilder( const Model& model, const Folder& parent)
{
	// handling sets
	std::set<Set> aspects = model->getChildSets();
	std::set<Set>::iterator aspect = aspects.begin();
	for( ; aspect != aspects.end(); ++aspect)
	{
		Entity new_elem( parent, *aspect);
		m_entities.push_back( new_elem);

		std::set<FCO> aspect_elements = (*aspect)->getMembers();
		std::set<FCO>::iterator asp_elem = aspect_elements.begin();
		for( ; asp_elem != aspect_elements.end(); ++asp_elem)
		{
			// an aspect member may be of type atom (any kind) or reference type (proxy) or
			// connection (like containment = role)
			// it cannot be of type set (aspect), or type model

			if ((*asp_elem)->getObjectMeta().type() == MON::OT_Atom ||
				(*asp_elem)->getObjectMeta().type() == MON::OT_Reference) 
			{ // if an atom or reference is to be an aspect member
				
				Relation rela( Relation::ASPECT_MEMBER_OP, *aspect, *asp_elem);

				if ( m_relations.end() == std::find( m_relations.begin(), m_relations.end(), rela))
					m_relations.push_back( rela);
				else
					global_vars.err << MSG_ERROR << "Internal warning: Relation ASPECT_MEMBER \"" << BON::FCO( *asp_elem) << "\" found already in relations.\n";
			}
			else if ((*asp_elem)->getObjectMeta().type() == MON::OT_Connection &&
				(*asp_elem)->getObjectMeta().name() == Relation::containment_str ) 
			{ // if a role is intended to be an aspect member
				FCO src_fco, dst_model;
				Connection containment( *asp_elem);
				if (containment)
				{
					src_fco = FCO( containment->getSrc());
					dst_model = FCO( containment->getDst());
				}
				
				Relation rela( Relation::ASPECT_MEMBER_OP, *aspect, src_fco, *asp_elem, dst_model); // special ASPECT_MEMBER_OP

				if ( m_relations.end() == std::find( m_relations.begin(), m_relations.end(), rela))
					m_relations.push_back( rela);
				else
					global_vars.err << MSG_ERROR << "Internal warning: Relation ASPECT_MEMBER \"" << BON::FCO( *asp_elem) << "\" found already in relations\n";
			}
			else
				global_vars.err << MSG_ERROR << "Cannot handle AspectMember operation. Op1: " << BON::FCO( *aspect) << " Op2: " << BON::FCO( *asp_elem) << "\n";
		}
	}

	// handling connections
	std::set<Connection> conns = model->getChildConnections();
	std::set<Connection>::iterator conn_it = conns.begin();
	for( ; conn_it != conns.end(); ++conn_it)
	{
			#if(0)
			**
			* in case of reference:
			* op1: FCO ( referred )
			* op2: Reference
			* 
			* in other cases: 
			* op1: container
			* op2: element
			* op3: association class (in case of connector)
			*      connection line class (in case of containment and foldercontainment)
			* op4: connection line class (in case of connector: source2connector)
			* op5: connection line class (in case of connector: connector2destination)
			*
			#endif

		std::string conn_kind = (*conn_it)->getObjectMeta().name();
		if ( conn_kind == Relation::containment_str ||
			conn_kind == Relation::folder_containment_str ||
			conn_kind == Relation::set_membership_str ||
			conn_kind == Relation::refer_to_str ||
			conn_kind == Relation::has_aspect_str ||
			conn_kind == Relation::has_constraint_str ||
			conn_kind == Relation::has_attribute_str)
		{
			FCO src = FCO((*conn_it)->getSrc());
			FCO dst = FCO((*conn_it)->getDst());

			Relation rela( Relation::CONTAINMENT_OP, dst, src); // create a relation object

			if ( conn_kind == Relation::containment_str)
			{
				rela.setOperation( Relation::CONTAINMENT_OP); 
				rela.setOp3( *conn_it);
			}
			else if ( conn_kind == Relation::folder_containment_str)
			{
				rela.setOperation( Relation::FOLDER_CONTAINMENT_OP); 
				rela.setOp3( *conn_it);
			}
			else if ( conn_kind == Relation::set_membership_str)
				rela.setOperation( Relation::SET_MEMBER_OP);
			else if ( conn_kind == Relation::refer_to_str)
				rela.setOperation( Relation::REFER_TO_OP);
			else if ( conn_kind == Relation::has_aspect_str)
				rela.setOperation( Relation::HAS_ASPECT_OP);
			else if ( conn_kind == Relation::has_constraint_str)
				rela.setOperation( Relation::HAS_CONSTRAINT_OP);
			else if ( conn_kind == Relation::has_attribute_str)
				rela.setOperation( Relation::HAS_ATTRIBUTE_OP);
			else
				global_vars.err << MSG_ERROR << "Unknown connection kind.\n";

			m_relations.push_back(rela);
		}
	}

	// handling proxies
	std::set<Reference> refs = model->getChildReferences();
	std::set<Reference>::iterator ref_it = refs.begin();
	for( ; ref_it != refs.end(); ++ref_it)
	{
		Entity new_elem( parent, *ref_it);
		m_entities.push_back( new_elem);

		try
		{
			FCO referred = (*ref_it)->getReferred();
			this->m_realObj[ *ref_it] = referred; //! a NULL proxy is part of m_realObj, having a NULL value attached
			if ( referred) // if not a null reference
				this->m_equivBag[ referred].insert( *ref_it);
			else // notify about null proxies
				global_vars.err << MSG_ERROR << "Null proxy object found: " << BON::FCO( *ref_it) << "\n";
		}
		catch( ... )
		{
			global_vars.err << MSG_ERROR << "Handling exeception thrown by Null proxy " << BON::FCO( *ref_it) << "\n";
		}
 	}
	
	// handling atoms
	std::set<Atom> atoms = model->getChildAtoms();
	std::set<Atom>::iterator atom_it = atoms.begin();
	for( ; atom_it != atoms.end(); ++atom_it)
	{
		if ( (*atom_it)->getStereotype() != OT_Set) // sets handled already
		{
			Entity new_elem( parent, *atom_it);
			
			std::string kind_name = (*atom_it)->getObjectMeta().name();
			if ( kind_name != Relation::connector_str.c_str() && 
				kind_name != Relation::inheritance_str.c_str() && 
				kind_name != Relation::int_inheritance_str.c_str() && 
				kind_name != Relation::imp_inheritance_str.c_str() && 
				kind_name != Relation::equivalence_str.c_str() &&
				kind_name != Relation::same_folder_str.c_str() && 
				kind_name != Relation::same_aspect_str.c_str())
			{
				m_entities.push_back( new_elem); // deserves the Entity status
			}
			else if ( kind_name == Relation::connector_str)
			{
				Connection in_connection_line;
				Connection out_connection_line;
				
				ConnectionEnd obj1, obj2, obja;

				std::set<Connection> con_list_ptr_i = (*atom_it)->getInConnLinks( Relation::connector_src.c_str()); // list of srcs
				std::set<Connection> con_list_ptr_o = (*atom_it)->getOutConnLinks( Relation::connector_dst.c_str());// list of dsts

				if ( !con_list_ptr_i.empty())
				{
					in_connection_line = * con_list_ptr_i.begin();
					obj1 = in_connection_line->getSrc();
				}
				if ( !con_list_ptr_o.empty())
				{
					out_connection_line = * con_list_ptr_o.begin();
					obj2 = out_connection_line->getDst();
				}
				if ( !in_connection_line || !out_connection_line)
					global_vars.err << MSG_ERROR << "Connector \"" << BON::FCO( *atom_it) << "\" does not have both SourceToConnector and ConnectorToDestination connections attached.\n";

				
				
				// the AssociationClass connections may be <in> OR <out>
				// first check for outgoing AssociationClass connections 
				std::multiset<ConnectionEnd> a_out_list = (*atom_it)->getOutConnEnds( Relation::connector_descr);
				std::multiset<ConnectionEnd> a_in_list = (*atom_it)->getInConnEnds( Relation::connector_descr);

				if ( !a_out_list.empty())
				{
					obja = * a_out_list.begin();
				}
				else if ( !a_in_list.empty()) // if not found outgoings check for incoming AssociationClass connections 
				{
					obja = * a_in_list.begin();
				}
				else 
					global_vars.err << MSG_ERROR << "Connector \"" << BON::FCO( *atom_it) << "\" has no Associated Class!\n";


				// insert into the relations table an association with 
				// src: obj1, dest: obj2, association class: obja, in line, and out_line
				if ( !obj1 || !obj2 || !obja || !in_connection_line || !out_connection_line)
				{
					global_vars.err << MSG_ERROR << "Association \"" << BON::FCO( obja) << "\" disregarded.\n";
				}
				else
				{
					Relation rela( 
						Relation::ASSOCIATION_OP, 
						FCO( obj1),
						FCO( obj2),
						FCO( obja),
						in_connection_line,
						out_connection_line);
					m_relations.push_back(rela);
				}
			}
			else if ( kind_name == Relation::inheritance_str)
			{
				ConnectionEnd obj1, obj2;
				std::multiset<ConnectionEnd> in_list = (*atom_it)->getInConnEnds( Relation::inheritance_base);
				std::multiset<ConnectionEnd> out_list = (*atom_it)->getOutConnEnds( Relation::inheritance_derived);
				std::multiset<ConnectionEnd>::iterator in_it = in_list.begin();
				std::multiset<ConnectionEnd>::iterator out_it = out_list.begin();

				int how_many_bases = 0;
				for( ; in_it != in_list.end(); ++in_it ) // if outer cycle performs more than once should be an error
				{
					obj1 = * in_it;
					++how_many_bases;
					if ( how_many_bases > 1) global_vars.err << MSG_WARNING << "Too many base classes at " << BON::FCO( *atom_it) << ". Multiple inheritance should be used with two different operators.\n";
					for( ; out_it != out_list.end(); ++out_it)
					{
						obj2 = * out_it;

						Relation rela( Relation::INHERITANCE_OP, FCO( obj1), FCO( obj2));
						m_relations.push_back(rela);
					}
				}
			} /* end if*/
			else if ( kind_name == Relation::imp_inheritance_str)
			{
				ConnectionEnd obj1, obj2;
				std::multiset<ConnectionEnd> in_list = (*atom_it)->getInConnEnds( Relation::imp_inheritance_base);
				std::multiset<ConnectionEnd> out_list = (*atom_it)->getOutConnEnds( Relation::imp_inheritance_derived);
				std::multiset<ConnectionEnd>::iterator in_it = in_list.begin();
				std::multiset<ConnectionEnd>::iterator out_it = out_list.begin();

				int how_many_bases = 0;
				for( ; in_it != in_list.end(); ++in_it ) // if outer cycle performs more than once should be an error
				{
					obj1 = * in_it;
					++how_many_bases;
					if ( how_many_bases > 1) global_vars.err << MSG_WARNING << "Too many base classes at " << BON::FCO( *atom_it) << ". Multiple inheritance should be used with two different operators.\n";
					for( ; out_it != out_list.end(); ++out_it)
					{
						obj2 = * out_it;

						Relation rela( Relation::IMP_INHERITANCE_OP, FCO( obj1), FCO( obj2));
						m_relations.push_back(rela);
					}
				}
			} /* end if*/
			else if ( kind_name == Relation::int_inheritance_str)
			{
				ConnectionEnd obj1, obj2;
				std::multiset<ConnectionEnd> in_list = (*atom_it)->getInConnEnds( Relation::int_inheritance_base);
				std::multiset<ConnectionEnd> out_list = (*atom_it)->getOutConnEnds( Relation::int_inheritance_derived);
				std::multiset<ConnectionEnd>::iterator in_it = in_list.begin();
				std::multiset<ConnectionEnd>::iterator out_it = out_list.begin();

				int how_many_bases = 0;
				for( ; in_it != in_list.end(); ++in_it ) // if outer cycle performs more than once should be an error
				{
					obj1 = * in_it;
					++how_many_bases;
					if ( how_many_bases > 1) global_vars.err << MSG_WARNING << "Too many base classes at " << BON::FCO( *atom_it) << ". Multiple inheritance should be used with two different operators.\n";
					for( ; out_it != out_list.end(); ++out_it)
					{
						obj2 = * out_it;

						Relation rela( Relation::INT_INHERITANCE_OP, FCO( obj1), FCO( obj2));
						m_relations.push_back(rela);
					}
				}
			} /* end if*/
			else if ( kind_name == Relation::equivalence_str)
			{
				ConnectionEnd obj1, obj2;
				std::multiset<ConnectionEnd> in_list = (*atom_it)->getInConnEnds( Relation::equivalence_right);
				std::multiset<ConnectionEnd> out_list = (*atom_it)->getOutConnEnds( Relation::equivalence_left);
				std::multiset<ConnectionEnd>::iterator in_it = in_list.begin();
				std::multiset<ConnectionEnd>::iterator out_it = out_list.begin();

				for( ; in_it != in_list.end(); ++in_it ) // if outer cycle performs more than once should be an error
				{
					obj1 = * in_it;
					for( ; out_it != out_list.end(); ++out_it)
					{
						obj2 = * out_it;

						Relation rela( Relation::EQUIVALENCE_OP, FCO( obj1), FCO( obj2), *atom_it);
						m_equivRelations.push_back(rela);
					}
				}
			} /* end if*/
			else if ( kind_name == Relation::same_aspect_str)
			{
				ConnectionEnd obj1, obj2;
				std::multiset<ConnectionEnd> in_list = (*atom_it)->getInConnEnds( Relation::same_aspect_right);
				std::multiset<ConnectionEnd> out_list = (*atom_it)->getOutConnEnds( Relation::same_aspect_left);
				std::multiset<ConnectionEnd>::iterator in_it = in_list.begin();
				std::multiset<ConnectionEnd>::iterator out_it = out_list.begin();

				for( ; in_it != in_list.end(); ++in_it ) // if outer cycle performs more than once should be an error
				{
					obj1 = * in_it;
					for( ; out_it != out_list.end(); ++out_it)
					{
						obj2 = * out_it;

						Relation rela( Relation::SAME_ASPECT_OP, FCO( obj1), FCO( obj2), *atom_it);
						m_equivRelations.push_back(rela);
					}
				}
			} /* end if*/
			else if ( kind_name == Relation::same_folder_str)
			{
				ConnectionEnd obj1, obj2;
				std::multiset<ConnectionEnd> in_list = (*atom_it)->getInConnEnds( Relation::same_folder_right);
				std::multiset<ConnectionEnd> out_list = (*atom_it)->getOutConnEnds( Relation::same_folder_left);
				std::multiset<ConnectionEnd>::iterator in_it = in_list.begin();
				std::multiset<ConnectionEnd>::iterator out_it = out_list.begin();

				for( ; in_it != in_list.end(); ++in_it ) // if outer cycle performs more than once should be an error
				{
					obj1 = * in_it;
					for( ; out_it != out_list.end(); ++out_it)
					{
						obj2 = * out_it;

						Relation rela( Relation::SAME_FOLDER_OP, FCO( obj1), FCO( obj2), *atom_it);
						m_equivRelations.push_back(rela);
					}
				}
			} /* end if*/
		}
	}
}


/*************************************************************************
This method is responsible for selecting in case of SameAspect or SameFolder operators the 
box whose name will be used (this will override the other name)
Rules:
	The name of the real object is used if one operand is proxy
	The name of the SameAspect operator is used if both or none operands are proxy
*/
void Component::selectFromSameAspectsFolders()
{
	Relation_Iterator rel_it = m_equivRelations.begin();
	for( ; rel_it != m_equivRelations.end(); ++rel_it)
	{
		if ( rel_it->getOperation() == Relation::SAME_ASPECT_OP || 
			rel_it->getOperation() == Relation::SAME_FOLDER_OP || 
			rel_it->getOperation() == Relation::EQUIVALENCE_OP)
		{
			{
				int p1 = isProxy( rel_it->getOp1());
				int p2 = isProxy( rel_it->getOp2());
				if( p1 == 2 || p2 == 2) // check for Null proxies
				{
					global_vars.err << MSG_ERROR << rel_it->getOperationStr() << " relation disregarded between " << rel_it->getOp1() << " and " << rel_it->getOp2() << ".\n";
					continue;
				}

				bool is_proxy_first  = p1 == 1;
				bool is_proxy_second = p2 == 1;
				
				ObjPointer selected_ptr = 0;
				if (is_proxy_first == is_proxy_second) // both or neither is proxy
					selected_ptr = rel_it->getOp3(); //set the sameaspect operator as resp pointer
				else if (is_proxy_first) // the second is not proxy so set is as the responsible object
					selected_ptr = rel_it->getOp2();
				else // the first is not a proxy
					selected_ptr = rel_it->getOp1();

				// the selected_ptr never points to a proxy -> later on it won't be replaced
				// with another object by Component::OperandSearchAndReplace

				// set back into the aspect/folder entity the responsible pointer
				ObjPointer to_find = is_proxy_first?m_realObj[rel_it->getOp1()]:rel_it->getOp1();

				Entity_Iterator it( m_entities.begin());
				while( it != m_entities.end()) 
				{ 
					if ( to_find == it->getPointer()) 
					{
						it->setRespPointer( selected_ptr);
					}
					++it; 
				}

				to_find = is_proxy_second?m_realObj[rel_it->getOp2()]:rel_it->getOp2();
				it = m_entities.begin();
				while( it != m_entities.end()) 
				{ 
					if ( to_find == it->getPointer()) 
					{
						it->setRespPointer( selected_ptr);
					}
					++it; 
				}
			}
		}
	}
}


Entity Component::entitySearch( const ObjPointer& p_ptr)
{
	Entity_Iterator it( m_entities.begin());
	while ( it != m_entities.end() && p_ptr != it->getPointer())
		++it;
	
	if ( it == m_entities.end())
		return Entity( BON::Folder(), BON::FCO());
	else 
		return *it;
}


/*
Searches the relation container and replaces the proxy pointers with their real object peer
and removes the proxy from the entities container

Based on the assumption that a proxy cannot refer to another proxy
*/
void Component::proxyFinder()
{
	RealMap_Iterator real_map_it = this->m_realObj.begin();
	for( ; real_map_it != m_realObj.end(); ++real_map_it)
	{
		ObjPointer proxy_for_sure = real_map_it->first;
		ObjPointer real_for_sure = real_map_it->second;

#ifdef _DEBUG
		std::string n1, n2;
		if ( proxy_for_sure != BON::FCO())
			n1 = proxy_for_sure->getName();
		if ( real_for_sure != BON::FCO())
			n2= real_for_sure->getName();
#endif
		if ( proxy_for_sure != BON::FCO() && real_for_sure != BON::FCO())
		{
			Entity_Iterator it( m_entities.begin());
			while ( it != m_entities.end() && proxy_for_sure != it->getPointer())
				++it;

			if ( it == m_entities.end())
			{
				global_vars.err << MSG_ERROR << "Entity not found during proxyFinder: " << proxy_for_sure << "\n";
			}
			else
			{
				operandSearchAndReplace( proxy_for_sure, real_for_sure);
				//proxies are in equivBag, so it will be marked, no need for it->deleted( true);//m_toBeDeletedEntities.push_back( proxy);
			}
		}
	}
}


void Component::equivalenceFinder()
{
	Relation_Iterator rel_it = m_equivRelations.begin();
	while ( rel_it != m_equivRelations.end())
	{
		if( rel_it->getOperation() == Relation::EQUIVALENCE_OP ||
			rel_it->getOperation() == Relation::SAME_FOLDER_OP ||
			rel_it->getOperation() == Relation::SAME_ASPECT_OP )
		{ // 
			ObjPointer obj1 = rel_it->getOp1(), obj2 = rel_it->getOp2();

			
			if( obj1 != obj2) //avoid trivial case: "A equiv A"
			{
				// obj1 will have obj2 and proxies of obj2 as its alter egos
				insertIntoEquivBag( obj2, obj1);
				operandSearchAndReplace( obj2, obj1); //"Right law": repl left (op2) with right(op1)
			}
			
			rel_it = m_equivRelations.erase( rel_it); // <- relies on that erase increments rel_it

		}	
		else 
		{
			ASSERT(0); // no other relations are part of m_equivRelations
			++rel_it;
		}
	}
	ASSERT( m_equivRelations.empty()); // no relation should have remained
}


// collects the equivalent objects into a bag
// obj1 is the key in the map
// obj2 is the later-to-be-removed object from the entities
void Component::insertIntoEquivBag( const ObjPointer& obj2, const ObjPointer& obj1)
{
	if ( obj1 == obj2) return; // avoid trivial case

	std::set< FCO > bag2;

	// check if obj2 has its own bag?
	EquivBag_Iterator it = m_equivBag.find( obj2);
	if ( it != m_equivBag.end())
	{
		bag2 = m_equivBag[ obj2];
		m_equivBag.erase( it);
	}

	m_equivBag[obj1].insert( bag2.begin(), bag2.end());
	m_equivBag[obj1].insert( obj1);
	m_equivBag[obj1].insert( obj2);

#ifdef _DEBUG
	std::string n1 = obj1->getName();
	std::string n2 = obj2->getName();

	std::set< FCO > :: const_iterator ii = m_equivBag[obj1].begin();
	for( ; ii != m_equivBag[obj1].end(); ++ii)
	{
		n2 = (*ii)->getName();
	}
#endif

}


void Component::markEquivEntities()
{
	EquivBag_Iterator it = m_equivBag.begin();
	for( ; it != m_equivBag.end(); ++it)
	{
		ASSERT( !it->first->isDeleted());
		std::set< ObjPointer > & clique = it->second;
		std::set< ObjPointer > :: const_iterator jt = clique.begin();
		//std::string n1 = it->first->getName();

		for( ; jt != clique.end(); ++jt)
		{
			//std::string n2 = (*jt)->getName();
			if ( *jt != it->first) // the responsible might be in its equiv bag
			{
				Entity_Iterator kt( m_entities.begin());
				while ( kt != m_entities.end() && *jt != kt->getPointer())
					++kt;

				if ( kt != m_entities.end())
				{
					kt->deleted( true);//m_toBeDeletedEntities.push_back( ent);
				}
				else
				{
					global_vars.err << MSG_ERROR << "Entity not found during markEquivEntities: " << BON::FCO( *jt) << "\n";
				}
			}
		}
	}
}


/*
Searches the releation container and replaces all occurences of find_obj 
with real_obj.
Important: It replaces only the first three pointer operands (since 4th and 5th operands are
connections which cannot be proxies)
*/
void Component::operandSearchAndReplace( const std::vector<ObjPointer>& find_obj_vec, const ObjPointer& real_obj)
{
	const static int len = 2;
	Relations* reldb[ len] = { &m_relations, &m_equivRelations};
	for ( int k = 0; k < len; ++k)
	{
		Relation_Iterator rel_it = reldb[k]->begin();
		for( ; rel_it != reldb[k]->end(); ++rel_it)
		{
			if (!rel_it->getOp1()) global_vars.err << MSG_ERROR << "First operand is Null Pointer\n";
			if (!rel_it->getOp2()) global_vars.err << MSG_ERROR << "Second operand is Null Pointer\n";

			ObjPointer obj = BON::FCO(); obj = rel_it->getOp1();
			//if ( obj == find_obj) 
			if ( std::find( find_obj_vec.begin(), find_obj_vec.end(), obj) != find_obj_vec.end()) 
				rel_it->setOp1( real_obj); 

			obj = BON::FCO(); obj = rel_it->getOp2();
			//if ( obj == find_obj) 
			if ( std::find( find_obj_vec.begin(), find_obj_vec.end(), obj) != find_obj_vec.end()) 
				rel_it->setOp2( real_obj); 

			// obj at this time may be BON::FCO() (in most case it is)
			obj = BON::FCO(); obj = rel_it->getOp3();
			//if ( obj != BON::FCO() && obj == find_obj) 
			if ( obj != BON::FCO() && std::find( find_obj_vec.begin(), find_obj_vec.end(), obj) != find_obj_vec.end()) 
				rel_it->setOp3( real_obj); 

			// added recently, to be tested well <!>
			obj = BON::FCO(); obj = rel_it->getOp4();
			//if ( obj != BON::FCO() && obj == find_obj) 
			if ( obj != BON::FCO() && std::find( find_obj_vec.begin(), find_obj_vec.end(), obj) != find_obj_vec.end()) 
				rel_it->setOp4(real_obj); 

			obj = BON::FCO(); obj = rel_it->getOp5();
			//if ( obj != BON::FCO() && obj == find_obj) 
			if ( obj != BON::FCO() && std::find( find_obj_vec.begin(), find_obj_vec.end(), obj) != find_obj_vec.end()) 
				rel_it->setOp5(real_obj); 
		}
	}
}


void Component::operandSearchAndReplace( const ObjPointer& find_obj, const ObjPointer& real_obj)
{
	operandSearchAndReplace( std::vector< ObjPointer>( 1, find_obj), real_obj);
}


void Component::removeProxiesAndEquiv()
{
	markEquivEntities();
}


#if(NEEDED_FOR_METAINTERPRETER_ONLY)
bool Component::nameSelector()
{
	bool res = true;

	dlg = new NameSpecDlg();

	Entity_Iterator it_1( m_entities.begin());
	for( ; it_1 != m_entities.end(); ++it_1 )
	{
		if( it_1->isDeleted()) continue;

		FCO fco( it_1->getPointer());
		FCO resp( it_1->getRespPointer());
		
		EquivBag_Iterator it_2 = m_equivBag.find( fco);
		if( it_2 == m_equivBag.end()) continue;
		
		if( !resp) resp = fco;

		// m_equivBag[ fco] already exists , is not empty
		// it_2->first = fco
		// it_2->second = m_equivBag[ fco];
		std::string defname = resp->getName(); // take its name from its name responsible
		std::string regname;

		if( fco->getObjectMeta().name() == "Aspect")
			regname = AspectRep::getMyRegistry( fco, it_1->getParentFolder())->getValueByPath( "/" + Any::NameSelectorNode_str);
		else
			regname = Any::getMyRegistry( fco, it_1->getParentFolder())->getValueByPath( "/" + Any::NameSelectorNode_str);

		
		bool is_any_alternative = false;
		bool is_def_among_names = false; // when the equivalance's name is used, that is not among the equivs
		bool is_reg_among_names = false; // check whether the registry value is among the valid choices
		std::set < ObjPointer >::iterator jt = it_2->second.begin();
		for( ; jt != it_2->second.end(); ++jt)
		{
			if( !isProxy( *jt))
			{
				std::string eqname = (*jt)->getName(); // the name of one equivalent

				if( eqname == defname)
					is_def_among_names = true;
				else
					is_any_alternative = true;

				if( eqname == regname)
					is_reg_among_names = true;
			}
		}

		if( is_any_alternative)
		{
			CString dd = defname.c_str();
			CString kind = fco->getObjectMeta().name().c_str();
			
			if( is_reg_among_names)	// if valid info in registry use it
				dlg->m_dn[ fco] = regname;
			else					// otherwise use the default name
				dlg->m_dn[ fco] = defname;

			if( !is_def_among_names)
			{
				dlg->m_map[ fco].insert( resp);
			}

			jt = it_2->second.begin();
			for( ; jt != it_2->second.end(); ++jt)
			{
				if( !isProxy( *jt))
				{
					dlg->m_map[ fco].insert( *jt);
				}
			}
		}
	}

	res = false;
	if( !dlg->m_dn.empty())	res = dlg->DoModal() == IDOK;
	if( res)
	{
		NameSpecDlg::DEFNAMES_MAP::iterator it_0 = dlg->m_result.begin();
		for( ; it_0 != dlg->m_result.end(); ++it_0)
		{
			BON::FCO key_fco = it_0->first;
			std::string newsel_name = it_0->second;
			BON::FCO newsel_resp;

			Entity_Iterator it_1( m_entities.begin());
			for( ; it_1 != m_entities.end(); ++it_1 )
			{
				if( it_1->isDeleted()) continue;

				FCO fco( it_1->getPointer());
				if( fco == key_fco) // found the entity
				{
					if( fco->getObjectMeta().name() == "Aspect")
						AspectRep::getMyRegistry( fco, it_1->getParentFolder())->setValueByPath( "/" + Any::NameSelectorNode_str, newsel_name);
					else
						Any::getMyRegistry( fco, it_1->getParentFolder())->setValueByPath( "/" + Any::NameSelectorNode_str, newsel_name);

					//find the new resppointer value in the equivBag
					EquivBag_Iterator it_2 = m_equivBag.find( key_fco);
					if( it_2 == m_equivBag.end()) continue;

					std::set < ObjPointer >::iterator it_3 = it_2->second.begin();
					for( ; !newsel_resp && it_3 != it_2->second.end(); ++it_3)
					{
						if( !isProxy( *it_3))
						{
							std::string eqname = (*it_3)->getName(); // the name of one equivalent

							if( eqname == newsel_name)
								newsel_resp = *it_3;
						}
					}

					if( newsel_resp) // found
						it_1->setRespPointer( newsel_resp); // set the new name resp
					//if not found then the the default resp is in charge, which is correct
				}
			}
		}
	}

	delete dlg;
	return res;
}
#endif

int Component::isProxy( const ObjPointer& ptr)
{
	if( ptr == BON::FCO()) return 0;
	RealMap_Iterator it = m_realObj.find( ptr);
	if( it != m_realObj.end() && it->second != BON::FCO()) return 1;//if (m_realObj.find[ ptr] != BON::FCO()) return 1;
	if( ptr->getObjectMeta().name().find("Proxy") != std::string::npos) return 2;

	return 0;
}


bool Component::checkForProxies()
{
	std::vector< ObjPointer> to_delete;
	bool correct = true;
	Entity_Iterator it(m_entities.begin());
	for( ; it != m_entities.end(); ++it)
	{
		if ( it->isDeleted()) continue;

		int res;
		if ( res = isProxy( it->getPointer()))
		{
			if ( res == 1)
				global_vars.err << MSG_ERROR << "Internal error: entity " << it->getPointer() << " is a proxy.";
			else if ( res == 2)
				global_vars.err << MSG_ERROR << "Entity " << it->getPointer() << " is a NULL proxy.";

			//continuing
			global_vars.err << " Disregarding it from the Paradigm sheet.\n";
			correct = false;
			to_delete.push_back( it->getPointer());
		}
	}

	unsigned int c = to_delete.size();
	Relation_Iterator rel_it = m_relations.begin();
	for( ; rel_it != m_relations.end(); ++rel_it)
	{
		if ( isProxy( rel_it->getOp1()))
			to_delete.push_back( rel_it->getOp1());

		if ( isProxy( rel_it->getOp2()))
			to_delete.push_back( rel_it->getOp2());

		if ( isProxy( rel_it->getOp3()))
			to_delete.push_back( rel_it->getOp3());

		if ( isProxy( rel_it->getOp4()))
			to_delete.push_back( rel_it->getOp4());

		if ( isProxy( rel_it->getOp5()))
			to_delete.push_back( rel_it->getOp5());
	}

	rel_it = m_equivRelations.begin();
	for( ; rel_it != m_equivRelations.end(); ++rel_it)
	{
		if ( isProxy( rel_it->getOp1()))
			to_delete.push_back( rel_it->getOp1());

		if ( isProxy( rel_it->getOp2()))
			to_delete.push_back( rel_it->getOp2());

		if ( isProxy( rel_it->getOp3()))
			to_delete.push_back( rel_it->getOp3());

		if ( isProxy( rel_it->getOp4()))
			to_delete.push_back( rel_it->getOp4());

		if ( isProxy( rel_it->getOp5()))
			to_delete.push_back( rel_it->getOp5());
	}

	if ( c != to_delete.size()) correct = false;

	std::vector< ObjPointer>::iterator ee = to_delete.begin();
	for( ; ee != to_delete.end(); ++ee)
	{
		Entity_Iterator ent_it = m_entities.begin();
		while( ent_it != m_entities.end())
		{
			if (*ee != ent_it->getPointer())
				++ent_it;
			else
				ent_it = m_entities.erase( ent_it);
		}

		Relation_Iterator rel_it = m_relations.begin();
		while( rel_it != m_relations.end())
		{
			if ( rel_it->getOp1() != *ee &&
				rel_it->getOp2() != *ee &&
				rel_it->getOp3() != *ee &&
				rel_it->getOp4() != *ee &&
				rel_it->getOp5() != *ee )
				++rel_it;
			else
				rel_it = m_relations.erase( rel_it);
		}
		rel_it = m_equivRelations.begin();
		while( rel_it != m_equivRelations.end())
		{
			if ( rel_it->getOp1() != *ee &&
				rel_it->getOp2() != *ee &&
				rel_it->getOp3() != *ee &&
				rel_it->getOp4() != *ee &&
				rel_it->getOp5() != *ee )
				++rel_it;
			else
				rel_it = m_equivRelations.erase( rel_it);
		}
	}
	if( !correct)
		global_vars.err << MSG_ERROR << "Some objects/relations disregarded by the Proxy Checker.\n";

	return correct;
}


void Component::inheritancesManager( Relation & rel_it)
{
	Relation::OPER_TYPE relation[ ::FCO::NUMBER_OF_INHERITANCES ] = { 
		Relation::INHERITANCE_OP,
		Relation::INT_INHERITANCE_OP,
		Relation::IMP_INHERITANCE_OP 
	};

	::FCO::INHERITANCE_TYPE new_relation[ ::FCO::NUMBER_OF_INHERITANCES ] = { 
			::FCO::REGULAR,
			::FCO::INTERFACE,
			::FCO::IMPLEMENTATION
	};

	int inh_type; // FCO::INHERITANCE_TYPE
	for( inh_type = 0; inh_type != ::FCO::NUMBER_OF_INHERITANCES; ++inh_type )
	{
		if ( rel_it.getOperation() == relation[inh_type] )
		{
			ObjPointer obj1 = rel_it.getOp1();
			ObjPointer obj2 = rel_it.getOp2();

			::FCO* base = m_sheet->findFCO( rel_it.getOp1());
			::FCO* derived = m_sheet->findFCO( rel_it.getOp2());
			if ( base != 0 && derived != 0)
			{
				base->addChild( new_relation[inh_type], derived);
				derived->addParent( new_relation[inh_type], base);
			}
			else
			{
				if ( base == 0 )
					global_vars.err << MSG_ERROR << "No base peer present in inheritance relation: " << obj1 << " <|---- " << obj2 << "\n";
				if ( derived == 0 )
					global_vars.err << MSG_ERROR << "No derived peer present in inheritance relation: " << obj1 << " <|---- " << obj2 << "\n";
			} // if
		} // if
	} // for inheritance
}


void Component::associationClassManager( Relation & rel_it)
{
	if ( rel_it.getOperation() == Relation::ASSOCIATION_OP )
	{
		ObjPointer obj1 = rel_it.getOp1();
		ObjPointer obj2 = rel_it.getOp2();
		ObjPointer obj3 = rel_it.getOp3();
		// the obj4 points to the connection line between the source and the connector
		// the obj5 points to the connection line between the connector and destination
		BON::Connection obj4( rel_it.getOp4());
		BON::Connection obj5( rel_it.getOp5());

		::FCO* op1 = m_sheet->findFCO( rel_it.getOp1());
		::FCO* op2 = m_sheet->findFCO( rel_it.getOp2());
		::FCO* op3 = m_sheet->findFCO( rel_it.getOp3());

		if ( op1 != 0 && op2 != 0 && op3 != 0)
		{
			// asking the rolenames and cardinalities
			std::string src_role = "src", dst_role = "dst";
			std::string src_card = "0..*", dst_card = "0..*";

			src_card = obj4->getAttribute( "Cardinality")->getStringValue();
			dst_card = obj5->getAttribute( "Cardinality")->getStringValue();

			src_role = obj4->getAttribute( "srcRolename")->getStringValue();
			if ( src_role != "" && src_role != "src") 
				global_vars.err << MSG_ERROR << "Incorrect Rolename attribute: <" << src_role << "> of " << obj4 << "\n";

			dst_role = obj5->getAttribute( "dstRolename")->getStringValue();
			if ( dst_role != "" && dst_role != "dst") 
				global_vars.err << MSG_ERROR << "Incorrect Rolename attribute: <" << dst_role << "> of " << obj5 << "\n";


			ConnJoint::SDList list_op1, list_op2;
			list_op1.push_back( op1);
			list_op2.push_back( op2);

			if ( op3->getMyKind() != Any::CONN ) global_vars.err << MSG_ERROR << "How can happen that the association class is Not a Connection.\n";
			ConnectionRep * conn_obj = dynamic_cast< ConnectionRep * > ( op3 );

			ConnJoint assoc( conn_obj,
				list_op1, list_op2, src_role == dst_role,
				src_card, dst_card); 

			if (conn_obj) conn_obj->addJoint( assoc ); // op3 describes the association of op1 and op2
			else global_vars.err << MSG_ERROR << "After dynamic_cast - conn" << obj3 << "\n";
		}
		else if ( obj3 == BON::FCO() )
			global_vars.err << MSG_ERROR << "Association relation is not complete. Association class missing. Op1: " << obj1 << " Op2: " << obj2 << "\n";
		else 
			global_vars.err << MSG_ERROR << "Association relation is not complete. Some part missing. Op1: " << obj1 << " Op2: " << obj2 << " Association class: " << obj3 << "\n";
	} // if
}


void Component::setMemberManager( Relation & rel_it)
{
	if ( rel_it.getOperation() == Relation::SET_MEMBER_OP )
	{
		ObjPointer obj1 = rel_it.getOp1();
		ObjPointer obj2 = rel_it.getOp2();

		::FCO* set = m_sheet->findFCO( rel_it.getOp1());
		::FCO* member = m_sheet->findFCO( rel_it.getOp2());
		if ( set != 0 && member != 0)
		{
			if ( set->getMyKind() != Any::SET ) global_vars.err  << MSG_ERROR << "Internal error: Not a set on left hand side of set_member relation\n";
			SetRep * set_obj = dynamic_cast< SetRep * > ( set );
			if (set_obj) set_obj->addMember( member);
			else global_vars.err  << MSG_ERROR << "Internal error: After dynamic_cast - set" << obj1 << "\n";

		}
		else
		{
			if ( set == 0 )
				global_vars.err << MSG_ERROR << "No set peer present in set_member relation. " << obj1 << " missing.\n";
			if ( member == 0 )
				global_vars.err << MSG_ERROR << "No member peer present in set_member relation. " << obj2 << " missing.\n";
		} // if
	} // if
}


void Component::refersToManager( Relation & rel_it)
{
	if ( rel_it.getOperation() == Relation::REFER_TO_OP )
	{
		ObjPointer obj1 = rel_it.getOp1();
		ObjPointer obj2 = rel_it.getOp2();

		::FCO* elem = m_sheet->findFCO( rel_it.getOp1());
		::FCO* ref = m_sheet->findFCO( rel_it.getOp2());
		if ( elem != 0 && ref != 0)
		{
			ReferenceRep * ref_obj = dynamic_cast<ReferenceRep *>( ref);
			if ( !ref_obj || ref->getMyKind() != Any::REF)
				global_vars.err << MSG_ERROR << "Not reference " << obj1 << " referring\n";
			else
				ref_obj->addInitialReferee( elem);
			elem->addRefersToMe( ref_obj);
		}
		else
		{
			if ( elem == 0 )
				global_vars.err << MSG_ERROR << "No referee peer present in RefersTo relation. " << obj1 << " missing.\n";
			if ( ref == 0 )
				global_vars.err << MSG_ERROR << "No reference peer present in RefersTo relation. " << obj2 << " missing.\n";
		} // if
	} // if
}


void Component::containmentManager( Relation & rel_it)
{
	if ( rel_it.getOperation() == Relation::CONTAINMENT_OP )
	{
		ObjPointer obj1 = rel_it.getOp1();
		ObjPointer obj2 = rel_it.getOp2();
		BON::Connection obj3( rel_it.getOp3());

		::FCO* model = m_sheet->findFCO( rel_it.getOp1());
		::FCO* fco = m_sheet->findFCO( rel_it.getOp2());

		if ( model != 0 && fco != 0)
		{
			ModelRep * model_obj = dynamic_cast< ModelRep * > ( model);
			if (!model_obj || model->getMyKind() != Any::MODEL) 
				global_vars.err << MSG_ERROR << "ERROR: after dynamic_cast - containment - model is 0 / Not model " << obj1 << " contains an FCO\n";

			if ( !obj3) 
				global_vars.err << MSG_ERROR << "ERROR: Null containment line class - containmentManager\n";

			bool is_a_port = obj3->getAttribute("IsPort")->getBooleanValue();
			std::string comp_role = obj3->getAttribute("Rolename")->getStringValue();
			std::string card = obj3->getAttribute("Cardinality")->getStringValue();

			RoleRep role( 
				comp_role,
				fco,
				model_obj,
				is_a_port, 
				card,
				false, // inh flag
				false); // long form flag -> will be set later by initRoleNames

			// is prevented multiple insertion? - yes, though similar rolename should be checked
			// this listing of the connections handles a rolename (a containment line) only once !!!
			model_obj->addRole( fco, role);
			//fco->iAmPartOf( model_obj);
		}
		else
		{
			if ( model == 0 )
				global_vars.err << MSG_ERROR << "No model peer present in containment relation. " << obj1 << " missing.\n";
			if ( fco == 0 )
				global_vars.err << MSG_ERROR << "No fco peer present in containment relation. " << obj2 << " missing.\n";
		} // if
	} // if
}


void Component::folderContainmentManager( Relation & rel_it)
{
	if ( rel_it.getOperation() == Relation::FOLDER_CONTAINMENT_OP )
	{
		ObjPointer obj1 = rel_it.getOp1();
		ObjPointer obj2 = rel_it.getOp2();
		BON::Connection obj3( rel_it.getOp3());

		Any* folder = m_sheet->findAny( rel_it.getOp1());
		Any* any = m_sheet->findAny( rel_it.getOp2());
		if ( folder != 0 && any != 0)
		{
			if ( folder->getMyKind() != Any::FOLDER ) global_vars.err << MSG_ERROR << "Not a folder " << obj1 << " contains an FCO\n";

			FolderRep * folder_obj = dynamic_cast< FolderRep * > ( folder );
			if (!folder_obj) global_vars.err << MSG_ERROR << "ERROR after dynamic_cast - folder_containment - folder " << obj1 << " is 0\n";
			
			std::string card = "0..*";//obj3->getAttribute( "Cardinality")->getStringValue();

			if ( any->getMyKind() != Any::FOLDER) 
			{
				::FCO * fco = dynamic_cast< ::FCO *>( any);
				if ( !fco) global_vars.err << MSG_ERROR << "ERROR after dynamic cast - folder containment - fco " << obj2 << " is 0\n";
				else folder_obj->addFCO( fco, card);
			}
			else 
			{
				FolderRep * sub_f = dynamic_cast< FolderRep *>( any);
				if ( !sub_f) global_vars.err << MSG_ERROR << "ERROR after dynamic cast - folder containment - subfolder " << obj2 << " is 0\n";
				else folder_obj->addSubFolderRep( sub_f, card);
			}
		}
		else
		{
			if ( folder == 0 )
				global_vars.err << MSG_ERROR << "No folder peer present in folder_containment relation. " <<  obj1 << " missing.\n";
			if ( any == 0 )
				global_vars.err << MSG_ERROR << "No element peer present in folder_containment relation. " << obj2 << " missing.\n";
		} // if
	} // if
}


#if(NEEDED_FOR_METAINTERPRETER_ONLY)
void Component::hasAspectManager( Relation & rel_it)
{
	if ( rel_it.getOperation() == Relation::HAS_ASPECT_OP )
	{
		ObjPointer obj1 = rel_it.getOp1();
		ObjPointer obj2 = rel_it.getOp2();

		::FCO* model = m_sheet->findFCO( rel_it.getOp1());
		Any* aspect = m_sheet->findAny( rel_it.getOp2());
		if ( model != 0 && aspect != 0)
		{
			ModelRep * model_obj = dynamic_cast< ModelRep * > ( model );
			AspectRep * aspect_obj = dynamic_cast< AspectRep * > ( aspect );
			if (!aspect_obj) 
				global_vars.err << MSG_ERROR << "ERROR after dynamic_cast - has aspect - aspect " << obj2 << " is 0\n";
			if (!model_obj || !aspect_obj || model->getMyKind() != Any::MODEL ) 
				global_vars.err << MSG_ERROR << "ERROR after dynamic_cast " << (aspect_obj?"aspect is 0":(model_obj?"model is 0":"wrong kind")) << "\n";
			else
				model_obj->addAspect( aspect_obj); //adding the aspect_ptr to the model
		}
		else
		{
			if ( model == 0 )
				global_vars.err << MSG_ERROR << "No model peer present in has_aspect relation. " << obj1 << " missing.\n";
			if ( aspect == 0 )
				global_vars.err << MSG_ERROR << "No aspect peer present in has_aspect relation. " << obj2 << " missing.\n";
		} // if
	} // if
}


void Component::aspectMemberManager( Relation & rel_it)
{
	if ( rel_it.getOperation() == Relation::ASPECT_MEMBER_OP )
	{
		ObjPointer obj1 = rel_it.getOp1();
		ObjPointer obj2 = rel_it.getOp2();

		Any* aspect = m_sheet->findAny( rel_it.getOp1());
		::FCO* member = m_sheet->findFCO( rel_it.getOp2());
		if ( aspect != 0 && member != 0)
		{
			if ( aspect->getMyKind() != Any::ASPECT ) global_vars.err << MSG_ERROR << "Not an aspect " << obj1 << "\n";

			AspectRep * aspect_obj = dynamic_cast< AspectRep * > ( aspect );
			if ( !aspect_obj) global_vars.err << MSG_ERROR << "ERROR after dynamic_cast - aspect member - aspect " << obj1 << " is 0\n";

			if ( rel_it.getOp3() != BON::FCO() && 
				rel_it.getOp4() != BON::FCO()) // means a role is only the aspect member
			{
				std::string rolename = Connection( rel_it.getOp3())->getAttribute("Rolename")->getStringValue();
				bool is_a_port = Connection( rel_it.getOp3())->getAttribute("IsPort")->getBooleanValue();
				std::string card = Connection( rel_it.getOp3())->getAttribute("Cardinality")->getStringValue();

				ModelRep * model = dynamic_cast<ModelRep*>( m_sheet->findFCO( rel_it.getOp4())); // op4 is the container model
				if ( model != 0)
				{
					RoleRep role( 
						rolename,
						member,
						model,
						is_a_port, 
						card,
						false, // inh flag
						false); // long form flag -> will be set later by initRoleNames

					aspect_obj->addRRole( role);
				}
				else // model cannot be found //<!> needs testing
					global_vars.err << MSG_ERROR << "Interal error: No model present in aspect_member relation. " << rel_it.getOp4() << " missing.\n";
			}
			else // means regular aspect membership
				aspect_obj->addFCO( member);
		}
		else
		{
			if ( aspect == 0 )
				global_vars.err << MSG_ERROR << "No aspect peer present in aspect_member relation. " << obj1 << " missing.\n";
			if ( member == 0 )
				global_vars.err << MSG_ERROR << "No member peer present in aspect_member relation. " << obj2 << " missing.\n";
		} // if
	} // if
}


void Component::hasConstraintManager( Relation & rel_it)
{
	if ( rel_it.getOperation() == Relation::HAS_CONSTRAINT_OP )
	{
		ObjPointer obj1 = rel_it.getOp1();
		ObjPointer obj2 = rel_it.getOp2();

		Any* any = m_sheet->findAny( rel_it.getOp1());
		Any* constraint = m_sheet->findAny( rel_it.getOp2());
		if ( any != 0 && constraint != 0)
		{
			ConstraintRep *constraint_obj = dynamic_cast< ConstraintRep * >(constraint);
			if (any && (any->isFCO() || any->getMyKind() == Any::FOLDER)) 
			{
				any->addInitialConstraintRep( constraint_obj);
				constraint_obj->attachedTo();
			}
			else
				global_vars.err << MSG_ERROR << obj2 << " constraint is owned by " << obj1 << " which is neither FCO nor Folder\n";
		}
		else
		{
			if ( any == 0 )
				global_vars.err << MSG_ERROR << "No owner (FCO or Folder) peer present in has_constraint relation. " << obj1 << " missing.\n";
			if ( constraint == 0 )
				global_vars.err << MSG_ERROR << "No Constraint peer present in has_constraint relation. " << obj2 << " missing.\n";
		} // if
	} // if
}
#endif


void Component::hasAttributeManager( Relation & rel_it)
{
	if ( rel_it.getOperation() == Relation::HAS_ATTRIBUTE_OP )
	{
		ObjPointer obj1 = rel_it.getOp1();
		ObjPointer obj2 = rel_it.getOp2();

		::FCO* fco = m_sheet->findFCO( rel_it.getOp1());
		Any* attr = m_sheet->findAny( rel_it.getOp2());
		if ( fco != 0 && attr != 0)
		{
			AttributeRep * attr_obj = dynamic_cast< AttributeRep *>( attr);
			fco->addInitialAttribute( attr_obj);
			if ( !attr_obj->isGlobal())
				attr_obj->addOwner( fco);
		}
		else
		{
			if ( fco == 0 )
				global_vars.err << MSG_ERROR << "No fco peer present in has_attribute relation. " << obj1 << " missing.\n";
			if ( attr == 0 )
				global_vars.err << MSG_ERROR << "No attribute peer present in has_attribute relation. " << obj2 << " missing.\n";
		} // if
	} // if
}


/**
 * Does the hard work for the aspects also like: 
 * inserting into model, inquering the aspect members
 */
void Component::CHECK()
{
#ifdef _DEBUG
	Relation_Iterator rel_it = m_relations.begin();
	// check if all the m_relations have their correct number of operands
	for( ; rel_it != m_relations.end(); ++rel_it)
	{
		if (rel_it->getOperation() == Relation::ASSOCIATION_OP)
		{
			bool three_op = rel_it->getOp3() != BON::FCO();
			if( rel_it->getOp1() == BON::FCO() || 
				rel_it->getOp2() == BON::FCO() || 
				rel_it->getOp3() == BON::FCO() || 
				rel_it->getOp4() == BON::FCO() || 
				rel_it->getOp5() == BON::FCO() )
				global_vars.err << MSG_ERROR << "Internal error: Assocation has one operand 0\n";
			Entity ent1 = entitySearch( rel_it->getOp1());
			Entity ent2 = entitySearch( rel_it->getOp2());
			Entity ent3 = entitySearch( rel_it->getOp3());

			ObjPointer o1 = rel_it->getOp1();
				
			ObjPointer ptr, o2 = rel_it->getOp2();

			if ( ent1 == Entity( BON::Folder(), BON::FCO()) )
			{
				ptr = rel_it->getOp1();
				m_entities.push_back( Entity( BON::Folder(), rel_it->getOp1()));
				if ( ptr != BON::FCO())
					global_vars.err << "Internal error: Operand 1 of " << rel_it->getOperationStr() << " is not present in Entities\n" <<
						ptr->getName() << " : " << ptr->getObjectMeta().name() << " has been inserted";
				else
					global_vars.err << "Internal error: Operand 1 of " << rel_it->getOperationStr() << " is null\n";
			}
			if ( ent2 == Entity( BON::Folder(), BON::FCO()))
			{
				ptr = rel_it->getOp2();
				m_entities.push_back( Entity( BON::Folder(), rel_it->getOp2()));
				if ( ptr != BON::FCO())
					global_vars.err << "Internal error: Operand 2 of " << rel_it->getOperationStr() << " is not present in Entities\n" << 
						ptr->getName() << " : " << ptr->getObjectMeta().name() << " has been inserted";
				else
					global_vars.err << "Internal error: Operand 2 of " << rel_it->getOperationStr() << " is null\n";
			}
			if ( ent3 == Entity( BON::Folder(), BON::FCO()))
			{
				if ( three_op )
				{
					ptr = rel_it->getOp3();
					m_entities.push_back( Entity( BON::Folder(), rel_it->getOp3()));
					global_vars.err << "Internal error: Operand 3 of " << rel_it->getOperationStr() << " is not present in Entities\n" << 
						ptr->getName() << " : " << ptr->getObjectMeta().name() << " has been inserted";
				}
				else 
				{
					global_vars.err << "Internal error: Association without Connection class: " << o1->getName() << " o " << o2->getName() << "\n"; 
				}
			}
		}
		else
		{
			if( rel_it->getOp1() == BON::FCO() ||
				rel_it->getOp2() == BON::FCO() )
				global_vars.err << "Internal error: " << rel_it->getOperationStr() << " has one operand 0\n";

			Entity ent1 = entitySearch( rel_it->getOp1());
			Entity ent2 = entitySearch( rel_it->getOp2());
			ObjPointer ptr = BON::FCO();
			if ( ent1 == Entity( BON::Folder(), BON::FCO()))
			{
				ptr = rel_it->getOp1();
				m_entities.push_back( Entity( BON::Folder(), rel_it->getOp1()));
				if ( ptr != BON::FCO())
					global_vars.err << "Internal error: Operand 1 of " << rel_it->getOperationStr() << " is not present in Entities\n" << 
						ptr->getName() << " : " << ptr->getObjectMeta().name() << " has been inserted";
				else
					global_vars.err << "Internal error: Operand 1 of " << rel_it->getOperationStr() + " is null\n"; 
			}
			if ( ent2 == Entity( BON::Folder(), BON::FCO()))
			{
				ptr = rel_it->getOp2();
				m_entities.push_back( Entity( BON::Folder(), rel_it->getOp2()));
				ObjPointer ptr2 = rel_it->getOp1();
				std::string peer1, peer2;
				if ( ptr2 != BON::FCO())
					peer1 = ptr2->getName() + " " + ptr2->getObjectMeta().name();
				if ( rel_it->getOp2())
					peer2 = rel_it->getOp2()->getName() + " " + rel_it->getOp2()->getObjectMeta().name();
				global_vars.err << "Internal error: Operand 2 of " << rel_it->getOperationStr() << " is not present in Entities\n" <<
					ptr->getName() << " : " << ptr->getObjectMeta().name() << " has been inserted\n" << 
					"Operand1: " << peer1 << "\n" <<
					"Operand2: " << peer2 << "\n";
				
				// try to check the members of a set
				if (rel_it->getOperation() == Relation::HAS_ASPECT_OP )
				{
					// we have an aspect pointed by op2
					ObjPointer t_ptr = rel_it->getOp2();
					ObjPointer obj = t_ptr;
					//BON::Set set = dynamic_cast<BON::Set>(obj);
					BON::Set set = BON::Set( obj);
					if (!set) global_vars.err << "Internal error: during set manipulation\n";
					
					std::set<FCO> list = set->getMembers();
					std::set<FCO>::iterator list_it = list.begin();
					std::string mmm;
					while ( list_it != list.end())
					{
						FCO actual = *list_it;
						Relation rela( Relation::ASPECT_MEMBER_OP, t_ptr, actual);
						m_relations.push_back(rela);
						mmm += actual->getName() + "/" + actual->getObjectMeta().name() + "\n";
						++list_it;
					}
					global_vars.err << "Internal error: Aspect members are:\n" << mmm << "\n";
				}
			}
			if ( rel_it->getOperation() == Relation::CONTAINMENT_OP ||
				rel_it->getOperation() == Relation::FOLDER_CONTAINMENT_OP )
			{
				if (rel_it->getOp3() == BON::FCO())
					global_vars.err << "Internal error: (Folder)Containment line object not present in m_relations list\n";
			}
		} // end if
	} // end for

	Relation_Iterator rel_it1 = m_relations.begin();
	// check if all the relations have their correct number of operands
	for( ; rel_it1 != m_relations.end(); ++rel_it1)
	{
		Relation_Iterator rel_it2 = m_relations.begin();
		// check if all the relations have their correct number of operands
		for( ; rel_it2 != m_relations.end(); ++rel_it2)
		{
			if (rel_it1->getOperation() == Relation::HAS_ATTRIBUTE_OP &&
				rel_it2->getOperation() == Relation::HAS_ATTRIBUTE_OP )
			{
				//CHECK if the same attribute is contained twice by the FCO
				if ( rel_it1->getOp1() == rel_it2->getOp1() 
					&& rel_it1->getOp2() == rel_it2->getOp2() 
					&& rel_it1 != rel_it2)
				{
					ObjPointer o1 = rel_it1->getOp1();
						
					ObjPointer o2 = rel_it1->getOp2();
					global_vars.err << "Internal error: Same attribute " << o2->getName() <<
						" contained twice by " << o1->getName() << "\n";
				}
			}
		}
	}
#endif
}

}; // namespace BON
