//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLObjectExBasic.h
//
//###############################################################################################################################################

#ifndef OCLObjectExBasic_h
#define OCLObjectExBasic_h

#include "StdAfx.h"
#include "OCLCommon.h"
#include "OCLObject.h"

	#define CREATE_BOOLEAN( pManager, bValue )	\
		OclMeta::Object( new OclBasic::Boolean( pManager, bValue ) )

	#define CREATE_STRING( pManager, strValue )	\
		OclMeta::Object( new OclBasic::String( pManager, strValue ) )

	#define CREATE_ENUMERATION( pManager, eValue )	\
		OclMeta::Object( new OclBasic::Enumeration( pManager, eValue ) )

	#define CREATE_REAL( pManager, dValue )	\
		OclMeta::Object( new OclBasic::Real( pManager, dValue ) )

	#define CREATE_INTEGER( pManager, lValue )	\
		OclMeta::Object( new OclBasic::Integer( pManager, lValue ) )

	#define CREATE_TYPE( pManager, lValue )	\
		OclMeta::Object( new OclBasic::Type( pManager, lValue ) )

	#define DECL_BOOLEAN( varName, var2Name )			\
		bool varName = false;									\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::Boolean* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_STRING( varName, var2Name )			\
		std::string varName;										\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::String* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_ENUMERATION( varName, var2Name )			\
		std::string varName;											\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::Enumeration* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_REAL( varName, var2Name )			\
		double varName = 0.0;										\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::Real* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_INTEGER( varName, var2Name )			\
		long varName = 0;											\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::Integer* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_TYPE( varName, var2Name )			\
		std::string varName;										\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::Type* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_BOOLEAN2( varName, var2Name )			\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::Boolean* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_STRING2( varName, var2Name )			\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::String* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_ENUMERATION2( varName, var2Name )			\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::Enumeration* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_REAL2( varName, var2Name )			\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::Real* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_INTEGER2( varName, var2Name )			\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::Integer* ) var2Name.GetImplementation() )->GetValue( varName )

	#define DECL_TYPE2( varName, var2Name )			\
		if ( ! var2Name.IsUndefined() )			\
			( ( OclBasic::Type* ) var2Name.GetImplementation() )->GetValue( varName )

	#define CREATE_SET( pManager, setValue )	\
		OclMeta::Object( new OclBasic::Set( pManager, setValue ) )

	#define CREATE_SEQUENCE( pManager, seqValue )	\
		OclMeta::Object( new OclBasic::Sequence( pManager, seqValue ) )

	#define CREATE_BAG( pManager, bagValue )	\
		OclMeta::Object( new OclBasic::Bag( pManager, bagValue ) )

	#define CREATE_ORDEREDSET( pManager, osetValue )	\
		OclMeta::Object( new OclBasic::OrderedSet( pManager, osetValue ) )

	#define DECL_COLLECTION( varName, var2Name )	\
		OclMeta::ObjectVector varName;	\
		if ( ! var2Name.IsUndefined() ) {		\
			if ( var2Name.GetTypeName() == "ocl::Set" )	 \
				( ( OclBasic::Set* ) var2Name.GetImplementation() )->GetValue( varName );	\
			else if ( var2Name.GetTypeName() == "ocl::Bag" )	\
				( ( OclBasic::Bag* ) var2Name.GetImplementation() )->GetValue( varName );	\
			else if ( var2Name.GetTypeName() == "ocl::Sequence" )	\
				( ( OclBasic::Sequence* ) var2Name.GetImplementation() )->GetValue( varName );	\
			else \
				( ( OclBasic::OrderedSet* ) var2Name.GetImplementation() )->GetValue( varName ); \
		} 

	#define DECL_COLLECTION2( varName, var2Name )	\
		if ( ! var2Name.IsUndefined() ) {		\
			if ( var2Name.GetTypeName() == "ocl::Set" )	\
				( ( OclBasic::Set* ) var2Name.GetImplementation() )->GetValue( varName );	\
			else if ( var2Name.GetTypeName() == "ocl::Bag" )	\
				( ( OclBasic::Bag* ) var2Name.GetImplementation() )->GetValue( varName );	\
			else if ( var2Name.GetTypeName() == "ocl::Sequence" )	\
				( ( OclBasic::Sequence* ) var2Name.GetImplementation() )->GetValue( varName );	\
			else \
				( ( OclBasic::OrderedSet* ) var2Name.GetImplementation() )->GetValue( varName );	\
		}

	#define DECL_ITERATOR( varName, var2Name )	\
		OclCommon::Smart< OclImplementation::ObjectIterator > varName( ( (OclImplementation::CompoundObject*) var2Name.GetImplementation() )->GetIterator() )

