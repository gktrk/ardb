/*	Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *	Copyright (C) 2002 Francois Gombault
 *	gombault.francois@wanadoo.fr
 *
 *	Copyright (C) 2009 Graham Smith
 *	graham.r.smith@gmail.com
 *
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include "deckmodel.h"
#include "interfacedata.h"
#include "updater.h"
#include "importxml.h"
#include "DeckUpload.h"
#include "sllogindialog.h"

#ifdef __WXMAC__
// required for some reason by libxslt on macOS X
#include <libxml/xmlexports.h>
#include <libxml/xmlversion.h>
#endif

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <math.h>
#include <stdlib.h>

#include <wx/listimpl.cpp>
#include <wx/wfstream.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>

WX_DEFINE_LIST(HappyList)


DeckModel *DeckModel::spInstance = NULL;

const wxString DeckModel::s_sDeckInfoQuery =
    wxT ("SELECT * FROM decks WHERE record_num = 0;");

const wxString DeckModel::s_sLibraryViewQuery =
    wxT ("SELECT number_used, "
         "		 card_name, "
         "		 set_name, "
         "		 card_type, "
         "		 card_ref, "
         "		 cost, "
         "		 requires, "
         "		 card_text "
         "FROM deck_view_library "
         "WHERE deck_ref = 0;");

const wxString DeckModel::s_sCryptViewQuery =
    wxT ("SELECT number_used, "
         "		 card_name, "
         "		 advanced, "
         "		 capacity, "
         "		 disciplines, "
         "		 title, "
         "		 card_type, "
         "		 groupnumber, "
         "		 set_name, "
         "		 card_text, "
         "		 card_ref "
         "FROM deck_view_crypt "
         "WHERE deck_ref = 0;");


DeckModel *
DeckModel::Instance ()
{
    if (spInstance == NULL) {
        spInstance = new DeckModel ();

        // Check if the deck model looks ok
        if (!spInstance->m_bReady) {
            // Something's wrong with the DB
            delete spInstance;
            spInstance = NULL;
        }
    }
    return spInstance;
}


void
DeckModel::DeleteInstance ()
{
    if (spInstance != NULL) {
        delete spInstance;
        spInstance = NULL;
    }
}


DeckModel::DeckModel () :
    m_bReady (TRUE),
    m_bSaved (TRUE),
    m_pView (NULL),
    m_lCryptAvg (0),
    m_lCryptMax (0),
    m_lCryptMin (0),
    m_lCryptCount (0),
    m_lLibraryCount (0),
    m_oCryptList (),
    m_oHappyList (),
    m_oLibraryList (),
    m_oLibStatsClans (),
    m_oLibStatsDisciplines (),
    m_oLibStatsRequirements (),
    m_sAuthor (),
    m_sDescription (),
    m_sName (),
    m_uiHappyDisciplineCount (3),
    m_uiHappyLibrarySize (90),
    m_uiHappyMasterPercentage (20)
{
    int iHeight = 600, iWidth = 800;
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
    if (pConfig) {
        wxString sDeckHeightEntry = wxT ("DeckWindowHeight"),
                 sDeckWidthEntry = wxT ("DeckWindowWidth");
        if (!pConfig->Read (sDeckHeightEntry, &iHeight)) {
            pConfig->Write (sDeckHeightEntry, iHeight);
            pConfig->Flush (TRUE);
        }
        if (!pConfig->Read (sDeckWidthEntry, &iWidth)) {
            pConfig->Write (sDeckWidthEntry, iWidth);
            pConfig->Flush (TRUE);
        }
    }
    m_pView = new DeckWindow (this, wxDefaultPosition, wxSize (iWidth, iHeight));

    RefreshModel (TRUE);
}


DeckModel::~DeckModel ()
{
    if (m_pView != NULL) {
        m_pView->Close ();
        m_pView = NULL;
    }
}


wxArrayString *
DeckModel::AddToCrypt (wxString sName, wxString sAdvanced, wxString sSet, unsigned int uiCount, bool bRefreshUI)
{
    Database *pDatabase = Database::Instance ();
    wxString sQuery;
    long lCardRef = -1;

    sName.Replace (wxT ("'"), wxT ("''"));

    // Check wether it's an advanced vampire or not
    if (sAdvanced.Length ()) {
        // Try to find an advanced vampire
        sQuery.Printf (wxT ("SELECT card_ref FROM crypt_view WHERE dumbitdown(card_name) LIKE dumbitdown('%s') AND advanced = 'Advanced' "), sName.c_str ());
    } else {
        // Try to find a regular vampire
        sQuery.Printf (wxT ("SELECT card_ref FROM crypt_view WHERE dumbitdown(card_name) LIKE dumbitdown('%s') "), sName.c_str ());
    }

    if (sSet.Length ()) {
        sQuery << wxT ("AND set_name = '") << sSet << wxT ("' ");
    }

    sQuery <<	 wxT ("LIMIT 1");

    RecordSet * pResult = pDatabase->Query (sQuery);

    if (pResult && pResult->GetCount () &&
            pResult->Item (0).GetCount ()) {
        // Add whatever we've found
        pResult->Item (0).Item (0).ToLong (&lCardRef);
        return AddToCrypt (lCardRef, uiCount, bRefreshUI);
    } else {
        // Or display an error
        wxLogError (wxT ("Couldn't find card %s"), sName.c_str ());
    }
    return NULL;
}


// lCardRef is cards_crypt.record_num
wxArrayString *
DeckModel::AddToCrypt (long lCardRef, unsigned int uiCount, bool bRefreshUI)
{
    Database *pDatabase = Database::Instance ();
    RecordSet oResultSet;
    wxString sQuery;
    long lRef;

    // if there's something to do
    if (uiCount != 0) {
        sQuery =
            wxT ("SELECT number_used "
                 "FROM decks_crypts "
                 "WHERE deck_ref = 0 AND "
                 "		card_ref = ");
        sQuery << lCardRef << wxT (";");

        pDatabase->Query (sQuery, &oResultSet);

        if (oResultSet.GetCount () == 0) {
            // we are adding a new card

            sQuery.Printf (wxT ("INSERT INTO decks_crypts "
                                "VALUES ("
                                "		  0, "	// deck_ref -> decks.record_num
                                "		  %ld, " // card_ref
                                "		  %d, " // number_used
                                "		  NULL);"),
                           lCardRef,
                           uiCount);

            pDatabase->Query (sQuery);
        } else {
            // we are adding more copies of an existing card

            sQuery.Printf (wxT ("UPDATE decks_crypts "
                                "SET number_used = number_used + %d "
                                "WHERE deck_ref = 0 AND "
                                "		 card_ref = %ld;"),
                           uiCount,
                           lCardRef);

            pDatabase->Query (sQuery);
        }

        m_bSaved = FALSE;
    }

    RefreshModel (bRefreshUI);

    if (uiCount != 0) {
        for (unsigned int i = 0; i < m_oCryptList.GetCount (); i++) {
            if (m_oCryptList.Item (i).Item (10).ToLong (&lRef)) {
                if (lRef == lCardRef) return &(m_oCryptList.Item (i));
            }
        }
        wxLogError (wxT ("Error while adding the card"));
    }
    return NULL;
}


wxArrayString *
DeckModel::AddToLibrary (wxString sName, wxString sSet, unsigned int uiCount, bool bRefreshUI)
{
    Database *pDatabase = Database::Instance ();
    wxString sQuery;
    long lCardRef = -1;

    sName.Replace (wxT ("'"), wxT ("''"));

    // Select the first candidate
    sQuery.Printf (wxT ("SELECT card_ref FROM library_view WHERE dumbitdown(card_name) LIKE dumbitdown('%s') "), sName.c_str ());

    if (sSet.Length ()) {
        sQuery << wxT ("AND set_name = '") << sSet << wxT ("' ");
    }

    sQuery <<	 wxT ("LIMIT 1");

    RecordSet * pResult = pDatabase->Query (sQuery);

    if (pResult && pResult->GetCount () &&
            pResult->Item (0).GetCount ()) {
        // Add whatever we've found
        pResult->Item (0).Item (0).ToLong (&lCardRef);
        return AddToLibrary (lCardRef, uiCount, bRefreshUI);
    } else {
        // Or display an error
        wxLogError (wxT ("Couldn't find card %s"), sName.c_str ());
    }
    return NULL;
}


// lCardRef is cards_library.record_num
wxArrayString *
DeckModel::AddToLibrary (long lCardRef, unsigned int uiCount, bool bRefreshUI)
{
    Database *pDatabase = Database::Instance ();
    RecordSet oResultSet;
    wxString sQuery;
    long lRef;

    if (uiCount != 0) {
        sQuery =
            wxT ("SELECT number_used "
                 "FROM decks_libraries "
                 "WHERE deck_ref = 0 AND "
                 "		card_ref = ");
        sQuery << lCardRef << wxT (";");

        pDatabase->Query (sQuery, &oResultSet);

        if (oResultSet.GetCount () == 0) {
            // we are adding a new card

            sQuery.Printf (wxT ("INSERT INTO decks_libraries "
                                "VALUES ("
                                "		  0, "	// deck_ref -> decks.record_num
                                "		  %ld, " // card_ref
                                "		  %d, " // number_used
                                "		  NULL);"),
                           lCardRef,
                           uiCount);

            pDatabase->Query (sQuery);
        } else {
            // we are adding more copies of an existing card

            sQuery.Printf (wxT ("UPDATE decks_libraries "
                                "SET number_used = number_used + %d "
                                "WHERE deck_ref = 0 AND "
                                "		 card_ref = %ld;"),
                           uiCount,
                           lCardRef);

            pDatabase->Query (sQuery);
        }

        m_bSaved = FALSE;
    }

    RefreshModel (bRefreshUI);

    if (uiCount != 0) {
        for (unsigned int i = 0; i < m_oLibraryList.GetCount (); i++) {
            if (m_oLibraryList.Item (i).Item (4).ToLong (&lRef)) {
                if (lRef == lCardRef) return &(m_oLibraryList.Item (i));
            }
        }
        wxLogError (wxT ("Error while adding the card"));
    }
    return NULL;
}


void
DeckModel::Clear ()
{
    Database *pDatabase = Database::Instance ();
    wxString sNull = wxT (""), sQuery;

    ShouldSaveWarning();

    //  SetAuthor (sNull);
    SetDescription (sNull);
    SetName (sNull);

    // Remove library cards
    sQuery.Printf (wxT ("DELETE FROM decks_libraries WHERE deck_ref = 0;"));
    pDatabase->Query (sQuery);
    // Remove crypt cards
    sQuery.Printf (wxT ("DELETE FROM decks_crypts WHERE deck_ref = 0;"));
    pDatabase->Query (sQuery);

    m_bSaved = TRUE;

    // Mirror the database changes and update the GUI
    RefreshModel (TRUE);
}


void
DeckModel::ComputeCryptHappiness ()
{
    InterfaceData *pUIData = InterfaceData::Instance ();
    Database *pDatabase = Database::Instance ();
    if (!pDatabase) return;

    long lCount, lDivisor, lComputedTotal;
    HappyBucket *pBucket, *pLastBucket;
    RecordSet *pRecordSet;
    unsigned int uiIndex, uiMasterCards;
    wxString sQuery, sLowerCaseDiscName;

    m_oHappyList.DeleteContents (TRUE);
    m_oHappyList.Clear ();

    if (m_uiHappyDisciplineCount == 0 || m_lCryptCount == 0) return;

    for (unsigned int i = 0; i < pUIData->GetCryptDisciplines ()->GetCount (); i++) {

	    sLowerCaseDiscName = pUIData->GetCryptDisciplines ()->Item (i)[0];
	    sLowerCaseDiscName = sLowerCaseDiscName.MakeLower ();

	    // Count disciplines
	    sQuery.Printf (wxT ("SELECT '%s ', sum(decks_crypts.number_used * cards_crypt.%s) FROM decks_crypts, cards_crypt WHERE (decks_crypts.deck_ref = 0) AND (decks_crypts.card_ref = cards_crypt.record_num);"), pUIData->GetCryptDisciplines ()->Item (i)[0].c_str (), sLowerCaseDiscName.c_str ());

	    pRecordSet = pDatabase->Query (sQuery);
	    if (pRecordSet) {
		if (pRecordSet->Item (0).Item (1).ToLong (&lCount) && lCount > 0) {
		    m_oHappyList.Append (new HappyBucket (pRecordSet->Item (0).Item (0), lCount));
		}
	    }
    }

    // Sort and get the top ranked disciplines
    m_oHappyList.Sort (HappyBucket::Compare);

    // The last discipline might be a tie, if so concatenate the names
    // And remove the extra disciplines, if any
    uiIndex = m_uiHappyDisciplineCount;
    if (m_oHappyList.GetCount () > m_uiHappyDisciplineCount) {
        pLastBucket = m_oHappyList.Item (uiIndex - 1)->GetData ();
        while (uiIndex < m_oHappyList.GetCount () && m_oHappyList.Item (uiIndex)) {
            pBucket = m_oHappyList.Item (uiIndex)->GetData ();
            if (pBucket->m_uiDiscTotal == pLastBucket->m_uiDiscTotal) {
                pLastBucket->m_sName << wxT ("or ") << pBucket->m_sName;
            }
            m_oHappyList.DeleteNode (m_oHappyList.Item (uiIndex));
        }
    }

    // Compute the number of Master cards
    uiMasterCards = m_uiHappyLibrarySize * m_uiHappyMasterPercentage / 100;

    // Compute the fomula's divisor
    lDivisor = m_lCryptCount;
    for (unsigned int i = 0; i < m_uiHappyDisciplineCount && i < m_oHappyList.GetCount (); i++) {
        pBucket = m_oHappyList.Item (i)->GetData ();
        lDivisor += pBucket->m_uiDiscTotal;
    }
    // Add disciplineless cards
    pBucket = new HappyBucket ();
    pBucket->m_sName = wxT ("disciplineless");
    pBucket->m_uiDiscTotal = m_lCryptCount;
    m_oHappyList.Append (pBucket);

    // Compute card counts
    lComputedTotal = uiMasterCards;
    for (unsigned int i = 0; i < m_oHappyList.GetCount (); i++) {
        pBucket = m_oHappyList.Item (i)->GetData ();
        pBucket->m_uiCardCount = (unsigned int) (((float ) pBucket->m_uiDiscTotal / lDivisor) *  (m_uiHappyLibrarySize - uiMasterCards));
        lComputedTotal += pBucket->m_uiCardCount;
    }

    // Because counts are rounded down, there might be some gaps to fill
    for (unsigned int i = 0; i < m_uiHappyLibrarySize - lComputedTotal; i++) {
        pBucket = m_oHappyList.Item (i % m_oHappyList.GetCount ())->GetData ();
        pBucket->m_uiCardCount++;
    }

    // Insert the Masters
    pBucket = new HappyBucket ();
    pBucket->m_sName = wxT ("Master");
    pBucket->m_uiCardCount = uiMasterCards;
    m_oHappyList.Insert (pBucket);
}


void
DeckModel::ComputeLibraryStats ()
{
    Database *pDatabase = Database::Instance ();
    if (!pDatabase) return;

    wxString sQuery;

    sQuery = wxT ("SELECT sum (decks_libraries.number_used), cards_library.clan FROM decks_libraries, cards_library WHERE (cards_library.clan != '') AND (decks_libraries.deck_ref = 0) AND (decks_libraries.card_ref = cards_library.record_num) GROUP BY cards_library.clan ORDER BY sum (decks_libraries.number_used) DESC;");

    pDatabase->Query (sQuery, &m_oLibStatsClans);

    sQuery = wxT ("SELECT sum (decks_libraries.number_used), cards_library.discipline FROM decks_libraries, cards_library WHERE (cards_library.discipline != '') AND (decks_libraries.deck_ref = 0) AND (decks_libraries.card_ref = cards_library.record_num) GROUP BY cards_library.discipline ORDER BY sum (decks_libraries.number_used) DESC;");

    pDatabase->Query (sQuery, &m_oLibStatsDisciplines);

    sQuery = wxT ("SELECT sum (decks_libraries.number_used), cards_library.requires FROM decks_libraries, cards_library WHERE (cards_library.requires != '') AND (decks_libraries.deck_ref = 0) AND (decks_libraries.card_ref = cards_library.record_num) GROUP BY cards_library.requires ORDER BY sum (decks_libraries.number_used) DESC;");

    pDatabase->Query (sQuery, &m_oLibStatsRequirements);

    RecordSet oTempRecord;

    sQuery = wxT ("SELECT sum (decks_libraries.number_used * cards_library.cost) FROM decks_libraries, cards_library WHERE (cards_library.cost LIKE '%blood%') AND (decks_libraries.deck_ref = 0) AND (decks_libraries.card_ref = cards_library.record_num);");

    pDatabase->Query (sQuery, &oTempRecord);
    oTempRecord.Item(0).Item(0).ToLong (&m_lTotalBloodCost);

    sQuery = wxT ("SELECT sum (decks_libraries.number_used * cards_library.cost) FROM decks_libraries, cards_library WHERE (cards_library.cost LIKE '%pool%') AND (decks_libraries.deck_ref = 0) AND (decks_libraries.card_ref = cards_library.record_num);");

    pDatabase->Query (sQuery, &oTempRecord);
    oTempRecord.Item(0).Item(0).ToLong (&m_lTotalPoolCost);
}


// Here if iCount == -1, then all copies of the card are removed
void
DeckModel::DelFromCrypt (long lCardRef, int iCount, bool bRefreshUI)
{
    Database *pDatabase = Database::Instance ();
    wxString sQuery;

    if (iCount > 0) {
        sQuery.Printf (wxT ("UPDATE decks_crypts "
                            "SET number_used = number_used - %d "
                            "WHERE deck_ref = 0 AND "
                            "		 card_ref = %ld;"),
                       iCount,
                       lCardRef);
        pDatabase->Query (sQuery);

        sQuery.Printf (wxT ("DELETE FROM decks_crypts "
                            "WHERE number_used <= 0"));
        pDatabase->Query (sQuery);
    } else {
        sQuery.Printf (wxT ("DELETE FROM decks_crypts "
                            "WHERE deck_ref = 0 AND "
                            "		 card_ref = %ld;"),
                       lCardRef);
        pDatabase->Query (sQuery);
    }

    m_bSaved = FALSE;

    RefreshModel (bRefreshUI);
}




// Here if iCount == -1, then all copies of the card are removed
void
DeckModel::DelFromLibrary (long lCardRef, int iCount, bool bRefreshUI)
{
    Database *pDatabase = Database::Instance ();
    wxString sQuery;

    if (iCount > 0) {
        sQuery.Printf (wxT ("UPDATE decks_libraries "
                            "SET number_used = number_used - %d "
                            "WHERE deck_ref = 0 AND "
                            "		 card_ref = %ld;"),
                       iCount,
                       lCardRef);
        pDatabase->Query (sQuery);

        sQuery.Printf (wxT ("DELETE FROM decks_libraries "
                            "WHERE number_used <= 0"));
        pDatabase->Query (sQuery);
    } else {
        sQuery.Printf (wxT ("DELETE FROM decks_libraries "
                            "WHERE deck_ref = 0 AND "
                            "		 card_ref = %ld;"),
                       lCardRef);
        pDatabase->Query (sQuery);
    }

    m_bSaved = FALSE;

    RefreshModel (bRefreshUI);
}

bool
DeckModel::ExportToHTML ()
{
    wxString sXSL, sConfEntry = wxT("DeckHTMLTemplate");
    wxString sFile = StripInvalidFilename(m_sName);

    sXSL << *g_pArdbDir
         << wxFileName::GetPathSeparator ()
         << wxT("xsl/deck2html_eldb.xsl");

    sFile.Append (wxT (".html"));

    wxFileDialog oFileDialog (m_pView, wxT ("Export deck..."),
                              wxT (""), sFile, wxT ("*.html"),
                              wxSAVE | wxOVERWRITE_PROMPT);
    if (oFileDialog.ShowModal () != wxID_OK) {
        return true;
    }

    sFile = oFileDialog.GetDirectory () << wxFileName::GetPathSeparator ()
            << oFileDialog.GetFilename ();

    return ExportWithXSL (sFile, &sXSL);
}


bool
DeckModel::ExportToJOL ()
{
    wxString sXSL, sConfEntry = wxT("DeckJOLTemplate");
    wxString sFile = StripInvalidFilename(m_sName);

    sXSL << *g_pArdbDir
         << wxFileName::GetPathSeparator ()
         << wxT("xsl/deck2jol.xsl");

    sFile.Append (wxT (".jol"));

    wxFileDialog oFileDialog (m_pView, wxT ("Export deck..."),
                              wxT (""), sFile, wxT ("*.jol"),
                              wxSAVE | wxOVERWRITE_PROMPT);
    if (oFileDialog.ShowModal () != wxID_OK) {
        return true;
    }

    sFile = oFileDialog.GetDirectory () << wxFileName::GetPathSeparator ()
            << oFileDialog.GetFilename ();

    return ExportWithXSL (sFile, &sXSL);
}

bool
DeckModel::ExportToLackey()
{
    wxString sXSL, sConfEntry = wxT("DeckLackeyTemplate");
    wxString sFile = StripInvalidFilename(m_sName);

    sXSL << *g_pArdbDir
         << wxFileName::GetPathSeparator ()
         << wxT("xsl/deck2lackey.xsl");

    sFile.Append (wxT (".lackey"));

    wxFileDialog oFileDialog (m_pView, wxT ("Export deck..."),
                              wxT (""), sFile, wxT ("*.lackey"),
                              wxSAVE | wxOVERWRITE_PROMPT);
    if (oFileDialog.ShowModal () != wxID_OK) {
        return true;
    }

    sFile = oFileDialog.GetDirectory () << wxFileName::GetPathSeparator ()
            << oFileDialog.GetFilename ();

    return ExportWithXSL (sFile, &sXSL);

}

bool
DeckModel::ExportToPhpBB ()
{
    wxString sXSL, sConfEntry = wxT("DeckPhpBBTemplate");
    wxString sFile = StripInvalidFilename(m_sName);

    sXSL << *g_pArdbDir
         << wxFileName::GetPathSeparator ()
         << wxT("xsl/deck2phpbb.xsl");

    sFile.Append (wxT ("-phpBB.txt"));

    wxFileDialog oFileDialog (m_pView, wxT ("Export deck..."),
                              wxT (""), sFile, wxT ("*.txt"),
                              wxSAVE | wxOVERWRITE_PROMPT);
    if (oFileDialog.ShowModal () != wxID_OK) {
        return true;
    }

    sFile = oFileDialog.GetDirectory () << wxFileName::GetPathSeparator ()
            << oFileDialog.GetFilename ();

    return ExportWithXSL (sFile, &sXSL);
}


bool
DeckModel::ExportToText ()
{
    wxString sXSL;
    wxString sConfEntry = wxT("DeckTextTemplate");
    wxString sFile = StripInvalidFilename(m_sName);

    sXSL << *g_pArdbDir
         << wxFileName::GetPathSeparator ()
         << wxT("xsl/deck2text.xsl");

    sFile.Append(wxT (".txt"));
    wxFileDialog oFileDialog (m_pView, wxT ("Export deck..."),
                              wxT (""), sFile, wxT ("*.txt"),
                              wxSAVE | wxOVERWRITE_PROMPT);

    if (oFileDialog.ShowModal () != wxID_OK) {
        return true;
    }

    sFile = oFileDialog.GetDirectory () << wxFileName::GetPathSeparator ()
            << oFileDialog.GetFilename ();

    return ExportWithXSL (sFile, &sXSL);
}


bool
DeckModel::ExportToXML ()
{
    wxString sFile = StripInvalidFilename(m_sName);

    sFile.Append (wxT (".xml"));

    wxFileDialog oFileDialog (m_pView, wxT ("Save deck..."),
                              wxT (""), sFile, wxT ("*.xml"),
                              wxSAVE | wxOVERWRITE_PROMPT);
    if (oFileDialog.ShowModal () != wxID_OK) {
        return true;
    }

    sFile = oFileDialog.GetDirectory () << wxFileName::GetPathSeparator ()
            << oFileDialog.GetFilename ();

    return ExportWithXSL (sFile, NULL);
}

bool
DeckModel::ExportToSecretLibrary(wxString &sUsername, wxString &sPassword)
{
    bool result;
    wxString sCryptXSL;
    wxString sLibraryXSL;
    wxString sTitleXSL;
    wxString sAuthorXSL;
    wxString sDescXSL;

    wxString sCrypt = wxT("");
    wxString sLibrary = wxT("");
    wxString sTitle = wxT("");
    wxString sAuthor = wxT("");
    wxString sDesc = wxT("");

    sCryptXSL << *g_pArdbDir
              << wxFileName::GetPathSeparator ()
              << wxT("xsl/crypt2text.xsl");

    sLibraryXSL << *g_pArdbDir
                << wxFileName::GetPathSeparator ()
                << wxT("xsl/library2text.xsl");

    sTitleXSL  << *g_pArdbDir
               << wxFileName::GetPathSeparator ()
               << wxT("xsl/title2text.xsl");

    sAuthorXSL  << *g_pArdbDir
                << wxFileName::GetPathSeparator ()
                << wxT("xsl/author2text.xsl");

    sDescXSL  << *g_pArdbDir
              << wxFileName::GetPathSeparator ()
              << wxT("xsl/desc2text.xsl");

    result = XmlToXslt(sCrypt, &sCryptXSL);

    if (result) {
        result = XmlToXslt(sLibrary, &sLibraryXSL);
    }

    if (result) {
        result = XmlToXslt(sTitle, &sTitleXSL);
    }

    if (result) {
        result = XmlToXslt(sAuthor, &sAuthorXSL);
    }

    if (result) {
        result = XmlToXslt(sDesc, &sDescXSL);
    }

    if (result) {
        result = DeckUpload::Upload(sCrypt,sLibrary, sTitle, sAuthor, sDesc, sUsername, sPassword);
    }

    return result;

}

bool
DeckModel::XmlToXslt(wxString &sResult, wxString *pXSL)
{
    bool bReturnValue;
    xmlDocPtr doc, res;					// document pointer
    xmlNodePtr nRoot = NULL, node = NULL; // node pointers
    xmlNodePtr nLibrary, nCrypt;			// more node pointers
    xmlDtdPtr dtd = NULL;					// DTD pointer
    xmlNodePtr nStylesheet = NULL;		// xsl stylesheet node pointer
    xsltStylesheetPtr cur = NULL;			// xsl stylesheet
    wxString sCount;
    xmlChar *resstr;
    int reslen;
#ifdef __WXMSW__
    wxString sTemp;
#endif

    LIBXML_TEST_VERSION;

    // Creates a new document, a node and set it as a root node
    doc = xmlNewDoc (BAD_CAST "1.0");
    nRoot = xmlNewNode (NULL, BAD_CAST "deck");
    my_xmlNewProp (nRoot, wxT ("formatVersion"), wxT ("-TODO-1.0"));
    my_xmlNewProp (nRoot, wxT ("databaseVersion"), wxT ("-TODO-20040101"));
    my_xmlNewProp (nRoot, wxT ("generator"), wxT ("Anarch Revolt Deck Builder"));
    xmlDocSetRootElement (doc, nRoot);

    // Creates a DTD declaration.
    dtd = xmlCreateIntSubset (doc, BAD_CAST "deck",
                              NULL, BAD_CAST "AnarchRevoltDeck.dtd");

    // Creates a default stylesheet declaration
    nStylesheet = xmlNewPI (BAD_CAST "xml-stylesheet", BAD_CAST "type=\"text/xsl\" href=\"xsl/deck2html_eldb.xsl\"");
    xmlAddPrevSibling (nRoot, nStylesheet);

    // Add the deck's information nodes
    my_xmlNewChild (nRoot, NULL, wxT ("name"), m_sName);
    my_xmlNewChild (nRoot, NULL, wxT ("date"), wxNow ());
    my_xmlNewChild (nRoot, NULL, wxT ("author"), m_sAuthor);
    // TODO: //author/@contact
#ifdef __WXMSW__
    // Under windows, this text needs CRLF reformating
    sTemp = m_sDescription;
    sTemp.Replace (wxT ("\n"), wxT ("\r\n"));
    my_xmlNewChild (nRoot, NULL, wxT ("description"), sTemp);
#else
    my_xmlNewChild (nRoot, NULL, wxT ("description"), m_sDescription);
#endif

    // Add a misc attributes node
    node = my_xmlNewChild (nRoot, NULL, wxT ("attributes"), wxT (""));
    my_xmlNewProp (node, wxT ("in_use"), wxT ("-TODO-yes"));
    my_xmlNewProp (node, wxT ("contains_proxy"), wxT ("-TODO-yes"));

    // Add the crypt node
    nCrypt = my_xmlNewChild (nRoot, NULL, wxT ("crypt"), wxT (""));
    sCount = wxT ("");
    sCount << m_lCryptCount;
    my_xmlNewProp (nCrypt, wxT("size"), sCount);
    sCount = wxT ("");
    sCount << m_lCryptMin;
    my_xmlNewProp (nCrypt, wxT("min"), sCount);
    sCount = wxT ("");
    sCount << m_lCryptMax;
    my_xmlNewProp (nCrypt, wxT("max"), sCount);
    sCount = wxT ("");
    sCount << m_lCryptAvg;
    my_xmlNewProp (nCrypt, wxT("avg"), sCount);

    // Add the crypt cards
    for (unsigned int i = 0; i < m_oCryptList.GetCount (); i++) {
        /*
        	Reminder of the query string used to fill m_oCryptList
        	"SELECT number_used, "
        	"		card_name, "
        	"		advanced, "
        	"		capacity, "
        	"		disciplines, "
        	"		title, "
        	"		card_type, "
        	"		groupnumber, "
        	"		set_name, "
        	"		card_text, "
        	"		card_ref "
        */

        // The card node contains mandatory props
        node = my_xmlNewChild (nCrypt, NULL, wxT ("vampire"), wxT (""));
        my_xmlNewProp (node, wxT ("databaseID"),
                       m_oCryptList.Item (i).Item (10));
        my_xmlNewProp (node, wxT ("count"),
                       m_oCryptList.Item (i).Item (0));

        // These are phony children nodes to help interoperabilty
        // and allow xsl to generate pretty things
        my_xmlNewChild (node, NULL, wxT ("name"),
                        m_oCryptList.Item (i).Item (1));
        my_xmlNewChild (node, NULL, wxT ("adv"),
                        m_oCryptList.Item (i).Item (2));
        my_xmlNewChild (node, NULL, wxT ("clan"),
                        m_oCryptList.Item (i).Item (6));
        my_xmlNewChild (node, NULL, wxT ("capacity"),
                        m_oCryptList.Item (i).Item (3));
        my_xmlNewChild (node, NULL, wxT ("disciplines"),
                        m_oCryptList.Item (i).Item (4));
        my_xmlNewChild (node, NULL, wxT ("title"),
                        m_oCryptList.Item (i).Item (5));
        my_xmlNewChild (node, NULL, wxT ("group"),
                        m_oCryptList.Item (i).Item (7));
        my_xmlNewChild (node, NULL, wxT ("set"),
                        m_oCryptList.Item (i).Item (8));
        my_xmlNewChild (node, NULL, wxT ("text"),
                        m_oCryptList.Item (i).Item (9));
    }

    // Add the library node
    nLibrary = my_xmlNewChild (nRoot, NULL, wxT ("library"), wxT (""));
    sCount = wxT ("");
    sCount << m_lLibraryCount;
    my_xmlNewProp (nLibrary, wxT ("size"), sCount);

    // Add the library cards
    for (unsigned int i = 0; i < m_oLibraryList.GetCount (); i++) {
        // Reminder of the query string used to fill m_oLibraryList
        // "SELECT number_used, "
        // "		 card_name, "
        // "		 set_name, "
        // "		 card_type, "
        // "		 card_ref, "
        // "		 cost, "
        // "		 requires, "
        // "		 card_text "

        // The card node contains mandatory props
        node = my_xmlNewChild (nLibrary, NULL, wxT ("card"), wxT (""));
        my_xmlNewProp (node, wxT ("databaseID"),
                       m_oLibraryList.Item (i).Item (4));
        my_xmlNewProp (node, wxT ("count"),
                       m_oLibraryList.Item (i).Item (0));

        // These are phony children nodes to help interoperabilty
        // and allow xsl to generate pretty things
        my_xmlNewChild (node, NULL, wxT ("name"),
                        m_oLibraryList.Item (i).Item (1));
        my_xmlNewChild (node, NULL, wxT ("type"),
                        m_oLibraryList.Item (i).Item (3));
        my_xmlNewChild (node, NULL, wxT ("set"),
                        m_oLibraryList.Item (i).Item (2));
        my_xmlNewChild (node, NULL, wxT ("cost"),
                        m_oLibraryList.Item (i).Item (5));
        my_xmlNewChild (node, NULL, wxT ("requirement"),
                        m_oLibraryList.Item (i).Item (6));
        my_xmlNewChild (node, NULL, wxT ("text"),
                        m_oLibraryList.Item (i).Item (7));
    }

    xmlChar acXSLname[1024];
    memcpy (acXSLname, pXSL->mb_str (wxConvLibc), (pXSL->Length () + 1) * sizeof (wxChar));

    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
    cur = xsltParseStylesheetFile(acXSLname);

    if (cur != NULL) {
        res = xsltApplyStylesheet(cur, doc, NULL);
        if (res != NULL) {
            bReturnValue = xsltSaveResultToString(&resstr, &reslen, res, cur) >= 0;
            xmlFreeDoc(res);

            if (bReturnValue) {
                //Convert resstr to wxString
                sResult = wxString((const char*)resstr,wxConvUTF8);
            }
        } else {
            bReturnValue = FALSE;
        }

        xsltFreeStylesheet(cur);
    } else {
        bReturnValue = FALSE;
        wxLogError (wxT ("Can't open XSL file %s"), pXSL->c_str ());
    }

    xsltCleanupGlobals();

    // Free the document
    xmlFreeDoc(doc);

    // Free the global variables that may have been allocated by the parser.
    xmlCleanupParser();

    return bReturnValue;
}


