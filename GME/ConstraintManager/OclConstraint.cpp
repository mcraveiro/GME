//###############################################################################################################################################
//
//	Object Constraint Language Generic Manager
//	OCLConstraint.h
//
//###############################################################################################################################################
#include "StdAfx.h"
#include "Solve4786.h"
#include "OCLConstraint.h"

#include "OCLObjectExBasic.h"
#include "OCLTokens.h"
#include "OCLParser.h"

//udmoclpat_oclconstraint_cpp_1 __please do not remove or change this line__

namespace Ocl
{
	Constraint::Stereotype StringToStereotype( const std::string& str )
	{
		if ( str == "inv" )
			return Constraint::CS_INVARIANT;
		if ( str == "pre" )
			return Constraint::CS_PRECONDITION;
		if ( str == "post" )
			return Constraint::CS_POSTCONDITION;
		if ( str == "defattribute" )
			return Constraint::CS_ATTRIBUTEDEF;
		return Constraint::CS_METHODDEF;
	}

//##############################################################################################################################################
//
//	C L A S S : Ocl::Constraint
//
//##############################################################################################################################################

	Constraint::Constraint()
		:  m_pManager( NULL ), m_pCtxConstraint( NULL ), m_pConstraint( NULL ), m_strName( "" ), m_strText( "" ), m_eState( CS_UNREGISTERED ), m_bContextSucceeded( false )
	{
	}

	Constraint::Constraint( OclTree::TreeManager* pManager )
		: m_pManager( pManager ), m_pCtxConstraint( NULL ), m_pConstraint( NULL ), m_strName( "" ), m_strText( "" ), m_eState( CS_UNREGISTERED ), m_bContextSucceeded( false )
	{
		if ( pManager )
			m_eState = CS_REGISTERED;
	}

	Constraint::Constraint( OclTree::TreeManager* pManager, const std::string& strName, const std::string& strText, bool bDynamic )
		: m_pManager( pManager ), m_pCtxConstraint( NULL ), m_pConstraint( NULL ), m_strText( strText ), m_strName( strName ), m_eState( CS_UNREGISTERED ), m_bDynamic( bDynamic ), m_bContextSucceeded( false )
	{
		if ( pManager ) {
			m_eState = CS_REGISTERED;
			if ( ! m_strName.empty() && ! m_strText.empty() )
				m_eState = CS_DEFINED;
		}
	}

	Constraint::~Constraint()
	{
		if ( m_pCtxConstraint )
			delete m_pCtxConstraint;
		if ( m_pConstraint )
			delete m_pConstraint;
	}

	Constraint::State Constraint::Register( OclTree::TreeManager* pManager )
	{
		if ( pManager && m_eState == CS_UNREGISTERED ) {
			m_eState = ( ! m_strName.empty() && ! m_strText.empty() ) ? CS_DEFINED : CS_REGISTERED;
			m_pManager = pManager;
		}
		return m_eState;
	}

	Constraint::State Constraint::Define( const std::string& strName, const std::string& strText, bool bDynamic )
	{
		if ( ! strName.empty() || ! strText.empty() && m_eState <= CS_DEFINED ) {
			m_bDynamic = bDynamic;
			if ( ! strName.empty() )
				m_strName = strName;
			if ( ! strText.empty() )
				m_strText = strText;
			if ( m_eState == CS_REGISTERED && ! m_strName.empty() && ! m_strText.empty() )
				m_eState = CS_DEFINED;
		}
		return m_eState;
	}

	std::string Constraint::GetDefinedName() const
	{
		return m_strName;
	}

	Constraint::State Constraint::GetState() const
	{
		return m_eState;
	}

	bool Constraint::IsValid() const
	{
		return ! ( m_eState == CS_CTX_PARSE_FAILED || m_eState == CS_CTX_CHECK_FAILED || m_eState == CS_PARSE_FAILED || m_eState == CS_CHECK_FAILED || m_eState == CS_CHECK_DEPENDENCY_FAILED );
	}

	bool Constraint::IsDefined() const
	{
		return m_eState >= CS_DEFINED;
	}

	std::string Constraint::GetText() const
	{
		return m_strText;
	}

	bool Constraint::IsDynamic() const
	{
		return m_bDynamic;
	}

