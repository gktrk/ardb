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

#include "updater.h"
#include <wx/mstream.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/filesys.h>
#include <wx/sstream.h>
#include <wx/url.h>
#include <wx/protocol/http.h>
#include <wx/dynlib.h>
#include <wx/datetime.h>

Updater *Updater::spInstance = NULL;


Updater::Updater () :
    wxDialog (0, -1, wxT ("Anarch Revolt - Database Updater"),
              wxDefaultPosition, wxSize (350, 250)),
    m_bUpdating (false),
    m_oDisciplinesArray (),
    m_pOKButton (NULL),
    m_pScrolledWindow (NULL),
    m_pScrolledSizer (NULL),
    m_pStatusLabel (NULL),
    m_sZipFile (wxT ("vtescsv.zip"))
{
    wxBoxSizer *pPapaSizer;

    pPapaSizer = new wxBoxSizer (wxVERTICAL);

    SetAutoLayout (TRUE);
    SetSizer (pPapaSizer);

    m_pScrolledWindow = new wxScrolledWindow (this);
    pPapaSizer->Add (m_pScrolledWindow, 1, wxEXPAND);
    m_pScrolledWindow->SetScrollRate (0, 10);
    m_pScrolledSizer = new wxBoxSizer (wxVERTICAL);
    m_pScrolledWindow->SetSizer (m_pScrolledSizer);
    m_pStatusLabel = new wxTextCtrl (m_pScrolledWindow, -1, wxT (""),
                                     wxDefaultPosition, wxSize (350, 200),
                                     wxTE_READONLY | wxTE_MULTILINE);
    m_pScrolledSizer->Add (m_pStatusLabel, 1, wxEXPAND);
    m_pScrolledSizer->Layout ();

    m_pOKButton = new wxButton (this, wxID_OK, wxT ("OK"));
    pPapaSizer->Add (m_pOKButton, 0, wxALIGN_CENTER_HORIZONTAL);
    m_pOKButton->Disable ();

    pPapaSizer->Layout ();
}

Updater::~Updater ()
{

}

// function adapted from the SQLiteBrowser project - I LOVE the GPL
void
Updater::decodeCSV(wxInputStream *file, char sep, char quote, int maxrecords, int *numfields, wxArrayString *pResult, bool bSkipFirstLine = true)
{
    wxString sChar, current = wxT("");
    bool inquotemode = false;
    bool inescapemode = false;
    bool wasinquotemode = false;
    int recs = 0;
    long ldummy;
    *numfields = 0;

    pResult->Alloc (3000);
    current.Alloc (3000);

    if (!pResult || !file) return;

    char acBuffer[2] = {'\0', '\0'};
    wxChar c = 0;

    if (bSkipFirstLine) {
        // Skip first line (column names)
        while (!file->Eof () && c != 10)
            c = file->GetC ();
    }

    while (!file->Eof ()) {
        //    c = file->GetC();

        file->Read (acBuffer, 1);

        // Avoid MS-Windows Latin 1 weird characters
        // ERRATUM: that obviously doesn't work at all :(
        if ((unsigned char) *acBuffer >= 0x80 &&
                (unsigned char) *acBuffer <= 0x9F) *acBuffer = ' ';

        sChar = wxString (acBuffer, wxConvISO8859_1);
        c = Updater::MakeAscii(sChar.GetChar(0));

        if (c==quote) {
            if (inquotemode) {
                if (inescapemode) {
                    inescapemode = false;
                    //add the escaped char here
                    current.Append(c);
                } else {
                    //are we escaping, or just finishing the quote?
                    char d = file->GetC();
                    if (d==quote) {
                        inescapemode = true;
                    } else {
                        inquotemode = false;
                    }
                    file->Ungetch(d);
                }
            } else {
                wasinquotemode = true;
                inquotemode = true;
            }
        } else if (c==sep) {
            if (inquotemode) {
                //add the sep here
                current.Append(c);
            } else {
                //not quoting, start new record
                current.Trim (true);
                current.Trim (false);
                current.Replace (wxT ("\""), wxT ("\"\""));
                if (wasinquotemode || !current.Length () ||
                        !current.ToLong (&ldummy)) {
                    current.Prepend (wxT ('"'));
                    current.Append ('"');
                }
                pResult->Add (current);
                wasinquotemode = false;
                current.Clear ();
            }
        } else if (c==10) {
            if (inquotemode) {
                //add the newline
                current.Append(c);
            } else {
                //not quoting, start new record
                current.Trim (true);
                current.Trim (false);
                current.Replace (wxT ("\""), wxT ("\"\""));
                if (wasinquotemode || !current.Length () ||
                        !current.ToLong (&ldummy)) {
                    current.Prepend (wxT ('"'));
                    current.Append ('"');
                }
                pResult->Add (current);
                wasinquotemode = false;
                current.Clear ();
                //for the first line, store the field count
                if (*numfields == 0) {
                    *numfields = pResult->GetCount();
                }
                recs++;
                if ((recs>maxrecords)&&(maxrecords!=-1)) {
                    break;
                }
            }
        } else if (c==13) {
            if (inquotemode) {
                //add the carrier return if in quote mode only
                current.Append(c);
            }
        } else {//another character type
            current.Append(c);
        }
    }
}

void
Updater::DeleteInstance ()
{
    if (spInstance != NULL) {
        delete spInstance;
        spInstance = NULL;
    }
}


