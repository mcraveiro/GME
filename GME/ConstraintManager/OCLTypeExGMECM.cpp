//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLTypeExGMECM.cpp
//
//###############################################################################################################################################
#include "Solve4786.h"
#include "OCLTypeExGMECM.h"

#include "OCLObjectExBasic.h"
#include "OCLObjectExGME.h"
#include "OCLCommonEx.h"
#include "math.h"
#include "mga.h"
#include <regex>

using std::unique_ptr;

#define NILNAMESPACE ""

namespace OclGmeCM
{
	typedef OclCommon::FormalParameterVector 	FPV;
	typedef OclCommon::FormalParameter			FP;
	typedef TypeSeq									TS;

	class Method
		: public OclImplementation::Method
	{
		protected :
			std::string GetKind( int iArgumentPos, const std::string& strMetaKind, bool bCanBeEmpty = false )
			{
				std::string strKind;
				if ( iArgumentPos != -1 && GetArgumentCount() > iArgumentPos ) {
					if ( GetArgument( iArgumentPos ).GetTypeName() == "ocl::String" ) {
						DECL_STRING2( strKind, GetArgument( iArgumentPos ) );
					}
					else {
						DECL_TYPE2( strKind, GetArgument( iArgumentPos ) );
					}
					if ( ! bCanBeEmpty && strKind.empty() )
						ThrowException( "Kind cannot be empty string." );
					if ( GetTypeManager()->IsTypeA( strKind, "gme::Object" ) <= 0 )
						ThrowException( "Kind does not exists." );
					if ( ! ( strMetaKind == "gme::Object" ) )
						if ( GetTypeManager()->IsTypeA( strKind, strMetaKind ) <= 0 )
							ThrowException( "Kind does not conform to " + strMetaKind + "." );
					return GetTypeManager()->GetType( strKind, NILNAMESPACE )->GetName().substr( 6 );
				}
				return strKind;
			}
	};

	#define GMEMETHOD( featureName )			class featureName : public Method

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::String
//
//##############################################################################################################################################

	METHOD( TStringEx_IntValue_Compatibility  )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetThis() );
			Trim( strThis );
			for ( unsigned int i = 0 ; i < strThis.length() ; i ++ )
				if ( strThis[ i ] < '0' || strThis[ i ] > '9' )
					ThrowException( "String cannot be converted to ocl::Integer." );
			long lValue = atol( strThis.c_str() );
			SetResult( CREATE_INTEGER( GetTypeManager(), lValue ) );
		}
	};

	METHOD( TStringEx_DoubleValue_Compatibility  )
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

	METHOD( TStringEx_Size_Compatibility )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetThis() );
			SetResult( CREATE_INTEGER( GetTypeManager(), strThis.length() ) );
		}
	};

	METHOD( TStringEx_Match )
	{
		void operator()()
		{
			DECL_STRING( strThis, GetThis() );
			DECL_STRING( strRegExp, GetArgument( 0 ) );
			CString strRegExp2 = OclCommonEx::Convert( strRegExp );
			try {
				std::wregex tester( strRegExp2 );

				SetResult( CREATE_BOOLEAN( GetTypeManager(), std::regex_search(static_cast<const TCHAR*>(OclCommonEx::Convert(strThis)), tester) ) );
			} catch (std::regex_error&) {
				ThrowException( "Regular Expression is not valid!" );
			}
		}
	};

	void TStringEx_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		((OclBasic::TString_MethodFactory) *this ).GetFeatures( signature, vecFeatures );

		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		TS vecType;
		FPV vecParams;

		if ( ( strName == "intValue" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TStringEx_IntValue_Compatibility(), false ) );
			return;
		}

		if ( ( strName == "doubleValue" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Real" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TStringEx_DoubleValue_Compatibility(), false ) );
			return;
		}

		if ( ( strName == "size" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TStringEx_Size_Compatibility(), false ) );
			return;
		}

		if ( ( strName == "match" && iCount == 1 ) ) {
			vecParams.push_back( FP( "regExp", "ocl::String", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TStringEx_Match(), false ) );
		}
	};