bool
DeckModel::ExportWithXSL (wxString &sFileName, wxString *pXSL)
{
#ifdef LIBXML_TREE_ENABLED

    bool bReturnValue;
    xmlDocPtr doc, res;					// document pointer
    xmlNodePtr nRoot = NULL, node = NULL; // node pointers
    xmlNodePtr nLibrary, nCrypt;			// more node pointers
    xmlDtdPtr dtd = NULL;					// DTD pointer
    xmlNodePtr nStylesheet = NULL;		// xsl stylesheet node pointer
    xsltStylesheetPtr cur = NULL;			// xsl stylesheet
    wxString sCount;
#ifdef __WXMSW__
    wxString sTemp;
#endif



    LIBXML_TEST_VERSION;

    // Creates a new document, a node and set it as a root node
    doc = xmlNewDoc (BAD_CAST "1.0");
    nRoot = xmlNewNode (NULL, BAD_CAST "deck");
    my_xmlNewProp (nRoot, wxT ("formatVersion"), wxT ("-TODO-1.0"));
    my_xmlNewProp (nRoot, wxT ("databaseVersion"), wxT ("-TODO-20040101"));
    my_xmlNewProp (nRoot, wxT ("generator"), wxT ("Anarch Revolt Deck Builder"));
    xmlDocSetRootElement (doc, nRoot);

    // Creates a DTD declaration.
    dtd = xmlCreateIntSubset (doc, BAD_CAST "deck",
                              NULL, BAD_CAST "AnarchRevoltDeck.dtd");

    // Creates a default stylesheet declaration
    nStylesheet = xmlNewPI (BAD_CAST "xml-stylesheet", BAD_CAST "type=\"text/xsl\" href=\"xsl/deck2html_eldb.xsl\"");
    xmlAddPrevSibling (nRoot, nStylesheet);

    // Add the deck's information nodes
    my_xmlNewChild (nRoot, NULL, wxT ("name"), m_sName);
    my_xmlNewChild (nRoot, NULL, wxT ("date"), wxNow ());
    my_xmlNewChild (nRoot, NULL, wxT ("author"), m_sAuthor);
    // TODO: //author/@contact
#ifdef __WXMSW__
    // Under windows, this text needs CRLF reformating
    sTemp = m_sDescription;
    sTemp.Replace (wxT ("\n"), wxT ("\r\n"));
    my_xmlNewChild (nRoot, NULL, wxT ("description"), sTemp);
#else
    my_xmlNewChild (nRoot, NULL, wxT ("description"), m_sDescription);
#endif

    // Add a misc attributes node
    node = my_xmlNewChild (nRoot, NULL, wxT ("attributes"), wxT (""));
    my_xmlNewProp (node, wxT ("in_use"), wxT ("-TODO-yes"));
    my_xmlNewProp (node, wxT ("contains_proxy"), wxT ("-TODO-yes"));

    // Add the crypt node
    nCrypt = my_xmlNewChild (nRoot, NULL, wxT ("crypt"), wxT (""));
    sCount = wxT ("");
    sCount << m_lCryptCount;
    my_xmlNewProp (nCrypt, wxT("size"), sCount);
    sCount = wxT ("");
    sCount << m_lCryptMin;
    my_xmlNewProp (nCrypt, wxT("min"), sCount);
    sCount = wxT ("");
    sCount << m_lCryptMax;
    my_xmlNewProp (nCrypt, wxT("max"), sCount);
    sCount = wxT ("");
    sCount << m_lCryptAvg;
    my_xmlNewProp (nCrypt, wxT("avg"), sCount);

    // Add the crypt cards
    for (unsigned int i = 0; i < m_oCryptList.GetCount (); i++) {
        /*
        	Reminder of the query string used to fill m_oCryptList
        	"SELECT number_used, "
        	"		card_name, "
        	"		advanced, "
        	"		capacity, "
        	"		disciplines, "
        	"		title, "
        	"		card_type, "
        	"		groupnumber, "
        	"		set_name, "
        	"		card_text, "
        	"		card_ref "
        */

        // The card node contains mandatory props
        node = my_xmlNewChild (nCrypt, NULL, wxT ("vampire"), wxT (""));
        my_xmlNewProp (node, wxT ("databaseID"),
                       m_oCryptList.Item (i).Item (10));
        my_xmlNewProp (node, wxT ("count"),
                       m_oCryptList.Item (i).Item (0));

        // These are phony children nodes to help interoperabilty
        // and allow xsl to generate pretty things
        my_xmlNewChild (node, NULL, wxT ("name"),
                        m_oCryptList.Item (i).Item (1));
        my_xmlNewChild (node, NULL, wxT ("adv"),
                        m_oCryptList.Item (i).Item (2));
        my_xmlNewChild (node, NULL, wxT ("clan"),
                        m_oCryptList.Item (i).Item (6));
        my_xmlNewChild (node, NULL, wxT ("capacity"),
                        m_oCryptList.Item (i).Item (3));
        my_xmlNewChild (node, NULL, wxT ("disciplines"),
                        m_oCryptList.Item (i).Item (4));
        my_xmlNewChild (node, NULL, wxT ("title"),
                        m_oCryptList.Item (i).Item (5));
        my_xmlNewChild (node, NULL, wxT ("group"),
                        m_oCryptList.Item (i).Item (7));
        my_xmlNewChild (node, NULL, wxT ("set"),
                        m_oCryptList.Item (i).Item (8));
        my_xmlNewChild (node, NULL, wxT ("text"),
                        m_oCryptList.Item (i).Item (9));
    }

    // Add the library node
    nLibrary = my_xmlNewChild (nRoot, NULL, wxT ("library"), wxT (""));
    sCount = wxT ("");
    sCount << m_lLibraryCount;
    my_xmlNewProp (nLibrary, wxT ("size"), sCount);

    // Add the library cards
    for (unsigned int i = 0; i < m_oLibraryList.GetCount (); i++) {
        // Reminder of the query string used to fill m_oLibraryList
        // "SELECT number_used, "
        // "		 card_name, "
        // "		 set_name, "
        // "		 card_type, "
        // "		 card_ref, "
        // "		 cost, "
        // "		 requires, "
        // "		 card_text "

        // The card node contains mandatory props
        node = my_xmlNewChild (nLibrary, NULL, wxT ("card"), wxT (""));
        my_xmlNewProp (node, wxT ("databaseID"),
                       m_oLibraryList.Item (i).Item (4));
        my_xmlNewProp (node, wxT ("count"),
                       m_oLibraryList.Item (i).Item (0));

        // These are phony children nodes to help interoperabilty
        // and allow xsl to generate pretty things
        my_xmlNewChild (node, NULL, wxT ("name"),
                        m_oLibraryList.Item (i).Item (1));
        my_xmlNewChild (node, NULL, wxT ("type"),
                        m_oLibraryList.Item (i).Item (3));
        my_xmlNewChild (node, NULL, wxT ("set"),
                        m_oLibraryList.Item (i).Item (2));
        my_xmlNewChild (node, NULL, wxT ("cost"),
                        m_oLibraryList.Item (i).Item (5));
        my_xmlNewChild (node, NULL, wxT ("requirement"),
                        m_oLibraryList.Item (i).Item (6));
        my_xmlNewChild (node, NULL, wxT ("text"),
                        m_oLibraryList.Item (i).Item (7));
    }


    // No XSL means we save in XML format
    if (pXSL == NULL) {
        // Write the xml tree to a file
        bReturnValue = xmlSaveFormatFileEnc (sFileName.mb_str (wxConvLibc),
                                             doc, "UTF-8", 1) >= 0;
        m_bSaved = bReturnValue;
    } else {
        xmlChar acXSLname[1024];
        memcpy (acXSLname, pXSL->mb_str (wxConvLibc),
                (pXSL->Length () + 1) * sizeof (wxChar));

        xmlSubstituteEntitiesDefault (1);
        xmlLoadExtDtdDefaultValue = 1;
        cur = xsltParseStylesheetFile (acXSLname);

        if (cur != NULL) {
            res = xsltApplyStylesheet (cur, doc, NULL);
            if (res != NULL) {
                bReturnValue = xsltSaveResultToFilename (sFileName.mb_str (wxConvLibc), res, cur, 0) >= 0;
                xmlFreeDoc (res);
            } else {
                bReturnValue = FALSE;
            }
            xsltFreeStylesheet (cur);
        } else {
            bReturnValue = FALSE;
            wxLogError (wxT ("Can't open XSL file %s"), pXSL->c_str ());
        }

        xsltCleanupGlobals ();
    }

    // Free the document
    xmlFreeDoc (doc);

    // Free the global variables that may have been allocated by the parser.
    xmlCleanupParser ();

    return bReturnValue;

#else
    wxLogError (wxT ("XML tree not supported."));
    return 0;
#endif
}


