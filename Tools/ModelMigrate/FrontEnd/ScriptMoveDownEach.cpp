#include "StdAfx.h"
#include ".\ScriptMoveDown.h"

//<xsl:template match="*[@kind='##|par1|##']">
//
//	<!-- each ##|par1|## kind is put into a ##|par3|##/##|par2|##   -->
//
//	<##|typ1|## kind="##|par1|##">
//		<name>New##|par1|##</name>
//		<##|typ2|## kind="##|par2|##" role="##|par2|##">
//			<name>New##|par2|##</name>
//
//			<xsl:element name="{name()}">          <!-- element name is not hardcoded -->
//		
//			<xsl:for-each select="@*">
//				<xsl:copy/>                    <!-- other attributes are just copied-->
//			</xsl:for-each>
//
//			<xsl:apply-templates/>
//		
//			</xsl:element>
//		</##|typ2|##>
//	</##|typ1|##>
//
//</xsl:template>
const char * strGeneralHeaderDown = "\
<!-- =============================================================================== -->\r\n\
<!--                         Kind movement down in a hierarchy                       -->\r\n\
<!--              This template rule will wrap certain kinds into containers         -->\r\n\
<!-- =============================================================================== -->\r\n\
\r\n\r\n";

const char * strTemplateHeadBegin = "<xsl:template match=\"*[@kind='";
const char * strTemplateHeadEnd   = "']\">\r\n";
const char * strTemplateTail   = "</xsl:template>\r\n";

//<!-- each ##|par1|## kind is put into a ##|par3|##/##|par2|##   -->

const char * strComment2Begin = "\r\n<!-- each ";
const char * strComment2Mid   = " kind is put into a ";
const char * strComment2End   = " -->\r\n";


//	<##|typ|## kind="##|par|##">
//		<name>New##|par|##</name>

const char * strSkeletonOpen = "\
\t<##|typ|## kind=\"##|par|##\">\r\n\
\t\t<name>New##|par|##</name>\
\r\n";

const char * strInternals = "\
\t\t<xsl:copy>\r\n\
\t\t<xsl:apply-templates select=\"@*\"/>\r\n\
\t\t<xsl:apply-templates/>\r\n\
\t\t</xsl:copy>\r\n";

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
	// pars[1..len] contains the hierarchy that will be created
	// pars.front() contains the element that will be wrapped (moved down)

	ASSERT( pars.size() >= 2);
	if( pars.size() < 2) return "";

	std::string sequence;
	std::string skeleton_head, skeleton_tail;

	for( unsigned int i = 1; i < pars.size(); ++i)
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
	
	pstream << strGeneralHeaderDown;

	pstream << strComment2Begin;
	pstream << pars[0]; // the node (element) moved
	pstream << strComment2Mid;
	pstream << sequence; // the wrapper structure
	pstream << strComment2End;

	pstream << strTemplateHeadBegin;
	pstream << pars[0]; // the node (element) moved
	pstream << strTemplateHeadEnd;

	pstream << skeleton_head;
	pstream << strInternals;
	pstream << skeleton_tail;

	pstream << strTemplateTail;

	return "";
}
