//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLGMECMFacade.cpp
//
//###############################################################################################################################################
#include "Solve4786.h"
#include "OCLGMECMFacade.h"
#include "GMEViolationDialog.h"
#include "GMESmallMessageBox.h"
#include "OCLTypeExGMECM.h"
#include "OCLCommonEx.h"
#include "OCLObjectExBasic.h"
#include "OCLObjectExGME.h"
#include <algorithm>

namespace OclGmeCM
{

//##############################################################################################################################################
//
//	C L A S S : OclGmeCM::ConstraintDefinitionFactory <<< + OclImplementation::ConstraintDefinitionFactory
//
//==============================================================================================================================================
//
//	D E S C R I P T I O N :
//
//##############################################################################################################################################

	class ConstraintAttribute
		: public OclImplementation::Attribute
	{
		private :
			OclGme::SpConstraintFunction 	m_spFunction;
			CComPtr<IMgaProject> 			m_spProject;

		public :
			ConstraintAttribute( CComPtr<IMgaProject> spProject, OclGme::SpConstraintFunction spFunction )
				: m_spFunction( spFunction ), m_spProject( spProject )
			{
			}

			void operator()()
			{
				if ( ! m_spFunction.Ptr() || m_spFunction->GetState() < Ocl::Constraint::CS_CHECK_DEPENDENCY_SUCCEEDED )
					ThrowException( "Constraint attribute definition cannot be evaluated." );
				OclTree::ObjectContextStack context;
				context.AddVariable( "project", CREATE_GMEPROJECT( GetTypeManager(), m_spProject ) );
				context.AddVariable( "self", GetThis() );
				SetResult( m_spFunction->Evaluate( context ) );
			}
	};

	class ConstraintMethod
		: public OclImplementation::Method
	{
		private :
			OclGme::SpConstraintFunction 	m_spFunction;
			CComPtr<IMgaProject> 			m_spProject;

		public :
			ConstraintMethod( CComPtr<IMgaProject> spProject, OclGme::SpConstraintFunction spFunction )
				: m_spFunction( spFunction ), m_spProject( spProject )
			{
			}

			OclTree::ViolationVector GetViolations() 
			{
				return m_spFunction->GetViolations();
			}

			void ClearViolations()
			{
				m_spFunction->ClearViolations();
			}


			void operator()()
			{

				if ( ! m_spFunction.Ptr() || m_spFunction->GetState() < Ocl::Constraint::CS_CHECK_DEPENDENCY_SUCCEEDED )
					ThrowException( "Constraint method definition cannot be evaluated." );
				OclTree::ObjectContextStack context;
				context.AddVariable( "project", CREATE_GMEPROJECT( GetTypeManager(), m_spProject ) );
				context.AddVariable( "self", GetThis() );
				OclCommon::FormalParameterVector vecParameters = m_spFunction->GetFormalParameters();
				for ( unsigned int i = 0 ; i < vecParameters.size() ; i++ )
					context.AddVariable( vecParameters[ i ].GetName(), ( (int) i >= GetArgumentCount() ) ? OclMeta::Object::UNDEFINED : GetArgument( i ) );
				SetResult( m_spFunction->Evaluate( context ) );
			}
	};

