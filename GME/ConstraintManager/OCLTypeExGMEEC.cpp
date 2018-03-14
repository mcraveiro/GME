//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLTypeExGMEEC.cpp
//
//###############################################################################################################################################
#include "Solve4786.h"
#include "OCLTypeExGMEEC.h"
#include "OCLObjectExBasic.h"

namespace OclGmeEC
{
	typedef OclCommon::FormalParameterVector 	FPV;
	typedef OclCommon::FormalParameter			FP;
	typedef TypeSeq									TS;

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::String
//
//##############################################################################################################################################

	void TStringEx_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		( ( OclBasic::TString_MethodFactory ) *this ).GetFeatures( signature, vecFeatures );

		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		TS vecType;
		FPV vecParams;

		if ( ( strName == "intValue" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "doubleValue" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType,  NULL, false ) );
			return;
		}

		if ( ( strName == "size" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "match" && iCount == 1 ) ) {
			vecParams.push_back( FP( "regExp", "ocl::String", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Collection
//
//##############################################################################################################################################

	void TCollectionEx_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		( ( OclBasic::TCollection_MethodFactory ) *this ).GetFeatures( signature, vecFeatures );

		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		TS vecType;
		FPV vecParams;

		if ( ( strName == "size" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "theOnly" ) && iCount == 0 ) {
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Project
//
//##############################################################################################################################################

	void TProject_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "name" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, NULL, false ) );
		}
	}

	void TProject_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( strName == "allInstancesOf" && iCount == 1 ) {
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Object" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( strName == "rootFolder" && iCount == 0 ) {
			vecType.push_back( "meta::RootFolder" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Object
//
//##############################################################################################################################################

	void TObject_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "name" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, NULL, false ) );
			return;
		}

		if ( strName == "kindName" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, NULL, false ) );
			return;
		}

		if ( strName == "metaKindName" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, NULL, false ) );
			return;
		}
	}

	void TObject_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams; TS vecType;

		if ( ( strName == "name" ) && iCount == 0 ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "kindName" ) && iCount == 0 ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "isNull" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "parent" ) && iCount == 0 ) {
			vecType.push_back( "gme::Object" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "isFCO" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "isFolder" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Folder
//
//##############################################################################################################################################

	void TFolder_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();

		if ( ( strName == "folders" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Folder" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "childFolders" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Folder" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "rootDescendants" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "allDescendants" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "rootChildren" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "models" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "models" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "atoms" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "atoms" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::FCO
//
//##############################################################################################################################################

	void TFCO_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "roleName" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, NULL, false ) );
			return;
		}
	}

	void TFCO_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		if ( ( strName == "roleName" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "subTypes" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "instances" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "type" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "baseType" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "isInstance" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "isType" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "folder" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "gme::Folder" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "referencedBy" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Reference" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "referencedBy" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Reference" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "memberOfSets" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Set" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "memberOfSets" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Set" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "connectedFCOs" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "connectedFCOs" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "connectedFCOs" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "connected" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "bagConnectedFCOs" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "bagConnectedFCOs" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "reverseConnectedFCOs" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "reverseConnectedFCOs" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "reverseConnectedFCOs" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "connectedAs" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "bagReverseConnectedFCOs" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "bagReverseConnectedFCOs" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "attachingConnPoints" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "attachingConnPoints" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "attachingConnPoints" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "attachingConnections" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "attachingConnections" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "attachingConnections" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "isConnectedTo" ) && iCount == 3 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "fco", "gme::FCO", true ) );
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "isConnectedTo" ) && iCount <= 3 && iCount >= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "fco", "gme::FCO", true ) );
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "isConnectedTo" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "fco", "gme::FCO", true ) );
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "attribute" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "name", "ocl::String", true ) );
			vecType.push_back( "ocl::Any" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}
