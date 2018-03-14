#ifndef ROOTFOLDER_H
#define ROOTFOLDER_H

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

class RootFolder 
{
public: // types
	typedef std::vector<FCO*>::iterator FCO_Iterator;
	typedef std::vector<FCO*>::const_iterator FCO_ConstIterator;
	typedef std::vector<FolderRep*>::const_iterator SubFolder_ConstIterator;

public:
	RootFolder() 
	{ 
		m_subFolderList.clear(); 
		m_fcoList.clear(); 
	}

	~RootFolder() 
	{ 
		m_subFolderList.clear(); 
		m_fcoList.clear(); 
	}

	void addRootElement( FCO * ptr);
	bool isInRoot( FCO * ptr) const;

	void addSubFolder( FolderRep * ptr);

protected:
	std::vector<FCO *> m_fcoList;
	std::vector<FolderRep*> m_subFolderList;

private: // forbiding copy
	RootFolder( const RootFolder&);
	void operator=( const RootFolder&);

};
#endif //ROOTFOLDER_H