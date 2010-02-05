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

#include "amountdialog.h"
#include "deckcrypttab.h"
#include "deckmodel.h"
#include "speling.h"

BEGIN_EVENT_TABLE (DeckCryptTab, wxPanel)
    EVT_BUTTON (ID_ADD_BUTTON, DeckCryptTab::OnAddButtonClick)
    EVT_TEXT (ID_CARD_INPUT, DeckCryptTab::OnCardInputChanged)
    EVT_TEXT_ENTER (ID_CARD_INPUT, DeckCryptTab::OnCardInputEnter)
    EVT_LISTBOX (ID_CARD_PICKER, DeckCryptTab::OnCardPickerSelection)
    EVT_LISTBOX_DCLICK (ID_CARD_PICKER, DeckCryptTab::OnCardPickerDoubleClick)
    EVT_LIST_ITEM_RIGHT_CLICK (ID_VAMPIRE_LIST, DeckCryptTab::OnVampireRightClick)
    EVT_LIST_COL_CLICK (ID_VAMPIRE_LIST, DeckCryptTab::OnColumnClick)
    EVT_LIST_ITEM_SELECTED(ID_VAMPIRE_LIST, DeckCryptTab::OnVampireSelect)
    EVT_LIST_ITEM_ACTIVATED(ID_VAMPIRE_LIST, DeckCryptTab::OnVampireActivate)
    EVT_LIST_KEY_DOWN(ID_VAMPIRE_LIST, DeckCryptTab::OnVampireListKeyDown)
    EVT_MENU (ID_POPUP_MORE, DeckCryptTab::OnPopupMore)
    EVT_MENU (ID_POPUP_LESS, DeckCryptTab::OnPopupLess)
    EVT_MENU (ID_POPUP_REMOVE_ALL, DeckCryptTab::OnPopupRemoveAll)
    EVT_SPINCTRL (ID_HAPPY_DISC, DeckCryptTab::OnHappyDiscChange)
    EVT_SPINCTRL (ID_HAPPY_MASTER, DeckCryptTab::OnHappyMasterChange)
    EVT_SPINCTRL (ID_HAPPY_SIZE, DeckCryptTab::OnHappySizeChange)
END_EVENT_TABLE ()

const wxString DeckCryptTab::s_sCardNamesQuery =
    wxT ("SELECT DISTINCT card_name, advanced "
         "FROM crypt_view "
         "ORDER BY dumbitdown(card_name) ASC, advanced");

