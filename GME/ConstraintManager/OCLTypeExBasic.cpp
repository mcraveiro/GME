//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLTypeExBasic.cpp
//
//###############################################################################################################################################
#include "Solve4786.h"
#include "OCLTypeExBasic.h"

#include "OCLObjectExBasic.h"
#include "math.h"
#include <algorithm> // needed for ms stl
#include <string>
#include <cctype>


namespace OclBasic
{
	typedef OclCommon::FormalParameterVector 	FPV;
	typedef OclCommon::FormalParameter			FP;
	typedef TypeSeq									TS;

	typedef OclImplementation::Iterator				OclIterator;

//##############################################################################################################################################
//
//	F U N C T I O N S
//
//##############################################################################################################################################

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Any
//
//##############################################################################################################################################

	OPERATOR( TAny_Equals )
	{
		void operator()()
		{
			SetResult( CREATE_BOOLEAN( GetTypeManager(), GetArgument( 0 ).Equals( GetArgument( 1 ) ) ) );
		}
	};

	OPERATOR( TAny_NotEquals )
	{
		void operator()()
		{
			SetResult( CREATE_BOOLEAN( GetTypeManager(), ! GetArgument( 0 ).Equals( GetArgument( 1 ) ) ) );
		}
	};

	OPERATOR( TAny_IdentityEquals )
	{
		void operator()()
		{
			SetResult( CREATE_BOOLEAN( GetTypeManager(), GetArgument( 0 ).GetImplementation() == GetArgument( 1 ).GetImplementation() ) );
		}
	};

	OPERATOR( TAny_IdentityNotEquals )
	{
		void operator()()
		{
			SetResult( CREATE_BOOLEAN( GetTypeManager(), GetArgument( 0 ).GetImplementation() != GetArgument( 1 ).GetImplementation() ) );
		}
	};

	METHOD( TAny_IsTypeOf )
	{
		void operator()()
		{
			DECL_TYPE( strType, GetArgument( 0 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), GetTypeManager()->IsTypeA( GetThis().GetTypeName(), strType ) == 0 ) );
		}
	};

	METHOD( TAny_IsKindOf )
	{
		void operator()()
		{
			DECL_TYPE( strType, GetArgument( 0 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), GetTypeManager()->IsTypeA( GetThis().GetTypeName(), strType ) >= 0 ) );
		}
	};

	void TAny_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "isTypeOf" || strName == "oclIsTypeOf" ) && iCount == 1 ) {
			vecParams.push_back( FP( "type", "ocl::Type", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TAny_IsTypeOf(), false ) );
			return;
		}

		if ( ( strName == "isKindOf" || strName == "oclIsKindOf" ) && iCount == 1 ) {
			vecParams.push_back( FP( "type", "ocl::Type", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TAny_IsKindOf(), false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::String
//
//##############################################################################################################################################

	OPERATOR( TString_Less )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetArgument( 0 ) );
			DECL_STRING( strValue, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), strThis < strValue ) );
		}
	};

	OPERATOR( TString_LessEquals )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetArgument( 0 ) );
			DECL_STRING( strValue, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), strThis <= strValue ) );
		}
	};

	OPERATOR( TString_Greater )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetArgument( 0 ) );
			DECL_STRING( strValue, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), strThis > strValue ) );
		}
	};

	OPERATOR( TString_GreaterEquals )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetArgument( 0 ) );
			DECL_STRING( strValue, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), strThis >= strValue ) );
		}
	};

	OPERATOR( TString_Plus )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetArgument( 0 ) );
			DECL_STRING( strValue, GetArgument( 1 ) );
			SetResult( CREATE_STRING( GetTypeManager(), strThis + strValue ) );
		}
	};

	ATTRIBUTE( TString_Size )
	{
		void operator()()
		{
			DECL_STRING( strValue, GetThis() );
			SetResult( CREATE_INTEGER( GetTypeManager(), strValue.length() ) );
		}
	};

	METHOD( TString_ToUpper )
	{
		void operator()()
		{
			DECL_STRING( strValue, GetThis() );
			std::transform(strValue.begin(), strValue.end(), strValue.begin(), std::toupper);
			SetResult( CREATE_STRING( GetTypeManager(), strValue ) );
		}
	};

	METHOD( TString_ToLower )
	{
		void operator()()
		{
			DECL_STRING( strValue, GetThis() );
			std::transform(strValue.begin(), strValue.end(), strValue.begin(), std::tolower);
			SetResult( CREATE_STRING( GetTypeManager(), strValue ) );
		}
	};

	METHOD( TString_Concat )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetThis() );
			DECL_STRING( strValue, GetArgument( 0 ) );
			SetResult( CREATE_STRING( GetTypeManager(), strThis + strValue ) );
		}
	};

	METHOD( TString_Substring )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetThis() );
			DECL_INTEGER( iFrom, GetArgument( 0 ) );
			if ( iFrom < 0 ) {
				ThrowException( "Argument 'from' is less than 0." );
				return;
			}
			if ( iFrom >= (int) strThis.length() ) {
				ThrowException( "Argument 'from' equals to or is greater than size of string." );
				return;
			}
			if ( GetArgumentCount() == 1 ) {
				SetResult( CREATE_STRING( GetTypeManager(), strThis.substr( iFrom ) ) );
				return;
			}
			DECL_INTEGER( iTo, GetArgument( 1 ) );
			if ( iTo < iFrom ) {
				ThrowException( "Argument 'to' greater than Argument 'from'." );
				return;
			}
			if ( iTo > (int) strThis.length() ) {
				ThrowException( "Argument 'to' is greater than size of string." );
				return;
			}
			std::string res = strThis.substr( iFrom, iTo-iFrom );
			SetResult( CREATE_STRING( GetTypeManager(), res ) );
		}
	};

	METHOD( TString_ToInteger  )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetThis() );
			Trim( strThis );
			unsigned int i = 0;
			if (strThis[ i ] == '-'  ||  strThis[ i ] == '+')
				i++;
			if (i >= strThis.length())
				ThrowException( "String cannot be converted to ocl::Integer." );
			for ( ; i < strThis.length() ; i ++ )
				if ( strThis[ i ] < '0' || strThis[ i ] > '9' )
					ThrowException( "String cannot be converted to ocl::Integer." );
			long lValue = atol( strThis.c_str() );
			SetResult( CREATE_INTEGER( GetTypeManager(), lValue ) );
		}
	};

	METHOD( TString_ToReal  )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetThis() );
			char* pchStop;
			double dValue = strtod( strThis.c_str(), &pchStop );
			if ( fabs( dValue ) == HUGE_VAL )
				ThrowException( "String cannot be converted to ocl::Real." );
			SetResult( CREATE_REAL( GetTypeManager(), dValue ) );
		}
	};

	METHOD( TString_Trim )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetThis() );
			Trim( strThis );
			SetResult( CREATE_STRING( GetTypeManager(), strThis ) );
		}
	};

	void TString_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "size" ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, new TString_Size(), false ) );
			return;
		}
	}

	void TString_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "toUpper" ) && iCount == 0 ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TString_ToUpper(), false ) );
			return;
		}

		if ( ( strName == "toLower" ) && iCount == 0 ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TString_ToLower(), false ) );
			return;
		}

		if ( ( strName == "concat" ) && iCount == 1 ) {
			vecParams.push_back( FP( "string", "ocl::String", true ) );
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TString_Concat(), false ) );
			return;
		}

		if ( ( strName == "toReal" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TString_ToReal(), false ) );
			return;
		}

		if ( ( strName == "toInteger" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TString_ToInteger(), false ) );
			return;
		}

		if ( ( strName == "substring" ) && iCount >= 1 && iCount <= 2 ) {
			vecParams.push_back( FP( "iFrom", "ocl::Integer", true ) );
			vecParams.push_back( FP( "iTo", "ocl::Integer", false ) );
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TString_Substring(), false ) );
			return;
		}

		if ( ( strName == "trim" && iCount == 0 ) ){
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TString_Trim(), false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Enumeration
//
//##############################################################################################################################################

	METHOD( TEnumeration_ToString  )
	{
		void operator()()
		{
			DECL_ENUMERATION( enumThis, GetThis() );
			SetResult( CREATE_STRING( GetTypeManager(), enumThis ) );
		}
	};

	void TEnumeration_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "toString" ) && iCount == 0 ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TEnumeration_ToString(), false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Boolean