Updater *
Updater::Instance ()
{
    if (spInstance == NULL) {
        spInstance = new Updater ();
    }
    return spInstance;
}

int
Updater::DoUpdate(UPDATE_TYPE utType)
{
    bool fUpdateDb;

    m_pStatusLabel->Clear();
    m_pOKButton->Disable();

    if (m_bUpdating) {
        return -1;
    }

    if (utType == UPDATE_FROM_MENU) {
	fUpdateDb = TRUE;
    } else {
	Log (wxT ("Checking dates of databases..."));
	fUpdateDb = UpdateDatabase();
    }

    if (fUpdateDb) {

	Show();
	wxYield ();

	Log (wxT ("Downloading..."));
	m_bUpdating = true;
	wxSafeYield(this);

	if (FetchCSVFiles() < 0) {

	    Log (wxT ("Failed.\n"));

	    wxFileDialog oFileDialog(NULL,
				     wxT ("Please locate vtescsv.zip"),
				     wxT (""), wxT ("vtescsv.zip"),
				     wxT ("*.zip"), wxOPEN);

	    if (oFileDialog.ShowModal() != wxID_OK) {
		Hide ();
		m_bUpdating = false;
		return -1;
	    }

	    m_sZipFile = oFileDialog.GetDirectory()
		<< wxFileName::GetPathSeparator()
		<< oFileDialog.GetFilename();

	    Log(wxT ("Opening "));
	    Log(m_sZipFile);
	    Log(wxT ("\n"));

	}

	Log(wxT ("\n"));
	UpdateDatabaseFromCSV();
	m_bUpdating = false;

	Log(wxT ("Database update has ended.\n"
		 "You may need to restart ARDB.\n"));
    }

    m_pOKButton->Enable();

    return 0;
}

bool Updater::UpdateDatabase()
{
    bool fUpdate = FALSE;
    wxString sServer = GetServerName();
    wxString sFile = GetFileName();
    wxFileName vtesdatabase(wxT("vtescsv.zip"));

    wxDateTime localFileTime;
    wxDateTime remoteFileTime;
    wxString localDisplayTime;

    if (vtesdatabase.FileExists()) {
        localFileTime = vtesdatabase.GetModificationTime();
    } else {
        //No file so set the time to long ago to trigger
        //an update
        localFileTime.ParseDate(wxT("01/01/1980"));
    }

    localFileTime = localFileTime.ToUTC();
    localDisplayTime = localFileTime.Format(_T("%A, %d %b %Y %H:%M:%S"));
    localDisplayTime += (_T(" GMT"));

    Log (wxT ("\n"));
    Log (wxT ("Offline Datebase: "));
    Log (localDisplayTime);
    Log (wxT ("\n"));

    wxURL url(wxT("http://") + sServer + sFile);

    if (url.GetError() == wxURL_NOERR) {

        wxInputStream *data;
        data=url.GetInputStream();
        wxHTTP* p = wxDynamicCast(&url.GetProtocol(),wxHTTP);
        wxString remoteDisplayTime = p->GetHeader(wxT("Last-Modified"));
        remoteFileTime.ParseDate(remoteDisplayTime);
        Log (wxT ("Online Datebase: "));
        Log(remoteDisplayTime);
        Log (wxT ("\n"));

    } else {
        //Error.  Probably no network access.  Lets not do an update
	remoteFileTime = localFileTime;
    }

    if (remoteFileTime.IsLaterThan(localFileTime) == TRUE) {

        wxMessageDialog oUpdate (NULL,
                                 wxT("Your database is not up to date and there is a newer version online. ")
                                 wxT("Do you wish to update? If you wish to update hit the OK button. ")
                                 wxT("If not hit the CANCEL button"),
                                 wxT ("Internet connection request"),
                                 wxOK | wxCANCEL | wxICON_QUESTION);

        if (oUpdate.ShowModal () == wxID_OK) {
	    fUpdate = TRUE;
	}
    }

    return fUpdate;

}

wxString Updater::GetServerName()
{
    wxString sServer (wxT ("www.white-wolf.com"));
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get();

    if (pConfig) {
        wxString sUpdateServer = wxT ("UpdateServer");
        pConfig->Read(sUpdateServer, &sServer);
    }

    return sServer;
}

wxString Updater::GetFileName()
{
    wxString sFile (wxT ("/VTES/downloads/vtescsv.zip"));
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get();

    if (pConfig) {
        wxString sUpdateFile = wxT ("UpdateFile");
        pConfig->Read(sUpdateFile, &sFile);
    }

    return sFile;
}

int
Updater::FetchCSVFiles()
{
    wxString sServer = GetServerName();
    wxString sFile = GetFileName();
    wxHTTP oHTTPCtrl;

    if (!oHTTPCtrl.Connect(sServer)) {
        wxMessageBox(wxString (wxT ("HTTP connection to ")) <<
                     sServer << wxT (" failed."), wxT ("HTTP Error"), wxICON_ERROR | wxOK);
        return -1;
    }

    wxInputStream *pInputStream = oHTTPCtrl.GetInputStream(sFile);

    if (!pInputStream) {
        wxMessageBox(wxString (wxT ("Failed to get :\nhttp://")) <<
                     sServer << sFile, wxT ("HTTP Error"), wxICON_ERROR | wxOK);
        return -1;
    }

    wxFileOutputStream oFileStream (wxT ("vtescsv.zip"));
    oFileStream << *pInputStream;

    return 0;
}

