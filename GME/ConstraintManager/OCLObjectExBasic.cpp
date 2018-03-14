//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLObjectExBasic.cpp
//
//###############################################################################################################################################
#include "StdAfx.h"
#include "Solve4786.h"
#include "OCLObjectExBasic.h"
#include "OCLType.h"
#include <algorithm>

namespace OclBasic
{

//##############################################################################################################################################
//
//	F U N C T I O N S
//
//##############################################################################################################################################

	std::string PrintCollection( const std::string& strTypeName, const OclMeta::ObjectVector& vecElements )
	{
		CString chBuffer;
		chBuffer.Format( _T("%lu"), vecElements.size() );
		return strTypeName + " { size: " + std::string( CStringA(chBuffer) ) + "; } ";
		/*
		std::string strObject = strTypeName + " { " + std::string( chBuffer ) + " , [ ";
		for ( int i = 0 ; i < vecElements.size() ; i++ ) {
			strObject = vecElements[ i ].Print();
			if ( i != vecElements.size() - 1 )
				strObject += ", ";
		}
		return strObject + " ] }";
		*/

	}

	int GetObjectCountInVector( OclMeta::ObjectVector& vecObjects, OclMeta::Object& object )
	{
		int i = 0;
		OclMeta::ObjectVector::iterator j = std::find( vecObjects.begin(), vecObjects.end(), object );
		while ( j != vecObjects.end() ) {
			i++;
			vecObjects.erase( j );
			j = std::find( vecObjects.begin(), vecObjects.end(), object );
		}
		return i;
	}

//##############################################################################################################################################
//
//	C L A S S : OclBasic::String <<< + OclBasic::Any
//
//##############################################################################################################################################

	String::String( OclMeta::TypeManager* pManager, const std::string& strTypeName, const std::string& value )
		: Any( pManager, strTypeName ), m_Value( value )
	{
	}


	String::String( OclMeta::TypeManager* pManager, const std::string& value )
		: Any( pManager, "ocl::String" ), m_Value( value )
	{
	}

	void String::GetValue( std::string& value ) const
	{
		value = m_Value;
	}

	void String::SetValue( const std::string& value )
	{
		m_Value = value;
	}

	Any* String::Clone() const
	{
		return new String( GetTypeManager(), m_Value );
	}

