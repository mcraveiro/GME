#include "stdafx.h"

#include "MgaUtil.h"

#include "Dumper.h"
#include "AspectSpecDlg.h"
#include "GlobalAspOrder.h"
#include "Options.h"
#include "Broker.h"
#include "Token.h"
#include "algorithm"
#include <afxdlgs.h>
#include <comdef.h>

#include "globals.h"
extern Globals global_vars;

int ind = 0;
CAspectSpecDlg *theAspectDlg = 0;

// keep in sync with AspectSpecDlg.cpp's DUMPER_NOT_SPECIFIED_STR
#define DUMPER_NOT_SPECIFIED_STR      "Not specified"


Sheet * Dumper::getInstance()
{ 
	if (!Sheet::m_theOnlyInstance) 
	{
		Sheet::m_theOnlyInstance = new Dumper();
		return Sheet::m_theOnlyInstance;
	}
	return Sheet::m_theOnlyInstance;
}


Dumper::Dumper(): Sheet()
{ 
}


Dumper::~Dumper()
{ 
	Broker::reset();
}


bool Dumper::findInFolders( FCO * fco)
{
	std::vector<FolderRep*>::iterator it( m_folderList.begin());
	while( it !=  m_folderList.end() && !(*it)->isFCOContained( fco))
		++it;
	return it !=  m_folderList.end();
}


void Dumper::inheritMoReSeCoFolAsp() //doInheritance()
{
	// propagates down the hierarchy 
	// -role,
	// -reference, ( has to be before Connection because of Ports) 
	// -setMember, 
	// -Connection, 
	// -FolderMember, 
	// -AspectMember relationship


	//
	// task 1
	// creating roles for each FCO __descendant__ which is present in the Containment relationship
	// and copying these roles to M2 and M3 also
	//
	// i.e.:
	//     FCO -----role-<>M1
	//      |              |
	//     /^\            /^\
	//   A1   A2        M2   M3
	// 


	//
	// task 2
	// expands the reference relation as above
	//
	// i.e.:
	//     FCO <-----------R1
	//      |              |
	//     /^\            /^\
	//   A1   A2        R2   R3
	//
	// as a result R1, R2 and R3 may point to an FCO, A1, A2


	//
	// task 3
	// connection
	//
	// i.e.
	//     FCOA -src------o-----dst- FCOB
	//      |             |            |
	//     /^\            C1          /^\
	//   A1   A2          |         B2   B3
	//                    |
	//                   /^\
	//                 C2   C3
	//
	// in this case C1, C2, C3 will have as src FCOA, A1, A2
	// and as destination FCOB, B2, B3
	//

	//
	// task 4
	// expands set membership relation for each FCO __descendant__
	// i.e.:
	//     FCO ------<>S1
	//      |          |
	//     /^\        /^\
	//   A1   A2    S2   S3
	// 
	// produces A1, A2 as new (inherited) set members for S1, S2, S3

	//
	// task 5
	// expands folder membership
	// no inheritance between folders !
	//
	//      FCOA -folder_member----FolderA
	//       |
	//      /^\
	//  FCOB   FCOC
	//
	// FolderA may contain FCOA, FCOB, FCOC


	//
	// task 6
	// expand aspect membership ( if FCOA is selected to appear in AspA)
	//      FCOA -set_member----AspA
	//       |
	//      /^\
	//  FCOB   FCOC
	//  
	//
	// if FCOA is member of AspA aspect, then FCOB and FCOC is also a member of AspA
	//
	// in case when only a role (of FCOA in a model) is specified as part of an aspect 
	// that role should be extended for FCOB and FCOC as well and for the model
	// descendants inherited from the model. I.e.
	//
	// AspA-----<>M1<>-------FCOA
	//            /\
	//            |
	//            M2
	//
	// if the role between M1--FCOA is specified as an aspect member then M2 in AspA should show
	// FCOA?

	// task 1
	ModelRep_Iterator mod_iter( m_modelList.begin());
	for( ; mod_iter != m_modelList.end(); ++mod_iter)
		(*mod_iter)->inherit();

	// task 2
	// the refs first because the connections need the refs !
	ReferenceRep_Iterator ref_iter( m_refList.begin());
	for( ; ref_iter != m_refList.end(); ++ref_iter)
		(*ref_iter)->inherit();

	ref_iter = m_refList.begin();
	for( ; ref_iter != m_refList.end(); ++ref_iter)
		if (!(*ref_iter)->finalize())
		{
			//global_vars.err << (*ref_iter)->getName() << " is referring to too many kinds\n";
		}

	// task 3
	ConnectionRep_Iterator conn_iter( m_connList.begin());
	for( ; conn_iter != m_connList.end(); ++conn_iter)
		(*conn_iter)->inherit();

	// task 4 - for all sets on the sheet
	SetRep_Iterator set_iter( m_setList.begin());
	for( ; set_iter != m_setList.end(); ++set_iter)
		(*set_iter)->inherit();

	// task 5: folder element hierarchies
	std::vector<FolderRep*>::iterator folder_iter( m_folderList.begin());
	for( ; folder_iter != m_folderList.end(); ++folder_iter)
		(*folder_iter)->extendMembership();

	// task 6
	std::vector<AspectRep*>::iterator asp_iter( m_aspectList.begin());
	for( ; asp_iter != m_aspectList.end(); ++asp_iter)
		(*asp_iter)->extendAspectWithDescendants();
}


void Dumper::inheritConstraintsAndAttributes(FCO * ptr, FCO * c_ptr)
{
	// adding constraints
	c_ptr->addFinalConstraintList( ptr->getInitialConstraintRepPtrList());

	// adding attributes
	c_ptr->addFinalAttributeList( ptr->getInitialAttributeRepPtrList());
}


void Dumper::inheritAspConsAttr()
{
	{FcoRep_Iterator it0( m_fcoRepList.begin());
	for( ; it0 != m_fcoRepList.end(); ++it0 )
	{
		std::vector<FCO*> children;
		(*it0)->getImpDescendants( children);
		children.push_back( *it0);

		std::vector<FCO*>::iterator child_it = children.begin();
		for( ; child_it != children.end(); ++child_it )
			inheritConstraintsAndAttributes( *it0, *child_it);
	}}
	
	{AtomRep_Iterator it1 = m_atomList.begin();
	for( ; it1 != m_atomList.end(); ++it1 )
	{
		std::vector<FCO*> children;
		(*it1)->getImpDescendants( children);
		children.push_back( *it1);

		std::vector<FCO*>::iterator child_it = children.begin();
		for( ; child_it != children.end(); ++child_it )
			inheritConstraintsAndAttributes( *it1, *child_it);
	}}

	{ModelRep_Iterator it2 = m_modelList.begin();
	for( ; it2 != m_modelList.end(); ++it2 )
	{
		std::vector<FCO*> children;
		(*it2)->getImpDescendants( children);
		children.push_back( *it2);

		std::vector<FCO*>::iterator child_it = children.begin();
		for( ; child_it != children.end(); ++child_it )
		{
			ModelRep * child_mod_ptr = dynamic_cast<ModelRep *>( *child_it);
			if ( !child_mod_ptr) global_vars.err << MSG_ERROR << "Internal error: after dynamic cast - inheritAspConsAttr.\n";

			// inherit aspects
			const ModelRep::AspectRepPtrList &asp_list = (*it2)->getAspectRepPtrList();
			child_mod_ptr->addFinalAspectList( asp_list);

			inheritConstraintsAndAttributes( *it2, *child_it);
		}
	}}

	{ConnectionRep_Iterator it3 = m_connList.begin();
	for( ; it3 != m_connList.end(); ++it3 )
	{
		std::vector<FCO*> children;
		(*it3)->getImpDescendants( children);
		children.push_back( *it3);

		std::vector<FCO*>::iterator child_it = children.begin();
		for( ; child_it != children.end(); ++child_it )
			inheritConstraintsAndAttributes( *it3, *child_it);
	}}

	{SetRep_Iterator it4 = m_setList.begin();
	for( ; it4 != m_setList.end(); ++it4 )
	{
		std::vector<FCO*> children;
		(*it4)->getImpDescendants( children);
		children.push_back( *it4);

		std::vector<FCO*>::iterator child_it = children.begin();
		for( ; child_it != children.end(); ++child_it )
		{
			inheritConstraintsAndAttributes( *it4, *child_it);
		}
	}}

	{ReferenceRep_Iterator it5 = m_refList.begin();
	for( ; it5 != m_refList.end(); ++it5 )
	{
		std::vector<FCO*> children;
		(*it5)->getImpDescendants( children);
		children.push_back( *it5);

		std::vector<FCO*>::iterator child_it = children.begin();
		for( ; child_it != children.end(); ++child_it )
			inheritConstraintsAndAttributes( *it5, *child_it);
	}}

	{// since folders don't inherit, the constraint list will not be changing
	FolderRep_Iterator folder_iter( m_folderList.begin());
	for( ; folder_iter != m_folderList.end(); ++folder_iter)
		(*folder_iter)->addFinalConstraintList( (*folder_iter)->getInitialConstraintRepPtrList());
	}
}


