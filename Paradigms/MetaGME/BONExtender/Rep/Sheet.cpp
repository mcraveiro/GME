#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"
#include "Sheet.h"
#include "logger.h"

#include "globals.h"
extern Globals global_vars;

Sheet * Sheet::m_theOnlyInstance = 0;

Sheet::Sheet()
	: m_projName(""),
	m_validProjName(""),
	m_projNamespace(""),
	m_validProjNamespace(""),
	m_rootFolder(),
	m_fcoRepList(),
	m_atomList(),
	m_modelList(),
	m_connList(),
	m_refList(),
	m_setList(),
	m_folderList(),
	m_attributeList()
{
	for( unsigned int i = 0; i < FCO::NUMBER_OF_INHERITANCES; ++i)
		m_numberOfCliques[ i ] = 0;
}


Sheet::~Sheet()
{
	for( unsigned int i = 0; i < FCO::NUMBER_OF_INHERITANCES; ++i)
		m_numberOfCliques[ i ] = 0;
	m_theOnlyInstance = 0;
	
	{
		FcoRep_Iterator it( m_fcoRepList.begin() );
		for( ; it != m_fcoRepList.end(); ++it)
		{
			FcoRep * act_ptr = * it;
			delete act_ptr;
		}
	}
	{
		AtomRep_Iterator it = m_atomList.begin();
		for( ; it != m_atomList.end(); ++it)
		{
			AtomRep * act_ptr = * it;
			delete act_ptr;
		}
	}
	{
		ModelRep_Iterator it = m_modelList.begin();
		for( ; it != m_modelList.end(); ++it)
		{
			ModelRep * act_ptr = * it;
			delete act_ptr;
		}
	}
	{
		ConnectionRep_Iterator it = m_connList.begin();
		for( ; it != m_connList.end(); ++it)
		{
			ConnectionRep * act_ptr = * it;
			delete act_ptr;
		}
	}
	{
		SetRep_Iterator it = m_setList.begin();
		for( ; it != m_setList.end(); ++it)
		{
			SetRep * act_ptr = * it;
			delete act_ptr;
		}
	}
	{
		ReferenceRep_Iterator it = m_refList.begin();
		for( ; it != m_refList.end(); ++it)
		{
			ReferenceRep * act_ptr = * it;
			delete act_ptr;
		}
	}
	{
		FolderRep_Iterator ai = m_folderList.begin();
		for( ; ai != m_folderList.end(); ++ai)
		{
			FolderRep * act_ptr = * ai;
			delete act_ptr;
		}
	}
	{
		AttributeRep_Iterator ai = m_attributeList.begin();
		for( ; ai != m_attributeList.end(); ++ai)
		{
			AttributeRep * act_ptr = * ai;
			delete act_ptr;
		}
	}
	m_fcoRepList.clear();
	m_atomList.clear();
	m_modelList.clear();
	m_connList.clear();
	m_refList.clear();
	m_setList.clear();

	m_folderList.clear();

	m_attributeList.clear();

	m_BON_Project_Root_Folder2 = BON::Folder();

	m_setOfParadigmSheets2.erase( m_setOfParadigmSheets2.begin(), m_setOfParadigmSheets2.end());
	m_setOfParadigmSheets2.clear();
}


void Sheet::setProject( BON::Project& project)
{
	m_validProjName = m_projName = project->getName();
	Any::convertToValidName( m_validProjName);

	BON::RegistryNode rn = project->getRootFolder()->getRegistry()->getChild( "Namespace");
	if( rn) m_validProjNamespace = m_projNamespace = rn->getValue();
	else    m_validProjNamespace = m_projNamespace = "";

	// convert the namespace specified (can be empty) to a valid identifier
	Any::convertToValidName( m_validProjNamespace);
}

/*virtual*/ bool Sheet::finalize()
{
	bool ret_val = false;
	if ( doInheritance( FCO::INTERFACE) 
		&& doInheritance( FCO::IMPLEMENTATION))
	{
		init();
		initRoleNames();

		multipleInheritance();
		ret_val = true;
	}
	else 
		global_vars.err << "Error: Inheritance problems\n";

	return ret_val;	
}

