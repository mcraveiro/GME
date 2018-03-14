#include "stdafx.h"
#include "CodeGen.h"
#include "Dumper.h"

/*const*/ extern int h_ind;

/*static*/ void CodeGen::folderGetterGeneric( const std::string& retval_folderkind, const std::string& folderkind, const std::string& method_name, bool extended, Any * cont, Method& m)
{
	std::string src, rtv, sgn;
	
	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + retval_folderkind + ", T>";
	sgn  = method_name + "(TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) +   "std::set<" + retval_folderkind + ", T> res;\n";
	src += indent(h_ind + 1) +   "std::set<BON::Folder> l = FolderImpl::getChildFolders();\n";
	src += indent(h_ind + 1) +   "for( std::set<BON::Folder>::iterator i = l.begin(); i != l.end(); ++i)\n";
	src += indent(h_ind + 1) +   "{\n";
	src += indent(h_ind + 2) +     retval_folderkind + " elem(*i);\n";

	if ( extended)
		src += indent(h_ind + 2) +   "if (elem)\n";
	else
		src += indent(h_ind + 2) +   "if (elem && elem->getObjectMeta().name() == \"" + folderkind + "\")\n";

	src += indent(h_ind + 3) +       "res.insert(elem);\n";
	src += indent(h_ind + 1) +   "}\n";
	src += indent(h_ind + 1) +   "return res;\n";
	src += indent(h_ind + 0) + "}\n";

	m.m_container = cont;
	m.m_template = true;
	m.m_virtual = false;
	m.m_returnValue = rtv;
	m.m_signature = sgn;
	m.m_implementation = src;
	m.m_comment = "getter for subfolder \"" + folderkind + "\"";

	return;
}


/*static*/ void CodeGen::kindGetter1Generic( const std::string& retval_kind, const std::string& kind, const std::string& method_name, Any * cont, Method& m)
{
	std::string src, rtv, sgn;;
	
	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + retval_kind + ", T>";
	sgn  = method_name + "(TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) +   "std::set<" + retval_kind + ", T> res;\n";
	src += indent(h_ind + 1) +   "std::set<BON::Object> kinds = FolderImpl::getChildObjects(\"" + kind + "\");\n";
	src += indent(h_ind + 1) +   "for( std::set<BON::Object>::iterator i = kinds.begin(); i != kinds.end(); ++i)\n";
	src += indent(h_ind + 1) +   "{\n";
	src += indent(h_ind + 2) +     retval_kind + " elem(*i);\n";
	src += indent(h_ind + 2) +     "ASSERT(elem);\n";
	src += indent(h_ind + 2) +     "res.insert(elem);\n";
	src += indent(h_ind + 1) +   "}\n";
	src += indent(h_ind + 1) +   "return res;\n";
	src += indent(h_ind + 0) + "}\n";

	m.m_template = true;
	m.m_virtual = false;
	m.m_returnValue = rtv;
	m.m_signature = sgn;
	m.m_implementation = src;
	m.m_comment = "getter for kind \"" + kind + "\"";
	m.m_container = cont;

	return;
}


/*static*/ void CodeGen::kindGetter2Generic( const std::string& retval_kind, const std::string& kind, const std::vector< std::string>& kind_vec, const std::string& method_name, Any * cont, Method& m)
{
	std::string src, rtv, sgn;;

	// the number of non-abstract fcos
	char len_s[10]; sprintf(len_s, "%u", kind_vec.size()); std::string len_str( len_s);
	
	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + retval_kind + ", T>";
	sgn  = method_name + "(TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) +   "std::set<" + retval_kind + ", T> res;\n";
	src += indent(h_ind + 1) +   "const int len = " + len_str + ";\n";
	src += indent(h_ind + 1) +   "std::set<BON::Object> kinds_vec[ len];\n";
	
	for( int k = 0; k < kind_vec.size(); ++k)
	{
		char k_s[10]; sprintf( k_s, "%i", k); std::string k_str( k_s);
		src += indent(h_ind + 1) + "kinds_vec[" + k_str + "] = FolderImpl::getChildObjects";
		src += "(\"" + kind_vec[k] + "\");\n";
	}
	src += indent(h_ind + 1) + "for( int k = 0; k < len; ++k)\n";
	src += indent(h_ind + 2) +   "for( std::set<BON::Object>::iterator i = kinds_vec[k].begin(); i != kinds_vec[k].end(); ++i)\n";
	src += indent(h_ind + 2) +   "{\n";
	src += indent(h_ind + 3) +     retval_kind + " elem(*i);\n";
	src += indent(h_ind + 3) +     "ASSERT(elem);\n";
	src += indent(h_ind + 3) +     "res.insert(elem);\n";
	src += indent(h_ind + 2) +   "}\n";
	src += indent(h_ind + 1) + "return res;\n";
	src += indent(h_ind + 0) + "}\n";

	m.m_template = true;
	m.m_virtual = false;
	m.m_returnValue = rtv;
	m.m_signature = sgn;
	m.m_implementation = src;
	m.m_comment = "aggregated getter for kind \"" + kind + "\" and its descendants";
	m.m_container = cont;
}


