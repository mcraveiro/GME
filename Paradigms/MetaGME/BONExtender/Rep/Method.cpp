#include "stdafx.h"

#include "method.h"
#include "Any.h"
#include "CodeGen.h"

static const std::string orn = "//********************************************************************************\n";
/*const*/ extern int h_ind;
/*const*/ extern int s_ind;

bool MethodLexicographicSort::operator()( const Method& op1, const Method& op2) const
{
	const std::string s1 = op1.m_signature;
	const std::string s2 = op2.m_signature;
	return ( s1.compare(s2) < 0);
}


Method::Method()
	: m_virtual( true)
	, m_static( false)
	, m_template( false)
	, m_returnValue()
	, m_signature()
	, m_implementation()
	, m_comment()
	, m_container(0)
{ }


std::string Method::getHeader()
{
	if ( m_template)
	{
		return  CodeGen::indent(h_ind) + orn +
				CodeGen::indent(h_ind) + "// " + m_comment + "\n" +
				CodeGen::indent(h_ind) + orn +
				m_returnValue + CodeGen::indent(2) + m_signature + "\n" + m_implementation;
	}

	std::string ret_val = (m_virtual?"virtual ":"") + m_returnValue;
	int fill_size = ret_val.size() < 20? 20 - ret_val.size(): ret_val.size() < 40? 40-ret_val.size(): ret_val.size() < 50? 50-ret_val.size():1;
	return CodeGen::indent(h_ind) + ret_val + CodeGen::fill( fill_size) + m_signature + ";";
}


std::string Method::getSource()
{
	if ( m_template || m_implementation.empty()) // templates or dummies like init/finalize
		return "";

	ASSERT( m_container);
	return 
		CodeGen::indent(s_ind) + orn + 
		CodeGen::indent(s_ind) + "// " + m_comment + "\n" + 
		CodeGen::indent(s_ind) + orn + 
		CodeGen::indent(s_ind) + m_returnValue + " " + m_container->getValidNmspc() + Any::NamespaceDelimiter_str + m_container->getValidNameImpl() + "::" + m_signature + "\n" + 
		m_implementation;
}


std::string Method::getExposed( const std::string& repl_cont)
{
	// the using directive needs the immediate parent to be specified before the '::'
	// so grandparent's name is not allowed
	// i.e: 
	//         A<>-----r
	//        /|\
	//         |
	//         B<>-----p
	//        /.\
	//         |
	//         C (implementation inheritance between B and C)
	//
	//  in such cases C inherits privately from A thus hides to its users the 
	//  methods inherited like: getr() and getp()
	//  these methods needed to be exposed by 
	//  using B::getp;
	//  using A::getr; directives
	//  but the latter i not allowed (A is granparent of C), so 'using B::getr;' has to be used
	//  this is valid since B does have getr() method (inherited)
	ASSERT( m_container);
	return CodeGen::indent(h_ind) + "using " + (repl_cont.empty()?m_container->getValidNameImpl():repl_cont) + "::" + m_signature.substr(0, m_signature.find('(')) + ";";
}


std::string Method::getHidden()
{
	return CodeGen::indent(h_ind) + (m_template?"":"virtual ") + m_returnValue + " " + m_signature + 
		" { throw std::string(\"Interface inherited kind. \" + getName() + \" doesn't have " + m_signature + " method.\"); }";
	
}


Method::operator bool()
{
	if ( m_container != 0) 
		return true;
	else
		return false;
}

