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

void Component::ProcessDiagram(BON::Model& diagram) 
{
	using namespace GMEConsole;
	ASSERT(diagram->getObjectMeta().name() == "NetDiagram"); 

	Console::Out::WriteLine(CString("Network ") + diagram->getName());

	std::set<BON::FCO> childModels = diagram->getChildFCOs("NetDiagram");
	for (std::set<BON::FCO>::iterator it = childModels.begin(); it != childModels.end(); it++) {
		ProcessDiagram(BON::Model(*it));
	}

	childModels = diagram->getChildFCOs("Router");
	for (std::set<BON::FCO>::iterator it = childModels.begin(); it != childModels.end(); it++) {
		ProcessRouter(BON::Model(*it));
	}

	std::set<MON::Atom> atoms = diagram->getProject()->getProjectMeta().atoms();
	for (std::set<MON::Atom>::iterator it = atoms.begin(); it != atoms.end(); it++) {
		Console::Out::WriteLine(CString(it->name().c_str()));
		std::set<MON::ConnectionEnd> target = it->connectionEnds();
		for (std::set<MON::ConnectionEnd>::iterator it2 = target.begin(); it2 != target.end(); it2++) {
//			Console::Out::WriteLine(CString("&nbsp") + (*it2->targets().begin()).name() );
		}
	}
	
//	childModels = diagram->getChildFCOs("GenNet");
//	for (std::set<BON::FCO>::iterator it = childModels.begin(); it != childModels.end(); it++) {
//		ProcessDiagram(BON::Model(*it));
//	}

	// Print RouterRefs and their refferents
	std::set<BON::FCO> childFCOs = diagram->getChildFCOs();
	for (std::set<BON::FCO>::iterator it = childFCOs.begin(); it != childFCOs.end(); it++) {
		if (BON::Reference(*it) && (*it)->getObjectMeta().name() == "RouterRef") {
			BON::Reference ref = *it;
			Console::Out::WriteLine(CString("RouterRef ") + 
				MakeHyperlink(ref) + "(" + MakeHyperlink(ref->getReferred()) + ")");
		}
	}

	// Print Administrators and their members (i.e. machines they administer)
	std::set<BON::Set> childSets = diagram->getChildSets();
	for (std::set<BON::Set>::iterator it = childSets.begin(); it != childSets.end(); it++) {
		if ((*it)->getObjectMeta().name() == "Administrator") {
			BON::Set admin = *it;
			std::set<BON::FCO> members = admin->getMembers();
			Console::Out::WriteLine(CString("Members of ") + admin->getName() + ": ");
			for (std::set<BON::FCO>::iterator it2 = members.begin(); it2 != members.end(); it2++) {
				Console::Out::WriteLine(CString("&nbsp;") + (*it2)->getName());
			}
		}
	}
	Console::Out::WriteLine(CString());
} 

void Component::ProcessRouter(BON::Model& router) {
	using namespace GMEConsole;
	std::string metaName = router->getObjectMeta().name();
	ASSERT(router->getObjectMeta().name() == "Router"); 

	Console::Out::WriteLine(CString("&nbsp;Router ") + MakeHyperlink(router));

	// Iterate through child Atoms, looking for Ports
	std::set<BON::Atom> atoms = router->getChildAtoms();
	for (std::set<BON::Atom>::iterator it = atoms.begin(); it != atoms.end(); it++) {
		if ((*it)->getObjectMeta().name() == "Port") {
			BON::Atom port = *it;
			// Read and print Attributes
			std::string iftype = port->getAttribute("IFType")->getStringValue();
			int ifspeed = port->getAttribute("IFSpeed")->getIntegerValue();
			std::string ipaddr = port->getAttribute("IPAddress")->getStringValue();
			Console::Out::WriteLine(CString("&nbsp;&nbsp;Port ") + MakeHyperlink(port) + 
				"(" + iftype + "; " + CComVariant(ifspeed) + "Kbps), Addr: " + ipaddr );
			// Print the other end of all Connections
			std::set<BON::Connection> conns = port->getConnLinks();
			for (std::set<BON::Connection>::iterator it2 = conns.begin(); it2 != conns.end(); it2++) {
				BON::ConnectionEnd other;
				if ((*it2)->getDst() == *it) {
					other = (*it2)->getSrc();
				} else {
					other = (*it2)->getDst();
				}
				if (BON::FCO(other)) {
					BON::FCO otherFCO(other);
					Console::Out::WriteLine(CString("&nbsp;&nbsp;&nbsp;Connected to ") + 
						otherFCO->getObjectMeta().name() + " " + MakeHyperlink(otherFCO));
				}
			}
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
	std::set<BON::Model> rootModels = project->getRootFolder()->getChildModels();
	for (std::set<BON::Model>::iterator it = rootModels.begin(); it != rootModels.end(); it++) {
		if ((*it)->getObjectMeta().name()== "NetDiagram") {
			ProcessDiagram(BON::Model(*it));
		}
	}
	std::set<BON::Folder> folders = project->getRootFolder()->getChildFolders();
	for (std::set<BON::Folder>::iterator it = folders.begin(); it != folders.end(); it++) {
		std::set<BON::Model> models = (*it)->getChildModels();
		for (std::set<BON::Model>::iterator it2 = models.begin(); it2 != models.end(); it2++) {
			if ((*it2)->getObjectMeta().name()== "NetDiagram") {
				ProcessDiagram(BON::Model(*it2));
			}
		}
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

