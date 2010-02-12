/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2002 Francois Gombault
 *  gombault.francois@wanadoo.fr
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

#include "cardtext.h"
#include "database.h"

void
CardText::DisplayCryptText(long lCardRef, wxArrayString *cardImageNames)
{
    wxArrayString oRecord;
    wxFont oFont;
    wxString sCopy;
    Database *pDatabase = Database::Instance ();
    RecordSet *pResultSet;
    wxString sRarityQuery, sViewQuery;
    wxString cardName;

    if (pDatabase && lCardRef > 0) {
        Freeze ();
        Clear ();

        oFont = GetDefaultStyle ().GetFont ();

        sViewQuery.Printf (wxT ("SELECT DISTINCT card_name, "
                                "       advanced, "
                                "       capacity, "
                                "       disciplines, "
                                "       card_type, "
                                "       title, "
                                "       groupnumber, "
                                "       card_text, "
                                "       superior, "
                                "       inferior "
                                "FROM crypt_view_with_proxy "
                                "WHERE card_ref = %ld"), lCardRef);
        pResultSet = pDatabase->Query (sViewQuery);
        if (pResultSet && pResultSet->GetCount () > 0) {
            oRecord = pResultSet->Item (0);

            // Get the name
            oFont.SetWeight (wxBOLD);
            SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT),
                                         wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));
            WriteText (oRecord.Item (0));

            wxString adv = oRecord.Item(1);

            cardName = CardNameToFileName(oRecord.Item(0));

            if (!adv.IsEmpty()) {
                cardName += wxT ("adv");
            }

            oFont.SetWeight (wxNORMAL);
            SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT),
                                         wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));
            // Advanced ?
            WriteText (wxT (" "));
            WriteText (oRecord.Item (1));
            WriteText (wxT ("\n"));

            // Get the clan and group
            WriteText (wxT ("Clan: "));
            WriteText (oRecord.Item (4));
            WriteText (wxT (" (group "));
            WriteText (oRecord.Item (6));
            WriteText (wxT (")\n"));

            // Get the capacity
            WriteText (wxT ("Capacity: "));
            WriteText (oRecord.Item (2));
            WriteText (wxT ("\n"));

            // Get the disciplines // Powers
            if (oRecord.Item (8).IsEmpty () && oRecord.Item (9).IsEmpty ()) {
                WriteText (wxT ("Powers (Disciplines): "));
                WriteText (oRecord.Item (3));
            } else {
                WriteText (wxT ("Disciplines: "));
                WriteText (oRecord.Item (8));
                WriteText (oRecord.Item (9));
            }
            WriteText (wxT ("\n"));

            // Get the text
            WriteText (oRecord.Item (7));
        } else {
            wxLogMessage (wxT ("Problem fetching card #%d"), lCardRef);
        }

        // Rarity
        sRarityQuery.Printf (wxT ("SELECT set_name, rarity_type FROM crypt_view WHERE card_ref IN (SELECT card_ref FROM crypt_view_with_proxy WHERE (name_ref IN (SELECT name_ref FROM crypt_view_with_proxy WHERE card_ref = %ld)) AND (advanced IN (SELECT advanced FROM crypt_view_with_proxy WHERE card_ref = %ld ))) ORDER BY set_ref"), lCardRef, lCardRef);
        pResultSet = pDatabase->Query (sRarityQuery);
        if (pResultSet && pResultSet->GetCount ()) {
            WriteText (wxT ("\n\nRarity:  "));
            for (unsigned int i = 0; i < pResultSet->GetCount (); i++) {
                if (pResultSet->Item (i).Item (0).StartsWith (wxT ("Promo"))) {
                    WriteText (wxT ("Promo"));

                    if (cardImageNames != NULL) {
                        cardImageNames->Add(wxT ("promo/") + cardName);
                    }
                } else {
                    WriteText (pResultSet->Item (i).Item (0));

                    if (cardImageNames != NULL) {
                        cardImageNames->Add((pResultSet->Item (i).Item (0)).Lower() + wxT ("/") + cardName);
                    }
                }
                WriteText (wxT (":"));
                WriteText (pResultSet->Item (i).Item (1));
                WriteText (wxT ("  "));
            }
        }
        Thaw ();
    }
}