DeckCryptTab::DeckCryptTab (DeckModel *pModel, wxNotebook *pParent) :
    wxPanel (pParent, -1),
    m_bReverseSortOrder (TRUE),
    m_bNoEvents (FALSE),
    m_iCycleCounter (-1),
    m_iCycleLowerValue (-1),
    m_iCycleUpperValue (-1),
    m_lSelectedVampire (-1),
    m_oArrayOfNames (),
    m_oCardList (),
    m_oNameList (),
    m_oPopupMenu (),
    m_pAddButton (NULL),
    m_pCardInput (NULL),
    m_pCardPicker (NULL),
    m_pCardText (NULL),
    m_pHappyFamiliesText (NULL),
    m_pModel (pModel),
    m_pParent (pParent),
    m_pSetPicker (NULL),
    m_pVampireCountLabel (NULL),
    m_pVampireList (NULL),
    m_uiColumnCount (10),
    m_uiSortColumn (0)
{
    //  m_pController->SetCryptView (this);
    m_pParent->AddPage (this, wxT ("Crypt"), FALSE, -1);

    wxBoxSizer *pPapaSizer, *pPickerSizer, *pFindSizer, *pTopSizer, *pSubPickerSizer, *pHappySizer, *pCardTextSizer;
    wxFlexGridSizer *pHappyGridSizer;
    wxNotebook *pNotebook;
    wxPanel *pHappyPanel, *pCardTextPanel;
    wxStaticText *pFindLabel;

    pPapaSizer = new wxBoxSizer (wxVERTICAL);
    pPickerSizer = new wxBoxSizer (wxVERTICAL);
    pHappySizer = new wxBoxSizer (wxVERTICAL);
    pSubPickerSizer = new wxBoxSizer (wxHORIZONTAL);
    pFindSizer = new wxBoxSizer (wxHORIZONTAL);
    pTopSizer = new wxBoxSizer (wxHORIZONTAL);

    // create the happy / cardtext notebook
    pNotebook = new wxNotebook (this , -1);

    // create the happy panel
    pHappyPanel = new wxPanel (pNotebook, -1);
    pHappyPanel->SetSizer (pHappySizer);
    m_pHappyFamiliesText = new wxTextCtrl (pHappyPanel, -1, wxT (""),
                                           wxDefaultPosition, wxDefaultSize,
                                           wxTE_RICH | wxTE_READONLY | wxTE_MULTILINE);
    pHappySizer->Add (m_pHappyFamiliesText, 1, wxEXPAND);

    m_pHappyDeckSizeSpin = new wxSpinCtrl (pHappyPanel, ID_HAPPY_SIZE, wxT ("90"));
    m_pHappyDisciplineSpin = new wxSpinCtrl (pHappyPanel, ID_HAPPY_DISC, wxT ("3"));
    m_pHappyMasterSpin = new wxSpinCtrl (pHappyPanel, ID_HAPPY_MASTER, wxT ("20"));

    pHappyGridSizer = new wxFlexGridSizer (3);
    pHappyGridSizer->Add (new wxStaticText (pHappyPanel, -1, wxT ("Deck size")));
    pHappyGridSizer->Add (new wxStaticText (pHappyPanel, -1, wxT ("Disciplines")));
    pHappyGridSizer->Add (new wxStaticText (pHappyPanel, -1, wxT ("Master %")));
    pHappyGridSizer->Add (m_pHappyDeckSizeSpin);
    pHappyGridSizer->Add (m_pHappyDisciplineSpin);
    pHappyGridSizer->Add (m_pHappyMasterSpin);
    pHappySizer->Add (pHappyGridSizer, 0, wxALIGN_RIGHT);
    pNotebook->AddPage (pHappyPanel, wxT ("Happy Families"));

    // Create the cardtext panel
    pCardTextPanel = new wxPanel (pNotebook, -1);
    pCardTextSizer = new wxBoxSizer (wxVERTICAL);
    pCardTextPanel->SetSizer (pCardTextSizer);
    m_pCardText = new CardText (pCardTextPanel, -1);
    pCardTextSizer->Add (m_pCardText, 1, wxEXPAND);
    pNotebook->AddPage (pCardTextPanel, wxT ("Vampire Text"));

    SetAutoLayout (TRUE);
    SetSizer (pPapaSizer);

    pFindLabel = new wxStaticText (this, -1, wxT ("Find :"));
    pFindSizer->Add (pFindLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_pCardInput = new wxTextCtrl (this, ID_CARD_INPUT, wxT (""),
                                   wxDefaultPosition, wxDefaultSize,
                                   wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
    m_pCardInput->SetToolTip (wxT ("Type in the name of a vampire, ENTER adds to deck"));
    pFindSizer->Add (m_pCardInput, 1);
    pPickerSizer->Add (pFindSizer, 0, wxEXPAND);

    m_pCardPicker = new wxListBox (this, ID_CARD_PICKER, wxDefaultPosition,
                                   wxDefaultSize, 0, NULL,
                                   wxLB_SINGLE | wxLB_ALWAYS_SB);
    //  m_pCardPicker->SetSize (-1, m_pCardPicker->GetSize ().GetHeight () * 2);
    FillCardPicker ();
    pPickerSizer->Add (m_pCardPicker, 1, wxEXPAND);

    pTopSizer->Add (pPickerSizer, 1, wxEXPAND);

    m_pSetPicker = new wxComboBox (this, ID_SET_PICKER, wxT (""), wxDefaultPosition,
                                   wxDefaultSize, 0, 0, wxCB_READONLY);
    pSubPickerSizer->Add (m_pSetPicker, 1);

    m_pAddButton = new wxButton (this, ID_ADD_BUTTON, wxT ("Add to crypt"));
    pSubPickerSizer->Add (m_pAddButton, 0);

    pPickerSizer->Add (pSubPickerSizer, 0, wxEXPAND);

    pTopSizer->Add (pNotebook, 1, wxEXPAND | wxLEFT, 5);

    pPapaSizer->Add (pTopSizer, 1, wxEXPAND | wxALL, 5);

    // Place the vampire count label
    m_pVampireCountLabel = new wxStaticText (this, -1, wxT (""), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    pPapaSizer->Add (m_pVampireCountLabel, 0, wxEXPAND | wxALL, 5);

    m_pVampireList = new wxListView (this, ID_VAMPIRE_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
    m_pVampireList->SetToolTip (wxT ("Double click to set amount\nRight click for menu"));

    m_pVampireList->InsertColumn (0, wxT ("Count"));
    m_pVampireList->InsertColumn (1, wxT ("Name"));
    m_pVampireList->InsertColumn (2, wxT ("Adv"));
    m_pVampireList->InsertColumn (3, wxT ("Cap"));
    m_pVampireList->InsertColumn (4, wxT ("Disciplines"));
    m_pVampireList->InsertColumn (5, wxT ("Title"));
    m_pVampireList->InsertColumn (6, wxT ("Clan"));
    m_pVampireList->InsertColumn (7, wxT ("Grp"));
    m_pVampireList->InsertColumn (8, wxT ("Set"));
    m_pVampireList->InsertColumn (9, wxT ("Text"));


    pPapaSizer->Add (m_pVampireList, 1, wxEXPAND | wxALL, 5);

    // vampire list popup menu
    m_oPopupMenu.Append (ID_POPUP_MORE, wxT ("More\tF7"));
    m_oPopupMenu.Append (ID_POPUP_LESS, wxT ("Less\tF6"));
    m_oPopupMenu.Append (ID_POPUP_REMOVE_ALL, wxT ("Remove all\tdel"));

    pPapaSizer->Layout ();
    pPapaSizer->Fit (m_pParent);
}


DeckCryptTab::~DeckCryptTab ()
{

}



void
DeckCryptTab::AddToDeck ()
{
    wxArrayString *pRecord;
    long lCardRef;

    pRecord = (wxArrayString *) m_pSetPicker->GetClientData (m_pSetPicker->GetSelection ());
    if (pRecord != NULL) {
        if (pRecord->Item (2).ToLong (&lCardRef)) {
            m_pModel->AddToCrypt (lCardRef, 1, TRUE);
            m_lSelectedVampire = lCardRef;
        }
    }

}


void
DeckCryptTab::FillCardPicker ()
{
    Database *pDatabase = Database::Instance ();
    wxString sFullCardName;

    pDatabase->Query (s_sCardNamesQuery, &m_oNameList);

    m_pCardPicker->Freeze ();
    for (unsigned int i = 0; i < m_oNameList.GetCount (); i++) {
        // Is it an advanced vampire ?
        if (!m_oNameList.Item (i).Item (1).IsEmpty ()) {
            m_pCardPicker->Append (m_oNameList.Item (i).Item (0).Append (wxT (" [Advanced]")).c_str ());
        } else { // regular vampire
            m_pCardPicker->Append (m_oNameList.Item (i).Item (0).c_str ());
        }


        // associated data is a pointer to the corresponding Record
        m_pCardPicker->SetClientData (i, &m_oNameList.Item (i));

        // Fill the other name array for "speling"
        m_oArrayOfNames.Add (m_oNameList.Item (i).Item (0));
    }

    m_pCardPicker->Thaw ();
}


void
DeckCryptTab::OnAddButtonClick (wxCommandEvent& WXUNUSED (event))
{
    if (m_bNoEvents) return;
    AddToDeck ();
}


void
DeckCryptTab::OnCardInputChanged (wxCommandEvent& WXUNUSED (event))
{
    bool bSearchLower = TRUE, bSearchUpper = TRUE;
    int iIndex = 0, iStringLength;

    if (m_bNoEvents) return;

    iStringLength = m_pCardInput->GetValue ().Len ();

    if (iStringLength <= 0) {
        return;
    }

    m_iCycleLowerValue = -1;
    m_iCycleUpperValue = -1;

    // search for the first card that meets the description
    while (bSearchLower) {
        // Here we search card names
        bSearchLower = m_pCardInput->GetValue ().CmpNoCase (m_pCardPicker->GetString (iIndex).Left (iStringLength).c_str ()) != 0;
        if (!bSearchLower) {
            m_iCycleLowerValue = iIndex;
        }
        iIndex++;
        if (iIndex >= m_pCardPicker->GetCount ()) {
            bSearchLower = FALSE;
        }
    }
    if (m_iCycleLowerValue >= 0) {
        // search for the last card that meets the description
        while (bSearchUpper) {
            // Here we search card names
            bSearchUpper = m_pCardInput->GetValue ().CmpNoCase (m_pCardPicker->GetString (iIndex).Left (iStringLength).c_str ()) == 0;
            if (!bSearchUpper) {
                m_iCycleUpperValue = iIndex;
            }
            iIndex++;
            if (iIndex >= m_pCardPicker->GetCount ()) {
                bSearchUpper = FALSE;
                m_iCycleUpperValue = iIndex;
            }
        }
    }

    // If we have found something
    if (m_iCycleLowerValue != -1) {
        m_iCycleCounter = 0;

        m_pCardPicker->SetSelection (m_iCycleLowerValue);
        m_pCardPicker->SetFirstItem (m_iCycleLowerValue);
    } else
        // Use our phat speling powaz
    {
        int iRet;
        wxString sVal (m_pCardInput->GetValue ());
        if ((iRet = check_speling (sVal, m_oArrayOfNames)) >= 0) {
            m_iCycleCounter = 0;
            m_iCycleLowerValue = iRet;
            m_iCycleUpperValue = iRet;
            m_pCardPicker->SetSelection (iRet);
            m_pCardPicker->SetFirstItem (iRet);
        }
    }

    // Force a call here since the selection event doesn't seem to be triggered
    wxCommandEvent oEvt;
    OnCardPickerSelection (oEvt);

}



void
DeckCryptTab::OnCardInputEnter (wxCommandEvent& WXUNUSED (event))
{
    if (m_bNoEvents) return;
    m_pCardInput->SetSelection (0, -1);
    AddToDeck ();
}



void
DeckCryptTab::OnCardPickerDoubleClick (wxCommandEvent& WXUNUSED (event))
{
    if (m_bNoEvents) return;
    AddToDeck ();
}


void
DeckCryptTab::OnCardPickerSelection (wxCommandEvent& WXUNUSED (event))
{
    Database *pDatabase = Database::Instance ();
    long lCardRef;
    wxString sQuery, sInfo, sTemp, sAdvanced = wxT ("");

    if (m_bNoEvents) return;

    sTemp = m_pCardPicker->GetStringSelection ();
    sTemp.Replace (wxT ("'"), wxT ("''"));

    if (sTemp.Replace (wxT (" [Advanced]"), wxT ("")))
        sAdvanced = wxT ("Advanced");

    sQuery.Printf (wxT ("SELECT crypt_view_with_proxy.card_name, "
                        "       crypt_view_with_proxy.set_name, "
                        "       crypt_view_with_proxy.card_ref, "
                        "       crypt_view_with_proxy.card_type, "
                        "       crypt_view_with_proxy.advanced, "
                        "       cards_sets.full_name, "
                        "       cards_sets.release_date "
                        "FROM crypt_view_with_proxy, cards_sets "
                        "WHERE crypt_view_with_proxy.set_ref = cards_sets.record_num "
                        "      AND card_name = '%s' "
                        "      AND advanced = '%s' "
                        "ORDER BY cards_sets.release_date DESC ;"),
                   sTemp.c_str (),
                   sAdvanced.c_str ());

    pDatabase->Query (sQuery.c_str (), &m_oCardList);

    if (m_oCardList.GetCount () != 0) {
        m_pSetPicker->Freeze ();

        m_pSetPicker->Clear ();

        m_oCardList.Item (0).Item (2).ToLong (&lCardRef);
        m_pCardText->DisplayCryptText(lCardRef,NULL);

        for (unsigned int i = 0; i < m_oCardList.GetCount (); i++) {
            sInfo.Clear ();

            // Add the name
            sInfo = m_oCardList.Item (i).Item (5);

            // associated data is a pointer to the record
            m_pSetPicker->Append (sInfo,
                                  (void *) &m_oCardList.Item (i));
            m_pSetPicker->SetSelection (0);
        }

        m_pSetPicker->Thaw ();
    }
}



void
DeckCryptTab::OnColumnClick (wxListEvent &event)
{
    unsigned int iColumn;
    wxListItem oItem;

    if (m_bNoEvents) return;

    iColumn = event.GetColumn ();

    if (iColumn == m_uiSortColumn) {
        m_bReverseSortOrder = !m_bReverseSortOrder;
    } else {
        m_bReverseSortOrder = FALSE;
        m_uiSortColumn = iColumn;
    }

    if (m_bReverseSortOrder) {
        m_pVampireList->SortItems (CompareFunction, iColumn + 1000);
    } else {
        m_pVampireList->SortItems (CompareFunction, iColumn);
    }

    // Make sure our selected line stays visible
//    m_lSelectedVampire = m_pVampireList->GetFirstSelected ();
//    m_pVampireList->Focus (m_lSelectedVampire);
}


void
DeckCryptTab::OnHappyDiscChange (wxSpinEvent& WXUNUSED (event))
{
    m_pModel->SetHappyDisciplineCount (m_pHappyDisciplineSpin->GetValue ());
    m_pModel->ComputeCryptHappiness ();
    UpdateHappyFamilies ();
}


void
DeckCryptTab::OnHappyMasterChange (wxSpinEvent& WXUNUSED (event))
{
    m_pModel->SetHappyMasterPercentage (m_pHappyMasterSpin->GetValue ());
    m_pModel->ComputeCryptHappiness ();
    UpdateHappyFamilies ();
}


void
DeckCryptTab::OnHappySizeChange (wxSpinEvent& WXUNUSED (event))
{
    m_pModel->SetHappyLibrarySize (m_pHappyDeckSizeSpin->GetValue ());
    m_pModel->ComputeCryptHappiness ();
    UpdateHappyFamilies ();
}


void
DeckCryptTab::OnPopupLess (wxCommandEvent& WXUNUSED (event))
{
    wxArrayLong oIndexArray;
    long lVampireIndex, lVampireRef;
    wxListItem oInfo;

    if (m_bNoEvents) return;

    lVampireIndex = m_pVampireList->GetFirstSelected ();

    while (lVampireIndex  >= 0) {
        // Get the associated info
        oInfo.m_itemId = lVampireIndex;
        oInfo.m_mask = wxLIST_MASK_DATA;
        m_pVampireList->GetItem (oInfo);

        // Find out the Vampire reference
        lVampireRef = oInfo.m_data;
        oIndexArray.Add (lVampireRef);

        lVampireIndex = m_pVampireList->GetNextSelected (lVampireIndex);
    }

    for (unsigned int i = 0; i < oIndexArray.Count (); i++) {
        m_pModel->DelFromCrypt (oIndexArray.Item (i), 1, FALSE);
    }

    Update ();
}


void
DeckCryptTab::OnPopupMore (wxCommandEvent& WXUNUSED (event))
{
    wxArrayLong oIndexArray;
    long lVampireIndex, lVampireRef;
    wxListItem oInfo;

    if (m_bNoEvents) return;

    lVampireIndex = m_pVampireList->GetFirstSelected ();

    while (lVampireIndex  >= 0) {
        // Get the associated info
        oInfo.m_itemId = lVampireIndex;
        oInfo.m_mask = wxLIST_MASK_DATA;
        m_pVampireList->GetItem (oInfo);

        // Find out the Vampire reference
        lVampireRef = oInfo.m_data;
        oIndexArray.Add (lVampireRef);

        lVampireIndex = m_pVampireList->GetNextSelected (lVampireIndex);
    }

    for (unsigned int i = 0; i < oIndexArray.Count (); i++) {
        m_pModel->AddToCrypt (oIndexArray.Item (i), 1, FALSE);
    }

    Update ();
}


void
DeckCryptTab::OnPopupRemoveAll (wxCommandEvent& WXUNUSED (event))
{
    wxArrayLong oIndexArray;
    long lVampireIndex, lVampireRef;
    wxListItem oInfo;

    if (m_bNoEvents) return;

    lVampireIndex = m_pVampireList->GetFirstSelected ();

    while (lVampireIndex  >= 0) {
        // Get the associated info
        oInfo.m_itemId = lVampireIndex;
        oInfo.m_mask = wxLIST_MASK_DATA;
        m_pVampireList->GetItem (oInfo);

        // Find out the Vampire reference
        lVampireRef = oInfo.m_data;
        oIndexArray.Add (lVampireRef);

        lVampireIndex = m_pVampireList->GetNextSelected (lVampireIndex);
    }

    for (unsigned int i = 0; i < oIndexArray.Count (); i++) {
        m_pModel->DelFromCrypt (oIndexArray.Item (i), -1, FALSE);
    }

    Update ();
}


void
DeckCryptTab::OnVampireActivate (wxListEvent& WXUNUSED (event))
{
#if 0
    wxArrayInt oArrayCount;
    wxArrayString oArrayName;

    if (m_bNoEvents) return;

    oArrayName.Add (m_sSelectedVampireName);
    oArrayCount.Add (m_lSelectedVampireAmount);

    AmountDialog * pDialog = new AmountDialog (&oArrayName, &oArrayCount);

    if (pDialog->ShowModal ()) {
        m_pModel->SetCryptRefAmount (m_lSelectedVampire, oArrayCount.Item (0));
        m_lSelectedVampireAmount = oArrayCount.Item (0);
    }

    delete pDialog;
#endif
}


void
DeckCryptTab::OnVampireListKeyDown (wxListEvent &event)
{
    wxCommandEvent oEvt;

    if (m_bNoEvents) return;

    switch (event.GetKeyCode ()) {
        // Keys to delete cards
    case WXK_BACK: // backspace
    case WXK_DELETE: // delete
        OnPopupRemoveAll (oEvt);
        break;
    case WXK_F6:
        OnPopupLess (oEvt);
        break;
    case WXK_F7:
        OnPopupMore (oEvt);
        break;
    default:
//       wxString sMessage = "Pressed key #";
//       sMessage << event.GetKeyCode ();
//       wxLogMessage (sMessage);
//       event.Skip ();
        break;
    }
}


void
DeckCryptTab::OnVampireRightClick (wxListEvent &event)
{
    long lVampire, lClickedVampire;

    if (m_bNoEvents) return;

    lClickedVampire = event.GetIndex ();

    if (!m_pVampireList->IsSelected (lClickedVampire)) {
        // unselect the other vampire(s)
        lVampire = m_pVampireList->GetFirstSelected ();
        while (lVampire != -1) {
            m_pVampireList->Select (lVampire, FALSE);
            lVampire = m_pVampireList->GetNextSelected (lVampire);
        }
        // and select this one
        m_pVampireList->Select (lClickedVampire, TRUE);
        m_pVampireList->Focus (lClickedVampire);
    }

    m_pVampireList->PopupMenu (&m_oPopupMenu, event.GetPoint ());
}


void
DeckCryptTab::OnVampireSelect (wxListEvent &event)
{
    long lVampire, lCount;

    if (m_bNoEvents) return;

    lVampire = event.GetIndex ();
    if (lVampire != m_pVampireList->GetFirstSelected ()) return;

    if ((lVampire >= 0) && (lVampire < m_pVampireList->GetItemCount ())) {
        wxListItem oInfo;

        // Get the associated info
        oInfo.m_itemId = lVampire;
        oInfo.m_col = 1;
        oInfo.m_mask = wxLIST_MASK_TEXT;
        m_pVampireList->GetItem (oInfo);

        m_pCardInput->Clear ();
        m_pCardInput->AppendText (oInfo.m_text);

        m_lSelectedVampire = m_pVampireList->GetItemData (lVampire);
        m_sSelectedVampireName = oInfo.m_text;

        // Get the associated amount
        oInfo.m_col = 0;
        m_pVampireList->GetItem (oInfo);
        oInfo.m_text.ToLong (&lCount);

        m_lSelectedVampireAmount = lCount;
    }
}


void
DeckCryptTab::SetVampireCount (long lCount, long lMin, long lMax, double dAvg)
{
    wxString sLabel;

    if (lCount > 0) {
        sLabel.Printf (wxT ("Crypt count: %ld    Average capacity: %.2f   (min: %ld, max: %ld)"), lCount, dAvg, lMin, lMax);
    } else {
        sLabel.Printf (wxT ("Crypt count: %ld"), lCount);
    }
    m_pVampireCountLabel->SetLabel (sLabel);
}


void
DeckCryptTab::Update ()
{
    UpdateCardList ();
    UpdateHappyFamilies ();
}


void
DeckCryptTab::UpdateCardList ()
{
    long lRef, lGroup;
    RecordSet *pCardList = m_pModel->GetCryptList ();

    m_alGroups[0] = 0;
    m_alGroups[1] = 0;

    m_bNoEvents = TRUE;

    m_pVampireList->Freeze ();
    m_pVampireList->DeleteAllItems ();

    for (unsigned int iLine = 0; iLine < pCardList->GetCount (); iLine++) {
        m_pVampireList->InsertItem (iLine, pCardList->Item (iLine).Item (0));
        if (pCardList->Item (iLine).Item (m_uiColumnCount).ToLong (&lRef)) {
            // the associated data is the vampire reference id
            m_pVampireList->SetItemData (iLine, lRef);
        }
        for (unsigned int iCol = 1; iCol < m_uiColumnCount; iCol++) {
            /* remember the view query is :
               "SELECT number_used, " 0
               "       card_name, " 1
               "       advanced, " 2
               "       capacity, " 3
               "       disciplines, " 4
               "       title, " 5
               "       card_type, " 6
               "       groupnumber, " 7
               "       set_name, " 8
               "       card_text, " 9
               "       card_ref "	   10
            */
            m_pVampireList->SetItem (iLine, iCol,
                                     pCardList->Item (iLine).Item (iCol));

            // Group checking
            if (iCol == 7 && pCardList->Item (iLine).Item (iCol).ToLong (&lGroup)) {
                if (m_alGroups[0] == 0)
                    m_alGroups[0] = lGroup;
                if (m_alGroups[1] == 0 && m_alGroups[0] != lGroup &&
                        (m_alGroups[0] == lGroup+1 || m_alGroups[0] == lGroup-1))
                    m_alGroups[1] = lGroup;

                if ((m_alGroups[0] != lGroup && m_alGroups[1] != lGroup) ||
                        (m_alGroups[1] != 0 && m_alGroups[0] != m_alGroups[1]+1 && m_alGroups[0] != m_alGroups[1]-1)) {
                    wxListItem oItem;
                    oItem.m_itemId = iLine;
                    oItem.SetTextColour (*wxRED);
                    m_pVampireList->SetItem (oItem);
                }
            }
            // end of group checking
        }
        if (m_lSelectedVampire == lRef) {
            m_pVampireList->Select (iLine, TRUE);
            m_pVampireList->Focus (iLine);
        }
    }

    // size columns with a minimum
    if (m_pModel->GetCryptCount () > 0) {
        for (unsigned int iCol = 0; iCol < m_uiColumnCount; iCol++) {
            m_pVampireList->SetColumnWidth (iCol, wxLIST_AUTOSIZE);
            if (m_pVampireList->GetColumnWidth (iCol) < 40) {
                m_pVampireList->SetColumnWidth (iCol, 40);
            }
        }
    }

    // Sort the list
    /*
    if (m_pVampireList->GetItemCount () > 0)
      {
        if (m_bReverseSortOrder)
    {
      m_pVampireList->SortItems (CompareFunction, m_uiSortColumn + 1000);
    }
        else
    {
      m_pVampireList->SortItems (CompareFunction, m_uiSortColumn);
    }
      }
    */

    SetVampireCount (m_pModel->GetCryptCount (),
                     m_pModel->GetCryptMin (),
                     m_pModel->GetCryptMax (),
                     m_pModel->GetCryptAvg ());

    m_pVampireList->Thaw ();

    m_bNoEvents = FALSE;
}


void
DeckCryptTab::UpdateHappyFamilies ()
{
    HappyBucket *pBucket;
    HappyList *pHappyList = m_pModel->GetHappyList ();
    wxFont oFont = m_pHappyFamiliesText->GetDefaultStyle ().GetFont ();;
    wxString sEntry;

    m_pHappyFamiliesText->Freeze ();
    m_pHappyFamiliesText->Clear ();

    if (pHappyList->GetCount () == 0) {
        m_pHappyFamiliesText->Thaw ();
        return;
    }

    oFont.SetWeight (wxBOLD);
    m_pHappyFamiliesText->SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));
    m_pHappyFamiliesText->WriteText (wxT ("Happy Families !\n\n"));
    oFont.SetWeight (wxNORMAL);
    m_pHappyFamiliesText->SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));

    m_pHappyFamiliesText->WriteText (wxT ("Suggested card amounts :\n\n"));

    for (unsigned int i = 0; i < pHappyList->GetCount (); i++) {
        pBucket = pHappyList->Item (i)->GetData ();
        sEntry.Clear ();
        sEntry << pBucket->m_uiCardCount << wxT ("x ")
               << pBucket->m_sName << wxT ("\n");
        m_pHappyFamiliesText->WriteText (sEntry);
    }
    m_pHappyFamiliesText->WriteText
    (wxT ("\n\"Happy Families\" formula courtesy of James McClellan (aka Legbiter)\nhttp://legbiter.tripod.com/hf/theory.htm"));

    m_pHappyFamiliesText->Thaw ();
}


