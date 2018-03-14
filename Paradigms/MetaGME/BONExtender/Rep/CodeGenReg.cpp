#include "stdafx.h"
#include "CodeGen.h"
#include "Dumper.h"

/*const*/ extern int s_ind;


/*static*/ void CodeGen::folderGetter( const std::string& retval_folderkind, const std::string& folderkind, const std::string& method_name, bool extended, Any * cont, Method& m)
{
	std::string src, comm;
	
	src  = indent(s_ind + 0) + "{\n";
	src += indent(s_ind + 1) +   "std::set<" + retval_folderkind + "> res;\n";
	src += indent(s_ind + 1) +   "std::set<BON::Folder> l = FolderImpl::getChildFolders();\n";
	src += indent(s_ind + 1) +   "for( std::set<BON::Folder>::iterator i = l.begin(); i != l.end(); ++i)\n";
	src += indent(s_ind + 1) +   "{\n";
	src += indent(s_ind + 2) +     retval_folderkind + " elem(*i);\n";
	if ( extended)
		src += indent(s_ind + 2) +   "if (elem)\n";
	else
		src += indent(s_ind + 2) +   "if (elem && elem->getObjectMeta().name() == \"" + folderkind + "\")\n";
	src += indent(s_ind + 3) +       "res.insert(elem);\n";
	src += indent(s_ind + 1) +   "}\n";
	src += indent(s_ind + 1) +   "return res;\n";
	src += indent(s_ind + 0) + "}\n\n\n";

	comm = "getter for subfolder \"" + folderkind + "\"";

	m.m_virtual = true;
	m.m_returnValue = "std::set<" + retval_folderkind + ">";
	m.m_signature = method_name + "()";
	m.m_implementation = src;
	m.m_container = cont;
	m.m_comment = comm;

	return;
}


/*static*/ void CodeGen::kindGetter1( const std::string& retval_kind, const std::string& kind, const std::string& method_name, Any * cont, Method& m)
{
	std::string src, comm;
	
	comm = "getter for kind \"" + kind + "\"";
	src  = indent(s_ind + 0) + "{\n";
	src += indent(s_ind + 1) +   "std::set<" + retval_kind + "> res;\n";
	src += indent(s_ind + 1) +   "std::set<BON::Object> kinds = FolderImpl::getChildObjects";
	src +=               "(\"" + kind + "\");\n";
	src += indent(s_ind + 1) +   "for( std::set<BON::Object>::iterator i = kinds.begin(); i != kinds.end(); ++i)\n";
	src += indent(s_ind + 1) +   "{\n";
	src += indent(s_ind + 2) +     retval_kind + " elem(*i);\n";
	src += indent(s_ind + 2) +     "ASSERT(elem);\n";
	src += indent(s_ind + 2) +     "res.insert(elem);\n";
	src += indent(s_ind + 1) +   "}\n";
	src += indent(s_ind + 1) +   "return res;\n";
	src += indent(s_ind + 0) + "}\n\n\n";

	m.m_virtual = true;
	m.m_returnValue = "std::set<" + retval_kind + ">";
	m.m_signature = method_name + "()";
	m.m_implementation = src;
	m.m_container = cont;
	m.m_comment = comm;

	return;
}


