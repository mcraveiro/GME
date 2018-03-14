<!-- =============================================================================== -->
<!--         This template rule will remove an attribute owned by a certain kind     -->
<!-- =============================================================================== -->

<xsl:template match="attribute[@kind='##|par1|##' and ../@kind='##|par2|##']"/>