//##############################################################################################################################################
//
//	T Y P E   O F   ocl::Collection
//
//##############################################################################################################################################

	METHOD( TCollectionEx_TheOnly_Compatibility )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			if ( collThis.size() == 0 ) {
				ThrowException( "Collection does not contain any object." );
				return;
			}
			if ( collThis.size() > 1 )
				ThrowException( "Collection contains more than one object." );
			SetResult( collThis[ 0 ] );
		}
	};

	METHOD( TCollectionEx_Size_Compatibility )
	{
		void operator()()
		{
			DECL_COLLECTION( collThis, GetThis() );
			SetResult( CREATE_INTEGER( GetTypeManager(), collThis.size() ) );
		}
	};

	void TCollectionEx_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		((OclBasic::TCollection_MethodFactory) *this ).GetFeatures( signature, vecFeatures );

		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		TS vecType;
		FPV vecParams;

		if ( ( strName == "size" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Integer" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollectionEx_Size_Compatibility(), false ) );
			return;
		}

		if ( ( strName == "theOnly" ) && iCount == 0 ) {
			vecType.push_back( TYPE_AGGREGATED_OBJECT );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TCollectionEx_TheOnly_Compatibility(), false ) );
			return;
		}
	};

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Project
//
//##############################################################################################################################################

	ATTRIBUTE( TProject_Name )
	{
		void operator()()
		{
			DECL_GMEPROJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			CString strName;
			COMTHROW( spThis->get_Name( PutOut( strName ) ) );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::Convert( strName ) ) );
		}
	};

	GMEMETHOD( TProject_AllInstancesOf )
	{
		void operator()()
		{
			DECL_GMEPROJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			std::string strKind = GetKind( 0, "gme::Object" );

			OclCommonEx::ObjectVector vecObjects;
			OclCommonEx::GetKindObjects( spThis, strKind, vecObjects );

			OclMeta::ObjectVector setOut;
			for ( unsigned int i = 0 ; i < vecObjects.size() ; i++ )
				setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), vecObjects[ i ].p ) );

			SetResult( CREATE_SET( GetTypeManager(), setOut ) );
		}
	};

	GMEMETHOD( TProject_RootFolder )
	{
		void operator()()
		{
			DECL_GMEPROJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			CComPtr<IMgaFolder> spFolder;
			COMTHROW( spThis->get_RootFolder( &spFolder ) );
			SetResult( CREATE_GMEOBJECT( GetTypeManager(), spFolder ) );
		}
	};

	void TProject_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "name" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, new TProject_Name(), false ) );
			return;
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
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TProject_AllInstancesOf(), false ) );
		}

		if ( strName == "rootFolder" && iCount == 0 ) {
			vecType.push_back( "meta::RootFolder" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TProject_RootFolder(), false ) );
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Object
//
//##############################################################################################################################################

	ATTRIBUTE( TObject_Name )
	{
		void operator()()
		{
			DECL_GMEOBJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::GetObjectName( spThis ) ) );
		}
	};

	ATTRIBUTE( TObject_KindName )
	{
		void operator()()
		{
			DECL_GMEOBJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::GetObjectKind( spThis ) ) );
		}
	};

	ATTRIBUTE( TObject_KindDisplayedName )
	{
		void operator()()
		{
			DECL_GMEOBJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::GetObjectDisplayedName( spThis ) ) );
		}
	};

	ATTRIBUTE( TObject_StereotypeName )
	{
		void operator()()
		{
			DECL_GMEOBJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			objtype_enum eType = OclCommonEx::GetObjectType ( spThis );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::ObjectTypeToString( eType ) ) );
		}
	};

	GMEMETHOD( TObject_Name_Compatibility )
	{
		void operator()()
		{
			DECL_GMEOBJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::GetObjectName( spThis ) ) );
		}
	};

	GMEMETHOD( TObject_KindName_Compatibility )
	{
		void operator()()
		{
			DECL_GMEOBJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::GetObjectKind( spThis ) ) );
		}
	};

	GMEMETHOD( TObject_Parent )
	{
		void operator()()
		{
			DECL_GMEOBJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			CComPtr<IMgaObject> spParent;
			spThis->GetParent( &spParent );
			SetResult( CREATE_GMEOBJECT( GetTypeManager(), spParent ) );
		}
	};

	GMEMETHOD( TObject_IsNull )
	{
		void operator()()
		{
			DECL_GMEOBJECT( spThis, GetThis() );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), ( spThis.p ) ? false : true ) );
		}
	};

	GMEMETHOD( TObject_IsFCO )
	{
		void operator()()
		{
			DECL_GMEOBJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), GetTypeManager()->IsTypeA( GetThis().GetTypeName(), "gme::FCO" ) >= 0 ) );
		}
	};

	GMEMETHOD( TObject_IsFolder )
	{
		void operator()()
		{
			DECL_GMEOBJECT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), GetTypeManager()->IsTypeA( GetThis().GetTypeName(), "gme::Folder" ) >= 0 ) );
		}
	};

	void TObject_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "name" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, new TObject_Name(), false ) );
			return;
		}

		if ( strName == "kindName" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, new TObject_KindName(), false ) );
			return;
		}

		if ( strName == "kindDisplayedName" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, new TObject_KindDisplayedName(), false ) );
			return;
		}

		if ( strName == "metaKindName" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, new TObject_StereotypeName(), false ) );
			return;
		}
	}

	void TObject_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "name" ) && iCount == 0 ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TObject_Name_Compatibility(), false ) );
			return;
		}

		if ( ( strName == "kindName" ) && iCount == 0 ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TObject_KindName_Compatibility(), false ) );
			return;
		}

		if ( ( strName == "isNull" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TObject_IsNull(), false ) );
			return;
		}

		if ( ( strName == "parent" ) && iCount == 0 ) {
			vecType.push_back( "gme::Object" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TObject_Parent(), false ) );
			return;
		}

		if ( ( strName == "isFCO" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TObject_IsFCO(), false ) );
			return;
		}

		if ( ( strName == "isFolder" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TObject_IsFolder(), false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Folder
//
//##############################################################################################################################################

	void TFolders__Folders( OclMeta::TypeManager* pManager, CComPtr<IMgaFolder> spFolder, OclMeta::ObjectVector& setFolders, bool bRecursive )
	{
		CComPtr<IMgaFolders> spFolders;
		COMTHROW( spFolder->get_ChildFolders( &spFolders ) );
		MGACOLL_ITERATE( IMgaFolder, spFolders ) {
			setFolders.push_back( CREATE_GMEOBJECT( pManager, MGACOLL_ITER ) );
			if ( bRecursive )
				TFolders__Folders( pManager, MGACOLL_ITER, setFolders, bRecursive );
		} MGACOLL_ITERATE_END;
	}

	void TFolders__FCOs( OclMeta::TypeManager* pManager, CComPtr<IMgaFolder> spFolder, OclMeta::ObjectVector& setFCOs, bool bRecursive )
	{
		CComPtr<IMgaFCOs> spFCOs;
		COMTHROW( spFolder->get_ChildFCOs( &spFCOs ) );
		MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
			setFCOs.push_back( CREATE_GMEOBJECT( pManager, MGACOLL_ITER ) );
		} MGACOLL_ITERATE_END;
		if ( bRecursive ) {
			CComPtr<IMgaFolders> spFolders;
			COMTHROW( spFolder->get_ChildFolders( &spFolders ) );
			MGACOLL_ITERATE( IMgaFolder, spFolders ) {
				TFolders__FCOs( pManager, MGACOLL_ITER, setFCOs, bRecursive );
			} MGACOLL_ITERATE_END;
		}
	}

	GMEMETHOD( TFolder_Folders )
	{
		void operator()()
		{
			DECL_GMEFOLDER( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			OclMeta::ObjectVector setFolders;
			TFolders__Folders( GetTypeManager(), spThis, setFolders, true );
			SetResult( CREATE_SET( GetTypeManager(), setFolders ) );
		}
	};

	GMEMETHOD( TFolder_ChildFolders )
	{
		void operator()()
		{
			DECL_GMEFOLDER( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			OclMeta::ObjectVector setFolders;
			TFolders__Folders( GetTypeManager(), spThis, setFolders, false );
			SetResult( CREATE_SET( GetTypeManager(), setFolders ) );
		}
	};

	GMEMETHOD( TFolder_RootDescendants )
	{
		void operator()()
		{
			DECL_GMEFOLDER( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			OclMeta::ObjectVector setFCOs;
			TFolders__FCOs( GetTypeManager(), spThis, setFCOs, true );
			SetResult( CREATE_SET( GetTypeManager(), setFCOs ) );
		}
	};

	GMEMETHOD( TFolder_RootChildren )
	{
		void operator()()
		{
			DECL_GMEFOLDER( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			OclMeta::ObjectVector setFCOs;
			TFolders__FCOs( GetTypeManager(), spThis, setFCOs, false );
			SetResult( CREATE_SET( GetTypeManager(), setFCOs ) );
		}
	};

	GMEMETHOD( TFolder_Models_Atoms )
	{
		private :
			std::string m_strObjType;

		public :
			TFolder_Models_Atoms( const std::string& strObjType )
				: m_strObjType( strObjType )
			{
			}

			void operator()()
			{
				DECL_GMEFOLDER( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );
				std::string strKind = GetKind( 0, ( m_strObjType == "OBJTYPE_MODEL" ) ? "gme::Model" : "gme::Atom" );

				CComPtr<IMgaProject> spProject;
				COMTHROW( spThis->get_Project( &spProject ) );
				CComPtr<IMgaFilter> spFilter;
				COMTHROW( spProject->CreateFilter( &spFilter ) );
				if ( ! strKind.empty() )
					COMTHROW( spFilter->put_Kind( CComBSTR( OclCommonEx::Convert( strKind ) ) ) );
				COMTHROW( spFilter->put_ObjType( CComBSTR( OclCommonEx::Convert( m_strObjType ) ) ) );
				COMTHROW( spFilter->put_Level( CComBSTR( "1-" ) ) );

				OclMeta::ObjectVector setOut;
				CComPtr<IMgaFCOs> spFCOs;
				COMTHROW( spThis->GetDescendantFCOs( spFilter, &spFCOs ) );
				MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
					setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), MGACOLL_ITER ) );
				} MGACOLL_ITERATE_END;

				SetResult( CREATE_SET( GetTypeManager(), setOut ) );
			}
	};

	void TFolder_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();

		if ( ( strName == "folders" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Folder" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFolder_Folders(), false ) );
		}

		if ( ( strName == "childFolders" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Folder" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFolder_ChildFolders(), false ) );
		}

		if ( ( strName == "rootDescendants" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFolder_RootDescendants(), false ) );
		}

		if ( ( strName == "allDescendants" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFolder_RootDescendants(), false ) );
		}

		if ( ( strName == "rootChildren" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFolder_RootChildren(), false ) );
		}

		if ( ( strName == "models" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFolder_Models_Atoms( "OBJTYPE_MODEL" ), false ) );
		}

		if ( ( strName == "models" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFolder_Models_Atoms( "OBJTYPE_MODEL" ), false ) );
		}

		if ( ( strName == "atoms" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFolder_Models_Atoms( "OBJTYPE_ATOM" ), false ) );
		}

		if ( ( strName == "atoms" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFolder_Models_Atoms( "OBJTYPE_ATOM" ), false ) );
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::FCO
//
//##############################################################################################################################################

	ATTRIBUTE( TFCO_RoleName )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::GetFCORole( spThis ) ) );
		}
	};

	GMEMETHOD( TFCO_RoleName_Compatibility )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::GetFCORole( spThis ) ) );
		}
	};

	GMEMETHOD( TFCO_ConnectedFCOs )
	{
		private :
			int m_iKindPos;

		public :
			TFCO_ConnectedFCOs( int iKindPos = -1 )
				: m_iKindPos( iKindPos )
			{
			}

			void operator()()
			{
				DECL_GMEFCO( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );
				std::string strRole;
				if ( m_iKindPos != 0 && GetArgumentCount() > 0 ) {
					DECL_STRING2( strRole, GetArgument( 0 ) );
					if ( strRole.empty() )
						ThrowException( "Role cannot be empty. Use gme::FCO::connectedFCOs( ocl::Type ) instead." );
				}
				std::string strKind = GetKind( m_iKindPos, "gme::Connection" );

				OclMeta::ObjectVector setOut;
				CComPtr<IMgaConnPoints> spStartCPs;
				COMTHROW( spThis->get_PartOfConns( &spStartCPs ) );
				MGACOLL_ITERATE( IMgaConnPoint , spStartCPs ) {
					CComPtr<IMgaConnPoint> spStartCP = MGACOLL_ITER;
					CComPtr<IMgaConnection> spConnection;
					COMTHROW( spStartCP->get_Owner( &spConnection ) );
					if ( strKind.empty() || strKind == OclCommonEx::GetObjectKind( spConnection.p ) ) {
						CComPtr<IMgaConnPoints> spEndCPs;
						COMTHROW( spConnection->get_ConnPoints( &spEndCPs ) );
						MGACOLL_ITERATE( IMgaConnPoint, spEndCPs ) {
							CComPtr<IMgaConnPoint> spEndCP = MGACOLL_ITER;
							if ( spStartCP != spEndCP && ( strRole.empty() || strRole == OclCommonEx::GetConnPointRole( spEndCP ) ) ) {
								CComPtr<IMgaFCO> spDestination;
								COMTHROW( spEndCP->get_Target( &spDestination ) );
								setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), spDestination ) );
							}
						} MGACOLL_ITERATE_END;
					}
				} MGACOLL_ITERATE_END;

				SetResult( CREATE_SET( GetTypeManager(), setOut ) );
			}
	};

	GMEMETHOD( TFCO_BagConnectedFCOs )
	{
		private :
			int m_iKindPos;

		public :
			TFCO_BagConnectedFCOs( int iKindPos = -1 )
				: m_iKindPos( iKindPos )
			{
			}

			void operator()()
			{
				DECL_GMEFCO( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );
				std::string strRole;
				if ( m_iKindPos != 0 && GetArgumentCount() > 0 ) {
					DECL_STRING2( strRole, GetArgument( 0 ) );
					if ( strRole.empty() )
						ThrowException( "Role cannot be empty. Use gme::FCO::bagConnectedFCOs( ocl::Type ) instead." );
				}
				std::string strKind = GetKind( m_iKindPos, "gme::Connection" );

				OclMeta::ObjectVector setOut;
				CComPtr<IMgaConnPoints> spStartCPs;
				COMTHROW( spThis->get_PartOfConns( &spStartCPs ) );
				MGACOLL_ITERATE( IMgaConnPoint , spStartCPs ) {
					CComPtr<IMgaConnPoint> spStartCP = MGACOLL_ITER;
					CComPtr<IMgaConnection> spConnection;
					COMTHROW( spStartCP->get_Owner( &spConnection ) );
					if ( strKind.empty() || strKind == OclCommonEx::GetObjectKind( spConnection.p ) ) {
						CComPtr<IMgaConnPoints> spEndCPs;
						COMTHROW( spConnection->get_ConnPoints( &spEndCPs ) );
						MGACOLL_ITERATE( IMgaConnPoint, spEndCPs ) {
							CComPtr<IMgaConnPoint> spEndCP = MGACOLL_ITER;
							if ( spStartCP != spEndCP && ( strRole.empty() || strRole == OclCommonEx::GetConnPointRole( spEndCP ) ) ) {
								CComPtr<IMgaFCO> spDestination;
								COMTHROW( spEndCP->get_Target( &spDestination ) );
								setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), spDestination ) );
							}
						} MGACOLL_ITERATE_END;
					}
				} MGACOLL_ITERATE_END;

				SetResult( CREATE_BAG( GetTypeManager(), setOut ) );
			}
	};

	// TFCO_Connected is obsolete and equivalent to TFCO_ConnectedFCOs

	GMEMETHOD( TFCO_ReverseConnectedFCOs )
	{
		private :
			int m_iKindPos;

		public :
			TFCO_ReverseConnectedFCOs( int iKindPos = -1 )
				: m_iKindPos( iKindPos )
			{
			}

			void operator()()
			{
				DECL_GMEFCO( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );
				std::string strRole;
				if ( m_iKindPos != 0 && GetArgumentCount() > 0 ) {
					DECL_STRING2( strRole, GetArgument( 0 ) );
					if ( strRole.empty() )
						ThrowException( "Role cannot be empty. Use gme::FCO::reverseConnectedFCOs( ocl::Type ) instead." );
				}
				std::string strKind = GetKind( m_iKindPos, "gme::Connection" );

				OclCommonEx::FCOVector vecFCOs;
				OclCommonEx::GetAssociationEnds( spThis, strRole, strKind, vecFCOs );
				OclMeta::ObjectVector setOut;
				for ( unsigned int i = 0 ; i < vecFCOs.size() ; i++ )
					setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), CComPtr<IMgaFCO>( vecFCOs[ i ].p ) ) );
				SetResult( CREATE_SET( GetTypeManager(), setOut ) );
			}
	};

	GMEMETHOD( TFCO_BagReverseConnectedFCOs )
	{
		private :
			int m_iKindPos;

		public :
			TFCO_BagReverseConnectedFCOs( int iKindPos = -1 )
				: m_iKindPos( iKindPos )
			{
			}

			void operator()()
			{
				DECL_GMEFCO( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );
				std::string strRole;
				if ( m_iKindPos != 0 && GetArgumentCount() > 0 ) {
					DECL_STRING2( strRole, GetArgument( 0 ) );
					if ( strRole.empty() )
						ThrowException( "Role cannot be empty. Use gme::FCO::bagReverseConnectedFCOs( ocl::Type ) instead." );
				}
				std::string strKind = GetKind( m_iKindPos, "gme::Connection" );

				OclMeta::ObjectVector setOut;
				CComPtr<IMgaConnPoints> spStartCPs;
				COMTHROW( spThis->get_PartOfConns( &spStartCPs ) );
				MGACOLL_ITERATE( IMgaConnPoint , spStartCPs ) {
					CComPtr<IMgaConnPoint> spStartCP = MGACOLL_ITER;
					if ( strRole.empty() || strRole == OclCommonEx::GetConnPointRole( spStartCP ) ) {
						CComPtr<IMgaConnection> spConnection;
						COMTHROW( spStartCP->get_Owner( &spConnection ) );
						if ( strKind.empty() || strKind == OclCommonEx::GetObjectKind( spConnection.p ) ) {
							CComPtr<IMgaConnPoints> spEndCPs;
							COMTHROW( spConnection->get_ConnPoints( &spEndCPs ) );
							MGACOLL_ITERATE( IMgaConnPoint, spEndCPs ) {
								CComPtr<IMgaConnPoint> spEndCP = MGACOLL_ITER;
								if ( spStartCP != spEndCP ) {
									CComPtr<IMgaFCO> spDestination;
									COMTHROW( spEndCP->get_Target( &spDestination ) );
									setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), spDestination ) );
								}
							} MGACOLL_ITERATE_END;
						}
					}
				} MGACOLL_ITERATE_END;

				SetResult( CREATE_BAG( GetTypeManager(), setOut ) );
			}
	};

	// TFCO_ConnectedAs is obsolete and equivalent to TFCO_ReverseConnectedFCOs

	GMEMETHOD( TFCO_IsConnectedTo )
	{
		private :
			int m_iKindPos;

		public :
			TFCO_IsConnectedTo( int iKindPos = -1 )
				: m_iKindPos( iKindPos )
			{
			}

			void operator()()
			{
				DECL_GMEFCO( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );
				DECL_GMEFCO( spPeer, GetArgument( 0 ) );
				if ( ! spPeer.p )
					ThrowException( "Argument is null." );
				std::string strRole;
				if ( m_iKindPos != 1 && GetArgumentCount() > 1 ) {
					DECL_STRING2( strRole, GetArgument( 1 ) );
					if ( strRole.empty() )
						ThrowException( "Role cannot be empty. Use gme::FCO::isConnectedTo( gme::FCO, ocl::Type ) instead." );
				}
				std::string strKind = GetKind( m_iKindPos, "gme::Connection" );

				CComPtr<IMgaConnPoints> spStartCPs;
				COMTHROW( spThis->get_PartOfConns( &spStartCPs ) );
				MGACOLL_ITERATE( IMgaConnPoint , spStartCPs ) {
					CComPtr<IMgaConnPoint> spStartCP = MGACOLL_ITER;
					CComPtr<IMgaConnection> spConnection;
					COMTHROW( spStartCP->get_Owner( &spConnection ) );
					if ( strKind.empty() || strKind == OclCommonEx::GetObjectKind( spConnection.p ) ) {
						CComPtr<IMgaConnPoints> spEndCPs;
						COMTHROW( spConnection->get_ConnPoints( &spEndCPs ) );
						MGACOLL_ITERATE( IMgaConnPoint, spEndCPs ) {
							CComPtr<IMgaConnPoint> spEndCP = MGACOLL_ITER;
							if ( spStartCP != spEndCP && ( strRole.empty() || strRole == OclCommonEx::GetConnPointRole( spEndCP ) ) ) {
								CComPtr<IMgaFCO> spDestination;
								COMTHROW( spEndCP->get_Target( &spDestination ) );
								if ( spDestination == spPeer ) {
									SetResult( CREATE_BOOLEAN( GetTypeManager(), true ) );
									return;
								}
							}
						} MGACOLL_ITERATE_END;
					}
				} MGACOLL_ITERATE_END;
				SetResult( CREATE_BOOLEAN( GetTypeManager(), false ) );
			}
	};

	GMEMETHOD( TFCO_AttachingConnPoints )
	{
		private :
			int m_iKindPos;

		public :
			TFCO_AttachingConnPoints( int iKindPos = -1 )
				: m_iKindPos( iKindPos )
			{
			}

			void operator()()
			{
				DECL_GMEFCO( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );
				std::string strRole;
				if ( m_iKindPos != 0 && GetArgumentCount() > 0 ) {
					DECL_STRING2( strRole, GetArgument( 0 ) );
					if ( strRole.empty() )
						ThrowException( "Role cannot be empty. Use gme::FCO::attachingConnPoints( ocl::Type ) instead." );
				}
				std::string strKind = GetKind( m_iKindPos, "gme::Connection" );

				OclMeta::ObjectVector setOut;
				CComPtr<IMgaConnPoints> spStartCPs;
				COMTHROW( spThis->get_PartOfConns( &spStartCPs ) );
				MGACOLL_ITERATE( IMgaConnPoint, spStartCPs ) {
					CComPtr<IMgaConnPoint> spStartCP = MGACOLL_ITER;
					if ( strRole.empty() || strRole == OclCommonEx::GetConnPointRole( spStartCP ) ) {
						CComPtr<IMgaConnection> spConnection;
						COMTHROW( spStartCP->get_Owner( &spConnection ) );
						if ( strKind.empty() || strKind == OclCommonEx::GetObjectKind( spConnection.p ) ) {
							setOut.push_back( CREATE_GMECONNECTIONPOINT( GetTypeManager(), spStartCP ) );
						}
					}
				} MGACOLL_ITERATE_END;

				SetResult( CREATE_SET( GetTypeManager(), setOut ) );
			}
	};

	GMEMETHOD( TFCO_AttachingConnections )
	{
		private :
			int m_iKindPos;

		public :
			TFCO_AttachingConnections( int iKindPos = -1 )
				: m_iKindPos( iKindPos )
			{
			}

			void operator()()
			{
				DECL_GMEFCO( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );
				std::string strRole;
				if ( m_iKindPos != 0 && GetArgumentCount() > 0 ) {
					DECL_STRING2( strRole, GetArgument( 0 ) );
					if ( strRole.empty() )
						ThrowException( "Role cannot be empty. Use gme::FCO::attachingConnections( ocl::Type ) instead." );
				}
				std::string strKind = GetKind( m_iKindPos, "gme::Connection" );

				OclMeta::ObjectVector setOut;
				CComPtr<IMgaConnPoints> spStartCPs;
				COMTHROW( spThis->get_PartOfConns( &spStartCPs ) );
				MGACOLL_ITERATE( IMgaConnPoint, spStartCPs ) {
					CComPtr<IMgaConnPoint> spStartCP = MGACOLL_ITER;
					if ( strRole.empty() || strRole == OclCommonEx::GetConnPointRole( spStartCP ) ) {
						CComPtr<IMgaConnection> spConnection;
						COMTHROW( spStartCP->get_Owner( &spConnection ) );
						if ( strKind.empty() || strKind == OclCommonEx::GetObjectKind( spConnection.p ) ) {
							setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), spConnection ) );
						}
					}
				} MGACOLL_ITERATE_END;

				SetResult( CREATE_SET( GetTypeManager(), setOut ) );
			}
	};

	GMEMETHOD( TFCO_ReferencedBy )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			std::string strKind = GetKind( 0, "gme::Reference" );

			OclMeta::ObjectVector setOut;
			CComPtr<IMgaFCOs> spRefs;
			COMTHROW( spThis->get_ReferencedBy( &spRefs ) );
			MGACOLL_ITERATE( IMgaFCO, spRefs ) {
				if ( strKind.empty() || strKind == OclCommonEx::GetObjectKind( MGACOLL_ITER.p ) ) {
					setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), MGACOLL_ITER ) );
				}
			} MGACOLL_ITERATE_END;

			SetResult( CREATE_SET( GetTypeManager(), setOut ) );
		}
	};

	GMEMETHOD( TFCO_MemberOfSets )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			std::string strKind = GetKind( 0, "gme::Set" );

			OclMeta::ObjectVector setOut;
			CComPtr<IMgaFCOs> spSets;
			COMTHROW( spThis->get_MemberOfSets( &spSets ) );
			MGACOLL_ITERATE( IMgaFCO, spSets ) {
				if ( strKind.empty() || strKind == OclCommonEx::GetObjectKind( MGACOLL_ITER.p ) ) {
					setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), MGACOLL_ITER ) );
				}
			} MGACOLL_ITERATE_END;

			SetResult( CREATE_SET( GetTypeManager(), setOut ) );
		}
	};

	GMEMETHOD( TFCO_SubTypes )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			OclMeta::ObjectVector setOut;
			CComPtr<IMgaFCOs> spFCOs;
			COMTHROW( spThis->get_DerivedObjects( &spFCOs ) );
			MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
				VARIANT_BOOL bInstance;
				COMTHROW( MGACOLL_ITER->get_IsInstance( &bInstance ) );
				if( ! bInstance ) {
					setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), MGACOLL_ITER ) );
				}
			} MGACOLL_ITERATE_END;

			SetResult( CREATE_SET( GetTypeManager(), setOut ) );
		}
	};

	GMEMETHOD( TFCO_Instances )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			OclMeta::ObjectVector setOut;
			CComPtr<IMgaFCOs> spFCOs;
			COMTHROW( spThis->get_DerivedObjects( &spFCOs ) );
			MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
				VARIANT_BOOL bInstance;
				COMTHROW( MGACOLL_ITER->get_IsInstance( &bInstance ) );
				if( bInstance )
					setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), MGACOLL_ITER ) );
			} MGACOLL_ITERATE_END;

			SetResult( CREATE_SET( GetTypeManager(), setOut ) );
		}
	};

	GMEMETHOD( TFCO_Type )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			CComPtr<IMgaFCO> spType;
			COMTHROW( spThis->get_DerivedFrom( &spType ) );
			SetResult( CREATE_GMEOBJECT( GetTypeManager(), spType ) );
		}
	};

	GMEMETHOD( TFCO_BaseType )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			CComPtr<IMgaFCO> spType;
			COMTHROW( spThis->get_BaseType( &spType ) );
			SetResult( CREATE_GMEOBJECT( GetTypeManager(), spType ) );
		}
	};

	GMEMETHOD( TFCO_IsInstance )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			VARIANT_BOOL bInstance;
			COMTHROW( spThis->get_IsInstance( &bInstance ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), ( bInstance ) ? true : false ) );
		}
	};

	GMEMETHOD( TFCO_IsType )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			VARIANT_BOOL bInstance;
			COMTHROW( spThis->get_IsInstance( &bInstance ) );
			SetResult( CREATE_BOOLEAN( GetTypeManager(), ( bInstance ) ? false : true ) );
		}
	};

	GMEMETHOD( TFCO_Folder )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			CComPtr<IMgaFCO> spRootFCO;
			COMTHROW( spThis->get_RootFCO( &spRootFCO ) );
			CComPtr<IMgaObject> spFolder;
			COMTHROW( spRootFCO->GetParent( &spFolder ) );
			SetResult( CREATE_GMEOBJECT( GetTypeManager(), spFolder ) );
		}
	};

	GMEMETHOD( TFCO_Attribute )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			DECL_STRING( strName, GetArgument( 0 ) );

			CComVariant objValue;
			if ( ! SUCCEEDED( spThis->get_AttributeByName( CComBSTR( OclCommonEx::Convert( strName ) ), PutOut( objValue ) ) ) )
				ThrowException( "Attribute [ " + strName + " ] does not exist." );
			switch( objValue.vt ) {
				case VT_R8 :
					SetResult( CREATE_REAL( GetTypeManager(), objValue.dblVal ) ); // double
					break;
				case VT_I4 :
					SetResult( CREATE_INTEGER( GetTypeManager(), objValue.lVal ) ); // long
					break;
				case VT_BOOL:
					SetResult( CREATE_BOOLEAN( GetTypeManager(), ( objValue.boolVal ) ? true : false ) ); // boolean
					break;
				case VT_BSTR : {
					CComPtr<IMgaMetaAttribute> spAttribute;
					CComPtr<IMgaMetaFCO> spMeta;
					COMTHROW( spThis->get_Meta( &spMeta ) );
					COMTHROW( spMeta->get_AttributeByName( CComBSTR( OclCommonEx::Convert( strName ) ), &spAttribute ) );
					attval_enum eType;
					COMTHROW( spAttribute->get_ValueType( &eType ) );
					if ( eType == ATTVAL_STRING )
						SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::Convert( CString( objValue.bstrVal ) ) ) ); // string
					else
						SetResult( CREATE_ENUMERATION( GetTypeManager(), OclCommonEx::Convert( CString( objValue.bstrVal ) ) ) ); // enum
					break;
				}
				default:
					SetResult( OclMeta::Object::UNDEFINED );
			}
		}
	};

