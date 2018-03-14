#pragma once

#include "BON.h"
#include "BONImpl.h"
#include "Extensions.h"
#include "MetaGMEBonX.h"

namespace MetaBON
{

class MetaGMEVisitor
	: public BON::Visitor
{
	public:
		typedef std::pair< BON::Folder, std::string >     FolderNamespPair;
		typedef std::vector< FolderNamespPair >           VectorOfPairs;


	protected:
		VectorOfPairs                                     m_data;

	public : 
		MetaGMEVisitor();
		virtual ~MetaGMEVisitor();

		void showOptions();

	public :
		virtual void visitFolderImpl( const BON::Folder& folder);
};

} // namespace
