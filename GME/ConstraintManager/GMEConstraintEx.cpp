//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	GMEConstraintEx.cpp
//
//###############################################################################################################################################
#include "stdafx.h"
#include "GMEConstraintEx.h"
#include "OCLCommonEx.h"

namespace OclGme
{
	typedef Ocl::Constraint OclConstraint;

//##############################################################################################################################################
//
//	C L A S S : OclGme::ConstraintBase <<< + Ocl::Constraint
//
//##############################################################################################################################################

	ConstraintBase::ConstraintBase()
		: OclConstraint()
	{
	}

	ConstraintBase::~ConstraintBase()
	{
	}

	ConstraintBase::Type ConstraintBase::GetType() const
	{
		return m_eType;
	}

	ConstraintBase::Location ConstraintBase::GetLocation() const
	{
		return m_eLocation;
	}

	void ConstraintBase::SetType( ConstraintBase::Type eType, const StringVector& vecLibraryPath )
	{
		m_eType = eType;
		switch( eType ) {
			case ConstraintBase::CT_CRITICAL_USER :
			case ConstraintBase::CT_NON_CRITICAL_USER :
				m_eLocation = ConstraintBase::CL_PROJECT; break;
			case ConstraintBase::CT_CRITICAL_LIBRARY :
			case ConstraintBase::CT_NON_CRITICAL_LIBRARY :
				m_eLocation = ConstraintBase::CL_LIBRARY; m_vecLibraryPath = vecLibraryPath; break;
			default :
				m_eLocation = ConstraintBase::CL_META; break;
		}
	}

	StringVector ConstraintBase::GetLibraryPath() const
	{
		return m_vecLibraryPath;
	}

	std::string ConstraintBase::GetFullName() const
	{
		return ( IsValid() ) ? OclConstraint::GetFullName() : GetDefinedName();
	}

	bool ConstraintBase::GetContextAndName( std::string& strContext, std::string& strName ) const
	{
		if ( IsValid() ) {
			strContext = GetContextType();
			strName = GetName();
			return true;
		}

		std::string strDName = GetDefinedName();
		size_t iPos = strDName.rfind( "::" );

		if ( iPos == std::string::npos ) {
			strContext = "?";
			strName = strDName;
			return false;
		}

		strContext = strDName.substr( 0, iPos );
		strName = strDName.substr( iPos + 2 );
		return false;
	}

//##############################################################################################################################################
//
//	C L A S S : OclGme::Constraint <<< + OclGme::ConstraintBase
//
//##############################################################################################################################################

	Constraint::Constraint( const std::string& strName, const std::string& strContextType, const std::string& strExpression, const std::string& strMessage, unsigned long ulEventMask, long lPriority, constraint_depth_enum eDepth, bool bFieldsMandatory )
		: ConstraintBase(), m_strMessage( strMessage ), m_ulEventMask( ulEventMask ), m_lPriority( lPriority ), m_eDepth( eDepth )
	{
		SetType( ( lPriority == 1 ) ? CT_CRITICAL_USER : CT_NON_CRITICAL_USER );

		m_strExpression = strExpression;
		Trim( m_strExpression );

		if ( m_strExpression.empty() && ! bFieldsMandatory )
			Define( strContextType + "::" + strName, "", true );
		else
			Define( strContextType + "::" + strName, "context " + strContextType + " inv " + strName + " :\r\n\r\n" + m_strExpression, true );
	}

	Constraint::Constraint( const std::string& strContextType, CComPtr<IMgaFCO> spFCO, bool bFieldsMandatory )
		: ConstraintBase()
	{
		m_spFCO = spFCO;

		COMTHROW( spFCO->get_IntAttrByName( CComBSTR( "ConstraintPriority" ), &m_lPriority ) );
		SetType( ( m_lPriority == 0 ) ? CT_CRITICAL_META : CT_NON_CRITICAL_META );

		// TODO : Obtain strMessage, ulEventMask, eDepth

		std::string strName = OclCommonEx::GetObjectName( spFCO.p );
		Trim( strName );

		CString strExpression;
		COMTHROW( spFCO->get_StrAttrByName( CComBSTR( "ConstraintEqn" ), PutOut( strExpression ) ) );
		m_strExpression = OclCommonEx::Convert( strExpression );
		Trim( m_strExpression );

		if ( m_strExpression.empty() && ! bFieldsMandatory )
			Define( strContextType + "::" + strName, "", true );
		else
			Define( strContextType + "::" + strName, "context " + strContextType + " inv " + strName + " :\r\n\r\n" + m_strExpression, true );
	}

