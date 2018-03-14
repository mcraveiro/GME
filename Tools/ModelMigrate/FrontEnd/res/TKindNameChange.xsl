<!-- =============================================================================== -->
<!--                  This template rule will alter a kind's name                    -->
<!-- =============================================================================== -->


<xsl:template match="*[@kind='##|par1|##']">

	<xsl:element name="{name()}">                          <!-- element name is not hardcoded -->

		<xsl:for-each select="@*">
			<xsl:choose>
				<xsl:when test="name()='kind'">        <!-- treat its kind attribute specially -->
					<xsl:attribute name="kind">##|par2|##</xsl:attribute>
				</xsl:when>
				<xsl:otherwise>
					<xsl:copy/>                    <!-- other attributes are just copied-->
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
	
		<xsl:apply-templates/>

	</xsl:element>

</xsl:template>