//---
GMEMETHOD( TFCO_AttrByDefault )
	{
		void operator()()
		{
			DECL_GMEFCO( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			DECL_STRING( strName, GetArgument( 0 ) );

			// Iterating through the FirstFCO's attributes
			CComPtr<IMgaMetaFCO> metafco;
			if ( ! SUCCEEDED( spThis->get_Meta(&metafco)))
				ThrowException( "There is no IMgaMetaFCO interface." );
			CComPtr<IMgaMetaAttribute> metaattrib;
			if ( ! SUCCEEDED( metafco->get_AttributeByName(CComBSTR( OclCommonEx::Convert( strName ) ), &metaattrib)))
				ThrowException( "Attribute [ " + strName + " ] does not exist." );
			CComPtr<IMgaAttribute> attrib;
			if ( ! SUCCEEDED( spThis->get_Attribute(metaattrib, &attrib)))
				ThrowException( "There is no IMgaAttribute interface of attribute [ " + strName + " ]." );
			long status;
			if ( ! SUCCEEDED( attrib->get_Status(&status)))
				ThrowException( "status cannot be read of attribute [ " + strName + " ]." );

			// Check whether it is a default value (if it is defined in the meta it's default)
			bool isDefault = (status < 0);
			SetResult( CREATE_BOOLEAN( GetTypeManager(), isDefault) ); // boolean
		}
	};

//---

	void TFCO_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "roleName" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, new TFCO_RoleName(), false ) );
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
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_RoleName_Compatibility(), false ) );
		}

		if ( ( strName == "subTypes" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_SubTypes(), false ) );
		}

		if ( ( strName == "instances" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_Instances(), false ) );
		}

		if ( ( strName == "type" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_Type(), false ) );
		}

		if ( ( strName == "baseType" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_BaseType(), false ) );
		}

		if ( ( strName == "isInstance" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_IsInstance(), false ) );
		}

		if ( ( strName == "isType" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_IsType(), false ) );
		}

		if ( ( strName == "folder" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "gme::Folder" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_Folder(), false ) );
		}

		if ( ( strName == "referencedBy" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Reference" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_ReferencedBy(), false ) );
		}

		if ( ( strName == "referencedBy" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Reference" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_ReferencedBy(), false ) );
		}

		if ( ( strName == "memberOfSets" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Set" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_MemberOfSets(), false ) );
		}

		if ( ( strName == "memberOfSets" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Set" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_MemberOfSets(), false ) );
		}

		if ( ( strName == "connectedFCOs" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_ConnectedFCOs( 1 ), false ) );
		}

		if ( ( strName == "connectedFCOs" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_ConnectedFCOs( 1 ), false ) );
		}

		if ( ( strName == "connectedFCOs" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_ConnectedFCOs( 0 ), false ) );
		}

		if ( ( strName == "bagConnectedFCOs" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_BagConnectedFCOs( 1 ), false ) );
		}

		if ( ( strName == "bagConnectedFCOs" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_BagConnectedFCOs( 0 ), false ) );
		}

		if ( ( strName == "connected" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_ConnectedFCOs( 1 ), false ) );
		}

		if ( ( strName == "reverseConnectedFCOs" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_ReverseConnectedFCOs( 1 ), false ) );
		}

		if ( ( strName == "reverseConnectedFCOs" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_ReverseConnectedFCOs( 1 ), false ) );
		}

		if ( ( strName == "reverseConnectedFCOs" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_ReverseConnectedFCOs( 0 ), false ) );
		}

		if ( ( strName == "bagReverseConnectedFCOs" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_BagReverseConnectedFCOs( 1 ), false ) );
		}

		if ( ( strName == "bagReverseConnectedFCOs" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Bag" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_BagReverseConnectedFCOs( 0 ), false ) );
		}

		if ( ( strName == "connectedAs" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_ReverseConnectedFCOs( 1 ), false ) );
		}

		if ( ( strName == "attachingConnPoints" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_AttachingConnPoints( 1 ), false ) );
		}

		if ( ( strName == "attachingConnPoints" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_AttachingConnPoints( 1 ), false ) );
		}

		if ( ( strName == "attachingConnPoints" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_AttachingConnPoints( 0 ), false ) );
		}

		if ( ( strName == "attachingConnections" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_AttachingConnections( 1 ), false ) );
		}

		if ( ( strName == "attachingConnections" ) && iCount <= 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_AttachingConnections( 1 ), false ) );
		}

		if ( ( strName == "attachingConnections" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_AttachingConnections( 0 ), false ) );
		}

		if ( ( strName == "isConnectedTo" ) && iCount == 3 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "fco", "gme::FCO", true ) );
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_IsConnectedTo( 2 ), false ) );
		}

		if ( ( strName == "isConnectedTo" ) && iCount <= 3 && iCount >= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "fco", "gme::FCO", true ) );
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_IsConnectedTo( 2 ), false ) );
		}

		if ( ( strName == "isConnectedTo" ) && iCount == 2 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "fco", "gme::FCO", true ) );
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_IsConnectedTo( 1 ), false ) );
		}

		if ( ( strName == "attribute" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;	
			vecParams.push_back( FP( "name", "ocl::String", true ) );
			vecType.push_back( "ocl::Any" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_Attribute(), false ) );
		}
