//###############################################################################################################################################
//
//	Meta and Builder Object Network V2.0 for GME
//	BON2Component.cpp
//
//###############################################################################################################################################

/*
	Copyright (c) Vanderbilt University, 2000-2004
	ALL RIGHTS RESERVED

	Vanderbilt University disclaims all warranties with regard to this
	software, including all implied warranties of merchantability
	and fitness.  In no event shall Vanderbilt University be liable for
	any special, indirect or consequential damages or any damages
	whatsoever resulting from loss of use, data or profits, whether
	in an action of contract, negligence or other tortious action,
	arising out of or in connection with the use or performance of
	this software.
*/
#include "StdAfx.h"
#include "BON2Component.h"

namespace BON
{

//###############################################################################################################################################
//
// 	C L A S S : BON::Component
//
//###############################################################################################################################################

Component::Component()
	: m_bIsInteractive( false )
{
}

Component::~Component()
{
	finalize( m_project );
	m_project = NULL;
}

// ====================================================
// This method is called after all the generic initialization is done
// This should be empty unless application-specific initialization is needed

void Component::initialize( Project& project )
{
	// ======================
	// Insert application specific code here
}

// ====================================================
// This method is called before the whole BON2 project released and disposed
// This should be empty unless application-specific finalization is needed

void Component::finalize( Project& project )
{
	// ======================
	// Insert application specific code here
}

// ====================================================
// This is the obsolete component interface
// This present implementation either tries to call InvokeEx, or does nothing except of a notification

void Component::invoke( Project& project, const std::set<FCO>& setModels, long lParam )
{
	#ifdef SUPPORT_OLD_INVOKE
		Object focus;
		invokeEx( project, focus, setModels, lParam );
	#else
		if ( m_bIsInteractive )
			AfxMessageBox("This BON2 Component does not support the obsolete invoke mechanism!");
	#endif
}

// ====================================================
// This is the main component method for Interpereters and Plugins.
// May also be used in case of invokeable Add-Ons

void Component::invokeEx( Project& project, FCO& currentFCO, const std::set<FCO>& setSelectedFCOs, long lParam )
{
	// ======================
	// Insert application specific code here

	// The Main method ! It is so simple... creating a dialog, and call the visitor with the rootFolder !

	CPrintDlg dlg;
	MyVisitor visitor( &dlg );
	project->getRootFolder()->accept( &visitor );
	dlg.DoModal();

}

// ====================================================
// GME currently does not use this function
// You only need to implement it if other invokation mechanisms are used

void Component::objectInvokeEx( Project& project, Object& currentObject, const std::set<Object>& setSelectedObjects, long lParam )
{
	if ( m_bIsInteractive )
		AfxMessageBox("This BON2 Component does not support objectInvokeEx method!");
}

// ====================================================
// Implement application specific parameter-mechanism in these functions

Util::Variant Component::getParameter( const std::string& strName )
{
	// ======================
	// Insert application specific code here

	return Util::Variant();
}

void Component::setParameter( const std::string& strName, const Util::Variant& varValue )
{
	// ======================
	// Insert application specific code here
}

#ifdef GME_ADDON

// ====================================================
// If the component is an Add-On, then this method is called for every Global Event

void Component::globalEventPerformed( globalevent_enum event )
{
	// ======================
	// Insert application specific code here
}

// ====================================================
// If the component is an Add-On, then this method is called for every Object Event

void Component::objectEventPerformed( Object& object, unsigned long event, VARIANT v )
{
	// ======================
	// Insert application specific code here
}

#endif GME_ADDON

//###############################################################################################################################################
//
// 	C L A S S : BON::ProcessingBaseImpl
//
//###############################################################################################################################################

	void ProcessingBaseImpl::initialize()
	{
		m_bParamsAccessed = false;
		childParameters();
	}

	// Here we use the standard way to decide whether an obtained Atom is one of the Param kinds. But, check the kind assignment of Parameter class below... we could use simply this :        if ( Parameter( *it ) ) m_vecParams.push_back( *it ).

	std::vector<Parameter>& ProcessingBaseImpl::childParameters()
	{
		if ( ! m_bParamsAccessed ) {
			std::set<FCO> children = getChildFCOs();
			for ( std::set<FCO>::iterator it = children.begin() ; it != children.end() ; it++ )
				if ( (*it)->getFCOMeta().name().find( "Param" ) != std::string::npos )
					m_vecParams.push_back( *it );
			m_bParamsAccessed = true;
		}
		return m_vecParams;
	}

