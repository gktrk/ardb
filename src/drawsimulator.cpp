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

#include "drawsimulator.h"
#include "deckmodel.h"
#include "interfacedata.h"

#include <stdlib.h>


BEGIN_EVENT_TABLE(DrawSimulator, wxFrame)
    EVT_BUTTON (ID_CLOSE_BUTTON, DrawSimulator::OnButtonClose)
    EVT_BUTTON (ID_DRAW_BUTTON, DrawSimulator::OnButtonDraw)
END_EVENT_TABLE()


DrawSimulator * DrawSimulator::s_pInstance = NULL;

void
DrawSimulator::DeleteInstance ()
{
    if (s_pInstance != NULL) delete s_pInstance;
    s_pInstance = NULL;
}


DrawSimulator *
DrawSimulator::Instance ()
{
    if (s_pInstance == NULL) {
        s_pInstance = new DrawSimulator ();
    }
    return s_pInstance;
}



DrawSimulator::DrawSimulator () :
    wxFrame (0, -1, wxT ("Anarch Revolt - Draw Simulator"), wxDefaultPosition, wxSize (320, 200)),
    m_oDrawnCards (),
    m_oDrawnVampires (),
    m_oRemainingCards (),
    m_oRemainingVampires (),
    m_pDrawnCardsText (NULL),
    m_pDrawnVampiresText (NULL),
    m_pRemainingCardsText (NULL),
    m_pRemainingVampiresText (NULL)
{
    SetIcon (*g_pIcon);

    // create sizer
    wxFlexGridSizer *pGridSizer = new wxFlexGridSizer (2, 5, 5);

    SetAutoLayout (TRUE);
    SetSizer (pGridSizer);

    wxStaticText *pLabel = new wxStaticText (this, -1, wxT ("Uncontrolled region :"));
    pGridSizer->Add (pLabel);
    pLabel = new wxStaticText (this, -1, wxT ("Cards in hand :"));
    pGridSizer->Add (pLabel);

    m_pDrawnVampiresText = new wxTextCtrl (this, -1, wxT (""), wxDefaultPosition, wxSize (250, 130), wxTE_READONLY | wxTE_MULTILINE);
    pGridSizer->Add (m_pDrawnVampiresText);
    m_pDrawnCardsText = new wxTextCtrl (this, -1, wxT (""), wxDefaultPosition, wxSize (250, 130), wxTE_READONLY | wxTE_MULTILINE);
    pGridSizer->Add (m_pDrawnCardsText);

    pLabel = new wxStaticText (this, -1, wxT ("Crypt :"));
    pGridSizer->Add (pLabel);
    pLabel = new wxStaticText (this, -1, wxT ("Library :"));
    pGridSizer->Add (pLabel);

    m_pRemainingVampiresText = new wxTextCtrl (this, -1, wxT (""), wxDefaultPosition, wxSize (250, 130), wxTE_READONLY | wxTE_MULTILINE);
    m_pRemainingVampiresText->SetDefaultStyle (wxTextAttr (wxColour (100, 100, 100)));
    pGridSizer->Add (m_pRemainingVampiresText);
    m_pRemainingCardsText = new wxTextCtrl (this, -1, wxT (""), wxDefaultPosition, wxSize (250, 130), wxTE_READONLY | wxTE_MULTILINE);
    m_pRemainingCardsText->SetDefaultStyle (wxTextAttr (wxColour (100, 100, 100)));
    pGridSizer->Add (m_pRemainingCardsText);

    wxButton *pDrawButton = new wxButton (this, ID_DRAW_BUTTON, wxT ("Redraw"));
    pGridSizer->Add (pDrawButton);
    wxButton *pCloseButton = new wxButton (this, ID_CLOSE_BUTTON, wxT ("Close"));
    pGridSizer->Add (pCloseButton, wxALIGN_RIGHT);

    pGridSizer->Fit (this);
    SetSizeHints (GetSize ().GetWidth (), GetSize ().GetHeight (), GetSize ().GetWidth (), GetSize ().GetHeight ());

    Draw ();
}


DrawSimulator::~DrawSimulator ()
{
    s_pInstance = NULL;
}


