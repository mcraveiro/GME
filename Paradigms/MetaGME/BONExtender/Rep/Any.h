#ifndef ANY_H
#define ANY_H

#include "BON.h"
#include "BONImpl.h"

#include "Any.h"
#include "string"
#include "vector"
#include "logger.h"
#include <fstream>
class Method;
/** This class is the abstract base of all kinds of parts/FCOs that can occur during a modeling process. */
class Any;

class AnyLexicographicSort
{
public:
  bool operator()( Any * op1, Any * op2) const;
};

class Any 
{
public: // constant strings
	static const std::string NamespaceDispSeparatorBeg_str;//"_"
	static const std::string NamespaceDispSeparatorEnd_str;//"_"
	static const std::string NamespaceDelimiter_str;//"::"
	static const std::string InRootFolder_str;//"InRootFolder"
	static const std::string NameSelectorNode_str; //"myNameIs"

	static const std::string m_startUPToken;
	static const std::string m_endUPToken;

	static const std::string MOFException_str;//"MOFException"
	static const std::string MOFOperation_str; //"MOFOperation"
	static const std::string MOFEnumeration_str;//"MOFEnumeration"
	static const std::string MOFStructure_str; //"MOFStructure"
	static const std::string MOFAlias_str; //"MOFAlias"

	static const std::string MOFStart_str;
	static const std::string MOFEnd_str;

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

public:
	Any( BON::FCO& ptr);
	virtual ~Any();

public:	
	virtual void initAttributes();
	void initNamespace();
	void resetNamespace();

	virtual KIND_TYPE getMyKind() const = 0;
	std::string getMyKindStr() const;
	virtual std::string doDump() = 0;

	//
	// the term 'name' is used in the following ways
	// validated names: C++ typenames created from the original name, e.g. a return type must be a valid C++ identifier
	// strict names: metamodel kinds and roles (these are used upon inquiries like getChild, IMPLEMENT_BON macros, and getObjectMeta().name() == "..."
	// 
	// long names: names prefixed with the namespace
	// long strict names are meta-conforming kindnames: 'mynamespace::mykind1'
	// long validated names are valid typenames appearing in the target code: 'mymeta_BONX::mytype' or 'mynamespace::mykind1'
	// IMPORTANT: if an element in the metamodel is not part of any namespace in the generated BONX it will still be part of
	// the default main namespace (provided by the user), that is where strict names and validated names will be completely different

	virtual std::string getName() const;    // returning the selected name for a kind i.e. "1stNode"
	std::string getValidName() const;       // c++ identifier (validated name) for the kind: "_1stNode"
	std::string getValidNameImpl() const;   // "_1stNodeImpl"
	
	// short namespace
	std::string getStrictNmspc() const;     // strictly returning the MetaGME def'd namespace (i.e. "NM.1") if defined, otherwise ""
	std::string getNmspc() const;           // the target namespace: "NM.1" if specified in meta, otherwise default namespace "meta5_BONX" (or the user-provided one)
	std::string getValidNmspc() const;      // c++ identifier (validated name) for the namespace "NM.1" -> "NM_1"

	// LNames = long names: namespace + name
	std::string getLStrictName() const;     // "NM.1::1stNode" or "1stNode"
	std::string getLName() const;           // "NM.1::1stNode" or "meta5_BON::1stNode"
	std::string getLValidName() const;      // "NM_1::_1stNode"
	std::string getLValidNameImpl() const;  // "NM_1::_1stNodeImpl"

	// LNamespace = long namespace: namespace + "::"
	std::string getLStrictNmspc() const;    // "" or "NM.1::"
	std::string getLNmspc() const;          // "NM.1::" or "meta5_BONX::"
	std::string getLValidNmspc() const;     // "NM_1::"
	BON::FCO getPtr() const { return m_ptr; }
	bool isInRootFolder();

	BON::Folder getParentFolder() const { return m_parentFolder; }
	void setParentFolder( const BON::Folder& writableRegFolder, const BON::Folder& nmspHolderFolder) { m_parentFolder = writableRegFolder; m_nmspRootFolder = nmspHolderFolder;}
	void setEquivPeers( const std::set< BON::FCO >& ps) { m_equivs = ps; } 

	std::string getMyPrefix( int which = 0) const;
	BON::RegistryNode getMyRegistry() const;
	BON::RegistryNode getMIRegistry() const;
	bool isFCO() const;

	static void convertToValidName( std::string & p);
	static bool checkIfValidName( const std::string & p);

	virtual void prepare();
	virtual void prepareMacros();
	virtual void prepareIniFin();
	void         prepareMOF();
	void         createAcceptMethod( bool pWithTraversalOfKids, bool pSpecialized, bool pRetValBool);

	void dumpGlobals();
	std::string dumpOrnament( bool is_abstract = false);

	virtual void dumpPre( std::string & h_file, std::string & c_file);
	virtual void dumpPost( std::string & h_file, std::string & c_file);
	virtual std::string dumpClassHeader();


	std::string getUserPart();

	bool isToBeEx() const;
	void toBeEx( bool t);

	static std::string processMOFNode( const BON::RegistryNode& n, int ind = 2);
	static std::string parseMOFException( const std::string& exc_str);
	static std::string parseMOFOperation( const std::string& exc_str);

protected:
public:
	void makeBackup();
	void initOutS();
	void initOutH( std::string& resu);
	void finiOutS();
	void finiOutH();
protected:
	void sendOutS( const std::string& content );
	void sendOutH( const std::string& content );
	std::ofstream m_sStream;
	std::ofstream m_hStream;

protected:
	// pointer of the BON object
	BON::FCO m_ptr;

	// pointer of the parent folder
	BON::Folder m_parentFolder;

	// namespace info taken from this folder
	// typically same with m_parentFolder
	BON::Folder m_nmspRootFolder;

	// equivalent peers
	std::set< BON::FCO > m_equivs;
	bool	m_isInRootFolder;

	// indicates if it is to be extended or not
	bool m_toEx;

	std::vector< Method > m_inifinMethods;
	std::string m_sectionMOF;
	std::string m_globalHeader;
	std::string m_globalSource;

	// part of this namespace:
	std::string m_namespace;

private: // forbiding copy
	Any( const Any&);
	const Any& operator=( const Any&);
};

#endif //ANY_H
