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


#ifdef __WXMAC__
// Apple stuff
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
#endif // __WXMAC__


#include "database.h"
#include "updater.h"

#include "ctype.h" // for tolower()


Database *Database::spInstance = NULL;


/*
** Implementation of the before() function
** returns the first element of the string
** arg 0 = string (TEXT)
** arg 1 = separator (TEXT)
*/
static void
beforeFunc(sqlite_func *context, int argc, const char **argv)
{
    const char *pHaystack, *pNeedle;
    char *pFirstNeedle;
    int iNeedleLength = 0;

    if (argc != 2) return;

    pHaystack = argv[0];
    pNeedle = argv[1];

    if (pHaystack == 0) {
        sqlite_set_result_string(context, 0, 0);
        return;
    }

    if (pNeedle != 0) {
        iNeedleLength = strlen (pNeedle);
    }

    pFirstNeedle = (char*)strstr (pHaystack, pNeedle);

    if (pFirstNeedle == 0) {
        sqlite_set_result_string(context, argv[0], -1);
    }
    //   else if (pFirstNeedle == pHaystack)
    //     {
    //       sqlite_set_result_string(context, 0, 0);
    //     }
    else {
        sqlite_set_result_string(context, argv[0], pFirstNeedle - pHaystack - iNeedleLength + 1);
    }
}


/*
** Implementation of the after() function
** returns the remaining of the string
** arg 0 = string (TEXT)
** arg 1 = separator (TEXT)
*/
static void
afterFunc(sqlite_func *context, int argc, const char **argv)
{
    const char *pHaystack, *pNeedle;
    const char *pFirstNeedle;
    int iNeedleLength = 0;

    if (argc !=2 ) return;

    wxString sHaystack (argv[0], wxConvISO8859_1), sNeedle (argv[1], wxConvISO8859_1);

    pHaystack = argv[0];
    pNeedle = argv[1];

    if (pHaystack == 0) {
        sqlite_set_result_string(context, 0, 0);
        return;
    }

    if (pNeedle != 0) {
        iNeedleLength = strlen (pNeedle);
    }

    //  pFirstNeedle = strstr (pHaystack, pNeedle);
    pFirstNeedle = pHaystack + sHaystack.Find (wxString (pNeedle, wxConvISO8859_1));

    if (pFirstNeedle == pHaystack - 1) {
        sqlite_set_result_string(context, 0, 0);
    } else {
        sqlite_set_result_string(context, pFirstNeedle + iNeedleLength, -1);
    }
}


/*
** Implementation of the after() function
** returns the remaining of the string
** arg 0 = string (TEXT)
** arg 1 = separator (TEXT)
*/
static void
trimFunc(sqlite_func *context, int argc, const char **argv)
{
    if (argc !=1 || argv[0] == 0) return;

    const char *pArg = argv[0];

    while (pArg[0] == ' ') pArg++;

    int iStringLength = strlen (pArg);
    char *pResultString = (char *) malloc ((iStringLength + 1) * sizeof (char));

    strncpy (pResultString, pArg, (iStringLength + 1) * sizeof (char));

    char *p = pResultString + strlen (pResultString) - 1;
    while (p[0] == ' ' && p >= pResultString)
        p--;

    // truncate at trailing space start
    *++p = '\0';

    sqlite_set_result_string(context, pResultString, -1);

    free(pResultString);
}


// "Dumb it down" takes a string, and returns
// its lowercase non-accented equivalent
static void
dumbitdownFunc(sqlite_func *context, int argc, const char **argv)
{
    char *pResultString;
    int iStringLength;

    if (argc != 1) return;

    iStringLength = strlen (argv[0]);
    pResultString = (char *) malloc ((iStringLength + 1) * sizeof (char));

    for (int i = 0; i < iStringLength; i++) {
        pResultString[i] = tolower (argv[0][i]);
        switch (argv[0][i]) {
        case 'à':
        case 'á':
        case 'â':
        case 'ã':
        case 'ä':
        case 'å':
            pResultString[i] = 'a';
            break;

        case 'ç':
            pResultString[i] = 'c';
            break;

        case 'é':
        case 'è':
        case 'ê':
        case 'ë':
            pResultString[i] = 'e';
            break;

        case 'ì':
        case 'í':
        case 'î':
        case 'ï':
            pResultString[i] = 'i';
            break;

        case 'ñ':
            pResultString[i] = 'n';
            break;

        case 'ò':
        case 'ó':
        case 'ô':
        case 'õ':
        case 'ö':
            pResultString[i] = 'o';
            break;

        case 'ù':
        case 'ú':
        case 'û':
        case 'ü':
            pResultString[i] = 'u';
            break;

        case 'ý':
        case 'ÿ':
            pResultString[i] = 'y';
            break;

        case '"':
        case '`':
            pResultString[i] = '\'';
            break;

        default:
            break;
        }
    }

    pResultString[iStringLength] = 0;
    sqlite_set_result_string(context, pResultString, -1);
    free(pResultString);
}