void Dumper::doAspectPartsMap()
{
	// builds the Parts for each Aspect of all Models
	ModelRep_Iterator jt( m_modelList.begin());
	for( ; jt != m_modelList.end(); ++jt)
	{
		ModelRep * mod_ptr = *jt;
		if (!mod_ptr->isAbstract())
		{
			mod_ptr->sortMyAspects(); // based on lessThan sort the aspects the model owns
			mod_ptr->createPartsInModelAspects();
		}
	}

	// for all aspects
	std::vector<AspectRep*>::iterator it( m_aspectList.begin());
	for( ; it != m_aspectList.end(); ++it)
	{
		AspectRep * asp_ptr = *it;
		asp_ptr->sortPartMap(); //sort the partseries in the map
	}
}


// returns false in severe error cases only: aspect selection impossible for modelreference
bool Dumper::am()
{
	bool aspect_selection_impossible( false);
	bool is_first_time_used_the_new_interpreter ( false);
	
	// these 3 variables work together, as all_parts is growing (by push_back) i is increasing
	std::vector<PartRep> all_parts;
	unsigned int i = 0;
	std::vector< std::string> default_kindaspect_values_for_parts;

	std::map< unsigned int, unsigned int> asp_map;

	theAspectDlg = new CAspectSpecDlg("Configure Aspect Mapping");
	if (!theAspectDlg)
	{
		global_vars.err << MSG_ERROR << "ERROR: Aspect Dialogue cannot be created.\n";
		return true;
	}

	// for all aspects	
	std::vector<AspectRep*>::iterator it( m_aspectList.begin());
	for( ; it != m_aspectList.end(); ++it)
	{
		AspectRep * asp_ptr = *it;
		const std::string asp_name = asp_ptr->getName();

		if(asp_ptr->getReadOnly())   // set all parts in all models to non-primary
		{
			ModelRep_Iterator jt( m_modelList.begin());
			for( ; jt != m_modelList.end(); ++jt)
			{
				ModelRep * parent_mod_ptr = *jt;
				std::string parent_name = parent_mod_ptr->getName();

				// take all parts from the Aspect within the parent Model
				const AspectRep::PartRepSeries & part_series = asp_ptr->getPartSeries( parent_mod_ptr);
				AspectRep::PartRepSeries_ConstIterator kt = part_series.begin();
				for ( ; kt != part_series.end(); ++kt)
				{
					PartRep part = *kt;
					std::string r_name = part.getRoleRepPtr()->getSmartRoleName();
					std::string primary_reg_path = "/PrimaryAspects/" + parent_name + ":" + r_name + "/" + asp_name;
					try {
							part.getFCOPtr()->getMyRegistry()->setValueByPath( primary_reg_path, "no");
					}
					catch(...) {
						global_vars.err << MSG_ERROR << "Internal error: Exception during registry related write operation.\n";
					}
				}
			}
			continue;  // do not display aspect in dialog; 
		}

		// for all models
		ModelRep_Iterator jt( m_modelList.begin());
		for( ; jt != m_modelList.end(); ++jt)
		{
			ModelRep * parent_mod_ptr = *jt;
			std::string parent_name = parent_mod_ptr->getName();

			// take all parts from the Aspect within the parent Model
			const AspectRep::PartRepSeries & part_series = asp_ptr->getPartSeries( parent_mod_ptr);
			AspectRep::PartRepSeries_ConstIterator kt = part_series.begin();
			for ( ; kt != part_series.end(); ++kt)
			{
				PartRep part = *kt;
				std::string r_name = part.getRoleRepPtr()->getSmartRoleName();
				std::string primary_reg_path = "/PrimaryAspects/" + parent_name + ":" + r_name + "/" + asp_name;
				std::string kindasp_reg_path   =  "/KindAspects/" + parent_name + ":" + r_name + "/" + asp_name;
				
				bool skip = false; // indicates if the primary field is useless
				bool is_model = ( part.getFCOPtr()->getMyKind() == Any::MODEL);

				bool is_ref_to_model = false;
				if ( part.getFCOPtr()->getMyKind() == Any::REF)
				{
					const ReferenceRep * ref_ptr = dynamic_cast<const ReferenceRep *>( part.getFCOPtr());
					std::string nn = ref_ptr->getName();
					if (ref_ptr->pointsToModels()) is_ref_to_model = true;
				}

				bool primary_aspect_needed ( parent_mod_ptr->howManyAspects() > 1);// meaning if user interaction is needed? ( is required?)
				bool kind_aspect_needed ( is_model || is_ref_to_model); // meaning if user interaction is needed? ( is required?)

				if ( primary_aspect_needed) // is member of more than one aspects of its model
				{
					/* 
						rationale:
								Asp1 ----- Model ------Asp2
													 /   \
											 Atom1   Atom2

								Asp1 contains: ..., Atom1, ...and doesn't contain Atom2
								Asp2 contains: ..., Atom2, ...and ...-//-         Atom1*/

					// check if part/role? is part of several aspects of parent_mod_ptr !
					int no_of_membership_in_aspects = parent_mod_ptr->searchMyAspectsForPart( part);
					if ( no_of_membership_in_aspects <= 1) // no need to be in dialogue
						primary_aspect_needed = false;
				}

				if ( !primary_aspect_needed)
				{
						// let 's try this
						std::string regVal;
						regVal = "yes";
						try {
							//part.getFCOPtr()->getPtr()->getRegistry()->setValueByPath( regPath, regVal);
							part.getFCOPtr()->getMyRegistry()->setValueByPath( primary_reg_path, regVal);
						}
						catch(...) {
							global_vars.err << MSG_ERROR << "Internal error: Exception during registry related write operation.\n";
						}
				}

				if ( !is_model && !is_ref_to_model)
				{
					// in case the part is not a model and primary aspect needed is false
					// the aspect mapping is useless -> not present in the dialog 
					skip = !primary_aspect_needed;//continue; // is not needed -> the value in the registry is "yes" for sure
				}
				else if ( is_model)
				{
					const ModelRep * sub_model = dynamic_cast<const ModelRep*>( part.getFCOPtr());
					int sub_model_no_of_aspects = sub_model->howManyAspects();
					if ( sub_model_no_of_aspects <= 1) // no more than one aspect
					{
						std::string regVal;
						regVal = " "; //notifying default mechanism
						try {
							//part.getFCOPtr()->getPtr()->getRegistry()->setValueByPath( regPath, regVal);
							part.getFCOPtr()->getMyRegistry()->setValueByPath( kindasp_reg_path, regVal);
						}
						catch(...) {
							global_vars.err << MSG_ERROR << "Internal error: Exception during registry related write operation.\n";
						}

						kind_aspect_needed = false;
					}
					skip = !primary_aspect_needed && !kind_aspect_needed; //continue
				}
				else if ( 0 && is_ref_to_model) // do NOT skip any ref_to_model
				{
					const ReferenceRep * reference = dynamic_cast<const ReferenceRep*>( part.getFCOPtr());
					std::vector< AspectRep *> asp_intersect = reference->getAspectsIntersection();
					if ( asp_intersect.size() == 1)
					{
						AspectRep * sel = *asp_intersect.begin();

						std::string regVal;
						regVal = sel->getName(); //specifying explicitly for reference (though default mechanism)
						try {
							//part.getFCOPtr()->getPtr()->getRegistry()->setValueByPath( regPath, regVal);
							part.getFCOPtr()->getMyRegistry()->setValueByPath( kindasp_reg_path, regVal);
						}
						catch(...) {
							global_vars.err << MSG_ERROR << "Internal error: Exception during registry related write operation.\n";
						}

						kind_aspect_needed = false;
					}
					skip = !primary_aspect_needed && !kind_aspect_needed; //continue
				}

				if (!skip)
				{
					// now we are facing a case when 
					// -either there is a model/ref which needs either the primary aspect or both primary and kind aspect specification
					//		(in other words for a model at least the primary aspect is needed at this point)
					// -or other element (non-model) needs the primary aspect spec.
					// 
					// In other words: primary_aspect_needed may be false
					//								 kind_aspect_needed may be false [for models and model references]
					//									but the two are not false at the same time
					if ( primary_aspect_needed)
					{
						std::string regVal;
						// inquiring the registry for the primary asp value
						std::string old_regVal1 = part.getFCOPtr()->getPtr()->getRegistry()->getValueByPath( primary_reg_path);
						std::string old_regVal2 = part.getFCOPtr()->getMyRegistryOld()->getValueByPath( primary_reg_path);
						regVal = part.getFCOPtr()->getMyRegistry()->getValueByPath( primary_reg_path);
						if (regVal.empty() && ( !old_regVal1.empty() || !old_regVal2.empty()))
						{
							regVal = old_regVal2.empty()? old_regVal1:old_regVal2;
							std::string n_msg = "PrimaryAspect registry information taken from the old place. Registry of: " + part.getFCOPtr()->getName() + " Registry path: " + primary_reg_path + ".\n";
							global_vars.err << MSG_ERROR << n_msg;
							is_first_time_used_the_new_interpreter = true;
						}

						if ( !regVal.empty() && regVal == "no")
						{
							part.setPrimary( false);
						} // else: true is default value
					}

					std::string default_kind_aspect_name = "N/A", kind_aspect_name = "N/A";
					// if the default_k_a_n variable is not set -> remains on error value
					
					if ( kind_aspect_needed && (is_model || is_ref_to_model)) // is there need for kind_asp_name?
					{
						default_kind_aspect_name = DUMPER_NOT_SPECIFIED_STR; kind_aspect_name = "N/A";
						
						if (is_ref_to_model) 
						{
							const ReferenceRep * reference = dynamic_cast<const ReferenceRep*>( part.getFCOPtr());
							// calculating first the default_aspect_value
							// does it have the asp_name aspect?
							std::vector< AspectRep *> asp_intersect = reference->getAspectsIntersection();
							std::vector< AspectRep *>::iterator asp_intersect_it = asp_intersect.begin();
							while( asp_intersect_it != asp_intersect.end() && (*asp_intersect_it)->getName() != asp_name)
								++asp_intersect_it;
							if ( asp_intersect_it != asp_intersect.end())
								default_kind_aspect_name = asp_name;
							else // if not then the first aspect is the default
							{
								// commented out on 11/2006, since we allow kindaspect to be left out from xmp in 
								//AspectRep * default_asp = reference->getFirstAspectFromIntersection();
								//if ( default_asp)
								//	default_kind_aspect_name = default_asp->getName();
								//else
								//{
								//	// error since the default selection failure means error ignoring what the registry value may have
								//	global_vars.err << MSG_WARNING << "Warning: Reference \"" << reference->getPtr() << "\" has no valid aspect. Intersection of aspects the referred models might have is empty.\n";
								//	aspect_selection_impossible = true;
								//}
								if( asp_intersect.empty())
								{
									// error since the default selection failure means error ignoring what the registry value may have
									// global_vars.err << MSG_WARNING << "Warning: Reference \"" << reference->getPtr() << "\" has no valid aspect. Intersection of aspects the referred models might have is empty.\n";
									//aspect_selection_impossible = true; // not used anymore here
								}
							}
						}
						else // model
						{
							const ModelRep * sub_model = dynamic_cast<const ModelRep*>( part.getFCOPtr());

							// calculating first the default_aspect_value
							std::vector< AspectRep*> sub_model_aspects = sub_model->getFinalAspectRepPtrList();
							std::vector< AspectRep*>::iterator sub_model_aspect_it = sub_model_aspects.begin();
							while( sub_model_aspect_it != sub_model_aspects.end() && (*sub_model_aspect_it)->getName() != asp_name)
								++sub_model_aspect_it;
							if ( sub_model_aspect_it != sub_model_aspects.end())
								default_kind_aspect_name = asp_name;
							else // if not then the first aspect is the default
							{
								AspectRep * def_asp = sub_model->getFirstAspect();
								if (def_asp)
									default_kind_aspect_name = def_asp->getName();
								else
								{
									global_vars.err << MSG_WARNING << "Warning: Model \"" << sub_model->getPtr() << "\" has no valid aspect.\n";
									aspect_selection_impossible = true;
								}
							}
						}
						// at this point the default_kind_aspect_name either is valid or == DUMPER_NOT_SPECIFIED_STR or an error msg has been dumped

						std::string regVal = "";
						// fetch the sel_kind_aspect from the registry
						std::string old_regVal1 = part.getFCOPtr()->getPtr()->getRegistry()->getValueByPath( kindasp_reg_path);
						std::string old_regVal2 = part.getFCOPtr()->getMyRegistryOld()->getValueByPath( kindasp_reg_path);
						regVal = part.getFCOPtr()->getMyRegistry()->getValueByPath( kindasp_reg_path);
						if ( regVal.empty() && ( !old_regVal1.empty() || !old_regVal2.empty()))
						{
							regVal = old_regVal2.empty()?old_regVal1:old_regVal2;
							std::string n_msg = "KindAspect    registry information taken from the old place. Registry of: " + part.getFCOPtr()->getName() + " Registry path: " + kindasp_reg_path + ".\n";
							global_vars.err << MSG_ERROR << n_msg;
							is_first_time_used_the_new_interpreter = true;
						}

						bool correct_info_in_reg = false;
						if ( !regVal.empty() && regVal != " " && regVal != "__GME_default_mechanism")
						{
							// check if the models or the referred models own indeed such aspect box
							std::vector<AspectRep*> same_aspects_on_sheet = findAspectBasedOnName( regVal); // multiple aspects having the same name
							if ( !same_aspects_on_sheet.empty()) 
							{
								if ( is_ref_to_model)
								{
									const ReferenceRep * reference = dynamic_cast<const ReferenceRep*>( part.getFCOPtr());

									// checking the registry value
									std::vector< AspectRep *> asp_intersect = reference->getAspectsIntersection();
									std::vector< AspectRep *>::iterator asp_intersect_it = asp_intersect.begin();
									AspectRep * ar = 0;
									while( !ar && asp_intersect_it != asp_intersect.end())
									{
										if ( regVal.compare( (*asp_intersect_it)->getName()) == 0)
											ar = *asp_intersect_it;
										else
											++asp_intersect_it;
									}
									if (ar)
									{
										kind_aspect_name = regVal;
										part.setKindAspectPtr( ar);
										correct_info_in_reg = true;
									}
									else
										global_vars.err << MSG_WARNING << "Warning: Reference \"" << reference->getPtr() << "\" registry value: \"" << regVal << "\". Aspect registry value not found among intersection of aspects the reference may have.\n";
								}
								else // if ( is_model)
								{
									const ModelRep * sub_model = dynamic_cast<const ModelRep*>( part.getFCOPtr());

									AspectRep * ar = sub_model->getMyFirstAspectFromSet( same_aspects_on_sheet);
									if ( ar)
									{
										kind_aspect_name = regVal;
										part.setKindAspectPtr( ar);
										correct_info_in_reg = true;
										
									}
									else
										global_vars.err << MSG_WARNING << "Warning: Model \"" << sub_model->getPtr() << "\" has no valid aspect stored in registry.\n";
								}
							}
						}
						if (!correct_info_in_reg)
						{
							kind_aspect_name = default_kind_aspect_name;
						}
					}
					all_parts.push_back( part);
					default_kindaspect_values_for_parts.push_back( default_kind_aspect_name);
					asp_map[
						(theAspectDlg->AddEntry( CString(asp_name.c_str()), CString(parent_name.c_str()) + ":" + r_name.c_str(), CString( kind_aspect_name.c_str()), primary_aspect_needed?(part.isPrimary()?"yes":"no"):"N/A", part.getFCOPtr() )) 
						] =	i++;

				}
			}
		}
	}
	if ( 0 &&aspect_selection_impossible)
	{
		TO("Aspect selection not possible!");
		delete theAspectDlg;
		return false;
	}

	if ( is_first_time_used_the_new_interpreter)
	{
		const std::string m_note = 
			"Probably this metamodel is interpreted with MetaGME2004 Interpreter for the first time because no data "
		  "has been found in the new registry. Some registry entries have been copied from the old registry. "
			"Next time you interpret this metamodel you should not see this message.";
		TO( m_note);
		global_vars.err << MSG_INFO << m_note << "\n";
	}

	int dlg_res = IDOK;
	if( global_vars.silent_mode)
	{ }
	else
		dlg_res = theAspectDlg->DoModal();

	if ( i != 0 && dlg_res == IDOK) 
	{
		CWaitCursor wait;

		std::map<unsigned int, unsigned int>::iterator it = asp_map.begin();
		// going through the map elements (members of the table) and fetch the user option
		while ( it != asp_map.end())
		{
			// the maps key is the row_id obtained from the AddEntry
			//          value is the index in the all_parts vector ( local variable)
			unsigned short row_id = it->first;
			unsigned int index = it->second;

			PartRep &act_part = all_parts[ index];
			++it;

			// the aspect this part is member of is the act_part.getAspectPtr() one !!!
			const AspectRep * act_aspect_rep = act_part.getContainerAspectPtr();
			if ( !act_aspect_rep) global_vars.err << MSG_ERROR << "Internal error: ZERO pointer after aspect mapping in act_part.getContainerAspectPtr.\n";				// there an exception was thrown
			ASSERT( act_aspect_rep);
			std::string act_asp_name = act_aspect_rep->getName();

			const ModelRep * act_mod_ptr = act_part.getParentPtr();
			if( !act_mod_ptr) global_vars.err << MSG_ERROR << "Internal error: ZERO pointer after aspect mapping in act_part.getParentPtr.\n";
			ASSERT( act_mod_ptr);
			std::string act_mod_name = act_mod_ptr->getName();

			std::string act_role_name = act_part.getRoleRepPtr()->getSmartRoleName();

			std::string regPath, regVal;
			CString dummy="";
			CString selected_kind_aspect_name;
			CString selected_primary_value;
			const void * useless_ptr;
			// fetch the values from the dialog based on row_id
			theAspectDlg->GetEntry(row_id, dummy, selected_kind_aspect_name, selected_primary_value, useless_ptr);
			
			// set the user specified option back into the act_part variable
			if ( !selected_primary_value.IsEmpty() && (selected_primary_value.Compare("no") == 0 || selected_primary_value.Compare("yes") == 0))
			{
				
				regPath = "/PrimaryAspects/" + act_mod_name + ":" + act_role_name + "/" + act_asp_name;
				regVal = (selected_primary_value.Compare("no") == 0)?"no":"yes";
				act_part.setPrimary( regVal.compare("no") == 0?false:true);

				// registering the Primary Aspect value
				try {
					//act_part.getFCOPtr()->getPtr()->getRegistry()->setValueByPath( regPath, regVal);
					act_part.getFCOPtr()->getMyRegistry()->setValueByPath( regPath, regVal);
				} catch( ...) {
					global_vars.err << MSG_ERROR << "Internal error: Exception during registry related write operation.\n";
				}
			}
			
			// if is a model or ref to a model then may have kind asp, otherwise not
			if ( ( act_part.getFCOPtr()->getMyKind() == Any::MODEL || act_part.getFCOPtr()->getMyKind() == Any::REF) && !selected_kind_aspect_name.IsEmpty() && selected_kind_aspect_name != "N/A")
			{
				// verify if the selected aspect exists on the sheet
				std::vector<AspectRep*> aspects_on_sheet = findAspectBasedOnName(std::string((LPCTSTR)selected_kind_aspect_name));
				if ( !aspects_on_sheet.empty()) 
				{
					// setting back the user selected aspect rep into act_part
					act_part.setKindAspectPtr( *aspects_on_sheet.begin());

					regPath = "/KindAspects/" + act_mod_name + ":" + act_role_name + "/" + act_asp_name;
					regVal = selected_kind_aspect_name;

					// if the user selected the default value for a model do not store it in the registry ->set a special value in the token 
					if ( act_part.getFCOPtr()->getMyKind() == Any::MODEL && default_kindaspect_values_for_parts[ index].compare( selected_kind_aspect_name) == 0)
					{
						// the default choice has been made / or stored previously in the registry - used for models only!!!
						//regVal = "__GME_default_mechanism"; // clear the registry from this, the dump will not contain this kindaspect token
						regVal = " "; // this notifies that some information contains so won't be confused with the first time interpretation
					}

					// registering the KindAspect value
					try {
						//bon_ptr->getRegistry()->setValueByPath( regPath, regVal);
						act_part.getFCOPtr()->getMyRegistry()->setValueByPath( regPath, regVal);
					} catch(...) {
						global_vars.err << MSG_ERROR << "Internal error: Exception during registry related write operation.\n";
					}
				}
				else // not found
				{
					act_part.setKindAspectPtr( 0);
					if( selected_kind_aspect_name == DUMPER_NOT_SPECIFIED_STR)
					{	// do nothing, allowed to choose this specifically
					}	//global_vars.err << MSG_WARNING << "Warning: Reference \"" << act_role_name << "\" can't be assigned a preferred aspect, since the intersection of target model aspects is empty.\n";
					else
						global_vars.err << MSG_WARNING << "Warning: User selected aspect not found on sheet! Aspect \"" << act_asp_name << "\" Model \"" << act_mod_name << "\" Role \"" << act_role_name << "\" Selected aspect name: \"" << (LPCTSTR) selected_kind_aspect_name << "\".\n";

					// registering empty KindAspect value
					regPath = "/KindAspects/" + act_mod_name + ":" + act_role_name + "/" + act_asp_name;
					regVal = "";

					// to erase the invalid value found
					try {
						act_part.getFCOPtr()->getMyRegistry()->setValueByPath( regPath, regVal);
					} catch(...) {
						global_vars.err << MSG_ERROR << "Internal error: Exception during registry related write operation.\n";
					}
				}
			}
			if ( ( act_part.getFCOPtr()->getMyKind() == Any::MODEL || act_part.getFCOPtr()->getMyKind() == Any::REF) && selected_kind_aspect_name == "N/A")
			{
				// the part is a model or ref and has one aspect 
				// that is why the kindaspect value hasn't been displayed
			}
		}
	}
	else if( i == 0)
		global_vars.err << MSG_INFO << "Note: Aspect mapping avoided. No user choice needed to be made.\n";
	else 
		global_vars.err << MSG_INFO << "Note: Aspect mapping cancelled. Some data may not have been written back to registry.\n";
	delete theAspectDlg;
	return true;
}


