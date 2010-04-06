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


#include "browserlibraryfilter.h"

BEGIN_EVENT_TABLE(BrowserLibraryFilter, wxDialog)
EVT_BUTTON (ID_OK_BUTTON, BrowserLibraryFilter::OnOKButtonClick)
EVT_BUTTON (ID_CANCEL_BUTTON, BrowserLibraryFilter::OnCancelButtonClick)
EVT_BUTTON (ID_CLEAR_BUTTON, BrowserLibraryFilter::OnClearButtonClick)
EVT_CHECKBOX (ID_CHECK_CLANLESS, BrowserLibraryFilter::OnClanlessClick)
EVT_CHECKBOX (ID_CHECK_DISCLESS, BrowserLibraryFilter::OnDisclessClick)
END_EVENT_TABLE()


BrowserLibraryFilter::BrowserLibraryFilter(BrowserLibraryController *pController) :
wxDialog (0, -1, wxT ("Library Filter"), wxDefaultPosition, wxDefaultSize),
// Initialisation of member objects and variables
    m_pCancelButton (NULL),
    m_pCardNameText (NULL),
    m_pClanLess (NULL),
    m_pClanList (NULL),
    m_pClearButton (NULL),
    m_pCombo (NULL),
    m_pController (pController),
    m_pCostBlood (NULL),
    m_pCostPool (NULL),
    m_pDisciplineLess (NULL),
    m_pDisciplineList (NULL),
    m_pEditionList (NULL),
    m_pMultiDiscipline (NULL),
    m_pOKButton (NULL),
    m_pOtherText (NULL),
    m_pRarityList (NULL),
    m_pReflex (NULL),
    m_pTitleList (NULL)
{
    wxAcceleratorEntry entries[3];
    entries[0].Set(wxACCEL_NORMAL, WXK_RETURN, ID_OK_BUTTON);
    entries[1].Set(wxACCEL_NORMAL, WXK_ESCAPE, ID_CANCEL_BUTTON);
    entries[2].Set(wxACCEL_NORMAL, WXK_DELETE, ID_CLEAR_BUTTON);
    wxAcceleratorTable accel(3, entries);
    this->SetAcceleratorTable(accel);

    InterfaceData *pUIData = InterfaceData::Instance ();

    wxBoxSizer *pPapaSizerV = new wxBoxSizer (wxVERTICAL);
    wxBoxSizer *pPapaSizer = new wxBoxSizer (wxHORIZONTAL);
    pPapaSizerV->Add(pPapaSizer);

    SetAutoLayout(TRUE);
    SetSizer(pPapaSizerV);

    // The card types
    wxStaticBoxSizer *pTypeBox = new wxStaticBoxSizer (new wxStaticBox (this, -1, wxT ("By card type")), wxVERTICAL);
    wxBoxSizer *pCardTypeSizer = new wxBoxSizer(wxVERTICAL);

    for (unsigned int i = 0; i < pUIData->GetTypes()->GetCount(); i++) {
        BuildType(i, pTypeBox);
    }

    m_pCombo = new wxCheckBox (this, -1, wxT ("Combo"));
    wxBoxSizer *pComboSizer = new wxBoxSizer (wxHORIZONTAL);
    wxString sComboIconName (wxT ("Type Combo"));
    wxStaticBitmap *pComboIcon = pUIData->MakeStaticBitmap (this, sComboIconName);
    if (pComboIcon) pComboSizer->Add (pComboIcon, 0, wxRIGHT, 5);
    pComboSizer->Add (m_pCombo);
    m_pReflex = new wxCheckBox (this, -1, wxT ("Reflex"));
    wxBoxSizer *pReflexSizer = new wxBoxSizer (wxHORIZONTAL);
    wxString sReflexIconName (wxT ("Type Reflex"));
    wxStaticBitmap *pReflexIcon = pUIData->MakeStaticBitmap(this, sReflexIconName);
    if (pReflexIcon) pReflexSizer->Add(pReflexIcon, 0, wxRIGHT, 5);
    pReflexSizer->Add (m_pReflex);

#ifdef __WXMSW__
    // this looks nicer in Windows
    pTypeBox->Add (pComboSizer, 0, wxALL, 3);
    pTypeBox->Add (pReflexSizer, 0, wxALL, 3);
#else
    pTypeBox->Add (pComboSizer);
    pTypeBox->Add (pReflexSizer);
#endif

    pPapaSizer->Add(pCardTypeSizer, 0, wxEXPAND | wxALL, 5);
    pCardTypeSizer->Add(pTypeBox, 0, wxEXPAND);

    // The Clans and Disciplines Sizer
    wxBoxSizer *pClanDiscSizer = new wxBoxSizer (wxVERTICAL);
    pPapaSizer->Add(pClanDiscSizer, 1, wxEXPAND | wxALL, 5);

    // The Disciplines list
    wxStaticBoxSizer *pDisciplineBox = new wxStaticBoxSizer (new wxStaticBox (this, -1, wxT ("By discipline")), wxVERTICAL);
    m_pDisciplineList = new wxListView (this, -1, wxDefaultPosition, wxSize (200, 75), wxLC_REPORT | wxLC_NO_HEADER);
    m_pDisciplineList->InsertColumn (0, wxEmptyString);
    for (unsigned int i = 0; i < pUIData->GetLibraryDisciplines ()->GetCount (); i++) {
        m_pDisciplineList->InsertItem (i, pUIData->GetLibraryDisciplines ()->Item (i)[0]);
    }
    pDisciplineBox->Add (m_pDisciplineList, 1, wxEXPAND | wxTOP, 5);
    m_pMultiDiscipline = new wxCheckBox (this, -1, wxT ("Multi Discipline"));
    m_pDisciplineLess = new wxCheckBox (this, ID_CHECK_DISCLESS, wxT ("Disciplineless"));
#ifdef __WXMSW__
    // this looks nicer in Windows
    pDisciplineBox->Add (m_pMultiDiscipline, 0, wxALL, 3);
    pDisciplineBox->Add (m_pDisciplineLess, 0, wxALL, 3);
#else
    pDisciplineBox->Add (m_pMultiDiscipline);
    pDisciplineBox->Add (m_pDisciplineLess);
#endif
    pClanDiscSizer->Add (pDisciplineBox, 1, wxEXPAND);

    // the Titles and Other sizers
    wxBoxSizer *pTitleOtherSizer = new wxBoxSizer (wxVERTICAL);
    pPapaSizer->Add (pTitleOtherSizer, 1, wxEXPAND | wxALL, 5);

    // Titles
    wxStaticBoxSizer *pTitleBox = new wxStaticBoxSizer (new wxStaticBox (this, -1, wxT ("By title")), wxVERTICAL);
    m_pTitleList = new wxListView (this, -1, wxDefaultPosition, wxSize (200, 75), wxLC_REPORT | wxLC_NO_HEADER);
    m_pTitleList->InsertColumn (0, wxEmptyString);
    // here we won't include any of the wildcard titles

    for (unsigned int i = 0; i < pUIData->GetTitles ()->GetCount () - 2; i++) {
        m_pTitleList->InsertItem (i, pUIData->GetTitles ()->Item (i)[0]);
    }

    pTitleBox->Add (m_pTitleList, 1, wxEXPAND | wxTOP, 5);
    pTitleOtherSizer->Add (pTitleBox, 0, wxEXPAND);

    // Clans
    wxStaticBoxSizer *pClanBox = new wxStaticBoxSizer (new wxStaticBox (this, -1, wxT ("By clan")), wxVERTICAL);
    m_pClanList = new wxListView (this, -1, wxDefaultPosition, wxSize (200, 75), wxLC_REPORT | wxLC_NO_HEADER);
    m_pClanList->InsertColumn (0, wxEmptyString);
    for (unsigned int i = 0; i < pUIData->GetClans ()->GetCount (); i++) {
        m_pClanList->InsertItem (i, pUIData->GetClans ()->Item (i));
    }
    pClanBox->Add (m_pClanList, 1, wxEXPAND | wxTOP, 5);
    wxGridSizer *pAnarchEtcSizer = new wxGridSizer (2);
    pClanBox->Add (pAnarchEtcSizer);
    m_pClanLess = new wxCheckBox (this, ID_CHECK_CLANLESS, wxT ("Clanless"));
#ifdef __WXMSW__
    // this looks nicer in Windows
    pAnarchEtcSizer->Add (m_pClanLess, 0, wxALL, 3);
#else
    pAnarchEtcSizer->Add (m_pClanLess);
#endif
    pClanDiscSizer->Add (pClanBox, 1, wxEXPAND);

    // Editions
    wxStaticBoxSizer *pEditionBox = new wxStaticBoxSizer (new wxStaticBox (this, -1, wxT ("By set")), wxHORIZONTAL);
    wxBoxSizer *pEditionSizer = new wxBoxSizer (wxVERTICAL);
    pEditionBox->Add (pEditionSizer, 1, wxEXPAND);
    m_pEditionList = new wxListView (this, -1, wxDefaultPosition, wxSize (200, 75), wxLC_REPORT | wxLC_NO_HEADER);
    m_pEditionList->InsertColumn (0, wxEmptyString);
    for (unsigned int i = 0; i < pUIData->GetEditions ()->GetCount (); i++) {
        m_pEditionList->InsertItem (i, pUIData->GetEditions ()->Item (i));
    }
    pEditionSizer->Add (m_pEditionList, 1, wxEXPAND | wxTOP, 5);
    pTitleOtherSizer->Add (pEditionBox, 1, wxEXPAND);


    // Rarity
    wxStaticBoxSizer *pRarityBox = new wxStaticBoxSizer (new wxStaticBox (this, -1, wxT ("By rarity")), wxHORIZONTAL);
    wxBoxSizer *pRaritySizer = new wxBoxSizer (wxVERTICAL);
    pRarityBox->Add (pRaritySizer, 1, wxEXPAND);
    m_pRarityList = new wxListView (this, -1, wxDefaultPosition, wxSize (200, 75), wxLC_REPORT | wxLC_NO_HEADER);
    m_pRarityList->InsertColumn (0, wxEmptyString);
    for (unsigned int i = 0; i < pUIData->GetRarities ()->GetCount (); i++) {
        m_pRarityList->InsertItem (i, pUIData->GetRarities ()->Item (i)[0]);
    }
    pRaritySizer->Add (m_pRarityList, 1, wxEXPAND | wxTOP, 5);
    pTitleOtherSizer->Add (pRarityBox, 1, wxEXPAND);


    // Card text
    wxStaticBoxSizer *pTextBox = new wxStaticBoxSizer (new wxStaticBox (this, -1, wxT ("By card text")), wxVERTICAL);
    wxFlexGridSizer *pTextSizer = new wxFlexGridSizer (2, 5, 5);

    wxStaticText *pLabel = new wxStaticText (this, -1, wxT ("Card name :"));
    pTextSizer->Add (pLabel, 0, wxALIGN_CENTER_VERTICAL);
    m_pCardNameText = new wxTextCtrl (this, -1, wxT (""), wxDefaultPosition, wxSize (150, -1));
    pTextSizer->Add (m_pCardNameText, 1, wxEXPAND);

    wxStaticText *pLabel2 = new wxStaticText (this, -1, wxT ("Other text :"));
    pTextSizer->Add (pLabel2, 0, wxALIGN_CENTER_VERTICAL);
    m_pOtherText = new wxTextCtrl (this, -1, wxT (""), wxDefaultPosition, wxSize (150, -1));
    pTextSizer->Add (m_pOtherText, 1, wxEXPAND);

    pTextBox->Add (pTextSizer, 1, wxEXPAND | wxTOP, 5);
    pTitleOtherSizer->Add (pTextBox, 0, wxEXPAND);

    // Other (cost blood, pool, burn option, etc..)
    wxStaticBoxSizer *pOtherBox = new wxStaticBoxSizer (new wxStaticBox (this, -1, wxT ("Other")), wxVERTICAL);
    wxFlexGridSizer *pOtherSizer = new wxFlexGridSizer (2);
    pOtherBox->Add (pOtherSizer, 1, wxEXPAND | wxTOP, 5);
    for (unsigned int i = 0; i < pUIData->GetLibraryReqs ()->GetCount (); i++) {
        BuildLibraryReqs (i, pOtherSizer, this);
    }

    for (unsigned int i = 0; i < pUIData->GetLibraryTexts ()->GetCount (); i++) {
        BuildLibraryTexts (i, pOtherSizer, this);
    }

    m_pCostBlood = new wxCheckBox (this, -1, wxT ("Cost Blood"));
    m_pCostPool = new wxCheckBox (this, -1, wxT ("Cost Pool"));

    pOtherSizer->Add(m_pCostBlood, 0, wxALL, 3);
    pOtherSizer->Add(m_pCostPool, 0, wxALL, 3);

    pClanDiscSizer->Add(pOtherBox, 0, wxEXPAND);


    // OK, Clear and Cancel buttons
    wxBoxSizer *pOKCancelSizer = new wxBoxSizer (wxHORIZONTAL);
    m_pOKButton = new wxButton (this, ID_OK_BUTTON, wxT ("OK"));
    pOKCancelSizer->Add (m_pOKButton, 0, wxRIGHT | wxALIGN_BOTTOM, 5);
    m_pClearButton = new wxButton (this, ID_CLEAR_BUTTON, wxT ("Clear All"));
    pOKCancelSizer->Add (m_pClearButton, 0, wxRIGHT | wxALIGN_BOTTOM, 5);
    m_pCancelButton = new wxButton (this, ID_CANCEL_BUTTON, wxT ("Cancel"));
    pOKCancelSizer->Add (m_pCancelButton, 0, wxRIGHT | wxALIGN_BOTTOM, 5);
    pPapaSizerV->Add (pOKCancelSizer, 1, wxALIGN_RIGHT | wxALL, 5);

    pPapaSizerV->Layout ();
    pPapaSizerV->Fit (this);

    m_pClanList->SetColumnWidth (0, m_pClanList->GetClientSize ().GetWidth () - 20);
    m_pDisciplineList->SetColumnWidth (0, m_pDisciplineList->GetClientSize ().GetWidth () - 20);
    m_pTitleList->SetColumnWidth (0, m_pTitleList->GetClientSize ().GetWidth () - 20);
    m_pEditionList->SetColumnWidth (0, m_pEditionList->GetClientSize ().GetWidth () - 20);
    m_pRarityList->SetColumnWidth (0, m_pRarityList->GetClientSize ().GetWidth () - 20);

    SetIcon (*g_pIcon);
}