namespace OclMeta
{
	class TypeManager;
};

namespace OclBasic
{

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Any = OclImplementation::Object
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	typedef OclImplementation::Object Any;

//##############################################################################################################################################
//
//	C L A S S : OclBasic::String <<< + OclBasic::Any
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class String
		: public Any
	{
		private :
			std::string m_Value;

		protected :
			String( OclMeta::TypeManager* pManager, const std::string& strTypeName, const std::string& value );
		public :
			String( OclMeta::TypeManager* pManager, const std::string& value );
			virtual Any* Clone() const;

			void GetValue( std::string& value ) const;
			void SetValue( const std::string& value );

			virtual bool Equals( const Any& object ) const;
			bool operator==( const String& object ) const;
			bool operator!=( const String& object ) const;

			virtual std::string Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Enumeration <<< + OclBasic::Any
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Enumeration
		: public Any
	{
		private :
			std::string m_Value;


		protected :
			Enumeration( OclMeta::TypeManager* pManager, const std::string& strTypeName, const std::string& value );
		public :
			Enumeration( OclMeta::TypeManager* pManager, const std::string& value );
			virtual Any* Clone() const;

			void GetValue( std::string& value ) const;
			void SetValue( const std::string& value );

			virtual bool Equals( const Any& object ) const;
			bool operator==( const Enumeration& object ) const;
			bool operator!=( const Enumeration& object ) const;

			virtual std::string Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Boolean <<< + OclBasic::Any
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Boolean
		: public Any
	{
		private :
			bool m_Value;

		protected :
			Boolean( OclMeta::TypeManager* pManager, const std::string& strTypeName, bool value );
		public :
			Boolean( OclMeta::TypeManager* pManager, bool value );
			virtual Any* Clone() const;

			void GetValue( bool& value ) const;
			void SetValue( bool value );

			virtual bool Equals( const Any& object ) const;
			bool operator==( const Boolean& object ) const;
			bool operator!=( const Boolean& object ) const;

			virtual std::string Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Real <<< + OclBasic::Any
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Real
		: public Any
	{
		private :
			double m_Value;

		protected :
			Real( OclMeta::TypeManager* pManager, const std::string& strTypeName, double value );
		public :
			Real( OclMeta::TypeManager* pManager, double value );
			virtual Any* Clone() const;

			void GetValue( double& value ) const;
			void SetValue( double value );

			virtual bool Equals( const Any& object ) const;
			bool operator==( const Real& object ) const;
			bool operator!=( const Real& object ) const;

			virtual std::string Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Integer <<< + OclBasic::Real
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Integer
		: public Real
	{
		protected :
			Integer( OclMeta::TypeManager* pManager, const std::string& strTypeName, long value );
		public :
			Integer( OclMeta::TypeManager* pManager, long value );
			virtual Any* Clone() const;

			void GetValue( long& value ) const;
			void SetValue( long value );

			bool operator==( const Integer& object ) const;
			bool operator!=( const Integer& object ) const;

			virtual std::string Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Type <<< + OclBasic::Any
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Type
		: public Any
	{
		private :
			std::string m_Value;

		protected :
			Type( OclMeta::TypeManager* pManager, const std::string& strTypeName, const std::string& value );
		public :
			Type( OclMeta::TypeManager* pManager, const std::string& value );
			virtual Any* Clone() const;

			void GetValue( std::string& value ) const;
			void SetValue( const std::string& value );

			virtual bool Equals( const Any& object ) const;
			bool operator==( const Type& object ) const;
			bool operator!=( const Type& object ) const;

			virtual std::string Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Collection = OclImplementation::CompoundObject
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	typedef OclImplementation::CompoundObject Collection;

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Set <<< + OclBasic::Collection
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Set
		: public Collection
	{
		private :
			OclMeta::ObjectVector	m_vecObjects;

		protected :
			Set( OclMeta::TypeManager* pManager, const std::string& strTypeName );
			Set( OclMeta::TypeManager* pManager, const std::string& strTypeName, const OclMeta::ObjectVector& vecObjects );
		public :
			Set( OclMeta::TypeManager* pManager );
			Set( OclMeta::TypeManager* pManager, const OclMeta::ObjectVector& vecObjects );
			virtual Any* Clone() const;

			void GetValue( OclMeta::ObjectVector& value ) const;
			void SetValue( const OclMeta::ObjectVector& value );

			virtual bool Equals( const Any& object ) const;
			bool operator==( const Set& object ) const;
			bool operator!=( const Set& object ) const;

			virtual void AddObject( const OclMeta::Object& object );
			virtual OclImplementation::ObjectIterator* GetIterator();

			virtual std::string Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Bag <<< + OclBasic::Collection
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Bag
		: public Collection
	{
		private :
			OclMeta::ObjectVector	m_vecObjects;

		protected :
			Bag( OclMeta::TypeManager* pManager, const std::string& strTypeName );
			Bag( OclMeta::TypeManager* pManager, const std::string& strTypeName, const OclMeta::ObjectVector& vecObjects );
		public :
			Bag( OclMeta::TypeManager* pManager );
			Bag( OclMeta::TypeManager* pManager, const OclMeta::ObjectVector& vecObjects );
			virtual Any* Clone() const;

			void GetValue( OclMeta::ObjectVector& value ) const;
			void SetValue( const OclMeta::ObjectVector& value );

			virtual bool Equals( const Any& object ) const;
			bool operator==( const Bag& object ) const;
			bool operator!=( const Bag& object ) const;

			virtual void AddObject( const OclMeta::Object& object );
			virtual OclImplementation::ObjectIterator* GetIterator();

			virtual std::string Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclBasic::Sequence <<< + OclBasic::Collection
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Sequence
		: public Collection
	{
		private :
			OclMeta::ObjectVector	m_vecObjects;

		protected :
			Sequence( OclMeta::TypeManager* pManager, const std::string& strTypeName );
			Sequence( OclMeta::TypeManager* pManager, const std::string& strTypeName, const OclMeta::ObjectVector& vecObjects );
		public :
			Sequence( OclMeta::TypeManager* pManager );
			Sequence( OclMeta::TypeManager* pManager, const OclMeta::ObjectVector& vecObjects );
			virtual Any* Clone() const;

			void GetValue( OclMeta::ObjectVector& value ) const;
			void SetValue( const OclMeta::ObjectVector& value );

			virtual bool Equals( const Any& object ) const;
			bool operator==( const Sequence& object ) const;
			bool operator!=( const Sequence& object ) const;

			virtual void AddObject( const OclMeta::Object& object );
			virtual OclImplementation::ObjectIterator* GetIterator();

			virtual std::string Print() const;
	};

//##############################################################################################################################################
//
//	C L A S S : OclBasic::OrderedSet <<< + OclBasic::Collection
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class OrderedSet
		: public Set
	{
		private :
			OclMeta::ObjectVector	m_vecObjects;
			void * m_pArgNode;

		protected :
			OrderedSet( OclMeta::TypeManager* pManager, const std::string& strTypeName );
			OrderedSet( OclMeta::TypeManager* pManager, const std::string& strTypeName, const OclMeta::ObjectVector& vecObjects );
		public :
			OrderedSet( OclMeta::TypeManager* pManager );
			OrderedSet( OclMeta::TypeManager* pManager, const OclMeta::ObjectVector& vecObjects );
			virtual Any* Clone() const;

			void GetValue( OclMeta::ObjectVector& value ) const;
			void SetValue( const OclMeta::ObjectVector& value );

//			void SetArgNode(void *arg);
//			void GetArgNode(void * &arg);

			virtual bool Equals( const Any& object ) const;
			bool operator==( const OrderedSet& object ) const;
			bool operator!=( const OrderedSet& object ) const;

			virtual void AddObject( const OclMeta::Object& object );
			virtual OclImplementation::ObjectIterator* GetIterator();

			virtual std::string Print() const;
	};

}; // namespace OclBasic

#endif // OCLObjectExBasic