wxString&
DeckModel::GetAuthor ()
{
    return m_sAuthor;
}


RecordSet *
DeckModel::GetCryptList ()
{
    return &m_oCryptList;
}


wxString&
DeckModel::GetDescription ()
{
    return m_sDescription;
}


RecordSet *
DeckModel::GetLibraryList ()
{
    return &m_oLibraryList;
}


wxString&
DeckModel::GetName ()
{
    return m_sName;
}


bool
DeckModel::ImportFromELD ()
{
    Database *pDatabase = Database::Instance ();
    int iNumFields, iIndex = 0;
    long lCount;
    wxArrayString oArray;
    wxString sFile, sQuery;

    if (!pDatabase) return FALSE;

    wxFileDialog oFileDialog (m_pView, wxT ("Import deck..."), wxT (""), wxT (""), wxT ("*.eld"), wxOPEN);
    if (oFileDialog.ShowModal () != wxID_OK) return TRUE;

    // Clear the current deck
    Clear ();

    sFile = oFileDialog.GetDirectory ()
            << wxFileName::GetPathSeparator () << oFileDialog.GetFilename ();

    wxFileInputStream oInput (sFile);
    Updater::decodeCSV (&oInput, '§', '"', -1, &iNumFields, &oArray, FALSE);

    for (unsigned int i = 0; i < oArray.GetCount (); i++) {
        // Remove unnecessary quotes
        oArray.Item (i).Replace (wxT ("\""), wxT (""));
        // Replace ticks with apostrophes
        oArray.Item (i).Replace (wxT ("`"), wxT ("'"));
    }

    SetName (oArray.Item (iIndex++));
    SetAuthor (oArray.Item (iIndex++));
    SetDescription (oArray.Item (iIndex++));

    pDatabase->Query (wxT ("BEGIN TRANSACTION;"));

    // Get crypt count
    oArray.Item (iIndex++).ToLong (&lCount);
    // Import crypt
    for (long c = 0; c < lCount; c++) {
        wxString& sItem = oArray.Item (iIndex++);

        // Check wether it's an advanced vampire or not
        if (sItem.Replace (wxT (" (ADV)"), wxT (""))) {
            // Try to find an advanced vampire
            AddToCrypt (sItem, wxT ("Advanced"), wxT (""), 1, FALSE);
        } else {
            // Try to find a regular vampire
            AddToCrypt (sItem, wxT (""), wxT (""), 1, FALSE);
        }
    }

    // Get library count
    oArray.Item (iIndex++).ToLong (&lCount);
    // Import library
    for (long c = 0; c < lCount; c++) {
        wxString& sItem = oArray.Item (iIndex++);
        AddToLibrary (sItem, wxT (""), 1, FALSE);
    }

    pDatabase->Query (wxT ("COMMIT TRANSACTION;"));

    RefreshModel (TRUE);

    m_bSaved = TRUE;

    return TRUE;
}