//
//##############################################################################################################################################

	OPERATOR( TBoolean_And )
	{
		void operator()()
		{
			DECL_BOOLEAN( bArg1, GetArgument( 0 ) );
			DECL_BOOLEAN( bArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), bArg1 && bArg2 ) );
		}
	};

	OPERATOR( TBoolean_Or )
	{
		void operator()()
		{
			DECL_BOOLEAN( bArg1, GetArgument( 0 ) );
			DECL_BOOLEAN( bArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), bArg1 || bArg2 ) );
		}
	};

	OPERATOR( TBoolean_Not )
	{
		void operator()()
		{
			DECL_BOOLEAN( bArg, GetArgument( 0 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), ! bArg ) );
		}
	};

	OPERATOR( TBoolean_Implies )
	{
		void operator()()
		{
			DECL_BOOLEAN( bArg1, GetArgument( 0 ) );
			DECL_BOOLEAN( bArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), ! bArg1 || bArg2 ) );
		}
	};

	OPERATOR( TBoolean_Xor )
	{
		void operator()()
		{
			DECL_BOOLEAN( bArg1, GetArgument( 0 ) );
			DECL_BOOLEAN( bArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), bArg1 && ! bArg2 || ! bArg1 && bArg2 ) );
		}
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Real
//
//##############################################################################################################################################

	OPERATOR( TReal_Equals )
	{
		void operator()()
		{
			DECL_REAL( dThis, GetArgument( 0 ) );
			DECL_REAL( dValue, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), dThis == dValue ) );
		}
	};

	OPERATOR( TReal_NotEquals )
	{
		void operator()()
		{
			DECL_REAL( dThis, GetArgument( 0 ) );
			DECL_REAL( dValue, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), dThis != dValue ) );
		}
	};

	OPERATOR( TReal_Minus1 )
	{
		void operator()()
		{
			DECL_REAL( dArg, GetArgument( 0 ) );
			SetResult( CREATE_REAL( GetTypeManager(), - dArg ) );
		}
	};

	OPERATOR( TReal_Plus )
	{
		void operator()()
		{
			DECL_REAL( dArg1, GetArgument( 0 ) );
			DECL_REAL( dArg2, GetArgument( 1 ) );
			SetResult( CREATE_REAL( GetTypeManager(), dArg1 + dArg2 ) );
		}
	};

	OPERATOR( TReal_Minus2 )
	{
		void operator()()
		{
			DECL_REAL( dArg1, GetArgument( 0 ) );
			DECL_REAL( dArg2, GetArgument( 1 ) );
			SetResult( CREATE_REAL( GetTypeManager(), dArg1 - dArg2 ) );
		}
	};

	OPERATOR( TReal_Slash )
	{
		void operator()()
		{
			DECL_REAL( dArg1, GetArgument( 0 ) );
			DECL_REAL( dArg2, GetArgument( 1 ) );
			SetResult( CREATE_REAL( GetTypeManager(), dArg1 / dArg2 ) );
		}
	};

	OPERATOR( TReal_Times )
	{
		void operator()()
		{
			DECL_REAL( dArg1, GetArgument( 0 ) );
			DECL_REAL( dArg2, GetArgument( 1 ) );
			SetResult( CREATE_REAL( GetTypeManager(), dArg1 * dArg2 ) );
		}
	};

	OPERATOR( TReal_Less )
	{
		void operator()()
		{
			DECL_REAL( dArg1, GetArgument( 0 ) );
			DECL_REAL( dArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), dArg1 < dArg2 ) );
		}
	};

	OPERATOR( TReal_Greater )
	{
		void operator()()
		{
			DECL_REAL( dArg1, GetArgument( 0 ) );
			DECL_REAL( dArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), dArg1 > dArg2 ) );
		}
	};

	OPERATOR( TReal_LessEquals )
	{
		void operator()()
		{
			DECL_REAL( dArg1, GetArgument( 0 ) );
			DECL_REAL( dArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), dArg1 <= dArg2 ) );
		}
	};

	OPERATOR( TReal_GreaterEquals )
	{
		void operator()()
		{
			DECL_REAL( dArg1, GetArgument( 0 ) );
			DECL_REAL( dArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), dArg1 >= dArg2 ) );
		}
	};

	FUNCTION( TReal_Abs )
	{
		void operator()()
		{
			DECL_REAL( dArg, GetArgument( 0 ) );
			SetResult( CREATE_REAL( GetTypeManager(), fabs( dArg ) ) );
		}
	};

	FUNCTION( TReal_Floor )
	{
		void operator()()
		{
			DECL_REAL( dArg, GetArgument( 0 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), (long)floor( dArg ) ) );
		}
	};

	FUNCTION( TReal_Round )
	{
		void operator()()
		{
			DECL_REAL( dArg, GetArgument( 0 ) );
			long lFloor = (long)floor( dArg );
			SetResult( CREATE_INTEGER( GetTypeManager(), lFloor + ( ( lFloor + 0.5 <= dArg && dArg >= 0 ) ? 1 : 0 ) ) );
		}
	};

	FUNCTION( TReal_Max )
	{
		void operator()()
		{
			DECL_REAL( dArg1, GetArgument( 0 ) );
			DECL_REAL( dArg2, GetArgument( 1 ) );
			SetResult( CREATE_REAL( GetTypeManager(), max( dArg1, dArg2 ) ) );
		}
	};

	FUNCTION( TReal_Min )
	{
		void operator()()
		{
			DECL_REAL( dArg1, GetArgument( 0 ) );
			DECL_REAL( dArg2, GetArgument( 1 ) );
			SetResult( CREATE_REAL( GetTypeManager(), min( dArg1, dArg2 ) ) );
		}
	};

	METHOD( TReal_Abs_Method )
	{
		void operator()()
		{
			DECL_REAL( dThis, GetThis() );
			SetResult( CREATE_REAL( GetTypeManager(), fabs( dThis ) ) );
		}
	};

	METHOD( TReal_Floor_Method )
	{
		void operator()()
		{
			DECL_REAL( dThis, GetThis() );
			SetResult( CREATE_INTEGER( GetTypeManager(), (long)floor( dThis ) ) );
		}
	};

	METHOD( TReal_Round_Method )
	{
		void operator()()
		{
			DECL_REAL( dThis, GetThis() );
			long lFloor = (long)floor( dThis );
			SetResult( CREATE_INTEGER( GetTypeManager(), lFloor + ( ( lFloor + 0.5 <= dThis && dThis >= 0 ) ? 1 : 0 ) ) );
		}
	};

	METHOD( TReal_Max_Method )
	{
		void operator()()
		{
			DECL_REAL( dThis, GetThis() );
			DECL_REAL( dArg, GetArgument( 0 ) );
			SetResult( CREATE_REAL( GetTypeManager(), max( dThis, dArg ) ) );
		}
	};

	METHOD( TReal_Min_Method )
	{
		void operator()()
		{
			DECL_REAL( dThis, GetThis() );
			DECL_REAL( dArg, GetArgument( 0 ) );
			SetResult( CREATE_REAL( GetTypeManager(), min( dThis, dArg ) ) );
		}
	};

	void TReal_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "abs" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReal_Abs_Method(), false ) );
			return;
		}

		if ( ( strName == "floor" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReal_Floor_Method(), false ) );
			return;
		}

		if ( ( strName == "round" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReal_Round_Method(), false ) );
			return;
		}

		if ( ( strName == "max" ) && iCount == 1 ) {
			vecParams.push_back( FP( "real", "ocl::Real", true ) );
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReal_Max_Method(), false ) );
			return;
		}

		if ( ( strName == "min" ) && iCount == 1 ) {
			vecParams.push_back( FP( "real", "ocl::Real", true ) );
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReal_Min_Method(), false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Integer
//
//##############################################################################################################################################

	OPERATOR( TInteger_Minus1 )
	{
		void operator()()
		{
			DECL_INTEGER( lArg, GetArgument( 0 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), - lArg ) );
		}
	};

	OPERATOR( TInteger_Plus )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), lArg1 + lArg2 ) );
		}
	};

	OPERATOR( TInteger_Minus2 )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), lArg1 - lArg2 ) );
		}
	};

	OPERATOR( TInteger_Div )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), lArg1 / lArg2 ) );
		}
	};

	OPERATOR( TInteger_Times )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), lArg1 * lArg2 ) );
		}
	};

	OPERATOR( TInteger_Mod )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), lArg1 % lArg2 ) );
		}
	};

	OPERATOR( TInteger_Less )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), lArg1 < lArg2 ) );
		}
	};

	OPERATOR( TInteger_Greater )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), lArg1 > lArg2 ) );
		}
	};

	OPERATOR( TInteger_LessEquals )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), lArg1 <= lArg2 ) );
		}
	};

	OPERATOR( TInteger_GreaterEquals )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), lArg1 >= lArg2 ) );
		}
	};

	FUNCTION( TInteger_Abs )
	{
		void operator()()
		{
			DECL_INTEGER( lArg, GetArgument( 0 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), labs( lArg ) ) );
		}
	};

	FUNCTION( TInteger_Max )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), max( lArg1, lArg2 ) ) );
		}
	};

	FUNCTION( TInteger_Min )
	{
		void operator()()
		{
			DECL_INTEGER( lArg1, GetArgument( 0 ) );
			DECL_INTEGER( lArg2, GetArgument( 1 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), min( lArg1, lArg2 ) ) );
		}
	};

	METHOD( TInteger_Abs_Method )
	{
		void operator()()
		{
			DECL_INTEGER( lThis, GetThis() );
			SetResult( CREATE_INTEGER( GetTypeManager(), labs( lThis ) ) );
		}
	};

	METHOD( TInteger_Max_Method )
	{
		void operator()()
		{
			DECL_INTEGER( lThis, GetThis() );
			DECL_INTEGER( lArg, GetArgument( 0 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), max( lThis, lArg ) ) );
		}
	};

	METHOD( TInteger_Min_Method )
	{
		void operator()()
		{
			DECL_INTEGER( lThis, GetThis() );
			DECL_INTEGER( lArg, GetArgument( 0 ) );
			SetResult( CREATE_INTEGER( GetTypeManager(), min( lThis, lArg ) ) );
		}
	};

	void TInteger_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "abs" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReal_Abs_Method(), false ) );
			return;
		}

		if ( ( strName == "max" ) && iCount == 1 ) {
			vecParams.push_back( FP( "integer", "ocl::Integer", true ) );
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReal_Max_Method(), false ) );
			return;
		}

		if ( ( strName == "min" ) && iCount == 1 ) {
			vecParams.push_back( FP( "integer", "ocl::Integer", true ) );
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReal_Min_Method(), false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Type
//
//##############################################################################################################################################

//##############################################################################################################################################
//
//	C O M M O N  F E A T U R E S   O F  ocl::Set , ocl::Bag , ocl::Sequence , ocl::OrderedSet
//
//##############################################################################################################################################

	ATTRIBUTE( TCollection_Size )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			SetResult( CREATE_INTEGER( GetTypeManager(), collThis.size() ) );
		}
	};

	METHOD( TCollection_IsEmpty )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), collThis.empty() ) );
		}
	};

	METHOD( TCollection_NotEmpty )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), ! collThis.empty() ) );
		}
	};

	METHOD( TCollection_Includes )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), std::find( collThis.begin(), collThis.end(), GetArgument( 0 ) ) != collThis.end() ) );
		}
	};

	METHOD( TCollection_Excludes )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), std::find( collThis.begin(), collThis.end(), GetArgument( 0 ) ) == collThis.end() ) );
		}
	};

	METHOD( TCollection_Count )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			long lResult = 0;
			for ( unsigned int i = 0 ; i < collThis.size() ; i++ )
				if ( collThis[ i ] == GetArgument( 0 ) )
					lResult++;
			SetResult( CREATE_INTEGER( GetTypeManager(), lResult ) );
		}
	};

	METHOD( TCollection_IncludesAll )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_ITERATOR( spIterator, GetArgument( 0 ) );
			while ( spIterator->HasNext() ) {
				OclMeta::ObjectVector::iterator i = std::find( collThis.begin(), collThis.end(), spIterator->GetNext() );
				if ( i == collThis.end() ) {
					SetResult( CREATE_BOOLEAN( GetTypeManager(), false ) );
					return;
				}
				collThis.erase( i );
			}
			SetResult( CREATE_BOOLEAN( GetTypeManager(), true ) );
		}
	};

	METHOD( TCollection_ExcludesAll )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_ITERATOR( spIterator, GetArgument( 0 ) );
			while ( spIterator->HasNext() )
				if ( std::find( collThis.begin(), collThis.end(), spIterator->GetNext() ) != collThis.end() ) {
					SetResult( CREATE_BOOLEAN( GetTypeManager(), false ) );
					return;
				}
			SetResult( CREATE_BOOLEAN( GetTypeManager(), true ) );
		}
	};

	METHOD( TCollection_AsSet )
	{
		void operator()()
		{
			if ( GetThis().GetTypeName() == "ocl::Set" ) {
				SetResult( GetThis() );
				return;
			}
			DECL_COLLECTION( collThis, GetThis() );
			SetResult( CREATE_SET( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TCollection_AsSequence )
	{
		void operator()()
		{
			if ( GetThis().GetTypeName() == "ocl::Sequence" ) {
				SetResult( GetThis() );
				return;
			}
			DECL_COLLECTION( collThis, GetThis() );
			SetResult( CREATE_SEQUENCE( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TCollection_AsBag )
	{
		void operator()()
		{
			if ( GetThis().GetTypeName() == "ocl::Bag" ) {
				SetResult( GetThis() );
				return;
			}
			DECL_COLLECTION( collThis, GetThis() );
			SetResult( CREATE_BAG( GetTypeManager(), collThis ) );
		}
	};
 
// -- 
	METHOD( TCollection_AsOrderedSet )
	{
		void operator()()
		{
			if ( GetThis().GetTypeName() == "ocl::OrderedSet" ) {
				SetResult( GetThis() );
				return;
			}
			DECL_COLLECTION( collThis, GetThis() );
			SetResult( CREATE_ORDEREDSET( GetTypeManager(), collThis ) );
		}
	};
// --
	ITERATOR( TCollection_Exists ) // true - if the collection contains at least one element - for which the condition evaluates to true
	{
		private:
			int iAccumulator;

		void Initialize()
		{
			OclIterator::Initialize();
			iAccumulator = -1;
		}

		void operator()()
		{
			if ( iAccumulator != 1 && ! GetSubResult().IsUndefined() ) {
				DECL_BOOLEAN( bArg, GetSubResult() );
				if ( bArg ) {
					iAccumulator = 1;
					SetDoStop( true );
				}
				else
					iAccumulator = 0;
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_BOOLEAN( GetTypeManager(), iAccumulator == 1 );
		}
	};

	ITERATOR( TCollection_ForAll )
	{
		private:
			int iAccumulator;

		void Initialize()
		{
			OclIterator::Initialize();
			iAccumulator = 1;
		}

		void operator()()
		{
			SetDoSnapshot( false );
			if ( iAccumulator != -1 ) {
				if ( GetSubResult().IsUndefined() ) {
					iAccumulator = -1;
					SetDoStop( true );
				}
				else {
					DECL_BOOLEAN( bArg, GetSubResult() );
					if ( ! bArg ) {
						iAccumulator = 0;
						SetDoStop( true );
						SetDoSnapshot( true );
					}
				}
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_BOOLEAN( GetTypeManager(), iAccumulator != 0 );
		}
	};

	ITERATOR( TCollection_IsUnique )
	{
		private :
			OclMeta::ObjectVector vecObjects;
			bool bResult;

		void Initialize()
		{
			OclIterator::Initialize();
			vecObjects.clear();
			bResult = true;
		}

		void operator()()
		{
			if ( bResult ) {
				if ( std::find( vecObjects.begin(), vecObjects.end(), GetSubResult() ) != vecObjects.end() ) {
					bResult = false;
					SetDoStop( true );
				}
				else
					vecObjects.push_back( GetSubResult() );
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_BOOLEAN( GetTypeManager(), bResult );
		}

		void Finalize()
		{
			vecObjects.clear();
			OclIterator::Finalize();
		}
	};

	ITERATOR( TCollection_Any ) // returns one element - the condition evaluates to true for that element
	{
		private :
			OclMeta::ObjectVector vecObjects;

		void Initialize()
		{
			OclIterator::Initialize();
			vecObjects.clear();
		}

		void operator()()
		{
			if ( GetSubResult().IsUndefined() )
				return;
			DECL_BOOLEAN( bResult, GetSubResult() );
			if ( bResult ) {
				vecObjects.push_back( GetArgument( 0 ) );
				SetDoStop( true );
			}
		}

		OclMeta::Object GetResult() const
		{
			return ( vecObjects.empty() ) ? OclMeta::Object::UNDEFINED : vecObjects[ 0 ];
		}

		void Finalize()
		{
			vecObjects.clear();
			OclIterator::Finalize();
		}
	};

	ITERATOR( TCollection_One ) // true - if the collection contains just one element - for which the condition evaluates to true
	{
		private :
			int iCount;

		void Initialize()
		{
			OclIterator::Initialize();
			iCount = 0;
		}

		void operator()()
		{
			if ( GetSubResult().IsUndefined() )
				return;
			DECL_BOOLEAN( bResult, GetSubResult() );
			if ( bResult ) {
				iCount++;
				if ( iCount > 1 )
					SetDoStop( true );
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_BOOLEAN( GetTypeManager(), iCount == 1 );
		}
	};

// -- 
//  
	ITERATOR( TCollection_SortedBy)
	{
		private :
			typedef std::map<long, OclMeta::Object> TintMap;
			TintMap intMap;
			bool isInt;
			typedef std::map<double, OclMeta::Object> TrealMap;
			TrealMap realMap;
			bool isReal;
			typedef std::map<std::string, OclMeta::Object> TstringMap;
			TstringMap stringMap;
			bool isString;


		void Initialize()
		{
			OclIterator::Initialize();
			intMap.clear();
			isInt = false;
			realMap.clear();
			isReal = false;
			stringMap.clear();
			isString = false;
		}

		void operator()()
		{
			OclMeta::Object keyObj = GetSubResult();
			if (!keyObj.IsComparable())
			{
				ThrowException( "Key must be: Integer or Real or String." );
				return;
			}
			OclMeta::Object store = GetSubOriResult();
			std::string keytype = keyObj.GetTypeName();

			if (keytype == "ocl::Integer")
			{
				isInt = true;
				DECL_INTEGER(key, keyObj);			
				intMap.insert(TintMap::value_type(key, store));
			}
			else if (keytype == "ocl::Real")
			{
				isReal = true;
				DECL_REAL(key, keyObj);			
				realMap.insert(TrealMap::value_type(key, store));
			}
			else if (keytype == "ocl::String")
			{
				isString = true;
				DECL_STRING(key, keyObj);			
				stringMap.insert(TstringMap::value_type(key, store));
			}
		
		}

		OclMeta::Object GetResult() const
		{
			OclMeta::ObjectVector vecSelecteds;
			vecSelecteds.clear();
			if (isInt)
			{
				TintMap::const_iterator it = intMap.begin();
				for (; it != intMap.end(); ++it)
					vecSelecteds.push_back( (*it).second);
			}
			else if (isReal)
			{
				TrealMap::const_iterator it = realMap.begin();
				for (; it != realMap.end(); ++it)
					vecSelecteds.push_back( (*it).second);
			}
			else if (isString)
			{
				TstringMap::const_iterator it = stringMap.begin();
				for (; it != stringMap.end(); ++it)
					vecSelecteds.push_back( (*it).second);
			}

			return CREATE_ORDEREDSET( GetTypeManager(), vecSelecteds );
		}

		void Finalize()
		{
			intMap.clear();
			isInt = false;
			realMap.clear();
			isReal = false;
			stringMap.clear();
			isString = false;
			OclIterator::Finalize();
		}
	};
// --

	void TCollection_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "size" ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, new TCollection_Size(), false ) );
			return;
		}
	}

	void TCollection_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "isEmpty" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_IsEmpty(), false ) );
			return;
		}

		if ( ( strName == "notEmpty" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_NotEmpty(), false ) );
			return;
		}

		if ( ( strName == "includes" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_Includes(), false ) );
			return;
		}

		if ( ( strName == "excludes" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_Excludes(), false ) );
			return;
		}

		if ( ( strName == "includesAll" ) && iCount == 1 ) {
			vecParams.push_back( FP( "collection", "ocl::Collection", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_IncludesAll(), false ) );
			return;
		}

		if ( ( strName == "excludesAll" ) && iCount == 1 ) {
			vecParams.push_back( FP( "collection", "ocl::Collection", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_ExcludesAll(), false ) );
			return;
		}

		if ( ( strName == "count" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_Count(), false ) );
			return;
		}

		if ( ( strName == "asSet" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_AsSet(), false ) );
			return;
		}

		if ( ( strName == "asBag" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_AsBag(), false ) );
			return;
		}

