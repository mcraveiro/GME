#ifndef ANY_H
#define ANY_H

#include "BON.h"
#include "BONImpl.h"

#include "Any.h"
#include "string"
#include "vector"
#include "logger.h"

/** This class is the abstract base of all kinds of parts/FCOs that can occur during a modeling process. */
class Any;
class ConstraintRep;

class AnyLexicographicSort
{
public:
  bool operator()( Any * op1, Any * op2) const;
};

class Any 
{
public: // constant strings
	static const std::string NamespaceDelimiter_str;//"::"
	static const std::string InRootFolder_str;//"InRootFolder"
	static const std::string DisplayedName_str;//"DisplayedName"
	static const std::string GeneralPreferences_str;//"GeneralPreferences"

	static const std::string NameSelectorNode_str;
	static const std::string DisplayedNameSelectorNode_str;
public: // types
	typedef enum
	{
		ATOM,
		MODEL,
		CONN,
		SET,
		REF,
		FCO_REP, // abstract term only
		ASPECT,
		FOLDER,
		CONSTRAINT,
		CONSTRAINTFUNC,
		ATTRIBUTE
	} KIND_TYPE;

	const static std::string KIND_TYPE_STR[];/* =
		{
		"ATOM",	// defined in Any.cpp If the order changes update there please
		"MODEL",
		"CONN",
		"SET",
		"REF",
		"FCO",
		"ASPECT",
		"FOLDER",
		"CONSTRAINT",
		"CONSTRAINFUNC",
		"ATTRIBUTE"
		};*/

	typedef std::vector< ConstraintRep* > ConstraintRepPtrList;
	typedef std::vector< ConstraintRep* >::const_iterator ConstraintRepPtrList_ConstIterator;

public:
	Any( BON::FCO& ptr);
	virtual ~Any();

public:	
	virtual void initAttributes() {}
	void initNamespace();
	void resetNamespace();
	std::string getNamespace() const;

	virtual KIND_TYPE getMyKind() const = 0;
	std::string getMyKindStr() const;
	virtual std::string doDump() = 0;
	virtual std::string getName() const;
	virtual std::string getDispName() const;
	virtual std::string dumpDispName() const;
	std::string dumpGeneralPref();
	virtual std::vector<Any*> getGeneralPrefAncestors() { return std::vector<Any*>(); }

	std::string dumpConstraints();

	BON::FCO getPtr() const { return m_ptr; }

	bool isInRootFolder();

	BON::Folder getParentFolder() const { return m_parentFolder; }
	void setParentFolder( const BON::Folder& writableRegFolder, const BON::Folder& nmspHolderFolder) { m_parentFolder = writableRegFolder; m_nmspRootFolder = nmspHolderFolder;}
	void setEquivPeers( const std::set< BON::FCO >& ps) { m_equivs = ps; }
	void setDisplayedName( const std::string& dispnm) { m_userSelectedDisplayName = dispnm; }

	virtual std::string getMyPrefix() const;
	virtual BON::RegistryNode getMyRegistry() const;
	BON::RegistryNode getMyRegistryOld() const;

	static std::string getMyPrefix( BON::FCO& fco, BON::Folder& f);
	static BON::RegistryNode getMyRegistry( BON::FCO& fco, BON::Folder& f);

	bool isFCO() const;
	std::string askMetaRef(const std::string & tok = "") const;

	// constraints
	void addInitialConstraintRep( ConstraintRep * );
	const ConstraintRepPtrList& getInitialConstraintRepPtrList() const;
	void addFinalConstraint( ConstraintRep * constraint);
	void addFinalConstraintList( const ConstraintRepPtrList & list);

protected:
	// pointer of the BON object
	BON::FCO m_ptr;

	// pointer of the parent folder
	// this must be writable because
	// the registry write operations
	// work through this ptr
	BON::Folder m_parentFolder;

	// namespace info taken from this folder
	// typically same with m_parentFolder
	BON::Folder m_nmspRootFolder;

	// equivalent peers
	std::set< BON::FCO > m_equivs;
	bool	m_isInRootFolder;
	std::string m_sAttrGenPref;

	// displayed name (if name selection dialog used) 
	std::string m_userSelectedDisplayName;

	// initial and final constraints
	ConstraintRepPtrList m_initialConstraintList;
	ConstraintRepPtrList m_finalConstraintList;

	// part of this namespace:
	std::string m_namespace;

private: // forbiding copy
	Any( const Any&);
	const Any& operator=( const Any&);
};

#endif //ANY_H