std::vector<AspectRep *> Dumper::findAspectBasedOnName(const std::string & name_to_find) const
{
	std::vector<AspectRep *> result;

	std::vector<AspectRep *>::const_iterator asp_iter( m_aspectList.begin());
	for( ; asp_iter != m_aspectList.end(); ++asp_iter )
	{
		if ( (*asp_iter)->getName() == name_to_find )
		{
			if ( std::find( result.begin(), result.end(), *asp_iter) == result.end())
				result.push_back( *asp_iter);
		}
	}
	return result;
}


void Dumper::createConstraints()
{
	//this method creates constraints (from the cardinalities) which will be inherited like the typical ones

	ConnectionRep_Iterator conn_iter( m_connList.begin());
	// for all connections on the sheet
	for( ; conn_iter != m_connList.end(); ++conn_iter)
		(*conn_iter)->createConstraints( this);

	// models
	std::vector<ModelRep*>::const_iterator it_m = m_modelList.begin();
	for( ; it_m != m_modelList.end(); ++it_m)
	{
		(*it_m)->createConstraints( this);
	}

	// atoms
	//std::vector<AtomRep*>::const_iterator it_b = m_atomList.begin();
	//for( ; it_b != m_atomList.end(); ++it_b)
	{
	}
	
	// sets
	//std::vector<SetRep*>::const_iterator it_c = m_setList.begin();
	//for( ; it_c != m_setList.end(); ++it_c)
	{
	}

	// references
	//std::vector<ReferenceRep*>::const_iterator it_ref = m_refList.begin();
	//for( ; it_ref != m_refList.end(); ++it_ref)
	{
	}

	// folders
	std::vector<FolderRep*>::iterator it_fd = m_folderList.begin();
	for( ; it_fd != m_folderList.end(); ++it_fd)
	{
		(*it_fd)->createConstraints( this);
	}
}


