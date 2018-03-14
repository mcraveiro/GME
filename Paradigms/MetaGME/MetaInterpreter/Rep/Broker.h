#ifndef BROKER_H
#define BROKER_H

#include "BON.h"
#include "BONImpl.h"
#include "Common.h"
#include "list"
#include "string"


class Broker
{
public:
	static const int INVALID_METAREF;
	static const int INITIAL_METAREF;
	static const std::string ROOTFOLDER_METAREF_STR;

	class MetaRefNode {
	public:
		MetaRefNode( BON::Object o = BON::Object(), BON::Folder f = BON::Folder(), const std::string &p = std::string(""), int m = INVALID_METAREF) : obj(o), folder(f), path(p), metaref(m) {};
		BON::Object obj;
		BON::Folder folder;
		std::string	path;
		int metaref;
	};

	static void reset();
	static void init();
	static int getNextConstraintId();
	static int getNextMetaRefId();

	static std::string getRegistryTokenName( const BON::Object& obj);
	static void initFromObj( BON::Object& obj, BON::Folder& folder, const std::string& kind);
	static void initFromAspectObj( BON::Object& obj, const std::string& name, BON::Folder& folder);
	static void initFromRegistry( BON::Object& obj, BON::Folder& folder, const BON::RegistryNode& regNode);

	static void checkDuplicates();


private:
	typedef std::list<MetaRefNode> MetaRefDB;
	typedef std::list<MetaRefNode>::iterator MetaRefDB_Iterator;
	static MetaRefDB m_metaRefDB;
	static int m_firstFree;

	static int m_constraintId;
	static int m_metaRefId;
};

#endif