/*static*/ void CodeGen::kindGetter2( const std::string& retval_kind, const std::string& kind, const std::vector< std::string>& kind_vec, const std::string& method_name, Any * cont, Method& m)
{
	std::string src, comm;

	// the number of non-abstract fcos
	char len_s[10]; sprintf(len_s, "%u", kind_vec.size()); std::string len_str( len_s);
	
	src  = indent(s_ind + 0) + "{\n";
	src += indent(s_ind + 1) +   "std::set<" + retval_kind + "> res;\n";
	src += indent(s_ind + 1) +   "const int len = " + len_str + ";\n";
	src += indent(s_ind + 1) +   "std::set<BON::Object> kinds_vec[ len];\n";
	
	for( int k = 0; k < kind_vec.size(); ++k)
	{
		char k_s[10]; sprintf( k_s, "%i", k); std::string k_str( k_s);
		src += indent(s_ind + 1) + "kinds_vec[" + k_str + "] = FolderImpl::getChildObjects";
		src += "(\"" + kind_vec[k] + "\");\n";
	}
	src += indent(s_ind + 1) + "for( int k = 0; k < len; ++k)\n";
	src += indent(s_ind + 2) +   "for( std::set<BON::Object>::iterator i = kinds_vec[k].begin(); i != kinds_vec[k].end(); ++i)\n";
	src += indent(s_ind + 2) +   "{\n";
	src += indent(s_ind + 3) +     retval_kind + " elem(*i);\n";
	src += indent(s_ind + 3) +     "ASSERT(elem);\n";
	src += indent(s_ind + 3) +     "res.insert(elem);\n";
	src += indent(s_ind + 2) +   "}\n";
	src += indent(s_ind + 1) + "return res;\n";
	src += indent(s_ind + 0) + "}\n\n\n";	

	comm = "aggregated getter for kind \"" + kind + "\" and its descendants";

	m.m_virtual = true;
	m.m_returnValue = "std::set<" + retval_kind + ">";
	m.m_signature = method_name + "()";
	m.m_implementation = src;
	m.m_container = cont;
	m.m_comment = comm;
}


/*static*/ void CodeGen::kindGetter3( const std::string& retval_kind, const std::string& kind, const std::string& method_name, Any * cont, Method& m)
{
	// currently not used
	std::string src, comm;

	comm = "EXCL getter for kind \"" + kind + "\"";

	src  = "{\n";
	src += "  std::set<" + retval_kind + "> res;\n";
	src += "  std::set<Object> l = FolderImpl::getChildObjects";
	src += "(\"" + kind + "\");\n";
	src += "  for( std::set<Object>::iterator i = l.begin(); i != l.end(); ++i)\n";
	src += "  {\n";
	src += "     " + retval_kind + " elem(*i);\n";
	src += "     ASSERT(elem);\n";
	src += "     res.insert(elem);\n";
	src += "  }\n";
	src += "  return res;\n";
	src += "}\n\n\n";	

	m.m_virtual = true;
	m.m_returnValue = "std::set<" + retval_kind + ">";
	m.m_signature = method_name + "_Excl()";
	m.m_implementation = src;
	m.m_container = cont;
	m.m_comment = comm;
}

/*static*/ void CodeGen::roleGetter1( const std::string& retval_kind, const std::string& inquire, const std::string& method_name, const std::string& fco_name, Any * cont, Method& m)
{
	std::string src, comm;

	src  = indent(s_ind + 0) + "{\n";
	src += indent(s_ind + 1) +   "std::set<" + retval_kind + "> res;\n";
	src += indent(s_ind + 1) +   "std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs";
	src +=                       "(\"" + inquire + "\");\n";
	src += indent(s_ind + 1) +   "for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)\n";
	src += indent(s_ind + 1) +   "{\n";
	src += indent(s_ind + 2) +       retval_kind + " elem(*i);\n";
	src += indent(s_ind + 2) +      "ASSERT(elem);\n";
	src += indent(s_ind + 2) +      "res.insert(elem);\n";
	src += indent(s_ind + 1) +   "}\n";
	src += indent(s_ind + 1) +   "return res;\n";
	src += indent(s_ind + 0) + "}\n\n\n";

	comm =  "getter for role \"" + inquire + "\" among \"" + fco_name + "\"s";

	m.m_virtual = true;
	m.m_returnValue = "std::set<" + retval_kind + ">";
	m.m_signature = method_name + "()";
	m.m_implementation = src;
	m.m_container = cont;
	m.m_comment = comm;

	return;
}


