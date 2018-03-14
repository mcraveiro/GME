#ifndef FCO_H
#define FCO_H

#include "Any.h"
#include "FCO.h"
#include "AttributeRep.h"

#include "vector"
#include "string"
#include "map"

#include "Method.h"
class ModelRep;
class ReferenceRep;
#include "RoleRep.h"

class FCO : public Any
{
public: // constant strings
	static const std::string IsAbstract_str;//"IsAbstract"
public: // typedefs
	typedef std::vector<ModelRep *> ModelRepPtrList;
	typedef std::vector<ModelRep *>::iterator ModelRepPtrList_Iterator;
	typedef std::vector<ModelRep *>::const_iterator ModelRepPtrList_ConstIterator;

	typedef std::vector<ReferenceRep *> ReferenceRepList;
	typedef std::vector<ReferenceRep *>::const_iterator ReferenceRepList_ConstIterator;
	typedef std::vector<ReferenceRep *>::iterator ReferenceRepList_Iterator;

	typedef std::vector<AttributeRep *> AttributeRepPtrList;
	typedef std::vector<AttributeRep *>::iterator AttributeRepPtrList_Iterator;
	typedef std::vector<AttributeRep *>::const_iterator AttributeRepPtrList_ConstIterator;

	typedef enum
	{
		REGULAR,
		INTERFACE,
		IMPLEMENTATION,
		NUMBER_OF_INHERITANCES // = 3
	} INHERITANCE_TYPE;

public:
	FCO( BON::FCO& ptr, BON::FCO& resp_ptr);
	virtual ~FCO(); 
	virtual KIND_TYPE getMyKind() const = 0;

	/*virtual*/ std::string getName() const;

	/*virtual*/ void initAttributes();
	// abstract
	bool isAbstract() const;
	void abstract( bool is);

	// stores the references that refer to "this"
	void addRefersToMe( ReferenceRep * ref_obj);
	// gives back the reference list
	const ReferenceRepList& getReferences() const;
	
	// stores the final references that refer to "this"
	void addFinalRefersToMe( ReferenceRep * ref_obj);
	// gives back the final reference list
	const ReferenceRepList& getFinalReferences() const;

	// return all references (even those which are references to reference to "this")
	ReferenceRepList getTransitiveReferencesToMe() const;
	
	// stores the information that this is part of a ModelRep
	void iAmPartOf(ModelRep * mod_ptr); // nn
	void iAmPartOfFinal(ModelRep * mod_ptr);

	// which models is this part of
	const ModelRepPtrList& modelsIAmPartOf(); // nn
	const ModelRepPtrList& modelsIAmPartOfFinal();

	bool amIPartOf(const ModelRep * mod_ptr) const; // nn
	bool amIPartOfFinal(const ModelRep * mod_ptr) const;

	// checker if this is part of any model
	bool checkIsPartOf(); // nn
	bool checkIsPartOfFinal();

	// checker
	bool checkInheritance();

	//
	// Inheritance related methods
	//

	// adds one parent to the parent list
	void addParent( INHERITANCE_TYPE type, FCO * ptr);
	// adds one child to the children list
	void addChild( INHERITANCE_TYPE type,  FCO * ptr);
	// get all parents
	const std::vector<FCO *>&  getParents( INHERITANCE_TYPE type) const;
	// get all children
	const std::vector<FCO *>& getChildren( INHERITANCE_TYPE type) const;

	// sets the Ancestors and Descendants lists (currently only the INTERFACE and IMPLEMENTATION)
	void setAncestors( INHERITANCE_TYPE type, const std::vector<FCO*> &);
	void setDescendants( INHERITANCE_TYPE type, const std::vector<FCO*> &);

	// the desc's/ancest's who share the same intface (REGULAR and INTERFACE combined)
	void getIntDescendants( std::vector<FCO*> & descendants) const;
	void getIntAncestors( std::vector<FCO*> & ancestors) const;

	// the desc's/ancest's who should have the same constraints and attributes (REGULAR and IMPLEMENTATION combined)
	void getImpDescendants( std::vector<FCO*> & descendants) const;
	void getImpAncestors( std::vector<FCO*> & ancestors) const;
	
	// finds out if "this" has "par" as a "type" parent
	bool hasParent( const FCO * par, INHERITANCE_TYPE type) const;
	// finds out the same, but in strict sense ( no combining of REGULAR and other)
	bool hasExactParent( const FCO * par, INHERITANCE_TYPE type) const;

	std::vector<FCO *> getAllChildren() const;
	std::vector<FCO *> getAllParents() const;

	std::vector<FCO *> getAllAncestors() const;
	std::vector<FCO *> getAllDescendants() const;

	void getRootDescsUntilNonFcoRep( std::vector< FCO *> & family);
	bool hasParentOfSameKind();
	void getAllInMyHierarchy( std::vector< FCO *>& family);

	// which level in the hierarchy (INT or IMP only!!!)
	void setLevel( INHERITANCE_TYPE type, int);
	int getLevel( INHERITANCE_TYPE type);

	// which clique is the fco part of
	void setCliqueId( INHERITANCE_TYPE type, int);
	int getCliqueId( INHERITANCE_TYPE type);