void Dumper::doDump()
{
	// root folder dump
	std::string mmm= "<?xml version=\"1.0\"?>\n<!DOCTYPE paradigm SYSTEM \"edf.dtd\">"
	"\n\n<paradigm "
	"name=\"" + xmlFilter(m_projName) + "\" " +
	(m_projVersion.empty() ? "" : "version=\"" + xmlFilter(m_projVersion) + "\" ") +
	"guid=\"" + m_projGUID + "\" " +
	"cdate=\"" + xmlFilter(m_projCreated) + "\" " +
	"mdate=\"" + xmlFilter(m_projModified) + "\" " +
	">\n\n" +
	"\t<comment>" + xmlFilter(m_projComment) + "</comment>\n\n" +
	"\t<author>" + xmlFilter(m_projAuthor) + "</author>\n\n";

	DMP( mmm); mmm = "";
	
	mmm = m_rootFolder.headDump(); // root folder header

	DMP( mmm); mmm = "";

	++ind;

	std::map< std::string, bool, StringLex> already_dumped;

	// some constraints are not attached to any FCO or Folder so they are global
	{
		AnyLexicographicSort lex;
		std::string last_name = "";
		std::sort( m_constraintList.begin(), m_constraintList.end(), lex );
		ConstraintRep_Iterator it_cons1 = m_constraintList.begin();
		for( ; it_cons1 != m_constraintList.end(); ++it_cons1)
			if (!(*it_cons1)->isAttached())
			{
				mmm += (*it_cons1)->doDump();
				if ( last_name != "" && last_name == (*it_cons1)->getName())
					global_vars.err << MSG_ERROR << "Error: Duplicate global constraint name found \"" << (*it_cons1)->getPtr() << "\".\n";
				last_name = (*it_cons1)->getName();
			}
	}

	DMP( mmm); mmm = "";

	// constraint functions are not part of the root folder
	// yet they are global
	{
		AnyLexicographicSort lex;
		std::string last_name = "";
		std::sort( m_constraintFuncList.begin(), m_constraintFuncList.end(), lex );

		ConstraintFuncRep_Iterator it_cons2 = m_constraintFuncList.begin();
		for( ; it_cons2 != m_constraintFuncList.end(); ++it_cons2)
		{
			mmm += (*it_cons2)->doDump();
			if ( last_name != "" && last_name == (*it_cons2)->getName())
				global_vars.err << MSG_ERROR << "Error: Duplicate constraint function name found \"" << (*it_cons2)->getPtr() << "\".\n";
			last_name = (*it_cons2)->getName();
		}
	}

	DMP( mmm); mmm = "";

	// inner objects like global attributes
	//mmm = m_rootFolder.dumpGlobalAttributes();
	{
		AnyLexicographicSort lex;
		std::string last_name = "";
		std::sort( m_attributeList.begin(), m_attributeList.end(), lex );

		AttributeRep_Iterator it = m_attributeList.begin();
		for( ; it != m_attributeList.end(); ++it)
			//if ((*it)->hasAnyOwner() < 1) // it means is a global variable: wrong, it may be local without owner!!
			if((*it)->isGlobal()) // thus not dumping orphan attributes
			{
				//if ((*it)->isViewable())
				mmm += (*it)->doDumpAttr("");
				if ( last_name != "" && last_name == (*it)->getName())
					global_vars.err << MSG_ERROR << "Error: Duplicate global attribute name found \"" << (*it)->getPtr() << "\".\n";
				last_name = (*it)->getName();
			}
	}

	DMP( mmm); mmm = "";
	
	// atoms
	{AnyLexicographicSort lex;
	std::string last_name = "", this_name = "";
	std::sort( m_atomList.begin(), m_atomList.end(), lex );
	std::vector<AtomRep*>::const_iterator it_b = m_atomList.begin();
	if ( it_b != m_atomList.end())
	{
		for( ; it_b != m_atomList.end(); ++it_b)
		{
			if ( !(*it_b)->isAbstract())
			{
				mmm += (*it_b)->doDump();
			}

			this_name = (*it_b)->getName();
			if (last_name != "" && last_name == this_name)
				global_vars.err << MSG_ERROR << "Error: Duplicate atom name found \"" << this_name << "\".\n";
			else if (already_dumped[ this_name])
				global_vars.err << MSG_ERROR << "Error: Duplicate fco name found \"" << this_name << "\".\n";

			already_dumped[this_name] = true;
			last_name = this_name;
		}
		DMP( mmm);
		mmm = "";
	}}
	
	// sets
	{AnyLexicographicSort lex;
	std::string last_name = "", this_name = "";
	std::sort( m_setList.begin(), m_setList.end(), lex );
	std::vector<SetRep*>::const_iterator it_c = m_setList.begin();
	if ( it_c != m_setList.end())
	{
		for( ; it_c != m_setList.end(); ++it_c)
		{
			if ( !(*it_c)->isAbstract())
			{
				mmm += (*it_c)->doDump();
			}

			this_name = (*it_c)->getName();
			if (last_name != "" && last_name == this_name)
				global_vars.err << MSG_ERROR << "Error: Duplicate set name found \"" << this_name << "\".\n";
			else if (already_dumped[ this_name])
				global_vars.err << MSG_ERROR << "Error: Duplicate fco name found \"" << this_name << "\".\n";

			already_dumped[this_name] = true;
			last_name = this_name;
		}
		DMP( mmm);
		mmm = "";
	}}

	// connections
	{AnyLexicographicSort lex;
	std::string last_name = "", this_name = "";
	std::sort( m_connList.begin(), m_connList.end(), lex );
	std::vector<ConnectionRep*>::const_iterator it_cn = m_connList.begin();
	if ( it_cn != m_connList.end())
	{
		for( ; it_cn != m_connList.end(); ++it_cn)
		{
			if ( !(*it_cn)->isAbstract())
			{
				mmm += (*it_cn)->doDump();
			}

			this_name = (*it_cn)->getName();
			if (last_name != "" && last_name == this_name)
				global_vars.err << MSG_ERROR << "Error: Duplicate connection name found \"" << this_name << "\".\n";
			else if (already_dumped[this_name])
				global_vars.err << MSG_ERROR << "Error: Duplicate fco name found \"" << this_name << "\".\n";

			already_dumped[this_name] = true;
			last_name = this_name;
		}
		DMP( mmm);
		mmm = "";
	}}

	// references
	{AnyLexicographicSort lex;
	std::string last_name = "", this_name = "";
	std::sort( m_refList.begin(), m_refList.end(), lex );
	std::vector<ReferenceRep*>::const_iterator it_ref = m_refList.begin();
	if ( it_ref != m_refList.end())
	{
		for( ; it_ref != m_refList.end(); ++it_ref)
		{
			if ( !(*it_ref)->isAbstract())
			{
				mmm += (*it_ref)->doDump();
			}

			this_name = (*it_ref)->getName();
			if (last_name != "" && last_name == this_name)
				global_vars.err << MSG_ERROR  << "Error: Duplicate reference name found \"" << this_name << "\".\n";
			else if (already_dumped[this_name])
				global_vars.err << MSG_ERROR  << "Error: Duplicate fco name found \"" << this_name << "\".\n";

			already_dumped[this_name] = true;
			last_name = this_name;
		}
		DMP( mmm);
		mmm = "";
	}}

	// models
	{AnyLexicographicSort lex;
	std::string last_name = "", this_name = "";
	std::sort( m_modelList.begin(), m_modelList.end(), lex );
	std::vector<ModelRep*>::const_iterator it_m = m_modelList.begin();
	if ( it_m != m_modelList.end())
	{
		for( ; it_m != m_modelList.end(); ++it_m)
		{
			this_name = (*it_m)->getName();
			if ( !(*it_m)->isAbstract())
			{
				mmm += (*it_m)->doDump();
			}
			if (last_name != "" && last_name == this_name)
				global_vars.err << MSG_ERROR  << "Error: Duplicate model name found \"" << this_name << "\".\n";
			else if (already_dumped[this_name])
				global_vars.err << MSG_ERROR  << "Error: Duplicate fco name found \"" << this_name << "\".\n";

			already_dumped[this_name] = true;

			last_name = this_name;
		}
		DMP( mmm);
		mmm = "";
	}}

	// folders
	{AnyLexicographicSort lex;
	std::string last_name = "", this_name = "";
	std::sort( m_folderList.begin(), m_folderList.end(), lex );
	std::vector<FolderRep*>::iterator it_fd = m_folderList.begin();
	if ( it_fd != m_folderList.end())
	{
		for( ; it_fd != m_folderList.end(); ++it_fd)
		{
			mmm += (*it_fd)->doDump();

			this_name = (*it_fd)->getName();
			if ( last_name != "" && last_name == this_name)
				global_vars.err << MSG_ERROR  << "Error: Duplicate folder name found \"" << this_name << "\".\n";
			else if (already_dumped[ this_name])
				global_vars.err << MSG_ERROR  << "Error: Duplicate name found \"" << this_name << "\".\n";

			already_dumped[ this_name] = true;
			last_name = this_name;
		}
		DMP( mmm);
		mmm = "";
	}}

	mmm += m_rootFolder.tailDump();
	// dump footer
	DMP( mmm);
	mmm = "";

	--ind;
	DMP("</paradigm>\n");

	// aspect names checked for uniqueness ( cannot be equal with kind names)
	// for all aspects	
	{std::vector<AspectRep*>::iterator it( m_aspectList.begin());
	for( ; it != m_aspectList.end(); ++it)
	{
		const std::string asp_name = (*it)->getName();
		if (already_dumped[ asp_name])
		{
			global_vars.err << MSG_WARNING  << "Error: Duplicate name found \"" << asp_name << "\" (aspect and kind names should be unique).\n";
		}
	}}

}


