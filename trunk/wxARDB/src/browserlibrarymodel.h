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


#ifndef _browserlibrarymodel_h
#define _browserlibrarymodel_h

#include <wx/notebook.h>
#include "database.h"
#include "types.h"

class BrowserLibraryController;
class BrowserLibraryTab;


class BrowserLibraryModel
{
public:
    // The queries
    wxString m_sAddQuery;
    wxString m_sKeepQuery;
    wxString m_sRemoveQuery;
    wxString m_sSortSuffix;
    wxString m_sViewQuery;
    wxString m_sWipeQuery;

    // Constructor and destructor
    BrowserLibraryModel (wxNotebook *pViewPanel, unsigned int uiNumber);
    ~BrowserLibraryModel ();

    // Get card data based on a certain query
    void QueryCardData (const wxString &sQuery, const wxString &sSummary, bool bUpdateView = TRUE);

    // Accessors
    unsigned int GetCardCount ();
    RecordSet *GetCardList ();
    unsigned int GetIDNumber () {
        return m_uiModelIDNumber;
    }
    unsigned int GetFilterCount ();
    wxString & GetFilterSummary (unsigned int uiFilterNumber);
    bool IsFilterVisible (unsigned int uiFilterNumber);

    void Reset ();

    // Undo stuff
    void DeleteFilter (unsigned int uiFilterNumber, bool bUpdateView = TRUE);
    void ExecFilterList (bool bUpdateView = TRUE);
    void LowerFilter (unsigned int uiFilterNumber, bool bUpdateView = TRUE);
    void RaiseFilter (unsigned int uiFilterNumber, bool bUpdateView = TRUE);
    void Undo (bool bUpdateView = TRUE);
    void SetFilterVisible (unsigned int uiFilterNumber, bool bVisible, bool bUpdateView = TRUE);
    void SetSortColumn (int iColumn);

private:
    // Card list
    RecordSet                 m_oCardList;

    wxArrayString             m_oQueryList;
    wxArrayString             m_oSummaryList;
    wxArrayLong               m_oVisibilityList;

    BrowserLibraryController *m_pController;
    BrowserLibraryTab        *m_pView;

    wxNotebook               *m_pViewPanel;

    unsigned int              m_uiCardCount;
    unsigned int              m_uiModelIDNumber;


};

#include "browserlibrarycontroller.h"
#include "browserlibrarytab.h"


#endif
