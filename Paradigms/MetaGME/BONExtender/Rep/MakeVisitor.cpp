#include "stdafx.h"
#include "MakeVisitor.h"
#include "Dumper.h"

#include "algorithm"

#include "globals.h"
extern Globals global_vars;

/*static*/ ClassAndNamespace ClassAndNamespace::makeIt( Any *ptr)
{
	return ClassAndNamespace( ptr->getValidName(), ptr->getValidNmspc(), ptr->getStrictNmspc());
}

ClassAndNamespace::ClassAndNamespace( const std::string& pName, const std::string& pSpace, const std::string& pStrictSpace)
	: m_name( pName)
	, m_space( pSpace)
	, m_strictSpace( pStrictSpace)
{
}

std::string ClassAndNamespace::name() const
{
	return m_name;
}

std::string ClassAndNamespace::space() const
{
	return m_space;
}

std::string ClassAndNamespace::strictSpace() const
{
	return m_strictSpace;
}



std::string ClassAndNamespace::infoName() const
{
	if( Dumper::m_iVisitSign == 1) // altered, short form like: visitComp
		return m_name;
	else                           // more informative: visitPicmlComp (namespace included)
		return m_strictSpace + m_name; // used only in places where the concatenation is informative, but not necessarily correct syntactically
}

std::string ClassAndNamespace::exactType() const
{
	return m_space + Any::NamespaceDelimiter_str + m_name;
}

void MakeVisitor::addEntity( Any::KIND_TYPE kind, const ClassAndNamespace& ent)
{
	m_entities[kind].push_back( ent);
}

std::string MakeVisitor::dumpVisitorHeader()
{
	std::string short_form_header_file;
	std::string::size_type pos = global_vars.header_file_name.rfind( '\\');
	if ( pos != std::string::npos)
	{
		if ( pos + 1 < global_vars.header_file_name.length())
			short_form_header_file = global_vars.header_file_name.substr( pos + 1);
		else
			short_form_header_file = "insert_your_specialized_header_filename_here.h";
	}
	else
		short_form_header_file = global_vars.header_file_name;

	std::string capitalized_name = m_className;
	std::transform( capitalized_name.begin(), capitalized_name.end(), capitalized_name.begin(), toupper);

	std::string mmm;
	mmm += "#ifndef " + capitalized_name + "_H\n";
	mmm += "#define " + capitalized_name + "_H\n";
	mmm += "\n";
	mmm += "#include \"BON.h\"\n";
	mmm += "#include \"BONImpl.h\"\n";
	mmm += "#include \"Extensions.h\"\n";
	mmm += "#include \"" + short_form_header_file + "\"\n";
	mmm += "\n";
	mmm += "namespace " + global_vars.m_namespace_name +"\n";
	mmm += "{\n";
	mmm += "\n";

	mmm += "class " + m_className + "\n";
	mmm += "	: public BON::Visitor\n";
	mmm += "{\n";
	//mmm += "		" + m_className + "();\n"; need any constructor?
	//mmm += "\n";

	mmm += "	public : \n";
	mmm += "		" + m_className + "();\n"; // constructor
	mmm += "		virtual ~" + m_className + "();\n\n"; // destructor
	if( Dumper::m_bGenAcceptSpeci) // custom visitor
	{
		
		// custom visitor does not need the generic visitors
		mmm += "	public :\n";

		for( unsigned int j = 0; j < HOW_MANY_ITEMS; ++j)
		for( unsigned int i = 0; i != m_entities[j].size(); ++i)
		{
			ClassAndNamespace& ent = m_entities[j][i];
			mmm += "		virtual bool visit" + ent.infoName() + "( const " + ent.exactType() + "& object );\n";
		}
		mmm += "};\n";
	}
	else
	{
		mmm += "	public :\n";

		mmm += "		void visitObjectImpl( const BON::Object& object );\n";
		if ( !m_entities[Any::FCO_REP].empty())
			mmm += "		void visitFCOImpl( const BON::FCO& fco );\n";
		if ( !m_entities[Any::ATOM].empty())
			mmm += "		void visitAtomImpl( const BON::Atom& atom );\n";
		if ( !m_entities[Any::SET].empty())
			mmm += "		void visitSetImpl( const BON::Set& set);\n";
		if ( !m_entities[Any::REF].empty())
			mmm += "		void visitReferenceImpl( const BON::Reference& ref );\n";
		if ( !m_entities[Any::CONN].empty())
			mmm += "		void visitConnectionImpl( const BON::Connection& conn);\n";
		if ( !m_entities[Any::MODEL].empty())
			mmm += "		void visitModelImpl( const BON::Model& model );\n";
		if ( !m_entities[Any::FOLDER].empty())
			mmm += "		void visitFolderImpl( const BON::Folder& fold );\n";

		mmm += "	protected :\n";

		for( unsigned int j = 0; j < HOW_MANY_ITEMS; ++j)
		for( unsigned int i = 0; i != m_entities[j].size(); ++i)
		{
			ClassAndNamespace& ent = m_entities[j][i];
			mmm += "		virtual bool visit" + ent.infoName() + "( const " + ent.exactType() + "& object );\n";
		}
		mmm += "};\n";
	}
	mmm += "\n";
	mmm += "} // namespace\n";
	mmm += "\n";
	mmm += "#endif // " + capitalized_name + "_H\n";

	return mmm;
}


