#include "StdAfx.h"
#include ".\ScriptMoveUp.h"

//<!-- =============================================================================== -->
//<!--                         Kind movement in a hierarchy                            -->
//<!--        This template rule will unwrap a kind from enclosing containers          -->
//<!-- =============================================================================== -->
//
//<!-- movement -->
//<xsl:template match="*[@kind='##|par3|##']">
//
//	<xsl:copy>
//	<xsl:apply-templates select="@*"/>
//	<xsl:apply-templates/>
//	</xsl:copy>
//
//	<!-- any kind of ##|par3|##/##|par2|##/##|par1|## becomes a sybling of ##|par3|## -->
//	<xsl:copy-of select="*[./@kind='##|par2|##']/*[./@kind='##|par1|##']">
//	</xsl:copy-of>
//</xsl:template>
//
//<!-- ignoring it in the old place -->
//<xsl:template match="*[./@kind='##|par1|##' and ../@kind='##|par2|##' and ../../@kind='##|par3|##']">
//	<xsl:comment> Item moved from here: <xsl:value-of select="@id"/></xsl:comment>
//</xsl:template>

const char * strGeneralHeaderUp = "\
<!-- =============================================================================== -->\r\n\
<!--                         Kind movement up in a hierarchy                         -->\r\n\
<!--        This template rule will unwrap a kind from enclosing containers          -->\r\n\
<!-- =============================================================================== -->\r\n\
\r\n\r\n";

//                <!-- movement -->
const char * strMovementHead1 = "\
<!-- movement -->\r\n\
<xsl:template match=\"*[@kind='";		// ##|par3|## will be inserted here

const char * strMovementHead3 = "\
']\">\r\n";

const char * strMovementBody1 = "\
\r\n\
\t<xsl:copy>\r\n\
\t<xsl:apply-templates select=\"@*\"/>\r\n\
\t<xsl:apply-templates/>\r\n\
\t</xsl:copy>\r\n\
\r\n";

const char * strMovementBodyComment1 = "\
\t<!-- each "; // sequence like: ##|par3|##/##|par2|##/##|par1|## 

const char * strMovementBodyComment3 = "\
 becomes a sybling of ";	// element : ##|par3|##

const char * strMovementBodyComment5 = "-->\r\n";

const char * strMovementBody2 = "\
\t<xsl:copy-of select=\"";
// sequence like: *[./@kind='##|par2|##']/*[./@kind='##|par1|##']
const char * strMovementBody4 = "\"/>\r\n\
\r\n";

const char * strMovementTail = "</xsl:template>\r\n\r\n";


//                <!-- ignoring -->
const char * strDisregardBegin1 = "<!-- ignoring "; // elem: ##|par1|##
const char * strDisregardBegin3 = " in the old place -->\r\n\
<xsl:template match=\"*["; // ./@kind='##|par1|##'  and ../@kind='##|par2|##' and ../../@kind='##|par3|##'

const char * strDisregardEnd = "]\">\r\n\
\t<xsl:comment> Item moved from here: <xsl:value-of select=\"@id\"/></xsl:comment>\r\n\
</xsl:template>\r\n\
\r\n";

ScriptMoveUp::ScriptMoveUp(void)
{
}

ScriptMoveUp::~ScriptMoveUp(void)
{
}

CString ScriptMoveUp::instantiat2( const std::vector< std::string >& pars, std::ostream& pstream)
{
	// pars contains the hierarchy that will be eliminated
	// pars.back() contains the element that will survive (moved up)

	ASSERT( pars.size() >= 2);

	std::string elemContn = pars.front();
	std::string elemMoved = pars.back();

	std::string seqA; // ##|par3|##/##|par2|##/##|par1|##
	std::string seqB; // *[./@kind='##|par2|##']/*[./@kind='##|par1|##']
	std::string seqC; // ./@kind='##|par1|##'  and ../@kind='##|par2|##' and ../../@kind='##|par3|##']">

	std::string path_pref = "./";

	for( unsigned int i = 0; i < pars.size(); ++i)
	{
		if( !seqA.empty()) seqA += '/';
		if( !seqB.empty()) seqB += '/';
		if( !seqC.empty()) seqC += " and ";

		seqA += pars[i];

		if( i != 0) // for seqB the first element is skipped
		{
			seqB += "*[./@kind='";
			seqB += pars[i];
			seqB += "']";
		}

		seqC += path_pref;
		seqC += "@kind='";
		seqC += pars[ pars.size() - i - 1];
		seqC += "'";

		if( i == 0) path_pref = "../";
		else path_pref += "../";
	}
	
	pstream << strGeneralHeaderUp;
	pstream << strMovementHead1;
	pstream << elemContn;
	pstream << strMovementHead3;

	pstream << strMovementBody1;
	pstream << strMovementBodyComment1;
	pstream << seqA;
	pstream << strMovementBodyComment3;
	pstream << elemContn;
	pstream << strMovementBodyComment5;

	pstream << strMovementBody2;
	pstream << seqB;
	pstream << strMovementBody4;

	pstream << strMovementTail;

	pstream << strDisregardBegin1;
	pstream << elemMoved;
	pstream << strDisregardBegin3;
	pstream << seqC;
	pstream << strDisregardEnd;

	return "";
}