	bool String::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "ocl::String" ) >= 0 )
			return m_Value == ( ( String* ) &object )->m_Value;
		else
			return false;
	}

	bool String::operator==( const String& object ) const
	{
		return Equals( object );
	}

	bool String::operator!=( const String& object ) const
	{
		return ! ( *this == object );
	}

	std::string String::Print() const
	{
		return "ocl::String { \"" + m_Value + "\" }";
	}

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Enumeration <<< + OclBasic::Any
//
//##############################################################################################################################################

	Enumeration::Enumeration( OclMeta::TypeManager* pManager, const std::string& strTypeName, const std::string& value )
		: Any( pManager, strTypeName ), m_Value( value )
	{
	}

	Enumeration::Enumeration( OclMeta::TypeManager* pManager, const std::string& value )
		: Any( pManager, "ocl::Enumeration" ), m_Value( value )
	{
	}

	void Enumeration::GetValue( std::string& value ) const
	{
		value = m_Value;
	}

	void Enumeration::SetValue( const std::string& value )
	{
		m_Value = value;
	}

	Any* Enumeration::Clone() const
	{
		return new Enumeration( GetTypeManager(), m_Value );
	}

	bool Enumeration::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "ocl::Enumeration" ) >= 0 )
			return m_Value == ( ( Enumeration* ) &object )->m_Value;
		else
			return false;
	}

	bool Enumeration::operator==( const Enumeration& object ) const
	{
		return Equals( object );
	}

	bool Enumeration::operator!=( const Enumeration& object ) const
	{
		return ! ( *this == object );
	}

	std::string Enumeration::Print() const
	{
		return "ocl::Enumeration { #" + m_Value + " }";
	}

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Boolean <<< + OclBasic::Any
//
//##############################################################################################################################################

	Boolean::Boolean( OclMeta::TypeManager* pManager, const std::string& strTypeName, bool value )
		: Any( pManager, strTypeName ), m_Value( value )
	{
	}

	Boolean::Boolean( OclMeta::TypeManager* pManager, bool value )
		: Any( pManager, "ocl::Boolean" ), m_Value( value )
	{
	}

	void Boolean::GetValue( bool& value ) const
	{
		value = m_Value;
	}

	void Boolean::SetValue( bool value )
	{
		m_Value = value;
	}

	Any* Boolean::Clone() const
	{
		return new Boolean( GetTypeManager(), m_Value );
	}

	bool Boolean::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "ocl::Boolean" ) >= 0 )
			return m_Value == ( ( Boolean* ) &object )->m_Value;
		else
			return false;
	}

	bool Boolean::operator==( const Boolean& object ) const
	{
		return Equals( object );
	}

	bool Boolean::operator!=( const Boolean& object ) const
	{
		return ! ( *this == object );
	}

	std::string Boolean::Print() const
	{
		return ( m_Value ) ? "ocl::Boolean { true }" : "ocl::Boolean { false }";
	}

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Real <<< + OclBasic::Any
//
//##############################################################################################################################################

	Real::Real( OclMeta::TypeManager* pManager, const std::string& strTypeName, double value )
		: Any( pManager, strTypeName ), m_Value( value )
	{
	}

	Real::Real( OclMeta::TypeManager* pManager, double value )
		: Any( pManager, "ocl::Real" ), m_Value( value )
	{
	}

	void Real::GetValue( double& value ) const
	{
		value = m_Value;
	}

	void Real::SetValue( double value )
	{
		m_Value = value;
	}

	Any* Real::Clone() const
	{
		return new Real( GetTypeManager(), m_Value );
	}

	bool Real::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "ocl::Real" ) >= 0 ) {
			double v = ( ( Real* ) &object )->m_Value;
			return m_Value == v;
		}
		else
			return false;
	}

	bool Real::operator==( const Real& object ) const
	{
		return Equals( object );
	}

	bool Real::operator!=( const Real& object ) const
	{
		return ! ( *this == object );
	}

	std::string Real::Print() const
	{
		char chBuffer[ 100 ];
		sprintf_s( chBuffer, "%lf", m_Value );
		return "ocl::Real { " + std::string( chBuffer ) + " }";
	}

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Integer <<< + OclBasic::Real
//
//##############################################################################################################################################

	Integer::Integer( OclMeta::TypeManager* pManager, const std::string& strTypeName, long value )
		: Real( pManager, strTypeName, (double) value )
	{
	}

	Integer::Integer( OclMeta::TypeManager* pManager, long value )
		: Real( pManager, "ocl::Integer", (double) value )
	{
	}

	void Integer::GetValue( long& value ) const
	{
		double dValue;
		Real::GetValue( dValue );
		value = (long) dValue;
	}

	void Integer::SetValue( long value )
	{
		Real::SetValue( (double) value );
	}

	Any* Integer::Clone() const
	{
		long lValue;
		GetValue( lValue );
		return new Integer( GetTypeManager(), lValue );
	}

	bool Integer::operator==( const Integer& object ) const
	{
		return Real::operator==( object );
	}

	bool Integer::operator!=( const Integer& object ) const
	{
		return ! ( *this == object );
	}

	std::string Integer::Print() const
	{
		double lValue;
		Real::GetValue( lValue );
		char chBuffer[ 100 ];
		sprintf_s( chBuffer, "%ld", (long) lValue );
		return "ocl::Integer { " + std::string( chBuffer ) + " }";
	}

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Type <<< + OclBasic::Any
//
//##############################################################################################################################################

	Type::Type( OclMeta::TypeManager* pManager, const std::string& strTypeName, const std::string& value )
		: Any( pManager, strTypeName ), m_Value( value )
	{
	}


	Type::Type( OclMeta::TypeManager* pManager, const std::string& value )
		: Any( pManager, "ocl::Type" ), m_Value( value )
	{
	}

	void Type::GetValue( std::string& value ) const
	{
		value = m_Value;
	}

	void Type::SetValue( const std::string& value )
	{
		m_Value = value;
	}

	Any* Type::Clone() const
	{
		return new Type( GetTypeManager(), m_Value );
	}

	bool Type::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "ocl::Type" ) >= 0 )
			return m_Value == ( ( Type* ) &object )->m_Value;
		else
			return false;
	}

	bool Type::operator==( const Type& object ) const
	{
		return Equals( object );
	}

	bool Type::operator!=( const Type& object ) const
	{
		return ! ( *this == object );
	}

	std::string Type::Print() const
	{
		return "ocl::Type { \"" + m_Value + "\" }";
	}

