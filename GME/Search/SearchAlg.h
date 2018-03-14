// SearchAlg.h: interface for the CSearch class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_SEARCHALG_H
#define AFX_SEARCHALG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComHelp.h"
#include "Input.h"
#include <afxcmn.h>
//#include "GMESearchDialog.h"

/**
* \brief The Search Class
*
* Performs the search algorithm. The input is validated and the output list empty when
* this class is instantiated. Any match found will be added to the tail of the output list.
*/
class CSearch  
{
public:
	CSearch(CInput inp);
//	CSearch(CInput inp, CGMESearchDialog *Searcher);
	virtual ~CSearch();
/**
* \brief Function to execute the correct input-based search.
*
* \param root : The project root folder, for a global search.
* \param scope : The collection of containers (if any) where local search should be performed 
* \param selected : The selected object, for a reference search.
* \param disp : An empty list that will be used to store the results.
* \param Progress : A Progress Control to keep the user aware that the search is progressing.
* \return Returns void, however, the actions of Search will fill the parameter disp with all matches found during the search.
*
*/
	void Search(IMgaFolder *root, IMgaObjects* scope, IMgaFCO *selected, CComPtr<IMgaFCOs> disp, CProgressCtrl *Progress);
    void SearchResults(CComPtr<IMgaFCOs> old_results,CComPtr<IMgaFCOs> disp, CProgressCtrl *Progress);
    bool Check(IMgaFCO* fco,int type);
    bool CheckLogicalCombination(IMgaFCO* fco,bool (CSearch::*Chk)(IMgaFCO* fco,bool first));
    int Matches(IMgaFCO* fco,bool first);
    bool PerformLogical(int first,int second);
	
private:

/**
* \brief Recursively search the current Folder and below	
*
* \param root : The current Folder we are on
*/
	void SearchFolderHierarchy(IMgaFolder *root);
/**
* \brief Recursively search the current Model and below	
*
* \param root : The current Model we are on
*/
	void SearchModelHierarchy(IMgaModel *root);
/**
* \brief Search through all the references to objects in the list.
*
* \param referenced : Object whose references will be searched
*/
	void SearchReferences(IMgaFCO *referenced);

/**
* \brief Check all the atoms for search matches.
*
* \param allObjects : list of objects currently being searched.
*/
	void CheckAllAtoms(IMgaFCOs *allObjects);

//check  all connections for matches
    void CheckAllConnections(IMgaFCOs *allObjects);
/**
* \brief Check all the references for search matches.
*
* \param allObjects : list of objects currently being searched.
*/
	void CheckAllReferences(IMgaFCOs *allObjects);
/**
* \brief Check all the sets for search matches.
*
* \param rootObjects : Model from which child objects will be searched.
*/
	void CheckAllSets(IMgaFCOs *allObjects);
/**
* \brief Check all the models for search matches.
*
* \param allObjects : list of objects currently being searched.
*/
	void CheckAllModels(IMgaFCOs *allObjects);

/**
* \brief Check this model against the search criteria.
*/
	bool CheckModel(IMgaFCO *Model);
/**
* \brief Check this atom against the search criteria.
*/
	bool CheckAtom(IMgaFCO *Atom);

    //check connection against search criteria
    bool CheckConnection(IMgaFCO *Connection);
/**
* \brief Check this reference against the search criteria.
*/
	bool CheckReference(IMgaFCO *Reference);
/**
* \brief Check this set against the search criteria.
*/
	bool CheckSet(IMgaFCO *Set);

/**
* \brief Check the object's attributes against the search criteria.
*/
	bool CheckAttributes(IMgaFCO *obj,bool first=true);

   // bool CheckAttributes2(IMgaFCO *obj,BOOL first=1);


	/// Where to store the results
	CComPtr<IMgaFCOs> results;
	// Progress bar
	CProgressCtrl *m_pgsSearch;
	
	/// The input criteria.
	CInput filter;

    bool EvaluateResult(std::vector<Attribute>& attrs);

   
//	CGMESearchDialog *SearchDialog;
};

#endif
