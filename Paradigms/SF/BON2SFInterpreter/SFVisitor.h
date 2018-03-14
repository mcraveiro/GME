#ifndef SFVISITOR_H
#define SFVISITOR_H

#include "BON.h"
#include "BONImpl.h"
#include "Extensions.h"
#include "SFBonExtension.h"

namespace SF_BON
{

class SFVisitor
	: public BON::Visitor
{
	public :
		void visitObjectImpl( const BON::Object& object );
		void visitAtomImpl( const BON::Atom& atom );
		void visitConnectionImpl( const BON::Connection& conn);
		void visitModelImpl( const BON::Model& model );
		void visitFolderImpl( const BON::Folder& fold );
	private :
		bool visitParameterBase( const ParameterBase& object );
		bool visitSignal( const Signal& object );
		bool visitParam( const Param& object );
		bool visitInputParam( const InputParam& object );
		bool visitOutputParam( const OutputParam& object );
		bool visitOutputSignal( const OutputSignal& object );
		bool visitInputSignal( const InputSignal& object );
		bool visitProcessing( const Processing& object );
		bool visitCompound( const Compound& object );
		bool visitPrimitive( const Primitive& object );
		bool visitDataflowConn( const DataflowConn& object );
		bool visitParameterConn( const ParameterConn& object );
		bool visitFolder( const Folder& object );
};

}; // namespace

#endif // SFVISITOR_H