// -- 
		if ( ( strName == "asOrderedSet" ) && iCount == 0 ) {
			vecType.push_back( "ocl::OrderedSet" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_AsOrderedSet(), false ) );
			return;
		}
// --

		if ( ( strName == "asSequence" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollection_AsSequence(), false ) );
			return;
		}
	}

	void TCollection_IteratorFactory::GetFeatures( const OclSignature::Iterator& signature, OclMeta::IteratorVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "exists" ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TCollection_Exists(), false ) );
			return;
		}

		if ( strName == "forAll" ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TCollection_ForAll(), false ) );
			return;
		}

		if ( strName == "isUnique" ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Any", vecType, new TCollection_IsUnique(), false ) );
			return;
		}

		if ( strName == "any" ) {
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TCollection_Any(), false ) );
			return;
		}

		if ( strName == "one" ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TCollection_One(), false ) );
			return;
		}
// -- 
		if ( strName == "sortedBy" ) {
			vecType.push_back( "ocl::OrderedSet" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT ); // ??
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Any", vecType, new TCollection_SortedBy(), false ) );
			return;
		}
// --
	}

//##############################################################################################################################################
//
//	T Y P E   O F  ocl::Set
//
//##############################################################################################################################################

	OPERATOR( TSet_Plus )  // unio Set
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetArgument( 0 ) );
			DECL_ITERATOR( spIterator, GetArgument( 1 ) );
			while ( spIterator->HasNext() ) {
				OclMeta::Object object = spIterator->GetNext();
				if ( std::find( collThis.begin(), collThis.end(), object ) == collThis.end() )
					collThis.push_back( object );
			}
			SetResult( CREATE_SET( GetTypeManager(), collThis ) );
		}
	};

	OPERATOR( TSet_PlusBag )  // unio Bag
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetArgument( 0 ) );
			DECL_ITERATOR( spIterator, GetArgument( 1 ) );
			while ( spIterator->HasNext() )
				collThis.push_back( spIterator->GetNext() );
			SetResult( CREATE_BAG( GetTypeManager(), collThis ) );
		}
	};

	OPERATOR( TSet_Times ) // intersection
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetArgument( 0 ) );
			DECL_ITERATOR( spIterator, GetArgument( 1 ) );
			OclMeta::ObjectVector collOut;
			while ( spIterator->HasNext() ) {
				OclMeta::Object object = spIterator->GetNext();
				if ( std::find( collThis.begin(), collThis.end(), object ) != collThis.end() )
					if ( std::find( collOut.begin(), collOut.end(), object ) == collOut.end() ) // unnecessary
						collOut.push_back( object );
			}
			SetResult( CREATE_SET( GetTypeManager(), collOut ) );
		}
	};

	OPERATOR( TSet_Minus )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetArgument( 0 ) );
			DECL_ITERATOR( spIterator, GetArgument( 1 ) );
			while ( spIterator->HasNext() ) {
				OclMeta::ObjectVector::iterator i =  std::find( collThis.begin(), collThis.end(), spIterator->GetNext() );
				if ( i != collThis.end() )
					collThis.erase( i );
			}
			SetResult( CREATE_SET( GetTypeManager(), collThis ) );
		}
	};

	OPERATOR( TSet_Percent ) // symmetricdifference
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetArgument( 0 ) );
			DECL_ITERATOR( spIterator, GetArgument( 1 ) );
			OclMeta::ObjectVector collOut;
			OclMeta::ObjectVector collTemp;
			while ( spIterator->HasNext() ) {
				OclMeta::Object object = spIterator->GetNext();
				if ( std::find( collThis.begin(), collThis.end(), object ) == collThis.end() )
					collOut.push_back( object );
				else
					collTemp.push_back( object );
			}
			for ( unsigned int i = 0 ; i < collThis.size() ; i++ )
				if ( std::find( collTemp.begin(), collTemp.end(), collThis[ i ] ) == collTemp.end() )
					collOut.push_back( collThis[ i ] );
			SetResult( CREATE_SET( GetTypeManager(), collOut ) );
		}
	};

	METHOD( TSet_Union )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_ITERATOR( spIterator, GetArgument( 0 ) );
			while ( spIterator->HasNext() ) {
				OclMeta::Object object = spIterator->GetNext();
				if ( std::find( collThis.begin(), collThis.end(), object ) == collThis.end() )
					collThis.push_back( object );
			}
			SetResult( CREATE_SET( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TSet_UnionBag )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_ITERATOR( spIterator, GetArgument( 0 ) );
			while ( spIterator->HasNext() )
				collThis.push_back( spIterator->GetNext() );
			SetResult( CREATE_BAG( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TSet_Intersection )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_ITERATOR( spIterator, GetArgument( 0 ) );
			OclMeta::ObjectVector collOut;
			while ( spIterator->HasNext() ) {
				OclMeta::Object object = spIterator->GetNext();
				if ( ! ( std::find( collThis.begin(), collThis.end(), object ) == collThis.end() ) )
					if ( std::find( collOut.begin(), collOut.end(), object ) == collOut.end() )
						collOut.push_back( object );
			}
			SetResult( CREATE_SET( GetTypeManager(), collOut ) );
		}
	};

	METHOD( TSet_Subtract )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_ITERATOR( spIterator, GetArgument( 0 ) );
			while ( spIterator->HasNext() ) {
				OclMeta::ObjectVector::iterator i =  std::find( collThis.begin(), collThis.end(), spIterator->GetNext() );
				if ( i != collThis.end() )
					collThis.erase( i );
			}
			SetResult( CREATE_SET( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TSet_SymmDiff )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_ITERATOR( spIterator, GetArgument( 0 ) );
			OclMeta::ObjectVector collOut;
			OclMeta::ObjectVector collTemp;
			while ( spIterator->HasNext() ) {
				OclMeta::Object object = spIterator->GetNext();
				if ( std::find( collThis.begin(), collThis.end(), object ) == collThis.end() )
					collOut.push_back( object );
				else
					collTemp.push_back( object );
			}
			for ( unsigned int i = 0 ; i < collThis.size() ; i++ )
				if ( std::find( collTemp.begin(), collTemp.end(), collThis[ i ] ) == collTemp.end() )
					collOut.push_back( collThis[ i ] );
			SetResult( CREATE_SET( GetTypeManager(), collOut ) );
		}
	};

	METHOD( TSet_Including )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			if ( std::find( collThis.begin(), collThis.end(), GetArgument( 0 ) ) == collThis.end() )
				collThis.push_back( GetArgument( 0 ) );
			SetResult( CREATE_SET( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TSet_Excluding )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			OclMeta::ObjectVector::iterator i = std::find( collThis.begin(), collThis.end(), GetArgument( 0 ) );
			if ( i  != collThis.end() )
				collThis.erase( i );
			SetResult( CREATE_SET( GetTypeManager(), collThis ) );
		}
	};

	ITERATOR( TSet_Select )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			if ( ! GetSubResult().IsUndefined() ) {
				DECL_BOOLEAN( bArg, GetSubResult() );
				if ( bArg && std::find( m_vecSelecteds.begin(), m_vecSelecteds.end(), GetArgument( 0 ) ) == m_vecSelecteds.end() )
					m_vecSelecteds.push_back( GetArgument( 0 ) );
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_SET( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	ITERATOR( TSet_Reject )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			if ( ! GetSubResult().IsUndefined() ) {
				DECL_BOOLEAN( bArg, GetSubResult() );
				if ( ! bArg && std::find( m_vecSelecteds.begin(), m_vecSelecteds.end(), GetArgument( 0 ) ) == m_vecSelecteds.end() )
					m_vecSelecteds.push_back( GetArgument( 0 ) );
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_SET( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	ITERATOR( TSet_Collect )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			m_vecSelecteds.push_back( GetSubResult() );
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_BAG( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	void TSet_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "union" ) && iCount == 1 ) {
			vecParams.push_back( FP( "set", "ocl::Set", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSet_Union(), false ) );

			vecParams.clear();
			vecType.clear();
			vecParams.push_back( FP( "bag", "ocl::Bag", true ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSet_UnionBag(), false ) );
			return;
		}

		if ( ( strName == "subtract" ) && iCount == 1 ) {
			vecParams.push_back( FP( "set", "ocl::Set", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSet_Subtract(), false ) );
			return;
		}

		if ( ( strName == "intersection" ) && iCount == 1 ) {
			vecParams.push_back( FP( "set", "ocl::Set", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSet_Intersection(), false ) );

			vecParams.clear();
			vecType.clear();
			vecParams.push_back( FP( "bag", "ocl::Bag", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSet_Intersection(), false ) );
			return;
		}

		if ( ( strName == "symmetricDifference" ) && iCount == 1 ) {
			vecParams.push_back( FP( "set", "ocl::Set", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSet_SymmDiff(), false ) );
			return;
		}

		if ( ( strName == "including" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSet_Including(), false ) );
			return;
		}

		if ( ( strName == "excluding" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSet_Excluding(), false ) );
			return;
		}
	}

	void TSet_IteratorFactory::GetFeatures( const OclSignature::Iterator& signature, OclMeta::IteratorVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "select" ) {
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TSet_Select(), false ) );
			return;
		}

		if ( strName == "reject" ) {
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TSet_Reject(), false ) );
			return;
		}

		if ( strName == "collect" ) {
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_EXPRESSION_RETURN );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Any", vecType, new TSet_Collect(), false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F  ocl::Sequence
//
//##############################################################################################################################################

	OPERATOR( TSequence_Plus )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetArgument( 0 ) );
			DECL_COLLECTION( collArg, GetArgument( 1 ) );
			for ( unsigned int i = 0 ; i < collArg.size() ; i++ )
				collThis.push_back( collArg[ i ] );
			SetResult( CREATE_SEQUENCE( GetTypeManager(), collThis) );
		}
	};

	METHOD( TSequence_Union )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_COLLECTION( collArg, GetArgument( 0 ) );
			for ( unsigned int i = 0 ; i < collArg.size() ; i++ )
				collThis.push_back( collArg[ i ] );
			SetResult( CREATE_SEQUENCE( GetTypeManager(), collThis) );
		}
	};

	METHOD( TSequence_Prepend )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			collThis.insert( collThis.begin(), GetArgument( 0 ) );
			SetResult( CREATE_SEQUENCE( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TSequence_Append )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			collThis.push_back( GetArgument( 0 ) );
			SetResult( CREATE_SEQUENCE( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TSequence_SubSequence )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_INTEGER( lFrom, GetArgument( 0 ) );
			if ( lFrom < 0 ) {
				ThrowException( "Argument 'from' is less than 0." );
				return;
			}
			if ( lFrom >= (int) collThis.size() ) {
				ThrowException( "Argument 'from' equals to or is greater than size of Sequence." );
				return;
			}
			if ( GetArgumentCount() == 1 ) {
				OclMeta::ObjectVector collOut;
				for ( unsigned long i = lFrom ; i < collThis.size() ; i++ )
					collOut.push_back( collThis[ i ] );
				SetResult( CREATE_SEQUENCE( GetTypeManager(), collOut ) );
				return;
			}
			DECL_INTEGER( lTo, GetArgument( 1 ) );
			if ( lTo < lFrom ) {
				ThrowException( "Argument 'to' greater than Argument 'from'." );
				return;
			}
			if ( lTo >= (int) collThis.size() ) {
				ThrowException( "Argument 'to' equals to or is greater than size of Sequence." );
				return;
			}
			OclMeta::ObjectVector collOut;
			for ( long i = lFrom ; i <= lTo ; i++ )
				collOut.push_back( collThis[ i ] );
			SetResult( CREATE_SEQUENCE( GetTypeManager(), collOut ) );
		}
	};

	METHOD( TSequence_At )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_INTEGER( lAt, GetArgument( 0 ) );
			if ( lAt < 0 ) {
				ThrowException( "Argument 'from' is less than 0." );
				return;
			}
			if ( lAt >= (int) collThis.size() ) {
				ThrowException( "Argument 'from' equals to or is greater than size of Sequence." );
				return;
			}
			SetResult( collThis[ lAt ] );
		}
	};

	METHOD( TSequence_InsertAt )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_INTEGER( lAt, GetArgument( 0 ) );
			if ( lAt < 0 )
				lAt = 0;
			if ( lAt >= (int) collThis.size() ) {
				collThis.push_back( GetArgument( 1 ) );
				SetResult( CREATE_SEQUENCE( GetTypeManager(), collThis ) );
				return;
			}
			OclMeta::ObjectVector vecOut;
			for ( int i = 0 ; i < (int) collThis.size() ; i++ ) {
				if ( i == lAt )
					vecOut.push_back( GetArgument( 1 ) );
				vecOut.push_back( collThis[ i ] );
			}
			SetResult( CREATE_SEQUENCE( GetTypeManager(), vecOut ) );
		}
	};

	METHOD( TSequence_IndexOf )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			for ( int i = 0 ; i < (int) collThis.size() ; i ++ )
				if ( collThis[ i ] == GetArgument( 0 ) ) {
					SetResult( CREATE_INTEGER( GetTypeManager(), i ) );
					return;
				}
			SetResult( CREATE_INTEGER( GetTypeManager(), -1 ) );
		}
	};

	METHOD( TSequence_First )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			if ( collThis.empty() ) {
				ThrowException( "Sequence is empty." );
				return;
			}
			SetResult( collThis[ 0 ] );
		}
	};

	METHOD( TSequence_Last )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			if ( collThis.empty() ) {
				ThrowException( "Sequence is empty." );
				return;
			}
			SetResult( collThis[ collThis.size() - 1 ] );
		}
	};

	METHOD( TSequence_Including )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			collThis.push_back( GetArgument( 0 ) );
			SetResult( CREATE_SEQUENCE( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TSequence_Excluding )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			bool bFound = false;
			do {
				bFound = false;
				OclMeta::ObjectVector::iterator i = std::find( collThis.begin(), collThis.end(), GetArgument( 0 ) );
				if ( i  != collThis.end() ) {
					collThis.erase( i );
					bFound = true;
				}
			} while ( bFound );
			SetResult( CREATE_SEQUENCE( GetTypeManager(), collThis ) );
		}
	};

	ITERATOR( TSequence_Select )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			if ( ! GetSubResult().IsUndefined() ) {
				DECL_BOOLEAN( bArg, GetSubResult() );
				if ( bArg )
					m_vecSelecteds.push_back( GetArgument( 0 ) );
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_SEQUENCE( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	ITERATOR( TSequence_Reject )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			if ( ! GetSubResult().IsUndefined() ) {
				DECL_BOOLEAN( bArg, GetSubResult() );
				if ( ! bArg )
					m_vecSelecteds.push_back( GetArgument( 0 ) );
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_SEQUENCE( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	ITERATOR( TSequence_Collect )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			m_vecSelecteds.push_back( GetSubResult() );
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_SEQUENCE( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	void TSequence_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "union" ) && iCount == 1 ) {
			vecParams.push_back( FP( "sequence", "ocl::Sequence", true ) );
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_Union(), false ) );
			return;
		}

		if ( ( strName == "prepend" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_Prepend(), false ) );
			return;
		}

		if ( ( strName == "append" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_Append(), false ) );
			return;
		}

		if ( ( strName == "subSequence" ) && iCount == 2 ) {
			vecParams.push_back( FP( "pos", "ocl::Integer", true ) );
			vecParams.push_back( FP( "pos", "ocl::Integer", true ) );
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_SubSequence(), false ) );
			return;
		}

		if ( ( strName == "first" ) && iCount == 0 ) {
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_First(), false ) );
			return;
		}

		if ( ( strName == "last" ) && iCount == 0 ) {
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_Last(), false ) );
			return;
		}

		if ( ( strName == "at" ) && iCount == 1 ) {
			vecParams.push_back( FP( "pos", "ocl::Integer", true ) );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_At(), false ) );
			return;
		}

		if ( ( strName == "indexOf" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_IndexOf(), false ) );
			return;
		}

		if ( ( strName == "insertAt" ) && iCount == 2 ) {
			vecParams.push_back( FP( "pos", "ocl::Integer", true ) );
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_InsertAt(), false ) );
			return;
		}

		if ( ( strName == "including" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_Including(), false ) );
			return;
		}

		if ( ( strName == "excluding" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSequence_Excluding(), false ) );
			return;
		}
	}

	void TSequence_IteratorFactory::GetFeatures( const OclSignature::Iterator& signature, OclMeta::IteratorVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "select" ) {
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TSequence_Select(), false ) );
			return;
		}

		if ( strName == "reject" ) {
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TSequence_Reject(), false ) );
			return;
		}

		if ( strName == "collect" ) {
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_EXPRESSION_RETURN );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Any", vecType, new TSequence_Collect(), false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F  ocl::Bag
//
//##############################################################################################################################################

	OPERATOR( TBag_Plus )  // unio
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetArgument( 0 ) );
			DECL_ITERATOR( spIterator, GetArgument( 1 ) );
			while ( spIterator->HasNext() )
				collThis.push_back( spIterator->GetNext() );
			SetResult( CREATE_BAG( GetTypeManager(), collThis ) );
		}
	};

	OPERATOR( TBag_Times ) // intersection Bag
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetArgument( 0 ) );
			DECL_ITERATOR( spIterator, GetArgument( 1 ) );
			OclMeta::ObjectVector collOut;
			while ( spIterator->HasNext() ) {
				OclMeta::Object object = spIterator->GetNext();
				OclMeta::ObjectVector::iterator i = std::find( collThis.begin(), collThis.end(), object );
				if ( i != collThis.end() ) {
					collOut.push_back( object );
					collThis.erase( i );
				}
			}
			SetResult( CREATE_BAG( GetTypeManager(), collOut ) );
		}
	};

	OPERATOR( TBag_TimesSet ) // intersection Set
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetArgument( 0 ) );
			DECL_ITERATOR( spIterator, GetArgument( 1 ) );
			OclMeta::ObjectVector collOut;
			while ( spIterator->HasNext() ) {
				OclMeta::Object object = spIterator->GetNext();
				OclMeta::ObjectVector::iterator i = std::find( collThis.begin(), collThis.end(), object );
				if ( i != collThis.end() ) {
					collOut.push_back( object );
					collThis.erase( i );
				}
			}
			SetResult( CREATE_SET( GetTypeManager(), collOut ) );
		}
	};


	METHOD( TBag_Union )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_ITERATOR( spIterator, GetArgument( 0 ) );
			while ( spIterator->HasNext() )
				collThis.push_back( spIterator->GetNext() );
			SetResult( CREATE_BAG( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TBag_Intersection )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_ITERATOR( spIterator, GetArgument( 0 ) );
			OclMeta::ObjectVector collOut;
			while ( spIterator->HasNext() ) {
				OclMeta::Object object = spIterator->GetNext();
				OclMeta::ObjectVector::iterator i = std::find( collThis.begin(), collThis.end(), object );
				if ( i != collThis.end() ) {
					collOut.push_back( object );
					collThis.erase( i );
				}
			}
			SetResult( CREATE_BAG( GetTypeManager(), collOut ) );
		}
	};

	METHOD( TBag_IntersectionSet )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_ITERATOR( spIterator, GetArgument( 0 ) );
			OclMeta::ObjectVector collOut;
			while ( spIterator->HasNext() ) {
				OclMeta::Object object = spIterator->GetNext();
				OclMeta::ObjectVector::iterator i = std::find( collThis.begin(), collThis.end(), object );
				if ( i != collThis.end() ) {
					collOut.push_back( object );
					collThis.erase( i );
				}
			}
			SetResult( CREATE_SET( GetTypeManager(), collOut ) );
		}
	};

	METHOD( TBag_Including )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			collThis.push_back( GetArgument( 0 ) );
			SetResult( CREATE_BAG( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TBag_Excluding )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			bool bFound = false;
			do {
				bFound = false;
				OclMeta::ObjectVector::iterator i = std::find( collThis.begin(), collThis.end(), GetArgument( 0 ) );
				if ( i  != collThis.end() ) {
					collThis.erase( i );
					bFound = true;
				}
			} while ( bFound );
			SetResult( CREATE_BAG( GetTypeManager(), collThis ) );
		}
	};

	ITERATOR( TBag_Select )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			if ( ! GetSubResult().IsUndefined() ) {
				DECL_BOOLEAN( bArg, GetSubResult() );
				if ( bArg )
					m_vecSelecteds.push_back( GetArgument( 0 ) );
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_BAG( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	ITERATOR( TBag_Reject )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			if ( ! GetSubResult().IsUndefined() ) {
				DECL_BOOLEAN( bArg, GetSubResult() );
				if ( ! bArg )
					m_vecSelecteds.push_back( GetArgument( 0 ) );
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_BAG( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	ITERATOR( TBag_Collect )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			m_vecSelecteds.push_back( GetSubResult() );
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_BAG( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	void TBag_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "union" ) && iCount == 1 ) {
			vecParams.push_back( FP( "bag", "ocl::Bag", true ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TBag_Union(), false ) );

			vecParams.clear();
			vecType.clear();
			vecParams.push_back( FP( "set", "ocl::Set", true ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TBag_Union(), false ) );
			return;
		}

		if ( ( strName == "intersection" ) && iCount == 1 ) {
			vecParams.push_back( FP( "bag", "ocl::Bag", true ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TBag_Intersection(), false ) );

			vecParams.clear();
			vecType.clear();
			vecParams.push_back( FP( "set", "ocl::Set", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TBag_IntersectionSet(), false ) );
			return;
		}

		if ( ( strName == "including" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TBag_Including(), false ) );
			return;
		}

		if ( ( strName == "excluding" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TBag_Excluding(), false ) );
			return;
		}
	}

	void TBag_IteratorFactory::GetFeatures( const OclSignature::Iterator& signature, OclMeta::IteratorVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "select" ) {
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TBag_Select(), false ) );
			return;
		}

		if ( strName == "reject" ) {
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TBag_Reject(), false ) );
			return;
		}

		if ( strName == "collect" ) {
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_EXPRESSION_RETURN );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Any", vecType, new TBag_Collect(), false ) );
			return;
		}
	}