bool Dumper::checkAll()
{
	const MON::Object& meta = this->m_BON_Project_Root_Folder->getObjectMeta();
	if (meta.project().displayedName() != "MetaGME")
		global_vars.err << MSG_WARNING  << "Warning: file's paradigm '" << meta.project().displayedName() << "' is not MetaGME\n";
	bool res0 = checkRootFolder();
	bool res1 = checkAllFCOs();
	bool res2 = checkOrphanAttributes();
	bool res3 = checkEmptyFolders();
	res0 = res0 && res1 && res2 && res3;
	return res0;
}


bool Dumper::checkRootFolder() const
{
	if (m_rootFolder.isEmpty())
	{
		global_vars.err << MSG_ERROR << "CHECK: RootFolder is empty.\n";
		return false;
	}
	return true;
}


bool Dumper::checkEmptyFolders() const
{
	bool res = true;
	FolderRep_ConstIterator it6 = m_folderList.begin();
	for( ; it6 != m_folderList.end(); ++it6 )
	{
		if ((*it6)->isEmpty())
		{
			global_vars.err << MSG_ERROR << "CHECK: Folder \"" << (*it6)->getPtr() << "\" is empty.\n"; 
			res = false; 
		}
	}
	return res;
}


bool Dumper::checkOrphanAttributes()
{
	bool res = true;
	AttributeRep_Iterator it = m_attributeList.begin();
	for( ; it != m_attributeList.end(); ++it)
	{
		if ((*it)->hasAnyOwner() < 1 && !(*it)->isGlobal())
		{
			res = false;
			global_vars.err << MSG_ERROR << "CHECK: \"" << (*it)->getPtr() << "\" local scope attribute not owned by any FCO.\n";
		}
	}
	return res;
}