/*static*/ void CodeGen::roleGetter2( const std::string& retval_kind, const std::string& inquire, const std::string& method_name, const std::string& fco_name, Any * cont, Method& m)
{
	std::string src, comm;

	src  = indent(s_ind + 0) + "{\n";
	src += indent(s_ind + 1) +   "std::set<" + retval_kind + "> res;\n";
	src += indent(s_ind + 1) +   "std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs";
	src +=                       "(\"" + inquire + "\");\n";
	src += indent(s_ind + 1) +   "for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)\n";
	src += indent(s_ind + 1) +   "{\n";
	src += indent(s_ind + 2) +      retval_kind + " elem(*i);\n";
	src += indent(s_ind + 2) +      "ASSERT(elem);\n";
	src += indent(s_ind + 2) +      "res.insert(elem);\n";
	src += indent(s_ind + 1) +   "}\n";
	src += indent(s_ind + 1) +   "return res;\n";
	src += indent(s_ind + 0) + "}\n\n\n";

	comm = "getter for role \"" + inquire + "\" among \"" + fco_name + "\"s";

	m.m_virtual = true;
	m.m_returnValue = "std::set<" + retval_kind + ">";
	m.m_signature = method_name + "()";
	m.m_implementation = src;
	m.m_container = cont;
	m.m_comment = comm;

	return;
}


/*static*/ void CodeGen::roleGetter3( const std::string& retval_kind, const std::string& method_name, const std::string& fco_name, const std::string& role_name, const std::vector< std::string > & roles, const std::string& dummy_str, Any * cont, Method& m)
{
	std::string src, comm;

	char len_s[10]; sprintf(len_s, "%u", roles.size()); std::string len_str( len_s);

	src  = indent(s_ind + 0) + "{\n";
	src += indent(s_ind + 1) +   "std::set<" + retval_kind + "> res;\n";
	src += indent(s_ind + 1) +   "const int len = " + len_str + ";\n";
	src += indent(s_ind + 1) +   "std::set<BON::FCO> roles_vec[ len];\n";
	
	for( int k = 0; k < roles.size(); ++k)
	{
		char k_s[10]; sprintf( k_s, "%i", k); std::string k_str( k_s);
		src += indent(s_ind + 1) + "roles_vec[" + k_str + "] = ModelImpl::getChildFCOsAs";
		src += "(\"";
		src += roles[k];
		src += "\");\n";
	}

	src += indent(s_ind + 1) + "for( int k = 0; k < len; ++k)\n";
	src += indent(s_ind + 2) +   "for( std::set<BON::FCO>::iterator i = roles_vec[k].begin(); i != roles_vec[k].end(); ++i)\n";
	src += indent(s_ind + 2) +   "{\n";
	src += indent(s_ind + 3) +      retval_kind + " elem(*i);\n";
	src += indent(s_ind + 3) +      "ASSERT(elem);\n";
	src += indent(s_ind + 3) +      "res.insert(elem);\n";
	src += indent(s_ind + 2) +   "}\n";
	src += indent(s_ind + 1) + "return res;\n";
	src += indent(s_ind + 0) + "}\n\n\n";	

	comm = "aggregated getter for role \"" + role_name + "\" among \"" + fco_name + "\"s and its descendants";

	m.m_virtual = true;
	m.m_returnValue = "std::set<" + retval_kind + ">"; 
	m.m_signature = method_name + "(" + dummy_str + ")";
	m.m_implementation = src;
	m.m_container = cont;
	m.m_comment = comm;

	return;
}


