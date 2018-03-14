//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLType.h
//
//###############################################################################################################################################

#include "Solve4786.h"
#include "OCLType.h"

#define NILNAMESPACE ""

namespace OclMeta
{

//##############################################################################################################################################
//
//	F U N C T I O N S
//
//##############################################################################################################################################

	void DisposeCallMap( CallResultMap& mapArg )
	{
		for ( CallResultMap::iterator i = mapArg.begin() ; i != mapArg.end() ; ++i )
			if ( (*i).second.bIsValid )
				delete (*i).second.uResult.pFeature;
			else
				delete (*i).second.uResult.pException;
		mapArg.clear();
	}

	template < class TItem >
	TItem* DisposeVector( std::vector<TItem*>& vecArg, int iGetPos = -1 )
	{
		TItem* pItem = NULL;
		for ( unsigned int i = 0 ; i < vecArg.size() ; i++ )
			if ( (int) i == iGetPos )
				pItem = vecArg[ i ];
			else
				delete vecArg[ i ];
		vecArg.clear();
		return pItem;
	}

	int MatchParametralFeature( TypeManager* pManager, const OclSignature::ParametralFeature& signature, const OclSignature::ParametralFeature& feature )
	{
		int iMatch = -1;
		int iParamCount = signature.GetParameterCount();
		if ( iParamCount >= feature.GetMinParameterCount() && iParamCount <= feature.GetParameterCount() ) {
			if ( iParamCount == 0 )
					return 0;
			for ( int i = 0 ; i < iParamCount ; i++ ) {
				int iIsA = pManager->IsTypeA( signature.GetParameter( i ).GetTypeName(), feature.GetParameter( i ).GetTypeName() );
				if ( iIsA == -1 ) {
					iMatch = -1;
					break;
				}
				else {
					if ( iMatch == -1 )
						iMatch = 0;
					iMatch += iIsA;
				}
			}
		}
		return iMatch;
	}

	template < class TFeature >
	TFeature* ReturnCallResult( const CallResult& callResult )
	{
		if ( callResult.bIsValid )
			return (TFeature*) callResult.uResult.pFeature;
		else
			throw OclCommon::Exception( *callResult.uResult.pException );
	}

	// recursion corrected: terge
	bool GetCallResult( const CallResultMap& mapArg, std::string/*const OclSignature::Feature*/& feature, CallResult& callResult )
	{
		CallResultMap::const_iterator i = mapArg.find( feature/*.Print()*/ );
		if ( i != mapArg.end() ) {
			callResult = (*i).second;
			return true;
		}
		return false;
	}

	template < class TParametralFeature, class TParametralSignature >
	void FilterParametralFeature( TypeManager* pManager, const TParametralSignature& signature, std::vector<TParametralFeature*>& vecArg, int iCodeAmbig, int iCodeExist, CallResult& callResult)
	{
		int iPos = -1;
		int iMatch = -1;
		for ( unsigned int j = 0 ; j < vecArg.size() ; j++ ) {
			int iMatch2 = 0;
			try {
				iMatch2 = MatchParametralFeature( pManager, signature, *vecArg[ j ] );
			} catch ( OclCommon::Exception ex ) {
				DisposeVector<TParametralFeature>( vecArg );
				throw ex;
			}
			if ( iMatch2 != -1 ) {
				if ( iMatch == -1 || iMatch2 < iMatch ) {
					iMatch = iMatch2;
					iPos = (int) j;
				}
				else if ( iMatch == iMatch2 && ! vecArg[ j ]->IsIdentical( *vecArg[ iPos ] ) ) {
					iPos = -1;
					callResult.bIsValid = false;
					callResult.uResult.pException = new OclCommon::Exception( OclCommon::Exception::ET_SEMANTIC, iCodeAmbig, signature.Print() );
					break;
				}
			}
		}
		TParametralFeature* pParametralFeature = DisposeVector<TParametralFeature>( vecArg, iPos );
		if ( ! pParametralFeature ) {
			callResult.bIsValid = false;
			callResult.uResult.pException = new OclCommon::Exception( OclCommon::Exception::ET_SEMANTIC, iCodeExist, signature.Print() );
		}
		else {
			callResult.bIsValid = true;
			callResult.uResult.pFeature = (Feature*)pParametralFeature;
		}
	}

