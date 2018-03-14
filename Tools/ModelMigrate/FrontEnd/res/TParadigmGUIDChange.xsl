<!-- =============================================================================== -->
<!--          This template rule will alter a project's paradigm GUID                -->
<!-- =============================================================================== -->


<xsl:template match="project[@metaguid='##|par1|##']">

	<xsl:element name="{name()}">                          <!-- element name is not hardcoded -->

		<xsl:for-each select="@*">
			<xsl:choose>
				<xsl:when test="name()='metaguid'">        <!-- treat its metaguid attribute specially -->
					<xsl:attribute name="metaguid">##|par2|##</xsl:attribute>
				</xsl:when>
				<xsl:otherwise>
					<xsl:copy/>                    <!-- other attributes are just copied-->
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
	
		<xsl:apply-templates/>

	</xsl:element>

</xsl:template>