void
DrawSimulator::Draw ()
{
    DeckModel *pDeckModel = DeckModel::Instance ();
    int iRandomNumber;
    long lCount;
    wxArrayString oTempArray;
    wxString sName;

    // Vampires
    m_oDrawnVampires.Clear ();
    m_oRemainingVampires.Clear ();

    if (pDeckModel->GetCryptList () &&  pDeckModel->GetCryptCount () > 0) {
        oTempArray.Clear ();
        // Break down the numbered list into a flat list
        for (unsigned int i = 0; i < pDeckModel->GetCryptList ()->GetCount (); i++) {
            for (pDeckModel->GetCryptList ()->Item (i).Item (0).ToLong (&lCount);
                    lCount > 0; lCount--) {
                sName = wxT ("");
                sName << pDeckModel->GetCryptList ()->Item (i).Item (1)
                      << wxT (" ")
                      << pDeckModel->GetCryptList ()->Item (i).Item (2)
                      << wxT (" (")
                      << pDeckModel->GetCryptList ()->Item (i).Item (3)
                      << wxT (")");
                oTempArray.Add (sName);
            }
        }
        // Shuffle the cards into a new list
        for (unsigned int i = oTempArray.GetCount (); i > 0; i--) {
            iRandomNumber = Random (i);
            sName = oTempArray.Item (iRandomNumber);
            m_oRemainingVampires.Add (sName);
            oTempArray.RemoveAt (iRandomNumber);
        }
        // Draw cards
        for (unsigned int i = 0; i < 4  && m_oRemainingVampires.GetCount (); i++) {
            iRandomNumber = Random (m_oRemainingVampires.GetCount ());
            m_oDrawnVampires.Add (m_oRemainingVampires.Item (iRandomNumber));
            m_oRemainingVampires.RemoveAt (iRandomNumber);
        }
        // Display stuff
        m_pDrawnVampiresText->Freeze ();
        m_pDrawnVampiresText->Clear ();
        m_pRemainingVampiresText->Freeze ();
        m_pRemainingVampiresText->Clear ();
        for (unsigned int i = 0; i < 4; i++) {
            if (m_oDrawnVampires.GetCount () > i) {
                m_pDrawnVampiresText->WriteText (m_oDrawnVampires.Item (i));
                m_pDrawnVampiresText->WriteText (wxT ("\n"));
            }
        }
        for (unsigned int i = 0; i < m_oRemainingVampires.GetCount (); i++) {
            m_pRemainingVampiresText->WriteText (m_oRemainingVampires.Item (i));
            m_pRemainingVampiresText->WriteText (wxT ("\n"));
        }
        m_pDrawnVampiresText->ShowPosition (0);
        m_pDrawnVampiresText->Thaw ();
        m_pRemainingVampiresText->ShowPosition (0);
        m_pRemainingVampiresText->Thaw ();
    }

    // Library
    m_oDrawnCards.Clear ();
    m_oRemainingCards.Clear ();

    if (pDeckModel->GetLibraryList () &&  pDeckModel->GetLibraryCount () > 0) {
        oTempArray.Clear ();
        // Break down the numbered list into a flat list
        for (unsigned int i = 0; i < pDeckModel->GetLibraryList ()->GetCount (); i++) {
            for (pDeckModel->GetLibraryList ()->Item (i).Item (0).ToLong (&lCount);
                    lCount > 0; lCount--) {
                oTempArray.Add ( pDeckModel->GetLibraryList ()->Item (i).Item (1));
            }
        }
        // Shuffle the cards into a new list
        for (unsigned int i = oTempArray.GetCount (); i > 0; i--) {
            iRandomNumber = Random (i);
            sName = oTempArray.Item (iRandomNumber);
            m_oRemainingCards.Add (sName);
            oTempArray.RemoveAt (iRandomNumber);
        }
        // Draw cards
        for (unsigned int i = 0; i < 7  && m_oRemainingCards.GetCount (); i++) {
            iRandomNumber = Random (m_oRemainingCards.GetCount ());
            m_oDrawnCards.Add (m_oRemainingCards.Item (iRandomNumber));
            m_oRemainingCards.RemoveAt (iRandomNumber);
        }
        // Display stuff
        m_pDrawnCardsText->Freeze ();
        m_pDrawnCardsText->Clear ();
        m_pRemainingCardsText->Freeze ();
        m_pRemainingCardsText->Clear ();
        for (unsigned int i = 0; i < 7; i++) {
            if (m_oDrawnCards.GetCount () > i) {
                m_pDrawnCardsText->WriteText (m_oDrawnCards.Item (i));
                m_pDrawnCardsText->WriteText (wxT ("\n"));
            }
        }
        for (unsigned int i = 0; i < m_oRemainingCards.GetCount (); i++) {
            m_pRemainingCardsText->WriteText (m_oRemainingCards.Item (i));
            m_pRemainingCardsText->WriteText (wxT ("\n"));
        }
        m_pDrawnCardsText->ShowPosition (0);
        m_pDrawnCardsText->Thaw ();
        m_pRemainingCardsText->ShowPosition (0);
        m_pRemainingCardsText->Thaw ();
    }
}


void
DrawSimulator::OnButtonClose (wxCommandEvent & WXUNUSED (event))
{
    Close ();
}


void
DrawSimulator::OnButtonDraw (wxCommandEvent & WXUNUSED (event))
{
    Draw ();
}


int
DrawSimulator::Random (int iMax)
{
    return (int) (iMax * (rand() * 1.0) / RAND_MAX);
}