void Sheet::init()
{
	{FcoRep_Iterator it0( m_fcoRepList.begin());
	for( ; it0 != m_fcoRepList.end(); ++it0 )
	{
		(*it0)->initNamespace();
		(*it0)->initAttributes();
		if ( isInRootFolder( *it0)) m_rootFolder.addRootElement( *it0);
	}}

	{AtomRep_Iterator it1 = m_atomList.begin();
	for( ; it1 != m_atomList.end(); ++it1 )
	{
		(*it1)->initNamespace();
		(*it1)->initAttributes();
		if ( isInRootFolder( *it1)) m_rootFolder.addRootElement( *it1);
	}}

	{ModelRep_Iterator it2 = m_modelList.begin();
	for( ; it2 != m_modelList.end(); ++it2 )
	{
		(*it2)->initNamespace();
		(*it2)->initAttributes();
		if ( isInRootFolder( *it2)) m_rootFolder.addRootElement( *it2);
	}}

	{ConnectionRep_Iterator it3 = m_connList.begin();
	for( ; it3 != m_connList.end(); ++it3 )
	{
		(*it3)->initNamespace();
		(*it3)->initAttributes();
		if ( isInRootFolder( *it3)) m_rootFolder.addRootElement( *it3);
	}}

	{SetRep_Iterator it4 = m_setList.begin();
	for( ; it4 != m_setList.end(); ++it4 )
	{
		(*it4)->initNamespace();
		(*it4)->initAttributes();
		if ( isInRootFolder( *it4)) m_rootFolder.addRootElement( *it4);
	}}

	{ReferenceRep_Iterator it5 = m_refList.begin();
	for( ; it5 != m_refList.end(); ++it5 )
	{
		(*it5)->initNamespace();
		(*it5)->initAttributes();
		if ( isInRootFolder( *it5)) m_rootFolder.addRootElement( *it5);
	}}
	
	{FolderRep_Iterator it6 = m_folderList.begin();
	for( ; it6 != m_folderList.end(); ++it6 )
	{
		(*it6)->initNamespace();
		(*it6)->initAttributes();
		if ( isInRootFolder( *it6)) m_rootFolder.addSubFolder( *it6);
	}}

	{AttributeRep_Iterator it7 = m_attributeList.begin();
	for( ; it7 != m_attributeList.end(); ++it7 )
	{
		(*it7)->initNamespace();
	}}
}


void Sheet::initRoleNames()
{
	ModelRep_Iterator jt1( m_modelList.begin());
	for( ; jt1 != m_modelList.end(); ++jt1)
	{
		ModelRep * mod_ptr = *jt1;
		mod_ptr->initRoles();
	}
}


FcoRep* Sheet::createFcoRep( BON::FCO& ptr, BON::FCO& resp_ptr)
{
	FcoRep * ll = new FcoRep( ptr, resp_ptr);
	addFcoRep( ll);
	return ll;
}


AtomRep* Sheet::createAtomRep( BON::FCO& ptr, BON::FCO& resp_ptr)
{
	AtomRep * ll = new AtomRep( ptr, resp_ptr);
	addAtomRep( ll);
	return ll;
}


ModelRep* Sheet::createModelRep( BON::FCO& ptr, BON::FCO& resp_ptr)
{
	ModelRep * ll = new ModelRep( ptr, resp_ptr);
	addModelRep( ll);
	return ll;
}


ConnectionRep* Sheet::createConnectionRep( BON::FCO& ptr, BON::FCO& resp_ptr)
{
	ConnectionRep * ll = new ConnectionRep( ptr, resp_ptr);
	addConnectionRep( ll);
	return ll;
}


SetRep* Sheet::createSetRep( BON::FCO& ptr, BON::FCO& resp_ptr)
{
	SetRep * ll = new SetRep( ptr, resp_ptr);
	addSetRep( ll);
	return ll;
}


ReferenceRep* Sheet::createReferenceRep( BON::FCO& ptr, BON::FCO& resp_ptr)
{
	ReferenceRep * ll = new ReferenceRep( ptr, resp_ptr);
	addReferenceRep( ll);
	return ll;
}


FolderRep* Sheet::createFolderRep( BON::FCO& ptr, BON::FCO& resp_ptr)
{
	FolderRep * ll = new FolderRep( ptr, resp_ptr);
	addFolderRep( ll);
	return ll;
}


