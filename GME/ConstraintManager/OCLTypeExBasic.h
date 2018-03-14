//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLTypeExBasic.h
//
//###############################################################################################################################################

#ifndef OCLTypeExBasic_h
#define OCLTypeExBasic_h

#include "StdAfx.h"
#include "OCLCommon.h"
#include "OCLFactory.h"
#include "OCLType.h"

namespace OclBasic
{

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Any
//
//##############################################################################################################################################

	class TAny_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::String
//
//##############################################################################################################################################

	class TString_AttributeFactory
		: public OclImplementation::AttributeFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures );
	};

	class TString_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};


//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Enumeration
//
//##############################################################################################################################################
	class TEnumeration_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Real
//
//##############################################################################################################################################

	class TReal_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Integer
//
//##############################################################################################################################################

	class TInteger_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Collection
//
//##############################################################################################################################################

	class TCollection_AttributeFactory
		: public OclImplementation::AttributeFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures );
	};

	class TCollection_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

	class TCollection_IteratorFactory
		: public OclImplementation::IteratorFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Iterator& signature, OclMeta::IteratorVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Set
//
//##############################################################################################################################################

	class TSet_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

	class TSet_IteratorFactory
		: public OclImplementation::IteratorFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Iterator& signature, OclMeta::IteratorVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Sequence
//
//##############################################################################################################################################

	class TSequence_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

	class TSequence_IteratorFactory
		: public OclImplementation::IteratorFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Iterator& signature, OclMeta::IteratorVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Bag
//
//##############################################################################################################################################

	class TBag_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

	class TBag_IteratorFactory
		: public OclImplementation::IteratorFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Iterator& signature, OclMeta::IteratorVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::OrderedSet
//
//##############################################################################################################################################

	class TOrderedSet_MethodFactory
		: public OclImplementation::MethodFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures );
	};

	class TOrderedSet_IteratorFactory
		: public OclImplementation::IteratorFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Iterator& signature, OclMeta::IteratorVector& vecFeatures );
	};

//##############################################################################################################################################
//
//	G L O B A L   F A C T O R I E S
//
//##############################################################################################################################################

	class OperatorFactory
		: public OclImplementation::OperatorFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Operator& signature, OclMeta::OperatorVector& vecFeatures );
	};

	class FunctionFactory
		: public OclImplementation::FunctionFactory
	{
		public :
			virtual void GetFeatures( const OclSignature::Function& signature, OclMeta::FunctionVector& vecFeatures );
	};

	class TypeFactory
		: public OclImplementation::TypeFactory
	{
		public :
			virtual void GetTypes( const std::string& strName, const std::string& strNSpace, std::vector<std::unique_ptr<OclMeta::Type>>& vecTypes, std::string& nameRes );
	};

}; // namespace OclBasic

#endif // OCLTypeExBasic_h