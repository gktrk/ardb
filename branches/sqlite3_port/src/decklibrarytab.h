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

#ifndef _decklibrarytab_h
#define _decklibrarytab_h

#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/wx.h>

#include "cardtext.h"
#include "database.h"

class DeckModel;
class DeckWindow;

class MyTreeCtrl; // defined below

class DeckLibraryTab : public wxPanel
{
  friend class DeckController;
  friend class DeckWindow;

private:
  DeckLibraryTab (DeckModel *pModel, wxNotebook *pParent);
  ~DeckLibraryTab ();

public:

  void Update ();
  void ShowPercentage();

private:
  bool            m_bFuzzySelect;
  bool            m_bNoEvents;
  bool			  m_bShowPercentage;
  
  // the cycling boundaries
  int             m_iCycleCounter;
  int             m_iCycleLowerValue;
  int             m_iCycleUpperValue;

  long            m_lAmount;
  long            m_lSelectedCard;
  long            m_lSelectedCardAmount;

  // the complete list of cards for the card picker combo box
  wxArrayString   m_oArrayOfNames;
  RecordSet       m_oCardList;
  RecordSet       m_oNameList;
  wxMenu          m_oPopupMenu;
  wxTreeItemId    m_oRootId;

  wxButton       *m_pAddButton;
  wxTextCtrl     *m_pCardInput;
  wxListBox      *m_pCardPicker;
  CardText       *m_pCardText;
  DeckModel      *m_pModel;
  wxNotebook     *m_pParent;
  wxComboBox     *m_pSetPicker;
  wxTextCtrl     *m_pStatsText;
  MyTreeCtrl     *m_pTree;

  wxString        m_sSelectedCardName;

  static const wxString s_sCardNamesQuery;


  wxTreeItemId AddOrUpdateTreeItem (wxTreeItemId oParent, wxString& sLabel, long lNumber = 1, float fHandPre = -1.0, long lCardRef = -1, bool bExpand = TRUE);
  void AddToDeck ();
  void AddToTree (wxString sType, wxString sName, wxString sSet, long lNumber, long lCardRef, float fHandPre);
  wxArrayLong * DeleteBranch (wxTreeItemId oItemId, bool bFirstCall = TRUE);

  void FillCardPicker ();

  void LessOfAnItem (wxTreeItemId oItemId, long lCount);
  void MoreOfAnItem (wxTreeItemId oItemId, long lCount);

  void OnAddButtonClick (wxCommandEvent& WXUNUSED (event));
  void OnCardInputChanged (wxCommandEvent& WXUNUSED (event));
  void OnCardInputEnter (wxCommandEvent& WXUNUSED (event));
  void OnCardPickerDoubleClick (wxCommandEvent& WXUNUSED (event));
  void OnCardPickerSelection (wxCommandEvent& WXUNUSED (event));
  void OnPopupFiveLess (wxCommandEvent& WXUNUSED (event));
  void OnPopupFiveMore (wxCommandEvent& WXUNUSED (event));
  void OnPopupLess (wxCommandEvent& WXUNUSED (event));
  void OnPopupMore (wxCommandEvent& WXUNUSED (event));
  void OnPopupRemoveAll (wxCommandEvent& WXUNUSED (event));
  void OnTreeItemActivate (wxTreeEvent& WXUNUSED (event));
  void OnTreeKeyDown (wxTreeEvent &event);
  void OnTreeRightClicked (wxTreeEvent &event);
  void OnTreeSelect (wxTreeEvent &event);

  void UpdateTotalCardCount ();
  void UpdateCardList ();
  float HandPercentage(long deckCount, long lNumber);
  void UpdateStats ();
  


  enum
    {
      ID_CARD_PICKER = wxID_HIGHEST + 1,
      ID_SET_PICKER,
      ID_CARD_INPUT,
      ID_ADD_BUTTON,
      ID_TREE,
      ID_POPUP_5_MORE,
      ID_POPUP_5_LESS,
      ID_POPUP_MORE,
      ID_POPUP_LESS,
      ID_POPUP_REMOVE_ALL
    };
  
  DECLARE_EVENT_TABLE()
};


// this tree control sorts items alphabetically 
// IGNORING the first word of the label (which should be the card count in this case)
class MyTreeCtrl : public wxTreeCtrl
{
 public:
  MyTreeCtrl () { }
  MyTreeCtrl (wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT ("listCtrl")) : wxTreeCtrl (parent, id, pos, size, style, validator, name), m_iFrozen (0) { }

#ifdef __WXGTK__  
  void Freeze () { m_iFrozen++; }
  void Refresh (bool eraseBackground = TRUE, const wxRect* rect = NULL) { if (!m_iFrozen) wxTreeCtrl::Refresh (eraseBackground, rect); }
  void Thaw () { if (m_iFrozen > 0) m_iFrozen--; Refresh(); }
#endif

 protected:
  int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);

 private:
  int m_iFrozen;

  // NB: due to an ugly wxMSW hack you _must_ use DECLARE_DYNAMIC_CLASS()
  //     if you want your overloaded OnCompareItems() to be called.
  //     OTOH, if you don't want it you may omit the next line - this will
  //     make default (alphabetical) sorting much faster under wxMSW.
  DECLARE_DYNAMIC_CLASS(MyTreeCtrl)
};


class MyTreeItemData : public wxTreeItemData
{
public:
  MyTreeItemData (long lValue) : m_lStuff (lValue) { }

  long GetValue () { return m_lStuff; }

private:
  long m_lStuff;
};



#endif
