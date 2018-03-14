<!-- =============================================================================== -->
<!--           This template rule will alter a project's paradigm name               -->
<!-- =============================================================================== -->


<xsl:template match="project[@metaname='##|par1|##']">

	<xsl:element name="{name()}">                          <!-- element name is not hardcoded -->

		<xsl:for-each select="@*">
			<xsl:choose>
				<xsl:when test="name()='metaname'">        <!-- treat its metaname attribute specially -->
					<xsl:attribute name="metaname">##|par2|##</xsl:attribute>
				</xsl:when>
				<xsl:otherwise>
					<xsl:copy/>                    <!-- other attributes are just copied-->
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
	
		<xsl:apply-templates/>

	</xsl:element>

</xsl:template>
