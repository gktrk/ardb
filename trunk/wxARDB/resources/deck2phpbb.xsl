<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:str="http://exslt.org/strings" extension-element-prefixes="str">

<xsl:output method="text" encoding="iso-8859-1"/>

  <!-- Characters we'll support.
       We could add control chars 0-31 and 127-159, but we won't. -->
  <xsl:variable name="ascii"> !"#$%&amp;'()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~</xsl:variable>
  <xsl:variable name="latin1"> ¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ</xsl:variable>

  <!-- Characters that usually don't need to be escaped -->
  <xsl:variable name="safe">!*-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz~</xsl:variable>

  <xsl:variable name="hex">0123456789ABCDEF</xsl:variable>



<xsl:template match="/">
<xsl:text>[size=18][b]Deck Name : </xsl:text><xsl:value-of select="deck/name"/><xsl:text>[/b][/size]&#013;&#010;[b][u]Author :[/u][/b] </xsl:text><xsl:value-of select="deck/author"/><xsl:text>&#013;&#010;[b][u]Description :[/u][/b]&#013;&#010;</xsl:text>
<xsl:value-of select="deck/description"/><xsl:text>&#013;&#010;&#013;&#010;&#013;&#010;[size=18][u]Crypt [</xsl:text><xsl:value-of select="deck/crypt/@size"/><xsl:text> vampires] Capacity min: </xsl:text><xsl:value-of select="deck/crypt/@min"/><xsl:text> max: </xsl:text><xsl:value-of select="deck/crypt/@max"/><xsl:text> average: </xsl:text><xsl:value-of select="deck/crypt/@avg"/><xsl:text>[/u][/size]&#013;&#010;</xsl:text>