	class ConstraintDefinitionFactory
		: public OclImplementation::ConstraintDefinitionFactory
	{
		public :
			Facade* m_pFacade;

			virtual void GetFeatures( const OclSignature::Attribute& signature, OclMeta::AttributeVector& vecFeatures )
			{
				OclGme::ConstraintFunctionVector vecFunctions = m_pFacade->m_vecMetaConstraintFunctions;
				for ( unsigned int i = 0 ; i < vecFunctions.size() ; i++ ) {
					if ( vecFunctions[ i ]->IsValid() && m_pFacade->GetTreeManager()->GetTypeManager()->IsTypeA( signature.GetTypeName(), vecFunctions[ i ]->GetContextType() ) >= 0 )
						if ( vecFunctions[ i ]->GetName() == signature.GetName() && vecFunctions[ i ]->GetStereotype() == Ocl::Constraint::CS_ATTRIBUTEDEF )
							vecFeatures.push_back( new OclMeta::Attribute( signature.GetName(), CreateReturnType( vecFunctions[ i ]->GetReturnType() ), new ConstraintAttribute( m_pFacade->GetProject(), vecFunctions[ i ] ), true, true ) );
				}
				vecFunctions = m_pFacade->m_vecUserConstraintFunctions;
				for (  unsigned int i = 0 ; i < vecFunctions.size() ; i++ ) {
					if ( vecFunctions[ i ]->IsValid() && m_pFacade->GetTreeManager()->GetTypeManager()->IsTypeA( signature.GetTypeName(), vecFunctions[ i ]->GetContextType() ) >= 0 )
						if ( vecFunctions[ i ]->GetName() == signature.GetName() && vecFunctions[ i ]->GetStereotype() == Ocl::Constraint::CS_ATTRIBUTEDEF )
							vecFeatures.push_back( new OclMeta::Attribute( signature.GetName(), CreateReturnType( vecFunctions[ i ]->GetReturnType() ), new ConstraintAttribute( m_pFacade->GetProject(), vecFunctions[ i ] ), true, true ) );
				}
			}

			virtual void GetFeatures( const OclSignature::Method& signature, OclMeta::MethodVector& vecFeatures )
			{
				OclGme::ConstraintFunctionVector vecFunctions = m_pFacade->m_vecMetaConstraintFunctions;
				for ( unsigned int i = 0 ; i < vecFunctions.size() ; i++ ) {
					if ( vecFunctions[ i ]->IsValid() && m_pFacade->GetTreeManager()->GetTypeManager()->IsTypeA( signature.GetTypeName(), vecFunctions[ i ]->GetContextType() ) >= 0 )
						if ( vecFunctions[ i ]->GetName() == signature.GetName() && vecFunctions[ i ]->GetStereotype() == Ocl::Constraint::CS_METHODDEF && (int) vecFunctions[ i ]->GetFormalParameters().size() == signature.GetParameterCount() )
							vecFeatures.push_back( new OclMeta::Method( signature.GetName(), CreateFormalParameters( vecFunctions[ i ]->GetFormalParameters() ), CreateReturnType( vecFunctions[ i ]->GetReturnType() ), new ConstraintMethod( m_pFacade->GetProject(), vecFunctions[ i ] ), true, true ) );
				}
				vecFunctions = m_pFacade->m_vecUserConstraintFunctions;
				for ( unsigned int i = 0 ; i < vecFunctions.size() ; i++ ) {
					if ( vecFunctions[ i ]->IsValid() && m_pFacade->GetTreeManager()->GetTypeManager()->IsTypeA( signature.GetTypeName(), vecFunctions[ i ]->GetContextType() ) >= 0 )
						if ( vecFunctions[ i ]->GetName() == signature.GetName() && vecFunctions[ i ]->GetStereotype() == Ocl::Constraint::CS_ATTRIBUTEDEF )
							vecFeatures.push_back( new OclMeta::Method( signature.GetName(), CreateFormalParameters( vecFunctions[ i ]->GetFormalParameters() ), CreateReturnType( vecFunctions[ i ]->GetReturnType() ), new ConstraintMethod( m_pFacade->GetProject(), vecFunctions[ i ] ), true, true ) );
				}
			}

		private :
			TypeSeq CreateReturnType( const std::string& strType )
			{
				TypeSeq vecType;
				OclCommon::Convert( strType, vecType );
				return vecType;
			}

			OclCommon::FormalParameterVector CreateFormalParameters( const OclCommon::FormalParameterVector& vecParamsIn ) const
			{
				OclCommon::FormalParameterVector vecParams;
				for ( unsigned int j = 0 ; j < vecParamsIn.size() ; j++ ) {
					TypeSeq vecType;
					OclCommon::Convert( vecParamsIn[ j ].GetTypeName(), vecType );
					vecParams.push_back( OclCommon::FormalParameter( vecParamsIn[ j ].GetName(), vecType[ 0 ], true ) );
				}
				return vecParams;
			}
	};

//##############################################################################################################################################
//
//	C L A S S : OclGmeCM::Facade
//
//##############################################################################################################################################

	// Initialization Finalization

	Facade::Facade()
		: m_spProject( NULL ), m_pTreeManager( NULL ), m_bViolationDlgExpanded( false )
	{
	}

	Facade::~Facade()
	{
		Finalize();
		if ( m_pTreeManager )
			delete m_pTreeManager;
	}

	CComPtr<IMgaProject> Facade::GetProject() const
	{
		return m_spProject;
	}

	CComPtr<IMgaMetaProject> Facade::GetMetaProject() const
	{
		CComPtr<IMgaMetaProject> spMetaProject;
		COMTHROW( m_spProject->get_RootMeta( &spMetaProject ) );
		return spMetaProject;
	}

	OclTree::TreeManager* Facade::GetTreeManager() const
	{
		return m_pTreeManager;
	}