/*static*/ void CodeGen::roleGetter4( const std::string& retval_kind, const std::string& method_name, const std::string& fco_name, const std::string& desc_k_name, const std::string& role_name, const std::string& nmsp, Any * cont, Method& m)
{
	std::string src, comm;

	src  = indent(s_ind + 0) + "{\n";
	src += indent(s_ind + 1) +   "std::set<" + retval_kind + "> res;\n";
	src += indent(s_ind + 1) +   "std::set<BON::FCO> roles = ModelImpl::getChildFCOsAs";
	src +=                       "(\"";
	src +=                              nmsp + desc_k_name + role_name;
	src +=                       "\");\n";
	src += indent(s_ind + 1) +   "for( std::set<BON::FCO>::iterator i = roles.begin(); i != roles.end(); ++i)\n";
	src += indent(s_ind + 1) +   "{\n";
	src += indent(s_ind + 2) +     retval_kind + " elem(*i);\n";
	src += indent(s_ind + 2) +     "ASSERT(elem);\n";
	src += indent(s_ind + 2) +     "res.insert(elem);\n";
	src += indent(s_ind + 1) +   "}\n";
	src += indent(s_ind + 1) +   "return res;\n";
	src += indent(s_ind + 0) + "}\n\n\n";

	comm = "getter for role \"" + nmsp + desc_k_name + role_name + "\" among \"" + nmsp + fco_name + "\"s and its descendants";

	m.m_virtual = true;
	m.m_returnValue = "std::set<" + retval_kind + ">";
	m.m_signature = method_name + "()";
	m.m_implementation = src;
	m.m_container = cont;
	m.m_comment = comm;

	return;
}


/*static*/ Method CodeGen::dumpGetInConnectionLinks( FCO *fco, ConnectionRep * conn)
{
	Method m;

	std::string mmm;

	mmm  = indent(s_ind + 0) + "{\n";
	mmm += indent(s_ind + 1) +   "std::set<" + conn->getLValidName() + "> result;\n";
	mmm += indent(s_ind + 1) +   "std::set<BON::Connection> conns = ConnectionEndImpl::getInConnLinks();\n";
	mmm += indent(s_ind + 1) +   "std::set<BON::Connection>::iterator it = conns.begin();\n";
	mmm += indent(s_ind + 1) +   "for( ; it != conns.end(); ++it)\n";
	mmm += indent(s_ind + 1) +   "{\n";
	mmm += indent(s_ind + 2) +     conn->getLValidName() + " c( *it);\n";
	mmm += indent(s_ind + 2) +     "if (c)\n";
	mmm += indent(s_ind + 3) +       "result.insert( c);\n";
	mmm += indent(s_ind + 1) +   "}\n";
	mmm += indent(s_ind + 1) +   "return result;\n";
	mmm += indent(s_ind + 0) + "}\n\n\n";

	m.m_signature = "getIn" + conn->getValidName() + "Links()";
	m.m_returnValue = "std::set<" + conn->getLValidName() + ">";
	m.m_implementation = mmm;
	m.m_container = fco;
	m.m_comment = "";

	return m;
}



/*static*/ Method CodeGen::dumpGetOutConnectionLinks( FCO *fco, ConnectionRep * conn)
{
	Method m;

	std::string mmm;

	mmm  = indent(s_ind + 0) + "{\n";
	mmm += indent(s_ind + 1) +   "std::set<" + conn->getLValidName() + "> result;\n";
	mmm += indent(s_ind + 1) +   "std::set<BON::Connection> conns = ConnectionEndImpl::getOutConnLinks();\n";
	mmm += indent(s_ind + 1) +   "std::set<BON::Connection>::iterator it = conns.begin();\n";
	mmm += indent(s_ind + 1) +   "for( ; it != conns.end(); ++it)\n";
	mmm += indent(s_ind + 1) +   "{\n";
	mmm += indent(s_ind + 2) +      conn->getLValidName() + " c( *it);\n";
	mmm += indent(s_ind + 2) +      "if (c)\n";
	mmm += indent(s_ind + 3) +        "result.insert( c);\n";
	mmm += indent(s_ind + 1) +   "}\n";
	mmm += indent(s_ind + 1) +   "return result;\n";
	mmm += indent(s_ind + 0) + "}\n\n\n";

	m.m_signature = "getOut" + conn->getValidName() + "Links()";
	m.m_returnValue = "std::set<" + conn->getLValidName() + ">";
	m.m_implementation = mmm;
	m.m_container = fco;
	m.m_comment = "";

	return m;
}



