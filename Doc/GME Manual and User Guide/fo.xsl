<?xml version='1.0'?> 
<xsl:stylesheet  
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    version="1.0"> 

<xsl:import href="http://docbook.sourceforge.net/release/xsl/current/fo/docbook.xsl"/> 

<xsl:param name="fop1.extensions">1</xsl:param>

<!-- section titles are labeled with section number -->
<xsl:param name="section.autolabel" select="1"/>

<!-- toplevel sections start on a new page -->
<xsl:attribute-set name="section.level1.properties">
  <xsl:attribute name="break-before">page</xsl:attribute>
</xsl:attribute-set>

<!-- TODO: consider using pgwide.properties -->
<xsl:attribute-set name="monospace.verbatim.properties" 
                   use-attribute-sets="verbatim.properties monospace.properties">
  <xsl:attribute name="wrap-option">wrap</xsl:attribute>
  <xsl:attribute name="hyphenation-character">\</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="monospace.verbatim.properties">
  <xsl:attribute name="font-size">8pt</xsl:attribute>
</xsl:attribute-set>

<!-- render gui* elements in bold -->
<xsl:template match="guibutton">
  <xsl:call-template name="inline.boldseq"/>
</xsl:template>
<xsl:template match="guilabel">
  <xsl:call-template name="inline.boldseq"/>
</xsl:template>
<xsl:template match="guimenu">
  <xsl:call-template name="inline.boldseq"/>
</xsl:template>
<xsl:template match="guimenuitem">
  <xsl:call-template name="inline.boldseq"/>
</xsl:template>
<xsl:template match="guisubmenu">
  <xsl:call-template name="inline.boldseq"/>
</xsl:template>


</xsl:stylesheet>  