	void Facade::Initialize( CComPtr<IMgaProject> spProject )
	{
		if ( m_pTreeManager )
			delete m_pTreeManager;

		m_spProject = spProject;

		m_bEnabled = true;
		m_bEnabledEvents = true;
		m_bEnabledInteractions = true;

		// Create Important Objects

		ConstraintDefinitionFactory* pCDFactory = new ConstraintDefinitionFactory();
		pCDFactory->m_pFacade = this;
		OclMeta::TypeManager* pTypeManager = new OclMeta::TypeManager( new TypeFactory( spProject ), new OclBasic::OperatorFactory(), new OclBasic::FunctionFactory(), pCDFactory );

		m_pTreeManager = new OclTree::TreeManager( pTypeManager, new OclTree::ObjectNodeAdaptor(), new OclTree::CollectionNodeAdaptor() );

		// Load Settings

		CComPtr<IMgaFolder> spRootFolder;
		COMTHROW( m_spProject->get_RootFolder( &spRootFolder ) );
		CComPtr<IMgaRegNode> spSettings;
		COMTHROW( spRootFolder->get_RegistryNode( CComBSTR( "ConstraintManagerSettings" ), &spSettings ) );

		CComPtr<IMgaRegNode> spNode;
		CString strTemp;

		COMTHROW( spSettings->get_SubNodeByName( CComBSTR( "ShortCircuitLogicalOperators" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		m_infoEvaluation.bEnabledSCLogical = strTemp.IsEmpty() || strTemp == "yes";
		spNode = NULL;
		strTemp.Empty();

		COMTHROW( spSettings->get_SubNodeByName( CComBSTR( "ShortCircuitIterators" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		m_infoEvaluation.bEnabledSCIterator = strTemp.IsEmpty() || strTemp == "yes";
		spNode = NULL;
		strTemp.Empty();

		COMTHROW( spSettings->get_SubNodeByName( CComBSTR( "Tracking" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		m_infoEvaluation.bEnabledTracking = strTemp.IsEmpty() || strTemp == "yes";
		spNode = NULL;
		strTemp.Empty();

		COMTHROW( spSettings->get_SubNodeByName( CComBSTR( "ViolationCount" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		if ( strTemp.IsEmpty() )
			m_infoEvaluation.iViolationCount = -2;
		else {
			m_infoEvaluation.iViolationCount = _ttoi( strTemp );
			if ( m_infoEvaluation.iViolationCount == 0 || m_infoEvaluation.iViolationCount < -2 || m_infoEvaluation.iViolationCount > 999 )
				m_infoEvaluation.iViolationCount = -2;
		}
		spNode = NULL;
		strTemp.Empty();

		COMTHROW( spSettings->get_SubNodeByName( CComBSTR( "ModelDepth" ), &spNode ) );
		COMTHROW( spNode->get_Value( PutOut( strTemp ) ) );
		if ( strTemp.IsEmpty() )
			m_infoEvaluation.iModelDepth = 0; // 1; // terge
		else {
			m_infoEvaluation.iModelDepth = _ttoi( strTemp );
			if ( m_infoEvaluation.iModelDepth < -1 || m_infoEvaluation.iModelDepth > 1 )
				m_infoEvaluation.iModelDepth = -1;
		}
	}

	void Facade::Finalize()
	{
	}

	EvaluationInfo	Facade::GetEvaluationInfo() const
	{
		return m_infoEvaluation;
	}

	void Facade::SetEvaluationInfo( const EvaluationInfo& info )
	{
		m_infoEvaluation = info;

		CComPtr<IMgaFolder> spRootFolder;
		COMTHROW( m_spProject->get_RootFolder( &spRootFolder ) );
		CComPtr<IMgaRegNode> spRootConstraints;
		CString strTemp;

		strTemp = ( info.bEnabledSCLogical ) ? "yes" : "no";
		COMTHROW( spRootFolder->put_RegistryValue( CComBSTR( "ConstraintManagerSettings/ShortCircuitLogicalOperators" ), CComBSTR( strTemp ) ) );

		strTemp = ( info.bEnabledSCIterator ) ? "yes" : "no";
		COMTHROW( spRootFolder->put_RegistryValue( CComBSTR( "ConstraintManagerSettings/ShortCircuitIterators" ), CComBSTR( strTemp ) ) );

		strTemp = ( info.bEnabledTracking ) ? "yes" : "no";
		COMTHROW( spRootFolder->put_RegistryValue( CComBSTR( "ConstraintManagerSettings/Tracking" ), CComBSTR( strTemp ) ) );

		strTemp.Empty(); strTemp.Format( _T("%d"), info.iViolationCount );
		COMTHROW( spRootFolder->put_RegistryValue( CComBSTR( "ConstraintManagerSettings/ViolationCount" ), CComBSTR( strTemp ) ) );

		strTemp.Empty(); strTemp.Format( _T("%d"), info.iModelDepth );
		COMTHROW( spRootFolder->put_RegistryValue( CComBSTR( "ConstraintManagerSettings/ModelDepth" ), CComBSTR( strTemp ) ) );
	}

	void Facade::LoadMetaConstraintFunctions( OclGme::ConstraintFunctionVector& vecFaileds )
	{
		// Load Meta Constraint Functions

		OclGme::ConstraintFunctionVector vecFounds;

		CComPtr<IMgaMetaFolder> spMetaFolder;
		COMTHROW( GetMetaProject()->get_RootFolder( &spMetaFolder ) );
		CComPtr<IMgaConstraints> spConstraints;
		COMTHROW( spMetaFolder->get_Constraints( &spConstraints ) );

		MGACOLL_ITERATE( IMgaConstraint, spConstraints ) {
			CString strNmspc;
			COMTHROW( MGACOLL_ITER->GetDefinedForNamespace( PutOut( strNmspc)));
			constraint_type_enum eType;
			COMTHROW( MGACOLL_ITER->get_Type( &eType ) );
			if ( eType == CONSTRAINT_TYPE_FUNCTION ) {
				OclGme::SpConstraintFunction spConstraintFunction( new OclGme::ConstraintFunction( MGACOLL_ITER ) );
				spConstraintFunction->SetNamespace( std::string(CStringA(strNmspc)));
				spConstraintFunction->Register( m_pTreeManager );
				vecFounds.push_back( spConstraintFunction );
			}
		} MGACOLL_ITERATE_END;

		LoadConstraintFunctions( vecFounds, vecFaileds, m_vecMetaConstraintFunctions );
		m_vecMetaConstraintFunctions = vecFounds;
	}

	void Facade::LoadUserConstraintFunctions( OclGme::ConstraintFunctionVector& vecFaileds )
	{
		// Load User Constraints

		OclGme::ConstraintFunctionVector vecFounds;

		LoadConstraintFunctions( vecFounds, vecFaileds, m_vecUserConstraintFunctions );
		m_vecUserConstraintFunctions = vecFounds;
	}

	typedef std::vector< OclMeta::DependencySet > DependencySetVector;

	void Facade::LoadConstraintFunctions( OclGme::ConstraintFunctionVector& vecFounds, OclGme::ConstraintFunctionVector& vecFaileds, OclGme::ConstraintFunctionVector& vecSucceededs )
	{
		// Parse, ParseContext

		for ( unsigned int i = 0 ; i < vecFounds.size() ; i++ ) {
			Ocl::Constraint::State eState = vecFounds[ i ]->Parse();
			if ( eState == Ocl::Constraint::CS_PARSE_SUCCEEDED ) {
				vecFounds[ i ]->ParseContext();
				eState = vecFounds[ i ]->CheckContext();
				if ( eState == Ocl::Constraint::CS_CTX_CHECK_SUCCEEDED )
					vecSucceededs.push_back( vecFounds[ i ] );
				else
					vecFaileds.push_back( vecFounds[ i ] );
			}
			else
				vecFaileds.push_back( vecFounds[ i ] );
		}

		// Check All

		OclGme::SpConstraintFunction spC;
		for ( unsigned int i = 0 ; i < vecSucceededs.size() ; i ++ ) {
			OclTree::TypeContextStack context;
			context.AddVariable( "project", TypeSeq( 1, "gme::Project" ) );
			spC = vecSucceededs[ i ];
			if ( vecSucceededs[ i ]->Check( context ) != Ocl::Constraint::CS_CHECK_SUCCEEDED )
				vecFaileds.push_back( vecSucceededs[ i ] );
		}

		// Sort only succeeded so far

		vecFounds = vecSucceededs;
		vecSucceededs.clear();
		DependencySetVector vecDependencySets;

		for ( unsigned int i = 0 ; i < vecFounds.size() ; i ++ )
			if ( vecFounds[ i ]->GetState() == Ocl::Constraint::CS_CHECK_SUCCEEDED ) {
				vecSucceededs.push_back( vecFounds[ i ] );
				vecDependencySets.push_back( vecFounds[ i ]->GetDependencySet() );
			}

		// Collect all Error Dependencies

		StringVector vecErrors;
		for ( unsigned int i = 0 ; i < vecFaileds.size() ; i++ ) {
			std::string strSignature;
			try {
				strSignature = GetSignature( vecFaileds[ i ] );
			}
			catch ( ... ) {
				strSignature = "#";
			}
			vecErrors.push_back( strSignature );
		}

		for (  unsigned int i = 0 ; i < vecErrors.size() ; i++ ) {
			for ( unsigned int j = 0 ; j < vecDependencySets.size() ; j++ ) {
				OclMeta::DependencySet::iterator it = OclMeta::Dependency::LookUp( vecDependencySets[ j ], vecErrors[ i ] );
				if ( it != vecDependencySets[ j ].end() ) {
					OclMeta::Dependency::SetChecked( vecDependencySets[ j ], it, true );
					std::string strSignature = GetSignature( vecSucceededs[ j ] );
					if ( std::find( vecErrors.begin(), vecErrors.end(), strSignature ) == vecErrors.end() )
						vecErrors.push_back( strSignature );
				}
			}
		}

		// Set result dependencies

		for ( unsigned int i = 0 ; i < vecSucceededs.size() ; i++ )
			if ( vecSucceededs[ i ]->SetDependencyResult( vecDependencySets[ i ] ) == Ocl::Constraint::CS_CHECK_DEPENDENCY_FAILED )
				vecFaileds.push_back( vecSucceededs[ i ] );

		// Sort only succeeded

		vecFounds = vecSucceededs;
		vecSucceededs.clear();

		for ( unsigned int i = 0 ; i < vecFounds.size() ; i ++ )
			if ( vecFounds[ i ]->GetState() == Ocl::Constraint::CS_CHECK_DEPENDENCY_SUCCEEDED )
				vecSucceededs.push_back( vecFounds[ i ] );

		// Complete vecFounds

		for ( unsigned int i = 0 ; i < vecFaileds.size() ; i ++ )
			vecFounds.push_back( vecFaileds[ i ] );
	}

	std::string Facade::GetSignature( OclGme::SpConstraintFunction spCF )
	{
		Ocl::Constraint::Stereotype eStereo = spCF->GetStereotype();
		if ( eStereo == Ocl::Constraint::CS_ATTRIBUTEDEF ) {
			OclSignature::Attribute signature( spCF->GetName(), spCF->GetContextType() );
			return signature.Print();
		}
		else {
			OclCommon::FormalParameterVector vecParameters = spCF->GetFormalParameters();
			StringVector vecTypes;
			for ( unsigned int i = 0 ; i < vecParameters.size() ; i++ ) {
				TypeSeq vecType;
				OclCommon::Convert( vecParameters[ i ].GetTypeName(), vecType );
				vecTypes.push_back( vecType[ 0 ] );
			}
			OclSignature::Method signature( spCF->GetName(), spCF->GetContextType(), vecTypes );
			return signature.Print();
		}
	}

	void Facade::LoadMetaConstraints( OclGme::ConstraintVector& vecFaileds )
	{
		// Load Meta Constraints

		OclCommonEx::MetaBaseVector vecMetas;
		OclCommonEx::GetMetaObjects( GetMetaProject(), "", OBJTYPE_NULL, vecMetas );

		OclGme::ConstraintVector vecFounds;
		for ( unsigned int i = 0 ; i < vecMetas.size() ; i ++ ) {
			std::string strName = "meta::" + OclCommonEx::GetObjectName( vecMetas[ i ].p );;
			CComPtr<IMgaConstraints> spConstraints;
			COMTHROW( vecMetas[ i ]->get_Constraints( &spConstraints ) );
			MGACOLL_ITERATE( IMgaConstraint, spConstraints ) {
				CString strNmspc;
				COMTHROW( MGACOLL_ITER->GetDefinedForNamespace( PutOut( strNmspc)));
				constraint_type_enum eType;
				COMTHROW( MGACOLL_ITER->get_Type( &eType ) );
				if ( eType != CONSTRAINT_TYPE_FUNCTION ) {
					OclGme::SpConstraint spConstraint( new OclGme::Constraint( strName, MGACOLL_ITER ) );
					spConstraint->SetNamespace( std::string(CStringA(strNmspc)));
					spConstraint->Register( m_pTreeManager );
					vecFounds.push_back( spConstraint );
				}
			} MGACOLL_ITERATE_END;
		}

		LoadConstraints( vecFounds, vecFaileds, m_vecMetaConstraints );
		m_vecMetaConstraints = vecFounds;
	}

	void Facade::LoadUserConstraints( OclGme::ConstraintVector& vecFaileds )
	{
		// Load User Constraints

		OclGme::ConstraintVector vecFounds;
		CComPtr<IMgaFolder> spRootFolder;
		COMTHROW( m_spProject->get_RootFolder( &spRootFolder ) );
		OclCommonEx::ObjectVector vecFolders;
		OclCommonEx::GetKindFolders( spRootFolder, "", true, vecFolders );
		for ( unsigned int i = 0 ; i < vecFolders.size() ; i++ ) {
			CComQIPtr<IMgaFolder> spFolder = vecFolders[ i ].p;
			StringVector vecLibraryPath = OclCommonEx::GetLibraryPath( spFolder.p );
			CComPtr<IMgaRegNode> spRootConstraints;
			COMTHROW( spFolder->get_RegistryNode( CComBSTR( "ConstraintDefinitions" ), &spRootConstraints ) );
			CComPtr<IMgaRegNodes> spRegNodes;
			COMTHROW( spRootConstraints->get_SubNodes( VARIANT_TRUE, &spRegNodes ) );
			MGACOLL_ITERATE( IMgaRegNode, spRegNodes ) {
				OclGme::SpConstraint spConstraint( new OclGme::Constraint( MGACOLL_ITER, vecLibraryPath ) );
				spConstraint->Register( m_pTreeManager );
				vecFounds.push_back( spConstraint );
			} MGACOLL_ITERATE_END;
		}

		LoadConstraints( vecFounds, vecFaileds, m_vecUserConstraints );
		m_vecUserConstraints = vecFounds;
	}

	void Facade::LoadConstraints( OclGme::ConstraintVector& vecFounds, OclGme::ConstraintVector& vecFaileds, OclGme::ConstraintVector& vecSucceededs )
	{
		for ( unsigned int i = 0 ; i < vecFounds.size() ; i++ ) {
			Ocl::Constraint::State eState = vecFounds[ i ]->Parse();
			if ( eState == Ocl::Constraint::CS_PARSE_SUCCEEDED ) {
				OclTree::TypeContextStack context;
				context.AddVariable( "project", TypeSeq( 1, "gme::Project" ) );
				eState = vecFounds[ i ]->Check( context );
				if ( eState == Ocl::Constraint::CS_CHECK_SUCCEEDED ) {
					eState = vecFounds[ i ]->SetDependencyResult( vecFounds[ i ]->GetDependencySet() );
					if ( eState == Ocl::Constraint::CS_CHECK_DEPENDENCY_SUCCEEDED )
						vecSucceededs.push_back( vecFounds[ i ] );
					else
						vecFaileds.push_back( vecFounds[ i ] );
				}
				else
					vecFaileds.push_back( vecFounds[ i ] );
			}
			else
				vecFaileds.push_back( vecFounds[ i ] );
		}
	}

	void Facade::GetObjectConstraints( CComPtr<IMgaObject> spObject, EvaluationRecordVector& vecInputs, unsigned long ulCurrentEventMask )
	{
		std::string strKind = "meta::" + OclCommonEx::GetObjectKind( spObject );

		// Collect Meta Constraints

		for ( unsigned int i = 0 ; i < m_vecMetaConstraints.size() ; i++ ) {
			if ( m_vecMetaConstraints[ i ]->IsValid() ) {
				CString sK = OclCommonEx::Convert( strKind );
				CString cT = OclCommonEx::Convert( m_vecMetaConstraints[ i ]->GetContextType()  );
				if ( strKind == m_vecMetaConstraints[ i ]->GetContextType() ) {
					OclGme::Constraint::EnableInfo eInfo = m_vecMetaConstraints[ i ]->GetEnableInfo( spObject );
					if ( eInfo > OclGme::Constraint::CE_NONE && eInfo <= OclGme::Constraint::CE_ENABLED_READONLY ) {
						if ( ulCurrentEventMask == 0x0 || ( m_vecMetaConstraints[ i ]->GetEventMask() & ulCurrentEventMask ) ) {
							EvaluationRecord input;
							input.spObject = CREATE_GMEOBJECT( m_pTreeManager->GetTypeManager(), spObject );
							input.spConstraint = m_vecMetaConstraints[ i ];
							vecInputs.push_back( input );
						}
					}
				}
			}
		}

		// Collect User Constraints

		for ( unsigned int i = 0 ; i < m_vecUserConstraints.size() ; i++ ) {
			if ( m_vecUserConstraints[ i ]->IsValid() ) {
				if ( strKind == m_vecUserConstraints[ i ]->GetContextType() ) {
					OclGme::Constraint::EnableInfo eInfo = m_vecUserConstraints[ i ]->GetEnableInfo( spObject );
					if ( eInfo > OclGme::Constraint::CE_NONE && eInfo <= OclGme::Constraint::CE_ENABLED_READONLY ) {
						if ( ulCurrentEventMask == 0x0 || ( m_vecUserConstraints[ i ]->GetEventMask() & ulCurrentEventMask ) ) {
							EvaluationRecord input;
							input.spObject = CREATE_GMEOBJECT( m_pTreeManager->GetTypeManager(), spObject );
							input.spConstraint = m_vecUserConstraints[ i ];
							vecInputs.push_back( input );
						}
					}
				}
			}
		}
	}

	void Facade::SortByPriority( EvaluationRecordVector& vecInputs ) const
	{
		EvaluationRecordVector vecInputs2 = vecInputs;
		vecInputs.clear();

		for ( unsigned int i = 0 ; i < vecInputs2.size() ; i++ ) {
			bool bWasAdded = false;
			for ( EvaluationRecordVector::iterator it = vecInputs.begin() ; it != vecInputs.end() ; ++it ) {
				if( vecInputs2[ i ].spConstraint->GetPriority() <= (*it).spConstraint->GetPriority() ) {
					vecInputs.insert( it, vecInputs2[ i ] );
					bWasAdded = true;
					break;
				}
			}
			if ( ! bWasAdded )
				vecInputs.push_back( vecInputs2[ i ] );
		}
		int num = vecInputs.size();
		for ( int ii = 0 ; num  &&  ii < num-1 ; ii++ ) 
		{
			TRACE("item: %d, p1: %d, p2: %d \n", ii, vecInputs[ii].spConstraint->GetPriority(), vecInputs[ii+1].spConstraint->GetPriority());
			ASSERT(vecInputs[ii].spConstraint->GetPriority() <= vecInputs[ii+1].spConstraint->GetPriority());
		}
	}

	bool Facade::getConstraintFunctionText(std::string &name, std::string &text)
	{
				text = "";
				for (unsigned int i = 0 ; i < m_vecUserConstraintFunctions.size() ; i++ ) 
				{
					bool valid = m_vecUserConstraintFunctions[ i ]->IsValid();
					std::string fname = m_vecUserConstraintFunctions[ i ]->GetName();// GetFullName();
					if ( valid  &&  fname == name)
					{
						text = m_vecUserConstraintFunctions[ i ]->GetText();
						return true;
					}
				}
				for ( unsigned int i = 0 ; i < m_vecMetaConstraintFunctions.size() ; i++ ) 
				{
					bool valid = m_vecMetaConstraintFunctions[ i ]->IsValid();
					std::string fname = m_vecMetaConstraintFunctions[ i ]->GetName();// GetFullName();
					if ( valid  &&  fname == name)
					{
						text = m_vecMetaConstraintFunctions[ i ]->GetText();
						return true;
					}
				}
				return false;
	}

	void Facade::addFunctionTexts(EvaluationRecord &rec)
	{
		int vnum = rec.vecViolations.size();
		int serial = 1;
		for (int k=0; k< vnum; k++)
		{
			std::string name = rec.vecViolations[k].methodName;
			if (!rec.vecViolations[k].methodName.empty())
			{
				FuncDesc::iterator it = rec.calledFunctions.find(name);
				if (it != rec.calledFunctions.end())
					continue;
				std::string text;
				if (getConstraintFunctionText(name, text))
				{
					FuncItem item;
					item.text = text;
					item.serial = serial;
					std::pair<FuncDesc::iterator, bool> pp = rec.calledFunctions.insert(FuncDesc::value_type(name, item));
					ASSERT(pp.second);
					if (pp.second)
						serial++;
				}
				else
					ASSERT(0);
			}
		}
	}

	void Facade::updateLineno(EvaluationRecord &rec)
	{
		int recsize = rec.vecViolations.size();
		for (int i=0; i < recsize; i++)
		{
			if (!rec.vecViolations[i].methodName.empty())
			{
				std::string name = rec.vecViolations[i].methodName;
				FuncDesc::iterator it = rec.calledFunctions.find(name);
				if (it != rec.calledFunctions.end())
					rec.vecViolations[i].position.iLine += it->second.serial*1000;
				else
					ASSERT(0);;
			}
		}
	}

	HRESULT Facade::EvaluateConstraints( EvaluationRecordVector& vecInputs, bool bShowProgress, IUnknown **punk )
	{
		// Initialization

		SortByPriority( vecInputs );

		CSmallMessageBox dlgProgress;
		if ( bShowProgress )
			dlgProgress.DoModeless( vecInputs.size() );

		CViolationDialog dlgErrors( m_bViolationDlgExpanded, NULL, m_spProject );

		int iViolationCount= 0;
		bool bWasCritical = false;
		bool bStopEvaluation = false;
		long lPriority = 1;
		bool closeNotRequested = true;

		int loopcount = vecInputs.size();
		for ( int i = 0 ; i < loopcount && ! bStopEvaluation ; i++ ) {

			OclGme::SpConstraint constraint = vecInputs[ i ].spConstraint;
//			vecInputs[ i ].vecViolations.clear();
			// Level condition for terminating the evaluation

			if ( m_infoEvaluation.iViolationCount == -1 && vecInputs[ i ].spConstraint->GetPriority() > lPriority && iViolationCount > 0 )
				break;

			// Evaluate

			CComPtr<IMgaObject> spObject;
			(( OclGmeCM::Object*) vecInputs[ i ].spObject.GetImplementation() )->GetValue( spObject );
			OclTree::ObjectContextStack context;
			context.AddVariable( "project", CREATE_GMEPROJECT( m_pTreeManager->GetTypeManager(), m_spProject ) );
			context.AddVariable( "self", vecInputs[ i ].spObject );
			vecInputs[ i ].spObject = vecInputs[ i ].spConstraint->Evaluate( context, m_infoEvaluation.bEnabledSCLogical, m_infoEvaluation.bEnabledSCIterator, m_infoEvaluation.bEnabledTracking );
			vecInputs[ i ].vecViolations = vecInputs[ i ].spConstraint->GetViolations();
			vecInputs[ i ].spConstraint->ClearViolations();

			// Refresh Progress

			if ( bShowProgress ) {
				closeNotRequested = dlgProgress.IncrementProgress();
				if ( !closeNotRequested )
					bStopEvaluation = true;
			}

			// Check the result

			bool bWasViolation = false;
			if ( vecInputs[ i ].spObject.IsUndefined() )
				bWasViolation = true;
			else {
				DECL_BOOLEAN( bResult, vecInputs[ i ].spObject );
				if ( ! bResult )
					bWasViolation = true;
			}

			// Terminate the evaluation if necessary

			if ( bWasViolation ) {
				if ( bShowProgress && m_infoEvaluation.iViolationCount == iViolationCount + 1 )
					bStopEvaluation = true;
				iViolationCount++;
				addFunctionTexts(vecInputs[ i ]);
				updateLineno(vecInputs[ i ]);
				dlgErrors.AddItem( vecInputs[ i ] );
				if ( ! bShowProgress && vecInputs[ i ].spConstraint->GetPriority() == 1 )
					bWasCritical = true;
			}
			lPriority = vecInputs[ i ].spConstraint->GetPriority();
//			vecInputs[ i ].vecViolations.clear();
		}

		// Finalization

		if ( bShowProgress )
			dlgProgress.UndoModeless();

		// Show Errors

		if ( iViolationCount > 0 ) {

			if ( bShowProgress )
				dlgErrors.EnableOK();
			else {
				if ( ! bWasCritical )
					dlgErrors.EnableOK();
				dlgErrors.EnableAbort();
			}

			INT_PTR iResult = dlgErrors.DoModal();
			m_bViolationDlgExpanded = dlgErrors.IsExpanded();
			if (punk)
				dlgErrors.GetGotoPunk(punk);
			if ( iResult != IDOK ) {
				SetErrorInfo(L"Constraint violation");
				return E_MGA_CONSTRAINT_VIOLATION;
			} else
				S_OK;
		}
		else
			if ( bShowProgress && closeNotRequested )
				CSmallMessageBox().DoModal();
		return S_OK;
	}

	void Facade::CollectConstraints( CComPtr<IMgaObject> spObject, bool bRecursive, EvaluationRecordVector& vecInputs )
	{
		GetObjectConstraints( spObject, vecInputs, 0x0 );

		if ( bRecursive ) {
			CComPtr<IMgaFCOs> spFCOs;
			CComQIPtr<IMgaFolder> spFolder = spObject;
			if ( spFolder.p ) {
				CComPtr<IMgaFolders> spFolders;
				COMTHROW( spFolder->get_ChildFolders( &spFolders ) );
				MGACOLL_ITERATE( IMgaFolder, spFolders ) {
					CollectConstraints( MGACOLL_ITER.p, true, vecInputs );
				} MGACOLL_ITERATE_END;
				COMTHROW( spFolder->get_ChildFCOs( &spFCOs ) );
			}
			else {
				CComQIPtr<IMgaModel> spModel = spObject;
				if ( spModel.p )
					COMTHROW( spModel->get_ChildFCOs( &spFCOs ) );
			}

			if ( spFCOs.p ) {
				MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
					CollectConstraints( MGACOLL_ITER.p, true, vecInputs );
				} MGACOLL_ITERATE_END;
			}
		}
	}

	void Facade::CollectConstraints( CComPtr<IMgaObject> spObject, unsigned long ulCurrentEventMask, int iCheckedLevels, EvaluationRecordVector& vecInputs )
	{
		CComPtr<IMgaObject> spParent;
		COMTHROW( spObject->GetParent( &spParent ) );

		if ( spParent.p ) {
			EvaluationRecordVector vecInnerInputs;
			GetObjectConstraints( spParent, vecInnerInputs, ulCurrentEventMask );

			for ( unsigned int i = 0 ; i < vecInnerInputs.size() ; i++ ) {
				switch( vecInnerInputs[ i ].spConstraint->GetDepth() ) {
					case CONSTRAINT_DEPTH_ZERO :
						break;
					case CONSTRAINT_DEPTH_ONE :
						if ( iCheckedLevels >= 1 )
							break;
					default :
						vecInputs.push_back( vecInnerInputs[ i ] );
				}
			}

			CollectConstraints( spParent, ulCurrentEventMask, iCheckedLevels + 1, vecInputs );
		}
	}

	void Facade::CollectConstraints( CComPtr<IMgaObject> spObject, EvaluationRecordVector& vecInputs )
	{
		CollectConstraints( spObject, m_infoEvaluation.iModelDepth == -1, vecInputs );
		if ( m_infoEvaluation.iModelDepth == 1 ) {
			CComQIPtr<IMgaModel> spModel = spObject;
			if ( spModel.p ) {
				CComPtr<IMgaFCOs> spFCOs;
				COMTHROW( spModel->get_ChildFCOs( &spFCOs ) );
				MGACOLL_ITERATE( IMgaFCO, spFCOs ) {
					CollectConstraints( MGACOLL_ITER.p, false, vecInputs );
				} MGACOLL_ITERATE_END;
			}
		}
	}

	HRESULT Facade::EvaluateAll(IUnknown **punk)
	{
		AfxGetApp()->DoWaitCursor( 1 );
		EvaluationRecordVector vecInputs;
		CComPtr<IMgaFolder> spRootFolder;
		COMTHROW( m_spProject->get_RootFolder( &spRootFolder ) );
		CollectConstraints( spRootFolder.p, true, vecInputs );
		AfxGetApp()->DoWaitCursor( -1 );
		return EvaluateConstraints( vecInputs, true, punk );
	}

	HRESULT Facade::Evaluate( CComPtr<IMgaObject> spObject, IUnknown **punk )
	{
		AfxGetApp()->DoWaitCursor( 1 );
		EvaluationRecordVector vecInputs;
		CollectConstraints( spObject, vecInputs );
		AfxGetApp()->DoWaitCursor( -1 );
		return EvaluateConstraints( vecInputs, true, punk );
	}

	HRESULT Facade::Evaluate( const OclCommonEx::ObjectVector& vecObjects, IUnknown **punk)
	{
		AfxGetApp()->DoWaitCursor( 1 );
		EvaluationRecordVector vecInputs;
		for ( unsigned int i = 0 ; i < vecObjects.size() ; i++ )
			CollectConstraints( vecObjects[ i ].p, vecInputs );
		AfxGetApp()->DoWaitCursor( -1 );
		return EvaluateConstraints( vecInputs, true, punk );
	}

	HRESULT Facade::Evaluate( CComPtr<IMgaObject> spObject, unsigned long ulCurrentEventMask)
	{
		AfxGetApp()->DoWaitCursor( 1 );
		EvaluationRecordVector vecInputs;
		GetObjectConstraints( spObject, vecInputs, ulCurrentEventMask );

		if ( ! ( ulCurrentEventMask & OBJEVENT_DESTROYED ) )
			CollectConstraints( spObject, ulCurrentEventMask, 0, vecInputs );
		AfxGetApp()->DoWaitCursor( -1 );
		return EvaluateConstraints( vecInputs, false, NULL);
	}

}; // namespace OclGmeCM