	template < class TTypeableFeature, class TTypeableSignature >
	void FilterTypeableFeature( const TTypeableSignature& signature, std::vector<TTypeableFeature*>& vecArg, int iCodeAmbig, int iCodeExist, CallResult& callResult )
	{
		if ( vecArg.size() > 1 ) {
			DisposeVector<TTypeableFeature>( vecArg );
			callResult.bIsValid = false;
			callResult.uResult.pException = new OclCommon::Exception( OclCommon::Exception::ET_SEMANTIC, iCodeAmbig, signature.Print() );
			return;
		}
		if ( vecArg.empty() ) {
			callResult.bIsValid = false;
			callResult.uResult.pException = new OclCommon::Exception( OclCommon::Exception::ET_SEMANTIC, iCodeExist, signature.Print() );
			return;
		}
		callResult.bIsValid = true;
		callResult.uResult.pFeature = vecArg[ 0 ];
	}

	template< class TFeature , class TFeatureSignature >
	bool FilterBaseType( TypeManager* pManager, const StringVector& vecSuperTypes, const TFeatureSignature& signature, std::vector<TFeature*>& vecArg, int iCodeExist, CallResult& callResult )
	{
		for ( unsigned int i = 0 ; i < vecSuperTypes.size() ; i++ ) {
			try {
				std::shared_ptr<Type> pType = pManager->GetType( vecSuperTypes[ i ], NILNAMESPACE );
				callResult = pType->GetResults( signature );
				if ( callResult.bIsValid )
					vecArg.push_back( (TFeature*) callResult.uResult.pFeature );
				else {
					if ( callResult.uResult.pException->GetCode() == iCodeExist )
						delete callResult.uResult.pException;
					else {
						DisposeVector<TFeature>( vecArg );
						return false;
					}
				}
			}
			catch( OclCommon::Exception ex ) {
				DisposeVector<TFeature>( vecArg );
				throw ex;
			}
		}
		return true;
	}

//##############################################################################################################################################
//
//	C L A S S : OclMeta::TypeManager
//
//##############################################################################################################################################

	TypeManager::TypeManager( OclImplementation::TypeFactory* pTypeFactory, OclImplementation::OperatorFactory* pOperatorFactory, OclImplementation::FunctionFactory* pFunctionFactory, OclImplementation::ConstraintDefinitionFactory* pCDFactory )
		: m_pTypeFactory( pTypeFactory ), m_pOperatorFactory( pOperatorFactory ), m_pFunctionFactory( pFunctionFactory ), m_pDefinitionFactory( pCDFactory )
	{
		pTypeFactory->m_pTypeManager = this;
		pOperatorFactory->m_pTypeManager = this;
		pFunctionFactory->m_pTypeManager = this;
		m_pDefinitionFactory->m_pTypeManager = this;
	}

	TypeManager::~TypeManager()
	{
		Clear();
		delete m_pTypeFactory;
		delete m_pOperatorFactory;
		delete m_pFunctionFactory;
		delete m_pDefinitionFactory;
	}

	OclImplementation::ConstraintDefinitionFactory* TypeManager::GetDefinitionFactory() const
	{
		return m_pDefinitionFactory;
	}

	void TypeManager::ClearTypes()
	{
		m_mapTypes.clear();
	}

	void TypeManager::ClearDynamicTypes()
	{
		TypeResultMap mapTypes = m_mapTypes;
		m_mapTypes.clear();
		for ( TypeResultMap::iterator i = mapTypes.begin() ; i != mapTypes.end() ; ++i )
			if ( ! (*i).second.bIsValid )
				i->second.pException.reset();
			else
				if ( (*i).second.pType->IsDynamic() )
					i->second.pType.reset();
				else
					m_mapTypes.insert( TypeResultMap::value_type( (*i).first, std::move((*i).second) ) );
	}

	void TypeManager::ClearGlobals()
	{
		DisposeCallMap( m_mapOperators );
		DisposeCallMap( m_mapFunctions );
	}

	void TypeManager::Clear()
	{
		ClearTypes();
		ClearGlobals();
	}