	Constraint::Constraint( const std::string& strContextType, CComPtr<IMgaConstraint> spConstraint )
		: ConstraintBase()
	{
		m_spConstraint = spConstraint;

		COMTHROW( spConstraint->get_Priority( &m_lPriority ) );
		SetType( ( m_lPriority == 1 ) ? CT_CRITICAL_META : CT_NON_CRITICAL_META );

		CString strMessage;
		COMTHROW( spConstraint->get_DisplayedName( PutOut( strMessage ) ) );
		m_strMessage = OclCommonEx::Convert( strMessage );

		COMTHROW( spConstraint->get_Depth( &m_eDepth ) );
		COMTHROW( spConstraint->get_EventMask( &m_ulEventMask ) );

		CString strTemp;
		COMTHROW( spConstraint->get_Name( PutOut( strTemp ) ) );
		std::string strName = OclCommonEx::Convert( strTemp );
		Trim( strName );

		CString strExpression;
		COMTHROW( spConstraint->get_Expression( PutOut( strExpression ) ) );
		m_strExpression = OclCommonEx::Convert( strExpression );
		Trim( m_strExpression );

		Define( strContextType + "::" + strName, "context " + strContextType + " inv " + strName + " :\r\n\r\n" + m_strExpression, false );
	}

	Constraint::Constraint( CComPtr<IMgaRegNode> spRegNode, const StringVector& vecLibPath )
		: ConstraintBase()
	{
		m_spRegNode = spRegNode;

		CComPtr<IMgaRegNode> spNode;
		CString strTemp;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Priority" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		m_lPriority = _ttol( strTemp );
		if ( ! vecLibPath.empty() )
			SetType( ( m_lPriority == 1 ) ? CT_CRITICAL_LIBRARY : CT_NON_CRITICAL_LIBRARY, vecLibPath );
		else
			SetType( ( m_lPriority == 1 ) ? CT_CRITICAL_USER : CT_NON_CRITICAL_USER );

		strTemp.Empty();
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Description" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		m_strMessage = OclCommonEx::Convert( strTemp );

		strTemp.Empty();
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Depth" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		strTemp.TrimLeft(); strTemp.TrimRight();
		if ( strTemp == "0" )
			m_eDepth = CONSTRAINT_DEPTH_ZERO;
		else
			if ( strTemp == "1" )
				m_eDepth = CONSTRAINT_DEPTH_ONE;
			else
				m_eDepth = CONSTRAINT_DEPTH_ANY;

		strTemp.Empty();
		spNode = NULL;


		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "EventMask" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		TCHAR* pchStop;
		m_ulEventMask = _tcstoul( strTemp, &pchStop, 10 );

		strTemp.Empty();
		spNode = NULL;

		COMTHROW( spRegNode->get_Name( PutOut( strTemp ) ) );
		int iPos = strTemp.ReverseFind( ':' );
		std::string strName = OclCommonEx::Convert( strTemp.Right( strTemp.GetLength() - iPos - 1 ) );
		std::string strContextType = OclCommonEx::Convert( strTemp.Left( iPos - 1 ) );
		Trim( strName );
		Trim( strContextType );

		strTemp.Empty();

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Expression" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		m_strExpression = OclCommonEx::Convert( strTemp );
		Trim( m_strExpression );

		Define( strContextType + "::" + strName, "context " + strContextType + " inv " + strName + " :\r\n\r\n" + m_strExpression, true );
	}

	Constraint::~Constraint()
	{
	}

	unsigned long Constraint::GetEventMask() const
	{
		return m_ulEventMask;
	}

	long Constraint::GetPriority() const
	{
		return m_lPriority;
	}

	constraint_depth_enum Constraint::GetDepth() const
	{
		return m_eDepth;
	}

	std::string Constraint::GetMessage() const
	{
		return m_strMessage;
	}

	std::string Constraint::GetExpression() const
	{
		return m_strExpression;
	}