/*static*/ void CodeGen::kindGetter3Generic( const std::string& retval_kind, const std::string& kind, const std::string& method_name, Any * cont, Method& m)
{
	// currently not used
	std::string src, rtv, sgn;

	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + retval_kind + ", T>";
	sgn  = method_name + "_Excl(TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) + "  std::set<" + retval_kind + ", T> res;\n";
	src += indent(h_ind + 1) + "  std::set<Object> l = FolderImpl::getChildObjects(\"" + kind + "\");\n";
	src += indent(h_ind + 1) + "  for( std::set<Object>::iterator i = l.begin(); i != l.end(); ++i)\n";
	src += indent(h_ind + 1) + "  {\n";
	src += indent(h_ind + 2) + "     " + retval_kind + " elem(*i);\n";
	src += indent(h_ind + 2) + "     ASSERT(elem);\n";
	src += indent(h_ind + 2) + "     res.insert(elem);\n";
	src += indent(h_ind + 1) + "  }\n";
	src += indent(h_ind + 1) + "  return res;\n";
	src += indent(h_ind + 0) + "}\n";	

	m.m_template = true;
	m.m_virtual = false;
	m.m_returnValue = rtv;
	m.m_signature = sgn;
	m.m_implementation = src;
	m.m_container = cont;
	m.m_comment = "EXCL getter for kind \"" + kind + "\"";
}




/*static*/ void CodeGen::roleGetter1Generic( const std::string& retval_kind, const std::string& inquire, const std::string& method_name, const std::string& fco_name, Any * cont, Method& tm)
{
	std::string src, rtv, sgn;

	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + retval_kind + ", T>";
	sgn  = method_name + "(TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) +   "std::set<" + retval_kind + ", T> res;\n";
	src += indent(h_ind + 1) +   "std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs(\"" + inquire + "\");\n";
	src += indent(h_ind + 1) +   "for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)\n";
	src += indent(h_ind + 1) +   "{\n";
	src += indent(h_ind + 2) +       retval_kind + " elem(*i);\n";
	src += indent(h_ind + 2) +      "ASSERT(elem);\n";
	src += indent(h_ind + 2) +      "res.insert(elem);\n";
	src += indent(h_ind + 1) +   "}\n";
	src += indent(h_ind + 1) +   "return res;\n";
	src += indent(h_ind + 0) + "}\n";

	tm.m_virtual = false;
	tm.m_template = true;
	tm.m_returnValue = rtv;
	tm.m_signature = sgn;
	tm.m_implementation = src;
	tm.m_container = cont;
	tm.m_comment = "getter for role \"" + inquire + "\" among \"" + fco_name + "\"s";

	return;
}


/*static*/ void CodeGen::roleGetter2Generic( const std::string& retval_kind, const std::string& inquire, const std::string& method_name, const std::string& fco_name, Any * cont, Method& tm)
{
	std::string src, rtv, sgn;

	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + retval_kind + ", T>";
	sgn  = method_name + "(TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) +   "std::set<" + retval_kind + ", T> res;\n";
	src += indent(h_ind + 1) +   "std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs(\"" + inquire + "\");\n";
	src += indent(h_ind + 1) +   "for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)\n";
	src += indent(h_ind + 1) +   "{\n";
	src += indent(h_ind + 2) +      retval_kind + " elem(*i);\n";
	src += indent(h_ind + 2) +      "ASSERT(elem);\n";
	src += indent(h_ind + 2) +      "res.insert(elem);\n";
	src += indent(h_ind + 1) +   "}\n";
	src += indent(h_ind + 1) +   "return res;\n";
	src += indent(h_ind + 0) + "}\n";

	tm.m_virtual = false;
	tm.m_template = true;
	tm.m_returnValue = rtv;
	tm.m_signature = sgn;
	tm.m_implementation = src;
	tm.m_container = cont;
	tm.m_comment = "getter for role \"" + inquire + "\" among \"" + fco_name + "\"s";

	return;
}


