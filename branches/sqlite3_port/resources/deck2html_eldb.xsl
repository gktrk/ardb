<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" encoding="iso-8859-1"/> 
<xsl:template match="/">

<xsl:comment>Stylesheet by Ville Virta - based on the original design by Francois Gombault</xsl:comment>
<html> <!-- xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" > -->
  <head>
    <style type="text/css">

      body { background: #FFFFFF; color: #000000; }
      table { font-family: Courier; font-size: 8pt; }
      
      .subtitle { background: #EEEEEE; font-weight: bold; }
      .generator { color: #888888; }

      a:link { color: #0000EE; text-decoration: underline; }
      a:hover { color: #000000; text-decoration: underline; }
      a:visited { color: #0000EE; text-decoration: underline; }

    </style>
    <title>V:TES deck - <xsl:value-of select="deck/name"/></title>
    <meta name="keywords" content="vtes,v:tes,jyhad,vampire,deck"/>
  </head>

  <body>
  <table width="650" align="center" cellspacing="0" cellpadding="0" border="0">
    <tr>
      <td width="130" valign="top">Deck Name:</td>
      <td width="520"><xsl:value-of select="deck/name"/></td>
    </tr>
    <tr>
      <td width="130" valign="top">Created by:</td>
      <td width="520"><xsl:value-of select="deck/author"/></td>
    </tr>
    <tr>
      <td width="130" valign="top">Description:</td>
      <td width="520">
        <xsl:call-template name="line-breaks">
	  <xsl:with-param name="text" select="string(deck/description)" />
	</xsl:call-template>
      </td>
    </tr>
    <tr><td height="20"></td></tr>
    </table>
    <table width="650" align="center" cellspacing="0" cellpadding="0" border="0">
    <tr><td colspan="6" class="subtitle">Crypt
      [<xsl:value-of select="deck/crypt/@size"/> vampires, average capacity: 
      <xsl:value-of select="deck/crypt/@avg"/>]</td></tr>

        <xsl:for-each select="/deck/crypt/vampire[not(name=preceding-sibling::vampire/name)]">
	  <xsl:sort select="capacity" data-type="number" order="descending"/>
          <xsl:sort select="name"/>
	    <xsl:variable name="xname" select="string(name)"/>
	    <xsl:for-each select="/deck/crypt/vampire[name=$xname and not(adv=preceding-sibling::vampire[name=$xname]/adv)]">
              <tr>
                <td nowrap="1" width="30"><xsl:call-template name="count-vampires"><xsl:with-param name="myname" select="string(name)" /><xsl:with-param name="myadv" select="string(adv)" /></xsl:call-template>x</td>
                <td nowrap="1"><a><xsl:attribute name='href'>http://www.secretlibrary.info/?crypt=<xsl:value-of select="name"/><xsl:if test="adv!=''"> (Adv)</xsl:if></xsl:attribute><xsl:value-of select="substring(name,1,23)"/>
		<xsl:if test="adv!=''"> (Adv)</xsl:if></a>
		</td>
                <td nowrap="1"><xsl:value-of select="disciplines"/></td>
                <td nowrap="1" width="26"><xsl:value-of select="capacity"/></td>
	        <td nowrap="1"><xsl:value-of select="clan"/>:<xsl:value-of select="group"/></td>
                <td nowrap="1"><xsl:value-of select="title"/></td>
	      </tr>
	    </xsl:for-each>
        </xsl:for-each>
	<tr><td height="20"></td></tr>
    </table>

    <table width="650" align="center" cellspacing="0" cellpadding="0" border="0">
    <tr><td colspan="2" class="subtitle">Library
    [<xsl:value-of select="deck/library/@size"/> cards]</td></tr>

    <xsl:call-template name="LIBPART">
      <xsl:with-param name="mytype" select="'Master'" />
    </xsl:call-template>
    <xsl:for-each select="/deck/library/card[not(type=preceding-sibling::card/type)]">
      <xsl:sort select="type"/>
      <xsl:if test="type!='Master' and type!='Combat'">
        <xsl:call-template name="LIBPART">
          <xsl:with-param name="mytype" select="string(type)" />
        </xsl:call-template>
      </xsl:if>
    </xsl:for-each>
    <xsl:call-template name="LIBPART">
      <xsl:with-param name="mytype" select="'Combat'" />
    </xsl:call-template>
    <tr><td height="20"></td></tr>

    <tr><td colspan="2" class="generator">Crafted with:
    <xsl:value-of select="//deck/@generator"/>
    [<xsl:value-of select="//deck/date"/>]</td></tr>
    </table>

  </body>
  </html>
</xsl:template>

<xsl:template name="LIBPART">

  <xsl:param name="mytype"/>
  <xsl:if test="//card[type = $mytype]">
    <tr><td colspan="2"><xsl:value-of select="$mytype"/> 
    [<xsl:value-of select="sum (//card[type = $mytype]/@count)"/>]</td></tr>
      <xsl:for-each select="//card[(type=$mytype) and not(name=preceding-sibling::card/name)]">
        <xsl:sort select="name"/>
          <tr>
            <td width="30"><xsl:call-template name="count-cards"><xsl:with-param name="myname" select="string(name)" /></xsl:call-template>x</td>
            <td width="620"><a><xsl:attribute name='href'>http://www.secretlibrary.info/?lib=<xsl:value-of select="name"/></xsl:attribute><xsl:value-of select="name"/></a></td>
          </tr>
      </xsl:for-each>
  </xsl:if>
  <tr><td height="12"></td></tr>
</xsl:template>

<xsl:template name="line-breaks">
  <xsl:param name="text"/>
  <xsl:choose>
    <xsl:when test="contains($text,'&#10;')">
      <xsl:value-of select="substring-before($text,'&#10;')"/>
      <br/>
      <xsl:call-template name="line-breaks">
        <xsl:with-param name="text" select="substring-after($text,'&#10;')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$text"/>
    </xsl:otherwise>
  </xsl:choose>
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

</xsl:stylesheet>