	std::shared_ptr<Type> TypeManager::GetType( const std::string& strName, const std::string& strNSpace  )
	{
		std::string nameResult;
		TypeResultMap::iterator i = m_mapTypes.find( strName );
		if ( i != m_mapTypes.end() ) {
			if ( (*i).second.bIsValid )
				return (*i).second.pType;
			else
				throw OclCommon::Exception( *(*i).second.pException );
		}
		TypeVector vecTypes;
		TypeResult typeResult;
		try {
			m_pTypeFactory->GetTypes( strName, strNSpace, vecTypes, nameResult );
			if ( vecTypes.empty() ) {
				typeResult.bIsValid = false;
				typeResult.pException = std::shared_ptr<OclCommon::Exception>(new OclCommon::Exception( OclCommon::Exception::ET_SEMANTIC, EX_TYPE_DOESNT_EXIST, strName ));
			}
			else {
				if ( vecTypes.size() > 1 ) {
					typeResult.bIsValid = false;
					typeResult.pException = std::shared_ptr<OclCommon::Exception>(new OclCommon::Exception( OclCommon::Exception::ET_SEMANTIC, EX_TYPE_AMBIGUOUS, strName ));
				}
				else {
					typeResult.bIsValid = true;
					typeResult.pType = std::shared_ptr<Type>(vecTypes[ 0 ].release());
				}
			}
		}
		catch ( OclCommon::Exception ex ) {
			typeResult.bIsValid = false;
			typeResult.pException = std::shared_ptr<OclCommon::Exception>(new OclCommon::Exception( ex ));
		}
		TypeResultMap::iterator it = m_mapTypes.insert( TypeResultMap::value_type( std::move(nameResult), std::move(typeResult) ) ).first; 
		// WAS: m_mapTypes.insert( TypeResultMap::value_type( strName, typeResult ) );
		RegisterType( typeResult );
		if ( typeResult.bIsValid )
			return typeResult.pType;
		else
			throw OclCommon::Exception( *typeResult.pException );
	}

	int TypeManager::IsTypeAR( const std::string& strName1, const std::string& strName2, int iLevel )
	{
		std::shared_ptr<Type> pType1 = GetType( strName1, NILNAMESPACE );
		if ( pType1->GetName() == strName2 )
			return iLevel;
		const StringVector& vecSuperTypes = pType1->GetSuperTypeNames();
		int iIsA = -1;
		for ( unsigned int i = 0 ; i < vecSuperTypes.size() ; i++ ) {
			int iIsA2 = IsTypeAR( vecSuperTypes[ i ], strName2, iLevel + 1 );
			if ( iIsA2 != -1 && ( iIsA == -1 || iIsA2 < iIsA ) )
				iIsA = iIsA2;
		}
		return iIsA;
	}

	int TypeManager::IsTypeA( const std::string& strName1, const std::string& strName2 )
	{
		std::shared_ptr<Type> pType2 = GetType( strName2, NILNAMESPACE );
		return IsTypeAR( strName1, pType2->GetName(), 0 );
	}

	int TypeManager::GetTypeDistance( const std::string& strName )
	{
		std::shared_ptr<Type> pType = GetType( ( strName.empty() ) ? "ocl::Any" : strName, NILNAMESPACE );
		const StringVector& vecSuperTypes = pType->GetSuperTypeNames();
		if ( vecSuperTypes.empty() )
			return 0;
		int iDistance = -1;
		for ( unsigned int i = 0 ; i < vecSuperTypes.size() ; i++ ) {
			int iDistance2 = GetTypeDistance( vecSuperTypes[ i ] );
			if ( iDistance == -1 || iDistance2 < iDistance )
				iDistance = iDistance2 + 1;
		}
		return iDistance;
	}

	std::string TypeManager::GetTypeBaseR( const std::string& strName1, const std::string& strName2 )
	{
		if ( strName1 == "ocl::Any" )
				return strName1;
		StringVector vecTypes;
		const StringVector& vecSuperTypes = GetType( strName1, NILNAMESPACE )->GetSuperTypeNames();
		unsigned int i;
		for ( i = 0 ; i < vecSuperTypes.size() ; i++ )
			vecTypes.push_back( GetTypeBase( vecSuperTypes[ i ], strName2 ) );
		int iIsA = -1;
		std::string strResult = "ocl::Any";
		for ( i = 0 ; i < vecTypes.size() ; i++ ) {
			int iIsA2 = GetTypeDistance( vecTypes[ i ] );
			if ( iIsA2 != -1 && ( iIsA == -1 || iIsA2 < iIsA ) ) {
				strResult = vecTypes[ i ];
				iIsA = iIsA2;
			}
		}
		return strResult;
	}

