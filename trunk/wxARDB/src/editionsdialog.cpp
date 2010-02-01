/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2002 Francois Gombault
 *  gombault.francois@wanadoo.fr
 *
 *  contributors:
 *    meshee.knight@gmail.com
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

#include "editionsdialog.h"

EditionsDialog *EditionsDialog::spInstance = NULL;

BEGIN_EVENT_TABLE(EditionsDialog, wxDialog)
    EVT_BUTTON (wxID_OK, EditionsDialog::OnOK )
    EVT_BUTTON (wxID_CANCEL, EditionsDialog::OnCancel )
    EVT_BUTTON (ID_USE_BUTTON, EditionsDialog::OnUseButtonClick)
    EVT_BUTTON (ID_IGNORE_BUTTON, EditionsDialog::OnIgnoreButtonClick)
END_EVENT_TABLE()


EditionsDialog::EditionsDialog () :
    wxDialog (0, -1, wxT ("Anarch Revolt - Choose Displayed Card Sets"), wxDefaultPosition, wxSize (350, 250)),
    m_oIgnoreEditionsArray (),
    m_oUseEditionsArray (),
    m_pIgnoreEditionsList (NULL),
    m_pUseEditionsList (NULL)
{
    // Get the editions in uses
    m_oUseEditionsArray = ardb_db_ef_get_inuse_editions();

    // Get the editions ignored
    m_oIgnoreEditionsArray = ardb_db_ef_get_ignored_editions();

    wxBoxSizer *pPapaSizer = new wxBoxSizer (wxVERTICAL);
    wxBoxSizer *pHorizontalSizer = new wxBoxSizer (wxHORIZONTAL);

    SetAutoLayout (TRUE);
    SetSizer (pPapaSizer);

    // Editions in use
    wxStaticBox *pUseEditionsStaticBox = new wxStaticBox (this, -1, wxT ("Displayed Sets"));
    wxStaticBoxSizer *pUseEditionsBox = new wxStaticBoxSizer (pUseEditionsStaticBox, wxVERTICAL);
    m_pUseEditionsList = new wxListView (this, -1, wxDefaultPosition, wxSize (200, 275), wxLC_REPORT | wxLC_NO_HEADER);
    m_pUseEditionsList->InsertColumn (0, wxEmptyString);
    for (unsigned int i = 0; i < m_oUseEditionsArray.GetCount (); i++) {
        m_pUseEditionsList->InsertItem (i, m_oUseEditionsArray[i]);
    }
    pUseEditionsBox->Add (m_pUseEditionsList, 1, wxEXPAND | wxTOP, 5);
    pHorizontalSizer->Add (pUseEditionsBox, 0, wxEXPAND);

    // Buttons
    wxButton *pUseButton = new wxButton (this, ID_USE_BUTTON,
                                         wxT ("<-- Use"));
    wxButton *pIgnoreButton = new wxButton (this, ID_IGNORE_BUTTON,
                                            wxT ("Ignore -->"));
    wxBoxSizer *pCentralSizer = new wxBoxSizer (wxVERTICAL);
    pCentralSizer->Add (pIgnoreButton, 0, wxALIGN_CENTER | wxALL, 15);
    pCentralSizer->Add (pUseButton, 0, wxALIGN_CENTER | wxALL, 15);
    pHorizontalSizer->Add (pCentralSizer, 0, wxEXPAND);

    // Editions not in use
    wxStaticBox *pIgnoreEditionsStaticBox = new wxStaticBox (this, -1, wxT ("Ignored Sets"));
    wxStaticBoxSizer *pIgnoreEditionsBox = new wxStaticBoxSizer (pIgnoreEditionsStaticBox, wxVERTICAL);
    m_pIgnoreEditionsList = new wxListView (this, -1, wxDefaultPosition, wxSize (200, 275), wxLC_REPORT | wxLC_NO_HEADER);
    m_pIgnoreEditionsList->InsertColumn (0, wxEmptyString);
    for (unsigned int i = 0; i < m_oIgnoreEditionsArray.GetCount (); i++) {
        m_pIgnoreEditionsList->InsertItem (i, m_oIgnoreEditionsArray[i]);
    }
    pIgnoreEditionsBox->Add (m_pIgnoreEditionsList, 1, wxEXPAND | wxTOP, 5);
    pHorizontalSizer->Add (pIgnoreEditionsBox, 0, wxEXPAND);

    pPapaSizer->Add (pHorizontalSizer, 1, wxEXPAND);

    wxBoxSizer *pOKCancelSizer = new wxBoxSizer (wxHORIZONTAL);

    wxButton *pCancelButton = new wxButton (this, wxID_CANCEL, wxT ("Cancel"));
    pOKCancelSizer->Add (pCancelButton, 0, wxRIGHT, 10);
    wxButton *pOKButton = new wxButton (this, wxID_OK, wxT ("OK"));
    pOKCancelSizer->Add (pOKButton, 0, wxLEFT, 10);

    pPapaSizer->Add (pOKCancelSizer, 0, wxALIGN_CENTER_HORIZONTAL);

    pPapaSizer->Layout ();
    pPapaSizer->Fit (this);

    m_pIgnoreEditionsList->SetColumnWidth (0, m_pIgnoreEditionsList->GetClientSize ().GetWidth () - 20);
    m_pUseEditionsList->SetColumnWidth (0, m_pUseEditionsList->GetClientSize ().GetWidth () - 20);


    Show ();
}


EditionsDialog::~EditionsDialog ()
{

}


void
EditionsDialog::DeleteInstance ()
{
    if (spInstance != NULL) {
        delete spInstance;
        spInstance = NULL;
    }
}


EditionsDialog *
EditionsDialog::Instance ()
{
    if (spInstance == NULL) {
        spInstance = new EditionsDialog ();
    }
    return spInstance;
}


void
EditionsDialog::OnCancel(wxCommandEvent& WXUNUSED (event))
{
    Hide ();
    DeleteInstance ();
}

void
EditionsDialog::OnIgnoreButtonClick (wxCommandEvent& WXUNUSED (event))
{
    for (int i = 0; i < m_pUseEditionsList->GetItemCount (); i++) {
        if (m_pUseEditionsList->IsSelected (i)) {
            m_pIgnoreEditionsList->InsertItem (m_pIgnoreEditionsList->GetItemCount (),
                                               m_pUseEditionsList->GetItemText (i));
            m_pUseEditionsList->DeleteItem (i);
            i--;
        }
    }
}


void
EditionsDialog::OnOK(wxCommandEvent& WXUNUSED (event))
{

    Hide ();
    //make wxListView into wxArrayString
    wxArrayString useEditions, ignoreEditions;
    for (int i = 0; i < m_pIgnoreEditionsList->GetItemCount(); i++) {
        ignoreEditions.Add(m_pIgnoreEditionsList->GetItemText(i));
    }

    for (int i = 0; i < m_pUseEditionsList->GetItemCount(); i++) {
        useEditions.Add(m_pUseEditionsList->GetItemText(i));
    }

    ardb_db_ef_move(useEditions, ignoreEditions);
    DeleteInstance ();
}


void
EditionsDialog::OnUseButtonClick (wxCommandEvent& WXUNUSED (event))
{
    for (int i = 0; i < m_pIgnoreEditionsList->GetItemCount (); i++) {
        if (m_pIgnoreEditionsList->IsSelected (i)) {
            m_pUseEditionsList->InsertItem (m_pUseEditionsList->GetItemCount (),
                                            m_pIgnoreEditionsList->GetItemText (i));
            m_pIgnoreEditionsList->DeleteItem (i);
            i--;
        }
    }
}

