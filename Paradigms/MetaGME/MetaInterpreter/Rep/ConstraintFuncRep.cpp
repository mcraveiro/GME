#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "ConstraintFuncRep.h"
#include "Regexp.h"

#include "globals.h"
extern Globals global_vars;


ConstraintFuncRep::ConstraintFuncRep( BON::FCO& ptr)
: Any( ptr),
	m_context(""), m_returntype(""), m_stereotype(""),
	m_definition(""), m_parameterlist("")
{
}

void ConstraintFuncRep::fetch()
{
	CString strWS = "[\t ]*";
	CString strID = "[_a-zA-Z][_a-zA-Z0-9]*";
	CString strT = strID + "(::" + strID + ")*";
	CString strP = strID + strWS + ":" + strWS + strT;

	std::string paramlist = m_ptr->getAttribute( "CFuncParamList")->getStringValue();
	Regexp rePL( "^(" + strWS + strP + ")?" + "(" + strWS + "," + strWS + strP + ")*$", TRUE );
	if ( ! rePL.CompiledOK() )
		global_vars.err << MSG_ERROR << "Internal Interpreter Error: [REGEXP] " << rePL.GetErrorString() << "\n";
	else
		if ( ! rePL.Match( paramlist.c_str() ) )
			global_vars.err << MSG_ERROR << "Invalid format for parameterlist in Constraint Function: " << m_ptr
			<< "\nCorrect format: <paramname> : <paramtype> [, <paramname> : <paramtype>] ...\n";
	
	m_parameterlist = paramlist;

	std::string contexttype = m_ptr->getAttribute( "CFuncContext")->getStringValue();
	if ( contexttype.empty() ) 
	{
		global_vars.err << MSG_ERROR << "No context found for Constraint Function: " << m_ptr << ". It will be Project implicitly.\n";
		contexttype = "gme::Project";
	}
	m_context = contexttype;
	bool modify = false;// if we modify the context that is just for the ConstraintManager's sake (otherwise it would crash)
	if( modify && m_ptr->isInLibrary() && !m_namespace.empty())
	{
		if( m_context.substr(0, 6) == "meta::" && m_context.substr( 6).find( "::") == std::string::npos)
		{ // short kind name used
			m_context.insert( 6, Any::NamespaceDelimiter_str);
			m_context.insert( 6, m_namespace); // thus a context like meta::Target becomes meta::NMSP::Target for library constraints
		}
	}

	std::string returntype = m_ptr->getAttribute( "CFuncReturnType")->getStringValue();
	if ( returntype.empty()) 
	{
		global_vars.err << MSG_ERROR << "No return type found for Constraint Function: " << m_ptr << ". It will be bool implicitly.\n";
		returntype = "ocl::Boolean";
	}
	m_returntype = returntype;

	std::string definition = m_ptr->getAttribute( "CFuncDefinition")->getStringValue();
	if ( definition.empty() )
		global_vars.err << MSG_ERROR <<"No definition found for Constraint Function: " << m_ptr << ".\n";
	m_definition = definition;
	
	std::string stereotype = m_ptr->getAttribute( "CFuncStereotype")->getStringValue();
	if ( stereotype == "attribute" && ! paramlist.empty()) 
	{
		global_vars.err << MSG_ERROR << "Attribute cannot have parameters. Parameterlist is ignored for Constraint Function: " << m_ptr << ".\n";
		paramlist.empty();
	}
	m_stereotype = stereotype;

	//m_defdForNamesp = m_ptr->getAttribute("DefinedForNamespace")->getStringValue();
	m_defdForNamesp = m_namespace;
}

/*virtual*/ std::string ConstraintFuncRep::getName() const
{
	ASSERT( m_ptr);
	return m_ptr->getName();
}

std::string ConstraintFuncRep::doDump()
{
	fetch();
	std::string mmm = "";
	
	mmm += indStr() + "<constraint type=\"function\" name=\"" + getName() + "\"";
	///z dump either the m_namespace variable /or/ the specific defdForNamesp variable owned by this
	///z m_namespace or m_defdForNamesp
	///z dump only if the object belongs to main namespace
	if( !m_defdForNamesp.empty() && m_ptr && m_ptr->isInLibrary()) mmm+= " defdfornamesp = \"" + m_defdForNamesp + "\"";
	mmm += ">\n";
	++ind;
	mmm += indStr() + "<![CDATA[" 
		+ m_stereotype + ";" + m_context + ";" 
		+ m_parameterlist + ";" + m_returntype + ";\n";
	mmm += indStr() + m_definition + "]]>\n";
	--ind;
	mmm += indStr() + "</constraint>\n"; 

	return mmm;
}