bool
DeckModel::ImportFromXML ()
{
    wxString sFile;

    wxFileDialog oFileDialog (m_pView, wxT ("Open deck..."), wxT (""), wxT (""), wxT ("*.xml"), wxOPEN);
    if (oFileDialog.ShowModal () == wxID_OK) {
        // Clear the current deck
        Clear ();

        sFile = oFileDialog.GetDirectory ()
                << wxFileName::GetPathSeparator () << oFileDialog.GetFilename ();

        if (!ImportFromXML (sFile)) {
            wxLogError (wxT ("An error occured while opening %s"), sFile.c_str ());
            return FALSE;
        }
    }
    m_bSaved = TRUE;
    return TRUE;
}


bool
DeckModel::ImportFromXML (wxString &sFileName, bool bImportAll)
{
    Database *pDatabase = Database::Instance();

    bool bMorphed = FALSE;
    xmlChar *pString, *pStringName, *pStringSet, *pStringAdvanced, *pDatabaseID = 0, *pCount = 0;
    xmlDocPtr doc; // the document tree
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
    xmlNodeSetPtr nodes;
    xmlNodePtr cur, tmpnode;
    int size;
    wxString xmlStringDoc;

    xmlInitParser();
    LIBXML_TEST_VERSION;

#ifdef __WXMSW__
    xmlStringDoc = ReadXmlFile(sFileName,true);
    doc = xmlParseDoc((xmlChar *)xmlStringDoc.c_str());
    if (doc == NULL) {
        xmlStringDoc = ReadXmlFile(sFileName,false);

        doc = xmlParseDoc((xmlChar *)xmlStringDoc.c_str());

        if (doc == NULL) {
            return 0;
        }
    }
#else
    doc = xmlParseFile (sFileName.mb_str (wxConvLibc));
    if (doc == NULL) return 0;
#endif

    // Create xpath evaluation context
    xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx == NULL) {
        xmlFreeDoc(doc);
        return 0;
    }

    // TODO: check the validity using the DTD

    pDatabase->Query (wxT ("BEGIN TRANSACTION;"));

    if (bImportAll) {
        // Get the deck's name
        xpathObj = xmlXPathEvalExpression (BAD_CAST "/deck/name", xpathCtx);
        if (xpathObj == NULL) {
            xmlXPathFreeContext(xpathCtx);
            xmlFreeDoc(doc);
            return 0;
        }
        nodes = xpathObj->nodesetval;
        if (nodes->nodeTab[0]) {
            pString = xmlXPathCastNodeToString (nodes->nodeTab[0]);
            if (pString) {
                // TODO: Get the character encoding from the xml file itself
                wxString sString ((const char *) pString, wxConvUTF8);
                wxString &sStringRef = sString;
                SetName (sStringRef);
                free (pString);
            }

        }
        free (xpathObj);

        // Get the deck's author
        xpathObj = xmlXPathEvalExpression (BAD_CAST "/deck/author", xpathCtx);
        if (xpathObj == NULL) {
            xmlXPathFreeContext(xpathCtx);
            xmlFreeDoc(doc);
            return 0;
        }
        nodes = xpathObj->nodesetval;
        if (nodes->nodeTab[0]) {
            pString = xmlXPathCastNodeToString (nodes->nodeTab[0]);
            if (pString) {
                wxString sString ((const char *) pString, wxConvUTF8);
                wxString &sStringRef = sString;
                SetAuthor (sStringRef);
                free (pString);
            }

        }
        free (xpathObj);

        // Get the deck's description
        xpathObj = xmlXPathEvalExpression (BAD_CAST "/deck/description", xpathCtx);
        if (xpathObj == NULL) {
            xmlXPathFreeContext(xpathCtx);
            xmlFreeDoc(doc);
            return 0;
        }
        nodes = xpathObj->nodesetval;
        if (nodes->nodeTab[0]) {
            pString = xmlXPathCastNodeToString (nodes->nodeTab[0]);
            if (pString) {
                wxString sString ((const char *) pString, wxConvUTF8);
                wxString &sStringRef = sString;
                SetDescription (sStringRef);
                free (pString);
            }

        }
        free (xpathObj);
    }

    // Get the deck's crypt
    xpathObj = xmlXPathEvalExpression (BAD_CAST "/deck/crypt/vampire/@*", xpathCtx);
    if (xpathObj == NULL) {
        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);
        return 0;
    }
    nodes = xpathObj->nodesetval;
    size = (nodes) ? nodes->nodeNr : 0;
    for(int i = 0; i < size; i++) {
        if (nodes->nodeTab[i]) {
            cur = nodes->nodeTab[i];
            pString = xmlXPathCastNodeToString (cur);
            if (!strcmp ((char *) cur->name, "databaseID"))
                pDatabaseID = pString;
            else if (!strcmp ((char *) cur->name, "count"))
                pCount = pString;

            if (pDatabaseID && pCount) {
                long lID, lCount;
                wxString sID ((const char *) pDatabaseID, wxConvLibc),
                         sCount ((const char *) pCount, wxConvLibc);

                sID.ToLong (&lID);
                sCount.ToLong (&lCount);

                wxArrayString * pResult = AddToCrypt (lID, lCount, FALSE);

                if (pResult) {
                    // Check that the correct card has been added
                    tmpnode = cur->parent->children;
                    while (tmpnode && strcmp ((char *) tmpnode->name, "name")) {
                        tmpnode = tmpnode->next;
                    }
                    if (tmpnode) {
                        pStringName = xmlXPathCastNodeToString (tmpnode);
                        if (pStringName) {
                            wxString sStringName ((const char *) pStringName, wxConvUTF8);

                            // Compare the card names
                            if (sStringName.Cmp (pResult->Item (1))) {
                                // Get the card set
                                tmpnode = cur->parent->children;
                                while (tmpnode && strcmp ((char *) tmpnode->name, "set")) {
                                    tmpnode = tmpnode->next;
                                }
                                pStringSet = NULL;
                                if (tmpnode) {
                                    pStringSet = xmlXPathCastNodeToString (tmpnode);
                                }

                                // Get the advanced flag
                                tmpnode = cur->parent->children;
                                while (tmpnode && strcmp ((char *) tmpnode->name, "adv")) {
                                    tmpnode = tmpnode->next;
                                }
                                pStringAdvanced = NULL;
                                if (tmpnode) {
                                    pStringAdvanced = xmlXPathCastNodeToString (tmpnode);
                                }

                                if (pStringSet && pStringAdvanced) {
                                    wxString sStringSet ((const char *) pStringSet, wxConvUTF8);
                                    wxString sStringAdv ((const char *) pStringAdvanced, wxConvUTF8);

                                    DelFromCrypt (lID, lCount, FALSE);
                                    AddToCrypt (sStringName, sStringAdv, sStringSet, lCount, FALSE);
                                    bMorphed = TRUE;

                                    free (pStringSet);
                                    free (pStringAdvanced);
                                } else {
                                    DelFromCrypt (lID, lCount, FALSE);
                                    AddToCrypt (sStringName, wxEmptyString, wxEmptyString,
                                                lCount, FALSE);
                                    bMorphed = TRUE;
                                }
                            }
                            free (pStringName);
                        }
                    }
                }

                free (pDatabaseID);
                free (pCount);
                pDatabaseID = 0;
                pCount = 0;
            }
        }
    }
    free (xpathObj);

    // Get the deck's library
    xpathObj = xmlXPathEvalExpression (BAD_CAST "/deck/library/card/@*", xpathCtx);
    if (xpathObj == NULL) {
        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);
        return 0;
    }
    nodes = xpathObj->nodesetval;
    size = (nodes) ? nodes->nodeNr : 0;
    for(int i = 0; i < size; i++) {
        if (nodes->nodeTab[i]) {
            cur = nodes->nodeTab[i];
            pString = xmlXPathCastNodeToString (cur);
            if (!strcmp ((char *) cur->name, "databaseID"))
                pDatabaseID = pString;
            else if (!strcmp ((char *) cur->name, "count"))
                pCount = pString;

            if (pDatabaseID && pCount) {
                long lID, lCount;
                wxString sID ((const char *) pDatabaseID, wxConvLibc),
                         sCount ((const char *) pCount, wxConvLibc);

                sID.ToLong (&lID);
                sCount.ToLong (&lCount);

                wxArrayString * pResult = AddToLibrary (lID, lCount, FALSE);

                if (pResult) {
                    // Check that the correct card has been added
                    tmpnode = cur->parent->children;
                    while (tmpnode && strcmp ((char *) tmpnode->name, "name")) {
                        tmpnode = tmpnode->next;
                    }
                    if (tmpnode) {
                        pStringName = xmlXPathCastNodeToString (tmpnode);
                        if (pStringName) {
                            wxString sStringName ((const char *) pStringName, wxConvUTF8);

                            // Compare the card names
                            if (sStringName.Cmp (pResult->Item (1))) {
                                // Get the card set
                                tmpnode = cur->parent->children;
                                while (tmpnode && strcmp ((char *) tmpnode->name, "set")) {
                                    tmpnode = tmpnode->next;
                                }
                                pStringSet = NULL;
                                if (tmpnode) {
                                    pStringSet = xmlXPathCastNodeToString (tmpnode);
                                    if (pStringSet) {
                                        wxString sStringSet ((const char *) pStringSet, wxConvUTF8);

                                        DelFromLibrary (lID, lCount, FALSE);
                                        AddToLibrary (sStringName, sStringSet, lCount, FALSE);
                                        bMorphed = TRUE;

                                        free (pStringSet);
                                    } else {
                                        DelFromLibrary (lID, lCount, FALSE);
                                        AddToLibrary (sStringName, wxEmptyString, lCount, FALSE);
                                        bMorphed = TRUE;
                                    }
                                }
                            }
                            free (pStringName);
                        }
                    }
                }

                free (pDatabaseID);
                free (pCount);
                pDatabaseID = 0;
                pCount = 0;
            }
        }
    }
    free (xpathObj);

    // free
    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);

    // Cleanup function for the XML library.
    xmlCleanupParser();

    pDatabase->Query (wxT ("COMMIT TRANSACTION;"));

    if (bMorphed) {
        ExportWithXSL (sFileName, NULL);
    }

    RefreshModel (TRUE);

    m_bSaved = TRUE;

    return 1;
}


