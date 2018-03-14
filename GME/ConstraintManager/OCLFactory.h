//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLFactory.h
//
//###############################################################################################################################################

#ifndef OCLFactory_h
#define OCLFactory_h

#include "OCLSignature.h"
#include "OCLFeature.h"

#include <memory>

namespace OclMeta {
	class Type;
	class TypeManager;
};

namespace OclImplementation
{
	//##############################################################################################################################################
	//
	//	T E M P L A T E   C L A S S : OclImplementation::FeatureFactory
	//
	//==============================================================================================================================================
	//
	//	D E S C R I P T I O N :
	//
	//##############################################################################################################################################

	template < class TFeatureSignature, class TFeatureVector >
	class FeatureFactory
	{
		private :
			OclMeta::TypeManager* m_pTypeManager;

		public :
			FeatureFactory()
			{
			}

			virtual ~FeatureFactory()
			{
			}

			virtual void GetFeatures( const TFeatureSignature& signature, TFeatureVector& vecFeatures )
			{
			}

			OclMeta::TypeManager* GetTypeManager() const
			{
				return m_pTypeManager;
			}

		friend class OclMeta::TypeManager;
	};

	typedef FeatureFactory< OclSignature::Attribute , OclMeta::AttributeVector > AttributeFactory;
	typedef FeatureFactory< OclSignature::Association , OclMeta::AssociationVector > AssociationFactory;
	typedef FeatureFactory< OclSignature::Iterator , OclMeta::IteratorVector > IteratorFactory;
	typedef FeatureFactory< OclSignature::Method , OclMeta::MethodVector > MethodFactory;
	typedef FeatureFactory< OclSignature::Operator , OclMeta::OperatorVector > OperatorFactory;
	typedef FeatureFactory< OclSignature::Function , OclMeta::FunctionVector > FunctionFactory;

	//##############################################################################################################################################
	//
	//	T E M P L A T E   C L A S S : OclImplementation::TypeFactory
	//
	//==============================================================================================================================================
	//
	//	D E S C R I P T I O N :
	//
	//##############################################################################################################################################

	class TypeFactory
	{
		private :
			OclMeta::TypeManager* m_pTypeManager;

		public :
			TypeFactory() {};

			virtual			~TypeFactory() {};

			virtual void	GetTypes( const std::string& strName, const std::string& strNSpace, std::vector< std::unique_ptr<OclMeta::Type> >& vecTypes, std::string& nameRes ) = 0;

			OclMeta::TypeManager* GetTypeManager() const
			{
				return m_pTypeManager;
			}

		friend class OclMeta::TypeManager;
	};

	//##############################################################################################################################################
	//
	//	C L A S S : OclImplementation::ConstraintDefinitionFactory
	//
	//==============================================================================================================================================
	//
	//	D E S C R I P T I O N :
	//
	//##############################################################################################################################################

	class ConstraintDefinitionFactory
	{
		private :
			OclMeta::TypeManager* m_pTypeManager;

		public :
			ConstraintDefinitionFactory() {};

			virtual	~ConstraintDefinitionFactory() {};

			virtual void GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
			{
			}

			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
			{
			}

			OclMeta::TypeManager* GetTypeManager() const
			{
				return m_pTypeManager;
			}

		friend class OclMeta::TypeManager;
	};

}; // namespace OclImplementation

#endif //OCLFactory_h