	Constraint::State Constraint::ParseContext()
	{
		if ( m_eState < CS_DEFINED )
			throw m_eState;
		if ( m_pCtxConstraint )
			return CS_CTX_PARSE_SUCCEEDED;
		m_pCtxConstraint = OCLParser::ParseConstraint( m_pManager, m_strText, true, m_poolCtxExceptions );
		if ( m_poolCtxExceptions.IsEmpty() && m_pCtxConstraint ) {
			if ( ! m_pConstraint )
				m_eState = CS_CTX_PARSE_SUCCEEDED;
			return CS_CTX_PARSE_SUCCEEDED;
		}
		if ( m_pCtxConstraint ) {
			delete m_pCtxConstraint;
			m_pCtxConstraint = NULL;
		}
		return m_eState = CS_CTX_PARSE_FAILED;
	}

	std::string Constraint::GetName() const
	{
		if ( m_pCtxConstraint )
			return m_pCtxConstraint->m_strName;
		if ( m_pConstraint )
			return m_pConstraint->m_strName;
		throw m_eState;
	}

	Constraint::Stereotype Constraint::GetStereotype() const
	{
		if ( m_pCtxConstraint )
			return StringToStereotype( m_pCtxConstraint->m_pContext->m_strStereotype );
		if ( m_pConstraint )
			return StringToStereotype( m_pConstraint->m_pContext->m_strStereotype );
		throw m_eState;
	}

	Constraint::State Constraint::CheckContext()
	{
		if ( m_eState == CS_CTX_CHECK_FAILED )
			return m_eState;
		if ( m_bContextSucceeded || m_eState > CS_CHECK_SUCCEEDED || m_eState == CS_CTX_CHECK_SUCCEEDED )
			return CS_CTX_CHECK_SUCCEEDED;
		if ( ! m_pCtxConstraint )
			throw CS_CTX_CHECK_FAILED;
		OclTree::TypeContext context( m_nmsp);
		m_bContextSucceeded = m_pCtxConstraint->Check( context );
		m_poolCtxExceptions = context.m_poolExceptions;
		Constraint::State eState = ( m_bContextSucceeded ) ? CS_CTX_CHECK_SUCCEEDED : CS_CTX_CHECK_FAILED;
		if ( m_eState == CS_CTX_PARSE_SUCCEEDED )
			m_eState = eState;
		return eState;
	}

	std::string Constraint::GetContextType() const
	{
		if ( m_bContextSucceeded )
			return m_pCtxConstraint->m_pContext->m_strType;
		if ( m_eState >= CS_CHECK_SUCCEEDED )
			return m_pConstraint->m_pContext->m_strType;
		throw m_eState;
	}

	std::string Constraint::GetFullName() const
	{
		return GetContextType() + "::" + GetName();
	}

	std::string Constraint::GetNamespace() const
	{
		return m_nmsp;
	}

	void Constraint::SetNamespace( const std::string& nm)
	{
		m_nmsp = nm;
	}


	std::string Constraint::GetReturnType() const
	{
		if ( m_bContextSucceeded || m_eState >= CS_CHECK_SUCCEEDED ) {
			if ( GetStereotype() == CS_INVARIANT )
				throw CS_INVARIANT;
			if ( m_bContextSucceeded )
				return m_pCtxConstraint->m_pContext->m_strReturnType;
			return m_pConstraint->m_pContext->m_strReturnType;
		}
		throw m_eState;
	}

	OclCommon::FormalParameterVector Constraint::GetFormalParameters() const
	{
		if ( m_bContextSucceeded || m_eState >= CS_CHECK_SUCCEEDED ) {
			if ( GetStereotype() == CS_INVARIANT )
				throw CS_INVARIANT;
			if ( m_bContextSucceeded )
				return m_pCtxConstraint->m_pContext->m_vecParameters;
			return m_pConstraint->m_pContext->m_vecParameters;
		}
		throw m_eState;
	}

	Constraint::State Constraint::Parse()
	{
		if ( m_eState < CS_DEFINED )
			throw m_eState;
		if ( m_eState == CS_PARSE_FAILED )
			return m_eState;
		if ( m_eState >= CS_PARSE_SUCCEEDED )
			return CS_PARSE_SUCCEEDED;
		m_pConstraint = OCLParser::ParseConstraint( m_pManager, m_strText, false, m_poolExceptions );
		if ( m_poolExceptions.IsEmpty() && m_pConstraint )
			return m_eState = CS_PARSE_SUCCEEDED;
		if ( m_pConstraint ) {
			delete m_pConstraint;
			m_pConstraint = NULL;
		}
		return m_eState = CS_PARSE_FAILED;
	}

