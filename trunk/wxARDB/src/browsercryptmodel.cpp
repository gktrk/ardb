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


#include "browsercryptmodel.h"


BrowserCryptModel::BrowserCryptModel (wxNotebook *pViewPanel, unsigned int uiNumber) :
    m_oQueryList (),
    m_oSummaryList (),
    m_oCardList (),
    m_oVisibilityList (),
    m_pController (NULL),
    m_pView (NULL),
    m_pViewPanel (pViewPanel),
    m_uiModelIDNumber (uiNumber),
    m_uiCardCount (0)
{
    m_sViewQuery.Printf (wxT ("SELECT DISTINCT "
                              "       sum(have) AS hav, "
                              "       sum(want) AS wan, "
                              "       sum(spare) AS spa, "
                              "       card_name, "
                              "       advanced, "
                              "       capacity, "
                              "       disciplines, "
                              "       card_type, "
                              "       title, "
                              "       groupnumber, "
                              "       card_text, "
                              "       min(card_ref), "
                              "       name_ref, "
                              "       dumbitdown(card_name) "
                              "FROM crypt_view "
                              "WHERE card_ref IN (SELECT card_name "
                              "                   FROM crypt_selection WHERE browser_num = %d) "
                              "GROUP BY name_ref, advanced "),
                         m_uiModelIDNumber);

    m_sAddQuery.Printf (wxT ("INSERT INTO crypt_selection SELECT %d, card_ref, NULL FROM crypt_view %s"),
                        m_uiModelIDNumber, wxT("%s"));

    m_sRemoveQuery.Printf (wxT ("DELETE FROM crypt_selection WHERE browser_num = %d AND card_name IN (SELECT card_ref FROM crypt_view %s)"), m_uiModelIDNumber,  wxT("%s"));

    m_sKeepQuery.Printf (wxT ("DELETE FROM crypt_selection WHERE browser_num = %d AND card_name NOT IN (SELECT card_ref FROM crypt_view %s)"), m_uiModelIDNumber, wxT("%s"));

    m_sWipeQuery.Printf (wxT ("DELETE FROM crypt_selection WHERE browser_num = %d; "),
                         m_uiModelIDNumber);

    SetSortColumn (4);
    Reset();

    m_pController = new BrowserCryptController (this);
    m_pView = new BrowserCryptTab (this, m_pController, m_pViewPanel, uiNumber);


}


BrowserCryptModel::~BrowserCryptModel ()
{
    if (m_pController) delete m_pController;
}


void
BrowserCryptModel::QueryCardData (const wxString &sQuery, const wxString &sSummary, bool bUpdateView)
{
    Database *pDatabase = Database::Instance ();
    RecordSet *pResult;
    wxString sVQuery = m_sViewQuery + m_sSortSuffix;

    if (pDatabase != NULL) {
        pResult = pDatabase->Query (sQuery.c_str (), &m_oCardList);
        pDatabase->Query (sVQuery.c_str (), &m_oCardList);

        if (!sSummary.IsEmpty () && pResult != NULL) {
            m_oQueryList.Add (sQuery);
            m_oSummaryList.Add (sSummary);
            m_oVisibilityList.Add (1);
        }
    }
    m_uiCardCount = m_oCardList.GetCount ();

    if (m_pView && bUpdateView) {
        m_pView->UpdateView ();
    }
}


unsigned int
BrowserCryptModel::GetFilterCount ()
{
    return m_oQueryList.GetCount ();
}


wxString &
BrowserCryptModel::GetFilterSummary (unsigned int uiFilterNumber)
{
    return m_oSummaryList.Item (uiFilterNumber);
}


unsigned int
BrowserCryptModel::GetCardCount ()
{
    return m_uiCardCount;
}


RecordSet *
BrowserCryptModel::GetCardList ()
{
    return &m_oCardList;
}


void
BrowserCryptModel::Reset ()
{
    Database *pDatabase = Database::Instance ();
    wxString sQuery;

    m_oQueryList.Clear ();
    m_oSummaryList.Clear ();
    m_oVisibilityList.Clear ();

    // Remove the old records
    pDatabase->Query (m_sWipeQuery, &m_oCardList);

    // Fill the crypt_selection table
    sQuery.Printf (m_sAddQuery, wxT (""));
    QueryCardData (sQuery, wxT (""), TRUE);
}


void
BrowserCryptModel::DeleteFilter (unsigned int uiFilterNumber, bool bUpdateView)
{
    if (uiFilterNumber < m_oQueryList.GetCount ()) {
        m_oQueryList.RemoveAt (uiFilterNumber);
        m_oSummaryList.RemoveAt (uiFilterNumber);
        m_oVisibilityList.RemoveAt (uiFilterNumber);
    }
    ExecFilterList (bUpdateView);
}