bool
DeckModel::MergeFromXML ()
{
    wxString sFile;

    wxFileDialog oFileDialog (NULL, wxT ("Merge deck..."), wxT (""), wxT (""), wxT ("*.xml"), wxOPEN | wxCHANGE_DIR);
    if (oFileDialog.ShowModal () == wxID_OK) {
#ifdef __WXMSW__
        sFile = oFileDialog.GetDirectory ()
                << wxT ("\\") << oFileDialog.GetFilename ();
#else
        sFile = oFileDialog.GetDirectory ()
                << wxT ("/") << oFileDialog.GetFilename ();
#endif

        if (!ImportFromXML (sFile, FALSE)) {
            wxLogError (wxT ("An error occured while opening %s"), sFile.c_str ());
            return FALSE;
        }
    }
    return TRUE;
}


xmlNodePtr
DeckModel::my_xmlNewChild (xmlNodePtr parent,
                           xmlNsPtr ns,
                           wxString sName,
                           wxString sContent)
{
#ifdef LIBXML_TREE_ENABLED
    return xmlNewTextChild (parent, ns,
                            BAD_CAST (const char *) sName.mb_str (wxConvUTF8),
                            BAD_CAST (const char *) sContent.mb_str (wxConvUTF8));
#endif
}


xmlAttrPtr
DeckModel::my_xmlNewProp (xmlNodePtr node,
                          wxString sName,
                          wxString sValue)
{
#ifdef LIBXML_TREE_ENABLED
    return xmlNewProp (node,
                       BAD_CAST (const char *) sName.mb_str (wxConvUTF8),
                       BAD_CAST (const char *) sValue.mb_str (wxConvUTF8));
#endif
}