/*static*/ void CodeGen::roleGetter3Generic( const std::string& retval_kind, const std::string& method_name, const std::string& fco_name, const std::string& role_name, const std::vector< std::string > & roles, const std::string& dummy_str, Any * cont, Method& tm)
{
	std::string src, rtv, sgn, comm;

	char len_s[10]; sprintf(len_s, "%u", roles.size()); std::string len_str( len_s);

	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + retval_kind + ", T>";
	sgn  = method_name + "(" + dummy_str + ((dummy_str.empty())?"":", ") + "TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) + "std::set<" + retval_kind + ", T> res;\n";
	src += indent(h_ind + 1) + "const int len = " + len_str + ";\n";
	src += indent(h_ind + 1) + "std::set<BON::FCO> roles_vec[ len];\n";
	
	for( int k = 0; k < roles.size(); ++k)
	{
		char k_s[10]; sprintf( k_s, "%i", k); std::string k_str( k_s);
		src += indent(h_ind + 1) + "roles_vec[" + k_str + "] = ModelImpl::getChildFCOsAs(\"" + roles[k] + "\");\n";
	}

	src += indent(h_ind + 1) + "for( int k = 0; k < len; ++k)\n";
	src += indent(h_ind + 2) +   "for( std::set<BON::FCO>::iterator i = roles_vec[k].begin(); i != roles_vec[k].end(); ++i)\n";
	src += indent(h_ind + 2) +   "{\n";
	src += indent(h_ind + 3) +      retval_kind + " elem(*i);\n";
	src += indent(h_ind + 3) +      "ASSERT(elem);\n";
	src += indent(h_ind + 3) +      "res.insert(elem);\n";
	src += indent(h_ind + 2) +   "}\n";
	src += indent(h_ind + 1) + "return res;\n";
	src += indent(h_ind + 0) + "}\n";

	tm.m_virtual = false;
	tm.m_template = true;
	tm.m_returnValue = rtv;
	tm.m_signature = sgn;
	tm.m_implementation = src;
	tm.m_container = cont;
	tm.m_comment = "aggregated getter for role \"" + role_name + "\" among \"" + fco_name + "\"s and its descendants";

	return;
}


/*static*/ void CodeGen::roleGetter4Generic( const std::string& retval_kind, const std::string& method_name, const std::string& fco_name, const std::string& desc_k_name, const std::string& role_name, const std::string& nmsp, Any * cont, Method& tm)
{
	std::string src, rtv, sgn, comm;

	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + retval_kind + ", T>";
	sgn  = method_name + "(TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) +   "std::set<" + retval_kind + ", T> res;\n";
	src += indent(h_ind + 1) +   "std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs(\"" + nmsp + desc_k_name + role_name + "\");\n";
	src += indent(h_ind + 1) +   "for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)\n";
	src += indent(h_ind + 1) +   "{\n";
	src += indent(h_ind + 2) +     retval_kind + " elem(*i);\n";
	src += indent(h_ind + 2) +     "ASSERT(elem);\n";
	src += indent(h_ind + 2) +     "res.insert(elem);\n";
	src += indent(h_ind + 1) +   "}\n";
	src += indent(h_ind + 1) +   "return res;\n";
	src += indent(h_ind + 0) + "}\n";

	tm.m_virtual = false;
	tm.m_template = true;
	tm.m_returnValue = rtv;
	tm.m_signature = sgn;
	tm.m_implementation = src;
	tm.m_container = cont;
	tm.m_comment = "getter for role \"" + nmsp + desc_k_name + role_name + "\" among \"" + nmsp + fco_name + "\"s and its descendants";

	return;
}


