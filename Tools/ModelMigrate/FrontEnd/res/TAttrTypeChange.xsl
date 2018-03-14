<!-- ================================================================================== -->
<!--  This template checks whether an attribute value can be interpreted as a number    -->
<!--  When the 2nd if test has the form "1=1", terminates the conversion process with   -->
<!--  an error message containing only the first occurence of a conversion failure      -->
<!--  When it has the form "1=0", it continues processing, placing a [CONVERROR] notice -->
<!--  into the target file, where the conversion check failed (a non-number was met)    -->
<!-- ================================================================================== -->

<xsl:template match="attribute[@kind='##|par1|##'##|OPTIONALLOCALCONDITION|##]/value">
	<xsl:if test="string(number(text())) = 'NaN'">
		<xsl:if test="1=##|par2|##">
			<xsl:message terminate="yes">Error during conversion to number: '<xsl:value-of select="."/>' in '<xsl:value-of select="../@kind"/>' attribute of <xsl:value-of select="name(../../)"/> '<xsl:value-of select="../../name"/>' with '<xsl:value-of select="../../@id"/>' id.</xsl:message>
		</xsl:if>
		<xsl:comment>[CONVERROR] Error during conversion to number: '<xsl:value-of select="."/>' in '<xsl:value-of select="../@kind"/>' attribute of <xsl:value-of select="name(../../)"/> '<xsl:value-of select="../../name"/>' with '<xsl:value-of select="../../@id"/>' id.
</xsl:comment>
	</xsl:if>

	<xsl:copy>
	<xsl:apply-templates select = "@*"/>
	<xsl:apply-templates/>
	</xsl:copy>

</xsl:template>