int
Updater::LoadDisciplinesFromCSV ()
{
    Database *pDatabase = Database::Instance ();
    wxZipInputStream oZipInputStream (m_sZipFile, wxT ("vtescrypt.csv"));
    int iZipLength = oZipInputStream.GetSize () * sizeof (char), i=0;
    unsigned int uiCrap=12;
    char *pCopyBuffer = new char [iZipLength];
    wxString sCurrent;

    oZipInputStream.Read (pCopyBuffer, iZipLength);
    do {
        switch (pCopyBuffer[i]) {
        case ',':
            m_oDisciplinesArray.Add (sCurrent);
            sCurrent.Clear ();
            break;
        default:
            sCurrent.Append (pCopyBuffer[i]);
        }
        i++;
    } while (pCopyBuffer[i] != '\n' &&
             pCopyBuffer[i] != '\r' &&
             i < iZipLength - 1);
    // add the last discipline
    if (sCurrent.Length () > 1) m_oDisciplinesArray.Add (sCurrent);
    
    delete pCopyBuffer;

    // make sure we have something
    if (m_oDisciplinesArray.GetCount () > uiCrap) {
        wxString sQuery;
        pDatabase->Query (wxT ("BEGIN TRANSACTION;"));

        // Drop the first items which are not disciplines
        m_oDisciplinesArray.RemoveAt (0, uiCrap);

        pDatabase->Query (wxT ("DELETE FROM disciplines;"));
        for (unsigned int c=0; c<m_oDisciplinesArray.GetCount (); c++) {
            sQuery.Printf (wxT("INSERT INTO disciplines"
                               " VALUES ('%s', '%s', '%s', NULL);"),
                           m_oDisciplinesArray[c].c_str (),
                           m_oDisciplinesArray[c].Left (3).Lower ().c_str (),
                           m_oDisciplinesArray[c].Left (3).Upper ().c_str ());
            pDatabase->Query (sQuery);
        }

        // handle the stupid Thanatosis discipline
        pDatabase->Query (wxT ("UPDATE disciplines SET infabbrev='thn', supabbrev='THN' WHERE name='Thanatosis';"));

	// Insert virtual disciplines Maleficia and Striga
        pDatabase->Query(wxT("INSERT INTO disciplines"
			     " VALUES ('Maleficia', 'mal', 'MAL', NULL);" ));

        pDatabase->Query(wxT("INSERT INTO disciplines"
			     " VALUES ('Striga', 'str', 'STR', NULL);" ));

        pDatabase->Query (wxT ("END TRANSACTION;"));

//       pDatabase->ToggleVerbose ();
//       pDatabase->Query (wxT ("SELECT * FROM disciplines;"));
//       pDatabase->ToggleVerbose ();

        return 1;
    }

    return 0;
}


//nClanCol is a little hack so that we can format up the clan names
//into the same format used by LSJ.  I do it here in C because I
//can do it much more easily in C than SQL  - Graham.
int
Updater::LoadTableFromCSV(wxString sTable, wxString sCSVFile, int nClanCol = -1)
{
    Database *pDatabase = Database::Instance ();

    int iNulls = 1;
    char *pCopyBuffer;
    int iZipLength, iNumFields;
    wxArrayString oList;
    wxString sValues, sQuery, sNulls, sMessage;

    wxZipInputStream oZipInputStream (m_sZipFile, sCSVFile);

    iZipLength = oZipInputStream.GetSize () * sizeof (char);
    pCopyBuffer = new char [iZipLength];
    oZipInputStream.Read (pCopyBuffer, iZipLength);

    wxMemoryInputStream oInputStream (pCopyBuffer, iZipLength);

    sMessage.Printf (wxT ("Importing file %s... "), sCSVFile.c_str ());
    Log (sMessage);

    Updater::decodeCSV (&oInputStream, ',', '"', -1, &iNumFields, &oList);

    //can not operate on an empty result
    if (iNumFields==0) return 0;

    //declare local variables we will need before the rollback jump
    int rowNum = 0;
    int colNum = 0;

    sNulls.Clear ();
    for (int i=0; i < iNulls; i++) sNulls.Append (wxT (", NULL"));

    //now lets import all data, one row at a time
    for (unsigned int i = 0; i < oList.GetCount (); i++) {
        wxString& sItem = oList.Item (i);
        if (colNum == 0) {
            sQuery = wxT ("INSERT INTO ");
            sQuery.Append (sTable);
            sQuery.Append (wxT (" VALUES("));
        }

	if (colNum == nClanCol) {

	    //This column is the clan column
	    //The clan name will have "" around it
	    //so we need to preserve these.
	    //We need to do the following:
	    //Remove any trailing 's' from the first word.
	    //Remove any trailing 's' from the last word.

	    //Remove trailing 's' last word
	    if (sItem.Len() > 2) {
		if (sItem[sItem.Len()-2] == 's') {
		    sItem[sItem.Len()-2] = '"';
		    sItem = sItem.Truncate(sItem.Len()-1);
		}
	    }

	    //Remove trailing 's' first word
	    int nPos = sItem.First(' ');
	    if (nPos != -1) {
		//Get the string before ' '
		wxString sTemp = sItem.BeforeFirst(' ');
		
		if (sTemp.Last() == 's') {
		    
		    sItem << sTemp.RemoveLast() << ' ' << sItem.AfterFirst(' ');
		    
		}
	    }
	}

        sQuery.Append (sItem);

        colNum++;
        if (colNum < iNumFields) {
            sQuery.Append (wxT(","));
        } else {

            colNum = 0;
            sQuery.Append (sNulls);
            sQuery.Append (wxT(");"));

            // Tweak : replace every occurences of 'Promo-xx' by 'Promo:xx',
            // it's easier to parse
            sQuery.Replace (wxT ("Promo-"), wxT ("Promo:"));

            // Remove all the ugly '{', '}', and '{()}' which mark erratas
            sQuery.Replace (wxT ("{()}"), wxT (""));
            sQuery.Replace (wxT ("{"), wxT (""));
            sQuery.Replace (wxT ("}"), wxT (""));

            //    printf (sQuery.mb_str (wxConvLibc));
            //    printf ("\n");

            if (pDatabase->Query (sQuery) == NULL) {
                wxLogError (wxT ("An error occured : canceling import."));
                Log(wxT ("\n"));
                delete pCopyBuffer;
                return -1;
            }
            rowNum++;
            if (!(rowNum % 100)) {
                Log (wxT ("."));
            }
        }
    }

    Log(wxT ("\n"));
    delete pCopyBuffer;
    return rowNum;
}