/*static*/ Method CodeGen::dumpGetInConnectionLinksGeneric( FCO *fco, ConnectionRep * conn)
{
	Method m;

	std::string src, rtv, sgn;

	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + conn->getLValidName() + ", T>";
	sgn  = "getIn" + conn->getValidName() + "Links(TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) +   "std::set<" + conn->getLValidName() + ", T> result;\n";
	src += indent(h_ind + 1) +   "std::set<BON::Connection> conns = ConnectionEndImpl::getInConnLinks();\n";
	src += indent(h_ind + 1) +   "std::set<BON::Connection>::iterator it = conns.begin();\n";
	src += indent(h_ind + 1) +   "for( ; it != conns.end(); ++it)\n";
	src += indent(h_ind + 1) +   "{\n";
	src += indent(h_ind + 2) +      conn->getLValidName() + " c( *it);\n";
	src += indent(h_ind + 2) +      "if (c)\n";
	src += indent(h_ind + 3) +        "result.insert( c);\n";
	src += indent(h_ind + 1) +   "}\n";
	src += indent(h_ind + 1) +   "return result;\n";
	src += indent(h_ind + 0) + "}\n";

	m.m_virtual = false;
	m.m_template = true;
	m.m_signature = sgn;
	m.m_returnValue = rtv;
	m.m_implementation = src;
	m.m_container = fco;
	m.m_comment = "";

	return m;
}

/*static*/ Method CodeGen::dumpGetOutConnectionLinksGeneric( FCO *fco, ConnectionRep * conn)
{
	Method m;

	std::string src, rtv, sgn;

	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + conn->getLValidName() + ", T>";
	sgn  = "getOut" + conn->getValidName() + "Links(TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) +   "std::set<" + conn->getLValidName() + ", T> result;\n";
	src += indent(h_ind + 1) +   "std::set<BON::Connection> conns = ConnectionEndImpl::getOutConnLinks();\n";
	src += indent(h_ind + 1) +   "std::set<BON::Connection>::iterator it = conns.begin();\n";
	src += indent(h_ind + 1) +   "for( ; it != conns.end(); ++it)\n";
	src += indent(h_ind + 1) +   "{\n";
	src += indent(h_ind + 2) +      conn->getLValidName() + " c( *it);\n";
	src += indent(h_ind + 2) +      "if (c)\n";
	src += indent(h_ind + 3) +        "result.insert( c);\n";
	src += indent(h_ind + 1) +   "}\n";
	src += indent(h_ind + 1) +   "return result;\n";
	src += indent(h_ind + 0) + "}\n";

	m.m_virtual = false;
	m.m_template = true;
	m.m_signature = sgn;
	m.m_returnValue = rtv;
	m.m_implementation = src;
	m.m_container = fco;
	m.m_comment = "";

	return m;
}

/*static*/ Method CodeGen::dumpGetBothConnectionLinksGeneric( FCO *fco, ConnectionRep * conn)
{
	Method m;

	std::string src, rtv, sgn;

	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + conn->getLValidName() + ", T>";
	sgn  = "get" + conn->getValidName() + "Links(TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) +   "std::set<" + conn->getLValidName() + ", T> result;\n";
	src += indent(h_ind + 1) +   "std::set<BON::Connection> conns = ConnectionEndImpl::getConnLinks();\n";
	src += indent(h_ind + 1) +   "std::set<BON::Connection>::iterator it = conns.begin();\n";
	src += indent(h_ind + 1) +   "for( ; it != conns.end(); ++it)\n";
	src += indent(h_ind + 1) +   "{\n";
	src += indent(h_ind + 2) +      conn->getLValidName() + " c( *it);\n";
	src += indent(h_ind + 2) +      "if (c)\n";
	src += indent(h_ind + 3) +        "result.insert( c);\n";
	src += indent(h_ind + 1) +   "}\n";
	src += indent(h_ind + 1) +   "return result;\n";
	src += indent(h_ind + 0) + "}\n";

	m.m_virtual = false;
	m.m_template = true;
	m.m_signature = sgn;
	m.m_returnValue = rtv;
	m.m_implementation = src;
	m.m_container = fco;
	m.m_comment = "";

	return m;
}