//##############################################################################################################################################
//
//	C L A S S : OclBasic::ObjectIterator <<< + OclImplementation::ObjectIterator
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ObjectIterator
		: public OclImplementation::ObjectIterator
	{
		private :
			OclMeta::ObjectVector::iterator m_iteratorPos;
			OclMeta::ObjectVector::iterator m_iteratorEnd;

		public :
			ObjectIterator( OclMeta::ObjectVector::iterator iteratorBegin, OclMeta::ObjectVector::iterator iteratorEnd )
				: OclImplementation::ObjectIterator(), m_iteratorPos( iteratorBegin ), m_iteratorEnd( iteratorEnd )
			{
			}

			virtual ~ObjectIterator()
			{
			}

			virtual bool HasNext()
			{
				return m_iteratorPos != m_iteratorEnd;
			}

			virtual OclMeta::Object GetNext()
			{
				return *( m_iteratorPos++ );
			}
	};

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Set <<< + OclBasic::Collection
//
//##############################################################################################################################################

	Set::Set( OclMeta::TypeManager* pManager, const std::string& strTypeName )
		: Collection( pManager, strTypeName )
	{
	}

	Set::Set( OclMeta::TypeManager* pManager, const std::string& strTypeName, const OclMeta::ObjectVector& vecObjects )
		: Collection( pManager, strTypeName )
	{
		for ( unsigned int i = 0 ; i < vecObjects.size() ; i++ )
			AddObject( vecObjects[ i ] );
	}

	Set::Set( OclMeta::TypeManager* pManager )
		: Collection( pManager, "ocl::Set" )
	{
	}

	Set::Set( OclMeta::TypeManager* pManager, const OclMeta::ObjectVector& vecObjects )
		: Collection( pManager, "ocl::Set" )
	{
		for ( unsigned int i = 0 ; i < vecObjects.size() ; i++ )
			AddObject( vecObjects[ i ] );
	}

	Any* Set::Clone() const
	{
		return new Set( GetTypeManager(), m_vecObjects );
	}

	void Set::GetValue( OclMeta::ObjectVector& value ) const
	{
		value = m_vecObjects;
	}

	void Set::SetValue( const OclMeta::ObjectVector& value )
	{
		for ( unsigned int i = 0 ; i < value.size() ; i++ )
			AddObject( value[ i ] );
	}

	bool Set::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "ocl::Set" ) >= 0 ) {
			OclMeta::ObjectVector vecObjects = ( ( Set* ) &object )->m_vecObjects;
			if ( m_vecObjects.size() != vecObjects.size() )
				return false;
			for ( unsigned int i = 0 ; i < m_vecObjects.size() ; i++ )
				if ( std::find( vecObjects.begin(), vecObjects.end(), m_vecObjects[ i ] ) == vecObjects.end() )
					return false;
			return true;
		}
		else
			return false;
	}

	bool Set::operator==( const Set& object ) const
	{
		return Equals( object );
	}

	bool Set::operator!=( const Set& object ) const
	{
		return ! ( *this == object );
	}

	void Set::AddObject( const OclMeta::Object& object )
	{
		OclMeta::ObjectVector::iterator i = std::find( m_vecObjects.begin(), m_vecObjects.end(), object );
			if ( i == m_vecObjects.end() )
				m_vecObjects.push_back( object );
	}

	OclImplementation::ObjectIterator* Set::GetIterator()
	{
		return new ObjectIterator( m_vecObjects.begin(), m_vecObjects.end() );
	}

	std::string Set::Print() const
	{
		return PrintCollection( GetTypeName(), m_vecObjects );
	}

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Bag <<< + OclBasic::Collection
//
//##############################################################################################################################################

	Bag::Bag( OclMeta::TypeManager* pManager, const std::string& strTypeName )
		: Collection( pManager, strTypeName )
	{
	}

	Bag::Bag( OclMeta::TypeManager* pManager, const std::string& strTypeName, const OclMeta::ObjectVector& vecObjects )
		: Collection( pManager, strTypeName )
	{
		m_vecObjects = vecObjects;
	}

	Bag::Bag( OclMeta::TypeManager* pManager )
		: Collection( pManager, "ocl::Bag" )
	{
	}

	Bag::Bag( OclMeta::TypeManager* pManager, const OclMeta::ObjectVector& vecObjects )
		: Collection( pManager, "ocl::Bag" )
	{
		m_vecObjects = vecObjects;
	}

	Any* Bag::Clone() const
	{
		return new Bag( GetTypeManager(), m_vecObjects );
	}

	void Bag::GetValue( OclMeta::ObjectVector& value ) const
	{
		value = m_vecObjects;
	}

	void Bag::SetValue( const OclMeta::ObjectVector& value )
	{
		m_vecObjects = value;
	}

	bool Bag::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "ocl::Bag" ) >= 0 ) {
			OclMeta::ObjectVector vecObjects2 = ( ( Bag* ) &object )->m_vecObjects;
			if ( m_vecObjects.size() != vecObjects2.size() )
				return false;

			OclMeta::ObjectVector vecObjects1 = m_vecObjects;

			OclMeta::ObjectVector::iterator i = vecObjects1.begin();
			while ( i != vecObjects1.end() ) {
				OclMeta::Object o = *i;
				vecObjects1.erase( i );
				if ( GetObjectCountInVector( vecObjects1, o ) + 1 != GetObjectCountInVector( vecObjects2, o ) )
					return false;
				i = vecObjects1.begin();
			}
			return true;
		}
		else
			return false;
	}

	bool Bag::operator==( const Bag& object ) const
	{
		return Equals( object );
	}

	bool Bag::operator!=( const Bag& object ) const
	{
		return ! ( *this == object );
	}

	void Bag::AddObject( const OclMeta::Object& object )
	{
		m_vecObjects.push_back( object );
	}

	OclImplementation::ObjectIterator* Bag::GetIterator()
	{
		return new ObjectIterator( m_vecObjects.begin(), m_vecObjects.end() );
	}

	std::string Bag::Print() const
	{
		return PrintCollection( GetTypeName(), m_vecObjects );
	}

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Sequence <<< + OclBasic::Collection
//
//##############################################################################################################################################

	Sequence::Sequence( OclMeta::TypeManager* pManager, const std::string& strTypeName )
		: Collection( pManager, strTypeName )
	{
	}

	Sequence::Sequence( OclMeta::TypeManager* pManager, const std::string& strTypeName, const OclMeta::ObjectVector& vecObjects )
		: Collection( pManager, strTypeName )
	{
		m_vecObjects = vecObjects;
	}

	Sequence::Sequence( OclMeta::TypeManager* pManager )
		: Collection( pManager, "ocl::Sequence" )
	{
	}

	Sequence::Sequence( OclMeta::TypeManager* pManager, const OclMeta::ObjectVector& vecObjects )
		: Collection( pManager, "ocl::Sequence" )
	{
		m_vecObjects = vecObjects;
	}

	Any* Sequence::Clone() const
	{
		return new Sequence( GetTypeManager(), m_vecObjects );
	}

	void Sequence::GetValue( OclMeta::ObjectVector& value ) const
	{
		value = m_vecObjects;
	}

	void Sequence::SetValue( const OclMeta::ObjectVector& value )
	{
		m_vecObjects = value;
	}

	bool Sequence::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "ocl::Sequence" ) >= 0 )
			return m_vecObjects == ( ( Sequence* ) &object )->m_vecObjects;
		else
			return false;
	}

	bool Sequence::operator==( const Sequence& object ) const
	{
		return Equals( object );
	}

	bool Sequence::operator!=( const Sequence& object ) const
	{
		return ! ( *this == object );
	}

	void Sequence::AddObject( const OclMeta::Object& object )
	{
		m_vecObjects.push_back( object );
	}

	OclImplementation::ObjectIterator* Sequence::GetIterator()
	{
		return new ObjectIterator( m_vecObjects.begin(), m_vecObjects.end() );
	}

	std::string Sequence::Print() const
	{
		return PrintCollection( GetTypeName(), m_vecObjects );
	}

