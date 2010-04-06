<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="iso-8859-1"/>
<xsl:template match="/">
<xsl:variable name="maxname" select="'                  '"/>
<xsl:variable name="name"><xsl:for-each select="//vampire/name"><xsl:sort data-type="number" select="string-length()" order="descending"/><xsl:if test="position()=1"><xsl:value-of select="substring($maxname, 0, string-length(.)+1)"/></xsl:if></xsl:for-each></xsl:variable>
<xsl:variable name="capacity" select="'   '"/>
<xsl:variable name="maxdisciplines" select="'                             '"/>
<xsl:variable name="disciplines"><xsl:for-each select="//disciplines"><xsl:sort data-type="number" select="string-length()" order="descending"/><xsl:if test="position()=1"><xsl:value-of select="substring($maxdisciplines, 0, string-length(.))"/></xsl:if></xsl:for-each></xsl:variable>
<xsl:variable name="maxtitle" select="'             '"/>
<xsl:variable name="title"><xsl:for-each select="//title"><xsl:sort data-type="number" select="string-length()" order="descending"/><xsl:if test="position()=1"><xsl:value-of select="substring($maxtitle, 0, string-length(.)+1)"/></xsl:if></xsl:for-each></xsl:variable>
<xsl:variable name="clan" select="'          '"/>



<xsl:for-each select="/deck/library/card[not(type=preceding-sibling::card/type)]">
<xsl:sort select="type"/>
<xsl:call-template name="LIBPART">
<xsl:with-param name="mytype" select="string(type)" />
</xsl:call-template>
</xsl:for-each>


<xsl:text>Crypt:&#013;</xsl:text>
<xsl:for-each select="/deck/crypt/vampire[not(name=preceding-sibling::vampire/name)]">
<xsl:sort select="@count" data-type="number" order="descending"/>
<xsl:sort select="capacity" data-type="number" order="descending"/>
<xsl:sort select="name"/>
<xsl:variable name="xname" select="string(name)"/>
<xsl:for-each select="/deck/crypt/vampire[name=$xname and not(adv=preceding-sibling::vampire[name=$xname]/adv)]">
<xsl:call-template name="count-vampires"><xsl:with-param name="myname" select="string(name)" /><xsl:with-param name="myadv" select="string(adv)" /></xsl:call-template><xsl:text>&#x9;</xsl:text><xsl:value-of select="substring(name,0,string-length($name)+1)"/><xsl:text>&#013;&#010;</xsl:text>
</xsl:for-each>
</xsl:for-each>


</xsl:template>

<xsl:template name="LIBPART">
<xsl:param name="mytype"/>
<xsl:if test="//card[type = $mytype]">

<xsl:for-each select="//card[(type=$mytype) and not(name=preceding-sibling::card/name)]">
<xsl:sort select="name"/>
<xsl:call-template name="count-cards"><xsl:with-param name="myname" select="string(name)" /></xsl:call-template><xsl:text>&#x9; </xsl:text><xsl:value-of select="name"/><xsl:text>&#013;&#010;</xsl:text>
</xsl:for-each>
</xsl:if>
</xsl:template>

<xsl:template name="count-vampires">
<xsl:param name="myname"/>
<xsl:param name="myadv"/>
<xsl:value-of select="sum(//@count[../name=$myname and ../adv=$myadv])"/>
</xsl:template>

<xsl:template name="count-cards">
<xsl:param name="myname"/>
<xsl:value-of select="sum(//@count[../name=$myname])"/>
</xsl:template>

<xsl:template name="longest-string">
<xsl:param name="nodes"/>
<xsl:for-each select="$nodes">
	

	
</xsl:for-each>
</xsl:template>

</xsl:stylesheet>
