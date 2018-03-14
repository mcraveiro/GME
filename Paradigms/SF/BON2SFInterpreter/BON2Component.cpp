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
#include "SFBonExtension.h"
#include "BON2Component.h"

namespace BON
{

using namespace SF_BON;

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
	Compound root =  Compound(currentFCO);

	if(!root) {
		AfxMessageBox("Interpretation must start from a Compound model!");
		return;
	}

	std::string filebase = root->getName();
	std::string cfile = filebase + ".c";
	std::string hfile = filebase + ".h";

	std::set<Primitive> primitives;
	root->traverse(primitives);

	FILE *fpt = fopen(hfile.c_str(),"wt");
	if(!fpt) {
		AfxMessageBox(CString("Error opening file ") + hfile.c_str());
		return;
	}

	PrimitiveImpl::writeScriptProto(fpt);
	PrimitiveImpl::writeContextDefs(fpt);
	ParamImpl::writeGlobals(fpt);
	fprintf(fpt,"\n");

    {
		for ( std::set<Primitive>::iterator it = primitives.begin() ; it != primitives.end() ; it++ )
			(*it)->writeContextCreate(fpt);
	}
	
	fclose(fpt);

	fpt = fopen(cfile.c_str(),"wt");
	if(!fpt) {
		AfxMessageBox("Error opening file " + CString(cfile.c_str()));
		return;
	}

	fprintf(fpt,"#include <string.h>\n");
	fprintf(fpt,"#include <mgk60.h>\n");
	fprintf(fpt,"#include \"%s\"\n\n",hfile.c_str());

	ParamImpl::writeGlobals(fpt,false);
	fprintf(fpt,"\n");

    {
		for ( std::set<Primitive>::iterator it = primitives.begin() ; it != primitives.end() ; it++ )
			(*it)->writeContextCreate(fpt,false);
	}
	fprintf(fpt,"\n");
	


	fprintf(fpt,"\nvoid register_scripts()\n{\n");
	PrimitiveImpl::writeScriptRegs(fpt);
	fprintf(fpt,"}\n\n");


	fprintf(fpt,"\nvoid build()\n{\n");

	{
		for ( std::set<Primitive>::iterator it = primitives.begin() ; it != primitives.end() ; it++ )
			(*it)->writeActorCreate(fpt);
	}
	fprintf(fpt,"\n");

	{
		for ( std::set<Primitive>::iterator it = primitives.begin() ; it != primitives.end() ; it++ )
			(*it)->writeSetContext(fpt);
	}
	fprintf(fpt,"\n");

	{
		for ( std::set<Primitive>::iterator it = primitives.begin() ; it != primitives.end() ; it++ )
			(*it)->writeActorConnect(fpt);
	}

	fprintf(fpt,"}\n");
	fclose(fpt);

	AfxMessageBox(CString(cfile.c_str()) + " and " + hfile.c_str() + " generated");
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

void  Component::globalEventPerformed( globalevent_enum event )
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


}; // namespace BON


