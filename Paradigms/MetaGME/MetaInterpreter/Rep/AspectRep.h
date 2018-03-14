#ifndef ASPECTREP_H
#define ASPECTREP_H

#include "Any.h"
#include "PartRep.h"

#include "map"
#include "vector"
#include "FCO.h"

class PartRep;
class RoleRep;
class AspectRep;
class ModelRep;

class PartCompare
{
public:
	bool operator()( const PartRep& peer1, const PartRep& peer2) const;
};

class AspectCompare
{
public:
	bool operator()( const AspectRep* peer1, const AspectRep* peer2) const;
};

class AspNamePriorityComp
{
public:
	typedef std::pair< std::string, int> Type;
	bool operator() ( const Type& p1, const Type& p2) const;
};

class AspectRep : public Any 
{
public: // types
	typedef std::vector< PartRep > PartRepSeries;
	typedef PartRepSeries::iterator PartRepSeries_Iterator;
	typedef PartRepSeries::const_iterator PartRepSeries_ConstIterator;
	typedef ModelRep* Key;
	typedef std::map< Key, PartRepSeries> ModelPartsMap;
	typedef ModelPartsMap::iterator ModelPartsMap_Iterator;
public:
	AspectRep( BON::FCO& ptr, BON::FCO& resp_ptr);
	~AspectRep();
	/*virtual*/ std::string doDump();
	/*virtual*/ Any::KIND_TYPE getMyKind() const;
	/*virtual*/ std::string getName() const;
	/*virtual*/ std::string getDispName() const;
	/*virtual*/ bool getReadOnly() const;

	/*virtual*/ std::string getMyPrefix() const;
	/*virtual*/ BON::RegistryNode getMyRegistry() const;
	static std::string getMyPrefix( BON::FCO& fco, BON::Folder& f);
	static BON::RegistryNode getMyRegistry( BON::FCO& fco, BON::Folder& f);

	void addPart2Map( ModelRep * mptr, const PartRep & part);

	void addFCO( FCO* fco); // to the initial list
	void addRRole( RoleRep & role);
	bool findFinalFCO( const FCO * fco) const; // in the final list
	bool findFinalRole( const RoleRep&) const;

	void extendAspectWithDescendants();
	void sortPartMap();

	const PartRepSeries& getPartSeries( ModelRep* mod_ptr);
	bool lessThan( const AspectRep& rep) const;
	void getXY( unsigned int * x, unsigned int *y) const;

	void setPriority( int pr);
	int getPriority() const;

	bool isDummy() const;
	
	static const std::string m_aspectRegistryRoot;
	static const std::string m_aspectMetaRefsRoot;
	static const std::string m_aspectNamesRoot;
protected:
	// this int is set if contains non-zero value
	// and if one of the aspects contains non-zero value it means that the aspects are
	// sorted based on this
	int		m_priority;
	
	/**
	 * inital and final list of FCO-s intended to be part of the Aspect
	 *
	 */
	std::vector<FCO*>		m_fcoInitialList;
	std::vector<RoleRep>	m_roleInitialList;
	std::vector<FCO*>		m_fcoFinalList;
	std::vector<RoleRep>	m_roleFinalList;

	ModelPartsMap			m_partsMap;

	/**
	 * This pointer is in charge of the aspect name, 
	 * NOTE: it may be an Aspect or SameAspect BON object
	 */
	BON::FCO m_respPointer;

private: // forbiding copy
	AspectRep(const AspectRep&);
	const AspectRep& operator=(const AspectRep&);
};
#endif //ASPECTREP_H