	std::string TypeManager::GetTypeBase( const std::string& strName1, const std::string& strName2 )
	{
		if ( IsTypeA( strName1, strName2 ) >= 0 )
				return strName2;
		if ( IsTypeA( strName2, strName1 ) >= 0 )
				return strName1;
		std::string strBase1 = GetTypeBaseR( strName1, strName2 );
		std::string strBase2 = GetTypeBaseR( strName2, strName1 );
		if ( strBase1 == "ocl::Any" )
				return strBase2;
		if ( strBase2 == "ocl::Any" )
				return strBase1;
		return ( GetTypeDistance( strBase1 ) < GetTypeDistance( strBase2 ) ) ? strBase1 : strBase2;
	}

	TypeSeq TypeManager::GetTypeBase( const TypeSeq& vecType1, const TypeSeq& vecType2 )
	{
		if ( vecType2.size() < vecType1.size() )
			return GetTypeBase( vecType2, vecType1 );
		TypeSeq vecType;
		for ( unsigned int i = 0 ; i < vecType1.size() ; i++ )
			vecType.push_back( GetTypeBase( vecType1[ i ], vecType2[ i ] ) );
		return vecType;
	}

	bool TypeManager::IsTypeA( const TypeSeq& vecType1, const TypeSeq& vecType2 )
	{
		// terge ?? 
		if ( vecType2.size() < vecType1.size() )
			return false;
		for ( unsigned int i = 0 ; i < vecType1.size() ; i++ )
			if ( IsTypeA( vecType1[ i ], vecType2[ i ] ) < 0 )
				return false;
		return true;
	}

	Operator* TypeManager::GetOperator( const OclSignature::Operator& signature )
	{
		CallResult callResult;
		callResult.uResult.pException = NULL;
		std::string signo = signature.Print();

		if ( GetCallResult( m_mapOperators, signo/*signature*/, callResult ) )
			return ReturnCallResult<Operator>( callResult );

		OperatorVector vecOperators;
		try {
			m_pOperatorFactory->GetFeatures( signature, vecOperators );
			FilterParametralFeature<Operator,OclSignature::Operator>( this, signature, vecOperators, EX_OPERATOR_AMBIGUOUS, EX_OPERATOR_DOESNT_EXIST, callResult );
		}
		catch ( OclCommon::Exception ex ) {
			DisposeVector<Operator>( vecOperators );
			callResult.bIsValid = false;
			callResult.uResult.pException = new OclCommon::Exception( ex );
		}

		m_mapOperators.insert( CallResultMap::value_type( signature.Print(), callResult ) );
		RegisterFeature( callResult );
		return ReturnCallResult<Operator>( callResult );
	}

	Function* TypeManager::GetFunction( const OclSignature::Function& signature )
	{
		CallResult callResult;
		callResult.uResult.pException = NULL;
		std::string signo = signature.Print();

		if ( GetCallResult( m_mapFunctions, signo/*signature*/, callResult ) )
			return ReturnCallResult<Function>( callResult );

		FunctionVector vecFunctions;
		try {
			m_pFunctionFactory->GetFeatures( signature, vecFunctions );
			FilterParametralFeature<Function,OclSignature::Function>( this, signature, vecFunctions, EX_FUNCTION_AMBIGUOUS, EX_FUNCTION_DOESNT_EXIST, callResult );
		}
		catch ( OclCommon::Exception ex ) {
			DisposeVector<Function>( vecFunctions );
			callResult.bIsValid = false;
			callResult.uResult.pException = new OclCommon::Exception( ex );
		}

		m_mapFunctions.insert( CallResultMap::value_type( signature.Print(), callResult ) );
		RegisterFeature( callResult );
		return ReturnCallResult<Function>( callResult );
	}