void
DeckModel::RefreshModel (bool bRefreshUI)
{
    Database *pDatabase = Database::Instance ();
    RecordSet oResultSet;
    wxString sQuery;

    // update this model's info
    pDatabase->Query (s_sDeckInfoQuery, &oResultSet);
    if (oResultSet.GetCount () != 0 &&
            oResultSet.Item (0).GetCount () > 2) {
        m_sName = oResultSet.Item (0).Item (0);
        m_sAuthor = oResultSet.Item (0).Item (1);
        m_sDescription = oResultSet.Item (0).Item (2);
    }

    // update this model's vampire list
    pDatabase->Query (s_sCryptViewQuery, &m_oCryptList);

    // get the crypt stats
    sQuery.Printf (wxT ("SELECT SUM(number_used), "
                        "		  MIN(capacity), "
                        "		  MAX(capacity), "
                        "		  SUM(1.0 * capacity * number_used) / SUM(number_used) "
                        "FROM deck_view_crypt "
                        "WHERE deck_ref = 0;"));
    pDatabase->Query (sQuery, &oResultSet);
    if (oResultSet.GetCount () != 0 &&
            oResultSet.Item (0).GetCount () != 0) {
        oResultSet.Item (0).Item (0).ToLong (&m_lCryptCount);
        oResultSet.Item (0).Item (1).ToLong (&m_lCryptMin);
        oResultSet.Item (0).Item (2).ToLong (&m_lCryptMax);
        oResultSet.Item (0).Item (3).ToDouble (&m_lCryptAvg);
        //m_lCryptAvg = roundf (m_lCryptAvg * 100) / 100;
        m_lCryptAvg = (m_lCryptAvg * 100) / 100;
    }

    // update this model's cardlist
    pDatabase->Query (s_sLibraryViewQuery, &m_oLibraryList);

    // count the cards
    sQuery.Printf (wxT ("SELECT SUM(number_used) "
                        "FROM decks_libraries "
                        "WHERE deck_ref = 0;"));
    pDatabase->Query (sQuery, &oResultSet);
    if (oResultSet.GetCount () != 0 &&
            oResultSet.Item (0).GetCount () != 0) {
        oResultSet.Item (0).Item (0).ToLong (&m_lLibraryCount);
    }

    // compute library stats
    ComputeLibraryStats ();

    // Compute Happy Families stuff
    ComputeCryptHappiness ();

    if (bRefreshUI) {
        // update the view
        m_pView->UpdateView ();
    }
}