bool Dumper::checkAllFCOs()
{
	bool res = true;
	{FcoRep_Iterator it0( m_fcoRepList.begin());
	for( ; it0 != m_fcoRepList.end(); ++it0 )
	{
		/*if (!(*it0)->checkInheritance()) these kinds always have different descendants
			res = false;*/
		if (!(*it0)->isAbstract()) 
		{ 
			global_vars.err << MSG_ERROR << "CHECK: FCO \"" << (*it0)->getPtr() << "\" should be abstract.\n";
			res = false; 
		}
		if (!(*it0)->isAbstract() && !(*it0)->checkIsPartOfFinal()) 
		{ 
			//global_vars.err << "CHECK: FCO \"" << (*it0)->getName() << "\" is not part of any model, so it doesn't have any role.\n"; 
			//res = false; 
		}
	}}

	{AtomRep_Iterator it1 = m_atomList.begin();
	for( ; it1 != m_atomList.end(); ++it1 )
	{
		if (!(*it1)->checkInheritance())
			res = false;
		if (!(*it1)->isAbstract() && !(*it1)->checkIsPartOfFinal()) 
		{ 
			//global_vars.err << "CHECK: Atom \"" << (*it1)->getName() << "\" is not part of any model, so it doesn't have any role.\n"; 
			//res = false; 
		}
	}}

	{ModelRep_Iterator it2 = m_modelList.begin();
	for( ; it2 != m_modelList.end(); ++it2 )
	{
		if (!(*it2)->checkInheritance())
			res = false;

		if (!(*it2)->isAbstract())
			if( !(*it2)->checkMyAspects( this))
				res = false;

		if (!(*it2)->isAbstract() && !(*it2)->checkIsPartOfFinal() && !m_rootFolder.isInRoot( *it2) && !findInFolders(*it2)) 
		{ 
			//global_vars.err << "CHECK: Model \"" << (*it2)->getName() << "\" is not part of any model neither in the root folder nor any other folder, so it doesn't have any role.\n";
			//res = false;
			//res = res; // this is an exception
		}
	}}

	{ConnectionRep_Iterator it3 = m_connList.begin();
	for( ; it3 != m_connList.end(); ++it3 )
	{
		if (!(*it3)->checkInheritance())
			res = false;
		if (!(*it3)->checkConnectionTargets())
			res = false;
		if (!(*it3)->isAbstract() && !(*it3)->checkIsPartOfFinal()) 
		{ 
			//global_vars.err << "CHECK: Connection \"" << (*it3)->getName() << "\" is not part of any model, so it doesn't have any role.\n";
			//res = false;
		}
	}}

	{SetRep_Iterator it4 = m_setList.begin();
	for( ; it4 != m_setList.end(); ++it4 )
	{
		if (!(*it4)->checkInheritance())
			res = false;
		if( !(*it4)->checkSetElements())
			res = false;
		if (!(*it4)->isAbstract() && !(*it4)->checkIsPartOfFinal()) 
		{ 
			//global_vars.err << "CHECK: Set \"" << (*it4)->getName() << "\" is not part of any model, so it doesn't have any role.\n"; 
			//res = false; 
		}
	}}

	{ReferenceRep_Iterator it5 = m_refList.begin();
	for( ; it5 != m_refList.end(); ++it5 )
	{
		if (!(*it5)->checkInheritance())
			res = false;
		if (!(*it5)->checkNotEmpty())
		{
			global_vars.err << MSG_WARNING << "CHECK: Reference \"" << (*it5)->getPtr() << "\" is not referring to any fco.\n";
			res = false;
		}
		else
		{
			// removed upon user request: 2/25/2004
			/*if (!(*it5)->checkAllTheSameKind())
			{
				global_vars.err << "CHECK: Reference \"" << (*it5)->getName() << "\" is referring to more than one kind of fcos.\n"; 
				res = false;
			}*/
			if (!(*it5)->isAbstract() && !(*it5)->checkIsPartOfFinal()) 
			{ 
				//global_vars.err << "CHECK: Reference \"" << (*it5)->getName() << "\" is not part of any model, so it doesn't have any role.\n"; 
				//res = false; 
			}
		}
	}}

  return res;
}


void Dumper::findMaxOfMetaRefs()
{
	{FcoRep_Iterator it0( m_fcoRepList.begin());
	for( ; it0 != m_fcoRepList.end(); ++it0 )
		Broker::initFromObj( (*it0)->getPtr(), (*it0)->getParentFolder(), (*it0)->getMyKindStr());}

	{AtomRep_Iterator it1 = m_atomList.begin();
	for( ; it1 != m_atomList.end(); ++it1 )
		Broker::initFromObj( (*it1)->getPtr(), (*it1)->getParentFolder(), (*it1)->getMyKindStr());}

	{ModelRep_Iterator it2 = m_modelList.begin();
	for( ; it2 != m_modelList.end(); ++it2 )
		Broker::initFromObj( (*it2)->getPtr(), (*it2)->getParentFolder(), (*it2)->getMyKindStr());}

	{ConnectionRep_Iterator it3 = m_connList.begin();
	for( ; it3 != m_connList.end(); ++it3 )
		Broker::initFromObj( (*it3)->getPtr(), (*it3)->getParentFolder(), (*it3)->getMyKindStr());}

	{SetRep_Iterator it4 = m_setList.begin();
	for( ; it4 != m_setList.end(); ++it4 )
		Broker::initFromObj( (*it4)->getPtr(), (*it4)->getParentFolder(), (*it4)->getMyKindStr());}

	{ReferenceRep_Iterator it5 = m_refList.begin();
	for( ; it5 != m_refList.end(); ++it5 )
		Broker::initFromObj( (*it5)->getPtr(), (*it5)->getParentFolder(), (*it5)->getMyKindStr());}

	{FolderRep_Iterator it6 = m_folderList.begin();
	for( ; it6 != m_folderList.end(); ++it6 )
		Broker::initFromObj( (*it6)->getPtr(), (*it6)->getParentFolder(), (*it6)->getMyKindStr());}

	{AspectRep_Iterator it7 = m_aspectList.begin();
	for( ; it7 != m_aspectList.end(); ++it7)
		Broker::initFromAspectObj( (*it7)->getPtr(), (*it7)->getName(), Sheet::m_BON_Project_Root_Folder);}

	{AttributeRep_Iterator it8 = m_attributeList.begin();
	for (; it8 != m_attributeList.end(); ++it8)
		Broker::initFromObj( (*it8)->getPtr(), (*it8)->getParentFolder(), (*it8)->getMyKindStr());}

	Broker::checkDuplicates();
	Broker::init();
}


bool Dumper::sortPtrs()
{
	//return true;
	AnyLexicographicSort lex;

	std::sort( m_folderList.begin(),  m_folderList.end(), lex );
	std::sort( m_fcoRepList.begin(),  m_fcoRepList.end(), lex );
	std::sort( m_atomList.begin(), m_atomList.end(), lex );
	std::sort( m_refList.begin(), m_refList.end(), lex );
	std::sort( m_connList.begin(), m_connList.end(), lex );
	std::sort( m_setList.begin(), m_setList.end(), lex );
	std::sort( m_modelList.begin(), m_modelList.end(), lex );	

	//std::sort( m_aspectList.begin(),  m_aspectList.end(), lex );
	std::sort( m_constraintList.begin(),  m_constraintList.end(), lex );
	std::sort( m_constraintFuncList.begin(),  m_constraintFuncList.end(), lex );
	std::sort( m_attributeList.begin(),  m_attributeList.end(), lex );

	return true;
}


