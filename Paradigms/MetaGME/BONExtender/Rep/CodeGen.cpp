#include "stdafx.h"
#include "CodeGen.h"
#include "Dumper.h"
#include "MakeVisitor.h"


/*const*/ int h_ind = 1;
/*const*/ int s_ind = 0;

/*static*/ std::string CodeGen::indent( int tabs /* = 0*/)
{
	return std::string( (unsigned) tabs, '\t');
}


/*static*/ std::string CodeGen::fill( int fill /* = 1*/)
{
	return std::string( (unsigned) fill, ' ');
}


/**
 * Folder related methods
 */

/*static*/ void CodeGen::dumpFoldGetter( FolderRep* sub, FolderRep *cont)
{
	std::string sub_nmsp( sub->getValidNmspc()), fol_nmsp( cont->getValidNmspc()), diff_nmsp( "");
	bool is_diff_nmsp = sub_nmsp != fol_nmsp;
	if( is_diff_nmsp) diff_nmsp = sub_nmsp;

	std::string method_name = FolderRep::subFolderGetterMethodName( sub, diff_nmsp);
	std::string retval_folderkind( sub->getLValidName()), folderkind( sub->getLStrictName());
	//retval_folderkind = folderkind = sub->getValidName();
	if ( !sub->isToBeEx())
		retval_folderkind = "BON::Folder";

	if ( Dumper::m_bGenRegular)
	{
		Method m;

		folderGetter( retval_folderkind, folderkind, method_name, sub->isToBeEx(), cont, m);

		cont->addMethod( m);
	}

	
	if ( Dumper::m_bGenTemplates)
	{
		Method m;

		folderGetterGeneric( retval_folderkind, folderkind, method_name, sub->isToBeEx(), cont, m);

		cont->addMethod( m);
	}
}

// used by the Folder only
/*static*/ void CodeGen::dumpKindGetter( FCO* fco, FolderRep * cont)
{
	std::string fco_nmsp( fco->getValidNmspc()), fol_nmsp( cont->getValidNmspc()), diff_nmsp( "");
	bool is_diff_nmsp = fco_nmsp != fol_nmsp;
	if( is_diff_nmsp) diff_nmsp = fco_nmsp;

	std::string method_name = FolderRep::kindGetterMethodName( fco, diff_nmsp);
	
	std::string retval_kind( fco->getLValidName()), kind( fco->getLStrictName());
	//retval_kind = kind = fco->getValidName();

	if (!fco->isToBeEx())
		retval_kind = "BON::FCO";

	std::string src, comm;
	
	std::vector<FCO*> r;
	fco->getIntDescendants( r);
	if ( r.empty())
	{
		if (!fco->isAbstract())
		{
			if ( Dumper::m_bGenRegular)
			{
				Method m;

				kindGetter1( retval_kind, kind, method_name, cont, m);

				cont->addMethod( m);
			}

			
			if ( Dumper::m_bGenTemplates)
			{
				Method m;

				kindGetter1Generic( retval_kind, kind, method_name, cont, m);

				cont->addMethod( m);
			}
		}
	}
	else // there are descendants
	{
		r.push_back( fco);
		
		std::vector< std::string > kind_vec;
		
		std::vector<FCO*>::iterator it = r.begin();
		for( ; it != r.end(); ++it)
		{
			if (!(*it)->isAbstract())
			{
				kind_vec.push_back( (*it)->getLStrictName());
			}
		}

		if ( !kind_vec.empty()) // there is non abstract desc of fco
		{
			if ( Dumper::m_bGenRegular)
			{
				Method m;

				kindGetter2( retval_kind, kind, kind_vec, method_name, cont, m);

				cont->addMethod( m);
			}

			
			if ( Dumper::m_bGenTemplates)
			{
				Method m;

				kindGetter2Generic( retval_kind, kind, kind_vec, method_name, cont, m);

				cont->addMethod( m);
			}
		}

		/* if there is a need for Excl getters
		if ( Dumper::m_bGenRegular)
		{
			Method m; // exclusive getter

			kindGetter3Generic( retval_kind, kind, method_name, cont, m);

			cont->addMethod( m);
		}


		if ( Dumper::m_bGenTemplates)
		{
			Method m; // exclusive getter

			kindGetter3Generic( retval_kind, kind, method_name, cont, m);

			cont->addMethod( m);
		}*/
	}
}