/*static*/ Method CodeGen::dumpGetInConnectionEndGeneric( FCO *fco, FCO* peer, ConnectionRep* conn, bool peer_may_be_refport)
{
	Method m;
	std::string peer_lcd;
	if (peer)
		peer_lcd = peer->getLValidName();
	else
		peer_lcd = "BON::FCO";

	std::string src, rtv, sgn;

	if ( peer_may_be_refport)
	{
		// if reference ports may be conn ends than the implementation is so simple
		rtv  = indent(h_ind + 0) + "template <class T>\n";
		rtv += indent(h_ind + 0) + "std::multiset<BON::ConnectionEnd, T>";
		sgn  = "get" + conn->getValidName() + "Srcs(TDP)";
		src  = indent(h_ind + 0) + "{\n";
		src += indent(h_ind + 1) +   "std::multiset<BON::ConnectionEnd> ends = BON::ConnectionEndImpl::getInConnEnds(\"" + conn->getLStrictName() + "\");\n";
		src += indent(h_ind + 1) +   "return std::multiset<BON::ConnectionEnd, T>( ends.begin(), ends.end());\n";
		src += indent(h_ind + 0) + "}\n";
		
		m.m_virtual = false;
		m.m_template = true;
		m.m_signature = sgn;
		m.m_returnValue = rtv;
		m.m_implementation = src;
		m.m_container = fco;
		m.m_comment = "returns src " + peer_lcd + "s and referenceports";
	}
	else
	{
		// if we know that no reference ports are between the connends
		rtv  = indent(h_ind + 0) + "template <class T>\n";
		rtv += indent(h_ind + 0) + "std::multiset<" + peer_lcd + ", T>";
		sgn  = "get" + conn->getValidName() + "Srcs(TDP)";
		src  = indent(h_ind + 0) + "{\n";
		src += indent(h_ind + 1) +   "std::multiset<" + peer_lcd + ", T> res;\n";
		src += indent(h_ind + 1) +   "{\n";
		// the name getInConnEnds is confusing but does the right thing
		src += indent(h_ind + 2) +     "std::multiset<BON::ConnectionEnd> in_ends = BON::ConnectionEndImpl::getInConnEnds(\"" + conn->getLStrictName() + "\");\n";
		src += indent(h_ind + 2) +     "for ( std::multiset<BON::ConnectionEnd>::iterator cit = in_ends.begin() ; cit != in_ends.end() ; ++cit )\n";
		src += indent(h_ind + 2) +     "{\n";
		src += indent(h_ind + 3) +       peer_lcd + " dst( *cit );\n";
		src += indent(h_ind + 3) +       "ASSERT(dst);\n";
		src += indent(h_ind + 3) +       "res.insert( dst);\n";
		src += indent(h_ind + 2) +     "}\n";
		src += indent(h_ind + 1) +   "}\n";
		src += indent(h_ind + 1) +   "return res;\n";
		src += indent(h_ind + 0) + "}\n";

		m.m_virtual = false;
		m.m_template = true;
		m.m_signature = sgn;
		m.m_returnValue = rtv;
		m.m_implementation = src;
		m.m_container = fco;
		m.m_comment = "returns src " + peer_lcd + "s";;
	}
	return m;
}