/*static*/ Method CodeGen::dumpGetBothConnectionLinks( FCO *fco, ConnectionRep * conn)
{
	Method m;

	std::string mmm;

	mmm  = indent(s_ind + 0) + "{\n";
	mmm += indent(s_ind + 1) +   "std::set<" + conn->getLValidName() + "> result;\n";
	mmm += indent(s_ind + 1) +   "std::set<BON::Connection> conns = ConnectionEndImpl::getConnLinks();\n";
	mmm += indent(s_ind + 1) +   "std::set<BON::Connection>::iterator it = conns.begin();\n";
	mmm += indent(s_ind + 1) +   "for( ; it != conns.end(); ++it)\n";
	mmm += indent(s_ind + 1) +   "{\n";
	mmm += indent(s_ind + 2) +     conn->getLValidName() + " c( *it);\n";
	mmm += indent(s_ind + 2) +     "if (c)\n";
	mmm += indent(s_ind + 3) +       "result.insert( c);\n";
	mmm += indent(s_ind + 1) +   "}\n";
	mmm += indent(s_ind + 1) +   "return result;\n";
	mmm += indent(s_ind + 0) + "}\n\n\n";

	m.m_signature = "get" + conn->getValidName() + "Links()";
	m.m_returnValue = "std::set<" + conn->getLValidName() + ">";
	m.m_implementation = mmm;
	m.m_container = fco;
	m.m_comment = "";

	return m;
}


/*static*/ Method CodeGen::dumpGetInConnectionEnd( FCO *fco, FCO* peer, ConnectionRep* conn, bool peer_may_be_refport)
{
	Method m;
	std::string peer_lcd;
	if (peer)
		peer_lcd = peer->getLValidName();
	else
		peer_lcd = "BON::FCO";

	std::string mmm, nnn;

	if ( peer_may_be_refport)
	{
		// if reference ports may be conn ends than the implementation is so simple
		nnn  = indent(s_ind + 0) + "{\n";
		nnn += indent(s_ind + 1) +   "return BON::ConnectionEndImpl::getInConnEnds(\"" + conn->getLStrictName() + "\");\n";
		nnn += indent(s_ind + 0) + "}\n\n\n";

		m.m_signature = "get" + conn->getValidName() + "Srcs()";
		m.m_returnValue = "std::multiset<BON::ConnectionEnd>";
		m.m_implementation = nnn;
		m.m_container = fco;
		m.m_comment = "returns src " + peer_lcd + "s and referenceports";
	}
	else
	{
		// if we know that no reference ports are between the connends
		mmm  = indent(s_ind + 0) + "{\n";
		mmm += indent(s_ind + 1) +   "std::multiset<" + peer_lcd + "> res;\n";
		mmm += indent(s_ind + 1) +   "{\n";
		// the name getInConnEnds is confusing but does the right thing
		mmm += indent(s_ind + 2) +     "std::multiset<BON::ConnectionEnd> in_ends = BON::ConnectionEndImpl::getInConnEnds(\"" + conn->getLStrictName() + "\");\n";
		mmm += indent(s_ind + 2) +     "for ( std::multiset<BON::ConnectionEnd>::iterator cit = in_ends.begin() ; cit != in_ends.end() ; ++cit )\n";
		mmm += indent(s_ind + 2) +     "{\n";
		mmm += indent(s_ind + 3) +       peer_lcd + " dst( *cit );\n";
		mmm += indent(s_ind + 3) +       "ASSERT(dst);\n";
		mmm += indent(s_ind + 3) +       "res.insert( dst);\n";
		mmm += indent(s_ind + 2) +     "}\n";
		mmm += indent(s_ind + 1) +   "}\n";
		mmm += indent(s_ind + 1) +   "return res;\n";
		mmm += indent(s_ind + 0) + "}\n\n\n";

		m.m_signature = "get" + conn->getValidName() + "Srcs()";
		m.m_returnValue = "std::multiset<" + peer_lcd + ">";
		m.m_implementation = mmm;
		m.m_container = fco;
		m.m_comment = "returns src " + peer_lcd + "s";
	}
	return m;
}