/**
 * Model related methods
 */

/*static*/ void CodeGen::dumpRoleGetter( FCO* fco, RoleRep * role, ModelRep * cont)
{
	// <!> fco == role->getFCOPtr() so fco could be removed from the parameter list
	std::string method_name;
	
	std::string src, comm; 

	std::string fco_nmsp( fco->getValidNmspc()), mod_nmsp( cont->getValidNmspc()), diff_nmsp( "");
	bool is_diff_nmsp = fco_nmsp != mod_nmsp;
	if( is_diff_nmsp) diff_nmsp = fco_nmsp;

	std::string l_fco_name = fco->getLName();
		
	std::vector<FCO*> desc;
	fco->getIntDescendants( desc);
	if ( desc.empty()) // no descendants, the RoleName can be used instead of the name
	{
		if ( !fco->isAbstract())
		{
			method_name = ModelRep::roleGetterMethodName2( fco, role, false, diff_nmsp );
			std::string inquire = fco->getLStrictNmspc() + role->getSmartRoleName(); 
			std::string retval_kind = fco->getLValidName();
			if ( !fco->isToBeEx())
			{
				FCO * ext_anc = fco->getExtedAnc();
				if ( ext_anc)
					retval_kind = ext_anc->getLValidName();
				else
					retval_kind = "BON::" + Any::KIND_TYPE_STR[fco->getMyKind()];
			}

			if ( Dumper::m_bGenRegular)
			{
				Method m;

				roleGetter1( retval_kind, inquire, method_name, l_fco_name, cont, m);

				cont->addMethod( m);
			}

			
			if ( Dumper::m_bGenTemplates)
			{
				Method tm;

				roleGetter1Generic( retval_kind, inquire, method_name, l_fco_name, cont, tm);

				cont->addMethod( tm);
			}
		}
		else { } // do nothing in case is abstract and it doesn't have descs
	}
	else // has descendants (some of them may be abstract)
	{
		method_name = ModelRep::roleGetterMethodName2( fco, role, false, diff_nmsp);

		desc.push_back( fco);
		int count = 0;
		std::vector<FCO*>::iterator it = desc.begin();
		for( ; it != desc.end(); ++it)
		{
			if (!(*it)->isAbstract())
				++count;
		}

		/*
		if count == 1 <==> if and only if isLongForm() == false
		meaning that no need to use long rolenames to distinguish
		*/
		if (count == 0) // are there non-abstract descs of fco (including itself)?
		{ 
			// all desc (including the roleowner) is abstract
			// no method generated
		}
		else if ( count == 1)
		{
			int k;
			for( k = 0; k < desc.size() && desc[k]->isAbstract(); ++k) { }
			if ( k >= desc.size()) throw("Index out of bound during dumpRoleGetter");

			method_name = ModelRep::roleGetterMethodName2( desc[k], role, false, diff_nmsp);

			std::string retval_kind = desc[k]->getLValidName();
			if( !desc[k]->isToBeEx())
			{
				FCO * ext_anc = desc[k]->getExtedAnc();
				if ( ext_anc)
					retval_kind = ext_anc->getLValidName();
				else
					retval_kind = "BON::" + Any::KIND_TYPE_STR[desc[k]->getMyKind()];
			}

			// there is only one non-abstract, meaning that short role name must be used
			
			//std::string inquire = role->getSmartRoleName();

			std::string inquire = desc[k]->getLStrictNmspc() + (role->getOnlyRoleName().empty()?desc[k]->getName():role->getOnlyRoleName());

			if ( Dumper::m_bGenRegular)
			{
				Method m;

				roleGetter2( retval_kind, inquire, method_name, l_fco_name, cont, m);

				cont->addMethod( m);
			}

			
			if ( Dumper::m_bGenTemplates)
			{
				Method tm;

				roleGetter2Generic( retval_kind, inquire, method_name, l_fco_name, cont, tm);

				cont->addMethod( tm);
			}
		}
		else if ( count > 1)
		{
			// more than one, so long role names to be used to the inquire
			
			// the aggregated can use "short form like" method name
			method_name = ModelRep::roleGetterMethodName2( fco, role, false, diff_nmsp); 

			std::string retval_kind = fco->getLValidName();
			if ( !fco->isToBeEx())
			{
				FCO * ext_anc = fco->getExtedAnc();
				if ( ext_anc)
					retval_kind = ext_anc->getLValidName();
				else
					retval_kind = "BON::" + Any::KIND_TYPE_STR[fco->getMyKind()];
			}

			std::string in_case_of_name_conflict_str;
			
			// name conflict happens if - role == "" AND
			//                          - contained fco (the role owner) is not abstract
			if (role->getOnlyRoleName().empty() && !role->getFCOPtr()->isAbstract())
				in_case_of_name_conflict_str = "int dummy"; // differentiate two methods with adding to the second method a dummy parameter
			
			std::vector< std::string > roles;
			for( int k = 0; k < desc.size(); ++k)
			{
				if ( !desc[k]->isAbstract())
				{
					roles.push_back( desc[k]->getLStrictNmspc() + desc[k]->getName() + role->getOnlyRoleName());
				}
			}

			if ( Dumper::m_bGenRegular)
			{
				Method m;

				roleGetter3( retval_kind, method_name, l_fco_name, role->getFCOPtr()->getLNmspc() + role->getOnlyRoleName(), roles, in_case_of_name_conflict_str, cont, m);

				cont->addMethod( m);
			}

			
			if ( Dumper::m_bGenTemplates)
			{
				Method tm;

				roleGetter3Generic( retval_kind, method_name, l_fco_name, role->getFCOPtr()->getLNmspc() + role->getOnlyRoleName(), roles, in_case_of_name_conflict_str, cont, tm);

				cont->addMethod( tm);
			}


			/* addition on 2/2/2004
			*/
			for(int k = 0; k < desc.size(); ++k)
			{
				if ( !desc[k]->isAbstract())
				{
					method_name = ModelRep::roleGetterMethodName3( desc[k], role, false, diff_nmsp);// separate long form name for each 


					std::string retval_kind = desc[k]->getLValidName();
					if ( !desc[k]->isToBeEx())
					{
						FCO * ext_anc = desc[k]->getExtedAnc();
						if ( ext_anc)
							retval_kind = ext_anc->getLValidName();
						else
							retval_kind = "BON::" + Any::KIND_TYPE_STR[ desc[k]->getMyKind()];
					}

					if ( Dumper::m_bGenRegular)
					{
						Method m;

						roleGetter4( retval_kind, method_name, fco->getName(), desc[k]->getName(), role->getOnlyRoleName(), fco->getLStrictNmspc(), cont, m);

						cont->addMethod( m);
					}

					
					if ( Dumper::m_bGenTemplates)
					{
						Method tm;

						roleGetter4Generic( retval_kind, method_name, fco->getName(), desc[k]->getName(), role->getOnlyRoleName(), fco->getLStrictNmspc(), cont, tm);

						cont->addMethod( tm);
					}
				}
			}
		}
	}
}


