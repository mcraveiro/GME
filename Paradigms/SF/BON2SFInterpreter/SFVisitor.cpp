#include "stdafx.h"
#include "SFVisitor.h"


namespace SF_BON
{

bool SFVisitor::visitParameterBase( const ParameterBase& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitSignal( const Signal& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitParam( const Param& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitInputParam( const InputParam& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitOutputParam( const OutputParam& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitOutputSignal( const OutputSignal& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitInputSignal( const InputSignal& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitProcessing( const Processing& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitCompound( const Compound& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitPrimitive( const Primitive& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitDataflowConn( const DataflowConn& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitParameterConn( const ParameterConn& object )
{
  if ( !object)
    return false;

  return true;
}


bool SFVisitor::visitFolder( const Folder& object )
{
  if ( !object)
    return false;

  return true;
}


void SFVisitor::visitObjectImpl( const BON::Object& obj )
{}


void SFVisitor::visitAtomImpl( const BON::Atom& atom )
{
	if ( !visitParameterBase( atom))
	if ( !visitSignal( atom))
	if ( !visitParam( atom))
	if ( !visitInputParam( atom))
	if ( !visitOutputParam( atom))
	if ( !visitOutputSignal( atom))
	if ( !visitInputSignal( atom))
	{
		// visiting other Atom
	}
}


void SFVisitor::visitConnectionImpl( const BON::Connection& conn )
{
	if ( !visitDataflowConn( conn))
	if ( !visitParameterConn( conn))
	{
		// visiting other Connection
	}
}


void SFVisitor::visitModelImpl( const BON::Model& model )
{
	if ( !visitProcessing( model))
	if ( !visitCompound( model))
	if ( !visitPrimitive( model))
	{
		// visiting other Model
	}
}


void SFVisitor::visitFolderImpl( const BON::Folder& fold )
{
	if ( !visitFolder( fold))
	{
		// visiting other Folder
	}
}



}; // namespace BON