void
DeckModel::ResizeLibrary (unsigned int uiCount)
{
    Database *pDatabase = Database::Instance ();
    wxString sQuery, sSummary;
    long lNewTotal = 0;

    do {
        float fTheoreticalTotal = 0, fFudge = 0, fRatio = (float) uiCount / GetLibraryCount ();
        lNewTotal = 0;
        sSummary = wxT ("Changes\n");

        pDatabase->Query (wxT ("BEGIN TRANSACTION;"));

        for (unsigned int c=0; c < GetLibraryList ()->GetCount (); c++) {
            long lOldAmount;
            GetLibraryList ()->Item (c)[0].ToLong (&lOldAmount);

            float fNewAmount = fRatio * lOldAmount;
            unsigned int uiNewAmount = (unsigned int) fNewAmount;

            float fProba = fNewAmount - uiNewAmount;
            float fRand = (float) rand () / RAND_MAX;
            if (fRand < fProba - fFudge) uiNewAmount++;

            int iDiff = uiNewAmount - lOldAmount;
            if (iDiff > 0) sSummary << wxT ("+");
            if (iDiff != 0) sSummary << iDiff << wxT (" ")<< GetLibraryList ()->Item (c)[1] << wxT ("\n");

            long lCardRef;
            GetLibraryList ()->Item (c)[4].ToLong (&lCardRef);
            sQuery.Printf (wxT ("UPDATE decks_libraries "
                                "SET number_used = %d "
                                "WHERE deck_ref = 0 AND "
                                "		 card_ref = %ld;"),
                           uiNewAmount,
                           lCardRef);
            pDatabase->Query (sQuery);

            lNewTotal += uiNewAmount;
            fTheoreticalTotal += fNewAmount;
            fFudge = (lNewTotal - fTheoreticalTotal) / 5;

            //	  printf ("%.3f %d %s fudge:%.3f\n", fNewAmount, uiNewAmount, GetLibraryList ()->Item (c)[1].c_str (), fFudge);
        }

        if (lNewTotal != uiCount) pDatabase->Query (wxT ("ROLLBACK TRANSACTION;"));
    } while (lNewTotal != uiCount);

    sQuery.Printf (wxT ("DELETE FROM decks_libraries "
                        "WHERE deck_ref = 0 AND "
                        "		 number_used = 0;"));
    pDatabase->Query (sQuery);

    pDatabase->Query (wxT ("END TRANSACTION;"));


    //  printf ("%d ~ %d\n", uiCount, lNewTotal);
    wxLogMessage (sSummary);

    RefreshModel (TRUE);
    m_bSaved = FALSE;
}


