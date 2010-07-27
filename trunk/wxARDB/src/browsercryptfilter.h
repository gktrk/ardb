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


#ifndef _browsercryptfilter_h
#define _browsercryptfilter_h

#include <wx/wx.h>
#include <wx/listctrl.h>


#include "cardtext.h"
#include "interfacedata.h"


class BrowserCryptController;

// This class is the dialog that will allow to filter the vampires
// The magic that happens when the user clicks the OK button is
// in the CryptBrowserController class
class BrowserCryptFilter: public wxDialog
{
    friend class BrowserCryptController;

public:
    BrowserCryptFilter(BrowserCryptController *pController);

    void SetDialogTitle (int iMode);
    void Reset ();

private:
    bool            m_bNoEvents;

    // the cycling boundaries
    int             m_iCycleCounter;
    int             m_iCycleLowerValue;
    int             m_iCycleUpperValue;

    // The ideal vampire
    long            m_lFeatherRef;

    // the complete list of cards for the card picker combo box
    wxArrayString   m_oArrayOfNames;
    RecordSet       m_oCardList;
    ComboArray      m_oDisciplinesCombos;
    RecordSet       m_oNameList;

    wxCheckBox   *m_apSpecials[100];
    wxCheckBox   *m_pAnarchCheckbox;
    wxCheckBox   *m_pBlackHandCheckbox;
    wxButton     *m_pCancelButton;
    wxCheckBox   *m_pCapEqualCheckbox;
    wxCheckBox   *m_pCapGreaterCheckbox;
    wxCheckBox   *m_pCapLesserCheckbox;
    wxSlider     *m_pCapEqualSlider;
    wxSlider     *m_pCapGreaterSlider;
    wxSlider     *m_pCapLesserSlider;
    wxTextCtrl   *m_pCardInput;
    wxListBox    *m_pCardPicker;
    CardText     *m_pCardText;
    wxListView   *m_pClanList;
    wxButton     *m_pClearButton;
    BrowserCryptController *m_pController;
    wxListView   *m_pEditionList;
    wxSlider     *m_pFeatherCapSlider;
    wxCheckBox   *m_pFeatherGrpCheckbox;
    wxListView   *m_pGroupList;
    wxButton     *m_pOKButton;
    wxTextCtrl   *m_pOtherText;
    wxListView   *m_pRarityList;
    wxListView   *m_pSectList;
    wxCheckBox   *m_pSeraphCheckbox;
    wxListView   *m_pTitleList;
    wxCheckBox   *m_pHave;
    wxCheckBox   *m_pHaveOrWant;
    wxTextCtrl   *m_pCardNameText;


    // helper functions to build the interface
    wxSizer *BuildDiscipline (unsigned int uiDisciplineNumber, wxSizer *pContainer, wxWindow *pWindow);
    void BuildSpecial (unsigned int uiSpecialNumber, wxSizer *pContainer, wxWindow *pWindow);
    void CreateOtherBox(wxPanel *pPanel, wxSizer *pSizer,
  			  InterfaceData *pUIData);
    void CreateSpecialBox(wxPanel *pPanel, wxSizer *pSizer,
			  InterfaceData *pUIData);

    void CreateCapacityBox(wxPanel *pPanel, wxSizer *pSizer,
		      InterfaceData *pUIData);

    void FillCardPicker ();


    // callbacks
    void OnCancelButtonClick (wxCommandEvent& WXUNUSED (event));
    void OnCapEqualScroll (wxScrollEvent& WXUNUSED (event));
    void OnCapGreaterScroll (wxScrollEvent& WXUNUSED (event));
    void OnCapLesserScroll (wxScrollEvent& WXUNUSED (event));
    void OnCardInputChanged (wxCommandEvent& WXUNUSED (event));
    void OnCardPickerSelection (wxCommandEvent& WXUNUSED (event));
    void OnClearButtonClick (wxCommandEvent& WXUNUSED (event));
    void OnOKButtonClick (wxCommandEvent& WXUNUSED (event));

    enum {
        ID_OK_BUTTON = wxID_HIGHEST + 1,
        ID_CANCEL_BUTTON,
        ID_CLEAR_BUTTON,
        ID_CAP_LESS_CHECKBOX,
        ID_CAP_LESS_SCALE,
        ID_CAP_EQ_CHECKBOX,
        ID_CAP_EQ_SCALE,
        ID_CAP_MORE_CHECKBOX,
        ID_CAP_MORE_SCALE,
        ID_CARD_INPUT,
        ID_CARD_PICKER
    };

    DECLARE_EVENT_TABLE()
};

#include "browsercryptcontroller.h"


#endif
