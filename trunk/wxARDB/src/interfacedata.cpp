/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2002 Francois Gombault
 *  gombault.francois@wanadoo.fr
 *
 *  Official project page: https://savannah.nongnu.org/projects/anarchdb/
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

#include "interfacedata.h"
#include "wx/file.h"
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/image.h>

// TODO: that would be nice if all this information was fetched from the database
wxIcon *g_pIcon;
wxBitmap *g_pSplashBitmap;

wxFrame *g_pMainWindow;

InterfaceData *InterfaceData::spInstance = NULL;

unsigned int g_uiGroupCount;

void
InterfaceData::DeleteInstance ()
{
    if (spInstance != NULL) {
        delete spInstance;
        spInstance = NULL;
    }
}


InterfaceData *
InterfaceData::Instance ()
{
    if (spInstance == NULL) {
        spInstance = new InterfaceData ();

        // Check if the database looks ok
        if (!spInstance->m_bReady) {
            // Something's wrong with the DB
            delete spInstance;
            spInstance = NULL;
            wxLogError (wxT ("Ugly database error"));
        }
    }
    return spInstance;
}


InterfaceData::InterfaceData () :
    m_bReady (FALSE)
{
    Database *pDatabase = Database::Instance ();
    if (!pDatabase) return;
    RecordSet *pResult;

    // try to find the directory with our images
    if (wxFile::Exists (wxT ("./Blank.xpm")))
        m_sImagedir = wxT("./");
    else if ( wxFile::Exists(wxT("vtesicons/Blank.xpm")) )
        m_sImagedir = wxT("vtesicons/");
    else {
        wxLogWarning(wxT("Can't find image files in either '.' or 'vtesicons'!"));
        m_sImagedir = wxT ("");
    }

    // Get the Clans
    pResult = pDatabase->Query (wxT ("SELECT DISTINCT card_type FROM crypt_view ORDER BY card_type ASC"), NULL);
    if (pResult) {
        for (unsigned int i = 0; i < pResult->GetCount (); i++) {
            m_oClanArray.Add (pResult->Item (i).Item (0));
        }
    }

    // Get the Types
    //  pResult = pDatabase->Query (wxT ("SELECT DISTINCT card_type FROM cards_types WHERE card_table = 'cards_library' ORDER BY card_type ASC"), NULL);
    pResult = pDatabase->Query (wxT ("SELECT DISTINCT trim(before(card_type, '/')) AS ct FROM library_view UNION SELECT DISTINCT trim(after(card_type, '/')) AS ct FROM library_view WHERE card_type LIKE '%/%' ORDER BY ct ASC"), NULL);
    if (pResult) {
        for (unsigned int i = 0; i < pResult->GetCount (); i++) {
            m_oTypeArray.Add (pResult->Item (i).Item (0).Trim ());
        }
    }

    // Get the Titles
    pResult = pDatabase->Query (wxT ("SELECT DISTINCT title, title FROM crypt_view WHERE length(title) > 0 ORDER BY title ASC"), &m_oTitleRecord);
    if (pResult) {
        wxArrayString *pEntry1 = new wxArrayString ();
        pEntry1->Add (wxT ("no title"));
        pEntry1->Add (wxT (""));
        m_oTitleRecord.Add (pEntry1);

        wxArrayString *pEntry2 = new wxArrayString ();
        pEntry2->Add (wxT ("any title"));
        pEntry2->Add (wxT ("_%"));
        m_oTitleRecord.Add (pEntry2);

    }

    // Get the disciplines
    pResult = pDatabase->Query (wxT ("SELECT DISTINCT name, '%' ||  infabbrev || '%', '%' || supabbrev || '%' FROM disciplines ORDER BY name ASC"), &m_oDisciplineRecord);


    //  pDatabase->ToggleVerbose ();

    // Get the editions
    pResult = pDatabase->Query (wxT ("SELECT DISTINCT full_name FROM cards_sets WHERE full_name NOT LIKE 'Proxy%' ORDER BY release_date DESC"));
    if (pResult) {
        for (unsigned int i = 0; i < pResult->GetCount (); i++) {
            m_oEditionArray.Add (pResult->Item (i).Item (0));
        }
    }

    // Rarities
    {
        wxArrayString *pRarity1 = new wxArrayString ();
        pRarity1->Add (wxT ("Promo"));
        pRarity1->Add (wxT ("2"));
        m_oRarityRecord.Add (pRarity1);

        wxArrayString *pRarity2 = new wxArrayString ();
        pRarity2->Add (wxT ("Rare"));
        pRarity2->Add (wxT ("R"));
        m_oRarityRecord.Add (pRarity2);

        wxArrayString *pRarity3 = new wxArrayString ();
        pRarity3->Add (wxT ("Uncommon"));
        pRarity3->Add (wxT ("U"));
        m_oRarityRecord.Add (pRarity3);

        wxArrayString *pRarity4 = new wxArrayString ();
        pRarity4->Add (wxT ("Common"));
        pRarity4->Add (wxT ("C"));
        m_oRarityRecord.Add (pRarity4);

        wxArrayString *pRarity5 = new wxArrayString ();
        pRarity5->Add (wxT ("Preconstructed"));
        pRarity5->Add (wxT ("P"));
        m_oRarityRecord.Add (pRarity5);

        wxArrayString *pRarity6 = new wxArrayString ();
        pRarity6->Add (wxT ("Vampire"));
        pRarity6->Add (wxT ("V"));
        m_oRarityRecord.Add (pRarity6);
    }

    //  pDatabase->ToggleVerbose ();

    // Get the specials
    wxString sConfEntry = wxT ("VampireSpecial1"), sValue;
    int i = 1;
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
    if (pConfig) {
        while (pConfig->Read (sConfEntry, &sValue)) {
            wxArrayString *pEntry = new wxArrayString ();
            pEntry->Add (sValue.BeforeFirst ('|'));
            pEntry->Add (sValue.AfterFirst ('|'));
            m_oSpecialRecord.Add (pEntry);
            i++;
            sConfEntry.Printf (wxT ("VampireSpecial%d"), i);
        }
    }

    // Get the library requirements
    sConfEntry = wxT ("LibraryReq1");
    i = 1;
    if (pConfig) {
        while (pConfig->Read (sConfEntry, &sValue)) {
            wxArrayString *pEntry = new wxArrayString ();
            pEntry->Add (sValue.BeforeFirst ('|'));
            pEntry->Add (sValue.AfterFirst ('|'));
            m_oLibraryReqRecord.Add (pEntry);
            i++;
            sConfEntry.Printf (wxT ("LibraryReq%d"), i);
        }
    }

    // Get the library text patterns
    sConfEntry = wxT ("LibraryText1");
    i = 1;
    if (pConfig) {
        while (pConfig->Read (sConfEntry, &sValue)) {
            wxArrayString *pEntry = new wxArrayString ();
            pEntry->Add (sValue.BeforeFirst ('|'));
            pEntry->Add (sValue.AfterFirst ('|'));
            m_oLibraryTextRecord.Add (pEntry);
            i++;
            sConfEntry.Printf (wxT ("LibraryText%d"), i);
        }
    }

    // Get the sects
    sConfEntry = wxT ("Sect1");
    i = 1;
    if (pConfig) {
        while (pConfig->Read (sConfEntry, &sValue)) {
            wxArrayString *pEntry = new wxArrayString ();
            pEntry->Add (sValue.BeforeFirst ('|'));
            pEntry->Add (sValue.AfterFirst ('|'));
            m_oSectRecord.Add (pEntry);
            i++;
            sConfEntry.Printf (wxT ("Sect%d"), i);
        }
    }

    // Get the group number
    sConfEntry = wxT ("HighestGroup");
    if (pConfig) {
        if (!pConfig->Read (sConfEntry, (int *) &g_uiGroupCount)) {
            pConfig->Write (sConfEntry, 6);
            g_uiGroupCount = 6;
            pConfig->Flush (TRUE);
        }
    }



    m_bReady = TRUE;
}


wxStaticBitmap *
InterfaceData::MakeStaticBitmap (wxWindow* pParent, wxString& sName)
{
    if (m_sImagedir.Length () > 0) {
        wxBitmap *pBitmap;
        wxImage oImage;
        wxStaticBitmap *pStaticBitmap;
        wxString sIconName = sName;

        sIconName.Prepend (m_sImagedir);
        sIconName.Append (wxT (".xpm"));

        if (!wxFile::Exists (sIconName)) {
            // Try to return a blank icon if we cannot find the correct one
            if (sName.Cmp(wxT ("Blank"))) {
                sIconName = wxT ("Blank");
                return MakeStaticBitmap (pParent, sIconName);
            } else
                return NULL;
        }

        if (oImage.LoadFile (sIconName)) {
            pBitmap = new wxBitmap (oImage);
            pStaticBitmap = new wxStaticBitmap (pParent, -1, *pBitmap);
            return pStaticBitmap;
        }
    }

    return NULL;
}