//----
		if ( ( strName == "attrByDefault" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "name", "ocl::String", true ) );
			vecType.push_back( "ocl::Boolean" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TFCO_AttrByDefault(), false ) );
		}
//----
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Connection
//
//##############################################################################################################################################

	GMEMETHOD( TConnection_ConnectionPoints )
	{
		void operator()()
		{
			DECL_GMECONNECTION( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			std::string strRole;
			if ( GetArgumentCount() > 0 ) {
				DECL_STRING2( strRole, GetArgument( 0 ) );
				if ( strRole.empty() )
					ThrowException( "Role cannot be empty. Use gme::Connection::connectionPoints() without role." );
			}

			OclMeta::ObjectVector setOut;
			CComPtr<IMgaConnPoints> spCPs;
			COMTHROW( spThis->get_ConnPoints( &spCPs ) );
			MGACOLL_ITERATE( IMgaConnPoint, spCPs ) {
				if ( strRole.empty() || strRole == OclCommonEx::GetConnPointRole( MGACOLL_ITER ) ) {
					setOut.push_back( CREATE_GMECONNECTIONPOINT( GetTypeManager(), MGACOLL_ITER ) );
				}
			} MGACOLL_ITERATE_END;

			SetResult( CREATE_SET( GetTypeManager(), setOut ) );
		}
	};

	GMEMETHOD( TConnection_ConnectionPoint )
	{
		void operator()()
		{
			DECL_GMECONNECTION( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			DECL_STRING( strRole, GetArgument( 0 ) );
			if ( strRole.empty() )
				ThrowException( "Role cannot be empty." );

			CComPtr<IMgaConnPoints> spCPs;
			COMTHROW( spThis->get_ConnPoints( &spCPs ) );
			MGACOLL_ITERATE( IMgaConnPoint, spCPs ) {
				if ( strRole == OclCommonEx::GetConnPointRole( MGACOLL_ITER ) ) {
					SetResult( CREATE_GMECONNECTIONPOINT( GetTypeManager(), MGACOLL_ITER ) );
					return;
				}
			} MGACOLL_ITERATE_END;
			ThrowException( "Role '" + strRole + "' is invalid." );
		}
	};

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
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TConnection_ConnectionPoints(), false ) );
			return;
		}

		if ( ( strName == "connectionPoint" ) && iCount == 1 ) {
			vecParams.push_back( FP( "role", "ocl::String", true ) );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TConnection_ConnectionPoint(), false ) );
			return;
		}

	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Reference