	// BON::Objects have to be cleared, actually you do not have to, but always recommended because of the reference counting... and it is good practice to "dispose" everything manually after we do not need it

	ProcessingBaseImpl::~ProcessingBaseImpl()
	{

		m_vecParams.clear();
	}

//###############################################################################################################################################
//
// 	C L A S S : BON::CompoundImpl
//
//###############################################################################################################################################

	IMPLEMENT_BONEXTENSION( Compound, "Compound" );  // Here we specified the kind name

	// Here we use type casting in order to access simply the priority() method implemented by us. The methods which are implemented in the BON Base classes are accessible with . operator. Ours are accessible with -> operator.

	int CompoundImpl::getHighPrimitivesCount()
	{
		int iCnt = 0;
		std::set<FCO> children = getChildFCOsAs( "PrimitiveParts" );
		for ( std::set<FCO>::iterator it = children.begin() ; it != children.end() ; it++ )
			if ( Primitive( *it )->priority() > 5 )
				iCnt++;
		return iCnt;
	}

//###############################################################################################################################################
//
// 	C L A S S : BON::PrimitiveImpl
//
//###############################################################################################################################################

	IMPLEMENT_BONEXTENSION( Primitive, "PrimitiveParts" ); // Here we specified the role name which is the only one

	// The implementor should not forget to call the base class' initialize
	// How simple an attribute access is!

	void PrimitiveImpl::initialize()
	{
		ProcessingBaseImpl::initialize();
		m_iPriority = getAttribute( "Priority" )->getIntegerValue();
	}

	long PrimitiveImpl::priority() const
	{
		return m_iPriority;
	}

//###############################################################################################################################################
//
// 	C L A S S : BON::ParameterImpl
//
//###############################################################################################################################################

	IMPLEMENT_BONEXTENSION( Parameter, "Param InputParam OutputParam" ); // Here we specified the kind names of the parameters .... but !!! check the IOParameter

	void ParameterImpl::initialize()
	{
		m_iSize = getAttribute( "Size" )->getIntegerValue();
		std::string str = getAttribute( "DataType" )->getStringValue();
		if ( str == "Integer" )
			m_eType = DT_Integer;
		else if ( str == "Real" )
			m_eType = DT_Real;
		else if ( str == "Character" )
			m_eType = DT_Character;
		else
			m_eType = DT_Pointer;
	}

	// this call corresponds to the well-known reverseConnectedFCOs in GmeOcl !
	// type cast again... ConnectionEnd can be an FCO (the real one), or its ReferencePorts.... ConnectionEnd is the Ascendant of FCO and ReferencePort

	std::set<IOParameter> ParameterImpl::inputParameters()
	{
		std::multiset<ConnectionEnd> params = getConnEndsAs( "ParameterConn", "dst" );
		std::set<IOParameter> params2;
		for ( std::multiset<ConnectionEnd>::iterator it = params.begin() ; it != params.end() ; it++ )
			if ( (*it)->isReferencePort() )
				params2.insert( IOParameter( ReferencePort( *it )->getFCO() ) );
			else
				params2.insert( IOParameter( FCO( *it ) ) );
		return params2;
	}

	// the assocPeers corresponds to the well-known connectedFCOs in GmeOcl ! The role targets the associationEnd in the another side.
	// It returns multiset, because set is not suitable if we want to know how many times an object is attached to an antoher one.

	std::set<IOParameter> ParameterImpl::outputParameters()
	{
		std::multiset<ConnectionEnd> params = getConnEnds( "", "dst" );
		std::set<IOParameter> params2;
		for ( std::multiset<ConnectionEnd>::iterator it = params.begin() ; it != params.end() ; it++ )
			params2.insert( ( (*it)->isReferencePort() ) ? ReferencePort( *it )->getFCO() : FCO( *it ) );
		return params2;
	}

	std::string ParameterImpl::className() const
	{
		return "Parameter";
	}

//###############################################################################################################################################
//
// 	C L A S S : BON::IOParameterImpl
//
//###############################################################################################################################################

	IMPLEMENT_BONEXTENSION( IOParameter, "InputParameters OutputParameters" ); // These are rolenames... At the Signal, everything will be clear.

	std::string IOParameterImpl::className() const
	{
		return "IOParameter";
	}

//###############################################################################################################################################
//
// 	C L A S S : BON::SignalImpl
//
//###############################################################################################################################################

