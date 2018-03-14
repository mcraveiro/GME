//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLSignature.cpp
//
//###############################################################################################################################################
#include "Solve4786.h"
#include "OCLSignature.h"

namespace OclSignature
{

//##############################################################################################################################################
//
//	A B S T R A C T   C L A S S : OclSignature::Feature
//
//##############################################################################################################################################

	Feature::Feature( const std::string& strName, FeatureKind eKind )
		: m_strName( strName ), m_eKind( eKind )
	{
	}

	Feature::~Feature()
	{
	}

	std::string Feature::GetName() const
	{
		return m_strName;
	}

	Feature::FeatureKind Feature::GetKind() const
	{
		return m_eKind;
	}

	std::string Feature::Print() const
	{
		return m_strName;
	}

	bool Feature::IsIdentical( const Feature& object ) const
	{
		return m_strName == object.m_strName;
	}

//##############################################################################################################################################
//
//	C L A S S : OclSignature::ParametralFeature
//
//##############################################################################################################################################

	ParametralFeature::ParametralFeature( const OclCommon::FormalParameterVector& vecParameters )
		: m_vecParameters( vecParameters )
	{
	}

	ParametralFeature::~ParametralFeature()
	{
	}

	int ParametralFeature::GetParameterCount() const
	{
		return m_vecParameters.size();
	}

	int ParametralFeature::GetMinParameterCount() const
	{
		for ( int i = m_vecParameters.size()-1 ; i >= 0 ; i-- )
			if ( m_vecParameters[ i ].IsRequired() )
				return i+1;
		return 0;
	}

	const OclCommon::FormalParameter& ParametralFeature::GetParameter( int i ) const
	{
		return m_vecParameters[ i ];
	}

	OclCommon::FormalParameterVector& ParametralFeature::GetParameters()
	{
		return m_vecParameters;
	}

	std::string ParametralFeature::Print() const
	{
		std::string strSignature = "( ";
		for ( int i = 0 ; i < GetParameterCount() ; i++ ) {
			OclCommon::FormalParameter parameter = GetParameter( i );
			if ( ! parameter.IsRequired() )
				strSignature += " { ";
			if ( i != 0 )
				strSignature += ", ";
			strSignature += parameter.GetTypeName();
			if ( i == GetParameterCount() - 1 && ! parameter.IsRequired() )
				strSignature += "} ";
		}
		return strSignature + " )";
	}

	bool ParametralFeature::IsIdentical( const ParametralFeature& object ) const
	{
		return m_vecParameters == object.m_vecParameters;
	}

//##############################################################################################################################################
//
//	C L A S S : OclSignature::TypeableFeature
//
//##############################################################################################################################################

	TypeableFeature::TypeableFeature( const std::string& strTypeName )
		: m_strTypeName( strTypeName )
	{
	}

	TypeableFeature::TypeableFeature()
		: m_strTypeName( "" )
	{
	}

	TypeableFeature::~TypeableFeature()
	{
	}

	std::string TypeableFeature::GetTypeName() const
	{
		return m_strTypeName;
	}

	std::string TypeableFeature::Print() const
	{
		if ( m_strTypeName.empty() )
			return "";
		return m_strTypeName + "::";
	}

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Attribute <<< + Feature , + TypeableFeature
//
//##############################################################################################################################################

	Attribute::Attribute( const std::string& strName, const std::string& strTypeName )
		: Feature( strName, Feature::FK_ATTRIBUTE ), TypeableFeature( strTypeName )
	{
	}

	Attribute::Attribute( const std::string& strName )
		: Feature( strName, Feature::FK_ATTRIBUTE ), TypeableFeature()
	{
	}

	Attribute::~Attribute()
	{
	}

	std::string Attribute::Print() const
	{
		return TypeableFeature::Print() + Feature::Print();
	}

	bool Attribute::IsIdentical( const Attribute& object ) const
	{
		return Feature::IsIdentical( object );
	}

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Assocation <<< + Feature , + TypeableFeature
//
//##############################################################################################################################################

	Association::Association( const std::string& strName, const std::string& strTypeName, const std::string& strAcceptable )
		: Feature( strName, Feature::FK_ASSOCIATION ), TypeableFeature( strTypeName ), m_strAcceptable( strAcceptable )
	{
	}

	Association::Association( const std::string& strName, const std::string& strAcceptable )
		: Feature( strName, Feature::FK_ASSOCIATION ), TypeableFeature(), m_strAcceptable( strAcceptable )
	{
	}

	Association::~Association()
	{
	}

	std::string Association::Print() const
	{
		if ( m_strAcceptable.empty() )
			return TypeableFeature::Print() + Feature::Print();
		return TypeableFeature::Print() + m_strAcceptable + "[ " + Feature::Print() + " ]";
	}

	std::string Association::GetAcceptableTypeName() const
	{
		return m_strAcceptable;
	}