//
//##############################################################################################################################################

	GMEMETHOD( TReference_UsedByConnPoints )
	{
		void operator()()
		{
			DECL_GMEREFERENCE( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			std::string strKind = GetKind( 0, "gme::Connection" );

			OclMeta::ObjectVector setOut;
			CComPtr<IMgaConnPoints> spCPs;
			COMTHROW( spThis->get_UsedByConns( &spCPs ) );
			MGACOLL_ITERATE( IMgaConnPoint, spCPs ) {
				if ( ! strKind.empty() ) {
					CComPtr<IMgaConnection> spConnection;
					COMTHROW( MGACOLL_ITER->get_Owner( &spConnection ) );
					if ( strKind != OclCommonEx::GetObjectKind( spConnection.p ) )
						continue;
				}
				setOut.push_back( CREATE_GMECONNECTIONPOINT( GetTypeManager(), MGACOLL_ITER ) );
			} MGACOLL_ITERATE_END;

			SetResult( CREATE_SET( GetTypeManager(), setOut ) );
		}
	};

	GMEMETHOD( TReference_RefersTo )
	{
		void operator()()
		{
			DECL_GMEREFERENCE( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			CComPtr<IMgaFCO> spOut;
			COMTHROW( spThis->get_Referred( &spOut ) );
			SetResult( CREATE_GMEOBJECT( GetTypeManager(), spOut ) );
		}
	};

	void TReference_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();

		if ( ( strName == "usedByConnPoints" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReference_UsedByConnPoints(), false ) );
		}

		if ( ( strName == "usedByConnPoints" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReference_UsedByConnPoints(), false ) );
		}

		if ( ( strName == "refersTo" ) && iCount == 0 ) {
			FPV vecParams; TS vecType;
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TReference_RefersTo(), false ) );
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   gme::Model
//
//##############################################################################################################################################

	GMEMETHOD( TModel_Models_Atoms )
	{
		private :
			std::string m_strObjType;

		public :
			TModel_Models_Atoms( const std::string& strObjType )
				: m_strObjType( strObjType )
			{
			}

			void operator()()
			{
				DECL_GMEMODEL( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );
				std::string strKind = GetKind( 0, ( m_strObjType == "OBJTYPE_MODEL" ) ? "gme::Model" : "gme::Atom" );

				CComPtr<IMgaProject> spProject;
				COMTHROW( spThis->get_Project( &spProject ) );
				CComPtr<IMgaFilter> spFilter;
				COMTHROW( spProject->CreateFilter( &spFilter ) );
				if ( ! strKind.empty() )
					COMTHROW( spFilter->put_Kind( CComBSTR( OclCommonEx::Convert( strKind ) ) ) );
				COMTHROW( spFilter->put_ObjType( CComBSTR( OclCommonEx::Convert( m_strObjType ) ) ) );
				COMTHROW( spFilter->put_Level( CComBSTR( "1-" ) ) );

				OclMeta::ObjectVector setOut;
				CComPtr<IMgaFCOs> spFCOs;
				COMTHROW( spThis->GetDescendantFCOs( spFilter, &spFCOs ) );
				MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
					setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), MGACOLL_ITER ) );
				} MGACOLL_ITERATE_END;

				SetResult( CREATE_SET( GetTypeManager(), setOut ) );
			}
	};

	GMEMETHOD( TModel_Parts )
	{
		private :
			objtype_enum 	m_eType;
			int					m_iKindPos;

		public :
			TModel_Parts( objtype_enum eType, int iKindPos = -1 )
				: m_iKindPos( iKindPos ), m_eType( eType )
			{
			}

			void operator()()
			{
				DECL_GMEMODEL( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );
				std::string strRole;
				if ( m_iKindPos != 0 && GetArgumentCount() > 0 ) {
					DECL_STRING2( strRole, GetArgument( 0 ) );
					if ( strRole.empty() )
						ThrowException( "Role cannot be empty. Use the approriate method without argument." );
				}
				std::string strKind = GetKind( m_iKindPos, "gme::" + ( ( m_eType == OBJTYPE_NULL ) ? "Object" : OclCommonEx::ObjectTypeToString( m_eType ) ) );

				OclMeta::ObjectVector setOut;
				CComPtr<IMgaFCOs> spFCOs;
				COMTHROW( spThis->get_ChildFCOs( &spFCOs ) );
				MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
					if ( m_eType == OBJTYPE_NULL || m_eType == OclCommonEx::GetObjectType( MGACOLL_ITER.p ) )
						if ( strRole.empty() || strRole == OclCommonEx::GetFCORole( MGACOLL_ITER ) )
							if ( strKind.empty() || strKind == OclCommonEx::GetObjectKind( MGACOLL_ITER.p ) )
								setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), MGACOLL_ITER ) );
				} MGACOLL_ITERATE_END;

				SetResult( CREATE_SET( GetTypeManager(), setOut ) );
			}
	};

	void TModel_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "models" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Models_Atoms( "OBJTYPE_MODEL" ), false ) );
		}

		if ( ( strName == "models" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Models_Atoms( "OBJTYPE_MODEL" ), false ) );
		}

		if ( ( strName == "atoms" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::String", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Models_Atoms( "OBJTYPE_ATOM" ), false ) );
		}

		if ( ( strName == "atoms" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Models_Atoms( "OBJTYPE_ATOM" ), false ) );
		}

		if ( ( strName == "parts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_NULL ), false ) );
		}

		if ( ( strName == "modelParts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_MODEL ), false ) );
		}

		if ( ( strName == "atomParts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_ATOM ), false ) );
		}

		if ( ( strName == "connectionParts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_CONNECTION ), false ) );
		}

		if ( ( strName == "referenceParts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Reference" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_REFERENCE ), false ) );
		}

		if ( ( strName == "setParts" ) && iCount <= 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "role", "ocl::String", false ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Set" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_SET ), false ) );
		}

		if ( ( strName == "parts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_NULL, 0 ), false ) );
		}

		if ( ( strName == "modelParts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Model" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_MODEL, 0 ), false ) );
		}

		if ( ( strName == "atomParts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Atom" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_ATOM, 0 ), false ) );
		}

		if ( ( strName == "connectionParts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_CONNECTION, 0 ), false ) );
		}

		if ( ( strName == "referenceParts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Reference" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_REFERENCE, 0 ), false ) );
		}

		if ( ( strName == "setParts" ) && iCount == 1 ) {
			FPV vecParams; TS vecType;
			vecParams.push_back( FP( "kind", "ocl::Type", true ) );
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::Set" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TModel_Parts( OBJTYPE_SET, 0 ), false ) );
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

	GMEMETHOD( TSet_Members )
	{
		void operator()()
		{
			DECL_GMESET( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			OclMeta::ObjectVector setOut;
			CComPtr<IMgaFCOs> spFCOs;
			COMTHROW( spThis->get_Members( &spFCOs ) );
			MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
				setOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), MGACOLL_ITER ) );
			} MGACOLL_ITERATE_END;

			SetResult( CREATE_SET( GetTypeManager(), setOut ) );
		}
	};

	void TSet_MethodFactory::GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		int iCount = signature.GetParameterCount();
		FPV vecParams;
		TS vecType;

		if ( ( strName == "members" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Set" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TSet_Members(), false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E    O F   gme::ConnectionPoint
//
//##############################################################################################################################################

	ATTRIBUTE( TConnectionPoint_CPRoleName )
	{
		void operator()()
		{
			DECL_GMECONNECTIONPOINT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::GetConnPointRole( spThis ) ) );
		}
	};

	GMEMETHOD( TConnectionPoint_CPRoleName_Compatibility )
	{
		void operator()()
		{
			DECL_GMECONNECTIONPOINT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );
			SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::GetConnPointRole( spThis ) ) );
		}
	};

	GMEMETHOD( TConnectionPoint_Owner )
	{
		void operator()()
		{
			DECL_GMECONNECTIONPOINT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			CComPtr<IMgaConnection> spConnection;
			COMTHROW( spThis->get_Owner( &spConnection ) );
			SetResult( CREATE_GMEOBJECT( GetTypeManager(), spConnection ) );
		}
	};

	GMEMETHOD( TConnectionPoint_Target )
	{
		void operator()()
		{
			DECL_GMECONNECTIONPOINT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			CComPtr<IMgaFCO> spTarget;
			COMTHROW( spThis->get_Target( &spTarget ) );
			SetResult( CREATE_GMEOBJECT( GetTypeManager(), spTarget ) );
		}
	};

	GMEMETHOD( TConnectionPoint_Peer )
	{
		void operator()()
		{
			DECL_GMECONNECTIONPOINT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			CComPtr<IMgaConnection> spConnection;
			CComPtr<IMgaConnPoints> spCPs;
			COMTHROW( spThis->get_Owner( &spConnection ) );
			COMTHROW( spConnection->get_ConnPoints( &spCPs ) );

			long lCount;
			COMTHROW( spCPs->get_Count( &lCount) );
			if( lCount != 2 )
				ThrowException( "Peer cannot be used for non-binary associations." );

			MGACOLL_ITERATE( IMgaConnPoint, spCPs ) {
				if ( MGACOLL_ITER != spThis ) {
					SetResult( CREATE_GMECONNECTIONPOINT( GetTypeManager(), MGACOLL_ITER ) );
					return;
				}
			} MGACOLL_ITERATE_END;
			SetResult( OclMeta::Object::UNDEFINED );
		}
	};

	GMEMETHOD( TConnectionPoint_UsedReferences )
	{
		void operator()()
		{
			DECL_GMECONNECTIONPOINT( spThis, GetThis() );
			if ( ! spThis.p )
				ThrowException( "Object is null." );

			OclMeta::ObjectVector vecOut;
			CComPtr<IMgaFCOs> spFCOs;
			COMTHROW( spThis->get_References( &spFCOs ) );
			MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
				vecOut.push_back( CREATE_GMEOBJECT( GetTypeManager(), MGACOLL_ITER ) );
			} MGACOLL_ITERATE_END;
			SetResult( CREATE_SEQUENCE( GetTypeManager(), vecOut ) );
		}
	};

	void TConnectionPoint_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		std::string strName = signature.GetName();
		TS vecType;

		if ( strName == "cpRoleName" ) {
			vecType.push_back( "ocl::String" );
			vecFeatures.push_back( new OclMeta::Attribute( strName, vecType, new TConnectionPoint_CPRoleName(), false ) );
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
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TConnectionPoint_CPRoleName_Compatibility(), false ) );
			return;
		}

		if ( ( strName == "owner" ) && iCount == 0 ) {
			vecType.push_back( "gme::Connection" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TConnectionPoint_Owner(), false ) );
			return;
		}

		if ( ( strName == "target" ) && iCount == 0 ) {
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TConnectionPoint_Target(), false ) );
			return;
		}

		if ( ( strName == "peer" ) && iCount == 0 ) {
			vecType.push_back( "gme::ConnectionPoint" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TConnectionPoint_Peer(), false ) );
			return;
		}

		if ( ( strName == "usedReferences" ) && iCount == 0 ) {
			vecType.push_back( "ocl::Sequence" );
			vecType.push_back( "gme::FCO" );
			vecFeatures.push_back( new OclMeta::Method( strName, vecParams, vecType, new TConnectionPoint_UsedReferences(), false ) );
			return;
		}
	}

