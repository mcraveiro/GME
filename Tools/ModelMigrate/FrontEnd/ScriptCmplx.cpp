#include "StdAfx.h"
#include ".\scriptcmplx.h"
#include <fstream>
#include <strstream>

/*static*/ const char * ScriptCmplx::output_log_file = "ValidatorLog.txt";

// this uses XSLT version 1.1
/*static*/ const char * ScriptCmplx::header = "\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.1\">\n\
\n\
<xsl:output method=\"xml\" omit-xml-declaration=\"no\" doctype-system=\"mga.dtd\" indent=\"yes\"/>\n\
\n\
<xsl:template match=\"/\">\n\
\t<xsl:apply-templates/>\n\
</xsl:template>\n\
\n\
\n";

/*static*/ const char * ScriptCmplx::kind_removed_log_cmd = "KINDREMOVAL: <xsl:value-of select = \"@kind\"/> [<xsl:value-of select = \"./name\"/>] in <xsl:value-of select=\"../@kind\"/> [<xsl:value-of select = \"../name\"/>]";
/*static*/ const char * ScriptCmplx::attr_removed_log_cmd = "ATTRREMOVAL: <xsl:value-of select = \"@kind\"/> attribute in <xsl:value-of select=\"../@kind\"/> [<xsl:value-of select = \"../name\"/>]";

// ---------------------------------------------------------
//                         body 
// ---------------------------------------------------------
//<xsl:template match="project">
//		<xsl:variable name="kinds_to_remove" select="//*[name() != 'attribute' and
//@kind != 'M1' and
//@kind != 'M2' and
//@kind != 'RootFolder'
//]"/>
//
//		<!-- separate variable for each attribute owner kind -- >
//		<xsl:variable name="attrs_to_remove1" select="//attribute[ 
//../@kind = 'M1' and 
//@kind != 'L1' and
//@kind != 'L2' and
//@kind != 'G1' and
//@kind != 'G2'
//]"/>
//
//
//		<!-- separate variable for each attribute owner kind -- >
//		<xsl:variable name="attrs_to_remove2" select="//attribute[ 
//../@kind = 'M2' and 
//@kind != 'L5' and
//@kind != 'L6' and
//@kind != 'G7' and
//@kind != 'G8'
//]"/>
//
//		<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
//		<!-- producing the summary text file -->
//		<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
//		<xsl:document href="removals.txt" method="text">Summary of kind and attribute removals
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//<xsl:for-each select="$kinds_to_remove">KINDREMOVAL: <xsl:value-of select = "@kind"/> [<xsl:value-of select = "./name"/>] in <xsl:value-of select="../@kind"/> [<xsl:value-of select = "../name"/>]
//</xsl:for-each>
//
//<xsl:for-each select="$attrs_to_remove">ATTRREMOVAL: <xsl:value-of select = "@kind"/> attribute in <xsl:value-of select="../@kind"/> [<xsl:value-of select = "../name"/>]
//</xsl:for-each>
//
//		</xsl:document>
//		
//		
//		<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
//		<!-- producing the summary at the beginning of the project -->
//		<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->
//<xsl:comment>
//
//Summary of kind and attribute removals 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ <xsl:for-each select="$kinds_to_remove">
//KINDREMOVAL: <xsl:value-of select = "@kind"/> [<xsl:value-of select = "./name"/>] in <xsl:value-of select="../@kind"/> [<xsl:value-of select = "../name"/>]</xsl:for-each>
//
//		<xsl:for-each select="$attrs_to_remove">
//ATTRREMOVAL: <xsl:value-of select = "@kind"/> attribute in <xsl:value-of select="../@kind"/> [<xsl:value-of select = "../name"/>]</xsl:for-each>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
//End of Summary 
//</xsl:comment>
//
//		<xsl:copy>
//		<xsl:apply-templates select="@*"/>
//		<xsl:apply-templates/>
//		</xsl:copy>
//</xsl:template>
//
//<xsl:template match="*[
//name() != 'attribute' and
//@kind != 'M1' and
//@kind != 'M2' and
//@kind != 'RootFolder'
//]">
//<xsl:comment>KINDREMOVAL: <xsl:value-of select = "@kind"/> [<xsl:value-of select = "./name"/>] in <xsl:value-of select="../@kind"/> [<xsl:value-of select = "../name"/>]</xsl:comment>
//</xsl:template>
//
//<!-- M1 has L1, L2 (local) and G1, G2 global attributes, but they aren't distinguished -->
//<xsl:template match="attribute[ 
//../@kind = 'M1' and 
//@kind != 'L1' and
//@kind != 'L2' and
//@kind != 'G1' and
//@kind != 'G2'
//]">
//<xsl:comment>ATTRREMOVAL: <xsl:value-of select = "@kind"/> attribute in <xsl:value-of select="../@kind"/> [<xsl:value-of select = "../name"/>]</xsl:comment>
//</xsl:template>

