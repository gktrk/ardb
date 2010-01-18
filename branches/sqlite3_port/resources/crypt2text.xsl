<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="iso-8859-1"/>
<xsl:template match="/">
<xsl:for-each select="/deck/crypt/vampire">
<xsl:sort select="name"/>
<xsl:call-template name="XVAMPIRES">
<xsl:with-param name="mycount" select="@count" />
<xsl:with-param name="myname" select="string(name)" />
<xsl:with-param name="myadvanced" select="string(adv)" />
</xsl:call-template>
</xsl:for-each>
</xsl:template>

<xsl:template name="XVAMPIRES">
<xsl:param name="mycount"/>
<xsl:param name="myname"/>
<xsl:param name="myadvanced"/>
<xsl:if test="$mycount > 0">
<xsl:value-of select="$myname"/>
<xsl:if test="$myadvanced != ''">
<xsl:text> (Adv)</xsl:text>
</xsl:if>
<xsl:text>&#013;&#010;</xsl:text>
<xsl:call-template name="XVAMPIRES">
<xsl:with-param name="mycount" select="$mycount - 1" />
<xsl:with-param name="myname" select="$myname" />
<xsl:with-param name="myadvanced" select="$myadvanced" />
</xsl:call-template>
</xsl:if>
</xsl:template>

</xsl:stylesheet>