	void TypeManager::RegisterFeature( CallResult& callResult )
	{
		if ( callResult.bIsValid ) {
			OclSignature::Feature::FeatureKind eKind = callResult.uResult.pFeature->GetKind();
			switch ( eKind ) {
				case OclSignature::Feature::FK_OPERATOR 		:
					{
						OclImplementation::Operator* impl = ( (Operator*) callResult.uResult.pFeature )->GetImplementation();
						if (impl != NULL)
							impl->m_pTypeManager = this;
					}
					break;
				case OclSignature::Feature::FK_FUNCTION 		:
					{
						OclImplementation::Function* impl = ( (Function*) callResult.uResult.pFeature )->GetImplementation();
						if (impl != NULL)
							impl->m_pTypeManager = this;
					}
					break;
				case OclSignature::Feature::FK_ATTRIBUTE 		:
					{
						OclImplementation::Attribute* impl = ( (Attribute*) callResult.uResult.pFeature )->GetImplementation();
						if (impl != NULL)
							impl->m_pTypeManager = this;
					}
					break;
				case OclSignature::Feature::FK_ASSOCIATION		:
					{
						OclImplementation::Association* impl = ( (Association*) callResult.uResult.pFeature )->GetImplementation();
						if (impl != NULL)
							impl->m_pTypeManager = this;
					}
					break;
				case OclSignature::Feature::FK_METHOD 			:
					{
						OclImplementation::Method* impl = ( (Method*) callResult.uResult.pFeature )->GetImplementation();
						if (impl != NULL)
							impl->m_pTypeManager = this;
					}
					break;
				case OclSignature::Feature::FK_ITERATOR 		:
					{
						OclImplementation::Iterator* impl = ( (Iterator*) callResult.uResult.pFeature )->GetImplementation();
						if (impl != NULL)
							impl->m_pTypeManager = this;
					}
					break;
			}
		}
	}

	void TypeManager::RegisterType( TypeResult& typeResult )
	{
		if ( typeResult.bIsValid ) {
			typeResult.pType->m_pTypeManager = this;
			typeResult.pType->m_pAttributeFactory->m_pTypeManager = this;
			typeResult.pType->m_pAssociationFactory->m_pTypeManager = this;
			typeResult.pType->m_pMethodFactory->m_pTypeManager = this;
			if ( typeResult.pType->IsCompound() )
				( (CompoundType*) typeResult.pType.get() )->m_pIteratorFactory->m_pTypeManager = this;
		}
	}

//##############################################################################################################################################
//
//	C L A S S : OclMeta::Type
//
//##############################################################################################################################################

	Type::Type( const std::string& strName, const StringVector& vecSuperTypes, OclImplementation::AttributeFactory* pAttributeFactory, OclImplementation::AssociationFactory* pAssociationFactory, OclImplementation::MethodFactory* pMethodFactory, bool bDynamic )
		: m_strName( strName ), m_vecSuperTypes( vecSuperTypes ), m_pAttributeFactory( pAttributeFactory ), m_pAssociationFactory( pAssociationFactory ), m_pMethodFactory( pMethodFactory ), m_bDynamic( bDynamic )
	{
	}

	Type::~Type()
	{
		DisposeCallMap( m_mapAttributes );
		DisposeCallMap( m_mapAssociations );
		DisposeCallMap( m_mapMethods );
		delete m_pAttributeFactory;
		delete m_pAssociationFactory;
		delete m_pMethodFactory;
	}

	bool Type::IsDynamic() const
	{
		return m_bDynamic;
	}

	std::string Type::GetName() const
	{
		return m_strName;
	}

	const StringVector& Type::GetSuperTypeNames() const
	{
		return m_vecSuperTypes;
	}

	TypeManager* Type::GetTypeManager() const
	{
		return m_pTypeManager;
	}

	bool Type::IsCompound() const
	{
		return false;
	}

	Attribute* Type::GetAttribute( const OclSignature::Attribute& signature )
	{
		CallResult callResult;
		std::string signo = signature.Print();

		if ( GetCallResult( m_mapAttributes, signo/*signature*/, callResult ) )
			return ReturnCallResult<Attribute>( callResult );

		callResult = GetResults( signature );

		m_mapAttributes.insert( CallResultMap::value_type( signature.Print(), callResult ) );
		m_pTypeManager->RegisterFeature( callResult );
		return ReturnCallResult<Attribute>( callResult );
	}