//----
		if ( ( strName == "attrByDefault" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "name", "ocl::String", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}
//----
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Connection
//
//##############################################################################################################################################

	void TConnection_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "connectionPoints" ) && iCount <= 1 ) {
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "connectionPoint" ) && iCount == 1 ) {
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Reference
//
//##############################################################################################################################################

	void TReference_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();

		if ( ( strName == "usedByConnPoints" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "usedByConnPoints" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "refersTo" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Model
//
//##############################################################################################################################################

	void TModel_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();

		if ( ( strName == "models" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "models" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "atoms" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "atoms" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "parts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "modelParts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "atomParts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "connectionParts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "referenceParts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Reference" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "setParts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Set" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "parts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "modelParts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "atomParts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "connectionParts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "referenceParts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Reference" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}

		if ( ( strName == "setParts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Set" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
		}
	}

//##############################################################################################################################################
//
//	T Y P E    O F   gme::Atom
//
//##############################################################################################################################################

//##############################################################################################################################################
//
//	T Y P E    O F   gme::Set
//
//##############################################################################################################################################

	void TSet_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "members" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E    O F   gme::ConnectionPoint
//
//##############################################################################################################################################

	void TConnectionPoint_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "cpRoleName" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, NULL, false ) );
			return;
		}
	}

	void TConnectionPoint_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "cpRoleName" ) && iCount == 0 ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "owner" ) && iCount == 0 ) {
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "target" ) && iCount == 0 ) {
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "peer" ) && iCount == 0 ) {
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}

		if ( ( strName == "usedReferences" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, NULL, false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   D E R I V E D   O F   gme::FCO
//
//##############################################################################################################################################

	TFCODerived_AttributeFactory::TFCODerived_AttributeFactory( CComPtr<IMgaFCO> spFCO )
		: m_spFCO( spFCO )
	{
	}

	void TFCODerived_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		OclCommonEx::FCOVector vecExamined;
		GetAttributeObjects( signature.GetName(), m_spFCO, vecFeatures, vecExamined );
	}

	void TFCODerived_AttributeFactory::GetAttributeObjects( const std::string& strName, CComPtr<IMgaFCO> spFCO, OclMeta::AttributeVector& vecFeatures, OclCommonEx::FCOVector& vecExamined )
	{
		if ( OclCommonEx::ContainsObject( spFCO, vecExamined ) )
			return;
		vecExamined.push_back( spFCO.p );

		OclCommonEx::FCOVector vecAttributes;
		OclCommonEx::GetAssociationEnds( spFCO, "dst", "HasAttribute", vecAttributes );
		for ( unsigned int i = 0 ; i < vecAttributes.size() ; i++ ) {
			if ( strName == OclCommonEx::GetObjectName( vecAttributes[ i ].p ) ) {
				std::string strKind = OclCommonEx::GetObjectKind( vecAttributes[ i ].p );
				std::string strReturnType;
				if ( strKind == "EnumAttribute" )
					strReturnType = "ocl::Enumeration";
				else
					if ( strKind == "BooleanAttribute" )
						strReturnType = "ocl::Boolean";
					else {
						CComVariant objAType;
						COMTHROW( vecAttributes[ i ]->get_AttributeByName( CComBSTR( "DataType" ), PutOut( objAType ) ) );
						CString strFieldType = objAType.bstrVal;
						if ( strFieldType == "integer" )
							strReturnType = "ocl::Integer";
						else
							if ( strFieldType == "double" )
								strReturnType = "ocl::Real";
							else
								strReturnType = "ocl::String";
					}
				vecFeatures.push_back( new OclMeta::Attribute( strName, TS( 1, strReturnType ), NULL, true ) );
			}
		}

		OclCommonEx::FCOVector vecParents;
		OclCommonEx::GetInheritances( spFCO, "Normal", true, vecParents );
		OclCommonEx::GetInheritances( spFCO, "Implementation", true, vecParents );
		for ( unsigned int i = 0 ; i < vecParents.size() ; i++ )
			GetAttributeObjects( strName, vecParents[ i ].p, vecFeatures, vecExamined );

		CComPtr<IMgaFCOs> spRefs;
		if ( OclCommonEx::GetAllObjects( spFCO, spRefs ) && spRefs.p ) {
			MGACOLL_ITERATE( IMgaFCO, spRefs ) {
				GetAttributeObjects( strName, MGACOLL_ITER, vecFeatures, vecExamined );
			} MGACOLL_ITERATE_END;
		}
		GetAttributeObjects( strName, spFCO, vecFeatures, vecExamined );
	}

//##############################################################################################################################################
//
//	T Y P E   F A C T O R Y
//
//##############################################################################################################################################

	TypeFactory::TypeFactory( CComPtr<IMgaProject> spProject )
		: m_spProject( spProject )
	{
	}

	void TypeFactory::GetTypes( const std::string& strName, const std::string& strNSpace, std::vector<std::unique_ptr<OclMeta::Type>>& vecTypes, std::string& strNameResult )
	{
		strNameResult = strName;
		bool bHasNamespace = strName.find( "::" ) != std::string::npos;

		GetDynamicTypes( strName, vecTypes, strNameResult);
		if ( ! bHasNamespace && ! vecTypes.empty() )
			return;

		GetStereotypes( strName, vecTypes );
		if ( ! bHasNamespace && ! vecTypes.empty() )
			return;

		if ( strName == "ocl::Any" || strName == "Any" ) {
			StringVector vecSupers;
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Any", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TAny_MethodFactory(), false ) ));
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
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Enumeration", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TEnumeration_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::String" || strName == "string" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::String", vecSupers, new OclBasic::TString_AttributeFactory(), new OclImplementation::AssociationFactory(), new TStringEx_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Real" || strName == "real" || strName == "double" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Real", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TReal_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Integer" || strName == "int" || strName == "long" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Real" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Integer", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TInteger_MethodFactory(), false ) ));
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
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Collection", vecSupers, new OclBasic::TCollection_AttributeFactory(), new OclImplementation::AssociationFactory(), new TCollectionEx_MethodFactory(),new OclBasic::TCollection_IteratorFactory(),  false ) ));
			return;
		}

		if ( strName == "ocl::Set" || strName == "Set" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Collection" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Set", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TSet_MethodFactory,new OclBasic::TSet_IteratorFactory(),  false ) ));
			return;
		}

		if ( strName == "ocl::Sequence" || strName == "Sequence" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Collection" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Sequence", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TSequence_MethodFactory(),new OclBasic::TSequence_IteratorFactory(),  false ) ));
			return;
		}

		if ( strName == "ocl::Bag" || strName == "Bag" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Collection" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Bag", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TBag_MethodFactory(),new OclBasic::TBag_IteratorFactory(),  false ) ));
			return;
		}