// --  OrderedSet inserted here
//##############################################################################################################################################
//
//	T Y P E   O F  ocl::OrderedSet
//
//##############################################################################################################################################

	METHOD( TOrderedSet_Prepend )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			collThis.insert( collThis.begin(), GetArgument( 0 ) );
			SetResult( CREATE_ORDEREDSET( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TOrderedSet_Append )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			collThis.push_back( GetArgument( 0 ) );
			SetResult( CREATE_ORDEREDSET( GetTypeManager(), collThis ) );
		}
	};

	METHOD( TOrderedSet_SubOrderedSet)
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_INTEGER( lFrom, GetArgument( 0 ) );
			if ( lFrom < 0 ) {
				ThrowException( "Argument 'from' is less than 0." );
				return;
			}
			if ( lFrom >= (int) collThis.size() ) {
				ThrowException( "Argument 'from' equals to or is greater than size of OrderedSet." );
				return;
			}
			if ( GetArgumentCount() == 1 ) {
				OclMeta::ObjectVector collOut;
				for ( unsigned long i = lFrom ; i < collThis.size() ; i++ )
					collOut.push_back( collThis[ i ] );
				SetResult( CREATE_ORDEREDSET( GetTypeManager(), collOut ) );
				return;
			}
			DECL_INTEGER( lTo, GetArgument( 1 ) );
			if ( lTo < lFrom ) {
				ThrowException( "Argument 'to' greater than Argument 'from'." );
				return;
			}
			if ( lTo >= (int) collThis.size() ) {
				ThrowException( "Argument 'to' equals to or is greater than size of OrderedSet." );
				return;
			}
			OclMeta::ObjectVector collOut;
			for ( long i = lFrom ; i <= lTo ; i++ )
				collOut.push_back( collThis[ i ] );
			SetResult( CREATE_ORDEREDSET( GetTypeManager(), collOut ) );
		}
	};

	METHOD( TOrderedSet_At )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_INTEGER( lAt, GetArgument( 0 ) );
			if ( lAt < 0 ) {
				ThrowException( "Argument 'from' is less than 0." );
				return;
			}
			if ( lAt >= (int) collThis.size() ) {
				ThrowException( "Argument 'from' equals to or is greater than size of OrederedSet." );
				return;
			}
			SetResult( collThis[ lAt ] );
		}
	};

	METHOD( TOrderedSet_InsertAt )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			DECL_INTEGER( lAt, GetArgument( 0 ) );
			if ( lAt < 0 )
				lAt = 0;
			if ( lAt >= (int) collThis.size() ) {
				collThis.push_back( GetArgument( 1 ) );
				SetResult( CREATE_ORDEREDSET( GetTypeManager(), collThis ) );
				return;
			}
			OclMeta::ObjectVector vecOut;
			for ( int i = 0 ; i < (int) collThis.size() ; i++ ) {
				if ( i == lAt )
					vecOut.push_back( GetArgument( 1 ) );
				vecOut.push_back( collThis[ i ] );
			}
			SetResult( CREATE_ORDEREDSET( GetTypeManager(), vecOut ) );
		}
	};

	METHOD( TOrderedSet_IndexOf )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			for ( int i = 0 ; i < (int) collThis.size() ; i ++ )
				if ( collThis[ i ] == GetArgument( 0 ) ) {
					SetResult( CREATE_INTEGER( GetTypeManager(), i ) );
					return;
				}
			SetResult( CREATE_INTEGER( GetTypeManager(), -1 ) );
		}
	};

	METHOD( TOrderedSet_First )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			if ( collThis.empty() ) {
				ThrowException( "Sequence is empty." );
				return;
			}
			SetResult( collThis[ 0 ] );
		}
	};

	METHOD( TOrderedSet_Last )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			if ( collThis.empty() ) {
				ThrowException( "Sequence is empty." );
				return;
			}
			SetResult( collThis[ collThis.size() - 1 ] );
		}
	};


	ITERATOR( TOrderedSet_Select )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			if ( ! GetSubResult().IsUndefined() ) {
				DECL_BOOLEAN( bArg, GetSubResult() );
				if ( bArg )
					m_vecSelecteds.push_back( GetArgument( 0 ) );
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_ORDEREDSET( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	ITERATOR( TOrderedSet_Reject )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			if ( ! GetSubResult().IsUndefined() ) {
				DECL_BOOLEAN( bArg, GetSubResult() );
				if ( ! bArg )
					m_vecSelecteds.push_back( GetArgument( 0 ) );
			}
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_ORDEREDSET( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	ITERATOR( TOrderedSet_Collect )
	{
		private :
			OclMeta::ObjectVector m_vecSelecteds;

		void Initialize()
		{
			OclIterator::Initialize();
			m_vecSelecteds.clear();
		}

		void operator()()
		{
			m_vecSelecteds.push_back( GetSubResult() );
		}

		OclMeta::Object GetResult() const
		{
			return CREATE_BAG( GetTypeManager(), m_vecSelecteds );
		}

		void Finalize()
		{
			m_vecSelecteds.clear();
			OclIterator::Finalize();
		}
	};

	void TOrderedSet_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "prepend" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::OrderedSet" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TOrderedSet_Prepend(), false ) );
			return;
		}

		if ( ( strName == "append" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::OrderedSet" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TOrderedSet_Append(), false ) );
			return;
		}

		if ( ( strName == "subOrderedSet" ) && iCount == 2 ) {
			vecParams.push_back( FP( "pos", "ocl::Integer", true ) );
			vecParams.push_back( FP( "pos", "ocl::Integer", true ) );
			vecType.push_back( "ocl::OrderedSet" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TOrderedSet_SubOrderedSet(), false ) );
			return;
		}

		if ( ( strName == "first" ) && iCount == 0 ) {
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TOrderedSet_First(), false ) );
			return;
		}

		if ( ( strName == "last" ) && iCount == 0 ) {
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TOrderedSet_Last(), false ) );
			return;
		}

		if ( ( strName == "at" ) && iCount == 1 ) {
			vecParams.push_back( FP( "pos", "ocl::Integer", true ) );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TOrderedSet_At(), false ) );
			return;
		}

		if ( ( strName == "indexOf" ) && iCount == 1 ) {
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TOrderedSet_IndexOf(), false ) );
			return;
		}

		if ( ( strName == "insertAt" ) && iCount == 2 ) {
			vecParams.push_back( FP( "pos", "ocl::Integer", true ) );
			vecParams.push_back( FP( "any", "ocl::Any", true ) );
			vecType.push_back( "ocl::OrderedSet" );
			vecType.push_back( TYPE_ARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TOrderedSet_InsertAt(), false ) );
			return;
		}

	}

	void TOrderedSet_IteratorFactory::GetFeatures( const OclSignature::Iterator& signature, OclMeta::IteratorVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "select" ) {
			vecType.push_back( "ocl::OrderedSet" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TOrderedSet_Select(), false ) );
			return;
		}

		if ( strName == "reject" ) {
			vecType.push_back( "ocl::OrderedSet" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Boolean", vecType, new TOrderedSet_Reject(), false ) );
			return;
		}

		if ( strName == "collect" ) {
			vecType.push_back( "ocl::OrderedSet" );
			vecType.push_back( TYPE_EXPRESSION_RETURN );
			vecFeatures.push_back( new OclMeta::Iterator( strName, "ocl::Any", vecType, new TOrderedSet_Collect(), false ) );
			return;
		}
	}