	Association* Type::GetAssociation( const OclSignature::Association& signature )
	{
		CallResult callResult;
		std::string signo = signature.Print();

		if ( GetCallResult( m_mapAssociations, signo/*signature*/, callResult ) )
			return ReturnCallResult<Association>( callResult );

		callResult = GetResults( signature );

		m_mapAssociations.insert( CallResultMap::value_type( signature.Print(), callResult ) );
		m_pTypeManager->RegisterFeature( callResult );
		return ReturnCallResult<Association>( callResult );
	}

	Method* Type::GetMethod( const OclSignature::Method& signature )
	{
		CallResult callResult;
		std::string signo = signature.Print();

		if ( GetCallResult( m_mapMethods, signo/*signature*/, callResult ) )
			return ReturnCallResult<Method>( callResult );

		callResult = GetResults( signature );

		m_mapMethods.insert( CallResultMap::value_type( signature.Print(), callResult ) );
		m_pTypeManager->RegisterFeature( callResult );
		return ReturnCallResult<Method>( callResult );
	}

	CallResult Type::GetResults( const OclSignature::Attribute& signature )
	{
		CallResult callResult;

		AttributeVector vecAttributes;
		try {
			m_pAttributeFactory->GetFeatures( signature, vecAttributes );
			m_pTypeManager->GetDefinitionFactory()->GetFeatures( signature, vecAttributes );

			FilterTypeableFeature<Attribute,OclSignature::Attribute>( signature, vecAttributes, EX_ATTRIBUTE_AMBIGUOUS, EX_ATTRIBUTE_DOESNT_EXIST, callResult );
			if ( callResult.bIsValid || callResult.uResult.pException->GetCode() != EX_ATTRIBUTE_DOESNT_EXIST )
				return callResult;
			else
				delete callResult.uResult.pException;

			if ( ! FilterBaseType<Attribute,OclSignature::Attribute>( m_pTypeManager, m_vecSuperTypes, signature, vecAttributes, EX_ATTRIBUTE_DOESNT_EXIST, callResult ) )
				return callResult;

			FilterTypeableFeature<Attribute,OclSignature::Attribute>( signature, vecAttributes, EX_ATTRIBUTE_AMBIGUOUS, EX_ATTRIBUTE_DOESNT_EXIST, callResult );
		}
		catch ( OclCommon::Exception ex ) {
			DisposeVector<Attribute>( vecAttributes );
			callResult.bIsValid = false;
			callResult.uResult.pException = new OclCommon::Exception( ex );
		}

		return callResult;
	}

	CallResult Type::GetResults( const OclSignature::Association& signature )
	{
		CallResult callResult;

		AssociationVector vecAssociations;
		try {
			m_pAssociationFactory->GetFeatures( signature, vecAssociations );

			FilterTypeableFeature<Association,OclSignature::Association>( signature, vecAssociations, EX_ASSOCIATION_AMBIGUOUS, EX_ASSOCIATION_DOESNT_EXIST, callResult );
			if ( callResult.bIsValid || callResult.uResult.pException->GetCode() != EX_ASSOCIATION_DOESNT_EXIST )
				return callResult;
			else
				delete callResult.uResult.pException;

			if ( ! FilterBaseType<Association,OclSignature::Association>( m_pTypeManager, m_vecSuperTypes, signature, vecAssociations, EX_ASSOCIATION_DOESNT_EXIST, callResult ) )
				return callResult;

			FilterTypeableFeature<Association,OclSignature::Association>( signature, vecAssociations, EX_ASSOCIATION_AMBIGUOUS, EX_ASSOCIATION_DOESNT_EXIST, callResult );
			}
		catch ( OclCommon::Exception ex ) {
			DisposeVector<Association>( vecAssociations );
			callResult.bIsValid = false;
			callResult.uResult.pException = new OclCommon::Exception( ex );
		}

		return callResult;
	}