/**
 * Reference related methods
 */

/*static*/ Method CodeGen::dumpRefGetter( ReferenceRep * cont, FCO * fco, const std::string& common_kind)
{
	Method m;
	std::string mmm, retval_kind;
	if (fco)
	{
		retval_kind = fco->getLValidName();

		if ( !fco->isToBeEx())
		{
			FCO * ext_anc = fco->getExtedAnc();
			if ( ext_anc)
				retval_kind = ext_anc->getLValidName();
			else
				retval_kind = "BON::" + Any::KIND_TYPE_STR[fco->getMyKind()];
		}
	}
	else if ( !common_kind.empty()) // using the common_kind if set
		retval_kind = common_kind;
	else
		retval_kind = "BON::FCO";

	mmm  = indent(s_ind + 0) + "{\n";
	mmm += indent(s_ind + 1) +   "BON::FCO r = BON::ReferenceImpl::getReferred();\n";
	mmm += indent(s_ind + 1) +   "return " + retval_kind + "(r);\n";
	mmm += indent(s_ind + 0) + "}\n\n\n";

	m.m_returnValue = retval_kind;
	m.m_signature = cont->refGetterTemplate(fco) + "()";
	m.m_implementation = mmm;
	m.m_container = cont;
	m.m_comment = "";

	return m;
}

