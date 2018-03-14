<!-- =============================================================================== -->
<!--      This template rule will alter a kind's stereotype from Model to Atom       -->
<!-- =============================================================================== -->


<xsl:template match="model[@kind='##|par1|##']">

	<xsl:element name="atom">                                                 <!-- element name is hardcoded -->

		<xsl:for-each select="@*">
			<xsl:choose>
				<xsl:when test="name()='childrelidcntr'"/>        <!-- omit this -->
				<xsl:otherwise>
					<xsl:copy/>                               <!-- other attributes are just copied-->
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>
	
		<!-- we have to delete inner fcos in the model like: model|atom|reference|set|connection   -->
		<!-- so we apply further templates only for name, regnode, constraint, attribute           -->
		<xsl:apply-templates select="name"/>
		<xsl:apply-templates select="regnode"/>
		<xsl:apply-templates select="constraint"/>
		<xsl:apply-templates select="attribute"/>

	</xsl:element>

</xsl:template>