//##############################################################################################################################################
//
//	G L O B A L   F A C T O R I E S
//
//##############################################################################################################################################

	void OperatorFactory::GetFeatures( const OclSignature::Operator& signature, OclMeta::OperatorVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();

		// ocl::Any

		if ( ( strName == "=" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Any", "ocl::Any", vecType, new TAny_Equals(), false ) );
		}

		if ( ( strName == "<>" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Any", "ocl::Any", vecType, new TAny_NotEquals(), false ) );
		}

		if ( ( strName == "==" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Any", "ocl::Any", vecType, new TAny_IdentityEquals(), false ) );
		}

		if ( ( strName == "!=" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Any", "ocl::Any", vecType, new TAny_IdentityNotEquals(), false ) );
		}

		// ocl::Boolean

		if ( ( strName == "not" ) && iCount == 1 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Boolean", vecType, new TBoolean_Not(), false ) );
		}
		if ( ( strName == "and" || strName == "&&" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Boolean", "ocl::Boolean", vecType, new TBoolean_And(), false ) );
		}

		if ( ( strName == "or" || strName == "||" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Boolean", "ocl::Boolean", vecType, new TBoolean_Or(), false ) );
		}

		if ( ( strName == "xor" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Boolean", "ocl::Boolean", vecType, new TBoolean_Xor(), false ) );
		}

		if ( ( strName == "implies" || strName == "=>" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Boolean", "ocl::Boolean", vecType, new TBoolean_Implies(), false ) );
		}

		// ocl::String

		if ( ( strName == "<" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::String", "ocl::String", vecType, new TString_Less(), false ) );
		}

		if ( ( strName == "<=" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::String", "ocl::String", vecType, new TString_LessEquals(), false ) );
		}

		if ( ( strName == ">" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::String", "ocl::String", vecType, new TString_Greater(), false ) );
		}

		if ( ( strName == ">=" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::String", "ocl::String", vecType, new TString_GreaterEquals(), false ) );
		}

		if ( ( strName == "+" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::String", "ocl::String", vecType, new TString_Plus(), false ) );
		}

		// ocl::Enumeration

		// ocl::Real

		if ( ( strName == "-" ) && iCount == 1 ) {
			TS vecType;
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Real", vecType, new TReal_Minus1(), false ) );
		}

		if ( ( strName == "-" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Real", "ocl::Real", vecType, new TReal_Minus2(), false ) );
		}

		if ( ( strName == "+" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Real", "ocl::Real", vecType, new TReal_Plus(), false ) );
		}

		if ( ( strName == "*" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Real", "ocl::Real", vecType, new TReal_Times(), false ) );
		}

		if ( ( strName == "/" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Real", "ocl::Real", vecType, new TReal_Slash(), false ) );
		}

		if ( ( strName == "<" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Real", "ocl::Real", vecType, new TReal_Less(), false ) );
		}

		if ( ( strName == "<=" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Real", "ocl::Real", vecType, new TReal_LessEquals(), false ) );
		}

		if ( ( strName == ">" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Real", "ocl::Real", vecType, new TReal_Greater(), false ) );
		}

		if ( ( strName == ">=" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Real", "ocl::Real", vecType, new TReal_GreaterEquals(), false ) );
		}

		// ocl::Integer

		if ( ( strName == "-" ) && iCount == 1 ) {
			TS vecType;
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Integer", vecType, new TInteger_Minus1(), false ) );
		}

		if ( ( strName == "-" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Integer", "ocl::Integer", vecType, new TInteger_Minus2(), false ) );
		}

		if ( ( strName == "+" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Integer", "ocl::Integer", vecType, new TInteger_Plus(), false ) );
		}

		if ( ( strName == "*" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Integer", "ocl::Integer", vecType, new TInteger_Times(), false ) );
		}

		if ( ( strName == "div" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Integer", "ocl::Integer", vecType, new TInteger_Div(), false ) );
		}

		if ( ( strName == "mod" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Integer", "ocl::Integer", vecType, new TInteger_Mod(), false ) );
		}

		if ( ( strName == "<" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Integer", "ocl::Integer", vecType, new TInteger_Less(), false ) );
		}

		if ( ( strName == "<=" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Integer", "ocl::Integer", vecType, new TInteger_LessEquals(), false ) );
		}

		if ( ( strName == ">" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Integer", "ocl::Integer", vecType, new TInteger_Greater(), false ) );
		}

		if ( ( strName == ">=" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Integer", "ocl::Integer", vecType, new TInteger_GreaterEquals(), false ) );
		}

		// ocl::Type

		// ocl::Set

		if ( ( strName == "+" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Set", "ocl::Set", vecType, new TSet_Plus(), false ) );
		}

		if ( ( strName == "+" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Set", "ocl::Bag", vecType, new TSet_PlusBag(), false ) );
		}

		if ( ( strName == "-" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Set", "ocl::Set", vecType, new TSet_Minus(), false ) );
		}

		if ( ( strName == "*" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Set", "ocl::Set", vecType, new TSet_Times(), false ) );
		}

		if ( ( strName == "*" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Set", "ocl::Bag", vecType, new TSet_Times(), false ) );
		}

		if ( ( strName == "%" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Set", "ocl::Set", vecType, new TSet_Percent(), false ) );
		}

		// ocl::Sequence

		if ( ( strName == "+" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Sequence", "ocl::Sequence", vecType, new TSequence_Plus(), false ) );
		}

		// ocl::Bag

		if ( ( strName == "+" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Bag", "ocl::Bag", vecType, new TBag_Plus(), false ) );
		}

		if ( ( strName == "+" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_COMPOUNDARGUMENT_SELF_BASE );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Bag", "ocl::Set", vecType, new TBag_Plus(), false ) );
		}

		if ( ( strName == "*" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Bag", "ocl::Bag", vecType, new TBag_Times(), false ) );
		}

		if ( ( strName == "*" ) && iCount == 2 ) {
			TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Operator( strName, "ocl::Bag", "ocl::Set", vecType, new TBag_TimesSet(), false ) );
		}
	}

	void FunctionFactory::GetFeatures( const OclSignature::Function& signature, OclMeta::FunctionVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();

		// ocl::Any

		// ocl::Boolean

		// ocl::String

		// ocl::Enumeration

		// ocl::Real

		if ( ( strName == "abs" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "real", "ocl::Real", true ) );
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Function( strName, vecParams, vecType, new TReal_Abs(), false ) );
		}

		if ( ( strName == "floor" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "real", "ocl::Real", true ) );
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Function( strName, vecParams, vecType, new TReal_Floor(), false ) );
		}

		if ( ( strName == "round" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "real", "ocl::Real", true ) );
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Function( strName, vecParams, vecType, new TReal_Round(), false ) );
		}

		if ( ( strName == "min" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "real1", "ocl::Real", true ) );
			vecParams.push_back( FP( "real2", "ocl::Real", true ) );
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Function( strName, vecParams, vecType, new TReal_Min(), false ) );
		}

		if ( ( strName == "max" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "real1", "ocl::Real", true ) );
			vecParams.push_back( FP( "real2", "ocl::Real", true ) );
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Function( strName, vecParams, vecType, new TReal_Max(), false ) );
		}

		// ocl::Integer

		if ( ( strName == "abs" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "integer", "ocl::Integer", true ) );
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Function( strName, vecParams, vecType, new TInteger_Abs(), false ) );
		}

		if ( ( strName == "min" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "integer1", "ocl::Integer", true ) );
			vecParams.push_back( FP( "integer2", "ocl::Integer", true ) );
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Function( strName, vecParams, vecType, new TInteger_Min(), false ) );
		}

		if ( ( strName == "max" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "integer1", "ocl::Integer", true ) );
			vecParams.push_back( FP( "integer2", "ocl::Integer", true ) );
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Function( strName, vecParams, vecType, new TInteger_Max(), false ) );
		}

		// ocl::Type

	}

	void TypeFactory::GetTypes( const std::string& strName, const std::string& , std::vector<std::unique_ptr<OclMeta::Type>>& vecTypes, std::string& strNameResult )
	{
		strNameResult = strName;
		if ( strName == "ocl::Any" || strName == "Any" ) {
			StringVector vecSupers;
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Any", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TAny_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Boolean" || strName == "bool" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Boolean", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclImplementation::MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Enumeration" || strName == "enum" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Enumeration", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TEnumeration_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::String" || strName == "string" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::String", vecSupers, new TString_AttributeFactory(), new OclImplementation::AssociationFactory(), new TString_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Real" || strName == "real" || strName == "double" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Real", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TReal_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Integer" || strName == "int" || strName == "long" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Real" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Integer", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TInteger_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Type" || strName == "Type" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Type", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclImplementation::MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Collection" || strName == "Collection" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Collection", vecSupers, new TCollection_AttributeFactory(), new OclImplementation::AssociationFactory(), new TCollection_MethodFactory(),new TCollection_IteratorFactory(),  false ) ));
			return;
		}

		if ( strName == "ocl::Set" || strName == "Set" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Collection" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Set", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TSet_MethodFactory(),new TSet_IteratorFactory(),  false ) ));
			return;
		}

		if ( strName == "ocl::Sequence" || strName == "Sequence" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Collection" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Sequence", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TSequence_MethodFactory(),new TSequence_IteratorFactory(),  false ) ));
			return;
		}

		if ( strName == "ocl::Bag" || strName == "Bag" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Collection" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Bag", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TBag_MethodFactory(),new TBag_IteratorFactory(),  false ) ));
			return;
		}
	}

}; // namespace OclBasic