// ---------------------------------------------------------
//                         end 
// ---------------------------------------------------------


/*static*/ const char * ScriptCmplx::tail = "\
<xsl:template match=\"*|@*\">\n\
\t<xsl:copy>\n\
\t<xsl:apply-templates select=\"@*\"/>\n\
\t<xsl:apply-templates/>\n\
\t</xsl:copy>\n\
</xsl:template>\n\
\n\
</xsl:stylesheet>";


ScriptCmplx::ScriptCmplx(void)
	: ScriptAbs()
{
}

ScriptCmplx::~ScriptCmplx(void)
{
}

void ScriptCmplx::gen( const Extractor::NAMES& pNames, const Extractor::ATTRS& pAttrs, const char *pfName)
{
	std::ofstream f( pfName, std::ios_base::out);
	std::ostrstream pred_for_kinds;

	
	//[ name() != 'attribute'
	// and @kind != 'M1'
	// and @kind != 'M2'
	// and @kind != 'A1'
	// and @kind != 'RootFolder'
	//]

	// let's produce the predicate above (for kinds):
	genPredKinds( pred_for_kinds, pNames);

	f << header;

	// ################################################
	// bulding an xsl:template for project manipulation
	// ################################################

	// the 'project' matching rule will output the summary
	f << "<xsl:template match=\"project\">" << std::endl;

	// one variable for storing the kinds that are removed
	f << "\t<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->" << std::endl;
	f << "\t<!--     one variable storing each unknown element node    -->" << std::endl;
	f << "\t<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->" << std::endl;
	f << "\t<xsl:variable name=\"kinds_to_remove\" select=\"//*";
	f << pred_for_kinds.str();
	f << "\"/>" << std::endl << std::endl;

	f << "\t<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->" << std::endl;
	f << "\t<!--    separate variables for each attribute owner kind   -->" << std::endl;
	f << "\t<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->" << std::endl;
	unsigned int i = 0;
	for( Extractor::ATTRS::const_iterator it = pAttrs.begin(); it != pAttrs.end(); ++it, ++i)
	{
		std::ostrstream predi;
		genPredAttrs( predi, it);
		f << "\t<xsl:variable name=\"attrs_to_remove" << i << "\" select=\"//attribute";
		f << predi.str() << "\"/>" << std::endl << std::endl;
	}

	f << "\t<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->" << std::endl;
	f << "\t<!-- producing the summary at the beginning of the project -->" << std::endl;
	f << "\t<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->" << std::endl;
	f << "<xsl:comment>" << std::endl;
	f << std::endl;
	f << "Summary of kind and attribute removals" << std::endl;
	f << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ <xsl:for-each select=\"$kinds_to_remove\">" << std::endl;
	f << kind_removed_log_cmd << "</xsl:for-each>" << std::endl;
	f << std::endl;

	if( !pAttrs.empty())
	{
		f << "<xsl:for-each select=\"";
		unsigned int i = 0;
		for( Extractor::ATTRS::const_iterator it = pAttrs.begin(); it != pAttrs.end(); ++it, ++i)
		{
			if( i != 0) f << "|";
			f << "$attrs_to_remove" << i;
		}
		f << "\">" << std::endl; // end the for-each opening tag
		f << attr_removed_log_cmd << "</xsl:for-each>" << std::endl;
	}
	f << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	f << "End of Summary" << std::endl;
	f << "</xsl:comment>" << std::endl;

	f << std::endl;
	f << "\t<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->" << std::endl;
	f << "\t<!--              producing the summary text file          -->" << std::endl;
	f << "\t<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->" << std::endl;
	f << "\t<xsl:document href=\"" << output_log_file << "\" method=\"text\">Summary of kind and attribute removals" << std::endl;
	f << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	f << "<xsl:for-each select=\"$kinds_to_remove\">" << kind_removed_log_cmd << std::endl;
	f << "</xsl:for-each>" << std::endl;
	f << std::endl;
	if( !pAttrs.empty())
	{
		f << "<xsl:for-each select=\"";
		unsigned int i = 0;
		for( Extractor::ATTRS::const_iterator it = pAttrs.begin(); it != pAttrs.end(); ++it, ++i)
		{
			if( i != 0) f << "|";
			f << "$attrs_to_remove" << i;
		}
		f << "\">"; // end the for-each opening tag
		f << attr_removed_log_cmd << std::endl;
		f << "</xsl:for-each>";
	}
	f << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	f << "End of Summary" << std::endl;
	f << "\t</xsl:document>" << std::endl;
	f << std::endl;
	f << "\t<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->" << std::endl;
	f << "\t<!--        launch inner processing of the project         -->" << std::endl;
	f << "\t<!-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ -->" << std::endl;
	f << "\t<xsl:copy>" << std::endl;
	f << "\t<xsl:apply-templates select=\"@*\"/>" << std::endl;
	f << "\t<xsl:apply-templates/>" << std::endl;
	f << "\t</xsl:copy>" << std::endl;
	f << "</xsl:template>" << std::endl;
	f << std::endl;
	// ################################################
	// end of bulding project manipulation template
	// ################################################

	// ################################################
	// bulding an xsl:template to ignore unknown kinds
	// ################################################

	f << "<xsl:template match=\"*" << pred_for_kinds.str() << "\">" << std::endl;
	f << "<xsl:comment>" << kind_removed_log_cmd << "</xsl:comment>" << std::endl;
	f << "</xsl:template>" << std::endl;
	f << std::endl;

	// ################################################
	// bulding several xsl:templates to ignore unknown attributes
	// ################################################
	for( Extractor::ATTRS::const_iterator it = pAttrs.begin(); it != pAttrs.end(); ++it)
	{
		std::ostrstream predi;
		genPredAttrs( predi, it);

		f << "<xsl:template match=\"attribute" << predi.str() << "\">" << std::endl;
		f << "<xsl:comment>" << attr_removed_log_cmd << "</xsl:comment>" << std::endl;
		f << "</xsl:template>" << std::endl;
		f << std::endl;
	}

	f << tail;

	f.close();
}

void ScriptCmplx::genPredAttrs( std::ostream& ostr, const Extractor::ATTRS::const_iterator& iter)
{
	Extractor::NAMES res = Extractor::tokenize( iter->second);
	if( !res.empty())
	{
		ostr << "[ ../@kind = '" << iter->first << "'" << std::endl;

		for( Extractor::NAMES::iterator jt = res.begin(); jt != res.end(); ++jt)
			ostr << " and @kind != '" << *jt << "'" << std::endl;

		ostr << "]";
	}
	ostr << std::ends;
}

void ScriptCmplx::genPredKinds( std::ostream& ostr, const Extractor::NAMES& pNames)
{
	ostr << "[ name() != 'attribute'" << std::endl;
	for( Extractor::NAMES::const_iterator it = pNames.begin(); it != pNames.end(); ++it)
	{
		ostr << " and @kind != '" << *it << "'" << std::endl;
	}

	ostr << "]";
	ostr << std::ends;
}