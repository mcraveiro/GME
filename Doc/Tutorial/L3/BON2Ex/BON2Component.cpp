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

#include "stdafx.h"
#include <Console.h>
#include <Formatter.h>
#include "BON2Component.h"
#include "networkingBonX.h"

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
	if ( m_project ) {
		m_project->finalizeObjects();
		finalize( m_project );
		m_project = NULL;
	}
}

// ====================================================
// This method is called after all the generic initialization is done
// This should be empty unless application-specific initialization is needed

void Component::initialize( Project& project )
{
}

static CString operator+(const CString& A, const std::string& B)
{
	return A + B.c_str();
}

static CString MakeHyperlink(BON::Object& obj)
{
	return GMEConsole::Formatter::MakeObjectHyperlink(obj->getName().c_str(), obj->getID().c_str());
}

void Component::ProcessDiagram(Networking::NetDiagram& diagram) 
{
	using namespace GMEConsole;

	Console::Out::WriteLine(CString("Network ") + diagram->getName());

	std::set<Networking::NetDiagram> childDiagrams = diagram->getNetDiagram();
	for (std::set<Networking::NetDiagram>::iterator it = childDiagrams.begin(); it != childDiagrams.end(); it++) {
		ProcessDiagram(*it);
	}

	std::set<Networking::Router> childRouters = diagram->getRouter();
	for (std::set<Networking::Router>::iterator it = childRouters.begin(); it != childRouters.end(); it++) {
		ProcessRouter(*it);
	}
	Console::Out::WriteLine(CString());
} 

void Component::ProcessRouter(Networking::Router& router) {
	using namespace GMEConsole;

	Console::Out::WriteLine(CString("&nbsp;Router ") + MakeHyperlink(router));

	std::set<Networking::Port> ports = router->getPort();
	for (std::set<Networking::Port>::iterator it = ports.begin(); it != ports.end(); it++) {
		Networking::Port port = *it;
		// Read and print Attributes
		std::string iftype = (*it)->getAttribute("IFType")->getStringValue();
		double ifspeed = port->getIFSpeed();
		std::string ipaddr = port->getIPAddress();
		Console::Out::WriteLine(CString("&nbsp;&nbsp;Port ") + MakeHyperlink(port) + 
			"(" + iftype + "; " + CComVariant(ifspeed) + "Kbps), Addr: " + ipaddr );
		// Print the other end of all Connections
		std::multiset<Networking::GenNet> dsts = port->getConnectionDsts();
		for (std::multiset<Networking::GenNet>::iterator it2 = dsts.begin(); it2 != dsts.end(); it2++) {
			Console::Out::WriteLine(CString("&nbsp;&nbsp;&nbsp;Connected to ") + 
				(*it2)->getObjectMeta().name() + " " + MakeHyperlink(*it2));
		}
	}

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
#ifdef GME_ADDON
	project->setAutoCommit( false);
#endif
	using namespace GMEConsole;
	Console::Out::WriteLine("Interpreter started...");

	Console::Out::WriteLine(CString("Router list for network ") + project->getRootFolder()->getName());
	std::set<BON::Object> diagrams = project->getRootFolder()->getChildObjects("NetDiagram");
	for (std::set<BON::Object>::iterator it = diagrams.begin(); it != diagrams.end(); it++) {
		Networking::NetDiagram diagram = *it;
		ProcessDiagram(diagram);
	}

	Console::Out::WriteLine("Interpreter completed...");
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

#endif // GME_ADDON

}; // namespace BON