	bool Association::IsIdentical( const Association& object ) const
	{
		return Feature::IsIdentical( object ) && m_strAcceptable == object.m_strAcceptable;
	}

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Iterator <<< + Feature , + TypeableFeature, + ParametralFeature
//
//##############################################################################################################################################

	Iterator::Iterator( const std::string& strName, const std::string& strTypeName, const std::string& strParameterType )
		: Feature( strName, Feature::FK_ITERATOR ), TypeableFeature( strTypeName ), ParametralFeature( OclCommon::FormalParameterVector( 1, OclCommon::FormalParameter( strParameterType, true ) ) )
	{
	}

	Iterator::Iterator( const std::string& strName, const std::string& strParameterType )
		: Feature( strName, Feature::FK_ITERATOR ), TypeableFeature(), ParametralFeature( OclCommon::FormalParameterVector( 1, OclCommon::FormalParameter( strParameterType, true ) ) )
	{
	}

	Iterator::~Iterator()
	{
	}

	std::string Iterator::Print() const
	{
		return TypeableFeature::Print() + Feature::Print() +  "( ... | " + GetParameter( 0 ).GetTypeName() + " )";
	}

	bool Iterator::IsIdentical( const Iterator& object ) const
	{
		return Feature::IsIdentical( object ) && ParametralFeature::IsIdentical( object );
	}

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Method <<< + Feature , + TypeableFeature, + ParametralFeature
//
//##############################################################################################################################################

	Method::Method( const std::string& strName, const std::string& strTypeName, const StringVector& vecTypes )
		: Feature( strName, Feature::FK_METHOD ), TypeableFeature( strTypeName ), ParametralFeature( OclCommon::FormalParameterVector() )
	{
		OclCommon::FormalParameterVector& vecParameters = GetParameters();
		for ( unsigned int i = 0 ; i < vecTypes.size() ; i++ )
			vecParameters.push_back( OclCommon::FormalParameter( vecTypes[ i ], true ) );
	}

	Method::Method( const std::string& strName, const OclCommon::FormalParameterVector& vecParameters )
		: Feature( strName, Feature::FK_METHOD ), TypeableFeature(), ParametralFeature( vecParameters )
	{
	}

	Method::~Method()
	{
	}

	std::string Method::Print() const
	{
		return  TypeableFeature::Print() + Feature::Print() + ParametralFeature::Print();
	}

	bool Method::IsIdentical( const Method& object ) const
	{
		return Feature::IsIdentical( object ) && ParametralFeature::IsIdentical( object );
	}

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Operator <<< + Feature , + ParametralFeature
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	Operator::Operator( const std::string& strName, const std::string& strOperand1Type )
		: Feature( strName, Feature::FK_OPERATOR ), ParametralFeature( OclCommon::FormalParameterVector( 1, OclCommon::FormalParameter( strOperand1Type, true ) ) )
	{
	}

	Operator::Operator( const std::string& strName, const std::string& strOperand1Type, const std::string& strOperand2Type )
		: Feature( strName, Feature::FK_OPERATOR ), ParametralFeature( OclCommon::FormalParameterVector() )
	{
		OclCommon::FormalParameterVector& vecParameters = GetParameters();
		vecParameters.push_back( OclCommon::FormalParameter( strOperand1Type, true ) );
		vecParameters.push_back( OclCommon::FormalParameter( strOperand2Type, true ) );
	}

	Operator::~Operator()
	{
	}

	std::string Operator::Print() const
	{
		return "operator[ " + Feature::Print() + " ]" + ParametralFeature::Print();
	}

	bool Operator::IsIdentical( const Operator& object ) const
	{
		return Feature::IsIdentical( object ) && ParametralFeature::IsIdentical( object );
	}

//##############################################################################################################################################
//
//	C L A S S : OclSignature::Function <<< + Feature , + ParametralFeature
//
//##############################################################################################################################################

	Function::Function( const std::string& strName, const StringVector& vecTypes )
		: Feature( strName, Feature::FK_METHOD ), ParametralFeature( OclCommon::FormalParameterVector() )
	{
		OclCommon::FormalParameterVector& vecParameters = GetParameters();
		for ( unsigned int i = 0 ; i < vecTypes.size() ; i++ )
			vecParameters.push_back( OclCommon::FormalParameter( vecTypes[ i ], true ) );
	}

	Function::Function( const std::string& strName, const OclCommon::FormalParameterVector& vecParameters )
		: Feature( strName, Feature::FK_METHOD ), ParametralFeature( vecParameters )
	{
	}

	Function::~Function()
	{
	}

	std::string Function::Print() const
	{
		return Feature::Print() + ParametralFeature::Print();
	}

	bool Function::IsIdentical( const Function& object ) const
	{
		return Feature::IsIdentical( object ) && ParametralFeature::IsIdentical( object );
	}


}; // namespace OclSignature