void
BrowserCryptModel::ExecFilterList (bool bUpdateView)
{
    Database *pDatabase = Database::Instance ();
    wxString sQuery;
    wxString sVQuery = m_sViewQuery + m_sSortSuffix;

    if (pDatabase != NULL) {
        // Reset the crypt_selection table
        pDatabase->Query (m_sWipeQuery, &m_oCardList);
        sQuery.Printf (m_sAddQuery, wxT (""));
        QueryCardData (sQuery, wxT (""), FALSE);

        // And apply the remaining visible filters
        for (unsigned int i = 0; i < m_oQueryList.GetCount (); i++) {
            if (m_oVisibilityList.Item (i) > 0) {
                pDatabase->Query (m_oQueryList.Item (i).c_str (), &m_oCardList, FALSE);
            }
        }
        pDatabase->Query (sVQuery.c_str (), &m_oCardList, bUpdateView);
    }
    m_uiCardCount = m_oCardList.GetCount ();
}


bool
BrowserCryptModel::IsFilterVisible (unsigned int uiFilterNumber)
{
    return m_oVisibilityList.Item (uiFilterNumber) > 0;
}


void
BrowserCryptModel::LowerFilter (unsigned int uiFilterNumber, bool bUpdateView)
{
    if ((uiFilterNumber < m_oQueryList.GetCount ()) &&
            (uiFilterNumber > 0)) {
        // Copy the filter
        wxString sQuery = m_oQueryList.Item (uiFilterNumber);
        wxString sSummary = m_oSummaryList.Item (uiFilterNumber);
        long lVisible = m_oVisibilityList.Item (uiFilterNumber);

        // Remove the filter from its old position
        m_oQueryList.RemoveAt (uiFilterNumber);
        m_oSummaryList.RemoveAt (uiFilterNumber);
        m_oVisibilityList.RemoveAt (uiFilterNumber);

        // Insert the filter at its new position
        m_oQueryList.Insert (sQuery, uiFilterNumber - 1);
        m_oSummaryList.Insert (sSummary, uiFilterNumber - 1);
        m_oVisibilityList.Insert (lVisible, uiFilterNumber - 1);
    }

    ExecFilterList (bUpdateView);
}


void
BrowserCryptModel::RaiseFilter (unsigned int uiFilterNumber, bool bUpdateView)
{
    // Let's not duplicate code
    LowerFilter (uiFilterNumber + 1, bUpdateView);
}


void
BrowserCryptModel::Undo (bool bUpdateView)
{
    DeleteFilter (m_oQueryList.GetCount () - 1, bUpdateView);
}


void
BrowserCryptModel::SetFilterVisible (unsigned int uiFilterNumber, bool bVisible, bool bUpdateView)
{
    m_oVisibilityList.Item (uiFilterNumber) = (int) bVisible;
    ExecFilterList (bUpdateView);
}


// How to sort the results
// Parameter is a column number (first column is 1)
// Negative means reverse sort order
void
BrowserCryptModel::SetSortColumn (int iColumn)
{
    m_sSortSuffix = wxT (" ORDER BY ");
    switch (iColumn) {
    case 1:
    case -1:
        m_sSortSuffix << wxT ("hav");
        break;
    case 2:
    case -2:
        m_sSortSuffix << wxT ("wan");
        break;
    case 3:
    case -3:
        m_sSortSuffix << wxT ("spa");
        break;
    case 4:
    case -4:
        m_sSortSuffix << wxT ("dumbitdown(card_name)");
        break;
    case 5:
    case -5:
        m_sSortSuffix << wxT ("advanced");
        break;
    case 6:
    case -6:
        m_sSortSuffix << wxT ("capacity");
        break;
    case 7:
    case -7:
        m_sSortSuffix << wxT ("disciplines");
        break;
    case 8:
    case -8:
        m_sSortSuffix << wxT ("card_type");
        break;
    case 9:
    case -9:
        m_sSortSuffix << wxT ("title");
        break;
    case 10:
    case -10:
        m_sSortSuffix << wxT ("groupnumber");
        break;
    case 11:
    case -11:
        m_sSortSuffix << wxT ("card_text");
        break;
    default:
        m_sSortSuffix << wxT ("dumbitdown(card_name)");
        break;
    }

    if (iColumn >= 0)
        m_sSortSuffix << wxT (" ASC ");
    else
        m_sSortSuffix << wxT (" DESC ");
}
