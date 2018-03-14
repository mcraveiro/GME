<!-- =============================================================================== -->
<!--       This template rule will rename an EnumAttribute's enumeration item        -->
<!-- =============================================================================== -->

<xsl:template match="attribute[@kind='##|par1|##'##|OPTIONALLOCALCONDITION|##]/value[./text() = '##|par2|##']">
	<value>##|par3|##</value>
</xsl:template>
