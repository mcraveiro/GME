<!-- =============================================================================== -->
<!--      This template rule will alter a kind's stereotype from Atom to Model       -->
<!-- =============================================================================== -->


<xsl:template match="atom[@kind='##|par1|##']">

	<xsl:element name="model">                     <!-- element name is hardcoded -->

		<xsl:for-each select="@*">
			<xsl:copy/>                    <!-- attributes are just copied-->
		</xsl:for-each>
	
		<xsl:apply-templates/>

	</xsl:element>

</xsl:template>