//##############################################################################################################################################
//
//	T Y P E   O F   D E R I V E D   O F   gme::FCO
//
//##############################################################################################################################################

	class FCODerived_Attribute
		: public OclImplementation::Attribute
	{
		private :
			std::string 	m_strName;

		public :
			FCODerived_Attribute( const std::string strName )
				: m_strName( strName )
			{
			}

			void operator()()
			{
				DECL_GMEFCO( spThis, GetThis() );
				if ( ! spThis.p )
					ThrowException( "Object is null." );

				CComVariant objValue;
				if ( ! SUCCEEDED( spThis->get_AttributeByName( CComBSTR( OclCommonEx::Convert( m_strName ) ), PutOut( objValue ) ) ) )
					ThrowException( "Attribute [ " + m_strName + " ] does not exist." );
				switch( objValue.vt ) {
					case VT_R8 :
						SetResult( CREATE_REAL( GetTypeManager(), objValue.dblVal ) ); // double
						break;
					case VT_I4 :
						SetResult( CREATE_INTEGER( GetTypeManager(), objValue.lVal ) ); // long
						break;
					case VT_BOOL:
						SetResult( CREATE_BOOLEAN( GetTypeManager(), ( objValue.boolVal ) ? true : false ) ); // boolean
						break;
					case VT_BSTR : {
						CComPtr<IMgaMetaAttribute> spAttribute;
						CComPtr<IMgaMetaFCO> spMeta;
						COMTHROW( spThis->get_Meta( &spMeta ) );
						COMTHROW( spMeta->get_AttributeByName( CComBSTR( OclCommonEx::Convert( m_strName ) ), &spAttribute ) );
						attval_enum eType;
						COMTHROW( spAttribute->get_ValueType( &eType ) );
						if ( eType == ATTVAL_STRING )
							SetResult( CREATE_STRING( GetTypeManager(), OclCommonEx::Convert( CString( objValue.bstrVal ) ) ) ); // string
						else
							SetResult( CREATE_ENUMERATION( GetTypeManager(), OclCommonEx::Convert( CString( objValue.bstrVal ) ) ) ); // enum
						break;
					}
					default:
						SetResult( OclMeta::Object::UNDEFINED );
				}
			}
	};

	TFCODerived_AttributeFactory::TFCODerived_AttributeFactory( CComPtr<IMgaMetaFCO> spMetaFCO )
		: m_spMetaFCO( spMetaFCO )
	{
	}

	void TFCODerived_AttributeFactory::GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
	{
		CComPtr<IMgaMetaAttributes> spAttributes;
		COMTHROW( m_spMetaFCO->get_Attributes( &spAttributes ) );
		MGACOLL_ITERATE( IMgaMetaAttribute, spAttributes ) {
			if ( signature.GetName() == OclCommonEx::GetObjectName( MGACOLL_ITER.p ) ) {
				attval_enum eType;
				COMTHROW( MGACOLL_ITER->get_ValueType( &eType ) );
				std::string strReturnType;
				switch ( eType ) {
					case ATTVAL_STRING 		: strReturnType = "ocl::String"; break;
					case ATTVAL_INTEGER 	: strReturnType = "ocl::Integer"; break;
					case ATTVAL_DOUBLE 		: strReturnType = "ocl::Real"; break;
					case ATTVAL_BOOLEAN 	: strReturnType = "ocl::Boolean"; break;
					case ATTVAL_ENUM 		: strReturnType = "ocl::Enumeration"; break;
				}
				if ( ! strReturnType.empty() )
					vecFeatures.push_back( new OclMeta::Attribute( signature.GetName(), TS( 1, strReturnType ), new FCODerived_Attribute( signature.GetName() ), true ) );
			}
		} MGACOLL_ITERATE_END;
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

	void TypeFactory::GetTypes( const std::string& strName, const std::string& strNSpace, std::vector<unique_ptr<OclMeta::Type>>& vecTypes, std::string& strNameResult )
	{
		strNameResult = strName; // will be overwritten by the GetDynamicTypes if it finds something
		bool bHasNamespace = strName.find( "::" ) != std::string::npos;
		std::string str2Name = strName;
		if( vecTypes.empty()
			&& !strNSpace.empty()                                                          // if strNSpace specified
			&& (!bHasNamespace || strName.substr( 0, strName.find( "::")) == "meta")       // either 'meta::' prefix found or no namespace found
			&& strName.find( std::string( "::" + strNSpace + "::"))  == std::string::npos  // "::strNSpace::" is not yet substr of strName
			&& strName != "meta::RootFolder" )                                             // special kind needs not modification: meta::RootFolder
		{
			
			if( strName.find( "meta::") != std::string::npos)                              // 'meta::' found
			{
				str2Name.insert( strName.find( "::" ), std::string( "::" + strNSpace));    // str2Name = meta::strNSpace::strName
			}
			else
			{
				str2Name = strNSpace + "::" + strName;                                     // str2Name = strNSpace::strName
			}
		}

		GetDynamicTypes( str2Name, vecTypes, strNameResult );
		if ( ! bHasNamespace && ! vecTypes.empty() )
			return;

		GetStereotypes( strName, vecTypes );
		if ( ! bHasNamespace && ! vecTypes.empty() )
			return;

		if ( strName == "ocl::Any" || strName == "Any" ) {
			StringVector vecSupers;
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Any", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TAny_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Boolean" || strName == "bool" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Boolean", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclImplementation::MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Enumeration" || strName == "enum" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Enumeration", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TEnumeration_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::String" || strName == "string" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::String", vecSupers, new OclBasic::TString_AttributeFactory(), new OclImplementation::AssociationFactory(), new TStringEx_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Real" || strName == "real" || strName == "double" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Real", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TReal_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Integer" || strName == "int" || strName == "long" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Real" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Integer", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TInteger_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Type" || strName == "Type" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "ocl::Type", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclImplementation::MethodFactory(), false ) ));
			return;
		}

		if ( strName == "ocl::Collection" || strName == "Collection" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Collection", vecSupers, new OclBasic::TCollection_AttributeFactory(), new OclImplementation::AssociationFactory(), new TCollectionEx_MethodFactory(),new OclBasic::TCollection_IteratorFactory(),  false ) ));
			return;
		}

		if ( strName == "ocl::Set" || strName == "Set" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Collection" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Set", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TSet_MethodFactory(),new OclBasic::TSet_IteratorFactory(),  false ) ));
			return;
		}

		if ( strName == "ocl::Sequence" || strName == "Sequence" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Collection" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Sequence", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TSequence_MethodFactory(),new OclBasic::TSequence_IteratorFactory(),  false ) ));
			return;
		}

		if ( strName == "ocl::Bag" || strName == "Bag" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Collection" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::Bag", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TBag_MethodFactory(),new OclBasic::TBag_IteratorFactory(),  false ) ));
			return;
		}

