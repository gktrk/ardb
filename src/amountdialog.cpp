/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2002 Francois Gombault
 *  gombault.francois@wanadoo.fr
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

#include "amountdialog.h"


BEGIN_EVENT_TABLE(AmountDialog, wxDialog)
    EVT_BUTTON (ID_OK_BUTTON, AmountDialog::OnOKButtonClick)
    EVT_BUTTON (ID_CANCEL_BUTTON, AmountDialog::OnCancelButtonClick)
END_EVENT_TABLE()


AmountDialog::AmountDialog (wxArrayString *pNameArray, wxArrayInt *pAmountArray) :
    wxDialog (0, -1, wxT ("Please set count"), wxDefaultPosition, wxDefaultSize),
    m_oSpinnerArray (),
    m_pAmountArray (pAmountArray),
    m_pNameArray (pNameArray)
{
    unsigned int uiCount = pNameArray->GetCount ();
    if (pAmountArray->GetCount () < uiCount) uiCount = pAmountArray->GetCount ();

    // create sizer
    wxFlexGridSizer *pGridSizer = new wxFlexGridSizer (2, 5, 5);

    SetAutoLayout (TRUE);
    SetSizer (pGridSizer);

    for (unsigned int ui = 0; ui < uiCount; ui++) {
        wxStaticText *pLabel = new wxStaticText (this, -1, pNameArray->Item (ui));
        pGridSizer->Add (pLabel);
        wxString sCount;
        sCount <<  pAmountArray->Item (ui);
        wxSpinCtrl *pSpinny = new wxSpinCtrl (this, -1, sCount);
        pGridSizer->Add (pSpinny);
        m_oSpinnerArray.Add ((void *) pSpinny);
    }

    m_pOKButton = new wxButton (this, ID_OK_BUTTON, wxT ("OK"));
    pGridSizer->Add (m_pOKButton);
    m_pCancelButton = new wxButton (this, ID_CANCEL_BUTTON, wxT ("Cancel"));
    pGridSizer->Add (m_pCancelButton);
    pGridSizer->Fit (this);
}


void
AmountDialog::OnOKButtonClick (wxCommandEvent&  WXUNUSED (event))
{
    m_pAmountArray->Clear ();
    for (unsigned int ui = 0; ui < m_oSpinnerArray.GetCount (); ui++) {
        wxSpinCtrl *pSpinny =  (wxSpinCtrl *) m_oSpinnerArray.Item (ui);
        m_pAmountArray->Add (pSpinny->GetValue ());
    }

    EndModal (1);
}


void
AmountDialog::OnCancelButtonClick (wxCommandEvent&  WXUNUSED (event))
{
    EndModal (0);
}