int
Database::Callback (void *pUserData, int argc, char **argv, char ** WXUNUSED (azColName))
{
    wxArrayString *pRecord = new wxArrayString();
    RecordSet *pResultSet = (RecordSet *) pUserData;
    Database *pDB = Database::Instance();

    for (int i = 0; i < argc; i++) {
        if (pDB->IsVerbose ()) {
            printf (argv[i]);
            printf (" ");
        }
        pRecord->Add(wxString (argv[i], wxConvISO8859_1));
    }

    if (pDB->IsVerbose ()) {
        printf ("\n");
    }

    pResultSet->Add(pRecord);

    return 0;
}


int
Database::CreateBlankDatabase ()
{
    char *pErrorMsg = 0;

    m_pCardsDB = sqlite_open (m_sDatabaseFileName.mb_str (wxConvISO8859_1), 0, &pErrorMsg);
    if (m_pCardsDB == 0) {
        wxLogError (wxT ("Can't open database %s: %s\n"), m_sDatabaseFileName.c_str (), pErrorMsg);
        m_bReady = FALSE;
    }
    if (m_pCardsDB) {
        CreateCoreTables ();
        CreateDeckTables ();
        CreateInventoryTables ();
        CreateViews ();
    }

    return 0;
}


int
Database::CreateCoreTables ()
{
    m_bMustUpdate = true;

    Query (wxT ("CREATE TABLE cards_names ("
                "       card_name STRING,"
                "       card_table STRING,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE INDEX card_name_table_index ON cards_names (card_table);"));

    Query (wxT ("CREATE TABLE cards_types ("
                "       card_type STRING,"
                "       card_table STRING,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE TABLE cards_sets ("
                "       set_name STRING,"
                "       release_date INTEGER,"
                "       full_name STRING,"
                "       company STRING,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE TABLE cards_texts ("
                "       card_text STRING,"
                "       card_table STRING,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE INDEX card_text_table_index ON cards_texts (card_table);"));

    Query (wxT( "CREATE TABLE disciplines ("
                "       name STRING UNIQUE,"
                "       infabbrev STRING,"
                "       supabbrev STRING,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE TABLE rarity_types ("
                "       rarity_type STRING UNIQUE,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE TABLE cards_library ("
                "       card_name INTEGER,"   /* FOREIGN KEY for cards_names.record_num */
                "       card_type INTEGER,"   /* FOREIGN KEY for cards_types.record_num */
                "       cost STRING,"
                "       burn_option STRING,"
                "       requires STRING,"
                "       discipline STRING,"
                "       clan STRING,"
                "       card_text INTEGER,"   /* FOREIGN KEY for cards_texts.record_num */
                "       release_set INTEGER," /* FOREIGN KEY for cards_sets.record_num */
                "       rarity INTEGER,"      /* FOREIGN KEY for rarity_types.record_num */
                "       artist STRING,"
                "       url STRING,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE INDEX library_req_index ON cards_library (requires);"));

    Query (wxT ("CREATE TABLE cards_library_ignored ("
                "       card_name INTEGER,"   /* FOREIGN KEY for cards_names.record_num */
                "       card_type INTEGER,"   /* FOREIGN KEY for cards_types.record_num */
                "       cost STRING,"
                "       burn_option STRING,"
                "       requires STRING,"
                "       discipline STRING,"
                "       clan STRING,"
                "       card_text INTEGER,"   /* FOREIGN KEY for cards_texts.record_num */
                "       release_set INTEGER," /* FOREIGN KEY for cards_sets.record_num */
                "       rarity INTEGER,"      /* FOREIGN KEY for rarity_types.record_num */
                "       artist STRING,"
                "       url STRING,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE TABLE cards_crypt ("
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
                "       rarity INTEGER,"      /* FOREIGN KEY for rarity_types.record_num */
                "       artist STRING,"
                "       url STRING,"
                "       animalism INTEGER,"
                "       auspex INTEGER,"
                "       celerity INTEGER,"
                "       chimerstry INTEGER,"
                "       daimoinon INTEGER,"
                "       dementation INTEGER,"
                "       dominate INTEGER,"
                "       fortitude INTEGER,"
                "       melpominee INTEGER,"
                "       mytherceria INTEGER,"
                "       necromancy INTEGER,"
                "       obeah INTEGER,"
                "       obfuscate INTEGER,"
                "       obtenebration INTEGER,"
                "       potence INTEGER,"
                "       presence INTEGER,"
                "       protean INTEGER,"
                "       quietus INTEGER,"
                "       sanguinus INTEGER,"
                "       serpentis INTEGER,"
                "       spiritus INTEGER,"
                "       temporis INTEGER,"
                "       thanatosis INTEGER,"
                "       thaumaturgy INTEGER,"
                "       valeren INTEGER,"
                "       vicissitude INTEGER,"
                "       visceratika INTEGER,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE TABLE cards_crypt_ignored ("
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
                "       rarity INTEGER,"      /* FOREIGN KEY for rarity_types.record_num */
                "       artist STRING,"
                "       url STRING,"
                "       animalism INTEGER,"
                "       auspex INTEGER,"
                "       celerity INTEGER,"
                "       chimerstry INTEGER,"
                "       daimoinon INTEGER,"
                "       dementation INTEGER,"
                "       dominate INTEGER,"
                "       fortitude INTEGER,"
                "       melpominee INTEGER,"
                "       mytherceria INTEGER,"
                "       necromancy INTEGER,"
                "       obeah INTEGER,"
                "       obfuscate INTEGER,"
                "       obtenebration INTEGER,"
                "       potence INTEGER,"
                "       presence INTEGER,"
                "       protean INTEGER,"
                "       quietus INTEGER,"
                "       sanguinus INTEGER,"
                "       serpentis INTEGER,"
                "       spiritus INTEGER,"
                "       temporis INTEGER,"
                "       thanatosis INTEGER,"
                "       thaumaturgy INTEGER,"
                "       valeren INTEGER,"
                "       vicissitude INTEGER,"
                "       visceratika INTEGER,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT( "CREATE TABLE library_selection ("
                "       browser_num INTEGER,"
                "       card_name INTEGER,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE TABLE crypt_selection ("
                "       browser_num INTEGER,"
                "       card_name INTEGER,"
                "       record_num INTEGER PRIMARY KEY);"));

    return 0;
}


int
Database::CreateDeckTables ()
{
    Query (wxT ("CREATE TABLE decks ("
                "       deck_name STRING,"
                "       deck_creator STRING,"
                "       deck_desc STRING,"
                "       deck_active BOOL,"
                "       deck_virtual BOOL,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE TABLE decks_crypts ("
                "       deck_ref INTEGER,"   /* FOREIGN KEY for decks.record_num */
                "       card_ref INTEGER,"   /* FOREIGN KEY for cards_crypt.record_num */
                "       number_used INTEGER,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE TABLE decks_libraries ("
                "       deck_ref INTEGER,"   /* FOREIGN KEY for decks.record_num */
                "       card_ref INTEGER,"   /* FOREIGN KEY for cards_library.record_num */
                "       number_used INTEGER,"
                "       record_num INTEGER PRIMARY KEY);"));


    // create the "deck-in-the-works" reference
    Query (wxT ("INSERT INTO decks VALUES('', '', '', 0, 1, 0);"));

    return 0;
}


int
Database::CreateInventoryTables ()
{
    Query (wxT ("CREATE TABLE inventory_library ("
                "       card_ref INTEGER UNIQUE,"   /* FOREIGN KEY for cards_library.record_num */
                "       number_owned INTEGER,"
                "       number_to_trade INTEGER,"
                "       number_wanted INTEGER,"
                "       record_num INTEGER PRIMARY KEY);"));

    Query (wxT ("CREATE TABLE inventory_crypt ("
                "       card_ref INTEGER UNIQUE,"   /* FOREIGN KEY for cards_crypt.record_num */
                "       number_owned INTEGER,"
                "       number_to_trade INTEGER,"
                "       number_wanted INTEGER,"
                "       record_num INTEGER PRIMARY KEY);"));

    return 0;
}


int
Database::CreateViews ()
{
    Query (wxT ("CREATE VIEW library_view AS "
                "SELECT cards_library.record_num AS card_ref,"
                "       cards_names.record_num AS name_ref,"
                "       cards_names.card_name AS card_name,"
                "       cards_types.card_type AS card_type,"
                "       cards_library.cost AS cost,"
                "       cards_library.burn_option AS burn_option,"
                "       cards_library.requires AS requires,"
                "       cards_library.clan AS clan,"
                "       cards_library.discipline AS discipline,"
                "       cards_texts.card_text AS card_text,"
                "       cards_sets.set_name AS set_name,"
                "       cards_sets.record_num AS set_ref,"
                "       rarity_types.rarity_type AS rarity_type,"
                "       cards_library.artist AS artist,"
                "       cards_library.url AS url, "
                "       inventory_library.number_owned AS have, "
                "       inventory_library.number_to_trade AS spare, "
                "       inventory_library.number_wanted AS want "
                "FROM   cards_library,"
                "       cards_names,"
                "       cards_types,"
                "       cards_sets,"
                "       cards_texts,"
                "       inventory_library, "
                "       rarity_types "
                "WHERE  cards_names.record_num = cards_library.card_name AND"
                "       cards_types.record_num = cards_library.card_type AND"
                "       cards_sets.record_num = cards_library.release_set AND"
                "       cards_texts.record_num = cards_library.card_text AND"
                "       rarity_types.record_num = cards_library.rarity AND"
                "       cards_library.record_num = inventory_library.card_ref AND"
                "       cards_sets.full_name NOT LIKE 'Proxy%';"));

    Query (wxT ("CREATE VIEW library_view_unified AS "
                "SELECT min(cards_library.record_num) AS card_ref,"
                "       cards_names.record_num AS name_ref,"
                "       cards_names.card_name AS card_name,"
                "       cards_types.card_type AS card_type,"
                "       cards_library.cost AS cost,"
                "       cards_library.burn_option AS burn_option,"
                "       cards_library.requires AS requires,"
                "       cards_library.clan AS clan,"
                "       cards_library.discipline AS discipline,"
                "       cards_texts.card_text AS card_text,"
                "       cards_library.artist AS artist,"
                "       cards_library.url AS url, "
                "       sum(inventory_library.number_owned) AS have, "
                "       sum(inventory_library.number_to_trade) AS spare, "
                "       sum(inventory_library.number_wanted) AS want "
                "FROM   cards_library,"
                "       cards_names,"
                "       cards_types,"
                "       cards_texts,"
                "       inventory_library "
                "WHERE  cards_names.record_num = cards_library.card_name AND"
                "       cards_types.record_num = cards_library.card_type AND"
                "       cards_texts.record_num = cards_library.card_text AND"
                "       cards_library.record_num = inventory_library.card_ref "
                "GROUP BY name_ref;"));

    Query (wxT ("CREATE VIEW library_view_with_proxy AS "
                "SELECT cards_library.record_num AS card_ref,"
                "       cards_names.record_num AS name_ref,"
                "       cards_names.card_name AS card_name,"
                "       cards_types.card_type AS card_type,"
                "       cards_library.cost AS cost,"
                "       cards_library.burn_option AS burn_option,"
                "       cards_library.requires AS requires,"
                "       cards_library.clan AS clan,"
                "       cards_library.discipline AS discipline,"
                "       cards_texts.card_text AS card_text,"
                "       cards_sets.set_name AS set_name,"
                "       cards_sets.record_num AS set_ref,"
                "       rarity_types.rarity_type AS rarity_type,"
                "       cards_library.artist AS artist,"
                "       cards_library.url AS url, "
                "       inventory_library.number_owned AS have, "
                "       inventory_library.number_to_trade AS spare, "
                "       inventory_library.number_wanted AS want "
                "FROM   cards_library,"
                "       cards_names,"
                "       cards_types,"
                "       cards_sets,"
                "       cards_texts,"
                "       inventory_library, "
                "       rarity_types "
                "WHERE  cards_names.record_num = cards_library.card_name AND"
                "       cards_types.record_num = cards_library.card_type AND"
                "       cards_sets.record_num = cards_library.release_set AND"
                "       cards_texts.record_num = cards_library.card_text AND"
                "       rarity_types.record_num = cards_library.rarity AND"
                "       cards_library.record_num = inventory_library.card_ref;"));

    Query (wxT ("CREATE VIEW crypt_view AS "
                "SELECT cards_crypt.record_num AS card_ref,"
                "       cards_names.record_num AS name_ref,"
                "       cards_names.card_name AS card_name,"
                "       cards_types.card_type AS card_type,"
                "       cards_crypt.adv AS advanced,"
                "       cards_crypt.groupnumber AS groupnumber,"
                "       cards_crypt.capacity AS capacity,"
                "       cards_crypt.disciplines AS disciplines,"
                "       cards_crypt.superior AS superior,"
                "       cards_crypt.inferior AS inferior,"
                "       cards_crypt.title AS title,"
                "       cards_texts.card_text AS card_text,"
                "       cards_sets.set_name AS set_name,"
                "       cards_sets.record_num AS set_ref,"
                "       rarity_types.rarity_type AS rarity_type,"
                "       cards_crypt.artist AS artist,"
                "       cards_crypt.url AS url, "
                "       inventory_crypt.number_owned AS have, "
                "       inventory_crypt.number_to_trade AS spare, "
                "       inventory_crypt.number_wanted AS want "
                "FROM   cards_crypt,"
                "       cards_names,"
                "       cards_types,"
                "       cards_sets,"
                "       cards_texts,"
                "       inventory_crypt, "
                "       rarity_types "
                "WHERE  cards_names.record_num = cards_crypt.card_name AND"
                "       cards_types.record_num = cards_crypt.clan AND"
                "       cards_sets.record_num = cards_crypt.release_set AND"
                "       cards_texts.record_num = cards_crypt.card_text AND"
                "       rarity_types.record_num = cards_crypt.rarity AND"
                "       cards_crypt.record_num = inventory_crypt.card_ref AND"
                "       cards_sets.full_name NOT LIKE 'Proxy%';"));

    Query (wxT ("CREATE VIEW crypt_view_unified AS "
                "SELECT min(cards_crypt.record_num) AS card_ref,"
                "       cards_names.record_num AS name_ref,"
                "       cards_names.card_name AS card_name,"
                "       cards_types.card_type AS card_type,"
                "       cards_crypt.adv AS advanced,"
                "       cards_crypt.groupnumber AS groupnumber,"
                "       cards_crypt.capacity AS capacity,"
                "       cards_crypt.disciplines AS disciplines,"
                "       cards_crypt.superior AS superior,"
                "       cards_crypt.inferior AS inferior,"
                "       cards_crypt.title AS title,"
                "       cards_texts.card_text AS card_text,"
                "       cards_crypt.artist AS artist,"
                "       cards_crypt.url AS url, "
                "       sum(inventory_crypt.number_owned) AS have, "
                "       sum(inventory_crypt.number_to_trade) AS spare, "
                "       sum(inventory_crypt.number_wanted) AS want "
                "FROM   cards_crypt,"
                "       cards_names,"
                "       cards_types,"
                "       cards_texts,"
                "       inventory_crypt "
                "WHERE  cards_names.record_num = cards_crypt.card_name AND"
                "       cards_types.record_num = cards_crypt.clan AND"
                "       cards_texts.record_num = cards_crypt.card_text AND"
                "       cards_crypt.record_num = inventory_crypt.card_ref "
                "GROUP BY name_ref, advanced;"));

    Query (wxT ("CREATE VIEW crypt_view_with_proxy AS "
                "SELECT cards_crypt.record_num AS card_ref,"
                "       cards_names.record_num AS name_ref,"
                "       cards_names.card_name AS card_name,"
                "       cards_types.card_type AS card_type,"
                "       cards_crypt.adv AS advanced,"
                "       cards_crypt.groupnumber AS groupnumber,"
                "       cards_crypt.capacity AS capacity,"
                "       cards_crypt.disciplines AS disciplines,"
                "       cards_crypt.superior AS superior,"
                "       cards_crypt.inferior AS inferior,"
                "       cards_crypt.title AS title,"
                "       cards_texts.card_text AS card_text,"
                "       cards_sets.set_name AS set_name,"
                "       cards_sets.record_num AS set_ref,"
                "       rarity_types.rarity_type AS rarity_type,"
                "       cards_crypt.artist AS artist,"
                "       cards_crypt.url AS url, "
                "       inventory_crypt.number_owned AS have, "
                "       inventory_crypt.number_to_trade AS spare, "
                "       inventory_crypt.number_wanted AS want "
                "FROM   cards_crypt,"
                "       cards_names,"
                "       cards_types,"
                "       cards_sets,"
                "       cards_texts,"
                "       inventory_crypt, "
                "       rarity_types "
                "WHERE  cards_names.record_num = cards_crypt.card_name AND"
                "       cards_types.record_num = cards_crypt.clan AND"
                "       cards_sets.record_num = cards_crypt.release_set AND"
                "       cards_texts.record_num = cards_crypt.card_text AND"
                "       rarity_types.record_num = cards_crypt.rarity AND"
                "       cards_crypt.record_num = inventory_crypt.card_ref;"));

    Query (wxT ("CREATE VIEW deck_view_crypt AS "
                "SELECT decks.record_num AS deck_ref,"
                "       decks.deck_name AS deck_name,"
                "       decks_crypts.number_used AS number_used,"
                "       cards_names.card_name AS card_name,"
                "       cards_types.card_type AS card_type,"
                "       cards_crypt.adv AS advanced,"
                "       cards_crypt.groupnumber AS groupnumber,"
                "       cards_crypt.capacity AS capacity,"
                "       cards_crypt.disciplines AS disciplines,"
                "       cards_crypt.title AS title,"
                "       cards_texts.card_text AS card_text,"
                "       cards_sets.set_name AS set_name,"
                "       rarity_types.rarity_type AS rarity,"
                "       cards_crypt.record_num AS card_ref "
                "FROM   decks_crypts,"
                "       cards_crypt,"
                "       decks,"
                "       cards_names,"
                "       cards_types,"
                "       cards_texts,"
                "       rarity_types,"
                "       cards_sets "
                "WHERE  cards_crypt.record_num = decks_crypts.card_ref AND"
                "       cards_texts.record_num = cards_crypt.card_text AND"
                "       cards_names.record_num = cards_crypt.card_name AND"
                "       cards_types.record_num = cards_crypt.clan AND"
                "       cards_sets.record_num = cards_crypt.release_set AND"
                "       rarity_types.record_num = cards_crypt.rarity;"));

    Query (wxT ("CREATE VIEW deck_view_library AS "
                "SELECT decks.record_num AS deck_ref,"
                "       decks.deck_name AS deck_name,"
                "       decks_libraries.number_used AS number_used,"
                "       cards_names.card_name AS card_name,"
                "       cards_types.card_type AS card_type,"
                "       cards_texts.card_text AS card_text,"
                "       cards_library.cost AS cost,"
                "       cards_library.requires AS requires,"
                "       cards_sets.set_name AS set_name,"
                "       rarity_types.rarity_type AS rarity,"
                "       cards_library.record_num AS card_ref "
                "FROM   decks_libraries,"
                "       cards_library,"
                "       decks,"
                "       cards_names,"
                "       cards_texts,"
                "       cards_types,"
                "       rarity_types,"
                "       cards_sets "
                "WHERE  cards_library.record_num = decks_libraries.card_ref AND"
                "       cards_texts.record_num = cards_library.card_text AND"
                "       cards_names.record_num = cards_library.card_name AND"
                "       cards_types.record_num = cards_library.card_type AND"
                "       cards_sets.record_num = cards_library.release_set AND"
                "       rarity_types.record_num = cards_library.rarity;"));

    Query (wxT ("CREATE VIEW deck_view AS "
                "SELECT deck_ref"
                "       deck_name,"
                "       number_used,"
                "       card_name,"
                "       card_type,"
                "       set_name,"
                "       rarity,"
                "       card_ref "
                "FROM deck_view_crypt "
                "UNION "
                "SELECT deck_ref,"
                "       deck_name,"
                "       number_used,"
                "       card_name,"
                "       card_type,"
                "       set_name,"
                "       rarity,"
                "       card_ref "
                "FROM deck_view_library;"));

    Query (wxT ("CREATE VIEW inventory_view_library AS "
                "SELECT cards_names.card_name AS card_name,"
                "       cards_library.record_num AS card_ref,"
                "       cards_sets.set_name AS set_name,"
                "       cards_sets.record_num as set_ref,"
                "       rarity_types.rarity_type AS rarity,"
                "       inventory_library.number_owned AS number_owned,"
                "       inventory_library.number_to_trade AS number_to_trade,"
                "       inventory_library.number_wanted AS number_wanted "
                "FROM   inventory_library,"
                "       cards_library,"
                "       cards_names,"
                "       cards_sets,"
                "       rarity_types "
                "WHERE  cards_library.record_num = inventory_library.card_ref AND"
                "       cards_names.record_num = cards_library.card_name AND"
                "       cards_sets.record_num = cards_library.release_set AND"
                "       rarity_types.record_num = cards_library.rarity;"));

    Query (wxT ("CREATE VIEW inventory_view_crypt AS "
                "SELECT cards_names.card_name AS card_name,"
                "       cards_crypt.adv AS advanced,"
                "       cards_crypt.record_num AS card_ref,"
                "       cards_sets.set_name AS set_name,"
                "       cards_sets.record_num as set_ref,"
                "       rarity_types.rarity_type AS rarity,"
                "       inventory_crypt.number_owned AS number_owned,"
                "       inventory_crypt.number_to_trade AS number_to_trade,"
                "       inventory_crypt.number_wanted AS number_wanted "
                "FROM   inventory_crypt,"
                "       cards_crypt,"
                "       cards_names,"
                "       cards_sets,"
                "       rarity_types "
                "WHERE  cards_crypt.record_num = inventory_crypt.card_ref AND"
                "       cards_names.record_num = cards_crypt.card_name AND"
                "       cards_sets.record_num = cards_crypt.release_set AND"
                "       rarity_types.record_num = cards_crypt.rarity;"));

    Query (wxT ("CREATE VIEW inventory_view AS "
                "SELECT card_name,"
                "       set_name,"
                "       rarity,"
                "       number_owned,"
                "       number_to_trade,"
                "       number_wanted "
                "FROM inventory_view_library "
                "UNION "
                "SELECT card_name,"
                "       set_name,"
                "       rarity,"
                "       number_owned,"
                "       number_to_trade,"
                "       number_wanted "
                "FROM inventory_view_crypt;"));

    return 0;
}



// TODO
int
Database::DeleteCoreTables ()
{
    // TODO
    return 0;
}


int
Database::DeleteDeckTables ()
{
    Query (wxT ("DROP TABLE decks;"));
    Query (wxT ("DROP TABLE decks_crypts;"));
    Query (wxT ("DROP TABLE decks_libraries;"));
    Query (wxT ("DROP VIEW deck_view_crypt;"));
    Query (wxT ("DROP VIEW deck_view_library;"));
    Query (wxT ("DROP VIEW deck_view;"));

    return 0;
}


int
Database::DeleteInventoryTables ()
{
    Query (wxT ("DROP TABLE inventory_crypt;"));
    Query (wxT ("DROP TABLE inventory_library;"));
    Query (wxT ("DROP VIEW inventory_view_crypt;"));
    Query (wxT ("DROP VIEW inventory_view_library;"));
    Query (wxT ("DROP VIEW inventory_view;"));

    return 0;
}


Database::Database () :
    m_bReady (TRUE),
    m_bMustUpdate (FALSE),
    m_bVerbose (FALSE),
    m_oResultSet (),
    m_pCardsDB (NULL),
    m_sDatabaseDirectory (wxT ("")),
    m_sDatabaseFileName (wxT ("cards.db"))
{
#ifdef __WXMAC__
    CFStringRef fileName;
    CFBundleRef appBundle;
    CFURLRef scriptFileURL;
    FSRef fileRef, fileRef2;
    FSSpec fileSpec;
    char *path;
    wxString sPath;

    //get CF URL for script
    if (! (appBundle = CFBundleGetMainBundle())) goto fail;
    if (! (fileName = CFStringCreateWithCString(NULL, "cards.db",
                      kCFStringEncodingASCII)))
        goto fail;
    if (! (scriptFileURL = CFBundleCopyResourceURL(appBundle, fileName, NULL,
                           NULL))) goto fail;

    //Get file reference from Core Foundation URL
    if (! CFURLGetFSRef(scriptFileURL, &fileRef)) goto fail;

    //dispose of the CF variables
    CFRelease(scriptFileURL);
    CFRelease(fileName);

    //convert FSRef to FSSpec
    if (FSGetCatalogInfo(&fileRef, kFSCatInfoNone, NULL, NULL, &fileSpec,
                         NULL)) goto fail;

    //create path string
    if (! (path = (char *) malloc(1024))) goto fail;
    //    if (FSMakePath(fileSpec, path, 1024)) goto fail;

    //create file reference from file spec
    if (FSpMakeFSRef(&fileSpec, &fileRef2)) goto fail;
    // and then convert the FSRef to a path
    if (FSRefMakePath(&fileRef2, (UInt8 *) path, 1024)) goto fail;

    //    if (! DoesFileExist(path)) goto fail;

    sPath = wxString (path, wxConvISO8859_1);
    sPath = sPath.BeforeLast ('/');
    if (!wxFileName::SetCwd (sPath))
        wxLogMessage (wxT ("Couldn't open resource dir %s"), sPath.c_str ());
fail:
#endif // __WXMAC__


    char *pErrorMsg = 0;
#ifdef __WXMSW__
    wxString sAppFolder = wxT ("Anarch Revolt Deck Builder");
#else
    wxString sAppFolder = wxT (".ardb");
#endif

    // search the database in the current dir
    if (!wxFileName::FileExists (m_sDatabaseFileName)) {
        // we haven't found it, so search in the user's home_dir/app_dir
        wxFileName oUserFileName (wxGetHomeDir () << wxFileName::GetPathSeparator ());

        if (oUserFileName.IsOk ()) {
            if (oUserFileName.DirExists ()) {
                oUserFileName.AppendDir (sAppFolder);

                // if the app directory doesn't exist, create it
                if (!oUserFileName.DirExists ()) {
                    if (oUserFileName.Mkdir (0755)) {
                        wxLogMessage (wxT ("Created directory %s\n"),
                                      oUserFileName.GetPath (wxPATH_GET_VOLUME).c_str ());
                    } else {
                        wxLogError (wxT ("Failed to create directory %s\n"),
                                    oUserFileName.GetPath (wxPATH_GET_VOLUME).c_str ());
                    }
                }

                // set home_dir/app_dir as our working directory
                oUserFileName.SetCwd ();
            } else {
                wxLogMessage (wxT ("Your home directory %s doesn't exist. Weird.\n"),
                              oUserFileName.GetPath (wxPATH_GET_VOLUME).c_str ());
            } // oUserFileName.DirExists ()
        } // oUserFileName.IsOk ()
    }

    // if the DB file doesn't exist
    if (!wxFileName::FileExists (m_sDatabaseFileName)) {
        wxLogMessage (wxT ("No existing database found : creating empty database %s\n"), m_sDatabaseFileName.c_str ());

        // try to create a new DB
        if (CreateBlankDatabase () < 0) {
            m_bReady = FALSE;
        }
    }
    // an existing DB file is present
    else {
        // try to open it
        m_pCardsDB = sqlite_open (m_sDatabaseFileName.mb_str (wxConvISO8859_1), 0, &pErrorMsg);
        if (m_pCardsDB == 0) {
            wxLogError (wxT ("Can't open database %s: %s\n"), m_sDatabaseFileName.c_str (), pErrorMsg);
            m_bReady = FALSE;
        }
        m_sDatabaseDirectory = wxFileName::GetCwd ();
    }

    if (m_bReady) {
        // Add our custom functions
        sqlite_create_function(m_pCardsDB, "before", 2, beforeFunc, 0);
        sqlite_function_type(m_pCardsDB, "before", SQLITE_TEXT);
        sqlite_create_function(m_pCardsDB, "after", 2, afterFunc, 0);
        sqlite_function_type(m_pCardsDB, "after", SQLITE_TEXT);
        sqlite_create_function(m_pCardsDB, "dumbitdown", 1, dumbitdownFunc, 0);
        sqlite_function_type(m_pCardsDB, "dumbitdown", SQLITE_TEXT);
        sqlite_create_function(m_pCardsDB, "trim", 1, trimFunc, 0);
        sqlite_function_type(m_pCardsDB, "trim", SQLITE_TEXT);
    }
}


Database::~Database ()
{
    if (m_pCardsDB != NULL) {
        sqlite_close (m_pCardsDB);
        m_pCardsDB = NULL;
    }
}


void
Database::DeleteInstance ()
{
    if (spInstance != NULL) {
        delete spInstance;
        spInstance = NULL;
    }
}


Database *
Database::Instance ()
{
    if (spInstance == NULL) {
        spInstance = new Database ();

        // Check if the database looks ok
        if (!spInstance->m_bReady) {
            // Something's wrong with the DB
            delete spInstance;
            spInstance = NULL;
            wxLogError (wxT ("Ugly database error"));
        }
    }
#if !TEST
    if (spInstance) {
        if (spInstance->m_bMustUpdate == true) {
            spInstance->m_bMustUpdate = false;
            Updater *pUpdater = Updater::Instance ();
            pUpdater->DoUpdate(UPDATE_FROM_MENU);
        }
    }
#endif

    return spInstance;
}


RecordSet *
Database::Query (const wxString &sQuery, RecordSet *pDest, bool bSilent)
{
    char *pErrorMessage;
    int iResult;
    RecordSet *pResultSet = &m_oResultSet;

    if (!m_bReady) {
        if (!bSilent) {
            wxLogError (wxT ("Database error: Not ready"));
        }
        return NULL;
    }

    if (pDest != 0) {
        pResultSet = pDest;
    }
    pResultSet->Clear ();

    if (m_bVerbose) {
        printf (sQuery.mb_str (wxConvISO8859_1));
        printf ("\n");
    }
    iResult = sqlite_exec (m_pCardsDB, sQuery.mb_str (wxConvISO8859_1), Callback, pResultSet, &pErrorMessage);

    if (iResult != SQLITE_OK) {
        if (bSilent == FALSE) {
            wxLogError (wxT ("Query was: %s"), sQuery.c_str ());
            wxString sError = wxString (pErrorMessage, wxConvLocal);
            wxLogError (wxT ("SQL error: %s"), sError.c_str ());
        }
        return NULL;
    }

    // unfortunately refreshing the interface is too cpu-hungry here
    // wxSafeYield ();

    return pResultSet;
}


