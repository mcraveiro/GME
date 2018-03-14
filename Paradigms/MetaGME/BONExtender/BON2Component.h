//###############################################################################################################################################
//
//	Meta and Builder Object Network V2.0 for GME
//	BON2Component.h
//
//###############################################################################################################################################

/*
	Copyright (c) Vanderbilt University, 2000-2004
	ALL RIGHTS RESERVED

	Vanderbilt University disclaims all warranties with regard to this
	software, including all implied warranties of merchantability
	and fitness.  In no event shall Vanderbilt University be liable for
	any special, indirect or consequential damages or any damages
	whatsoever resulting from loss of use, data or profits, whether
	in an action of contract, negligence or other tortious action,
	arising out of or in connection with the use or performance of
	this software.
*/

#ifndef BON2Component_h
#define BON2Component_h

#include "BON.h"
#include "BONImpl.h"
#include <ComponentConfig.h>

#include "string"
#include "list"
#include "map"
#include "algorithm"

#include "entity.h"
#include "relation.h"

#include "entity.h"
#include "FCO.h"
#include "Sheet.h"


namespace BON
{

//###############################################################################################################################################
//
// 	C L A S S : BON::Component
//
//###############################################################################################################################################

class Component
{
	//==============================================================
	// IMPLEMENTOR SPECIFIC PART
	// Insert application specific members and method deifinitions here

private:

	typedef std::list< Entity > Entities;
	typedef Entities::iterator Entity_Iterator;
	typedef Entities::const_iterator Entity_Const_Iterator;

	typedef std::list< Relation > Relations;
	typedef Relations::iterator Relation_Iterator;
	typedef Relations::const_iterator Relation_Const_Iterator;

	typedef FCO ObjPointer;
	typedef std::map< ObjPointer, ObjPointer> RealMap;
	typedef std::map< ObjPointer, ObjPointer>::iterator RealMap_Iterator;
	typedef std::map< ObjPointer, std::set < ObjPointer > > EquivBag;
	typedef std::map< ObjPointer, std::set < ObjPointer > >::iterator EquivBag_Iterator;

private: // members
	std::string m_projectName;

	Entities m_entities;
	//Entities m_toBeDeletedEntities;
	Relations m_relations;
	Relations m_equivRelations;
	RealMap m_realObj;

	EquivBag m_equivBag;

	std::set<Model> m_setOfParShs;

private: // methods
	void initMembers( Project& project);
public:
	void finiMembers();

private:
	void scanProject( Project& project);
	void scanModels( const Model& model, const Folder& parent);
	void scanSubModels( const Model& model);
	void scanSubFolders( const std::set<Folder>& folders, std::set<Folder>& result);
	void entityBuilder( const Model& model, const Folder& parent);

	void selectFromSameAspectsFolders();
	void proxyFinder();
	void equivalenceFinder();
	void removeProxiesAndEquiv();
	int isProxy( const ObjPointer& ptr);
	void operandSearchAndReplace( const std::vector<ObjPointer>& proxy_obj, const ObjPointer& real_obj);
	void operandSearchAndReplace( const ObjPointer& proxy_obj, const ObjPointer& real_obj);
	void insertIntoEquivBag( const ObjPointer& obj1, const ObjPointer& obj2);
	void markEquivEntities();
	Entity entitySearch( const ObjPointer& p_ptr);
	bool checkForProxies();
	
	void CHECK();

private:
	Sheet * m_sheet;
	void createSheet(); // TODO: modify this method to create your specific object
	bool populateSheet( Project& project );
	bool executeSheet();
	void deleteSheet();

	void inheritancesManager( Relation & );
	void refersToManager( Relation & );
	void associationClassManager( Relation & );
	void setMemberManager( Relation & );
	void containmentManager( Relation & );
	void folderContainmentManager( Relation & );
	void hasAttributeManager( Relation & );

	//==============================================================
	// BON2 SPECIFIC PART
	// Do not modify anything below

	// Member variables
	public :
		Project 	m_project;
		bool		m_bIsInteractive;

	public:
		Component();
		~Component();

	public:
		void initialize( Project& project );
		void finalize( Project& project );
		void invoke( Project& project, const std::set<FCO>& setModels, long lParam );
		void invokeEx( Project& project, FCO& currentFCO, const std::set<FCO>& setSelectedFCOs, long lParam );
		void objectInvokeEx( Project& project, Object& currentObject, const std::set<Object>& setSelectedObjects, long lParam );
		Util::Variant getParameter( const std::string& strName );
		void setParameter( const std::string& strName, const Util::Variant& varValue );

	#ifdef GME_ADDON
		void globalEventPerformed( globalevent_enum event );
		void objectEventPerformed( Object& object, unsigned long event, VARIANT v );
	#endif
};

}; // namespace BON

#endif // Bon2Component_H