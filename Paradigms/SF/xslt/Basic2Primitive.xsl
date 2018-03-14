<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="xml" omit-xml-declaration="no" doctype-system="mga.dtd" indent="yes"/>

<xsl:template match="/">
		<xsl:apply-templates/>
</xsl:template>


<xsl:template match="project">
		<project>
		<xsl:attribute name="guid"><xsl:value-of select="attribute::guid"/></xsl:attribute>
		<xsl:attribute name="cdate"><xsl:value-of select="attribute::cdate"/></xsl:attribute>
		<xsl:attribute name="mdate"><xsl:value-of select="attribute::mdate"/></xsl:attribute>
		<xsl:attribute name="metaguid"><xsl:value-of select="attribute::metaguid"/></xsl:attribute>
		<xsl:attribute name="metaname"><xsl:value-of select="attribute::metaname"/></xsl:attribute>
		<xsl:apply-templates/>
		</project>
</xsl:template>


<xsl:template match="folder">
		<folder>
		<xsl:attribute name="id"><xsl:value-of select="attribute::id"/></xsl:attribute>
		<xsl:attribute name="relid"><xsl:value-of select="attribute::relid"/></xsl:attribute>
		<xsl:attribute name="childrelidcntr"><xsl:value-of select="attribute::childrelidcntr"/></xsl:attribute>
		<xsl:attribute name="kind"><xsl:value-of select="attribute::kind"/></xsl:attribute>
		<xsl:apply-templates/>
		</folder>
</xsl:template>


<!-- =============================================================================== -->
<!--              This template will alter models with Basic kind                    -->
<!-- =============================================================================== -->
<xsl:template match="model[attribute::kind='Basic']">
		<model>
		<xsl:attribute name="id"><xsl:value-of select="attribute::id"/></xsl:attribute>
		<xsl:attribute name="kind">Primitive</xsl:attribute>
		
		<xsl:if test="@role='BasicParts'">
				<xsl:attribute name="role">PrimitiveParts</xsl:attribute>
		</xsl:if>
		
		<xsl:attribute name="relid"><xsl:value-of select="attribute::relid"/></xsl:attribute>
		
		<xsl:if test="@childrelidcntr">
				<xsl:attribute name="childrelidcntr"><xsl:value-of select="attribute::childrelidcntr"/></xsl:attribute>
		</xsl:if>
		
		<xsl:apply-templates/>
		
		</model>
</xsl:template>


<xsl:template match="model">
				<model>
				<xsl:attribute name="id"><xsl:value-of select="attribute::id"/></xsl:attribute>
				<xsl:attribute name="kind"><xsl:value-of select="attribute::kind"/></xsl:attribute>
				<xsl:if test="@role">
						<xsl:attribute name="role"><xsl:value-of select="attribute::role"/></xsl:attribute>
				</xsl:if>
				<xsl:attribute name="relid"><xsl:value-of select="attribute::relid"/></xsl:attribute>
				<xsl:if test="@childrelidcntr">
						<xsl:attribute name="childrelidcntr"><xsl:value-of select="attribute::childrelidcntr"/></xsl:attribute>
				</xsl:if>
				<xsl:apply-templates/>
				</model>
</xsl:template>


<xsl:template match="attribute | value | regnode | comment | author | name | atom | connection | set ">
		<xsl:copy-of select="."/>
</xsl:template>


</xsl:stylesheet>