void
DeckModel::SetAuthor (wxString &sAuthor)
{
    Database *pDatabase = Database::Instance ();
    RecordSet oResultSet;
    wxString sQuery, sEscapedValue;

    if (m_sAuthor.Cmp(sAuthor) != 0) {
	 m_sAuthor = sAuthor;
	 sEscapedValue = sAuthor;
	 sEscapedValue.Replace (wxT ("'"), wxT ("''"));
	 
	 sQuery.Printf (wxT ("UPDATE decks "
			     "SET deck_creator = '%s' "
			     "WHERE record_num = 0"),
			sEscapedValue.c_str ());

	 pDatabase->Query (sQuery, &oResultSet);

	 m_bSaved = FALSE;
    }
}


void
DeckModel::SetCryptRefAmount (long lCardRef, unsigned int uiCount, bool bRefreshUI)
{
    Database *pDatabase = Database::Instance ();
    wxString sQuery;

    if (uiCount == 0) {
        DelFromCrypt (lCardRef, -1, bRefreshUI);
        return;
    }

    sQuery.Printf (wxT ("UPDATE decks_crypts "
                        "SET number_used = %d "
                        "WHERE deck_ref = 0 AND "
                        "		 card_ref = %ld;"),
                   uiCount,
                   lCardRef);

    pDatabase->Query (sQuery);

    m_bSaved = FALSE;
    RefreshModel (bRefreshUI);
}


void
DeckModel::SetDescription (wxString &sDescription)
{
    Database *pDatabase = Database::Instance ();
    RecordSet oResultSet;
    wxString sQuery, sEscapedValue;

    if (m_sDescription.Cmp(sDescription) != 0) {

	 m_sDescription = sDescription;	 
	 

	 sEscapedValue = sDescription;
	 sEscapedValue.Replace (wxT ("'"), wxT ("''"));

	 sQuery.Printf (wxT ("UPDATE decks "
			     "SET deck_desc = '%s' "
			     "WHERE record_num = 0"),
			sEscapedValue.c_str ());

	 pDatabase->Query (sQuery, &oResultSet);

	 m_bSaved = FALSE;
    }
}


void
DeckModel::SetLibraryRefAmount (long lCardRef, unsigned int uiCount, bool bRefreshUI)
{
    Database *pDatabase = Database::Instance ();
    wxString sQuery;

    if (uiCount == 0) {
        DelFromLibrary (lCardRef, -1, bRefreshUI);
        return;
    }

    sQuery.Printf (wxT ("UPDATE decks_libraries "
                        "SET number_used = %d "
                        "WHERE deck_ref = 0 AND "
                        "		 card_ref = %ld;"),
                   uiCount,
                   lCardRef);

    pDatabase->Query (sQuery);

    m_bSaved = FALSE;
    RefreshModel (bRefreshUI);
}


void
DeckModel::SetName (wxString &sName)
{
    Database *pDatabase = Database::Instance ();
    RecordSet oResultSet;
    wxString sQuery, sEscapedValue;

    if (m_sName.Cmp(sName) != 0) {
	 
	 m_sName = sName;
	 sEscapedValue = sName;
	 sEscapedValue.Replace (wxT ("'"), wxT ("''"));

	 sQuery.Printf (wxT ("UPDATE decks "
			     "SET deck_name = '%s' "
			     "WHERE record_num = 0"),
			sEscapedValue.c_str ());

	 pDatabase->Query (sQuery, &oResultSet);

	 m_bSaved = FALSE;
    }
}


void
DeckModel::ShouldSaveWarning ()
{
    if (!m_bSaved) {
        wxMessageDialog oWarningDialog (NULL, 
					wxT ("Do you want to save this deck before proceeding ?"), 
					wxT ("Save deck ?"), 
					wxYES | wxNO | wxICON_QUESTION);

        if (oWarningDialog.ShowModal () == wxID_YES) {
            ExportToXML ();
        }

        m_bSaved = TRUE;
    }
}

wxString DeckModel::StripInvalidFilename(wxString name)
{
    wxString result = name;
    wxChar invalidChars[] = {wxT('"'), wxT('*'), wxT('<'), wxT('>'), wxT('['), wxT(']'), wxT('='), wxT('+'), wxT('\\'), wxT('/'), wxT(','), wxT('.'), wxT(':'), wxT(';'), wxT('{'), wxT('}')};

    for(int i=0; i<name.Len(); i++) {
        for(int j=0; j<sizeof(invalidChars); j++) {
            if (name[i] == invalidChars[j]) {
                result[i] = wxT(' ');
            }
        }
    }

    return result;
}