bool Dumper::checkUniqueAspectNames()
{
	std::map< std::string, AspectRep *> names;
	for( unsigned int i = 0; i < m_aspectList.size(); ++i)
	{
		if( names.find( m_aspectList[i]->getName()) == names.end())
			names[ m_aspectList[i]->getName()] = m_aspectList[i];
		else // found duplicate names
			global_vars.err << MSG_WARNING << "Warning: Duplicate aspect name \"" 
			<< m_aspectList[i]->getName() << "\" found. See objects: " 
			<< m_aspectList[i]->getPtr() << " and " 
			<< names.find( m_aspectList[i]->getName())->second->getPtr() << " ! Use SameAspect relation to connect them.\n";
	}
	return true;
}

/*
This method and aspect order selection assumes no two aspects have been defined 
with the same name. Meaning that all aspect with the same name have to be 
connected with the SameAaspect opearator to (a proxy of) another Aspect //<!>
*/
bool Dumper::aspectOrderSel()
{
	unsigned int sz = m_aspectList.size();
	if ( sz == 0) return false;
	else if ( sz == 1) return true; // if one aspect no need for ordering, introduced on 5/13/2004

	GlobalAspOrder g;
	const std::string aspect_order_registry_token = "/AspectOrder";
	std::vector< AspectRep * > dlg_aspects_init_order;// aspect order used by the dlg ( m_aspectList.size(), 0);

	std::sort( m_aspectList.begin(), m_aspectList.end(), AspectCompare());// sort the aspects based on the current information

	BON::RegistryNode rn = Sheet::m_BON_Project_Root_Folder->getRegistry()->getChild( AspectRep::m_aspectRegistryRoot + aspect_order_registry_token);
	if ( rn && rn->getValue() == "valid") // fetch priority values from the Registry
	{
		// for each name there is only one priority value in the registry
		// so in case of duplicate (non-equivalent, but similar) aspects the priority value 
		// will be common
		std::set< AspNamePriorityComp::Type, AspNamePriorityComp> asp_order;
		std::set<BON::RegistryNode> ch = rn->getChildren();
		std::set<BON::RegistryNode>::iterator i = ch.begin();
		for( ; i != ch.end(); ++i)
		{
			asp_order.insert( AspNamePriorityComp::Type( (*i)->getName(), (*i)->getIntegerValue()));
		}
	
		std::set< AspNamePriorityComp::Type, AspNamePriorityComp>::iterator it = asp_order.begin();
		for( ; it != asp_order.end(); ++it)
		{
			std::vector<AspectRep *> asp_with_this_name = findAspectBasedOnName( it->first);
			for( unsigned int l = 0; l < asp_with_this_name.size(); ++l) // size might be 0 in case of inconsistent registry 
				dlg_aspects_init_order.push_back( asp_with_this_name[ l]); // set the same priority value for each similar aspect
		}

		int needed = m_aspectList.size() - dlg_aspects_init_order.size();
		
		ASSERT( needed >= 0); // ASSERT if more elements then available aspects in m_aspectList
			
		if ( needed > 0) // if fewer elements inserted then required
		{
			std::vector<AspectRep*>::iterator aspvec_it = m_aspectList.begin();
			for( ; aspvec_it != m_aspectList.end() && needed > 0; ++aspvec_it)
			{
				std::string asp_name = (*aspvec_it)->getName();
				// search for this "aspect name" in the dlg_init_asp vector
				std::vector<AspectRep*>::iterator asp_init_it = dlg_aspects_init_order.begin();
				while( asp_init_it != dlg_aspects_init_order.end() && (*asp_init_it)->getName() != asp_name )
					++asp_init_it;
				if ( asp_init_it == dlg_aspects_init_order.end()) // not found, so either newly introduced aspect or the registry information has been partly deleted
				{
					dlg_aspects_init_order.push_back( *aspvec_it);
					--needed;
				}
			}
		}
		ASSERT( dlg_aspects_init_order.size() == m_aspectList.size()); // == m_aspectList.size());
	} 
	else // if registry info not available or not equal to "valid"
		dlg_aspects_init_order = m_aspectList;

	ASSERT( dlg_aspects_init_order.size() == m_aspectList.size());
	g.addAspects( dlg_aspects_init_order );

	int dlg_res;// = IDCANCEL; // use the metamodel values (since the OnInitDialog and OnOk make some important jobs
	if( global_vars.silent_mode)
	{
		g.m_resultAspects = dlg_aspects_init_order;
		dlg_res = IDOK;
	}
	else
		dlg_res = g.DoModal();

	if ( dlg_res == IDOK)
	{
		const std::string path( "/" + AspectRep::m_aspectRegistryRoot + aspect_order_registry_token);
		Sheet::m_BON_Project_Root_Folder->getRegistry()->setValueByPath( path, "valid");
		
		ASSERT( g.m_resultAspects.size() == m_aspectList.size());
		unsigned int i = 0;
		for( ; i < g.m_resultAspects.size(); ++i)
		{
			AspectRep * curr_asp = g.m_resultAspects[i];
			curr_asp->setPriority( i+1);// set priority values from 1
	
			std::string subtoken = "/" + curr_asp->getName(); 
			char idx_str[32];
			sprintf( idx_str, "%d", i+1);
			Sheet::m_BON_Project_Root_Folder->getRegistry()->setValueByPath( path + subtoken, idx_str);//similar aspects will use the same priority by overwriting the values in the same regnode
		}

		// check if all have non/zero value -> if the user pressed OK then no aspect should have the priority value set to zero!!!
		for( i = 0; i < m_aspectList.size(); ++i)
		{
			if ( m_aspectList[i]->getPriority() == 0)
			{
				ASSERT(0);
				global_vars.err << MSG_INFO << "Internal error: aspect \"" << m_aspectList[i]->getName() << "\" has 0 priority value set after the dialog. Reset to 0x7fffffff.\n";
				m_aspectList[i]->setPriority( 0x7fffffff); // a big number
			}
		}

	}
	// sort the aspects based on the current information
	std::sort( m_aspectList.begin(), m_aspectList.end(), AspectCompare());

	return true;
}


bool Dumper::build()
{
	bool success = true;
	CWaitCursor wait;
	findMaxOfMetaRefs();
	
	checkUniqueAspectNames();
	aspectOrderSel();
	wait.Restore();
	
	sortPtrs(); // sort the containers

	createConstraints();
	inheritMoReSeCoFolAsp();		// models, references, set, connections, folder and aspect membership
	inheritAspConsAttr();				// hasAspect, hasConstraints, hasAttribute

	if ( !checkAll()) // creates dummy aspects if needed
	{
		TO("Please correct the errors!");
		//success = false;
	}

	doAspectPartsMap();

	bool am_res = am();
	if (!am_res)
		success = false;

	wait.Restore();
	doDump();

	return success;
}