void
Updater::Log (wxString sText)
{
    Freeze ();
    m_pStatusLabel->SetInsertionPointEnd ();
    //  m_pScrolledSizer->Detach (m_pStatusLabel);
    m_pStatusLabel->WriteText (sText);
    //  m_pScrolledSizer->Prepend (m_pStatusLabel);
    m_pScrolledSizer->FitInside (m_pScrolledWindow);
    m_pScrolledWindow->LineDown ();
    Thaw ();
    // refresh the interface
    wxSafeYield (this);
}


int
Updater::UpdateDatabaseFromCSV ()
{
    Database *pDatabase = Database::Instance ();
    wxString sQuery;

    LoadDisciplinesFromCSV();

    pDatabase->Query (wxT ("BEGIN TRANSACTION;"));

    pDatabase->Query (wxT ("CREATE TABLE Library ("
                           "       name TEXT,"
                           "       type TEXT,"
                           "       clan TEXT,"
                           "       discipline TEXT,"
                           "       poolcost TEXT,"
                           "       bloodcost TEXT,"
                           "       convictioncost TEXT,"
                           "       burn_option TEXT,"
                           "       cardtext TEXT,"
                           "       flavortext TEXT,"
                           "       edition TEXT,"
                           "       requirement TEXT,"
                           "       title TEXT,"
                           "       artist TEXT,"
                           "       edition_save TEXT);")); // this field is required to perform the multiple passes

    sQuery = wxT ("CREATE TABLE Crypt ("
                  "       name TEXT,"
                  "       type TEXT,"
                  "       clan TEXT,"
                  "       adv TEXT,"
                  "       groupnumber int,"
                  "       capacity int,"
                  "       disciplines TEXT,"
                  "       cardtext TEXT,"
                  "       edition TEXT,"
                  "       title TEXT,"
                  "       banned TEXT,"
                  "       artist TEXT,");
    for (unsigned int c=0; c<m_oDisciplinesArray.GetCount (); c++) {
        sQuery.Append (wxT (" "));
        sQuery.Append (m_oDisciplinesArray[c].Lower ());
        sQuery.Append (wxT (" int,"));
    }
    sQuery.Append (wxT (" edition_save TEXT);")); // this field is required to perform the multiple passes
    pDatabase->Query (sQuery);


    pDatabase->Query (wxT( "CREATE TABLE cards_library_unsorted ("
                           "       card_name INTEGER,"   /* FOREIGN KEY for cards_names.record_num */
                           "       card_type INTEGER,"   /* FOREIGN KEY for cards_types.record_num */
                           "       cost STRING,"
                           "       burn_option STRING,"
                           "       requires STRING,"
                           "       discipline STRING,"
                           "       clan STRING,"
                           "       card_text INTEGER,"   /* FOREIGN KEY for cards_texts.record_num */
                           "       release_set INTEGER," /* FOREIGN KEY for cards_sets.record_num */
                           "       release_date INTEGER,"
                           "       rarity INTEGER,"      /* FOREIGN KEY for rarity_types.record_num */
                           "       artist STRING,"
                           "       url STRING,"
                           "       record_num INTEGER PRIMARY KEY);"));

    sQuery = wxT ("CREATE TABLE cards_crypt_unsorted ("
                  "       card_name INTEGER,"   /* FOREIGN KEY for cards_names.record_num */
                  "       clan INTEGER,"        /* FOREIGN KEY for cards_type.record_num */
                  "       adv INTEGER,"
                  "       groupnumber INTEGER,"
                  "       capacity INTEGER,"
                  "       disciplines STRING,"
                  "       superior STRING,"
                  "       inferior STRING,"
                  "       title STRING,"
                  "       card_text INTEGER,"   /* FOREIGN KEY for cards_texts.record_num */
                  "       release_set INTEGER," /* FOREIGN KEY for cards_set.record_num */
                  "       release_date INTEGER,"
                  "       rarity INTEGER,"      /* FOREIGN KEY for rarity_types.record_num */
                  "       artist STRING,"
                  "       url STRING,");
    for (unsigned int c=0; c<m_oDisciplinesArray.GetCount (); c++) {
        sQuery.Append (wxT (" "));
        sQuery.Append (m_oDisciplinesArray[c].Lower ());
        sQuery.Append (wxT (" INTEGER,"));
    }
    sQuery.Append (wxT (" record_num INTEGER PRIMARY KEY);"));
    pDatabase->Query (sQuery);

    pDatabase->Query (wxT( "CREATE TABLE cards_sets_unsorted ("
                           "       set_name STRING UNIQUE,"
                           "       release_date INTEGER,"
                           "       full_name STRING,"
                           "       company STRING,"
                           "       unused STRING);"));



    int iVamps = LoadTableFromCSV (wxT ("Crypt"), wxT ("vtescrypt.csv"), 2);

    //Set any group ANY to 0
    pDatabase->Query (wxT ("UPDATE Crypt SET groupnumber = 0 WHERE groupnumber = '*';"));

    int iCards = LoadTableFromCSV (wxT ("Library"), wxT ("vteslib.csv"), 2);
    int iSets = LoadTableFromCSV (wxT ("cards_sets_unsorted"), wxT ("vtessets.csv"));

    if (iVamps <= 0 || iCards <= 0 || iSets <= 0) {
        wxLogError (wxT("Cancelling changes to prevent database corruption"));
        Log (wxT("ERROR -> UPDATE ABORTED (ROLLBACK)\n"));
        pDatabase->Query (wxT ("ROLLBACK TRANSACTION;"));
        return -1;
    }

    //##############################################################################""
    // Here starts the convertion to ARDB's database schema
    //

    Log (wxT ("Converting data... "));


    /* Populating the tables */

    pDatabase->Query (wxT ("DELETE FROM cards_names;"));
    pDatabase->Query (wxT ("INSERT INTO cards_names SELECT DISTINCT name, NULL, NULL FROM Library ORDER BY name ASC;"));
    pDatabase->Query (wxT ("UPDATE cards_names SET card_table = 'cards_library';"));
    pDatabase->Query (wxT ("INSERT INTO cards_names SELECT DISTINCT name, NULL, NULL FROM Crypt ORDER BY name ASC;"));
    pDatabase->Query (wxT ("UPDATE cards_names SET card_table = 'cards_crypt' WHERE card_table ISNULL;"));

    /* Tweak a few things in the import tables */
    pDatabase->Query (wxT ("UPDATE Library SET bloodcost = bloodcost || ' blood' WHERE bloodcost != ''"));
    pDatabase->Query (wxT ("UPDATE Library SET poolcost = poolcost || ' pool ' WHERE poolcost != ''"));
    pDatabase->Query (wxT ("UPDATE Library SET convictioncost = convictioncost || ' conv ' WHERE convictioncost != ''"));
    /* Handle "Burn Option card text notice" before copy of data*/
    pDatabase->Query (wxT ("UPDATE Library SET cardtext = cardtext || ' Burn option.' WHERE burn_option = 'Y'"));
    pDatabase->Query (wxT ("UPDATE Crypt SET clan = 'Imbued' WHERE type = 'Imbued'"));

    //Clean up the clan names.  We need to remove trailing 's' from the first word in the clan name
    //and trailing 's' from teh last word in the clan name

    pDatabase->Query (wxT ("UPDATE Crypt SET clan = 'Imbued' WHERE type = 'Imbued'"));

    pDatabase->Query (wxT ("DELETE FROM cards_texts;"));
    pDatabase->Query (wxT ("INSERT INTO cards_texts SELECT DISTINCT cardtext, NULL, NULL FROM Library ORDER BY cardtext ASC;"));
    pDatabase->Query (wxT ("UPDATE cards_texts SET card_table = 'cards_library';"));
    pDatabase->Query (wxT ("INSERT INTO cards_texts SELECT DISTINCT cardtext, NULL, NULL FROM Crypt ORDER BY cardtext ASC;"));
    pDatabase->Query (wxT ("UPDATE cards_texts SET card_table = 'cards_crypt' WHERE card_table ISNULL;"));

    pDatabase->Query (wxT ("DELETE FROM cards_types;"));
    pDatabase->Query (wxT ("INSERT INTO cards_types SELECT DISTINCT type, NULL, NULL FROM Library ORDER BY type ASC;"));
    pDatabase->Query (wxT ("UPDATE cards_types SET card_table = 'cards_library';"));
    pDatabase->Query (wxT ("INSERT INTO cards_types SELECT DISTINCT clan, NULL, NULL FROM Crypt ORDER BY clan ASC;"));
    pDatabase->Query (wxT ("UPDATE cards_types SET card_table = 'cards_crypt' WHERE card_table ISNULL;"));

    pDatabase->Query (wxT ("DELETE FROM rarity_types;"));


    /* add our homemade 'Proxy' set */
    //  pDatabase->Query (wxT ("UPDATE Library SET edition = edition || ', Proxy:C';"));
    //  pDatabase->Query (wxT ("UPDATE Crypt SET edition = edition || ', Proxy:C';"));


    // let's backup the edition info, it'll be destroyed in the loop
    pDatabase->Query (wxT ("UPDATE Library SET edition_save = edition;"));
    pDatabase->Query (wxT ("UPDATE Crypt SET edition_save = edition;"));

    // this loop inserts the promo cards into the cards_sets_unsorted table
    for (int iLoop = 0; iLoop < iSets; iLoop++) {
        pDatabase->Query (wxT ("INSERT OR IGNORE INTO cards_sets_unsorted "
                               "SELECT before (edition, ':') || after (before (edition, ','), ':'),"
                               "       after (before (edition, ','), ':'),"
                               "       'Promo',"
                               "       '', "
                               "       NULL "
                               "FROM Library "
                               "WHERE edition LIKE 'Promo%';"));

        pDatabase->Query (wxT ("INSERT OR IGNORE INTO rarity_types "
                               "SELECT DISTINCT after (before (edition, ','), ':'),"
                               "        NULL "
                               "FROM Library "
                               "WHERE edition NOTNULL;"));

        pDatabase->Query (wxT ("UPDATE Library SET edition = after (edition, ' ');"));

        pDatabase->Query (wxT ("INSERT OR IGNORE INTO cards_sets_unsorted "
                               "SELECT before (edition, ':') || after (before (edition, ','), ':'),"
                               "       after (before (edition, ','), ':'),"
                               "       'Promo',"
                               "       '', "
                               "       NULL "
                               "FROM Crypt "
                               "WHERE edition LIKE 'Promo%';"));

        pDatabase->Query (wxT ("INSERT OR IGNORE INTO rarity_types "
                               "SELECT DISTINCT after (before (edition, ','), ':'),"
                               "        NULL "
                               "FROM Crypt "
                               "WHERE edition NOTNULL;"));

        pDatabase->Query (wxT ("UPDATE Crypt SET edition = after (edition, ' ');"));

        Log (wxT ("."));
    }

    // This adds all the 'proxy' sets, which double the existing ones
    pDatabase->Query (wxT ("INSERT INTO cards_sets_unsorted SELECT ' Proxy' || set_name, release_date, 'Proxy ' || full_name, 'homemade', NULL FROM cards_sets_unsorted;"));

    Log (wxT ("50%"));

    // Sort the sets
    pDatabase->Query (wxT ("DELETE FROM cards_sets"));

    pDatabase->Query (wxT ("INSERT INTO cards_sets "
                           "SELECT set_name,"
                           "       release_date,"
                           "       full_name,"
                           "       company,"
                           "       NULL "
                           "FROM cards_sets_unsorted "
                           "ORDER BY release_date ASC, set_name ASC, full_name ASC;"));
    //Change the temporary '   Proxy_*' names into a cleaner 'Proxy'
    /*
      pDatabase->Query (wxT ("UPDATE cards_sets "
      "SET set_name = 'Proxy' "
      "WHERE set_name LIKE ' Proxy %';"));
    */

    // let's restore the backuped edition info
    pDatabase->Query (wxT ("UPDATE Library SET edition = edition_save;"));
    pDatabase->Query (wxT ("UPDATE Crypt SET edition = edition_save;"));

    // This loop fills the cards_*_unsorted tables
    for (int iLoop = 0; iLoop < iSets; iLoop++) {
        /* tweak the Promo:XX editions so they'll be like PromoXX:XX */
        pDatabase->Query (wxT ("UPDATE Library "
                               "SET edition = 'Promo' "
                               "              || before(after(edition, 'Promo:'), ',') "
                               "              || ':' || after(edition, 'Promo:') "
                               "WHERE edition LIKE 'Promo%';"));
        pDatabase->Query (wxT ("UPDATE Crypt "
                               "SET edition = 'Promo' "
                               "              || before(after(edition, 'Promo:'), ',') "
                               "              || ':' || after(edition, 'Promo:') "
                               "WHERE edition LIKE 'Promo%';"));

        pDatabase->Query (wxT ("INSERT INTO cards_library_unsorted "
                               "SELECT"
                               "       cards_names.record_num,"
                               "       cards_types.record_num,"
                               "       Library.poolcost || Library.bloodcost || Library.convictioncost,"
                               "       Library.burn_option,"
                               "       Library.requirement,"
                               "       Library.discipline,"
                               "       Library.clan,"
                               "       cards_texts.record_num,"
                               "       cards_sets.record_num,"
                               "       cards_sets.release_date,"
                               "       rarity_types.record_num,"
                               "       Library.artist,"        // artist
                               "       '',"            // url
                               "       NULL "
                               "FROM"
                               "       Library,"
                               "       cards_names,"
                               "       cards_types,"
                               "       cards_sets,"
                               "       cards_texts,"
                               "       rarity_types "
                               "WHERE"
                               "       Library.edition NOTNULL AND"
                               "       cards_names.card_table = 'cards_library' AND"
                               "       cards_names.card_name = Library.name AND"
                               "       cards_texts.card_table = 'cards_library' AND"
                               "       cards_texts.card_text = Library.cardtext AND"
                               "       cards_types.card_type = Library.type AND"
                               "       cards_sets.set_name = before (Library.edition, ':') AND"
                               "       rarity_types.rarity_type = after (before (Library.edition, ','), ':');"));

        pDatabase->Query (wxT ("UPDATE Library SET edition = after (edition, ' ');"));

        // Delete the useless records to speed up things
        pDatabase->Query (wxT ("DELETE FROM Library WHERE edition ISNULL;"));

        sQuery = wxT ("INSERT INTO cards_crypt_unsorted "
                      "SELECT"
                      "       cards_names.record_num,"
                      "       cards_types.record_num,"
                      "       Crypt.adv,"
                      "       Crypt.groupnumber,"
                      "       Crypt.capacity,"
                      "       Crypt.disciplines,"
                      "       '',"             // supdisciplines
                      "       '',"             // infdisciplines
                      "       Crypt.type || ':' || Crypt.title,"
                      "       cards_texts.record_num,"
                      "       cards_sets.record_num,"
                      "       cards_sets.release_date,"
                      "       rarity_types.record_num,"
                      "       Crypt.artist,"           // artist
                      "       '',");           // url
        for (unsigned int c=0; c<m_oDisciplinesArray.GetCount (); c++) {
            sQuery.Append (wxT (" Crypt."));
            sQuery.Append (m_oDisciplinesArray[c].Lower ());
            sQuery.Append (wxT (","));
        }
        sQuery.Append (wxT ("NULL "
                            "FROM"
                            "       Crypt,"
                            "       cards_names,"
                            "       cards_types,"
                            "       cards_sets,"
                            "       cards_texts,"
                            "       rarity_types "
                            "WHERE"
                            "       Crypt.edition NOTNULL AND"
                            "       cards_names.card_table = 'cards_crypt' AND"
                            "       cards_names.card_name = Crypt.name AND"
                            "       cards_types.card_type = Crypt.clan AND"
                            "       cards_texts.card_table = 'cards_crypt' AND"
                            "       cards_texts.card_text = Crypt.cardtext AND"
                            "       cards_sets.set_name = before (Crypt.edition, ':') AND"
                            "       rarity_types.rarity_type = after (before (Crypt.edition, ','), ':');"));
        pDatabase->Query (sQuery);

        pDatabase->Query (wxT ("UPDATE Crypt SET edition = after (edition, ' ');"));

        // Delete the useless records to speed up things
        pDatabase->Query (wxT ("DELETE FROM Crypt WHERE edition ISNULL;"));

        Log (wxT ("."));
    }

    Log (wxT ("\n"));

    // Insert the crypt proxies
    sQuery = wxT ("INSERT INTO cards_crypt_unsorted "
                  "SELECT card_name,"
                  "        clan,"
                  "        adv,"
                  "        groupnumber,"
                  "        capacity,"
                  "        disciplines,"
                  "        superior,"
                  "        inferior,"
                  "        title,"
                  "        card_text,"
                  "        min (release_set)-1,"
                  "        release_date,"
                  "        rarity,"
                  "        artist,"
                  "        url,");
    for (unsigned int c=0; c<m_oDisciplinesArray.GetCount (); c++) {
        sQuery.Append (wxT (" "));
        sQuery.Append (m_oDisciplinesArray[c].Lower ());
        sQuery.Append (wxT (","));
    }
    sQuery.Append (wxT ("        NULL "
                        "FROM cards_crypt_unsorted "
                        "GROUP BY card_name, adv;"));
    pDatabase->Query (sQuery);

    // Insert the library proxies
    pDatabase->Query (wxT ("INSERT INTO cards_library_unsorted "
                           "SELECT card_name,"
                           "       card_type,"
                           "       cost,"
                           "       burn_option,"
                           "       requires,"
                           "       discipline,"
                           "       clan, "
                           "       card_text,"
                           "       min (release_set)-1,"
                           "       release_date,"
                           "       rarity,"
                           "       artist,"
                           "       url,"
                           "       NULL "
                           "FROM cards_library_unsorted "
                           "GROUP BY card_name;"));


    // sort the cards
    Log (wxT ("Sorting the cards...\n"));

    pDatabase->Query (wxT ("DROP TABLE cards_crypt"));
    pDatabase->Query (wxT ("DROP TABLE cards_crypt_ignored"));

    sQuery = wxT ("CREATE TABLE cards_crypt ("
                  "       card_name INTEGER,"   /* FOREIGN KEY for cards_names.record_num */
                  "       clan INTEGER,"        /* FOREIGN KEY for cards_type.record_num */
                  "       adv INTEGER,"
                  "       groupnumber INTEGER,"
                  "       capacity INTEGER,"
                  "       disciplines STRING,"
                  "       superior STRING,"
                  "       inferior STRING,"
                  "       title STRING,"
                  "       card_text INTEGER,"   /* FOREIGN KEY for cards_texts.record_num */
                  "       release_set INTEGER," /* FOREIGN KEY for cards_set.record_num */
                  "       release_date INTEGER,"
                  "       rarity INTEGER,"      /* FOREIGN KEY for rarity_types.record_num */
                  "       artist STRING,"
                  "       url STRING,");
    for (unsigned int c=0; c<m_oDisciplinesArray.GetCount (); c++) {
        sQuery.Append (wxT (" "));
        sQuery.Append (m_oDisciplinesArray[c].Lower ());
        sQuery.Append (wxT (" INTEGER,"));
    }
    sQuery.Append (wxT (" record_num INTEGER PRIMARY KEY);"));
    pDatabase->Query (sQuery);
    sQuery.Replace (wxT ("cards_crypt"), wxT ("cards_crypt_ignored"));
    pDatabase->Query (sQuery);

    sQuery = wxT ("INSERT INTO cards_crypt "
                  "SELECT card_name,"
                  "        clan,"
                  "        adv,"
                  "        groupnumber,"
                  "        capacity,"
                  "        disciplines,"
                  "        superior,"
                  "        inferior,"
                  "        title,"
                  "        card_text,"
                  "        release_set,"
                  "        release_date,"
                  "        rarity,"
                  "        artist,"
                  "        url,");
    for (unsigned int c=0; c<m_oDisciplinesArray.GetCount (); c++) {
        sQuery.Append (wxT (" "));
        sQuery.Append (m_oDisciplinesArray[c].Lower ());
        sQuery.Append (wxT (","));
    }
    sQuery.Append (wxT ("        NULL "
                        "FROM cards_crypt_unsorted "
                        "ORDER BY release_date ASC, release_set ASC, card_name ASC;"));
    pDatabase->Query (sQuery);

    // let's refresh the interface
    wxSafeYield (this);

    pDatabase->Query (wxT ("DELETE FROM cards_library"));
    pDatabase->Query (wxT ("DELETE FROM cards_library_ignored"));

    pDatabase->Query (wxT ("INSERT INTO cards_library "
                           "SELECT card_name,"
                           "       card_type,"
                           "       cost,"
                           "       burn_option,"
                           "       requires,"
                           "       discipline,"
                           "       clan, "
                           "       card_text,"
                           "       release_set,"
                           "       rarity,"
                           "       artist,"
                           "       url,"
                           "       NULL "
                           "FROM cards_library_unsorted "
                           "ORDER BY release_date ASC, release_set ASC, card_name ASC;"));

    // rebuild discipline information
    Log (wxT ("Rebuilding discipline information...\n"));

    for (unsigned int c=0; c<m_oDisciplinesArray.GetCount (); c++) {
        // "UPDATE cards_crypt SET inferior = inferior || 'ani ' WHERE animalism = 1;"
        sQuery.Printf (wxT ("UPDATE cards_crypt "
                            "SET inferior = inferior || (SELECT infabbrev FROM disciplines WHERE name ='%s') || ' ' "
                            "WHERE %s = 1;"),
                       m_oDisciplinesArray[c].c_str (),
                       m_oDisciplinesArray[c].Lower ().c_str ());
        pDatabase->Query (sQuery);
    }

    for (unsigned int c=0; c<m_oDisciplinesArray.GetCount (); c++) {
        // "UPDATE cards_crypt SET superior = superior || 'ANI ' WHERE animalism = 2;"
        sQuery.Printf (wxT ("UPDATE cards_crypt "
                            "SET superior = superior || (SELECT supabbrev FROM disciplines WHERE name ='%s') || ' ' "
                            "WHERE %s = 2;"),
                       m_oDisciplinesArray[c].c_str (),
                       m_oDisciplinesArray[c].Lower ().c_str ());
        pDatabase->Query (sQuery);
    }

    pDatabase->Query (wxT ("UPDATE cards_crypt SET disciplines = superior || inferior WHERE title LIKE 'Vampire%';"));

    // Clean up the mess with titles
    pDatabase->Query (wxT ("UPDATE cards_crypt SET title = after (title, ':');"));


    // Fix errata'ed cards which have -none- as a requirement
    pDatabase->Query (wxT ("UPDATE cards_library SET clan = NULL WHERE clan like '%none%';"));


    // create null inventory
    pDatabase->Query (wxT ("INSERT OR IGNORE INTO inventory_library SELECT record_num, 0, 0, 0, NULL FROM cards_library;"));
    pDatabase->Query (wxT ("INSERT OR IGNORE INTO inventory_crypt SELECT record_num, 0, 0, 0, NULL FROM cards_crypt;"));

    Log (wxT ("Cleaning up...\n"));

    pDatabase->Query (wxT ("DROP TABLE Crypt;"));
    pDatabase->Query (wxT ("DROP TABLE Library;"));
    pDatabase->Query (wxT ("DROP TABLE cards_crypt_unsorted;"));
    pDatabase->Query (wxT ("DROP TABLE cards_library_unsorted;"));
    pDatabase->Query (wxT ("DROP TABLE cards_sets_unsorted;"));

    pDatabase->Query (wxT ("COMMIT TRANSACTION;"));

    pDatabase->Query (wxT ("VACUUM;"));

    return 0;
}