/*static*/ Method CodeGen::dumpGetOutConnectionEndGeneric( FCO *fco, FCO* peer, ConnectionRep* conn, bool peer_may_be_refport)
{
	Method m;
	std::string peer_lcd;
	if (peer)
		peer_lcd = peer->getLValidName();
	else
		peer_lcd = "BON::FCO";

	std::string src, rtv, sgn;

	if ( peer_may_be_refport)
	{
		// if reference ports may be conn ends than the implementation is so simple
		rtv  = indent(h_ind + 0) + "template <class T>\n";
		rtv += indent(h_ind + 0) + "std::multiset<BON::ConnectionEnd, T>";
		sgn  = "get" + conn->getValidName() + "Dsts(TDP)";
		src  = indent(h_ind + 0) + "{\n";
		src += indent(h_ind + 1) +   "std::multiset<BON::ConnectionEnd> ends = BON::ConnectionEndImpl::getOutConnEnds(\"" + conn->getLStrictName() + "\");\n";
		src += indent(h_ind + 1) +   "return std::multiset<BON::ConnectionEnd, T>( ends.begin(), ends.end());\n";
		src += indent(h_ind + 0) + "}\n";

		
		m.m_virtual = false;
		m.m_template = true;
		m.m_signature = sgn;
		m.m_returnValue = rtv;
		m.m_implementation = src;
		m.m_container = fco;
		m.m_comment = "returns dst " + peer_lcd + "s and referenceports";
	}
	else
	{
		// if we know that no reference ports are between the connends
		rtv  = indent(h_ind + 0) + "template <class T>\n";
		rtv += indent(h_ind + 0) + "std::multiset<" + peer_lcd + ", T>";
		sgn  = "get" + conn->getValidName() + "Dsts(TDP)";
		src  = indent(h_ind + 0) + "{\n";
		src += indent(h_ind + 1) +   "std::multiset<" + peer_lcd + ", T> res;\n";
		src += indent(h_ind + 1) +   "{\n";
		// the name getOutConnEnds is confusing but does the right thing
		src += indent(h_ind + 2) +     "std::multiset<BON::ConnectionEnd> out_ends = BON::ConnectionEndImpl::getOutConnEnds(\"" + conn->getLStrictName() + "\");\n";
		src += indent(h_ind + 2) +     "for ( std::multiset<BON::ConnectionEnd>::iterator cit = out_ends.begin() ; cit != out_ends.end() ; ++cit )\n";
		src += indent(h_ind + 2) +     "{\n";
		src += indent(h_ind + 3) +       peer_lcd + " dst( *cit );\n";
		src += indent(h_ind + 3) +       "ASSERT(dst);\n";
		src += indent(h_ind + 3) +       "res.insert( dst);\n";
		src += indent(h_ind + 2) +     "}\n";
		src += indent(h_ind + 1) +   "}\n";
		src += indent(h_ind + 1) +   "return res;\n";
		src += indent(h_ind + 0) + "}\n";

		m.m_virtual = false;
		m.m_template = true;
		m.m_signature = sgn;
		m.m_returnValue = rtv;
		m.m_implementation = src;
		m.m_container = fco;
		m.m_comment = "returns dst " + peer_lcd + "s";
	}
	return m;
}


/*static*/ Method CodeGen::dumpGetBothConnectionEndGeneric( FCO *fco, FCO* peer, ConnectionRep* conn, bool peer_may_be_refport)
{
	Method m;
	std::string peer_lcd;
	if (peer)
		peer_lcd = peer->getLValidName();
	else
		peer_lcd = "BON::FCO";

	std::string src, rtv, sgn;

	if ( peer_may_be_refport)
	{
		// if reference ports may be conn ends than the implementation is so simple
		rtv  = indent(h_ind + 0) + "template <class T>\n";
		rtv += indent(h_ind + 0) + "std::multiset<BON::ConnectionEnd, T>";
		sgn  = "get" + conn->getValidName() + "Ends(TDP)";
		src  = indent(h_ind + 0) + "{\n";
		src += indent(h_ind + 1) +   "std::multiset<BON::ConnectionEnd> ends = BON::ConnectionEndImpl::getConnEnds(\"" + conn->getLStrictName() + "\");\n";
		src += indent(h_ind + 1) +   "return std::multiset<BON::ConnectionEnd, T>( ends.begin(), ends.end());\n";
		src += indent(h_ind + 0) + "}\n";
		
		
		m.m_virtual = false;
		m.m_template = true;
		m.m_signature = sgn;
		m.m_returnValue = rtv;
		m.m_implementation = src;
		m.m_container = fco;
		m.m_comment = "returns src and dst " + peer_lcd + "s and referenceports";
	}
	else
	{
		rtv  = indent(h_ind + 0) + "template <class T>\n";
		rtv += indent(h_ind + 0) + "std::multiset<" + peer_lcd + ", T>";
		sgn  = "get" + conn->getValidName() + "Ends(TDP)";
		src  = indent(h_ind + 0) + "{\n";
		src += indent(h_ind + 1) +   "std::multiset<" + peer_lcd + ", T> res;\n";
		src += indent(h_ind + 1) +   "{\n";
		src += indent(h_ind + 2) +     "std::multiset<BON::ConnectionEnd> in_ends = BON::ConnectionEndImpl::getInConnEnds(\"" + conn->getLStrictName() + "\");\n";
		src += indent(h_ind + 2) +     "for ( std::multiset<BON::ConnectionEnd>::iterator cit = in_ends.begin() ; cit != in_ends.end() ; ++cit )\n";
		src += indent(h_ind + 2) +     "{\n";
		src += indent(h_ind + 3) +       peer_lcd + " dst( *cit );\n";
		src += indent(h_ind + 3) +       "ASSERT(dst);\n";
		src += indent(h_ind + 3) +       "res.insert( dst);\n";
		src += indent(h_ind + 2) +     "}\n";
		src += indent(h_ind + 2) +     "std::multiset<BON::ConnectionEnd> out_ends = BON::ConnectionEndImpl::getOutConnEnds(\"" + conn->getLStrictName() + "\");\n";
		src += indent(h_ind + 2) +     "for ( std::multiset<BON::ConnectionEnd>::iterator cot = out_ends.begin() ; cot != out_ends.end() ; ++cot )\n";
		src += indent(h_ind + 2) +     "{\n";
		src += indent(h_ind + 3) +       peer_lcd + " dst( *cot );\n";
		src += indent(h_ind + 3) +       "ASSERT(dst);\n";
		src += indent(h_ind + 3) +       "res.insert( dst);\n";
		src += indent(h_ind + 2) +     "}\n";
		src += indent(h_ind + 1) +   "}\n";
		src += indent(h_ind + 1) +   "return res;\n";
		src += indent(h_ind + 0) + "}\n";

		m.m_virtual = false;
		m.m_template = true;
		m.m_signature = sgn;
		m.m_returnValue = rtv;
		m.m_implementation = src;
		m.m_container = fco;
		m.m_comment = "returns src and dst " + peer_lcd + "s";
	}
	return m;
}