// this function is used to sort the vampire list
// the column must be specified
// if the column number is >1000, then the sort order is reversed
int
DeckCryptTab::CompareFunction(long lItem1, long lItem2, long lSortColumn)
{
    int iReturnValue;
    unsigned int i = 0;
    long lValue1, lValue2, lActualCol, lRef;
    RecordSet *pRecordSet;
    wxArrayString *pRecord1, *pRecord2;
    wxString *pString1, *pString2;

    pRecord1 = NULL;
    pRecord2 = NULL;

    pRecordSet = DeckModel::Instance ()->GetCryptList ();
    while ((pRecord1 == NULL || pRecord2 == NULL)
            && i < pRecordSet->GetCount ()) {
        pRecordSet->Item (i).Item (10).ToLong (&lRef);
        if (lRef == lItem1) pRecord1 = &pRecordSet->Item (i);
        if (lRef == lItem2) pRecord2 = &pRecordSet->Item (i);
        i++;
    }

    if (pRecord1 == NULL || pRecord2 == NULL) return 0;

    if (lSortColumn >= 1000) {
        lActualCol = lSortColumn - 1000;
    } else {
        lActualCol = lSortColumn;
    }

    // Columns like capacity or group are numerical
    if (lActualCol == 0 || lActualCol == 3 || lActualCol == 7) {
        // Numerical comparison
        pRecord1->Item (lActualCol).ToLong (&lValue1);
        pRecord2->Item (lActualCol).ToLong (&lValue2);

        iReturnValue = lValue1 - lValue2;
    } else {
        // String comparison
        pString1 = &(pRecord1->Item (lActualCol));
        pString2 = &(pRecord2->Item (lActualCol));

        iReturnValue = pString1->CmpNoCase (pString2->c_str ());
    }

    if (lSortColumn >= 1000) {
        return -iReturnValue;
    } else {
        return iReturnValue;
    }

}