AttributeRep* Sheet::createBoolAttributeRep( BON::FCO& ptr)
{
	AttributeRep * ll = new BoolAttributeRep( ptr);
	addAttributeRep( ll);
	return ll;
}


AttributeRep* Sheet::createEnumAttributeRep( BON::FCO& ptr)
{
	AttributeRep * ll = new EnumAttributeRep( ptr);
	addAttributeRep( ll);
	return ll;
}


AttributeRep* Sheet::createFieldAttributeRep( BON::FCO& ptr)
{
	AttributeRep * ll = new FieldAttributeRep( ptr);
	addAttributeRep( ll);
	return ll;
}


void Sheet::addFcoRep( FcoRep * ptr)
{
	m_fcoRepList.push_back( ptr);
}


void Sheet::addAtomRep( AtomRep * ptr)
{
	m_atomList.push_back( ptr);
}


void Sheet::addModelRep( ModelRep * ptr)
{
	m_modelList.push_back( ptr);
}


void Sheet::addConnectionRep( ConnectionRep * ptr)
{
	m_connList.push_back( ptr);
}


void Sheet::addReferenceRep( ReferenceRep * ptr)
{
	m_refList.push_back( ptr);
}


void Sheet::addSetRep( SetRep * ptr)
{
	m_setList.push_back( ptr);
}


void Sheet::addFolderRep( FolderRep * ptr)
{
	m_folderList.push_back(ptr);
}


void Sheet::addAttributeRep( AttributeRep * ptr)
{
	m_attributeList.push_back(ptr);
}



/**
 * This method returns a pointer
 * Since the container consists of pointers to FCO
 * the iterator is a pointer to an FCO pointer
 * it has FCO** type
 * *it has FCO* type
 * the same refers to the methods below
 */
FCO* Sheet::findFCO( const BON::FCO& ptr)
{
	FcoRep_Iterator it0( m_fcoRepList.begin());
	for( ; it0 != m_fcoRepList.end(); ++it0 )
	{
		if ((*it0)->getPtr() == ptr) return *it0;
	}

	AtomRep_Iterator it1 = m_atomList.begin();
	for( ; it1 != m_atomList.end(); ++it1 )
	{
		if ((*it1)->getPtr() == ptr) return *it1;
	}

	ModelRep_Iterator it2 = m_modelList.begin();
	for( ; it2 != m_modelList.end(); ++it2 )
	{
		if ((*it2)->getPtr() == ptr) return *it2;
	}

	ConnectionRep_Iterator it3 = m_connList.begin();
	for( ; it3 != m_connList.end(); ++it3 )
	{
		if ((*it3)->getPtr() == ptr) return *it3;
	}

	SetRep_Iterator it4 = m_setList.begin();
	for( ; it4 != m_setList.end(); ++it4 )
	{
		if ((*it4)->getPtr() == ptr) return *it4;
	}

	ReferenceRep_Iterator it5 = m_refList.begin();
	for( ; it5 != m_refList.end(); ++it5 )
	{
		if ((*it5)->getPtr() == ptr) return *it5;
	}
	return 0;
}


ReferenceRep* Sheet::findRef( const BON::FCO& ptr)
{
	ReferenceRep_Iterator it( m_refList.begin());
	for( ; it != m_refList.end(); ++it )
	{
		if ((*it)->getPtr() == ptr) return *it;
	}
	return 0;

}


Any* Sheet::findAny( const BON::FCO& ptr)
{
	FolderRep_Iterator it5 = m_folderList.begin();
	for( ; it5 != m_folderList.end(); ++it5 )
	{
		if ((*it5)->getPtr() == ptr) return *it5;
	}

	AttributeRep_Iterator it4 = m_attributeList.begin();
	for( ; it4 != m_attributeList.end(); ++it4 )
	{
		if ((*it4)->getPtr() == ptr) return *it4;
	}

	return findFCO( ptr);
}


bool Sheet::isInRootFolder( Any * elem)
{
	return elem->isInRootFolder();
}


