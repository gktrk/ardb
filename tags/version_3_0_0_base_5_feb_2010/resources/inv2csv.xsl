<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:str="http://exslt.org/strings" extension-element-prefixes="str">

<xsl:output method="text" encoding="iso-8859-1"/>
<xsl:template match="/">
<xsl:text>"ELDB - Inventory"&#013;&#010;</xsl:text>
<xsl:for-each select="/inventory/crypt/vampire">
<xsl:sort select="name"/>
<xsl:text>"</xsl:text><xsl:call-template name="replace-doublequotes"><xsl:with-param name="str" select="string(name)" /></xsl:call-template><xsl:if test="adv != ''"><xsl:text> (ADV)</xsl:text></xsl:if><xsl:text>",</xsl:text>
<xsl:value-of select="@have"/><xsl:text>,</xsl:text>
<xsl:value-of select="@need"/><xsl:text>,"","Crypt"&#013;&#010;</xsl:text>
</xsl:for-each>
<xsl:for-each select="/inventory/library/card">
<xsl:sort select="name"/>
<xsl:text>"</xsl:text><xsl:call-template name="replace-doublequotes"><xsl:with-param name="str" select="string(name)" /></xsl:call-template><xsl:text>",</xsl:text>
<xsl:value-of select="@have"/><xsl:text>,</xsl:text>
<xsl:value-of select="@need"/><xsl:text>,"","Library"&#013;&#010;</xsl:text>
</xsl:for-each>
</xsl:template>


<xsl:variable name="nasty">"</xsl:variable>

 <xsl:template name="replace-doublequotes">
    <xsl:param name="str"/>   
    <xsl:if test="$str">
      <xsl:variable name="first-char" select="substring($str,1,1)"/>
      <xsl:choose>
        <xsl:when test="contains($first-char,$nasty)">
	  <xsl:text>'</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$first-char"/>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:if test="string-length($str) &gt; 1">
        <xsl:call-template name="replace-doublequotes">
          <xsl:with-param name="str" select="substring($str,2)"/>
        </xsl:call-template>
      </xsl:if>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>
