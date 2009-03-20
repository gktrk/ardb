/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2009 Graham Smith
 *  graham.r.smith@gmail.com
 *  
 *  Official project page: http://code.google.com/p/ardb/
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
 */

#include "DeckUpload.h"
#include <wx/sstream.h>
#include <wx/protocol/http.h>
#include <wx/textfile.h>

// Only alphanum is safe.
const char SAFE[256] =
{
	/*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
	/* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,

	/* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
	/* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
	/* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
	/* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,

	/* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

	/* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

// Uploads a deck to secret library
bool DeckUpload::Upload(wxString &sCrypt, wxString &sLibrary)
{
	wxHTTP httpPost;
	wxString postBuffer = wxT("");
	wxString encCrypt;
	wxString encLibrary;
	
	httpPost.SetHeader(wxT("Content-type"), wxT("application/x-www-form-urlencoded"));
	httpPost.SetTimeout(10); // 10 seconds of timeout instead of 10 minutes ...

	encCrypt = UriEncode(sCrypt);
	encLibrary = UriEncode(sLibrary);

	postBuffer += wxT("sl_deck_submit=1&");
	postBuffer += wxT("sl_user_agent=ardb&");
	postBuffer += wxT("sl_agent_version=2.9.0&");
	postBuffer += wxT("username=graham&");
	postBuffer += wxT("password=clare1&");
	postBuffer += wxT("title=Test+Deck&");
	postBuffer += wxT("author=Graham+Smith&");
	postBuffer += wxT("description=Description+of+the+deck&");
	postBuffer += wxT("public=1&");
	postBuffer += wxT("crypt=");
	postBuffer += encCrypt;
	postBuffer += wxT("&");
	postBuffer += wxT("library=");
	postBuffer += encLibrary;

	httpPost.SetPostBuffer(postBuffer);

	httpPost.Connect(wxT("www.secretlibrary.info"));
	
	//while (!httpPost.Connect(wxT("www.secretlibrary.info/api.php"))) // only the server, no pages here yet ...
	//{
		//wxSleep(5);
	//}

	wxInputStream *httpStream = httpPost.GetInputStream(wxT("/api.php"));

	if (httpPost.GetError() == wxPROTO_NOERR)
	{
		wxString res;
		wxStringOutputStream out_stream(&res);
		httpStream->Read(out_stream);


		wxMessageBox(res);
	}
	
	return TRUE;

}

wxString DeckUpload::UriEncode(const wxString &sSrc)
{
	const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
	const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
	const int SRC_LEN = sSrc.Length();
	unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
	unsigned char * pEnd = pStart;
	const unsigned char * const SRC_END = pSrc + SRC_LEN;

	for (; pSrc < SRC_END; ++pSrc)
	{
		if (SAFE[*pSrc])
		{
			*pEnd++ = *pSrc;
		}
		else
		{
			// escape this char
			*pEnd++ = '%';
			*pEnd++ = DEC2HEX[*pSrc >> 4];
			*pEnd++ = DEC2HEX[*pSrc & 0x0F];
		}
	}

	wxString sResult((char *)pStart, (char *)pEnd);
	
	delete [] pStart;
	return sResult;
}