void
BrowserLibraryFilter::BuildLibraryReqs (unsigned int uiSpecialNumber, wxSizer *pContainer, wxWindow *pWindow)
{
    InterfaceData *pUIData = InterfaceData::Instance ();
    wxCheckBox *pCheckBox;

    pCheckBox = new wxCheckBox (pWindow, -1, pUIData->GetLibraryReqs ()->Item (uiSpecialNumber)[0]);
#ifdef __WXMSW__
    // this looks nicer in Windows
    pContainer->Add (pCheckBox, 0, wxALL, 3);
#else
    pContainer->Add (pCheckBox);
#endif
    m_apLibraryReqs[uiSpecialNumber] = pCheckBox;
}


void
BrowserLibraryFilter::BuildLibraryTexts (unsigned int uiSpecialNumber, wxSizer *pContainer, wxWindow *pWindow)
{
    InterfaceData *pUIData = InterfaceData::Instance ();
    wxCheckBox *pCheckBox;

    pCheckBox = new wxCheckBox (pWindow, -1, pUIData->GetLibraryTexts ()->Item (uiSpecialNumber)[0]);
#ifdef __WXMSW__
    // this looks nicer in Windows
    pContainer->Add (pCheckBox, 0, wxALL, 3);
#else
    pContainer->Add (pCheckBox);
#endif
    m_apLibraryTexts[uiSpecialNumber] = pCheckBox;
}


