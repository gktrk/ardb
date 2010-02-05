<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="iso-8859-1"/>
<xsl:template match="/"><xsl:text>CRYPT {jol#} {deckname} {password} {game email}&#013;&#010;</xsl:text>
<xsl:for-each select="/deck/crypt/vampire">
<xsl:sort select="name"/>
<xsl:call-template name="XVAMPIRES">
<xsl:with-param name="mycount" select="@count" />
<xsl:with-param name="myname" select="string(name)" />
<xsl:with-param name="myadvanced" select="string(adv)" />
<xsl:with-param name="mydisciplines" select="string(disciplines)" />
<xsl:with-param name="mycapacity" select="string(capacity)" />
<xsl:with-param name="myclan" select="string(clan)" />
</xsl:call-template>
</xsl:for-each><xsl:text>ENDREG&#013;&#010;&#013;&#010;LIBRARY {jol#} {deckname} {password} {game email}&#013;&#010;</xsl:text>
<xsl:for-each select="/deck/library/card">
<xsl:sort select="type"/>
<xsl:sort select="name"/>
<xsl:call-template name="XLIBRARYCARDS">
<xsl:with-param name="mycount" select="@count" />
<xsl:with-param name="myname" select="string(name)" />
</xsl:call-template>
</xsl:for-each>ENDREG<xsl:text>&#013;&#010;</xsl:text></xsl:template>

<xsl:template name="XVAMPIRES">
<xsl:param name="mycount"/>
<xsl:param name="myname"/>
<xsl:param name="myadvanced"/>
<xsl:param name="mydisciplines"/>
<xsl:param name="mycapacity"/>
<xsl:param name="myclan"/>
<xsl:if test="$mycount > 0">
<xsl:value-of select="$myname"/><xsl:if test="$myadvanced != ''"><xsl:text> [Advanced]</xsl:text></xsl:if><xsl:text> - </xsl:text><xsl:value-of select="$mydisciplines"/><xsl:text>, </xsl:text><xsl:value-of select="$mycapacity"/><xsl:text>, </xsl:text><xsl:value-of select="$myclan"/><xsl:text>&#013;&#010;</xsl:text>
<xsl:call-template name="XVAMPIRES">
<xsl:with-param name="mycount" select="$mycount - 1" />
<xsl:with-param name="myname" select="$myname" />
<xsl:with-param name="myadvanced" select="$myadvanced" />
<xsl:with-param name="mydisciplines" select="$mydisciplines" />
<xsl:with-param name="mycapacity" select="$mycapacity" />
<xsl:with-param name="myclan" select="$myclan" />
</xsl:call-template>
</xsl:if>
</xsl:template>

<xsl:template name="XLIBRARYCARDS">
<xsl:param name="mycount"/>
<xsl:param name="myname"/>
<xsl:if test="$mycount > 0">
<xsl:value-of select="$myname"/><xsl:text>&#013;&#010;</xsl:text>
<xsl:call-template name="XLIBRARYCARDS">
<xsl:with-param name="mycount" select="$mycount - 1" />
<xsl:with-param name="myname" select="$myname" />
</xsl:call-template>
</xsl:if>
</xsl:template>


</xsl:stylesheet>