// -- ??
		if ( strName == "ocl::OrderedSet" || strName == "OrderedSet" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Set" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::OrderedSet", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TOrderedSet_MethodFactory(),new OclBasic::TOrderedSet_IteratorFactory(),  false ) ));
			return;
		}
// --
	}

	void TypeFactory::GetStereotypes( const std::string& strName, std::vector<std::unique_ptr<OclMeta::Type>>& vecTypes )
	{
		if ( strName == "gme::Object" || strName == "Object" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Object", vecSupers, new TObject_AttributeFactory(), new OclImplementation::AssociationFactory(), new TObject_MethodFactory(), true ) ));
			return;
		}

		if ( strName == "gme::Folder" || strName == "Folder" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::Object" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Folder", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TFolder_MethodFactory(), true ) ));
			return;
		}

		if ( strName == "gme::FCO" || strName == "FCO" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::Object" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::FCO", vecSupers, new TFCO_AttributeFactory(), new OclImplementation::AssociationFactory(), new TFCO_MethodFactory(), true ) ));
			return;
		}

		if ( strName == "gme::Model" || strName == "Model" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::FCO" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Model", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TModel_MethodFactory(), true ) ));
			return;
		}

		if ( strName == "gme::Atom" || strName == "Atom" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::FCO" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Atom", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclImplementation::MethodFactory(), true ) ));
			return;
		}

		if ( strName == "gme::Set" || strName == "Set" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::FCO" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Set", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TSet_MethodFactory(), true ) ));
			return;
		}

		if ( strName == "gme::Reference" || strName == "Reference" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::FCO" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Reference", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TReference_MethodFactory(), true ) ));
			return;
		}

		if ( strName == "gme::Connection" || strName == "Connection" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::FCO" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Connection", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TConnection_MethodFactory(), true ) ));
			return;
		}

		if ( strName == "gme::Project" || strName == "Project" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Project", vecSupers, new TProject_AttributeFactory(), new OclImplementation::AssociationFactory(), new TProject_MethodFactory(), true ) ));
			return;
		}

		if ( strName == "gme::ConnectionPoint" || strName == "ConnPoint" || strName == "ConnectionPoint" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::ConnectionPoint", vecSupers, new TConnectionPoint_AttributeFactory(), new OclImplementation::AssociationFactory(), new TConnectionPoint_MethodFactory(), true ) ));
			return;
		}

		if ( strName == "meta::RootFolder" || strName == "RootFolder" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::Folder" );
			vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "meta::RootFolder", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclImplementation::MethodFactory(), true ) ));
			return;
		}
	}

	void TypeFactory::GetDynamicTypes( const std::string& strName, std::vector<std::unique_ptr<OclMeta::Type>>& vecTypes, std::string& )
	{
		std::string strRealName = strName;
		if ( strName.size() > 6 && strName.substr( 0, 6 ) == "meta::" )
			strRealName = strName.substr( 6 );



		CComPtr<IMgaFilter> spFilter;
		COMTHROW( m_spProject->CreateFilter( &spFilter ) );
		COMTHROW( spFilter->put_Name( CComBSTR( OclCommonEx::Convert( strRealName ) ) ) );
		COMTHROW( spFilter->put_ObjType( CComBSTR( "OBJTYPE_ATOM" ) ) );

		CComPtr<IMgaFCOs> spFCOs;
		COMTHROW( m_spProject->AllFCOs( spFilter, &spFCOs ) );
		MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
			std::string strStereotype = OclCommonEx::GetObjectKind( MGACOLL_ITER.p );
			if ( strStereotype == "Atom" || strStereotype == "Model" || strStereotype == "Connection" || strStereotype == "Reference" || strStereotype == "Set" || strStereotype == "Folder" || strStereotype == "FCO" ) {

				if ( strRealName == OclCommonEx::GetObjectName( MGACOLL_ITER.p ) ) {
					if ( OclCommonEx::IsAbstract( MGACOLL_ITER ) )
						throw OclCommon::Exception( OclCommon::Exception::ET_SEMANTIC, "Type [ ? ] is not available in Constraints or ConstraintFunctions because it is abstract.", strName );

					OclImplementation::AttributeFactory* pAttFactory = NULL;
					OclImplementation::AssociationFactory* pAssFactory = new OclImplementation::AssociationFactory();
					std::string strSupertype;

					if ( strStereotype == "Atom" ) {
						strSupertype = "gme::Atom";
						pAttFactory = new TFCODerived_AttributeFactory( MGACOLL_ITER );
					}
					else if ( strStereotype == "Model" ) {
						strSupertype = "gme::Model";
						pAttFactory = new TFCODerived_AttributeFactory( MGACOLL_ITER );
					}
					else if ( strStereotype == "Connection" ) {
						strSupertype = "gme::Connection";
						pAttFactory = new TFCODerived_AttributeFactory( MGACOLL_ITER );
					}
					else if ( strStereotype == "Set" ) {
						strSupertype = "gme::Set";
						pAttFactory = new TFCODerived_AttributeFactory( MGACOLL_ITER );
					}
					else if ( strStereotype == "Reference" ) {
						strSupertype = "gme::Reference";
						pAttFactory = new TFCODerived_AttributeFactory( MGACOLL_ITER );
					}
					else if ( strStereotype == "Folder" ) {
						strSupertype = "gme::Folder";
						pAttFactory = new OclImplementation::AttributeFactory();
					}

					vecTypes.push_back(std::unique_ptr<OclMeta::Type>( new OclMeta::Type( "meta::" + strRealName, StringVector( 1, strSupertype ), pAttFactory, pAssFactory, new OclImplementation::MethodFactory(), true ) ));
				}
			}
		} MGACOLL_ITERATE_END;
	}

}; // namespace OclGmeCM