#include "StdAfx.h"
#include ".\ScriptMoveDown.h"

//<!-- =============================================================================== -->
//<!--        This template rule will wrap a kind into other container kinds aka       -->
//<!--        a kind moved down in the containment hierarchy                           -->
//<!--        This movement down inserts just a few objects (par3, par4, par5)         -->
//<!--        because every par1 found in par2 is wrapped into the same par3/par4/par5 -->
//<!-- =============================================================================== -->
//
//<!-- each ##|par2|##/##|par1|## kind is put into a ##|par3|##/##|par4|##/##|par5|##/##|par1|## -->
//<xsl:template match="*[@kind='##|par2|##']">
//	<xsl:copy>
//	<xsl:apply-templates select="@*"/>
//	<xsl:apply-templates select="*[ not(@kind) or @kind != '##|par1|##']"/>
//	<##|typ3|## kind="##|par3|##">
//		<name>New##|par3|##</name>
//		<##|typ4|## kind="##|par4|##">
//			<name>New##|par4|##</name>
//			<##|typ5|## kind="##|par5|##">
//				<name>New##|par5|##</name>
//				<xsl:apply-templates select="*[ @kind = '##|par1|##']"/>
//			</##|typ5|##>
//		</##|typ4|##>
//	</##|typ3|##>
//	</xsl:copy>
//</xsl:template>

const char * strGeneralHeaderDown = "\
<!-- =============================================================================== -->\r\n\
<!--                         Kind movement down in a hierarchy                       -->\r\n\
<!--              This template rule will wrap certain kinds into containers         -->\r\n\
<!--               Objects found in parent are wrapped into the same object          -->\r\n\
<!-- =============================================================================== -->\r\n\
\r\n\r\n";

//<!-- every ##|par1|## kind in ##|par2|## is put into a ##|par4|##/##|par3|##   -->

const char * strComment2Begin = "\r\n<!-- every ";
const char * strComment2Mid1  = " kind in ";
const char * strComment2Mid2  = " is put into a ";
const char * strComment2End   = " -->\r\n";

//<xsl:template match="*[@kind='##|par2|##']">
const char * strTemplateHeadBegin = "<xsl:template match=\"*[@kind='";
const char * strTemplateHeadEnd   = "']\">\r\n";
const char * strTemplateTail   = "</xsl:template>\r\n";

//	<xsl:copy>
//	<xsl:apply-templates select="@*"/>
//	<xsl:apply-templates select="*[ not(@kind) or @kind != '##|par1|##']"/>
const char * strBody1 = "\
\t<xsl:copy>\r\n\
\t<xsl:apply-templates select=\"@*\"/>\r\n\
\t<xsl:apply-templates select=\"*[ not(@kind) or @kind != '";

const char * strBody2 = "']\"/>\r\n";

const char * strBody3 = "\t</xsl:copy>\r\n";

//	<##|typ|## kind="##|par|##">
//		<name>New##|par|##</name>

const char * strSkeletonOpen = "\
\t<##|typ|## kind=\"##|par|##\">\r\n\
\t\t<name>New##|par|##</name>\
\r\n";

//  <xsl:apply-templates select="*[ @kind = '##|par1|##']"/>
const char * strInternals1 = "\
\t\t<xsl:apply-templates select=\"*[ @kind = '";

const char * strInternals2 = "\
']\"/>\r\n";

const char * strSkeletonClose = "\
\t</##|typ|##>\r\n";

ScriptMoveDown::ScriptMoveDown(void)
{
}

ScriptMoveDown::~ScriptMoveDown(void)
{
}

CString ScriptMoveDown::instantiat2( const std::vector< std::string >& pars, std::ostream& pstream)
{
	// pars[2..len] contains the hierarchy that will be created
	// pars[0] == pars.front() contains the element that will be wrapped (moved down)
	// pars[1] contains the parent of the element (matching based on this)

	ASSERT( pars.size() >= 3);
	if( pars.size() < 3) return "";

	std::string sequence;
	std::string skeleton_head, skeleton_tail;

	for( unsigned int i = 2; i < pars.size(); ++i)
	{
		if( sequence.length() != 0)
		{
			sequence += '/';
		}

		CString pi = pars[i].c_str();
		CString type = "folder";
		CString kind = pi;
		if( pi.Right(2) == ":M")
		{
			type = "model";
			kind = pi.Left( pi.GetLength() - 2);
		} else if( pi.Right(2) == ":F")
		{
			kind = pi.Left( pi.GetLength() - 2);
		}

		sequence += kind;

		CString open_tags( strSkeletonOpen);
		CString clos_tags( strSkeletonClose);

		open_tags.Replace( "##|typ|##", type);
		open_tags.Replace( "##|par|##", kind);

		clos_tags.Replace( "##|typ|##", type);

		skeleton_head += open_tags;
		skeleton_tail = (LPCTSTR) clos_tags + skeleton_tail;
	}
	
	// generic comment
	pstream << strGeneralHeaderDown;

	// specific comment
	pstream << strComment2Begin;
	pstream << pars[0]; // the node (element) moved
	pstream << strComment2Mid1;
	pstream << pars[1];
	pstream << strComment2Mid2;
	pstream << sequence; // the wrapper structure
	pstream << strComment2End;

	// template match = 
	pstream << strTemplateHeadBegin;
	pstream << pars[1]; // the parent of the moved node
	pstream << strTemplateHeadEnd;

	// body 
	pstream << strBody1;
	pstream << pars[0]; // the moved
	pstream << strBody2;

	// skeleton the moved to be wrapped into
	pstream << skeleton_head;

	pstream << strInternals1;
	pstream << pars[0]; // the moved element
	pstream << strInternals2;

	pstream << skeleton_tail;

	pstream << strBody3;
	pstream << strTemplateTail;

	return "";
}
