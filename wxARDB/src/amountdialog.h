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

#ifndef _amountdialoh_h
#define _amountdialoh_h

#include <wx/wx.h>
#include <wx/spinctrl.h>

class AmountDialog : public wxDialog
{
public:
    AmountDialog (wxArrayString *pNameArray, wxArrayInt *pAmountArray);
    ~AmountDialog () { };

private:
    wxArrayPtrVoid  m_oSpinnerArray;

    wxArrayInt     *m_pAmountArray;
    wxArrayString  *m_pNameArray;
    wxButton       *m_pCancelButton;
    wxButton       *m_pOKButton;

    void OnCancelButtonClick (wxCommandEvent& WXUNUSED (event));
    void OnOKButtonClick (wxCommandEvent& WXUNUSED (event));

    enum {
        ID_OK_BUTTON = wxID_HIGHEST + 1,
        ID_CANCEL_BUTTON
    };


    DECLARE_EVENT_TABLE()
};



#endif