void
BrowserLibraryFilter::BuildType (unsigned int uiTypeNumber, wxSizer *pContainer)
{
    wxBoxSizer *pSizer;
    wxCheckBox *pCheckBox;
    InterfaceData *pUIData = InterfaceData::Instance ();

    pSizer = new wxBoxSizer(wxHORIZONTAL);
    pCheckBox = new wxCheckBox(this, -1, pUIData->GetTypes()->Item(uiTypeNumber));

    wxString sIconName;
    sIconName.Printf (wxT ("Type %s"),
                      pUIData->GetTypes ()->Item (uiTypeNumber).c_str ());
    wxStaticBitmap *pIcon = pUIData->MakeStaticBitmap(this, sIconName);
    if (pIcon) pSizer->Add (pIcon, 0, wxRIGHT, 5);

    pSizer->Add (pCheckBox);

#ifdef __WXMSW__
    // this looks nicer in Windows
    pContainer->Add (pSizer, 0, wxALL, 3);
#else
    pContainer->Add (pSizer);
#endif
    m_apTypes.Insert (pCheckBox, uiTypeNumber);
}


void
BrowserLibraryFilter::OnCancelButtonClick (wxCommandEvent& WXUNUSED (event))
{
    EndModal (0);
}


void
BrowserLibraryFilter::OnClanlessClick (wxCommandEvent& WXUNUSED (event))
{
    m_pClanList->Enable (!m_pClanLess->IsChecked ());
}