	// This implementation as you can see is assigned to every Atom in the Project. We can do that... because every Atom (Signals, Parameters) can be contained by Primitives. Of course the name should not be Signal. It is only a demo.

	IMPLEMENT_BONEXTENSION( Signal, "BON::Atom" );

	bool SignalImpl::isMyParentPrimitive()
	{
		if ( getParentModel( "Primitive" ) )
			return true;
		return false;
	}

//###############################################################################################################################################
//
// 	C L A S S : BON::MyVisitor
//
//###############################################################################################################################################

	// The Implementation assignment has a precedence :
	//	At first the engine match the available rolenames.... If it find one and the object has it, then the implementation is assigned.
	//	If there was no rolename, then the kind of the object is checked...
	//	If the kind was not suitable, then it tries the metakinds e.g. BON::Atom, BON::Reference....
	// 	If it is wrong neither, then the object will not get an implementation.

	// About Casting regarding the SF meta
	// Everything will be a Signal.... This is only an example... It is not appropriate due to the meta !
	// Every Param, InputParam, OutputParam will match to BON::Parameter
	// InputParam, OutputParam will match to IOParameter because their role
	// Compound will be Compound by its kind
	// Primitive will be Primitive by its role.... Important .... Primitives which resides in a Folder will NOT match because it does not have a containment role

	MyVisitor::MyVisitor( CPrintDlg* pDlg )
		: m_pDlg( pDlg )
	{
		pDlg->m_strValue = "";
	}

	void MyVisitor::visitObjectImpl( const Object& object )
	{
		PrintLine( "-----------------------------------------------------------------------" );
		PrintLine( "ObjectName : " + object->getName() );
		PrintLine( "ObjectKind : " + object->getObjectMeta().name() );
		PrintLine( "ObjectPath : " + object->getPath() );
	}

	void MyVisitor::visitFCOImpl( const FCO& object )
	{
		MON::Containment role = object->getRole();
		if ( role )
			PrintLine( "ContainmentRole : " + role.name() );
	}

	void MyVisitor::visitAtomImpl( const Atom& object )
	{
		PrintLine( "ObjectMetaKind : Atom" );
		if ( ! visitIOParameter( IOParameter( object ) ) )
			if ( ! visitParameter( Parameter( object ) ) )
				if ( ! visitSignal( Signal( object ) ) )
					PrintLine( "Something wrong" );
	}

	void MyVisitor::visitModelImpl( const Model& object )
	{
		PrintLine( "ObjectMetaKind : Model" );
		if ( ! visitCompound( Compound( object ) ) )
			if ( ! visitPrimitive( Primitive( object ) ) )
				PrintLine( "Something wrong" );
	}

	void MyVisitor::PrintLine( const std::string& str )
	{
		m_pDlg->m_strValue += CString( str.c_str() ) + "\r\n";
	}

	bool MyVisitor::visitPrimitive( const Primitive& object )
	{
		if ( object ) {
			PrintLine( "ClassExtension : Primitive" );
			PrintLine( "Size value : " + (std::string) Util::Variant( object->priority() ) );
			return true;
		}
		return false;
	}

	bool MyVisitor::visitCompound( const Compound& object )
	{
		if ( object ) {
			PrintLine( "ClassExtension : Compound" );
			PrintLine( "HighPrimitiveCnt value : " + (std::string) Util::Variant( (long) object->getHighPrimitivesCount() ) );
			return true;
		}
		return false;
	}

	bool MyVisitor::visitParameter( const Parameter& object )
	{
		if ( object ) {
			PrintLine( "ClassExtension : Parameter" );
			PrintLine( "Size value : " + (std::string) Util::Variant( object->m_iSize ) );
			PrintLine( "ClassName by Class: " + (std::string) Util::Variant( object->className() ) );
			return true;
		}
		return false;
	}

	bool MyVisitor::visitIOParameter( const IOParameter& object )
	{
		if ( object ) {
			PrintLine( "ClassExtension : IOParameter" );
			PrintLine( "Size value : " + (std::string) Util::Variant( object->m_iSize ) );
			PrintLine( "ClassName by Class: " + (std::string) Util::Variant( object->className() ) );
			return true;
		}
		return false;
	}

	bool MyVisitor::visitSignal( const Signal& object )
	{
		if ( object ) {
			PrintLine( "ClassExtension : Signal" );
			PrintLine( "Is parent Primitive value : " + (std::string) Util::Variant( object->isMyParentPrimitive() ) );
			return true;
		}
		return false;
	}

}; // namespace BON