	Constraint::State Constraint::Check( OclTree::TypeContextStack& ctxTypes )
	{
		if ( m_eState < CS_PARSE_SUCCEEDED )
			throw m_eState;
		if ( m_eState == CS_CHECK_FAILED )
			return m_eState;
		if ( m_eState >= CS_CHECK_SUCCEEDED )
			return CS_CHECK_SUCCEEDED;
		OclTree::TypeContext context( m_nmsp);
		context.m_ctxTypes = ctxTypes;
		bool bResult = m_pConstraint->Check( context );
		m_poolExceptions = context.m_poolExceptions;
		return m_eState = ( bResult ) ? CS_CHECK_SUCCEEDED : CS_CHECK_FAILED;
	}

	OclMeta::DependencySet Constraint::GetDependencySet() const
	{
		if ( m_eState < CS_CHECK_SUCCEEDED )
			throw m_eState;
		return m_pConstraint->m_pContext->m_setDependencies;
	}

	Constraint::State Constraint::SetDependencyResult( const OclMeta::DependencySet& setDependencies )
	{
		if ( m_eState != CS_CHECK_SUCCEEDED )
			throw m_eState;
		bool bWasError = false;
		for ( OclMeta::DependencySet::const_iterator i = setDependencies.begin() ; i != setDependencies.end() ; ++i ) {
			if ( (*i).m_bFailed ) {
				bWasError = true;
				OclCommon::Exception exp( OclCommon::Exception( OclCommon::Exception::ET_SEMANTIC, EX_CONSTRAINT_DEF_FAILED, (*i).m_strSignature, (*i).m_position.iLine, (*i).m_position.iColumn ) );
				m_poolExceptions.Add( exp );
			}
		}
		return m_eState = ( bWasError ) ? CS_CHECK_DEPENDENCY_FAILED : CS_CHECK_DEPENDENCY_SUCCEEDED;
	}

	std::string Constraint::Print() const
	{
		if ( m_eState >= CS_CHECK_SUCCEEDED )
			return m_pConstraint->Print( "" );
		throw m_eState;
	}

	std::string Constraint::PrintTree() const
	{
		if ( m_eState >= CS_PARSE_SUCCEEDED )
			return m_pConstraint->Print( "" );
		throw m_eState;
	}

	OclMeta::Object Constraint::Evaluate( OclTree::ObjectContextStack& ctxObjects, bool bEnableLogicalShortCircuit, bool bEnableIteratorShortCircuit, bool bEnableTracking )
	{
		if ( m_eState < CS_CHECK_DEPENDENCY_SUCCEEDED )
			throw m_eState;
		OclTree::ObjectContext context;
		context.oCtx = ctxObjects;
		context.bDoSnapshot = true;
		context.iViolationCount = 0;
		context.m_bHasException = false;
		context.m_bShortCircuitLogical = bEnableLogicalShortCircuit;
		context.m_bShortCircuitIterator = bEnableIteratorShortCircuit;
		context.m_bEnableTracking = bEnableTracking;

		OclMeta::Object spResult = m_pConstraint->Evaluate( context );
		m_vecViolations = context.vecViolations;
		if ( spResult.IsUndefined() )
			m_eState = CS_EVAL_FAILED;
		else
			m_eState = CS_EVAL_SUCCEEDED;
		return spResult;
	}

	OclCommon::ExceptionPool Constraint::GetExceptions()
	{
		if ( m_eState <= CS_DEFINED )
			throw m_eState;
		return ( m_eState > CS_CTX_PARSE_FAILED && m_eState <= CS_CTX_CHECK_SUCCEEDED ) ? m_poolCtxExceptions : m_poolExceptions;
	}

	OclCommon::ExceptionPool Constraint::GetExceptions( bool bContext )
	{
		if ( m_eState <= CS_DEFINED )
			throw m_eState;
		return ( bContext ) ? m_poolCtxExceptions : m_poolExceptions;
	}

	// this method called once at the end not for each constarint function ?? !!
	// m_vecViolations will lose for the constarint functions
	OclTree::ViolationVector Constraint::GetViolations()
	{
		if ( m_eState < CS_EVAL_FAILED )
			throw m_eState;
		return m_vecViolations;
	}

	void Constraint::ClearViolations()
	{
		int db = m_vecViolations.size();
		m_vecViolations.clear();;
		db = m_vecViolations.size();
	}

//udmoclpat_oclconstraint_cpp_2 __please do not remove or change this line__

}; // namespace Ocl