/*static*/ Method CodeGen::dumpGetOutConnectionEnd( FCO *fco, FCO* peer, ConnectionRep* conn, bool peer_may_be_refport)
{
	Method m;
	std::string peer_lcd;
	if (peer)
		peer_lcd = peer->getLValidName();
	else
		peer_lcd = "BON::FCO";

	std::string mmm, nnn;

	if ( peer_may_be_refport)
	{
		nnn  = indent(s_ind + 0) + "{\n";
		nnn += indent(s_ind + 1) +   "return BON::ConnectionEndImpl::getOutConnEnds(\"" + conn->getLStrictName() + "\");\n";
		nnn += indent(s_ind + 0) + "}\n\n\n";

		m.m_signature = "get" + conn->getValidName() + "Dsts()";
		m.m_returnValue = "std::multiset<BON::ConnectionEnd>";
		m.m_implementation = nnn;
		m.m_container = fco;
		m.m_comment = "returns dst " + peer_lcd + "s and referenceports";
	}
	else
	{
		mmm  = indent(s_ind + 0) + "{\n";
		mmm += indent(s_ind + 1) +   "std::multiset<" + peer_lcd + "> res;\n";
		mmm += indent(s_ind + 1) +   "{\n";
		mmm += indent(s_ind + 2) +     "std::multiset<BON::ConnectionEnd> out_ends = BON::ConnectionEndImpl::getOutConnEnds(\"" + conn->getLStrictName() + "\");\n";
		mmm += indent(s_ind + 2) +     "for ( std::multiset<BON::ConnectionEnd>::iterator cit = out_ends.begin() ; cit != out_ends.end() ; ++cit )\n";
		mmm += indent(s_ind + 2) +     "{\n";
		mmm += indent(s_ind + 3) +       peer_lcd + " dst( *cit );\n";
		mmm += indent(s_ind + 3) +       "ASSERT(dst);\n";
		mmm += indent(s_ind + 3) +       "res.insert( dst);\n";
		mmm += indent(s_ind + 2) +     "}\n";
		mmm += indent(s_ind + 1) +   "}\n";
		mmm += indent(s_ind + 1) +   "return res;\n";
		mmm += indent(s_ind + 0) + "}\n\n\n";

		m.m_signature = "get" + conn->getValidName() + "Dsts()";
		m.m_returnValue = "std::multiset<" + peer_lcd + ">";
		m.m_implementation = mmm;
		m.m_container = fco;
		m.m_comment = "returns dst " + peer_lcd + "s";
	}

	return m;
}