wxChar Updater::MakeAscii(wxChar c)
{
    wxChar result = c;

    switch (c) {
    case 'à':
    case 'á':
    case 'â':
    case 'ã':
    case 'ä':
    case 'å':
        result = 'a';
        break;

    case 'ç':
        result = 'c';
        break;

    case 'é':
    case 'è':
    case 'ê':
    case 'ë':
        result = 'e';
        break;

    case 'ì':
    case 'í':
    case 'î':
    case 'ï':
        result = 'i';
        break;

    case 'ñ':
        result = 'n';
        break;

    case 'ò':
    case 'ó':
    case 'ô':
    case 'õ':
    case 'ö':
        result = 'o';
        break;

    case 'ù':
    case 'ú':
    case 'û':
    case 'ü':
        result = 'u';
        break;

    case 'ý':
    case 'ÿ':
        result = 'y';
        break;

    default:
        break;
    }

    return result;
}

// The commented out code below needs to be wrapped in a function
// and used to provide the merged vampire details.
//This will be added at somepoint in the future.
/*
  RecordSet *pRecordSet;

  pRecordSet = pDatabase->Query (wxT("SELECT c2.name, c2.clan, c2.groupnumber, c2.capacity, c2.disciplines, c1.cardtext, c2.cardtext FROM crypt c1, crypt c2 WHERE c1.name = c2.name AND c1.adv = '' AND c2.adv = 'Advanced';"));

  if (pRecordSet && pRecordSet->Count())
  {

  }


  pDatabase->Query (wxT ("DROP TABLE Crypt;"));
  pDatabase->Query (wxT ("DROP TABLE Library;"));


  pDatabase->Query (wxT ("COMMIT TRANSACTION;"));
  return 1;
*/