//##############################################################################################################################################
//
//	C L A S S : OclBasic::OrderedSet <<< + OclBasic::Collection
//
//##############################################################################################################################################

	OrderedSet::OrderedSet( OclMeta::TypeManager* pManager, const std::string& strTypeName )
		: Set( pManager, strTypeName )
	{
		m_pArgNode = NULL;
	}

	OrderedSet::OrderedSet( OclMeta::TypeManager* pManager, const std::string& strTypeName, const OclMeta::ObjectVector& vecObjects )
		: Set( pManager, strTypeName )
	{
		for ( unsigned int i = 0 ; i < vecObjects.size() ; i++ )
				AddObject( vecObjects[ i ] );
		m_pArgNode = NULL;
	}

	OrderedSet::OrderedSet( OclMeta::TypeManager* pManager )
		: Set( pManager, "ocl::OrderedSet" )
	{
		m_pArgNode = NULL;
	}

	OrderedSet::OrderedSet( OclMeta::TypeManager* pManager, const OclMeta::ObjectVector& vecObjects )
		: Set( pManager, "ocl::OrderedSet" )
	{
		for ( unsigned int i = 0 ; i < vecObjects.size() ; i++ )
			AddObject( vecObjects[ i ] );
		m_pArgNode = NULL;
	}

	Any* OrderedSet::Clone() const
	{
		return new OrderedSet( GetTypeManager(), m_vecObjects );
	}

	void OrderedSet::GetValue( OclMeta::ObjectVector& value ) const
	{
		value = m_vecObjects;
	}
	
	void OrderedSet::SetValue( const OclMeta::ObjectVector& value )
	{
		for ( unsigned int i = 0 ; i < value.size() ; i++ )
			AddObject( value[ i ] );
	}

	bool OrderedSet::Equals( const Any& object ) const
	{
		if ( GetTypeManager()->IsTypeA( object.GetTypeName(), "ocl::OrderedSet" ) >= 0 ) {
			OclMeta::ObjectVector vecObjects = ( ( OrderedSet* ) &object )->m_vecObjects;
			if ( m_vecObjects.size() != vecObjects.size() )
				return false;
			for ( unsigned int i = 0 ; i < m_vecObjects.size() ; i++ )
			{
				if ( m_vecObjects[ i ] != vecObjects[ i ] )
					return false;
			}
			return true;
		}
		else
			return false;
	}

	bool OrderedSet::operator==( const OrderedSet& object ) const
	{
		return Equals( object );
	}

	bool OrderedSet::operator!=( const OrderedSet& object ) const
	{
		return ! ( *this == object );
	}

	void OrderedSet::AddObject( const OclMeta::Object& object )
	{
		OclMeta::ObjectVector::iterator i = std::find( m_vecObjects.begin(), m_vecObjects.end(), object );
			if ( i == m_vecObjects.end() )
				m_vecObjects.push_back( object );
	}

	OclImplementation::ObjectIterator* OrderedSet::GetIterator()
	{
		return new ObjectIterator( m_vecObjects.begin(), m_vecObjects.end() );
	}

	std::string OrderedSet::Print() const
	{
		return PrintCollection( GetTypeName(), m_vecObjects );
	}

}; // namespace OclBasic