/*static*/ std::string CodeGen::folderKidsTemplate()
{
	std::string mmm;
	mmm  = indent(s_ind + 1) +     "// then its children\n";
	mmm += indent(s_ind + 1) +     "std::set<BON::Folder> subfolders = BON::FolderImpl::getChildFolders();\n";
	mmm += indent(s_ind + 1) +     "for( std::set<BON::Folder>::const_iterator it = subfolders.begin(); it != subfolders.end(); ++it)\n";
	mmm += indent(s_ind + 1) +     "{\n";
	mmm += indent(s_ind + 2) +         "(*it)->accept( pVisitor);\n";
	mmm += indent(s_ind + 1) +     "}\n\n";
	
	mmm += indent(s_ind + 1) +     "std::set<BON::FCO> children = BON::FolderImpl::getRootFCOs();\n";
	mmm += indent(s_ind + 1) +     "for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)\n";
	mmm += indent(s_ind + 1) +     "{\n";
	mmm += indent(s_ind + 2) +         "(*it)->accept( pVisitor);\n";
	mmm += indent(s_ind + 1) +     "}\n";

	return mmm;
}

/*static*/ std::string CodeGen::modelKidsTemplate()
{
	std::string mmm;
	mmm  = indent(s_ind + 1) +     "// then its children\n";
	mmm += indent(s_ind + 1) +     "std::set<BON::FCO> children = ModelImpl::getChildFCOs();\n";
	mmm += indent(s_ind + 1) +     "for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)\n";
	mmm += indent(s_ind + 1) +     "{\n";
	mmm += indent(s_ind + 2) +         "(*it)->accept( pVisitor);\n";
	mmm += indent(s_ind + 1) +     "}\n";

	return mmm;
}


/*static*/ Method CodeGen::acceptMethod( Any * any, bool pWithTraversalOfKids, bool pSpecialized, bool pRetValBool)
{
	std::string kd = any->getMyKindStr();
	std::string nm = any->getValidName();
	std::string tp = any->getLValidName();
	ClassAndNamespace can = ClassAndNamespace::makeIt( any);
	//std::string sp1 = any->getValidNmspc();
	//std::string sp2 = any->getNmspc();
	std::string sp_val = any->getStrictNmspc();
	Any::convertToValidName( sp_val); // validated namespace name

	Method m;
	std::string mmm;

	// pWithTraversalOfKids is ignored
		mmm  = indent(s_ind + 0) + "{\n";
	if( pSpecialized)
	{
		mmm += indent(s_ind + 1) +     "// visit the " + tp + "\n";
		//mmm += indent(s_ind + 1) +     "pVisitor->visit" + ( Dumper::m_iVisitSign == 1?"":sp_val) + nm + "( " + tp + "( this));\n\n"; // for example: "pVisitor->visitTwoCompound( Two::Compound( this))"
		if( pRetValBool)
		{
			mmm += indent(s_ind + 1) +     "bool rv = pVisitor->visit" + can.infoName() + "( " + can.exactType() + "( this));\n";
		}
		else
		{
			mmm += indent(s_ind + 1) +     "pVisitor->visit" + can.infoName() + "( " + can.exactType() + "( this));\n";
		}
		
		if( pWithTraversalOfKids) // models 
		{
			if( any->getMyKind() == Any::MODEL)
			{
				mmm += "\n" + modelKidsTemplate();
			}
			else if( any->getMyKind() == Any::FOLDER)
			{
				mmm += "\n" + folderKidsTemplate();
			}
		}

		if( pRetValBool)
		{
			mmm += indent(s_ind + 1) +     "return rv;\n";
		}
	} else 
	{
		mmm += indent(s_ind + 1) +     "// visit the " + kd + "\n";
		mmm += indent(s_ind + 1) +     "pVisitor->visit" + kd + "( BON::" + kd + "( this));\n\n"; // for example: "pVisitor->visitModel( BON::Model( this))"

		if( pWithTraversalOfKids)
		{
			if( any->getMyKind() == Any::MODEL)
			{
				mmm += modelKidsTemplate();
			}
			else if( any->getMyKind() == Any::FOLDER)
			{
				mmm += folderKidsTemplate();
			}
		}
	}
	mmm += indent(s_ind + 0) + "}\n\n\n";


	m.m_virtual = true;
	if( pSpecialized)    //accept( SpecVisitor *pVisitor)
	{
		m.m_returnValue = pRetValBool ? "bool":"void";
		m.m_signature = "accept( " + global_vars.m_namespace_name + Any::NamespaceDelimiter_str + MakeVisitor::getVisitorName( Dumper::getInstance()->getValidName()) + " *pVisitor)";
	}
	else
	{
		m.m_returnValue = "void";
		m.m_signature = "accept( BON::Visitor *pVisitor)";
	}
	m.m_implementation = mmm;
	m.m_container = any;
	m.m_comment = "";


	return m;
}