	// sgets the closest extended ancestor
	void setExtedAnc( FCO * ptr);
	FCO * getExtedAnc() const;
	FCO* findRspPtr( const std::string& responsible);

	// adds a non-extended descendant which has to be included in the IMPLEMENT_BONEXT macro (its kind name)
	void addNonExtedDesc( FCO * ptr);
	const std::vector<FCO*>& getNonExtedDescVector() const;
	std::string dumpNonExtedDescKinds() const;

	// multiple and virtual inheritance detectors
	bool multipleInheritanceStep1();
	bool multipleInheritanceStep2();
	void showMultipleInheritance();

	const std::vector<FCO *> & getMultipleBaseClasses() const;
	void addMultipleBaseClasses( const std::vector<FCO *>& );

	const std::vector<FCO *> & getVirtualBaseClasses() const;
	void addVirtualBaseClasses( FCO * );
	void addVirtualBaseClasses( const std::vector<FCO *>& );

	// attributes of "this"
	void addInitialAttribute( AttributeRep * attr);
	const AttributeRepPtrList& getInitialAttributeRepPtrList() const;

	// dumpers

	/*virtual*/ void prepare();
	/*virtual*/ void prepareMacros();
	void prepareAttributes();

	void addClassGlobal( const std::string& m) { m_classGlobalPart += m; }
	void addConnMethod( Method& m) { m_connectionMethods.push_back( m); }
	void dumpConnMethods( std::string & h_file, std::string & c_file);
	void dumpAttrMethods( std::string & h_file, std::string & c_file);

	/*virtual*/ void dumpPre( std::string & h_file, std::string & c_file);
	/*virtual*/ std::string dumpClassHeader();
	void dumpFCO( std::string & h_file, std::string & c_file);

	virtual std::string hideAndExpose();
	virtual std::string expose( const std::string& repl); //specialize this in the ModelRep, SetRep, ReferenceRep
	virtual std::string hide(); //specialize this in the ModelRep, SetRep, ReferenceRep

	static std::vector<FCO *> intersect( const std::vector<FCO*>&, const std::vector<FCO*>&);
	static std::vector<FCO *> lcdIntersect( const std::vector<FCO*>&);
	static std::string lcdKindIntersect( const std::vector<FCO*>&);
	static bool equal( std::vector<FCO*>&, std::vector<FCO*>&);

protected:
	// the name responsible in case of equivalences
	BON::FCO m_respPointer;

	// the IsAbstract attribute value
	bool m_isAbstract;

	// store the list of references which refer to this
	std::vector<ReferenceRep *> m_references;
	// store the list of references which refer to this and to ancestors
	std::vector<ReferenceRep *> m_finalReferences;

	// models this FCO is Part OF (Containment)
	ModelRepPtrList m_partOf;
	ModelRepPtrList m_partOfFinal;

	// attributes
	AttributeRepPtrList m_initialAttributeList;

	// stores the direct descendant list according to 
	// Regular, Interface and Interface inheritance types
	std::vector<FCO *> m_childList[ NUMBER_OF_INHERITANCES ];
	
	// stores the direct ancestors
	std::vector<FCO *> m_parentList[ NUMBER_OF_INHERITANCES ];

	// stores all ancestors : currently REGULAR is empty, using it causes assertion
	std::vector<FCO*> m_ancestors[ NUMBER_OF_INHERITANCES ];

	// stores all descendants : currently REGULAR is empty, using it causes assertion
	std::vector<FCO*> m_descendants[ NUMBER_OF_INHERITANCES ];

	// the level the FCO stays in the inheritance hierarchy
	// initially the value is -1
	// 0 stands for the root level and so on
	// currently REGULAR is empty, using it causes an assertion
	int m_level[ NUMBER_OF_INHERITANCES ]; 


	// the clique (from inheritance point of view) the fco is part of
	// currently REGULAR is empty, using it causes an assertion
	int m_clique[ NUMBER_OF_INHERITANCES ]; ;

/*
       X
       |
 C1    A     C2
   \ / | \  /
    B1 F  B2
    |\   /
    |  D
     \ |
       E

D.m_multipleBaseClasses = A, X
E.m_multipleBaseClasses = A, X, B1, C1

B1.m_virtualBaseClasses = A !!!!
B2.m_virtualBaseClasses = A 
E.m_virtualBaseClasses = B1
D.m_virtualBaseClasses = B1
*/	
	
	std::vector<FCO*> m_multipleBaseClasses;
	std::vector<FCO*> m_virtualBaseClasses;

	std::string m_classGlobalPart;

	std::vector<Method> m_connectionMethods;
	std::vector<Method> m_attributeMethods;

	// stores the youngest ancestor which is extended
	FCO * m_extedAnc;
	// stores the descendants which have this as the extedAnc (the youngest ancestor which is extended)
	std::vector<FCO *> m_nonExtedDescs;

	// protected methods
	std::string exposeConnMethods();
	std::string exposeAttrMethods( const std::string& repl_container);
	std::string hideConnMethods();
	std::string hideAttrMethods();
	

private: // forbiding copy
	FCO( const FCO &);
	const FCO& operator=( const FCO&);

};
#endif //FCO_H