<xsl:for-each select="/deck/crypt/vampire[not(name=preceding-sibling::vampire/name)]">
<xsl:sort select="@count" data-type="number" order="descending"/>
<xsl:sort select="capacity" data-type="number" order="descending"/>
<xsl:sort select="name"/>
<xsl:variable name="xname" select="string(name)"/>
<xsl:for-each select="/deck/crypt/vampire[name=$xname and not(adv=preceding-sibling::vampire[name=$xname]/adv)]">
<xsl:call-template name="count-vampires"><xsl:with-param name="myname" select="string(name)" /><xsl:with-param name="myadv" select="string(adv)" /></xsl:call-template><xsl:text>x [url=http://www.secretlibrary.info/?crypt==</xsl:text><xsl:call-template name="url-encode"><xsl:with-param name="str" select="string(name)"/></xsl:call-template><xsl:if test="adv!=''">%20(ADV)</xsl:if><xsl:text>] </xsl:text><xsl:value-of select="string(name)"/><xsl:choose><xsl:when test="adv != ''"><xsl:text> Adv </xsl:text></xsl:when><xsl:otherwise><xsl:text>     </xsl:text></xsl:otherwise></xsl:choose><xsl:text> [/url] (</xsl:text><xsl:value-of select="capacity"/><xsl:text>) </xsl:text><xsl:call-template name="str:tokenize"><xsl:with-param name="string" select="string(disciplines)" /><xsl:with-param name="delimiters" select="' '"/></xsl:call-template><xsl:text> </xsl:text><xsl:value-of select="string(title)"/><xsl:text> </xsl:text><xsl:value-of select="string(clan)"/> (group <xsl:value-of select="group"/><xsl:text>)&#013;&#010;</xsl:text>
</xsl:for-each>
</xsl:for-each>
<xsl:text>&#013;&#010;&#013;&#010;[size=18][u]Library [</xsl:text><xsl:value-of select="deck/library/@size"/><xsl:text> cards][/u][/size]&#013;&#010;&#013;&#010;</xsl:text>
<xsl:for-each select="/deck/library/card[not(type=preceding-sibling::card/type)]">
<xsl:sort select="type"/>
<xsl:call-template name="LIBPART">
<xsl:with-param name="mytype" select="string(type)" />
</xsl:call-template>
</xsl:for-each>
<xsl:text>&#013;&#010;Crafted with : </xsl:text><xsl:value-of select="//deck/@generator"/><xsl:text>. [</xsl:text><xsl:value-of select="//deck/date"/><xsl:text>]&#013;&#010;</xsl:text>
</xsl:template>

<xsl:template name="LIBPART">
<xsl:param name="mytype"/>
<xsl:if test="//card[type = $mytype]">
<xsl:text>[b][u]</xsl:text><xsl:value-of select="$mytype"/><xsl:text> [</xsl:text><xsl:value-of select="sum (//card[type = $mytype]/@count)"/><xsl:text>][/u][/b]&#013;&#010;</xsl:text>
<xsl:for-each select="//card[(type=$mytype) and not(name=preceding-sibling::card/name)]">
<xsl:sort select="name"/>
<xsl:text>  </xsl:text><xsl:call-template name="count-cards"><xsl:with-param name="myname" select="string(name)" /></xsl:call-template><xsl:text>x [url=http://www.secretlibrary.info/?lib=</xsl:text><xsl:call-template name="url-encode"><xsl:with-param name="str" select="string(name)"/></xsl:call-template><xsl:text>] </xsl:text><xsl:value-of select="string(name)"/><xsl:text> [/url]&#013;&#010;</xsl:text>
</xsl:for-each><xsl:text>&#013;&#010;</xsl:text>
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

  <xsl:template name="url-encode">
    <xsl:param name="str"/>   
    <xsl:if test="$str">
      <xsl:variable name="first-char" select="substring($str,1,1)"/>
      <xsl:choose>
        <xsl:when test="contains($safe,$first-char)">
          <xsl:value-of select="$first-char"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:variable name="codepoint">
            <xsl:choose>
              <xsl:when test="contains($ascii,$first-char)">
                <xsl:value-of select="string-length(substring-before($ascii,$first-char)) + 32"/>
              </xsl:when>
              <xsl:when test="contains($latin1,$first-char)">
                <xsl:value-of select="string-length(substring-before($latin1,$first-char)) + 160"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:message terminate="no">Warning: string contains a character that is out of range <xsl:value-of select="$first-char"/>! Substituting "?".</xsl:message>
                <xsl:text>63</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:variable>
        <xsl:variable name="hex-digit1" select="substring($hex,floor($codepoint div 16) + 1,1)"/>
        <xsl:variable name="hex-digit2" select="substring($hex,$codepoint mod 16 + 1,1)"/>
        <xsl:value-of select="concat('%',$hex-digit1,$hex-digit2)"/>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:if test="string-length($str) &gt; 1">
        <xsl:call-template name="url-encode">
          <xsl:with-param name="str" select="substring($str,2)"/>
        </xsl:call-template>
      </xsl:if>
    </xsl:if>
  </xsl:template>


<xsl:template name="str:tokenize">
	<xsl:param name="string" select="''"/>
  <xsl:param name="delimiters" select="' '"/>
  <xsl:choose>
    <xsl:when test="not($string)"/>
    <xsl:when test="not($delimiters)">
      <xsl:call-template name="str:_tokenize-characters">
        <xsl:with-param name="string" select="$string"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="str:_tokenize-delimiters">
        <xsl:with-param name="string" select="$string"/>
        <xsl:with-param name="delimiters" select="$delimiters"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="str:_tokenize-characters">
  <xsl:param name="string"/>
  <xsl:if test="$string">
    <token><xsl:value-of select="substring($string, 1, 1)"/></token>
    <xsl:call-template name="str:_tokenize-characters">
      <xsl:with-param name="string" select="substring($string, 2)"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="str:_tokenize-delimiters">
  <xsl:param name="string"/>
  <xsl:param name="delimiters"/>
  <xsl:variable name="delimiter" select="substring($delimiters, 1, 1)"/>
  <xsl:choose>
    <xsl:when test="not($delimiter)">
      <xsl:choose>
	<xsl:when test="$string != ''">
          <token>:<xsl:value-of select="$string"/>: </token>
        </xsl:when>
        <xsl:otherwise>
          <token><xsl:value-of select="$string"/></token>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="contains($string, $delimiter)">
      <xsl:if test="not(starts-with($string, $delimiter))">
        <xsl:call-template name="str:_tokenize-delimiters">
          <xsl:with-param name="string" select="substring-before($string, $delimiter)"/>
          <xsl:with-param name="delimiters" select="substring($delimiters, 2)"/>
        </xsl:call-template>
      </xsl:if>
      <xsl:call-template name="str:_tokenize-delimiters">
        <xsl:with-param name="string" select="substring-after($string, $delimiter)"/>
        <xsl:with-param name="delimiters" select="$delimiters"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="str:_tokenize-delimiters">
        <xsl:with-param name="string" select="$string"/>
        <xsl:with-param name="delimiters" select="substring($delimiters, 2)"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


</xsl:stylesheet>