void Sheet::gatherNodes( NODE_VECTOR & nodes)
{
	nodes.insert( nodes.end(), m_fcoRepList.begin(), m_fcoRepList.end());
	nodes.insert( nodes.end(), m_atomList.begin(), m_atomList.end());
	nodes.insert( nodes.end(), m_modelList.begin(), m_modelList.end());
	nodes.insert( nodes.end(), m_connList.begin(), m_connList.end());
	nodes.insert( nodes.end(), m_setList.begin(), m_setList.end());
	nodes.insert( nodes.end(), m_refList.begin(), m_refList.end());

}


void Sheet::assignCliqueId( CLIQUE_VECTOR & clique_id, const NODE_VECTOR & nodes)
{
	unsigned int cl_id = 0;
	unsigned int len = nodes.size();

	for(unsigned int i = 0 ; i < len; ++i)
		clique_id.push_back(i+1); // assign a clique id to each element (starting from 1)
}


void Sheet::replaceCliqueId( CLIQUE_VECTOR & clique_id, unsigned int id1, unsigned int id2)
{
	unsigned int search_id = id1, replace_id = id2;
	if ( replace_id > search_id) // the replace value should be lower than the replacable
	{
		unsigned int storage = replace_id;
		replace_id = search_id;
		search_id = storage;
	}

	for (unsigned int k = 0; k < clique_id.size(); ++k)
		if ( clique_id[k] == search_id)
			clique_id[k] = replace_id;
}


unsigned int Sheet::howManyCliques( CLIQUE_VECTOR & clique_id)
{
	std::map<unsigned int,unsigned int> t_m;
	for (unsigned int k = 0; k < clique_id.size(); ++k)
	{
		unsigned int curr_id = clique_id[k];
		++t_m[curr_id];
	}
	unsigned int current_clique_id_max = 1;
	unsigned int val = 0;
	std::map<unsigned int,unsigned int>::iterator i = t_m.begin();
	for( ; i != t_m.end(); ++i)
	{
		// to count how many clique id's are used (there we have keys in the map)
		++val;

		// since map is ordered based on the keys ( = clique ids) we can
		// replace the clique ids with as low clique id as we want if
		// we start from the lower keys
		// old clique series may look like: 1, 2, 7, 41
		unsigned int old_id = i->first;
		if ( old_id != current_clique_id_max) // no need to replace the old values with the same value
			for (unsigned int j = 0; j < clique_id.size(); ++j)
				if ( clique_id[j] == old_id)
					clique_id[j] = current_clique_id_max;

		++current_clique_id_max;
	}
	// now the clique_ids will be sequential: 1,2,3,...
	return val;	
}


void Sheet::getChildren( unsigned int base, const NODE_VECTOR & nodes, UI_LIST & children, FCO::INHERITANCE_TYPE inh_type)
{
	for (unsigned int i = 0; i < nodes.size(); ++i)
		if ( edge(i, base, nodes, inh_type)) 
		{
			unsigned int j = 0;
			while( j < children.size() && children[j] != i)
				++j;
			if ( j == children.size()) 
				children.push_back( i);
		}
}

bool Sheet::multipleInheritance()
{
	FcoRep_Iterator it0( m_fcoRepList.begin());
	for( ; it0 != m_fcoRepList.end(); ++it0 )
	{
		(*it0)->multipleInheritanceStep1();
		(*it0)->multipleInheritanceStep2();
	}

	AtomRep_Iterator it1 = m_atomList.begin();
	for( ; it1 != m_atomList.end(); ++it1 )
	{
		(*it1)->multipleInheritanceStep1();
		(*it1)->multipleInheritanceStep2();
	}

	ModelRep_Iterator it2 = m_modelList.begin();
	for( ; it2 != m_modelList.end(); ++it2 )
	{
		(*it2)->multipleInheritanceStep1();
		(*it2)->multipleInheritanceStep2();
	}

	ConnectionRep_Iterator it3 = m_connList.begin();
	for( ; it3 != m_connList.end(); ++it3 )
	{
		(*it3)->multipleInheritanceStep1();
		(*it3)->multipleInheritanceStep2();
	}

	SetRep_Iterator it4 = m_setList.begin();
	for( ; it4 != m_setList.end(); ++it4 )
	{
		(*it4)->multipleInheritanceStep1();
		(*it4)->multipleInheritanceStep2();
	}

	ReferenceRep_Iterator it5 = m_refList.begin();
	for( ; it5 != m_refList.end(); ++it5 )
	{
		(*it5)->multipleInheritanceStep1();
		(*it5)->multipleInheritanceStep2();
	}
	return true;
}