/*static*/ Method CodeGen::dumpGetBothConnectionEnd( FCO *fco, FCO* peer, ConnectionRep* conn, bool peer_may_be_refport)
{
	Method m;
	std::string peer_lcd;
	if (peer)
		peer_lcd = peer->getLValidName();
	else
		peer_lcd = "BON::FCO";

	std::string mmm, nnn;

	if ( peer_may_be_refport)
	{
		nnn  = indent(s_ind + 0) + "{\n";
		nnn += indent(s_ind + 1) +  "return BON::ConnectionEndImpl::getConnEnds(\"" + conn->getLStrictName() + "\");\n";
		nnn += indent(s_ind + 0) + "}\n\n\n";

		m.m_signature = "get" + conn->getValidName() + "Ends()";
		m.m_returnValue = "std::multiset<BON::ConnectionEnd>";
		m.m_implementation = nnn;
		m.m_container = fco;
		m.m_comment = "returns src and dst " + peer_lcd + "s and referenceports";
	}
	else
	{
		mmm += indent(s_ind + 0) + "{\n";
		mmm += indent(s_ind + 1) +   "std::multiset<" + peer_lcd + "> res;\n";
		mmm += indent(s_ind + 1) +   "{\n";
		mmm += indent(s_ind + 2) +     "std::multiset<BON::ConnectionEnd> in_ends = BON::ConnectionEndImpl::getInConnEnds(\"" + conn->getLStrictName() + "\");\n";
		mmm += indent(s_ind + 2) +     "for ( std::multiset<BON::ConnectionEnd>::iterator cit = in_ends.begin() ; cit != in_ends.end() ; ++cit )\n";
		mmm += indent(s_ind + 2) +     "{\n";
		mmm += indent(s_ind + 3) +       peer_lcd + " dst( *cit );\n";
		mmm += indent(s_ind + 3) +       "ASSERT(dst);\n";
		mmm += indent(s_ind + 3) +       "res.insert( dst);\n";
		mmm += indent(s_ind + 2) +     "}\n";
		mmm += indent(s_ind + 2) +     "std::multiset<BON::ConnectionEnd> out_ends = BON::ConnectionEndImpl::getOutConnEnds(\"" + conn->getLStrictName() + "\");\n";
		mmm += indent(s_ind + 2) +     "for ( std::multiset<BON::ConnectionEnd>::iterator cot = out_ends.begin() ; cot != out_ends.end() ; ++cot )\n";
		mmm += indent(s_ind + 2) +     "{\n";
		mmm += indent(s_ind + 3) +       peer_lcd + " dst( *cot );\n";
		mmm += indent(s_ind + 3) +       "ASSERT(dst);\n";
		mmm += indent(s_ind + 3) +       "res.insert( dst);\n";
		mmm += indent(s_ind + 2) +     "}\n";
		mmm += indent(s_ind + 1) +   "}\n";
		mmm += indent(s_ind + 1) +   "return res;\n";
		mmm += indent(s_ind + 0) + "}\n\n";

		m.m_signature = "get" + conn->getValidName() + "Ends()";
		m.m_returnValue = "std::multiset<" + peer_lcd + ">";
		m.m_implementation = mmm;
		m.m_container = fco;
		m.m_comment = "returns src and dst " + peer_lcd + "s";
	}
	return m;
}


/*static*/ Method CodeGen::dumpGetSrc( FCO *fco, ConnectionRep * conn, bool fco_may_be_refport)
{
	Method m;
	std::string oper1_lcd;
	if (fco && fco->isToBeEx())
		oper1_lcd = fco->getLValidName();
	else if ( fco && fco->getExtedAnc())
		oper1_lcd = fco->getExtedAnc()->getLValidName();
	else
		oper1_lcd = "BON::FCO";

	std::string mmm, nnn;

	if ( fco_may_be_refport)
	{
		nnn  = indent(s_ind + 0) + "{\n";
		nnn += indent(s_ind + 1) +   "return ConnectionImpl::getSrc();\n"; // otherwise recursive call resulted
		nnn += indent(s_ind + 0) + "}\n\n\n";

		m.m_signature = "getSrc()";
		m.m_returnValue = "BON::ConnectionEnd";
		m.m_implementation = nnn;
		m.m_container = conn;
		m.m_comment = "getSrc() return value is a ConnectionEnd";
	}
	else
	{
		mmm  = indent(s_ind + 0) + "{\n";
		mmm += indent(s_ind + 1) +   "BON::ConnectionEnd ce = ConnectionImpl::getSrc();\n"; // otherwise recursive call resulted
		mmm += indent(s_ind + 1) +   oper1_lcd + " sp( ce);\n";
		mmm += indent(s_ind + 1) +   "if ( sp)\n";
		mmm += indent(s_ind + 2) +     "return sp;\n";
		mmm += indent(s_ind + 0) + "\n";
		mmm += indent(s_ind + 1) +   oper1_lcd + " empty;\n";
		mmm += indent(s_ind + 1) +   "return empty;\n";
		mmm += indent(s_ind + 0) + "}\n\n\n";

		m.m_signature = "getSrc()";
		m.m_returnValue = oper1_lcd;
		m.m_implementation = mmm;
		m.m_container = conn;
		m.m_comment = "getSrc() return value is a ConnectionEnd casted to " + oper1_lcd;
	}
	return m;
}