/*static*/ Method CodeGen::dumpSetGetterGeneric( SetRep * cont, const FCO * fco, const std::string& common_kind, bool aggreg /*=false*/, bool dummy_par /*=false*/)
{
	ASSERT( fco || ( aggreg && !common_kind.empty())); //assert if fco is 0 and aggreg is false
	std::string retval_kind; // the return value cannot be "Compound" if Compound is not extended

	if (fco)
	{
		retval_kind = fco->getLValidName();

		if ( !fco->isToBeEx()) 
		{
			FCO * ext_anc = fco->getExtedAnc();
			if ( ext_anc)
				retval_kind = ext_anc->getLValidName();
			else
				retval_kind = "BON::" + Any::KIND_TYPE_STR[fco->getMyKind()];
		}
	}
	else if ( !common_kind.empty()) // using the common_kind if set
		retval_kind = common_kind;
	else // not intended for usage in such cases
		ASSERT(0);

	Method m;

	std::string src, rtv, sgn;

	rtv  = indent(h_ind + 0) + "template <class T>\n";
	rtv += indent(h_ind + 0) + "std::set<" + retval_kind + ", T>";
	sgn  = cont->setGetterTemplate( fco) + "(" + (dummy_par?" int dummy, ":"") + "TDP)";
	src  = indent(h_ind + 0) + "{\n";
	src += indent(h_ind + 1) +   "std::set<" + retval_kind + ", T> res;\n";
	src += indent(h_ind + 1) +   "std::set<BON::FCO> elems = BON::SetImpl::getMembers();\n";
	src += indent(h_ind + 1) +   "std::set<BON::FCO>::iterator elem = elems.begin();\n";
	src += indent(h_ind + 1) +   "for( ; elem != elems.end(); ++elem)\n";
	src += indent(h_ind + 1) +   "{\n";
	src += indent(h_ind + 2) +     retval_kind + " r( *elem);\n";

	if ( fco)
	{
		if ( !aggreg) // not aggregated getter
		{
			src += indent(h_ind + 2) +   "if ( r && r->getObjectMeta().name() == \"" + fco->getValidName() + "\")\n";
		}
		else // aggregated getter, casting to the common base, which is not a common kind
		{
			src += indent(h_ind + 2) + "if ( r)\n";
		}
	}
	else // aggregated getter casting to the common kind
		src += indent(h_ind + 2) +   "if ( r)\n";

	src += indent(h_ind + 3) +       "res.insert( r);\n";
	src += indent(h_ind + 1) +   "}\n";
	src += indent(h_ind + 1) +   "return res;\n";
	src += indent(h_ind + 0) + "}\n";

	m.m_virtual = false;
	m.m_template = true;
	m.m_returnValue = rtv;
	m.m_signature = sgn;
	m.m_implementation = src;
	m.m_container = cont;
	if ( !aggreg)
		m.m_comment = "specialized getter for " + fco->getValidName() + " setmembers";
	else //aggreg
		m.m_comment = "aggregated getter for setmembers";

	return m;
}

