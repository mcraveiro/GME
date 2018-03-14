#include "stdafx.h"
#include "MetaGMEVisitor.h"
#include "NmspSpec.h"

namespace MetaBON
{

MetaGMEVisitor::MetaGMEVisitor()  { }

MetaGMEVisitor::~MetaGMEVisitor() { }

// virtual
void MetaGMEVisitor::visitFolderImpl( const BON::Folder& folder) // rootfolder's accept will call this
{
	if( folder->getFolderMeta().name() == "RootFolder")
	{
		std::string nmsp = "";
		BON::RegistryNode rn = folder->getRegistry()->getChild( "Namespace");
		if( rn) nmsp = rn->getValue();
		else    nmsp = "";

		m_data.push_back( std::make_pair( folder, nmsp));

		//folder->getProject()->consoleMsg( folder->getName(), MSG_INFO);
	}
}

void MetaGMEVisitor::showOptions()
{
	std::string msg;
	for( VectorOfPairs::const_iterator it = m_data.begin()
		; it != m_data.end()
		; ++it)
	{
		msg += it->first->getName() + " : " + it->second + "<br>";
	}

	if( !msg.empty())
	{
		//m_data.begin()->first->getProject()->consoleMsg( msg, MSG_INFO);
	}

	NmspSpec dlg;
	dlg.setData( m_data);
	if( dlg.DoModal() == IDOK)
	{
		dlg.getData( m_data);

		// write back the data
		for( VectorOfPairs::const_iterator it = m_data.begin()
			; it != m_data.end()
			; ++it)
		{
			if( !it->first->isInLibrary()) // libraries nested in libraries can't be written
				it->first->getRegistry()->getChild( "Namespace")->setValue( it->second);
		}
	}
}

} // namespace

