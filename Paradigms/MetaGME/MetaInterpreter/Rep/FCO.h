#ifndef FCO_H
#define FCO_H

#include "Any.h"
#include "FCO.h"
#include "AttributeRep.h"

#include "vector"
#include "string"
#include "map"
#include <algorithm>
#include <iterator>

class ModelRep;
class ReferenceRep;
#include "RoleRep.h"

class FCO : public Any
{
public: // constant strings
	static const std::string IsAbstract_str;//"IsAbstract"
	static const std::string Icon_str;//"Icon"
	static const std::string PortIcon_str;//"PortIcon"
	static const std::string Decorator_str;//"Decorator"
	static const std::string IsHotspotEnabled_str;//"IsHotspotEnabled"
	static const std::string IsTypeShown_str;//"IsTypeShown"
	static const std::string NamePosition_str;//"NamePosition"
	static const std::string SubTypeIcon_str;//"SubTypeIcon"
	static const std::string InstanceIcon_str;//"InstanceIcon"
	static const std::string NameWrapNum_str;//"NameWrapNum"
	static const std::string IsNameEnabled_str;//"IsNameEnabled"
	static const std::string IsResizable_str;//"IsResizable"
	static const std::string AutoRouterPref_str;//"AutoRouterPref"
	static const std::string HelpURL_str;//"HelpURL"
	static const std::string ShowPorts_str;//"ShowPorts"

	static const std::string IsGradientFillEnabled_str;//"IsGradientFillEnabled"
	static const std::string GradientFillColor_str;//"GradientFillColor"
	static const std::string GradientFillDirection_str;//"GradientFillDirection"
	static const std::string IsShadowCastEnabled_str;//"IsShadowCastEnabled"
	static const std::string ShadowColor_str;//"ShadowColor"
	static const std::string ShadowThickness_str;//"ShadowThickness"
	static const std::string ShadowDirection_str;//"ShadowDirection"
	static const std::string IsRoundRectangleEnabled_str;//"IsRoundRectangleEnabled"
	static const std::string RoundRectangleRadius_str;//"RoundRectangleRadius"

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

	/*virtual*/ void initAttributes();
	/*virtual*/ std::string getName() const;
	/*virtual*/ std::string getDispName() const;

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
	const std::vector<FCO *>&  getParents( INHERITANCE_TYPE type);
	// get all children
	const std::vector<FCO *>& getChildren( INHERITANCE_TYPE type);

	// sets the Ancestors and Descendants lists (currently only the INTERFACE and IMPLEMENTATION)
	void setAncestors( INHERITANCE_TYPE type, const std::vector<FCO*> &);
	void setDescendants( INHERITANCE_TYPE type, const std::vector<FCO*> &);

	// the desc's/ancest's who share the same intface (REGULAR and INTERFACE combined)
	void getIntDescendants( std::vector<FCO*> & descendants) const;
	void getIntAncestors( std::vector<FCO*> & ancestors) const;

	// the desc's/ancest's who should have the same constraints and attributes (REGULAR and IMPLEMENTATION combined)
	void getImpDescendants( std::vector<FCO*> & descendants) const;
	void getImpAncestors( std::vector<FCO*> & ancestors) const;
	virtual std::vector<Any*> getGeneralPrefAncestors() {
		std::vector<FCO*> ret;
		getImpAncestors(ret);
		std::vector<Any*> retAny;
		std::copy(ret.begin(), ret.end(), std::back_inserter(retAny));
		return retAny;
	}

	// finds out if "this" has "par" as a "type" parent
	bool hasParent( const FCO * par, INHERITANCE_TYPE type) const;

	// attributes of "this"
	void addInitialAttribute( AttributeRep * attr);
	const AttributeRepPtrList& getInitialAttributeRepPtrList() const;

	void addFinalAttribute( AttributeRep * attr);
	void addFinalAttributeList(const AttributeRepPtrList& t_list);
	bool findFinalAttributeBasedOnName( const std::string & name);

	std::string dumpAttributes();
	std::string dumpAttributeList( bool check_viewable = false);
	std::string dumpIcon();
	std::string dumpPortIcon();
	std::string dumpDecorator();
	std::string dumpHotspotEnabled();
	std::string dumpTypeShown();
	std::string dumpNamePosition() const;
	std::string dumpSubTypeIcon() const;
	std::string dumpInstanceIcon() const;
	std::string dumpNameWrap() const;
	std::string dumpNameEnabled() const;
	std::string dumpResizable() const;
	std::string dumpAutoRouterPref() const;
	std::string dumpHelpURL() const;

	std::string dumpGradientFillEnabled() const;
	std::string dumpGradientFillColor() const;
	std::string dumpGradientFillDirection() const;
	std::string dumpShadowCastEnabled() const;
	std::string dumpShadowColor() const;
	std::string dumpShadowThickness() const;
	std::string dumpShadowDirection() const;
	std::string dumpRoundRectangleEnabled() const;
	std::string dumpRoundRectangleRadius() const;


protected:
	/**
	 * This pointer is in charge of the name, 
	 * may point to an FCO or an Equivalence operator
	 */
	BON::FCO m_respPointer;
	
	// the IsAbstract attribute value
	bool m_isAbstract;

	bool m_bAttrIsHotspotEnabled;
	bool m_bAttrIsTypeShown;
	bool m_bAttrIsNameEnabled;
	bool m_bAttrIsResizable;

	int m_iAttrNamePosition;
	int m_iAttrNameWrapNum;

	std::string m_sAttrDispName;
	std::string m_sAttrIcon;
	std::string m_sAttrPortIcon;
	std::string m_sAttrSubTypeIcon;
	std::string m_sAttrInstanceIcon;
	std::string m_sAttrDecorator;
	std::string m_sAttrAutoRouterPref;
	std::string m_sAttrHelpURL;

	bool m_bAttrIsGradientFillEnabled;
	COLORREF m_iAttrGradientFillColor;
	int m_iAttrGradientFillDirection;
	bool m_bAttrIsShadowCastEnabled;
	COLORREF m_iAttrShadowColor;
	int m_iAttrShadowThickness;
	int m_iAttrShadowDirection;
	bool m_bAttrIsRoundRectangleEnabled;
	int m_iAttrRoundRectangleRadius;

	// store the list of references which refer to this
	std::vector<ReferenceRep *> m_references;
	// store the list of references which refer to this and to ancestors
	std::vector<ReferenceRep *> m_finalReferences;

	// models this FCO is Part OF (Containment)
	ModelRepPtrList m_partOf;
	ModelRepPtrList m_partOfFinal;

	// attributes
	AttributeRepPtrList m_initialAttributeList;
	AttributeRepPtrList m_finalAttributeList;

	// stores the direct descendant list according to 
	// Regular, Interface and Interface inheritance types
	std::vector<FCO *> m_childList[ NUMBER_OF_INHERITANCES ];
	
	// stores the direct ancestors
	std::vector<FCO *> m_parentList[ NUMBER_OF_INHERITANCES ];

	// stores all ancestors : currently REGULAR is empty, using it causes assertion
	std::vector<FCO*> m_ancestors[ NUMBER_OF_INHERITANCES ];

	// stores all descendants : currently REGULAR is empty, using it causes assertion
	std::vector<FCO*> m_descendants[ NUMBER_OF_INHERITANCES ];

private: // forbiding copy
	FCO( const FCO &);
	const FCO& operator=( const FCO&);

};
#endif //FCO_H
