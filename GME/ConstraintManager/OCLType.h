//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLType.h
//
//###############################################################################################################################################

#ifndef OCLType_h
#define OCLType_h

#include "OCLCommon.h"
#include "OCLException.h"
#include "OCLFactory.h"
#include "OCLFeature.h"
#include "OCLSignature.h"

#include <memory>

namespace OclMeta
{
	class Type;
	class CompoundType;
	class TypeManager;

	typedef std::vector< std::unique_ptr<Type> > TypeVector;

	union UCallResult
	{
		Feature* 					pFeature;
		OclCommon::Exception*	pException;
	};

	struct CallResult
	{
		bool 			bIsValid;
		UCallResult	uResult;
	};

	struct TypeResult
	{
		bool			bIsValid;
		std::shared_ptr<Type> pType;
		std::shared_ptr<OclCommon::Exception> pException;
	};

	typedef std::map< std::string , CallResult > CallResultMap;
	typedef std::map< std::string , TypeResult > TypeResultMap;

//##############################################################################################################################################
//
//	C L A S S : OclMeta::TypeManager
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class TypeManager
	{
		private :
			TypeResultMap										m_mapTypes;
			CallResultMap											m_mapOperators;
			CallResultMap											m_mapFunctions;

			OclImplementation::TypeFactory*					m_pTypeFactory;
			OclImplementation::OperatorFactory*				m_pOperatorFactory;
			OclImplementation::FunctionFactory*				m_pFunctionFactory;
			OclImplementation::ConstraintDefinitionFactory* 	m_pDefinitionFactory;

		public :
						TypeManager( OclImplementation::TypeFactory* pTypeFactory, OclImplementation::OperatorFactory* pOperatorFactory, OclImplementation::FunctionFactory* pFunctionFactory, OclImplementation::ConstraintDefinitionFactory* pCDFactory );
			virtual		~TypeManager();

			OclImplementation::ConstraintDefinitionFactory* GetDefinitionFactory() const;
			void 		Clear();
			void 		ClearTypes();
			void 		ClearDynamicTypes();
			void 		ClearGlobals();

			std::shared_ptr<Type> GetType( const std::string& strName, const std::string& strNSpace );

			int 			GetTypeDistance( const std::string& strName );
			int 			IsTypeA( const std::string& strName1, const std::string& strName2 );
			std::string 	GetTypeBase( const std::string& strName1, const std::string& strName2 );
			bool 		IsTypeA( const TypeSeq& vecType1, const TypeSeq& vecType2 );
			TypeSeq 	GetTypeBase( const TypeSeq& vecType1, const TypeSeq& vecType2 );

			Operator*	GetOperator( const OclSignature::Operator& signature );
			Function*	GetFunction( const OclSignature::Function& signature );
			void		RegisterFeature( CallResult& callResult );
			void 		RegisterType( TypeResult& typeResult );

		private :
			int 			IsTypeAR( const std::string& strName1, const std::string& strName2, int iLevel );
			std::string 	GetTypeBaseR( const std::string& strName1, const std::string& strName2 );
	};

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Type
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class Type
	{
		private :
			bool 											m_bDynamic;
			std::string 									m_strName;
			StringVector									m_vecSuperTypes;
			CallResultMap									m_mapAttributes;
			CallResultMap									m_mapAssociations;
			CallResultMap									m_mapMethods;
			OclImplementation::AttributeFactory*		m_pAttributeFactory;
			OclImplementation::AssociationFactory*		m_pAssociationFactory;
			OclImplementation::MethodFactory*			m_pMethodFactory;
			TypeManager*								m_pTypeManager;

		public :
											Type( const std::string& strName, const StringVector& vecSuperTypes, OclImplementation::AttributeFactory* pAttributeFactory, OclImplementation::AssociationFactory* pAssociationFactory, OclImplementation::MethodFactory* pMethodFactory, bool bDynamic );
			virtual 							~Type();

					bool 					IsDynamic() const;
					std::string 			GetName() const;
					const StringVector& 	GetSuperTypeNames() const;
			virtual	bool					IsCompound() const;

					Attribute* 				GetAttribute( const OclSignature::Attribute& signature );
					Association* 			GetAssociation( const OclSignature::Association& signature );
					Method* 				GetMethod( const OclSignature::Method& signature );
	//	protected :
					CallResult 				GetResults( const OclSignature::Attribute& signature );
					CallResult 				GetResults( const OclSignature::Association& signature );
					CallResult 				GetResults( const OclSignature::Method& signature );
			virtual 	CallResult 				GetResults( const OclSignature::Iterator& signature );
		protected :
					TypeManager* 		GetTypeManager() const;

		friend class TypeManager;
	};

//##############################################################################################################################################
//
//	C L A S S : OclMeta::CompoundType <<< + OclMeta::Type
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class CompoundType
		: public Type
	{
		private :
			CallResultMap									m_mapIterators;
			OclImplementation::IteratorFactory*		m_pIteratorFactory;

		public :
											CompoundType( const std::string& strName, const StringVector& vecSuperTypes, OclImplementation::AttributeFactory* pAttributeFactory, OclImplementation::AssociationFactory* pAssociationFactory, OclImplementation::MethodFactory* pMethodFactory, OclImplementation::IteratorFactory* pIteratorFactory, bool bDynamic );
			virtual 							~CompoundType();

			virtual	bool					IsCompound() const;

					Iterator* 				GetIterator(int level, const OclSignature::Iterator& signature );
	//	protected :
			virtual 	CallResult 				GetResults( const OclSignature::Iterator& signature );

		friend class TypeManager;
	};

}; // namespace OclMeta

#endif