void Sheet::vectorCopy( UI_LIST & target, const UI_LIST & source, UI_LIST & duplicates)
{
	for(unsigned int i = 0; i < source.size(); ++i)
	{
		unsigned int j = 0;
		while( j != target.size() && target[j] != source[i])
			++j;
		if ( j == target.size())
			target.push_back( source[i]);
		else
			duplicates.push_back( source[i]);
	}
}


bool Sheet::edge( unsigned int i, unsigned int j, const NODE_VECTOR & nodes, FCO::INHERITANCE_TYPE inh_type)
{
	return nodes[i]->hasParent( nodes[j], inh_type);
}


bool Sheet::doInheritance( FCO::INHERITANCE_TYPE inh_type)
{
	unsigned int i, j;
	std::map<unsigned int, unsigned int> depends_upon;
	CLIQUE_VECTOR clique_vec;
	NODE_VECTOR node_vec;

	gatherNodes( node_vec);

	assignCliqueId( clique_vec, node_vec);

	for( i = 0; i < node_vec.size(); ++i)
		for ( j = 0; j < node_vec.size(); ++j)
		{
			if ( edge(i,j, node_vec, inh_type))
			{
				++depends_upon[i];
				// replace all clique_id[j] clique ids with clique_id[i]
				if ( clique_vec[j] != clique_vec[i])
					replaceCliqueId( clique_vec, clique_vec[j], clique_vec[i]);
			}
		}
	// initially will contain the roots
	std::list<unsigned int> elements, elements_backup;
	std::vector<int> level( node_vec.size());
	for( int reset_i = 0; reset_i < node_vec.size(); ++reset_i)
		level[reset_i] = -1;

	
	std::vector<unsigned int> dupl_temp;

	m_numberOfCliques[ inh_type] = howManyCliques( clique_vec);

	for( i = 0; i < node_vec.size(); ++i)
		if (depends_upon[i]==0)
		{
			elements.push_back(i);
			level[i] = 0; // the roots are placed on "level 0"
		}

	if (elements.size() < m_numberOfCliques[ inh_type]) // because from each clique there must be at least 1 root
	{
		TO("Circle found in model");
		global_vars.err << "Circle found in model\n";
		//std::cout << "Circle\n";
		// based on that if in a clique there is no node with outgoing degree of 0
		// then that (directed) clique contains a (directed) circle
		return false;
	}

	std::map< unsigned int, std::vector<unsigned int> > ancestors;
	std::map< unsigned int, std::vector<unsigned int> > descendants;

	bool loop = false;
	
	elements_backup = elements;

	while ( !elements.empty() && !loop)
	{
		// taking new descendants of current elements in the list
		unsigned int base = *elements.begin();
		elements.pop_front();

		// children of base
		std::vector<unsigned int> children;
		getChildren( base, node_vec, children, inh_type);
		for( j = 0; j < children.size(); ++j)
		{
			unsigned int child = children[j];
			elements.push_back( child);

			if (ancestors[child].end() == 
					std::find(ancestors[child].begin(), ancestors[child].end(), base))
				ancestors[child].push_back(base); // insert if not present

			vectorCopy( ancestors[child], ancestors[base], dupl_temp);
			for ( unsigned int k = 0; k < ancestors[child].size(); ++k)
				if ( edge( ancestors[child][k], child, node_vec, inh_type))
				{
					FCO * fco1 = node_vec[ancestors[child][k]];
					FCO * fco2 = node_vec[child];
					TO("Loop between " + fco1->getName() + " and " + fco2->getName());
					global_vars.err << "Loop between " << fco1->getName() << " and " << fco2->getName() << "\n";
					//std::cout << "Loop between " << ancestors[child][k] << " and " << child << "\n";
					loop = true;

					return false;
				}
		}
	}

	/*Level calculation*/
	// this cycle will determine the level elements are staying on
	loop = false;
	while ( !elements_backup.empty() && !loop)
	{
		// taking new descendants of current elements in the list
		unsigned int base = *elements_backup.begin(); 
		elements_backup.pop_front();
	
		// children of base
		std::vector<unsigned int> children;
		getChildren( base, node_vec, children, inh_type);
		for( j = 0; j < children.size(); ++j)
		{
			unsigned int child = children[j];
			elements_backup.push_back( child);

			/*if ( level[child] == -1) level[child] = level[base] + 1;
			else*/ if ( level[child] < level[base] + 1) level[child] = level[base] + 1;
		}
	}

	// using the ancestors database build up the m_desc and m_anc fields of each fco
	//for( i = 0; i < elements.size(); ++i)
	std::map< unsigned int, std::vector<unsigned int> >::iterator anc_it = ancestors.begin();
	for( ; anc_it != ancestors.end(); ++anc_it)
	{
		unsigned int id = anc_it->first;
		std::vector<unsigned int> & ancestors_of_id = anc_it->second;
		
		//for ( unsigned int k = 0; k < ancestors[i].size(); ++k)
		for( unsigned int k = 0; k < ancestors_of_id.size(); ++k)
		{
			// ancestor[i][k] is ancestor of i
			//std::string m1 = node_vec[ancestors_of_id[k]]->getName() + " anc of " + node_vec[id]->getName();
			//TO( m1);
			
			/*unsigned int l = 0;
			while( l < descendants[ancestors_of_id[k]].size() && descendants[ancestors_of_id[k]][l] != id)
				++l;
			if ( l == descendants[ancestors_of_id[k]].size())*/
			if ( std::find( descendants[ancestors_of_id[k]].begin(), descendants[ancestors_of_id[k]].end(), id) ==
				descendants[ancestors_of_id[k]].end()) // not found then insert
			{
				descendants[ancestors_of_id[k]].push_back(id);
				//std::string m1 = node_vec[ancestors_of_id[k]]->getName() + " has desc " + node_vec[id]->getName();
			//TO( m1);
			}
		}
	}

	//for( i = 0; i < elements.size(); ++i)
	for( i = 0; i < node_vec.size(); ++i)
	{
		//std::string mmma, mmmd, mmm;
		std::vector< FCO*> anc_list, desc_list;
		for ( unsigned int k = 0; k < ancestors[i].size(); ++k)
		{
			anc_list.push_back( node_vec[ancestors[i][k]]);
			//mmma += node_vec[ancestors[i][k]]->getName() + " ";
		}

		for ( unsigned int l = 0; l < descendants[i].size(); ++l)
		{
			desc_list.push_back( node_vec[descendants[i][l]]);
			//mmmd += node_vec[descendants[i][l]]->getName() + " ";
		}
		//mmm = node_vec[i]->getName() + std::string( (inh_type==FCO::INTERFACE)?" INTERFACE":(inh_type==FCO::IMPLEMENTATION)?" IMPLEMENTATION":" OTHER");
		//TO ( mmm + "\nanc:\n" + mmma + "\ndesc:\n" + mmmd);
		node_vec[i]->setAncestors( inh_type, anc_list);
		node_vec[i]->setDescendants( inh_type, desc_list);
		node_vec[i]->setLevel( inh_type, level[i]);
		node_vec[i]->setCliqueId( inh_type, clique_vec[i]);
	}
	return true;
}