void
BrowserLibraryFilter::OnClearButtonClick (wxCommandEvent& WXUNUSED (event))
{
    Reset ();
}


void
BrowserLibraryFilter::OnDisclessClick (wxCommandEvent& WXUNUSED (event))
{
    m_pDisciplineList->Enable (!m_pDisciplineLess->IsChecked ());
    m_pMultiDiscipline->Enable (!m_pDisciplineLess->IsChecked ());
}


void
BrowserLibraryFilter::OnOKButtonClick (wxCommandEvent& WXUNUSED (event))
{
    m_pController->UpdateModel (this);
    EndModal (1);
}


void
BrowserLibraryFilter::Reset ()
{
    m_pOtherText->Clear ();
    m_pCardNameText->Clear ();
    for (unsigned int i = 0; i < m_apTypes.GetCount (); i++) {
        m_apTypes[i]->SetValue (FALSE);
    }
    for (int i = 0; i < m_pTitleList->GetItemCount (); i++) {
        m_pTitleList->Select (i, FALSE);
    }
    for (int i = 0; i < m_pEditionList->GetItemCount (); i++) {
        m_pEditionList->Select (i, FALSE);
    }
    for (int i = 0; i < m_pRarityList->GetItemCount (); i++) {
        m_pRarityList->Select (i, FALSE);
    }
    for (int i = 0; i < m_pClanList->GetItemCount (); i++) {
        m_pClanList->Select (i, FALSE);
    }
    for (int i = 0; i < m_pDisciplineList->GetItemCount (); i++) {
        m_pDisciplineList->Select (i, FALSE);
    }
    /*
      m_pAnarchCheckbox->SetValue (FALSE);
      m_pBlackHandCheckbox->SetValue (FALSE);
      m_pSeraphCheckbox->SetValue (FALSE);
      m_pBurnOption->SetValue (FALSE);
      m_pCostBlood->SetValue (FALSE);
      m_pCostPool->SetValue (FALSE);
      m_pTrifle->SetValue (FALSE);
      m_pTrophy->SetValue (FALSE);
    */
    m_pClanLess->SetValue (FALSE);
    m_pDisciplineLess->SetValue (FALSE);
    m_pMultiDiscipline->SetValue (FALSE);
    m_pCombo->SetValue (FALSE);
    m_pReflex->SetValue (FALSE);
}


void
BrowserLibraryFilter::SetDialogTitle (int iMode)
{
    // Set the dialog's title
    switch (iMode) {
        case LIBRARY_FILTER_ADD:
            SetTitle (wxT ("Add cards..."));
            break;
        case LIBRARY_FILTER_REMOVE:
            SetTitle (wxT ("Remove cards..."));
            break;
        case LIBRARY_FILTER_KEEP:
            SetTitle (wxT ("Select cards..."));
            break;
    }
}


