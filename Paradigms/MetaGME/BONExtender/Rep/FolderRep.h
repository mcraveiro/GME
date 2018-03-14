#ifndef FOLDERREP_H
#define FOLDERREP_H

#include "Any.h"
#include "FCO.h"
#include "FolderRep.h"
#include "ModelRep.h"
#include "AtomRep.h"
#include "ConnectionRep.h"
#include "SetRep.h"
#include "ReferenceRep.h"
#include "FcoRep.h"

#include "list"
#include "vector"
/*
class FolderElem
{
	FolderElem( const Any * m_ptr, const std::string &m_card);
	bool operator==( const FolderElem& peer);
	Any * getElem() const;
	const std::string & getCard() const;
private:
	const FolderElem& operator=(const FolderElem& peer);
	FolderElem( const FolderElem&);

	Any * m_elem;
	std::string m_card;
};
*/
class FolderRep : public Any
{
public: // types
	typedef std::vector<FCO*>::iterator FCO_Iterator;
	typedef std::vector<FolderRep*>::iterator SubFolder_Iterator;
	typedef std::vector<FolderRep*>::const_iterator SubFolder_ConstIterator;

public:
/*
A FolderRep is created by using the id, ptr and resp_ptr which is the object 
selected using the SameFolder Selection Mechanism when SameFolder relation is met.
That is why a folder has to redefine the getName operations
*/
	FolderRep( BON::FCO& ptr, BON::FCO& resp_ptr); 
	~FolderRep();

	/*virtual*/ Any::KIND_TYPE getMyKind() const { return Any::FOLDER; }
	/*virtual*/ std::string getName() const;

	void addFCO( FCO * ptr, const std::string & card);
	bool isFCOContained( FCO * otr);

	void addSubFolderRep( FolderRep * ptr, const std::string & card);
	void extendMembership();

	std::string doDump();

	void addMethod(Method& m) { m_methods.push_back( m); }
	void createMethods();

	static std::string subFolderGetterMethodName(FolderRep * fold, const std::string& diff_nmsp);
	static std::string kindGetterMethodName( FCO * fco, const std::string& diff_nmsp);

protected:
	std::vector<FCO *> m_fcoList;
	std::vector<std::string> m_fcoCardList;

	/**
	 * contains all subfolders (including itself)
	 */
	std::vector<FolderRep*> m_subFolderList;
	/* Cardinality for the folders above */
	std::vector<std::string> m_subCardList;

	/**
	 * This pointer is in charge of the aspect name, 
	 * may point to a Folder or a SameFolder operator
	 */
	BON::FCO m_respPointer;

	std::vector<Method> m_methods;

private: // forbiding copy
	FolderRep( const FolderRep&);
	const FolderRep& operator=( const FolderRep&);

};
#endif //FOLDERREP_H
