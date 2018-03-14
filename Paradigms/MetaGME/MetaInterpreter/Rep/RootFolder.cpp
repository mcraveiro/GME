#include "stdafx.h"

#include "RootFolder.h"
#include "Broker.h"
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


bool RootFolder::isEmpty() const
{
	return m_fcoList.empty() && m_subFolderList.empty();
}


void RootFolder::addSubFolder( FolderRep *ptr)
{
	m_subFolderList.push_back( ptr);
}


std::string RootFolder::headDump() 
{ 
	++ind;
	std::string mmm = indStr() + "<folder name = \"RootFolder\" metaref = \"" + Broker::ROOTFOLDER_METAREF_STR + "\" ";

	AnyLexicographicSort lex;
	std::sort( m_subFolderList.begin(), m_subFolderList.end(), lex);
	SubFolder_ConstIterator it_f = m_subFolderList.begin();
	if  ( it_f != m_subFolderList.end())
	{
		mmm +=" subfolders = \"";
		bool first = true;
		while( it_f != m_subFolderList.end())
		{
			if (!first) mmm += " ";
			mmm += (*it_f)->getName();
			first = false;
			++it_f;
		}
		mmm +="\"";
	}

	std::sort( m_fcoList.begin(), m_fcoList.end(), lex);

	if ( !m_fcoList.empty())
	{
		std::string nnn;
		bool first = true;
		FCO_ConstIterator it = m_fcoList.begin();
		while ( it != m_fcoList.end())
		{
			if ((*it)->isFCO() && !(*it)->isAbstract())
			{
				if ( !first) nnn += " ";
				nnn += (*it)->getName();
				first = false;
			}
			++it;
		}
		
		if ( !nnn.empty()) 
			mmm += " rootobjects = \"" + nnn + "\"";
	}
	mmm += " >\n";
	return mmm;
}


std::string RootFolder::tailDump() 
{ 
	--ind;
	std::string mmm;
	mmm = indStr() + "</folder>\n";

	return mmm;
}
