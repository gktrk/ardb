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

#ifndef _deckcrypttab_h
#define _deckcrypttab_h

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>

#include "cardtext.h"
#include "database.h"

class DeckModel;
class DeckWindow;

class DeckCryptTab : public wxPanel
{
  friend class DeckController;
  friend class DeckWindow;

 public:
  void Update ();

 private:
  DeckCryptTab (DeckModel *pModel, wxNotebook *pParent);
  ~DeckCryptTab ();

  long            m_alGroups[2];

  bool            m_bReverseSortOrder;
  bool            m_bNoEvents;

  // the cycling boundaries
  int             m_iCycleCounter;
  int             m_iCycleLowerValue;
  int             m_iCycleUpperValue;

  long            m_lSelectedVampire;
  long            m_lSelectedVampireAmount;

  // the complete list of cards for the card picker combo box
  wxArrayString   m_oArrayOfNames;
  RecordSet       m_oCardList;
  RecordSet       m_oNameList;
  wxMenu          m_oPopupMenu;

  wxButton       *m_pAddButton;
  wxTextCtrl     *m_pCardInput;
  wxListBox      *m_pCardPicker;
  CardText       *m_pCardText;
  wxSpinCtrl     *m_pHappyDeckSizeSpin;
  wxSpinCtrl     *m_pHappyDisciplineSpin;
  wxTextCtrl     *m_pHappyFamiliesText;
  wxSpinCtrl     *m_pHappyMasterSpin;
  DeckModel      *m_pModel;
  wxNotebook     *m_pParent;
  wxComboBox     *m_pSetPicker;
  wxStaticText   *m_pVampireCountLabel;
  wxListView     *m_pVampireList;

  wxString        m_sSelectedVampireName;

  unsigned int    m_uiColumnCount;
  unsigned int    m_uiSortColumn;

  static const wxString s_sCardNamesQuery;

  void AddToDeck ();

  void FillCardPicker ();

  void OnAddButtonClick (wxCommandEvent& WXUNUSED (event));
  void OnCardInputChanged (wxCommandEvent& WXUNUSED (event));
  void OnCardInputEnter (wxCommandEvent& WXUNUSED (event));
  void OnCardPickerDoubleClick (wxCommandEvent& WXUNUSED (event));
  void OnCardPickerSelection (wxCommandEvent& WXUNUSED (event));
  void OnColumnClick (wxListEvent &event);
  void OnHappyDiscChange (wxSpinEvent& WXUNUSED (event));
  void OnHappyMasterChange (wxSpinEvent& WXUNUSED (event));
  void OnHappySizeChange (wxSpinEvent& WXUNUSED (event));
  void OnPopupLess (wxCommandEvent& WXUNUSED (event));
  void OnPopupMore (wxCommandEvent& WXUNUSED (event));
  void OnPopupRemoveAll (wxCommandEvent& WXUNUSED (event));
  void OnVampireActivate (wxListEvent& WXUNUSED (event));
  void OnVampireListKeyDown (wxListEvent &event);
  void OnVampireRightClick (wxListEvent &event);
  void OnVampireSelect (wxListEvent &event);

  void SetVampireCount (long lCount, long lMin, long lMax, double dAvg);
  void UpdateCardList ();
  void UpdateHappyFamilies ();

  static int wxCALLBACK CompareFunction (long lItem1, long lItem2, long lSortData);

  enum
    {
      ID_VAMPIRE_LIST = wxID_HIGHEST + 1,
      ID_ADD_BUTTON,
      ID_CARD_PICKER,
      ID_SET_PICKER,
      ID_CARD_INPUT,
      ID_POPUP_MORE,
      ID_POPUP_LESS,
      ID_POPUP_REMOVE_ALL,
      ID_HAPPY_SIZE,
      ID_HAPPY_DISC,
      ID_HAPPY_MASTER
    };

  DECLARE_EVENT_TABLE()
};


#endif