///*obsolete*/
///*static*/ Method CodeGen::folderAcceptMethod( Any * cont, bool pWithTraversalOfKids, bool pSpecialized)
//{
//	Method m;
//	std::string mmm;
//
//	mmm  = indent(s_ind + 0) + "{\n";
//	mmm += indent(s_ind + 1) +     "// visit first the folder\n";
//	mmm += indent(s_ind + 1) +     "pVisitor->visitFolder( BON::Folder( this));\n\n";
//
//	mmm += indent(s_ind + 1) +     "// then its children\n";
//	mmm += indent(s_ind + 1) +     "std::set<BON::Folder> subfolders = BON::FolderImpl::getChildFolders();\n";
//	mmm += indent(s_ind + 1) +     "for( std::set<BON::Folder>::const_iterator it = subfolders.begin(); it != subfolders.end(); ++it)\n";
//	mmm += indent(s_ind + 1) +     "{\n";
//	mmm += indent(s_ind + 2) +         "(*it)->accept( pVisitor);\n";
//	mmm += indent(s_ind + 1) +     "}\n\n";
//	
//	mmm += indent(s_ind + 1) +     "std::set<BON::FCO> children = BON::FolderImpl::getRootFCOs();\n";
//	mmm += indent(s_ind + 1) +     "for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)\n";
//	mmm += indent(s_ind + 1) +     "{\n";
//	mmm += indent(s_ind + 2) +         "(*it)->accept( pVisitor);\n";
//	mmm += indent(s_ind + 1) +     "}\n";
//	mmm += indent(s_ind + 0) + "}\n\n\n";
//
//	m.m_virtual = true;
//	m.m_returnValue = "void";
//	m.m_signature = "accept( BON::Visitor *pVisitor)";
//	m.m_implementation = mmm;
//	m.m_container = cont;
//	m.m_comment = "";
//
//	return m;
//}
//
///*static*/ Method CodeGen::modelAcceptMethod( Any * cont, bool pWithTraversalOfKids, bool pSpecialized)
//{
//	Method m;
//	std::string mmm;
//
//	mmm  = indent(s_ind + 0) + "{\n";
//	mmm += indent(s_ind + 1) +     "// visit first the model\n";
//	mmm += indent(s_ind + 1) +     "pVisitor->visitModel( BON::Model( this));\n\n";
//	mmm += indent(s_ind + 1) +     "// then its children\n";
//	mmm += indent(s_ind + 1) +     "std::set<BON::FCO> children = ModelImpl::getChildFCOs();\n";
//	mmm += indent(s_ind + 1) +     "for( std::set<BON::FCO>::const_iterator it = children.begin(); it != children.end(); ++it)\n";
//	mmm += indent(s_ind + 1) +     "{\n";
//	mmm += indent(s_ind + 2) +         "(*it)->accept( pVisitor);\n";
//	mmm += indent(s_ind + 1) +     "}\n";
//	mmm += indent(s_ind + 0) + "}\n\n\n";
//
//	m.m_virtual = true;
//	m.m_returnValue = "void";
//	m.m_signature = "accept( BON::Visitor *pVisitor)";
//	m.m_implementation = mmm;
//	m.m_container = cont;
//	m.m_comment = "";
//
//	return m;
//}

