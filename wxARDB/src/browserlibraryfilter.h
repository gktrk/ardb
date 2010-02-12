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


#ifndef _browserlibraryfilter_h
#define _browserlibraryfilter_h

#include <wx/wx.h>
#include <wx/listctrl.h>


#include "interfacedata.h"

class BrowserLibraryController;
WX_DEFINE_ARRAY(wxCheckBox *, wxArrayOfCheckboxes);

// This class is the dialog that will allow to filter the cards
// The magic that happens when the user clicks the OK button is
// in the LibraryBrowserController class
class BrowserLibraryFilter: public wxDialog
{
    friend class BrowserLibraryController;

public:
    BrowserLibraryFilter(BrowserLibraryController *pController);

    void SetDialogTitle (int iMode);
    void Reset ();

private:
    wxCheckBox   *m_apLibraryReqs[100];
    wxCheckBox   *m_apLibraryTexts[100];
    wxButton     *m_pCancelButton;
    wxTextCtrl   *m_pCardNameText;
    wxCheckBox   *m_pClanLess;
    wxListView   *m_pClanList;
    wxButton     *m_pClearButton;
    wxCheckBox   *m_pCombo;
    BrowserLibraryController *m_pController;
    wxCheckBox   *m_pCostBlood;
    wxCheckBox   *m_pCostPool;
    wxCheckBox   *m_pDisciplineLess;
    wxListView   *m_pDisciplineList;
    wxListView   *m_pEditionList;
    wxCheckBox   *m_pMultiDiscipline;
    wxButton     *m_pOKButton;
    wxTextCtrl   *m_pOtherText;
    wxListView   *m_pRarityList;
    wxCheckBox   *m_pReflex;
    wxListView   *m_pTitleList;
    wxArrayOfCheckboxes   m_apTypes;

    // helper functions to build the interface
    void BuildType (unsigned int uiTitleNumber, wxSizer *pContainer);
    void BuildLibraryReqs (unsigned int uiSpecialNumber, wxSizer *pContainer, wxWindow *pWindow);
    void BuildLibraryTexts (unsigned int uiSpecialNumber, wxSizer *pContainer, wxWindow *pWindow);

    // callbacks
    void OnCancelButtonClick (wxCommandEvent& WXUNUSED (event));
    void OnClanlessClick (wxCommandEvent& WXUNUSED (event));
    void OnClearButtonClick (wxCommandEvent& WXUNUSED (event));
    void OnDisclessClick (wxCommandEvent& WXUNUSED (event));
    void OnOKButtonClick (wxCommandEvent& WXUNUSED (event));

    enum {
        ID_OK_BUTTON = wxID_HIGHEST + 1,
        ID_CANCEL_BUTTON,
        ID_CLEAR_BUTTON,
        ID_CHECK_CLANLESS,
        ID_CHECK_DISCLESS
    };

    DECLARE_EVENT_TABLE()
};

#include "browserlibrarycontroller.h"


#endif
