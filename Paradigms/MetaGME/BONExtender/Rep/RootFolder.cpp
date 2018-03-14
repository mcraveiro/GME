#include "stdafx.h"

#include "RootFolder.h"
#include "Any.h"

#include "algorithm"

extern int ind;

void RootFolder::addRootElement( FCO * ptr)
{
	m_fcoList.push_back( ptr);
}


bool RootFolder::isInRoot( FCO * ptr) const
{
	FCO_ConstIterator fco_it = std::find( m_fcoList.begin(), m_fcoList.end(), ptr);
	return fco_it != m_fcoList.end();
}


void RootFolder::addSubFolder( FolderRep *ptr)
{
	m_subFolderList.push_back( ptr);
}