/*static*/ bool Dumper::doParseCardinality(const std::string& cardinality, const std::string &target, const std::string &contextDesc, std::string &expr)
{
	//const CString cardinality = cardinality_c.c_str();
	//const CString target = target_c.c_str();
	//const CString contextDesc = contextDesc_c.c_str();

	//CTokenEx	tok; // <!> this to be replaced by the Token class
	Tokenizer tok;

	//CString expr = expr_c.c_str();
	expr = "";

	//CString card(cardinality);
	std::string card( cardinality);
	//card.Remove(_T('\t'));
	tok.removeWSP( card);
	//card.Remove(_T('\n'));
	//card.Remove(_T(' '));

	bool berr = false;
	//CStringArray ORs;
	std::vector< std::string > ORs;
	//tok.Split(card, ",", ORs);
	tok.split( card, ",", ORs);
	//for (int i = 0; i < ORs.GetSize(); i++) {
	for (int i = 0; i < ORs.size(); i++) {
		//CStringArray bounds;
		std::vector< std::string > bounds;
		//tok.Split(ORs[i], "..", bounds);
		tok.split( ORs[i], "..", bounds);

		//CString orexpr;
		std::string orexpr;

		//if (bounds.GetSize() == 1) {
		if (bounds.size() == 1) {
			int multip;
			//if(sscanf((LPCTSTR)bounds[0],"%d",&multip) != 1) {
			if(sscanf( bounds[0].c_str(),"%d",&multip) != 1) {
				berr = true;
				break;
			}
			else {
				if (multip < 0) {
					global_vars.err << MSG_ERROR << "Error: Multiplicity is negative: " << contextDesc << ".\n";
				}
				//orexpr.Format("(%s = %d)", target, multip);
				orexpr = "(" + target + " = ";
				char tmp[32];
				sprintf( tmp, "%d", multip);
				orexpr += tmp;
				orexpr += ")";
			}
		}
		//else if (bounds.GetSize() == 2) {
		else if (bounds.size() == 2) {
			int multipmin, multipmax;
			//CString andexpr1, andexpr2;
			std::string andexpr1, andexpr2;

			//if(sscanf((LPCTSTR)bounds[0],"%d",&multipmin) != 1) {
			if(sscanf( bounds[0].c_str(),"%d",&multipmin) != 1) {
				berr = true;
				break;
			}
			else {
				if (multipmin < 0) {
					global_vars.err << MSG_ERROR << "Error: Minimum multiplicity is negative: " << contextDesc << ".\n";
				}
				if (multipmin > 0) {
					//andexpr1.Format("(%s >= %d)", target, multipmin);
					andexpr1 = "(" + target + " >= ";
					char tmp[32];
					sprintf( tmp, "%d", multipmin);
					andexpr1 += tmp;
					andexpr1 += ")";
				}
			}

			//if(sscanf((LPCTSTR)bounds[1],"%d",&multipmax) != 1) {
			if(sscanf( bounds[1].c_str(),"%d",&multipmax) != 1) {
				if (bounds[1] != "*") {
					berr = true;
					break;
				}
			}
			else {
				//andexpr2.Format("(%s <= %d)", target, multipmax);
				andexpr2 = "(" + target + " <= ";
				char tmp[32];
				sprintf( tmp, "%d", multipmax);
				andexpr2 += tmp;
				andexpr2 += ")";
				if (multipmax < 0) {
					global_vars.err << MSG_ERROR << "Error: Maximum multiplicity is negative: " << contextDesc << ".\n";
				}
				if (multipmax < multipmin) {
					global_vars.err << MSG_ERROR << "Error: Minimum multiplicity > Maximum multiplicity : " << contextDesc << ".\n";
				}
			}

			//if ((!andexpr1.IsEmpty()) && (!andexpr2.IsEmpty())) {
			if ((!andexpr1.empty()) && (!andexpr2.empty())) {
				//orexpr.Format("( %s and %s )", andexpr1, andexpr2);
				orexpr = "( " + andexpr1 + " and " + andexpr2 + " )";
			}
			//else if (!andexpr1.IsEmpty()) {
			else if (!andexpr1.empty()) {
				orexpr = andexpr1;
			}
			//else if (!andexpr2.IsEmpty()) {
			else if (!andexpr2.empty()) {
				orexpr = andexpr2;
			}
		}

		//if (!orexpr.IsEmpty()) {
		if (!orexpr.empty()) {
			//if (!expr.IsEmpty()) {
			if (!expr.empty()) {
				expr += " or ";
			}
			expr += orexpr;
		}
	}

	if (berr) {
		expr = "";
	}

	//expr_c = (LPCTSTR) expr;
	return berr;
}

/*static*/ std::string Dumper::makeValidFileName( const std::string& pInFile)
{
	std::string res;
	std::string badchars(_T("\\/:*?\"<>|"));
	for( unsigned int i = 0; i < pInFile.length(); ++i)
	{
		if( badchars.find( pInFile[i] ) == std::string::npos)
			res += pInFile[i];
		else
			res += '_';
	}
	return res;
}

/*static*/ int Dumper::selectOutputFiles( const BON::Project& proj, const std::string& p_proj_name, const std::string& proj_path)
{
	std::string proj_name = makeValidFileName( p_proj_name);
	try {
		global_vars.xmp_file_name = proj_name + ".xmp";
		global_vars.err_file_name = global_vars.xmp_file_name + ".log";
		CString def_name = global_vars.xmp_file_name.c_str();

		if( global_vars.silent_mode)
		{
			global_vars.xmp_file_name = proj_path + proj_name + ".xmp";
			try {
				CFile f( global_vars.xmp_file_name.c_str(), CFile::modeCreate);
				global_vars.xmp_file_name = f.GetFilePath();
				f.Close();
				CFile::Remove( global_vars.xmp_file_name.c_str());
			}
			catch(...) {
				global_vars.xmp_file_name = proj_path + proj_name + ".xmp";
			}
			global_vars.err_file_name = global_vars.xmp_file_name + ".log";
		}
		else
		{
			CWaitCursor w;
			CFileDialog dlg(FALSE, "xmp", def_name,
				OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"XML Paradigm Files (*.xmp)|*.xmp|");
			dlg.GetOFN().lpstrInitialDir = proj_path.c_str();

			int dlg_res = IDOK;
			dlg_res = dlg.DoModal();
			if( dlg_res == IDOK) 
			{
				CString filename = dlg.GetPathName();
				global_vars.xmp_file_name = (LPCTSTR) filename;
				global_vars.err_file_name = global_vars.xmp_file_name + ".log";
			}
			else  // IDCANCEL , using default name
			{
				//global_vars.xmp_file_name = 
				return 4;
			}
		}
	}	catch (...)	{
		global_vars.err << MSG_ERROR << "Exception caught! Using default filenames.\n";
	}
	return 1;
}


/*static*/ int Dumper::selectOptions( const BON::Project& proj) 
{
	int v1( 0), v2( 0), v3( 0), vp( 1); //default values

	try {
		
		// load the saved options from the registry
		if( !Options::fetchOpts( proj, &v1, &v2, &v3, &vp)) return 1;

		global_vars.genConstr.reg_cont_mask = v1;
		global_vars.genConstr.fol_cont_mask = v2;
		global_vars.genConstr.connect_mask  = v3;
		global_vars.genConstr.priority      = vp;

	} catch(...) {
		global_vars.err << MSG_ERROR << "[MetaInterpreter] Option load did not succeed.";
	}

	if( global_vars.silent_mode)
	{
		return 1;
	}
	else
	{
		Options dlg;
		dlg.setDefs( v1, v2, v3, vp);
		int v = dlg.DoModal();
		if( v == IDOK) 
		{
			bool dont_ask = false;
			dlg.getResults( &v1, &v2, &v3, &vp, &dont_ask);
			
			global_vars.genConstr.reg_cont_mask = v1;
			global_vars.genConstr.fol_cont_mask = v2;
			global_vars.genConstr.connect_mask  = v3;
			global_vars.genConstr.priority      = vp;

			// save options in the registry
			try {
				Options::saveOpts( proj, v1, v2, v3, vp, dont_ask);
			} catch(...) { 
				global_vars.err << MSG_ERROR << "[MetaInterpreter] Option saving did not succeed.";
			}
		}
		
		return (v == IDOK)?1:2;
	}
}


// REGISTERING THE PARADIGM
/*static*/ void Dumper::registerIt( const std::string& f_name)
{
	int res = IDYES;

	if (global_vars.skip_paradigm_register)
	{
		res = IDNO;
	}

	if( global_vars.silent_mode)
 		TO( "Successfully generated " + CString(f_name.c_str()));
	else
		res = AfxMessageBox( "Successfully generated " + CString(f_name.c_str()) + "\n\nWould you like to register your new paradigm ?", MB_YESNO | MB_ICONINFORMATION);

	if ( res == IDYES) 
	{
		CWaitCursor wait;
		CComPtr<IMgaRegistrar> registrar;
		HRESULT hr = registrar.CoCreateInstance(L"MGA.MgaRegistrar");
		if (SUCCEEDED(hr)) {
			CComBSTR path = CString("XML=") + f_name.c_str();
			hr = registrar->RegisterParadigmFromData(path, NULL, REGACCESS_USER);
		}
		if (FAILED(hr)) {
			_bstr_t error;
			CComQIPtr<ISupportErrorInfo> supperr = registrar;
			if (supperr) {
				CComPtr<IErrorInfo> errorInfo;
				if (SUCCEEDED(GetErrorInfo(0, &errorInfo))) {
					errorInfo->GetDescription(error.GetAddress());
					error = L"Error occurred while registering new paradigm: " + error;
				}
			}
			if (error == _bstr_t()) {
				error = L"Error occurred while registering new paradigm.";
			}
			if( global_vars.silent_mode)
				TO(error);
			else
				AfxMessageBox(error, MB_ICONSTOP | MB_OK);
		}
	}
}


/*static*/ std::string Dumper::xmlFilter( const std::string& in)
{
	std::map<char, std::string> m;
	m['&'] = "&amp;";
	m['>'] = "&gt;";
	m['<'] = "&lt;";
	m['"'] = "&quot;";
	m['\''] = "&apos;";

	std::string ret;
	for( std::string::const_iterator i = in.begin(); i != in.end(); ++i)
		if ( m.find( *i) != m.end())
			ret += m[*i];
		else
			ret += *i;

	return ret;
}
