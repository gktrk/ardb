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


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class BrowserFrame;

#include "browserlibrarytab.h"
#include "deckmodel.h"
#include "inventorymodel.h"
#include "main.h"
#include "speling.h"

#include <wx/confbase.h>
#include <wx/fileconf.h>

#include <wx/dir.h>

#define CARD_IMAGE_DIR wxT("cardimages")


BEGIN_EVENT_TABLE (BrowserLibraryTab, wxPanel)
// the top buttons
    EVT_BUTTON (ID_ADD_BUTTON, BrowserLibraryTab::OnAddButtonClick)
    EVT_BUTTON (ID_REMOVE_BUTTON, BrowserLibraryTab::OnRemoveButtonClick)
    EVT_BUTTON (ID_KEEP_BUTTON, BrowserLibraryTab::OnKeepButtonClick)
    EVT_BUTTON (ID_RESET_BUTTON, BrowserLibraryTab::OnResetButtonClick)
    EVT_BUTTON (ID_INVENTORY_BUTTON, BrowserLibraryTab::OnInventoryButtonClick)
// the find text field
    EVT_TEXT (ID_FIND_TEXTCTRL, BrowserLibraryTab::OnFindTextChange)

    EVT_SPLITTER_SASH_POS_CHANGED(ID_SPLITTER_WINDOW, BrowserLibraryTab::OnSplitterMoved)
//  EVT_SIZE (BrowserLibraryTab::OnTabResize)
END_EVENT_TABLE ()


BrowserLibraryTab::BrowserLibraryTab (BrowserLibraryModel *pModel, BrowserLibraryController *pController, wxNotebook *pParent, unsigned int uiNumber) :
    wxPanel (pParent, -1),
// Initialisation of member objects and variables
    m_bDisplayInventory (FALSE),
    m_lSplitterPosition (430),
    m_oArrayOfNames (),
    m_oPopupMenu (),
    m_oTabSize (),
    m_pAddButton (NULL),
    m_pController (pController),
    m_pFilterGrid (NULL),
    m_pInventoryButton (NULL),
    m_pKeepButton (NULL),
    m_pModel (pModel),
    m_pRemoveButton (NULL),
    m_pResetButton (NULL),
    m_pParent (pParent),
    m_pPapaSizer (NULL),
    m_pSplitterWindow (NULL),
    m_pFindText (NULL),
    m_pCardCountLabel (NULL),
    m_pCardGrid (NULL),
    m_pCardText (NULL),
    m_uiFilterListHeight (40),
    m_uiNumber (uiNumber),
    m_uiSortColumn (0)
{
    Init ();
}


BrowserLibraryTab::~BrowserLibraryTab ()
{
    if (m_pModel) delete m_pModel;
    m_pModel = NULL;

    wxString sConfEntry = wxT ("SplitterPositionL");
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
    if (pConfig) {
        pConfig->Write (sConfEntry, m_lSplitterPosition);
        pConfig->Flush (TRUE);
    }
}

void BrowserLibraryTab::SetFocus()
{
    if (m_pFindText != NULL) {
        m_pFindText->SetFocus();
    }
}