	OclConstraint::State Constraint::Write( CComPtr<IMgaRegNode> spRegNode )
	{
		if ( GetState() < OclConstraint::CS_CHECK_SUCCEEDED )
			return GetState();

		// COMTHROW( spRegNode->put_Name( CComBSTR( OclCommonEx::Convert( GetFullName() ) ) ) );

		CComPtr<IMgaRegNode> spNode;
		CString strTemp;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( L"EventMask" ), &spNode ) );
		strTemp.Format( _T("%d"), m_ulEventMask );
		COMTHROW( spNode->put_Value( CComBSTR( strTemp ) ) );
		strTemp.Empty();
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( L"Description" ), &spNode ) );
		COMTHROW( spNode->put_Value( CComBSTR( OclCommonEx::Convert( m_strMessage ) ) ) );
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( L"Priority" ), &spNode ) );
		strTemp.Format( _T("%d"), m_lPriority );
		COMTHROW( spNode->put_Value( CComBSTR( strTemp ) ) );
		strTemp.Empty();
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Depth" ), &spNode ) );
		if ( m_eDepth == CONSTRAINT_DEPTH_ZERO )
			strTemp = "0";
		else
			if ( m_eDepth == CONSTRAINT_DEPTH_ONE )
				strTemp = "1";
			else
				strTemp = "any";
		COMTHROW( spNode->put_Value( CComBSTR( strTemp ) ) );
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Expression" ), &spNode ) );
		COMTHROW( spNode->put_Value( CComBSTR( OclCommonEx::Convert( m_strExpression ) ) ) );

		return GetState();
	}

	Constraint::EnableInfo Constraint::GetEnableInfo( CComPtr<IMgaObject> spObject ) const
	{
		// Check if constraint is valid.

		if ( GetState() < OclConstraint::CS_CHECK_SUCCEEDED )
			return Constraint::CE_NONE;

		// Check if context kind is the appropriate.

		if ( ! ( "meta::" + OclCommonEx::GetObjectKind( spObject ) == GetContextType() ) )
			return Constraint::CE_NONE;

		// Check if object is to be deleted.

		long lStatus;
		COMTHROW( spObject->get_Status( &lStatus ) );
		if ( lStatus != OBJECT_EXISTS )
			return Constraint::CE_ENABLED;

		// Obtain library path of the object

		StringVector vecObjectPath = OclCommonEx::GetLibraryPath( spObject );

		// In case of Library constraint check if object is in the effect range of library (in the library itself or in the project)

		if ( GetLocation() == ConstraintBase::CL_LIBRARY ) {
			StringVector vecConstraintPath = GetLibraryPath();
			for ( unsigned int i = 0 ; i < min( vecConstraintPath.size(), vecObjectPath.size() ) ; i++ )
				if ( vecObjectPath[ i ] != vecConstraintPath[ i ] )
					return Constraint::CE_NONE;
			if ( ! vecObjectPath.empty() && vecObjectPath.size() < vecConstraintPath.size() )
				return Constraint::CE_NONE;
		}
		if ( GetLocation() == ConstraintBase::CL_PROJECT && ! vecObjectPath.empty() )
			return Constraint::CE_NONE;

		// Check if constraint defined in the paradigm and has high priority

		if ( GetLocation() == ConstraintBase::CL_META && GetPriority() == 1 )
			return Constraint::CE_ENABLED_READONLY;

		// Obtain the flag of the object.

		CComQIPtr<IMgaFCO> spFCO = spObject;
		CComQIPtr<IMgaFolder> spFolder = spObject;
		CComPtr<IMgaRegNode> spRegNode;
		if ( spFCO.p )
			COMTHROW( spFCO->get_RegistryNode( CComBSTR( OclCommonEx::Convert( "ConstraintEnabling/" + GetFullName() ) ), &spRegNode ) );
		else
			COMTHROW( spFolder->get_RegistryNode( CComBSTR( OclCommonEx::Convert( "ConstraintEnabling/" + GetFullName() ) ), &spRegNode ) );

		CString strEnable;
		COMTHROW( spRegNode->get_Value( PutOut( strEnable ) ) );
		strEnable.MakeLower();

		// If the object is in a library

		if ( ! vecObjectPath.empty() )
			return ( strEnable == "yes" || strEnable.IsEmpty() && GetLocation() == ConstraintBase::CL_META ) ? Constraint::CE_ENABLED_READONLY : Constraint::CE_DISABLED_READONLY;

		// If the constraint is defined in a library and it has high priority

		if ( strEnable == "yes" && GetLocation() == ConstraintBase::CL_LIBRARY && GetPriority() == 1 )
			return Constraint::CE_ENABLED_READONLY;

		// Obtain the inheritance flag

		long lFlagStatus;
		COMTHROW( spRegNode->get_Status( &lFlagStatus ) );

		if ( lFlagStatus == 0 )
			return ( strEnable == "yes" || strEnable.IsEmpty() && GetLocation() == ConstraintBase::CL_META ) ? Constraint::CE_ENABLED : Constraint::CE_DISABLED;
		else
			return ( strEnable == "yes" || strEnable.IsEmpty() && GetLocation() == ConstraintBase::CL_META ) ? Constraint::CE_ENABLED_INHERITED : Constraint::CE_DISABLED_INHERITED;
	}