// -- ??
		if ( strName == "ocl::OrderedSet" || strName == "OrderedSet" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Set" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::CompoundType( "ocl::OrderedSet", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclBasic::TOrderedSet_MethodFactory(),new OclBasic::TOrderedSet_IteratorFactory(),  false ) ));
			return;
		}
// --
	}

	void TypeFactory::GetStereotypes( const std::string& strName, std::vector<unique_ptr<OclMeta::Type>>& vecTypes )
	{
		if ( strName == "gme::Object" || strName == "Object" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Object", vecSupers, new TObject_AttributeFactory(), new OclImplementation::AssociationFactory(), new TObject_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "gme::Folder" || strName == "Folder" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::Object" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Folder", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TFolder_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "gme::FCO" || strName == "FCO" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::Object" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::FCO", vecSupers, new TFCO_AttributeFactory(), new OclImplementation::AssociationFactory(), new TFCO_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "gme::Model" || strName == "Model" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::FCO" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Model", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TModel_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "gme::Atom" || strName == "Atom" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::FCO" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Atom", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new OclImplementation::MethodFactory(), false ) ));
			return;
		}

		if ( strName == "gme::Set" || strName == "Set" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::FCO" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Set", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TSet_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "gme::Reference" || strName == "Reference" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::FCO" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Reference", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TReference_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "gme::Connection" || strName == "Connection" ) {
			StringVector vecSupers;
			vecSupers.push_back( "gme::FCO" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Connection", vecSupers, new OclImplementation::AttributeFactory(), new OclImplementation::AssociationFactory(), new TConnection_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "gme::Project" || strName == "Project" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::Project", vecSupers, new TProject_AttributeFactory(), new OclImplementation::AssociationFactory(), new TProject_MethodFactory(), false ) ));
			return;
		}

		if ( strName == "gme::ConnectionPoint" || strName == "ConnPoint" || strName == "ConnectionPoint" ) {
			StringVector vecSupers;
			vecSupers.push_back( "ocl::Any" );
			vecTypes.push_back(unique_ptr<OclMeta::Type>( new OclMeta::Type( "gme::ConnectionPoint", vecSupers, new TConnectionPoint_AttributeFactory(), new OclImplementation::AssociationFactory(), new TConnectionPoint_MethodFactory(), false ) ));
			return;
		}
	}

	void TypeFactory::GetDynamicTypes( const std::string& strName, std::vector<unique_ptr<OclMeta::Type>>& vecTypes, std::string& strNameResult )
	{
		size_t jPos = strName.find( "gme::" );
		size_t kPos = strName.find( "ocl::" );
		if( jPos == std::string::npos             // neither 'gme::' 
		 && kPos == std::string::npos)            // nor     'ocl::' is found
		{
			// lookup in meta:
			size_t iPos = strName.find( "meta::");
			std::string strN = strName;
			std::string strNS;
			if( iPos != std::string::npos)        // if 'meta::' found
			{
				strNS = "meta";
				strN = strName.substr( iPos + 6); // let's remove 'meta::' from the name
			}

			// search for that kind in the Metaproject
			OclCommonEx::MetaBaseVector vecMetas;
			CComPtr<IMgaMetaProject> spMetaProject;
			COMTHROW( m_spProject->get_RootMeta( &spMetaProject ) );
			OclCommonEx::GetMetaObjects( spMetaProject, strN, OBJTYPE_NULL, vecMetas );
			
			// if found, use longer form
			if( vecMetas.size() > 0)
				strNameResult = "meta::" + strN;

			for ( unsigned int i = 0 ; i < vecMetas.size() ; i++ ) {
				objtype_enum eType = OclCommonEx::GetObjectType( vecMetas[ i ].p );
				std::string strSuperType = "gme::" + OclCommonEx::ObjectTypeToString( eType );
				std::string strType = "meta::" + OclCommonEx::GetObjectName( vecMetas[ i ].p );
				CComQIPtr<IMgaMetaFCO> spMetaFCO = vecMetas[ i ].p;
				vecTypes.push_back(unique_ptr<OclMeta::Type>(new OclMeta::Type( strType, StringVector( 1, strSuperType ), ( ! spMetaFCO.p ) ? new OclImplementation::AttributeFactory() : new TFCODerived_AttributeFactory( spMetaFCO.p ), new OclImplementation::AssociationFactory(), new OclImplementation::MethodFactory(), false ) ));
			}
		}
	}

}; // namespace OclGmeCM