void
BrowserLibraryTab::Init ()
{
    wxBoxSizer *pFilterSizer;
    wxString sTabName;

    if (m_uiNumber) {
        sTabName.Printf (wxT ("Library Browser (%d)"), m_uiNumber + 1);
    } else {
        sTabName.Printf (wxT ("Library Browser"));
    }



    m_pPapaSizer = new wxBoxSizer (wxVERTICAL);
    SetSizer (m_pPapaSizer);

    // Place the find textbox and various filter buttons
    pFilterSizer = new wxBoxSizer (wxHORIZONTAL);

    wxStaticText *pFindLabel = new wxStaticText (this, -1, wxT ("Find :"));
    pFilterSizer->Add (pFindLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    m_pFindText = new wxTextCtrl (this, ID_FIND_TEXTCTRL, wxT (""), wxDefaultPosition, wxSize (150, -1));
    pFilterSizer->Add (m_pFindText, 0);

    pFilterSizer->Add (10, 0, 1);

    m_pKeepButton = new wxButton (this, ID_KEEP_BUTTON, wxT ("Select..."));
    pFilterSizer->Add (m_pKeepButton, 0, wxRIGHT, 5);
    m_pAddButton = new wxButton (this, ID_ADD_BUTTON, wxT ("Add..."));
    pFilterSizer->Add (m_pAddButton, 0, wxRIGHT, 5);
    m_pRemoveButton = new wxButton (this, ID_REMOVE_BUTTON, wxT ("Remove..."));
    pFilterSizer->Add (m_pRemoveButton, 0, wxRIGHT, 5);
    m_pResetButton = new wxButton (this, ID_RESET_BUTTON, wxT ("Reset"));
    pFilterSizer->Add (m_pResetButton, 0, wxRIGHT, 5);

    m_pPapaSizer->Add (pFilterSizer, 0, wxEXPAND | wxALL, 5);

    // The filter list
    m_pFilterGrid = new BrowserLibraryFilterGrid (m_pModel, this);
    m_pFilterGrid->SetToolTip (wxT ("Right click for menu"));
    m_pFilterGrid->SetSize (100, m_uiFilterListHeight);
    m_pFilterGrid->SetColLabelSize (0);
    m_pFilterGrid->SetRowLabelSize (0);
    m_pFilterGrid->EnableGridLines (TRUE);
    m_pFilterGrid->SetGridLineColour (this->GetBackgroundColour ());
    m_pFilterGrid->SetMargins (0, 0);
    m_pFilterGrid->CreateGrid (1, 1);
    m_pFilterGrid->SetRowSize (0, m_uiFilterListHeight - 1);
    m_pPapaSizer->Add (m_pFilterGrid, 0, wxEXPAND);

    // Place the vampire count label and inventory button
    wxBoxSizer *pInvCountSizer = new wxBoxSizer (wxHORIZONTAL);
    m_pInventoryButton = new wxButton (this, ID_INVENTORY_BUTTON, wxT ("Inventory"));
    pInvCountSizer->Add (m_pInventoryButton, 0, wxALL, 5);
    m_pCardCountLabel = new wxStaticText (this, -1, wxT ("two\nlines"), wxDefaultPosition, wxDefaultSize);
    pInvCountSizer->Add (m_pCardCountLabel, 1, wxEXPAND);
    m_pPapaSizer->Add (pInvCountSizer, 0, wxEXPAND);

    // Insert the splitter window
    m_pSplitterWindow = new wxSplitterWindow (this, ID_SPLITTER_WINDOW, wxDefaultPosition, wxDefaultSize,
            wxSP_3D | wxSP_LIVE_UPDATE);
    m_pSplitterWindow->SetMinimumPaneSize (20);

    // Create the vampire list
    m_pCardGrid = new BrowserLibraryCardGrid (m_pModel, this, m_pSplitterWindow);
    m_pCardGrid->CreateGrid (0, 9);
    m_pCardGrid->SetSelectionMode (wxGrid::wxGridSelectRows);
    m_pCardGrid->SetGridLineColour (this->GetBackgroundColour ());
    m_pCardGrid->SetToolTip (wxT ("Double click adds to deck,\nRight click for menu"));
    m_pCardGrid->SetRowLabelSize (0);
    m_pCardGrid->SetColLabelSize (20);
    m_pCardGrid->SetColMinimalAcceptableWidth (0);
    m_pCardGrid->SetColLabelValue (0, wxT ("Have"));
    m_pCardGrid->SetColLabelValue (1, wxT ("Want"));
    m_pCardGrid->SetColLabelValue (2, wxT ("Spare"));
    m_pCardGrid->SetColLabelValue (3, wxT ("Name"));
    m_pCardGrid->SetColLabelValue (4, wxT ("Type"));
    m_pCardGrid->SetColLabelValue (5, wxT ("Requires"));
    m_pCardGrid->SetColLabelValue (6, wxT ("Cost"));
    m_pCardGrid->SetColLabelValue (7, wxT ("Text"));
    m_pCardGrid->SetColLabelValue (8, wxT ("Id"));

    wxPanel *pCardTextPanel = new wxPanel(m_pSplitterWindow,-1);
    wxBoxSizer *pCardTextSizer = new wxBoxSizer (wxHORIZONTAL);

    m_pCardText = new CardText (pCardTextPanel, -1);

    m_pCardViewer = NULL;

    //if (wxDir::Exists(CARD_IMAGE_DIR)) {
    m_pCardViewer = new CardViewer(pCardTextPanel);
    //}

    pCardTextSizer->Add(m_pCardText, 3, wxEXPAND);

    if (m_pCardViewer != NULL) {
        pCardTextSizer->Add(m_pCardViewer, 1, wxEXPAND);
    }

    pCardTextPanel->SetSizer(pCardTextSizer);

    wxString sConfEntry = wxT ("SplitterPositionL");
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
    if (pConfig) {
        if (!pConfig->Read (sConfEntry, &m_lSplitterPosition)) {
            pConfig->Write (sConfEntry, m_lSplitterPosition);
            pConfig->Flush (TRUE);
        }
    }

    m_pPapaSizer->Add (m_pSplitterWindow, 1, wxEXPAND|wxFIXED_MINSIZE);

    m_pPapaSizer->Layout ();

#ifdef __WXMSW__
    if (!m_uiNumber) {
        //Required for Windows.  Without, controls are not displayed.
        m_pPapaSizer->Fit(m_pParent);
    }
#endif

    // Put both in the splitter window
    if (m_pSplitterWindow) m_pSplitterWindow->SetSashGravity(0.6);
    m_pSplitterWindow->SplitHorizontally (m_pCardGrid, pCardTextPanel, m_lSplitterPosition);

    m_oTabSize = GetSize ();

    UpdateView();
    m_pParent->AddPage (this, sTabName, TRUE);
}

void
BrowserLibraryTab::UpdateView ()
{
    if (!m_pCardGrid || !m_pModel) return;

    // update display of inventory columns
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
    if (pConfig) {
        wxString sDisplayInventory
        = wxT ("DisplayInventory");
        if (!pConfig->Read (sDisplayInventory, &m_bDisplayInventory)) {
            pConfig->Write (sDisplayInventory, m_bDisplayInventory);
            pConfig->Flush (TRUE);
        }
    }

    UpdateFilterList ();

    FillCardList ();

    if (m_pCardGrid->GetNumberRows () > 0) {
        for (unsigned int iCol = 0; iCol < 3; iCol++) {
            if (m_bDisplayInventory)
                m_pCardGrid->AutoSizeColumn (iCol);
            else
                m_pCardGrid->SetColSize (iCol, 0);
        }

        // Size last column (Id) to 0
        m_pCardGrid->SetColSize (m_pCardGrid->GetNumberCols () - 1, 0);

        for (int iCol = 3; iCol < m_pCardGrid->GetNumberCols () - 1; iCol++) {
            m_pCardGrid->AutoSizeColumn (iCol);
            if (m_pCardGrid->GetColSize (iCol) < 30) {
                m_pCardGrid->SetColSize (iCol, 30);
            }
        }
    }

    SetCardCount (m_pModel->GetCardCount ());
    SetCardText ();

    SetCursor (*wxSTANDARD_CURSOR);
    m_pCardGrid->ForceRefresh ();
    m_pCardGrid->Scroll (0,0);
}


void
BrowserLibraryTab::UpdateFilterList ()
{
    unsigned int i;
    wxListItem oItem;

    if (!m_pFilterGrid || !m_pModel) return;

    if (m_pModel->GetFilterCount () > 0) {
        m_pFilterGrid->Freeze ();

        m_pFilterGrid->DeleteCols (0, m_pFilterGrid->GetNumberCols (), FALSE);
        m_pFilterGrid->AppendCols (m_pModel->GetFilterCount (), FALSE);
        for (i = 0; i < m_pModel->GetFilterCount (); i++) {
            m_pFilterGrid->SetCellValue (0, i, m_pModel->GetFilterSummary (i));
            m_pFilterGrid->SetReadOnly (0, i);
            m_pFilterGrid->AutoSizeColumn (i);
            if (m_pModel->IsFilterVisible (i)) {
                m_pFilterGrid->SetCellBackgroundColour(0, i, wxColour (100, 180, 100));
            } else {
                m_pFilterGrid->SetCellBackgroundColour(0, i, wxColour (180, 100, 100));
            }
        }
        //if (m_pPapaSizer) m_pPapaSizer->Layout ();

        m_pFilterGrid->MakeCellVisible (0, i - 1);
        m_pFilterGrid->AutoSizeRow (0);
        m_pFilterGrid->SetClientSize (100, m_pFilterGrid->GetRowSize (0));

        if (m_pPapaSizer->GetItem (m_pFilterGrid) == NULL) {
            m_pPapaSizer->Insert (1, m_pFilterGrid, 0, wxEXPAND);
        }
        m_pFilterGrid->Show ();
        m_pFilterGrid->Thaw ();
    } else {
        if (m_pPapaSizer->GetItem (m_pFilterGrid) != NULL) {
            m_pPapaSizer->Detach (m_pFilterGrid);
        }
        m_pFilterGrid->Hide ();
    }
    m_pPapaSizer->Layout ();
}


void
BrowserLibraryTab::FillCardList ()
{
    wxString sCopy;

    //  wxStopWatch sw;

    if (!m_pCardGrid || !m_pModel || !m_pModel->GetCardList ()) return;

    m_oArrayOfNames.Clear ();

    m_pCardGrid->Freeze ();
    m_pCardGrid->BeginBatch ();

    if (m_pCardGrid->GetNumberRows() > 0) {
        m_pCardGrid->DeleteRows (0, m_pCardGrid->GetNumberRows ());
    }

    m_pCardGrid->AppendRows (m_pModel->GetCardCount (), FALSE);

    for (unsigned int iLine = 0; iLine < m_pModel->GetCardCount (); iLine++) {
        m_oArrayOfNames.Add (m_pModel->GetCardList ()->Item (iLine).Item (10));
        m_pCardGrid->AutoSizeRow (iLine);

        for (int iCol = 0; iCol < m_pCardGrid->GetNumberCols (); iCol++) {
            sCopy = m_pModel->GetCardList ()->Item (iLine).Item (iCol);
            sCopy.Replace (wxT ("\r"), wxT (" "));
            sCopy.Replace (wxT ("\n"), wxT (" "));

            m_pCardGrid->SetCellValue (iLine, iCol, sCopy);
        }
    }

    //  Fit ();
    m_pCardGrid->MakeCellVisible (0, 0);
    m_pCardGrid->EndBatch ();
    m_pCardGrid->Thaw ();

    //  wxLogMessage (_T("items inserted in %ldms\n"), sw.Time());
}


void
BrowserLibraryTab::SetCardText (long lCardRef)
{
    if (!m_pCardText) return;

    //  m_pSplitterWindow->SetSashPosition (m_lSplitterPosition);

    if (lCardRef == -1) {
        m_pCardText->Clear ();

        if (m_pCardViewer != NULL) {
            m_pCardViewer->Clear();
        }
    } else {
        wxArrayString cardNames;
        m_pCardText->DisplayLibraryText(lCardRef,&cardNames);

        if (m_pCardViewer != NULL) {
            if (cardNames.Count() > 0) {
                m_pCardViewer->SetImage(&cardNames);
            } else {
                m_pCardViewer->Clear();
            }
        }
    }
}


void
BrowserLibraryTab::SetCardCount (unsigned int uiCount)
{
    wxString sLabel;

    sLabel.Printf (wxT ("Library count: %d"), uiCount);
    m_pCardCountLabel->SetLabel (sLabel);
}


void
BrowserLibraryTab::OnAddButtonClick (wxCommandEvent& WXUNUSED (event))
{
    BrowserLibraryFilter oDialog (m_pController);

    m_pController->SetFilterMode (LIBRARY_FILTER_ADD);
    oDialog.CentreOnParent ();
    oDialog.SetDialogTitle (LIBRARY_FILTER_ADD);
    oDialog.Reset ();
    oDialog.ShowModal ();
}


void
BrowserLibraryTab::OnRemoveButtonClick (wxCommandEvent& WXUNUSED (event))
{
    BrowserLibraryFilter oDialog (m_pController);

    m_pController->SetFilterMode (LIBRARY_FILTER_REMOVE);
    oDialog.CentreOnParent ();
    oDialog.SetDialogTitle (LIBRARY_FILTER_REMOVE);
    oDialog.Reset ();
    oDialog.ShowModal ();
}


void
BrowserLibraryTab::OnInventoryButtonClick (wxCommandEvent& WXUNUSED (event))
{
    // Toggle mode
    m_bDisplayInventory = !m_bDisplayInventory;

    m_pCardGrid->DisableCellEditControl ();

    //save display preference
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
    if (pConfig) {
        wxString sDisplayInventory = wxT ("DisplayInventory");
        pConfig->Write (sDisplayInventory, m_bDisplayInventory);
        pConfig->Flush (TRUE);
    }

    // update display
    for (unsigned int iCol = 0; iCol < 3; iCol++) {
        if (m_bDisplayInventory)
            m_pCardGrid->AutoSizeColumn (iCol);
        else
            m_pCardGrid->SetColSize (iCol, 0);
    }
    m_pCardGrid->ForceRefresh ();

}


void
BrowserLibraryTab::OnKeepButtonClick (wxCommandEvent& WXUNUSED (event))
{
    BrowserLibraryFilter oDialog (m_pController);

    m_pController->SetFilterMode (LIBRARY_FILTER_KEEP);
    oDialog.CentreOnParent ();
    oDialog.SetDialogTitle (LIBRARY_FILTER_KEEP);
    oDialog.Reset ();
    oDialog.ShowModal ();
}


void
BrowserLibraryTab::OnResetButtonClick (wxCommandEvent& WXUNUSED (event))
{
    SetCursor (*wxHOURGLASS_CURSOR);

    m_pModel->Reset ();
}


void
BrowserLibraryTab::OnSplitterMoved (wxSplitterEvent& event)
{
    if (m_pCardGrid->MyGetSelectedRows ().Count ()) {
        m_pCardGrid->MakeCellVisible (m_pCardGrid->MyGetSelectedRows ().Item (0), 0);
    }

    if (m_pSplitterWindow &&
            m_pSplitterWindow->GetSashPosition () > 0)
        m_lSplitterPosition = m_pSplitterWindow->GetSashPosition ();

    event.Skip ();
}


void
BrowserLibraryTab::OnTabResize (wxSizeEvent &event)
{
    /*
    int iDeltaHeight, iSashPosition;

    if (m_pSplitterWindow != NULL)
    {
    iDeltaHeight = event.GetSize ().GetHeight () - m_oTabSize.GetHeight ();
    iSashPosition = m_pSplitterWindow->GetSashPosition ();
    m_pSplitterWindow->SetSashPosition (iSashPosition + iDeltaHeight);
    m_oTabSize = event.GetSize ();
    }
    */

    event.Skip ();
}


void
BrowserLibraryTab::OnFindTextChange (wxCommandEvent& WXUNUSED (event))
{
    bool bSearch = TRUE;
    int iCard = 0;
    long lCardRef;
    wxArrayString *pRecord, *pMatch = 0;
    int iStringLength = m_pFindText->GetValue ().Len ();

    m_pCardGrid->Freeze ();
    while (bSearch && (unsigned int) iCard < m_pModel->GetCardCount ()) {
        // Here we search vampire names
        pRecord = &(m_pModel->GetCardList ()->Item (iCard));
        bSearch = m_pFindText->GetValue ().CmpNoCase (pRecord->Item (10).Left (iStringLength).c_str ()) != 0;
        if (!bSearch) {
            pMatch = pRecord;
        } else {
            iCard++;
        }
    }

    // Not found, use our phat speling powaz
    if (bSearch) {
        wxString sVal (m_pFindText->GetValue ());
        if ((iCard = check_speling (sVal, m_oArrayOfNames)) >= 0) {
            bSearch = FALSE;
        }
    }

    if (bSearch == FALSE) {
        // Unselect current vampire(s)
        m_pCardGrid->ClearSelection ();

        m_pCardGrid->MakeCellVisible (m_pCardGrid->GetNumberRows () - 1, 0);
        m_pCardGrid->MakeCellVisible (iCard, 0);
        m_pCardGrid->SelectRow (iCard);
        m_pCardGrid->GetCellValue (iCard, m_pCardGrid->GetNumberCols () -1).ToLong (&lCardRef);
        SetCardText (lCardRef);
    }

    m_pCardGrid->Thaw ();
}







/* ****************************************************************
*
* class BrowserLibraryFilterGrid
*
****************************************************************** */


BEGIN_EVENT_TABLE (BrowserLibraryFilterGrid, wxGrid)
    EVT_GRID_CELL_RIGHT_CLICK (BrowserLibraryFilterGrid::OnFilterRightClick)
    EVT_MENU (ID_POPUP_REMOVE, BrowserLibraryFilterGrid::OnFilterPopupRemove)
    EVT_MENU (ID_POPUP_HIDE, BrowserLibraryFilterGrid::OnFilterPopupHide)
    EVT_MENU (ID_POPUP_MOVELEFT, BrowserLibraryFilterGrid::OnFilterPopupMoveLeft)
    EVT_MENU (ID_POPUP_MOVERIGHT, BrowserLibraryFilterGrid::OnFilterPopupMoveRight)
END_EVENT_TABLE ()


BrowserLibraryFilterGrid::BrowserLibraryFilterGrid (BrowserLibraryModel *pModel, BrowserLibraryTab *pParent) :
    wxGrid (pParent, -1),
    m_oFilterPopupMenu (),
    m_pModel (pModel),
    m_pParent (pParent)
{
    // Filter List Popup Menu
    m_oFilterPopupMenu.Append (ID_POPUP_REMOVE, wxT ("Remove"));
    m_oFilterPopupMenu.Append (ID_POPUP_HIDE, wxT ("Switch on/off"));
    m_oFilterPopupMenu.Append (ID_POPUP_MOVELEFT, wxT ("Move left"));
    m_oFilterPopupMenu.Append (ID_POPUP_MOVERIGHT, wxT ("Move right"));
}


BrowserLibraryFilterGrid::~BrowserLibraryFilterGrid ()
{

}


void
BrowserLibraryFilterGrid::OnFilterPopupHide (wxCommandEvent& WXUNUSED (event))
{
    bool bState;
    long lFilter;

    // repaint where the popup was
    Update ();

    m_pParent->SetCursor (*wxHOURGLASS_CURSOR);

    for (lFilter = GetNumberCols () - 1; lFilter >= 0; lFilter--) {
        if (IsInSelection (0, lFilter)) {
            bState = m_pModel->IsFilterVisible (lFilter);
            m_pModel->SetFilterVisible (lFilter, !bState, FALSE);
        }
    }
    m_pParent->UpdateView ();
}


void
BrowserLibraryFilterGrid::OnFilterPopupMoveLeft (wxCommandEvent& WXUNUSED (event))
{
    long lFilter;

    // repaint where the popup was
    Update ();

    m_pParent->SetCursor (*wxHOURGLASS_CURSOR);

    for (lFilter = 0; lFilter < GetNumberCols (); lFilter++) {
        if (IsInSelection (0, lFilter)) {
            m_pModel->LowerFilter (lFilter, FALSE);
        }
    }
    m_pParent->UpdateView ();

}


void
BrowserLibraryFilterGrid::OnFilterPopupMoveRight (wxCommandEvent& WXUNUSED (event))
{
    long lFilter;

    // repaint where the popup was
    Update ();

    m_pParent->SetCursor (*wxHOURGLASS_CURSOR);

    for (lFilter = GetNumberCols () - 1; lFilter >= 0; lFilter--) {
        if (IsInSelection (0, lFilter)) {
            m_pModel->RaiseFilter (lFilter, FALSE);
        }
    }
    m_pParent->UpdateView ();

}


void
BrowserLibraryFilterGrid::OnFilterPopupRemove (wxCommandEvent& WXUNUSED (event))
{
    long lFilter;

    // repaint where the popup was
    Update ();

    m_pParent->SetCursor (*wxHOURGLASS_CURSOR);

    for (lFilter = GetNumberCols () - 1; lFilter >= 0; lFilter--) {
        if (IsInSelection (0, lFilter)) {
            m_pModel->DeleteFilter (lFilter, FALSE);
        }
    }
    m_pParent->UpdateView ();
}


void
BrowserLibraryFilterGrid::OnFilterRightClick (wxGridEvent &event)
{
    long lClickedFilter;

    lClickedFilter = event.GetCol ();
    if (!IsInSelection (0, lClickedFilter)) {
        // unselect the other filter(s)
        ClearSelection ();
        // and select this one
        SelectBlock (0, lClickedFilter, 0, lClickedFilter);
    }
    PopupMenu (&m_oFilterPopupMenu, event.GetPosition ());
}


/* ****************************************************************
*
* class BrowserLibraryCardGrid
*
****************************************************************** */

BEGIN_EVENT_TABLE (BrowserLibraryCardGrid, wxGrid)
    EVT_GRID_CELL_CHANGE(BrowserLibraryCardGrid::OnInventoryChange)
    EVT_GRID_CELL_RIGHT_CLICK (BrowserLibraryCardGrid::OnCardRightClick)
    EVT_GRID_LABEL_LEFT_CLICK (BrowserLibraryCardGrid::OnColumnClick)
    EVT_GRID_CELL_LEFT_DCLICK (BrowserLibraryCardGrid::OnCardActivated)
    EVT_GRID_SELECT_CELL(BrowserLibraryCardGrid::OnCardSelect)
    EVT_MENU (ID_POPUP_ADD, BrowserLibraryCardGrid::OnPopupAddToDeck)
    EVT_MENU (ID_POPUP_ADD_ALL, BrowserLibraryCardGrid::OnPopupAddAllToDeck)
    EVT_MENU (ID_POPUP_REMOVE, BrowserLibraryCardGrid::OnPopupRemove)
    EVT_MENU (ID_POPUP_COPY_ALL, BrowserLibraryCardGrid::OnPopupCopyAll)
END_EVENT_TABLE ()


BrowserLibraryCardGrid::BrowserLibraryCardGrid (BrowserLibraryModel *pModel, BrowserLibraryTab *pTab, wxWindow *pParent) :
    wxGrid (pParent, -1),
    m_bReverseSortOrder (FALSE),
    m_oSelection (),
    m_oCardPopupMenu (),
    m_pModel (pModel),
    m_pParent (pParent),
    m_pTab (pTab),
    m_uiSortColumn (3)
{
    // Card List Popup Menu
    m_oCardPopupMenu.Append (ID_POPUP_ADD, wxT ("Add selected to deck"));
    m_oCardPopupMenu.Append (ID_POPUP_ADD_ALL, wxT ("Add all to deck"));
    m_oCardPopupMenu.Append (ID_POPUP_REMOVE, wxT ("Remove selected"));
    //  m_oCardPopupMenu.AppendSeparator ();
    //   m_oCardPopupMenu.Append (ID_POPUP_COPY_ALL, "Copy all list to clipboard");

}


BrowserLibraryCardGrid::~BrowserLibraryCardGrid ()
{

}


wxArrayInt
BrowserLibraryCardGrid::MyGetSelectedRows ()
{
    wxGridCellCoordsArray upperlefts=GetSelectionBlockTopLeft ();
    wxGridCellCoordsArray lowerrights=GetSelectionBlockBottomRight ();

    m_oSelection.Clear ();
    for(size_t i = 0; i < upperlefts.Count (); ++i) {
        for (int j = upperlefts.Item (i).GetRow ();
                j <= lowerrights.Item (i).GetRow (); ++j) {
            m_oSelection.Add (j);
        }
    }
    return m_oSelection;
}


void
BrowserLibraryCardGrid::OnCardActivated (wxGridEvent &event)
{
    // Add to deck unless we're in the inventory columns
    if (event.GetCol () >= 3)
        OnPopupAddToDeck (event);
}


void
BrowserLibraryCardGrid::OnCardRightClick (wxGridEvent &event)
{
    PopupMenu (&m_oCardPopupMenu, event.GetPosition ());

    event.Skip ();
}


void
BrowserLibraryCardGrid::OnCardSelect (wxGridEvent& event)
{
    long lCardRef;

    GetCellValue (event.GetRow (), GetNumberCols () -1).ToLong (&lCardRef);
    m_pTab->SetCardText (lCardRef);

    //  wxLogMessage (wxT ("%d,%d"), event.GetRow (), event.GetCol ());
    if (event.GetCol () < 3) {
        EnableEditing (TRUE);
    } else {
        EnableEditing (FALSE);
    }
    event.Skip ();
    MakeCellVisible (event.GetRow (), 0);
}


void
BrowserLibraryCardGrid::OnColumnClick (wxGridEvent &event)
{
    unsigned int uiColumn = event.GetCol ();

    if (uiColumn == m_uiSortColumn) {
        m_bReverseSortOrder = !m_bReverseSortOrder;
    } else {
        m_bReverseSortOrder = FALSE;
        m_uiSortColumn = uiColumn;
    }

    uiColumn++;
    if (m_bReverseSortOrder) uiColumn = -uiColumn;
    m_pModel->SetSortColumn (uiColumn);

    m_pModel->ExecFilterList (TRUE);
    m_pTab->FillCardList ();
}


void
BrowserLibraryCardGrid::OnInventoryChange (wxGridEvent& event)
{
    InventoryModel *pInventoryModel = InventoryModel::Instance ();

    unsigned int uiCol = event.GetCol (),
                 uiRow = event.GetRow ();
    long lHave, lWant, lSpare;
    wxString sName;

    GetCellValue (uiRow, 0).ToLong (&lHave);
    GetCellValue (uiRow, 1).ToLong (&lWant);
    GetCellValue (uiRow, 2).ToLong (&lSpare);
    sName = GetCellValue (uiRow, 3);

    if (uiCol > 2) {
        wxLogError (wxT ("You are not supposed to edit this column."));
        return;
    }

    pInventoryModel->SetHWSLibraryName (sName, wxEmptyString,
                                        lHave, lWant, lSpare);
}


void
BrowserLibraryCardGrid::OnPopupAddAllToDeck (wxCommandEvent& WXUNUSED (event))
{
    DeckModel *pDeck = DeckModel::Instance ();
    long lCardRef = 0;

    if (m_pModel->GetCardCount () > 20) {
        wxMessageDialog oWarningDialog (NULL, wxT ("That's a lot of cards, are you sure ?"), wxT ("Really ?"), wxYES | wxNO | wxICON_QUESTION);

        if (oWarningDialog.ShowModal () != wxID_YES) {
            return;
        }
    }

    for (unsigned int iCard = 0; iCard < m_pModel->GetCardCount (); iCard++) {
        m_pModel->GetCardList ()->Item (iCard).Item (8).ToLong (&lCardRef);
        pDeck->AddToLibrary (lCardRef, 1, FALSE);
    }
    // force refresh with a dummy operation
    pDeck->AddToLibrary (lCardRef, 0, TRUE);
}


void
BrowserLibraryCardGrid::OnPopupAddToDeck (wxCommandEvent& WXUNUSED (event))
{
    DeckModel *pDeck = DeckModel::Instance ();
    long lCardRef = 0;

    if (MyGetSelectedRows ().Count () > 20) {
        wxMessageDialog oWarningDialog (NULL, wxT ("That's a lot of cards, are you sure ?"), wxT ("Really ?"), wxYES | wxNO | wxICON_QUESTION);

        if (oWarningDialog.ShowModal () != wxID_YES) {
            return;
        }
    }

    for (unsigned int i = 0; i < MyGetSelectedRows ().Count (); i++) {
        GetCellValue (MyGetSelectedRows ().Item (i), GetNumberCols () -1).ToLong (&lCardRef);
        pDeck->AddToLibrary (lCardRef, 1, FALSE);
    }
    // force refresh with a dummy operation
    pDeck->AddToLibrary (lCardRef, 0, TRUE);
}


void
BrowserLibraryCardGrid::OnPopupCopyAll (wxCommandEvent& WXUNUSED (event))
{
    unsigned int iCard;
    wxString sTempList, sCardList;
    wxTextDataObject *pDataObject;

    // pre-allocate the memory
    //  sCardList.Alloc (100 * m_pModel->GetCardCount ());
    //  sTempList.Alloc (100 * m_pModel->GetCardCount ());

    // Write the text to the clipboard
    if (wxTheClipboard->Open ()) {
        wxTheClipboard->Clear ();

        for (iCard = 0; iCard < m_pModel->GetCardCount (); iCard++) {
            // 	  sTempList.Printf ("%s%s\n", sCardList.c_str (), m_pModel->GetCardList ()->Item (iCard).Item (0).c_str ());
            // 	  sCardList = sTempList;
            sCardList += m_pModel->GetCardList ()->Item (iCard).Item (0);
            sCardList.Append (wxT ("\n"));
        }
        // This data object is held by the clipboard,
        // so we don't want to delete it here
        pDataObject = new wxTextDataObject(sCardList);
        //     pDataObject->SetText (sCardList);
        wxTheClipboard->SetData (pDataObject);
        wxTheClipboard->Close ();
    } else {
        wxLogError (wxT ("Unable to access the clipboard."));
        // that data object becomes useless
        //delete pDataObject;
    }
}


void
BrowserLibraryCardGrid::OnPopupRemove (wxCommandEvent& WXUNUSED (event))
{
    long lCardRef;
    wxArrayLong oIndexArray;
    wxArrayString oNameArray;

    SetCursor (*wxHOURGLASS_CURSOR);

    for (unsigned int i = 0; i < MyGetSelectedRows ().Count (); i++) {
        GetCellValue (MyGetSelectedRows ().Item (i), GetNumberCols () -1).ToLong (&lCardRef);
        oIndexArray.Add (lCardRef);
        oNameArray.Add (GetCellValue (MyGetSelectedRows ().Item (i), 3));
    }

    for (unsigned int i = 0; i < oIndexArray.Count (); i++) {
        m_pTab->m_pController->RemoveCard (oIndexArray.Item (i), oNameArray.Item (i), FALSE);
    }

    m_pTab->UpdateView ();
}