	CallResult Type::GetResults( const OclSignature::Method& signature )
	{
		CallResult callResult;

		MethodVector vecMethods;
		try {
			m_pMethodFactory->GetFeatures( signature, vecMethods);
			m_pTypeManager->GetDefinitionFactory()->GetFeatures( signature, vecMethods );

			FilterParametralFeature<Method,OclSignature::Method>( m_pTypeManager, signature, vecMethods, EX_METHOD_AMBIGUOUS, EX_METHOD_DOESNT_EXIST, callResult );
			if ( callResult.bIsValid || callResult.uResult.pException->GetCode() != EX_METHOD_DOESNT_EXIST )
				return callResult;
			else
				delete callResult.uResult.pException;

			if ( ! FilterBaseType<Method,OclSignature::Method>( m_pTypeManager, m_vecSuperTypes, signature, vecMethods, EX_METHOD_DOESNT_EXIST, callResult ) )
				return callResult;

			FilterParametralFeature<Method,OclSignature::Method>( m_pTypeManager, signature, vecMethods, EX_METHOD_AMBIGUOUS, EX_METHOD_DOESNT_EXIST, callResult );
		}
		catch ( OclCommon::Exception ex ) {
			DisposeVector<Method>( vecMethods );
			callResult.bIsValid = false;
			callResult.uResult.pException = new OclCommon::Exception( ex );
		}

		return callResult;
	}

	CallResult Type::GetResults( const OclSignature::Iterator& signature )
	{
		CallResult callResult;
		callResult.bIsValid = false;
		callResult.uResult.pException = new OclCommon::Exception( OclCommon::Exception::ET_SEMANTIC, EX_ITERATOR_DOESNT_EXIST, signature.Print() );
		return callResult;
	}

//##############################################################################################################################################
//
//	C L A S S : OclMeta::CompoundType
//
//##############################################################################################################################################

	CompoundType::CompoundType( const std::string& strName, const StringVector& vecSuperTypes, OclImplementation::AttributeFactory* pAttributeFactory, OclImplementation::AssociationFactory* pAssociationFactory, OclImplementation::MethodFactory* pMethodFactory, OclImplementation::IteratorFactory* pIteratorFactory, bool bDynamic )
		: Type( strName, vecSuperTypes, pAttributeFactory, pAssociationFactory, pMethodFactory, bDynamic ), m_pIteratorFactory( pIteratorFactory )
	{
	}

	CompoundType::~CompoundType()
	{
		DisposeCallMap( m_mapIterators );
		delete m_pIteratorFactory;
	}

	bool CompoundType::IsCompound() const
	{
		return true;
	}

	Iterator* CompoundType::GetIterator(int level, const OclSignature::Iterator& signature )
	{
		CallResult callResult;

		// recursion corrected: terge
		char signoStr[100];
		std::string signo = signature.Print();
#ifdef _WIN32
		_itoa_s(level, signoStr, sizeof(signoStr), 10);  
#else
		sprintf( signoStr, "%ld", level );
#endif
		signo += signoStr;

		if ( GetCallResult( m_mapIterators, signo/*signature*/, callResult ) )
			return ReturnCallResult<Iterator>( callResult );

		callResult = GetResults( signature );

		m_mapIterators.insert( CallResultMap::value_type( signo/*signature.Print()*/, callResult ) );
		GetTypeManager()->RegisterFeature( callResult );
		return ReturnCallResult<Iterator>( callResult );
	}

	CallResult CompoundType::GetResults( const OclSignature::Iterator& signature )
	{
		CallResult callResult;

		IteratorVector vecIterators;
		try {
			m_pIteratorFactory->GetFeatures( signature, vecIterators);

			FilterParametralFeature<Iterator,OclSignature::Iterator>( GetTypeManager(), signature, vecIterators, EX_ITERATOR_AMBIGUOUS, EX_ITERATOR_DOESNT_EXIST, callResult );
			if ( callResult.bIsValid || callResult.uResult.pException->GetCode() != EX_ITERATOR_DOESNT_EXIST )
				return callResult;
			else
				delete callResult.uResult.pException;

			if ( ! FilterBaseType<Iterator,OclSignature::Iterator>( GetTypeManager(), GetSuperTypeNames(), signature, vecIterators, EX_ITERATOR_DOESNT_EXIST, callResult ) )
				return callResult;

			FilterParametralFeature<Iterator,OclSignature::Iterator>( GetTypeManager(), signature, vecIterators, EX_ITERATOR_AMBIGUOUS, EX_ITERATOR_DOESNT_EXIST, callResult );
		}
		catch ( OclCommon::Exception ex ) {
			DisposeVector<Iterator>( vecIterators );
			callResult.bIsValid = false;
			callResult.uResult.pException = new OclCommon::Exception( ex );
		}

		return callResult;
	}

}; // namespace OclMeta