/*static*/ Method CodeGen::dumpGetDst( FCO *fco, ConnectionRep * conn, bool fco_may_be_refport)
{
	Method m;
	std::string oper2_lcd;
	if (fco && fco->isToBeEx())
		oper2_lcd = fco->getLValidName();
	else if ( fco && fco->getExtedAnc())
		oper2_lcd = fco->getExtedAnc()->getLValidName();
	else
		oper2_lcd = "BON::FCO";

	std::string mmm, nnn;

	if ( fco_may_be_refport)
	{
		nnn  = indent(s_ind + 0) + "{\n";
		nnn += indent(s_ind + 1) +   "return ConnectionImpl::getDst();\n"; // otherwise recursive call would result
		nnn += indent(s_ind + 0) + "}\n\n\n";

		m.m_signature = "getDst()";
		m.m_returnValue = "BON::ConnectionEnd";
		m.m_implementation = nnn;
		m.m_container = conn;
		m.m_comment = "getDst() return value is a ConnectionEnd";
	}
	else
	{
		mmm  = indent(s_ind + 0) + "{\n";
		mmm += indent(s_ind + 1) +   "BON::ConnectionEnd ce = ConnectionImpl::getDst();\n";
		mmm += indent(s_ind + 1) +   oper2_lcd + " sp( ce);\n";
		mmm += indent(s_ind + 1) +   "if ( sp)\n";
		mmm += indent(s_ind + 2) +     "return sp;\n";
		mmm += indent(s_ind + 0) + "\n";
		mmm += indent(s_ind + 1) +   oper2_lcd + " empty;\n";
		mmm += indent(s_ind + 1) +   "return empty;\n";
		mmm += indent(s_ind + 0) + "}\n\n\n";

		m.m_signature = "getDst()";
		m.m_returnValue = oper2_lcd;
		m.m_implementation = mmm;
		m.m_container = conn;
		m.m_comment = "getDst() return value is a ConnectionEnd casted to " + oper2_lcd;
	}
	return m;
}


/*static*/ Method CodeGen::dumpSetGetter( SetRep * cont, const FCO * fco, const std::string& common_kind, bool aggreg /*=false*/, bool dummy_par /*=false*/)
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

	std::string mmm;
	mmm  = indent(s_ind + 0) + "{\n";
	mmm += indent(s_ind + 1) +   "std::set<" + retval_kind + "> res;\n";
	mmm += indent(s_ind + 1) +   "std::set<BON::FCO> elems = BON::SetImpl::getMembers();\n";
	mmm += indent(s_ind + 1) +   "std::set<BON::FCO>::iterator elem = elems.begin();\n";
	mmm += indent(s_ind + 1) +   "for( ; elem != elems.end(); ++elem)\n";
	mmm += indent(s_ind + 1) +   "{\n";
	mmm += indent(s_ind + 2) +     retval_kind + " r( *elem);\n";

	if ( fco)
	{
		if ( !aggreg) // not aggregated getter
		{
			mmm += indent(s_ind + 2) +   "if ( r && r->getObjectMeta().name() == \"" + fco->getLStrictName() + "\")\n";
		}
		else // aggregated getter, casting to the common base, which is not a common kind
		{
			mmm += indent(s_ind + 2) + "if ( r)\n";
		}
	}
	else // aggregated getter casting to the common kind
		mmm += indent(s_ind + 2) +   "if ( r)\n";

	mmm += indent(s_ind + 3) +       "res.insert( r);\n";
	mmm += indent(s_ind + 1) +   "}\n";
	mmm += indent(s_ind + 1) +   "return res;\n";
	mmm += indent(s_ind + 0) + "}\n\n\n";

	m.m_returnValue = "std::set<" + retval_kind + ">";
	m.m_signature = cont->setGetterTemplate( fco) + "(" + (dummy_par?" int dummy":"") + ")";
	m.m_implementation = mmm;
	m.m_container = cont;
	if ( !aggreg)
		m.m_comment = "specialized getter for " + fco->getLValidName() + " setmembers";
	else //aggreg
		m.m_comment = "aggregated getter for setmembers";

	return m;
}



