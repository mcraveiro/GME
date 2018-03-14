#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"
#include "Sheet.h"
#include "logger.h"
#include <algorithm>
#include "globals.h"
extern Globals global_vars;

Sheet * Sheet::m_theOnlyInstance = 0;
/*static*/ BON::Folder Sheet::m_BON_Project_Root_Folder = BON::Folder(); // important: clear its content upon destruction
Sheet::Sheet()
: m_projName(""),
	m_projVersion(),
	m_projGUID(),
	m_projCreated(),
	m_projModified(),
	m_projAuthor(),
	m_projComment(),
	m_projNamespace(),
	m_rootFolder(),
	m_fcoRepList(),
	m_atomList(),
	m_modelList(),
	m_connList(),
	m_refList(),
	m_setList(),
	m_folderList(),
	m_aspectList(),
	m_constraintList(),
	m_constraintFuncList(),
	m_attributeList()
{
}


Sheet::~Sheet()
{
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
		AspectRep_Iterator ai = m_aspectList.begin();
		for( ; ai != m_aspectList.end(); ++ai)
		{
			AspectRep * act_ptr = * ai;
			delete act_ptr;
		}
	}
	{
		ConstraintRep_Iterator ai = m_constraintList.begin();
		for( ; ai != m_constraintList.end(); ++ai)
		{
			ConstraintRep * act_ptr = * ai;
			delete act_ptr;
		}
	}
	{
		ConstraintFuncRep_Iterator ai = m_constraintFuncList.begin();
		for( ; ai != m_constraintFuncList.end(); ++ai)
		{
			ConstraintFuncRep * act_ptr = * ai;
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

	m_aspectList.clear();
	m_constraintList.clear();
	m_constraintFuncList.clear();

	m_attributeList.clear();
	Sheet::m_BON_Project_Root_Folder = BON::Folder();
}


void Sheet::setProject( BON::Project& project)
{
	m_projName = project->getName();
	m_projVersion = project->getVersion();
	m_projGUID = project->getGUID();
	m_projCreated = project->getCreationTime();
	m_projModified = project->getChangeTime();
	m_projAuthor = project->getAuthor();
	m_projComment = project->getComment();
	
	BON::RegistryNode rn = project->getRootFolder()->getRegistry()->getChild( "Namespace");
	if( rn) m_projNamespace = rn->getValue();
	else    m_projNamespace = "";

	// check the paradigm's name, containing spaces, other special chars, excluding '.' and '_'
	std::string new_name;
	if( !Sheet::makeValidParadigmName( m_projName, new_name))
	{
		global_vars.err << MSG_ERROR << "Error: Incorrect paradigm name: \"" << m_projName << "\". Use only alphanumerics, '.' and '_'. No spaces please. Using modified name: \"" << new_name << "\".\n";
		m_projName = new_name;
	}

	// check the namespace specified (can be empty)
	new_name = "";
	if( !Sheet::makeValidNamespace( m_projNamespace, new_name))
	{
		global_vars.err << MSG_ERROR << "Error: Incorrect namespace specified: \"" << m_projNamespace << "\". Use only alphanumerics and '_'. Using modified name: \"" << new_name << "\".\n";
		m_projNamespace = new_name;
	}
}

/*virtual*/ bool Sheet::finalize()
{
	bool ret_val = false;
	if ( doInheritance( FCO::INTERFACE) 
		&& doInheritance( FCO::IMPLEMENTATION))
	{
		init();
		initRoleNames();
		ret_val = true;
	}
	else 
		global_vars.err << MSG_ERROR << "Error: Inheritance problems\n";

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

	{ConstraintFuncRep_Iterator it8 = m_constraintFuncList.begin();
	for( ; it8 != m_constraintFuncList.end(); ++it8 )
	{
		(*it8)->initNamespace();
	}}

	{ConstraintRep_Iterator it9 = m_constraintList.begin();
	for( ; it9 != m_constraintList.end(); ++it9 )
	{
		(*it9)->initNamespace();
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


AspectRep* Sheet::createAspectRep( BON::FCO& ptr, BON::FCO& resp_ptr)
{
	AspectRep * ll = new AspectRep( ptr, resp_ptr);
	addAspectRep( ll);
	return ll;
}


ConstraintRep* Sheet::createConstraintRep( BON::FCO& ptr)
{
	ConstraintRep * ll = new ConstraintRep( ptr);
	addConstraintRep( ll);
	return ll;
}


ConstraintFuncRep* Sheet::createConstraintFuncRep( BON::FCO& ptr)
{
	ConstraintFuncRep * ll = new ConstraintFuncRep( ptr);
	addConstraintFuncRep( ll);
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


void Sheet::addAspectRep( AspectRep * ptr)
{
	m_aspectList.push_back( ptr);
}


void Sheet::addFolderRep( FolderRep * ptr)
{
	m_folderList.push_back(ptr);
}


void Sheet::addConstraintRep( ConstraintRep * ptr)
{
	m_constraintList.push_back( ptr);
}


void Sheet::addConstraintFuncRep( ConstraintFuncRep * ptr)
{
	m_constraintFuncList.push_back( ptr);
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

	AspectRep_Iterator it1( m_aspectList.begin());
	for( ; it1 != m_aspectList.end(); ++it1 )
	{
		if ((*it1)->getPtr() == ptr) return *it1;
	}

	ConstraintRep_Iterator it2 = m_constraintList.begin();
	for( ; it2 != m_constraintList.end(); ++it2 )
	{
		if ((*it2)->getPtr() == ptr) return *it2;
	}

	ConstraintFuncRep_Iterator it3 = m_constraintFuncList.begin();
	for( ; it3 != m_constraintFuncList.end(); ++it3 )
	{
		if ((*it3)->getPtr() == ptr) return *it3;
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
		clique_id.push_back(i+1);

}


void Sheet::replaceCliqueId( CLIQUE_VECTOR & clique_id, unsigned int id1, unsigned int id2)
{
	unsigned int search_id = id1, replace_id = id2;

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
	unsigned int val = 0;
	std::map<unsigned int,unsigned int>::iterator i = t_m.begin();
	for( ; i != t_m.end(); ++i)
	{
		++val;
	}
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
	std::list<unsigned int> elements;
	std::vector<unsigned int> dupl_temp;

	unsigned int no_of_cliques = howManyCliques( clique_vec);

	for( i = 0; i < node_vec.size(); ++i)
		if (depends_upon[i]==0)
			elements.push_back(i);

	if (elements.size() < no_of_cliques) // because from each clique there must be at least 1 root
	{
		TO("Inheritance loop found in model!!! Please correct it!\n");
		global_vars.err << MSG_ERROR << "Inheritance loop found in model!!! Please correct it!\n";
		//std::cout << "Circle\n";
		// based on that if in a clique there is no node with outgoing degree of 0
		// then that (directed) clique contains a (directed) circle
		return false;
	}

	std::map< unsigned int, std::vector<unsigned int> > ancestors;
	std::map< unsigned int, std::vector<unsigned int> > descendants;

	bool loop = false;
	
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
					global_vars.err << MSG_ERROR << "Loop between " << fco1 << " and " << fco2 << "\n";
					//std::cout << "Loop between " << ancestors[child][k] << " and " << child << "\n";
					loop = true;

					return false;
				}
		}
	}
	
	// using the ancestors database build up the m_desc and m_anc fields of each fco
	std::map< unsigned int, std::vector<unsigned int> >::iterator anc_it = ancestors.begin();
	for( ; anc_it != ancestors.end(); ++anc_it)
	{
		unsigned int id = anc_it->first;
		std::vector<unsigned int> & ancestors_of_id = anc_it->second;
		
		for( unsigned int k = 0; k < ancestors_of_id.size(); ++k)
		{
			// ancestor[i][k] is ancestor of i
			//std::string m1 = node_vec[ancestors_of_id[k]]->getName() + " anc of " + node_vec[id]->getName();
			//TO( m1);
			
			if ( std::find( descendants[ancestors_of_id[k]].begin(), descendants[ancestors_of_id[k]].end(), id) ==
				descendants[ancestors_of_id[k]].end()) // not found then insert
			{
				descendants[ancestors_of_id[k]].push_back(id);
				std::string m1 = node_vec[ancestors_of_id[k]]->getName() + " has desc " + node_vec[id]->getName();
			//TO( m1);
			}
		}
	}

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
	}
	return true;
}

/*static*/ bool Sheet::makeValidParadigmName( const std::string& pInName, std::string& res)
{
	res = pInName;
	bool inv(res.length() == 0); // invalid name?, (if empty then invalid)
	// check the paradigm's name, containing spaces, other special chars, except '.', '_'
	for( unsigned int i = 0; i < res.length(); ++i)
	{
		bool v = res[i] >= 'a' && res[i] <= 'z' ||
			res[i] >= 'A' && res[i] <= 'Z' ||
			res[i] >= '0' && res[i] <= '9' ||
			res[i] == '.' || 
			res[i] >= '_';

		if( !v)
		{
			res[i] = '_';
			inv = true;
		}
	}
	return !inv;
}

/*static*/ bool Sheet::makeValidNamespace( const std::string& pInName, std::string& res)
{
	res = pInName;
	bool inv = false;
	for( unsigned int i = 0; i < res.length(); ++i)
	{
		bool v = res[i] >= 'a' && res[i] <= 'z'
			||   res[i] >= 'A' && res[i] <= 'Z'
			||   res[i] >= '0' && res[i] <= '9' && i >= 1 // first char can't be a digit
			||   res[i] == '_';

		if( !v)
		{
			res[i] = '_';
			inv = true;
		}
	}
	return !inv;
}