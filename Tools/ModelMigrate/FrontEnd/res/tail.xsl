<!-- tail begins -->

<xsl:template match="*|@*">
	<xsl:copy>
	<xsl:apply-templates select="@*"/>
	<xsl:apply-templates/>
	</xsl:copy>
</xsl:template>

</xsl:stylesheet>