std::vector< FCO *> Sheet::sortBasedOnLevels()
{
	// since the regular inheritance is splitted to interface and implementation inheritance and
	// inheritance loops are possible if these two are combined
	// this ordering process may not be successful
	//FCO::INHERITANCE_TYPE curr_type = INTERFACE;
	std::vector< FCO *> res;
	order( FCO::INTERFACE, res);
	bool good = check( FCO::IMPLEMENTATION, res);
	if (!good)
	{
		res.clear(); // start from the beginning
		order( FCO::IMPLEMENTATION, res);
		good = check( FCO::INTERFACE, res);
		if (!good) global_vars.err << "Cannot establish an order which would be good for both IMPLEMENTATION and INTERFACE hierarchy\n";
	}
	return res;
}


void Sheet::order( FCO::INHERITANCE_TYPE type, std::vector<FCO *>& res)
{
	// ordering of cliques based on how many models they contain
	// the key = clique_id is unique
	// the value = how many models the clique contains
	std::map< unsigned int, unsigned int> how_many_models_in_clique;

	// important init step: there will be cliques with no models, so the following step
	// will overwrite the values for the cliques (the keys) the models are part of
	for( int clique_index0 = 1; clique_index0 <= m_numberOfCliques[ type]; ++clique_index0)
		how_many_models_in_clique[ clique_index0] = 0;

	ModelRep_Iterator itm = m_modelList.begin();
	for( ; itm != m_modelList.end(); ++itm )
		++how_many_models_in_clique[ (*itm)->getCliqueId( type)];

	// a map containing the number of models as a key, and the list of cliqueid-s 
	// containing that amount of models
	std::map<unsigned int, std::list<unsigned int> > how_many_inverse;
	std::map<unsigned int,unsigned int>::iterator hmmic_ind = how_many_models_in_clique.begin();
	for( ; hmmic_ind != how_many_models_in_clique.end(); ++hmmic_ind)
		how_many_inverse[ hmmic_ind->second].push_back( hmmic_ind->first);
	// we have and ascending order of cliques based on the number of models contained

	std::map<unsigned int, std::list<unsigned int> >::reverse_iterator hminv_ind = how_many_inverse.rbegin();
	//for( int clique_index = 1; clique_index <= m_numberOfCliques; ++clique_index)
	for( ; hminv_ind != how_many_inverse.rend(); ++hminv_ind)
	{
		std::list<unsigned int>::iterator cliquelist_ind = hminv_ind->second.begin();
		for( ; cliquelist_ind != hminv_ind->second.end(); ++cliquelist_ind)
		{
			unsigned int clique_index = *cliquelist_ind;
			int curr_level = 0;
			bool more = true;
			while ( more)
			{
				int len_before = res.size();
				{
					ModelRep_Iterator it2 = m_modelList.begin();
					for( ; it2 != m_modelList.end(); ++it2 )
						if ( (*it2)->getCliqueId( type) == clique_index && (*it2)->getLevel( type) == curr_level)
							res.push_back( *it2);
				}
				{
					FcoRep_Iterator it0( m_fcoRepList.begin());
					for( ; it0 != m_fcoRepList.end(); ++it0 )
						if ( (*it0)->getCliqueId( type) == clique_index && (*it0)->getLevel( type) == curr_level)
							res.push_back( *it0);
				}
				{
					AtomRep_Iterator it1 = m_atomList.begin();
					for( ; it1 != m_atomList.end(); ++it1 )
						if ( (*it1)->getCliqueId( type) == clique_index && (*it1)->getLevel( type) == curr_level)
							res.push_back( *it1);
				}
				{
					ReferenceRep_Iterator it5 = m_refList.begin();
					for( ; it5 != m_refList.end(); ++it5 )
						if ( (*it5)->getCliqueId( type) == clique_index && (*it5)->getLevel( type) == curr_level)
							res.push_back( *it5);
				}
				{
					ConnectionRep_Iterator it3 = m_connList.begin();
					for( ; it3 != m_connList.end(); ++it3 )
						if ( (*it3)->getCliqueId( type) == clique_index && (*it3)->getLevel( type) == curr_level)
							res.push_back( *it3);
				}
				{
					SetRep_Iterator it4 = m_setList.begin();
					for( ; it4 != m_setList.end(); ++it4 )
						if ( (*it4)->getCliqueId( type) == clique_index && (*it4)->getLevel( type) == curr_level)
							res.push_back( *it4);
				}

				++curr_level;
				more = ( len_before != res.size());
			}
		}
	}
}


bool Sheet::check( FCO::INHERITANCE_TYPE type, std::vector<FCO*>& res)
{
	// it will store the currently reached maximum level (the lowest level in
	// the inheritance tree) per each clique
	std::map< int, int> m_maxLevelPerClique;

	std::vector<FCO*>::iterator it = res.begin();
	for( ; it != res.end(); ++it)
	{
		int curr_level = (*it)->getLevel( type);
		int curr_clique = (*it)->getCliqueId( type);

		if( m_maxLevelPerClique.find( curr_clique) == m_maxLevelPerClique.end()) // not existing yet
			m_maxLevelPerClique[ curr_clique ] = curr_level;
		else // check if the current max level is less then the level found at *it
		{
			if( curr_level < m_maxLevelPerClique[ curr_clique]) // an ancestor is after a descendant in the ordering
				return false; // not a good ordering from 'type' point of view
		}
	}
	return true;
}
