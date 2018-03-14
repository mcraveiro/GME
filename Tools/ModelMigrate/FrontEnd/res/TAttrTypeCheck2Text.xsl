<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="text"/>

<xsl:template match="/">Attribute value conversion test started.
<xsl:apply-templates/>
Attribute value conversion test ended.
</xsl:template>

<!-- =============================================================================== -->
<!--        This template rule produces a short listing of the non-convertible       -->
<!--           attribute values from the whole input file into a text file           -->
<!-- =============================================================================== -->

<xsl:template match="attribute[@kind='##|par1|##'##|OPTIONALLOCALCONDITION|##]/value">
	<xsl:if test="string(number(text())) = 'NaN'">[CONVERROR] Error during conversion to number: '<xsl:value-of select="."/>' in '<xsl:value-of select="../@kind"/>' attribute of <xsl:value-of select="name(../../)"/> '<xsl:value-of select="../../name"/>' with '<xsl:value-of select="../../@id"/>' id.
<xsl:message terminate="no">[CONVERROR]s found. Check the target file for details.</xsl:message>
</xsl:if>
</xsl:template>

<!--prevent other stuff getting into the output -->
<xsl:template match="text()">
</xsl:template>

<xsl:template match="*|@*">
	<xsl:copy>
	<xsl:apply-templates select="@*"/>
	<xsl:apply-templates/>
	</xsl:copy>
</xsl:template>

</xsl:stylesheet>