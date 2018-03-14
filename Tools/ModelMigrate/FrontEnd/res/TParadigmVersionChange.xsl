<!-- =============================================================================== -->
<!--         This template rule will alter a project's paradigm version              -->
<!-- =============================================================================== -->


<xsl:template match="project[@metaversion='##|par1|##']">

	<xsl:element name="{name()}">                          <!-- element name is not hardcoded -->

		<xsl:for-each select="@*">
			<xsl:choose>
				<xsl:when test="name()='metaversion'">        <!-- treat its metaversion attribute specially -->
					<xsl:attribute name="metaversion">##|par2|##</xsl:attribute>
				</xsl:when>
				<xsl:otherwise>
					<xsl:copy/>                    <!-- other attributes are just copied-->
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
	
		<xsl:apply-templates/>

	</xsl:element>

</xsl:template>