//##############################################################################################################################################
//
//	C L A S S : OclGme::ConstraintFunction <<< + OclGme::ConstraintBase
//
//##############################################################################################################################################

	ConstraintFunction::ConstraintFunction( const std::string& strName, const std::string& strContextType, const std::string& strParameterList, const std::string& strReturnType, bool bIsAttribute, const std::string& strExpression, bool bFieldsMandatory )
		: ConstraintBase()
	{
		CString strTemp = OclCommonEx::Convert( strParameterList );
		strTemp.Replace( _T(","), _T(";") );
		m_strParameterList = OclCommonEx::Convert( strTemp );
		Trim( m_strParameterList );

		m_strExpression = strExpression;
		Trim( m_strExpression );

		SetType( ( bIsAttribute ) ? ConstraintBase::CT_ATTRIBUTE_USER : ConstraintBase::CT_METHOD_USER );
		std::string strStereotype = ( bIsAttribute ) ? " defattribute " : " defmethod ";

		Define( strContextType + "::" + strName, "context " + strContextType + "::" + strName + "( " + m_strParameterList + " ) : " + strReturnType + strStereotype + strName + " :\r\n\r\n" + m_strExpression );
	}

	ConstraintFunction::ConstraintFunction( CComPtr<IMgaFCO> spFCO, bool bFieldsMandatory )
		: ConstraintBase()
	{
		m_spFCO = spFCO;

		CString strParameterList;
		COMTHROW( spFCO->get_StrAttrByName( CComBSTR( L"CFuncParamList" ), PutOut( strParameterList ) ) );
		strParameterList.Replace( _T(","), _T(";") );
		m_strParameterList = OclCommonEx::Convert( strParameterList );
		Trim( m_strParameterList );

		CString strExpression;
		COMTHROW( spFCO->get_StrAttrByName( CComBSTR( L"CFuncDefinition" ), PutOut( strExpression ) ) );
		m_strExpression = OclCommonEx::Convert( strExpression );
		Trim( m_strExpression );

		CString strAttrStereotype;
		COMTHROW( spFCO->get_StrAttrByName( CComBSTR( L"CFuncStereotype" ), PutOut( strAttrStereotype ) ) );
		SetType( ( strAttrStereotype == _T("attribute") ) ? ConstraintBase::CT_ATTRIBUTE_META : ConstraintBase::CT_METHOD_META );
		std::string strStereotype = " def" + OclCommonEx::Convert( strAttrStereotype ) + " ";

		std::string strName = OclCommonEx::GetObjectName( spFCO.p );

		CString strAttrContextType;
		COMTHROW( spFCO->get_StrAttrByName( CComBSTR( "CFuncContext" ), PutOut( strAttrContextType ) ) );
		std::string strContextType = OclCommonEx::Convert( strAttrContextType );
		Trim( strContextType );

		CString strAttrReturnType;
		COMTHROW( spFCO->get_StrAttrByName( CComBSTR( "CFuncReturnType" ), PutOut( strAttrReturnType ) ) );
		std::string strReturnType = OclCommonEx::Convert( strAttrReturnType );

		Define( strContextType + "::" + strName, "context " + strContextType + "::" + strName + "( " + m_strParameterList + " ) : " + strReturnType + strStereotype + strName + " :\r\n\r\n" + m_strExpression );
	}

	ConstraintFunction::ConstraintFunction( CComPtr<IMgaConstraint> spConstraint )
		: ConstraintBase()
	{
		m_spConstraint = spConstraint;

		CString strCFName;
		COMTHROW( spConstraint->get_Name( PutOut( strCFName ) ) );
		if ( strCFName.IsEmpty() )
			ReadFromOldFormat();
		else
			ReadFromNewFormat();
	}

	void ConstraintFunction::ReadFromOldFormat()
	{
		SetType( ConstraintBase::CT_METHOD_META );
		std::string strStereotype = " defmethod ";
		std::string strContextType = "gme::Project";
		std::string strReturnType = "ocl::Boolean";
		std::string strName;

		CString strFunctionData;
		CString strTemp;
		COMTHROW( m_spConstraint->get_Expression( PutOut( strFunctionData ) ) );
		strFunctionData.TrimLeft();
		if ( strFunctionData.GetLength() > 8 ) {
			strFunctionData = strFunctionData.Right( strFunctionData.GetLength() - 8 );
			strFunctionData.TrimLeft();

			int iPos = strFunctionData.Find( _T("(") );
			if ( iPos != -1 ) {
				strTemp = strFunctionData.Left( iPos );
				strTemp.TrimLeft(); strTemp.TrimRight();
				strName = OclCommonEx::Convert( strTemp );

				strFunctionData = strFunctionData.Right( strFunctionData.GetLength() - iPos - 1 );
				iPos = strFunctionData.Find( _T(")") );
				if ( iPos != -1 ) {
					strTemp = strFunctionData.Left( iPos );
					strTemp.TrimLeft(); strTemp.TrimRight();
					strTemp.Replace( _T(","), _T(";") );
					m_strParameterList = OclCommonEx::Convert( strTemp );

					strFunctionData = strFunctionData.Right( strFunctionData.GetLength() - iPos - 1 );
					m_strExpression = OclCommonEx::Convert( strFunctionData );
					Trim( m_strExpression );
				}
			}
		}
		Define( strContextType + "::" + strName, "context " + strContextType + "::" + strName + "( " + m_strParameterList + " ) : " + strReturnType + strStereotype + strName + " :\r\n\r\n" + m_strExpression );
	}

	void ConstraintFunction::ReadFromNewFormat()
	{
		CString strCFName;
		COMTHROW( m_spConstraint->get_Name( PutOut( strCFName ) ) );
		std::string strName = OclCommonEx::Convert( strCFName );
		Trim( strName );

		CString strFunctionData;
		CString strTemp;
		COMTHROW( m_spConstraint->get_Expression( PutOut( strFunctionData ) ) );

		std::string strStereotype = " defmethod ";
		int iPos = strFunctionData.Find( ';' );
		if ( iPos != -1 ) {
			strTemp = strFunctionData.Left( iPos );
			strTemp.TrimLeft(); strTemp.TrimRight();
			strStereotype = OclCommonEx::Convert( strTemp );
			if ( strStereotype == "attribute" )
				strStereotype = " defattribute ";
			else
				strStereotype = " defmethod ";
			strFunctionData = strFunctionData.Right( strFunctionData.GetLength() - iPos - 1 );
		}
		SetType( ( strStereotype == " defattribute " ) ? ConstraintBase::CT_ATTRIBUTE_META : ConstraintBase::CT_METHOD_META );

		std::string strContextType = "gme::Project";
		iPos = strFunctionData.Find( ';' );
		if ( iPos != -1 ) {
			strTemp = strFunctionData.Left( iPos );
			strTemp.TrimLeft(); strTemp.TrimRight();
			strContextType = OclCommonEx::Convert( strTemp );
			Trim( strContextType );
			strFunctionData = strFunctionData.Right( strFunctionData.GetLength() - iPos - 1 );
		}

		iPos = strFunctionData.Find( _T(';') );
		if ( iPos != -1 ) {
			strTemp = strFunctionData.Left( iPos );
			strTemp.TrimLeft(); strTemp.TrimRight();
			strTemp.Replace( _T(","), _T(";") );
			m_strParameterList = OclCommonEx::Convert( strTemp );
			strFunctionData = strFunctionData.Right( strFunctionData.GetLength() - iPos - 1 );
		}

		std::string strReturnType = "ocl::Boolean";
		iPos = strFunctionData.Find( ';' );
		if ( iPos != -1 ) {
			strTemp = strFunctionData.Left( iPos );
			strTemp.TrimLeft(); strTemp.TrimRight();
			strReturnType = OclCommonEx::Convert( strTemp );
			strFunctionData = strFunctionData.Right( strFunctionData.GetLength() - iPos - 1 );
		}

		strFunctionData.TrimLeft(); strFunctionData.TrimRight();
		m_strExpression = OclCommonEx::Convert( strFunctionData );

		Define( strContextType + "::" + strName, "context " + strContextType + "::" + strName + "( " + m_strParameterList + " ) : " + strReturnType + strStereotype + strName + " :\r\n\r\n" + m_strExpression );
	}

	ConstraintFunction::ConstraintFunction( CComPtr<IMgaRegNode> spRegNode )
		: ConstraintBase()
	{
		m_spRegNode = spRegNode;

		CComPtr<IMgaRegNode> spNode;
		CString strTemp;

		COMTHROW( spRegNode->get_Name( PutOut( strTemp ) ) );
		std::string strName = OclCommonEx::Convert( strTemp );
		Trim( strName );

		strTemp.Empty();

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Context" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		std::string strContextType = OclCommonEx::Convert( strTemp );
		Trim( strContextType );

		strTemp.Empty();
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Stereotype" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		std::string strStereotype = OclCommonEx::Convert( strTemp );
		Trim( strStereotype );
		if ( strStereotype == "attribute" )
			strStereotype = " defattribute ";
		else
			strStereotype = " defmethod ";
		SetType( ( strStereotype == " defattribute " ) ? ConstraintBase::CT_ATTRIBUTE_USER : ConstraintBase::CT_METHOD_USER );

		strTemp.Empty();
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "ParameterList" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		m_strParameterList = OclCommonEx::Convert( strTemp );
		Trim( m_strParameterList );

		strTemp.Empty();
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "ReturnType" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		std::string strReturnType = OclCommonEx::Convert( strTemp );
		Trim( strReturnType );

		strTemp.Empty();
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Expression" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		m_strExpression = OclCommonEx::Convert( strTemp );
		Trim( m_strExpression );

		Define( strContextType + "::" + strName, "context " + strContextType + "::" + strName + "( " + m_strParameterList + " ) : " + strReturnType + strStereotype + strName + " :\r\n\r\n" + m_strExpression, true );
	}

	std::string ConstraintFunction::GetParameterList() const
	{
		return m_strParameterList;
	}

	std::string ConstraintFunction::GetExpression() const
	{
		return m_strExpression;
	}

	OclConstraint::State ConstraintFunction::Write( CComPtr<IMgaRegNode> spRegNode )
	{
		if ( GetState() < OclConstraint::CS_CHECK_SUCCEEDED )
			return GetState();

		// COMTHROW( spRegNode->put_Name( CComBSTR( OclCommonEx::Convert( GetName() ) ) ) );

		CComPtr<IMgaRegNode> spNode;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Context" ), &spNode ) );
		COMTHROW( spNode->put_Value( CComBSTR( OclCommonEx::Convert( GetContextType() ) ) ) );
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Stereotype" ), &spNode ) );
		COMTHROW( spNode->put_Value( CComBSTR( ( GetType() == ConstraintBase::CT_ATTRIBUTE_USER ) ? "attribute" : "method" ) ) );
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "ParameterList" ), &spNode ) );
		COMTHROW( spNode->put_Value( CComBSTR( OclCommonEx::Convert( m_strParameterList ) ) ) );
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "ReturnType" ), &spNode ) );
		COMTHROW( spNode->put_Value( CComBSTR( OclCommonEx::Convert( GetReturnType() ) ) ) );
		spNode = NULL;

		COMTHROW( spRegNode->get_SubNodeByName( CComBSTR( "Expression" ), &spNode ) );
		COMTHROW( spNode->put_Value( CComBSTR( OclCommonEx::Convert( m_strExpression ) ) ) );

		return GetState();
	}

}; // namespace OclGme