std::string MakeVisitor::dumpVisitorSource( )
{
	std::string mmm;

	mmm += "#include \"stdafx.h\"\n";
	mmm += "#include \"" + m_className +".h\"\n";
	mmm += "\n";
	//mmm += "using namespace BON;\n";
	mmm += "\nnamespace " + global_vars.m_namespace_name +"\n{\n\n";
	
	// ctors
	mmm += m_className + "::" + m_className + "()  { }\n\n";
	mmm += m_className + "::~" + m_className + "() { }\n\n";

	for( unsigned int j = 0; j < HOW_MANY_ITEMS; ++j)
	for( unsigned int i = 0; i != m_entities[j].size(); ++i)
	{
		ClassAndNamespace& ent = m_entities[j][i];
		mmm += dumpSpecificMethods( ent);
	}
	
	if( !Dumper::m_bGenAcceptSpeci) // not a custom visitor
		mmm += dumpGenericMethods();

	mmm += "\n} // namespace\n\n";

	return mmm;
}


std::string MakeVisitor::dumpSpecificMethods( const ClassAndNamespace& ent)
{
	std::string mmm;
	mmm+="bool " + m_className + "::visit" + ent.infoName() + "( const " + ent.exactType() + "& object )\n";
	mmm+="{\n";
	mmm+="	if ( !object)\n";
	mmm+="		return false;\n";
	mmm+="\n";
	mmm+="	return true;\n";
	mmm+="}\n\n\n";

	return mmm;
}


std::string MakeVisitor::dumpGenericMethods()
{
	std::string mmm;


	// visitObjectImpl
	mmm+="void " + m_className + "::visitObjectImpl( const BON::Object& obj )\n";
	mmm+="{}\n\n\n";


	// visitFCOImpl
	if ( !m_entities[Any::FCO_REP].empty()) {
	mmm+="void " + m_className + "::visitFCOImpl( const BON::FCO& fco )\n";
	mmm+="{\n";
	{
		for( unsigned int i = 0; i != m_entities[Any::FCO_REP].size(); ++i) 
			mmm+="	if ( !visit" + m_entities[Any::FCO_REP][i].infoName() + "( fco))\n";
	}
	mmm+="	{\n";
	mmm+="		// visiting other fco\n";
	mmm+="	}\n";
	mmm+="}\n\n\n";
	}


	// visitAtomImpl
	if ( !m_entities[Any::ATOM].empty()) {
	mmm+="void " + m_className + "::visitAtomImpl( const BON::Atom& atom )\n";
	mmm+="{\n";
	{
		for( unsigned int i = 0; i != m_entities[Any::ATOM].size(); ++i) 
			mmm+="	if ( !visit" + m_entities[Any::ATOM][i].infoName() + "( atom))\n";
	}
	mmm+="	{\n";
	mmm+="		// visiting other Atom\n";
	mmm+="	}\n";
	mmm+="}\n\n\n";
	}


	// visitSetImpl
	if ( !m_entities[Any::SET].empty()) {
	mmm+="void " + m_className + "::visitSetImpl( const BON::Set& set )\n";
	mmm+="{\n";
	{
		for( unsigned int i = 0; i != m_entities[Any::SET].size(); ++i) 
			mmm+="	if ( !visit" + m_entities[Any::SET][i].infoName() + "( set))\n";
	}
	mmm+="	{\n";
	mmm+="		// visiting other Set\n";
	mmm+="	}\n";
	mmm+="}\n\n\n";
	}


	// visitReferenceImpl
	if ( !m_entities[Any::REF].empty()) {
	mmm+="void " + m_className + "::visitReferenceImpl( const BON::Reference& ref )\n";
	mmm+="{\n";
	{
		for( unsigned int i = 0; i != m_entities[Any::REF].size(); ++i) 
			mmm+="	if ( !visit" + m_entities[Any::REF][i].infoName() + "( ref))\n";
	}
	mmm+="	{\n";
	mmm+="		// visiting other Reference\n";
	mmm+="	}\n";
	mmm+="}\n\n\n";
	}


	// visitConnectionImpl
	if ( !m_entities[Any::CONN].empty()) {
	mmm+="void " + m_className + "::visitConnectionImpl( const BON::Connection& conn )\n";
	mmm+="{\n";
	{
		for( unsigned int i = 0; i != m_entities[Any::CONN].size(); ++i) 
			mmm+="	if ( !visit" + m_entities[Any::CONN][i].infoName() + "( conn))\n";
	}
	mmm+="	{\n";
	mmm+="		// visiting other Connection\n";
	mmm+="	}\n";
	mmm+="}\n\n\n";
	}

	
	// visitModelImpl
	if ( !m_entities[Any::MODEL].empty()) {
	mmm+="void " + m_className + "::visitModelImpl( const BON::Model& model )\n";
	mmm+="{\n";
	{
		for( unsigned int i = 0; i != m_entities[Any::MODEL].size(); ++i) 
			mmm+="	if ( !visit" + m_entities[Any::MODEL][i].infoName() + "( model))\n";
	}
	mmm+="	{\n";
	mmm+="		// visiting other Model\n";
	mmm+="	}\n";
	mmm+="}\n\n\n";
	}
	

	// visitFolderImpl
	if ( !m_entities[Any::FOLDER].empty()) {
	mmm+="void " + m_className + "::visitFolderImpl( const BON::Folder& fold )\n";
	mmm+="{\n";
	{
		for( unsigned int i = 0; i != m_entities[Any::FOLDER].size(); ++i) 
			mmm+="	if ( !visit" + m_entities[Any::FOLDER][i].infoName() + "( fold))\n";
	}
	mmm+="	{\n";
	mmm+="		// visiting other Folder\n";
	mmm+="	}\n";
	mmm+="}\n\n\n";
	}


	return mmm;
}