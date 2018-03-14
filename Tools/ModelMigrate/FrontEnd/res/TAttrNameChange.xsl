<!-- =============================================================================== -->
<!--                    This template rule will rename an attribute                  -->
<!-- =============================================================================== -->

<xsl:template match="attribute[@kind='##|par1|##'##|OPTIONALLOCALCONDITION|##]">
	<attribute kind="##|par2|##">
	<xsl:apply-templates/>
	</attribute>
</xsl:template>