void
CardText::DisplayLibraryText(long lCardRef, wxArrayString *cardImageNames)
{
    wxArrayString oRecord;
    wxFont oFont;
    Database *pDatabase = Database::Instance ();
    RecordSet *pResultSet;
    wxString sRarityQuery, sViewQuery;
    wxString cardName;

    if (pDatabase && lCardRef > 0) {
        Freeze ();
        Clear ();

        oFont = GetDefaultStyle ().GetFont ();

        // Get the associated pointer
        sViewQuery.Printf (wxT ("SELECT  card_name, "
                                "       card_type, "
                                "       requires || min(' ', requires) || discipline  || min(' ', discipline) || clan || min(' ', clan), "
                                "       cost, "
                                "       card_text "
                                "FROM library_view_with_proxy "
                                "WHERE card_ref = %ld"), lCardRef);
        pResultSet = pDatabase->Query (sViewQuery);
        if (pResultSet && pResultSet->GetCount () > 0) {
            oRecord = pResultSet->Item (0);

            cardName = CardNameToFileName(oRecord.Item(0));

            // Get the name
            oFont.SetWeight (wxBOLD);
            SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));
            WriteText (oRecord.Item (0));
            oFont.SetWeight (wxNORMAL);
            SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));
            WriteText (wxT ("\n"));

            // Get the type
            if (!oRecord.Item (1).IsEmpty ()) {
                WriteText (wxT ("Type: "));
                WriteText (oRecord.Item (1));
                WriteText (wxT ("\n"));
            }

            // Get the requirement
            if (!oRecord.Item (2).IsEmpty ()) {
                WriteText (wxT ("Requires: "));
                WriteText (oRecord.Item (2));
                WriteText (wxT ("\n"));
            }

            // Get the cost
            if (!oRecord.Item (3).IsEmpty ()) {
                WriteText (wxT ("Cost: "));
                WriteText (oRecord.Item (3));
                WriteText (wxT ("\n"));
            }

            // Get the text
            WriteText (oRecord.Item (4));

            // Rarity
        } else {
            wxLogMessage (wxT ("Problem fetching card #%d"), lCardRef);
        }

        sRarityQuery.Printf (wxT ("SELECT set_name, rarity_type FROM library_view WHERE card_ref IN (SELECT card_ref FROM library_view_with_proxy WHERE name_ref IN (SELECT name_ref FROM library_view_with_proxy WHERE card_ref = %ld)) ORDER BY set_ref"), lCardRef);
        pResultSet = pDatabase->Query (sRarityQuery);
        if (pResultSet && pResultSet->GetCount ()) {
            WriteText (wxT ("\n\nRarity:  "));
            for (unsigned int i = 0; i < pResultSet->GetCount (); i++) {
                if (pResultSet->Item (i).Item (0).StartsWith (wxT ("Promo"))) {
                    WriteText (wxT ("Promo"));

                    if (cardImageNames != NULL) {
                        cardImageNames->Add(wxT ("promo/") + cardName);
                    }

                } else {
                    WriteText (pResultSet->Item (i).Item (0));

                    if (cardImageNames != NULL) {
                        cardImageNames->Add((pResultSet->Item (i).Item (0)).Lower() + wxT ("/") + cardName);
                    }
                }

                WriteText (wxT (":"));
                WriteText (pResultSet->Item (i).Item (1));
                WriteText (wxT ("  "));
            }
        }
        Thaw ();
    }
}

wxString CardText::CardNameToFileName(wxString name)
{
    wxString filename;
    filename.Alloc(name.Len());

    int j = 0;

    for (int i = 0; i < name.Len(); i++) {
        switch (tolower(name[i])) {
        case 'à':
        case 'á':
        case 'â':
        case 'ã':
        case 'ä':
        case 'å':
            filename += 'a';
            break;

        case 'ç':
            filename += 'c';
            j++;
            break;

        case 'é':
        case 'è':
        case 'ê':
        case 'ë':
            filename += 'e';
            j++;
            break;

        case 'ì':
        case 'í':
        case 'î':
        case 'ï':
            filename += 'i';
            j++;
            break;

        case 'ñ':
            filename += 'n';
            j++;
            break;

        case 'ò':
        case 'ó':
        case 'ô':
        case 'õ':
        case 'ö':
            filename += 'o';
            j++;
            break;

        case 'ù':
        case 'ú':
        case 'û':
        case 'ü':
            filename += 'u';
            j++;
            break;

        case 'ý':
        case 'ÿ':
            filename += 'y';
            j++;
            break;

        case '"':
        case '`':
        case ' ':
        case ',':
        case '-':
        case '.':
        case '\'':
        case '(':
        case ')':
        case '!':
        case ':':
        case '/':
            break;

        default:
            filename += tolower(name[i]);
            break;
        }
    }

    return filename;

}
