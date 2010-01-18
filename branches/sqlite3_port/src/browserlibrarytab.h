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


#ifndef _browserlibrarytab_h
#define _browserlibrarytab_h

#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <wx/grid.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>

#include "browserlibraryfilter.h"
#include "cardtext.h"
#include "types.h"

#include "cardviewer.h"

class BrowserLibraryController;
class BrowserLibraryCardGrid; // declared below
class BrowserLibraryFilterGrid; // declared below
class BrowserLibraryModel;

class BrowserLibraryTab : public wxPanel
{
  friend class BrowserLibraryController;
  friend class BrowserLibraryModel;
  friend class BrowserLibraryCardGrid;

private:
  BrowserLibraryTab (BrowserLibraryModel *pModel, BrowserLibraryController *pController, wxNotebook* pParent, unsigned int uiNumber);
  ~BrowserLibraryTab ();

public:
  void Init ();
  void UpdateView ();
  void SetFocus();

private:
  bool               m_bDisplayInventory;
  long               m_lSplitterPosition;

  wxArrayString      m_oArrayOfNames;
  wxMenu             m_oPopupMenu;
  wxSize             m_oTabSize;

  wxButton *         m_pAddButton;
  BrowserLibraryController *m_pController;
  BrowserLibraryFilterGrid *m_pFilterGrid;
  wxButton *         m_pInventoryButton;
  wxButton *         m_pKeepButton;
  BrowserLibraryModel *m_pModel;
  wxButton *         m_pRemoveButton;
  wxButton *         m_pResetButton;
  wxNotebook *       m_pParent;
  wxSizer *          m_pPapaSizer;
  wxSplitterWindow * m_pSplitterWindow;
  wxTextCtrl *       m_pFindText;
  wxStaticText *     m_pCardCountLabel;
  BrowserLibraryCardGrid * m_pCardGrid;
  CardText *         m_pCardText;
  CardViewer*		m_pCardViewer;
    
  unsigned int       m_uiFilterListHeight;
  unsigned int       m_uiNumber;
  unsigned int       m_uiSortColumn;

  void FillCardList ();

  void OnAddButtonClick (wxCommandEvent& WXUNUSED (event));
  void OnFindTextChange (wxCommandEvent& WXUNUSED (event));
  void OnInventoryButtonClick (wxCommandEvent& WXUNUSED (event));
  void OnKeepButtonClick (wxCommandEvent& WXUNUSED (event));
  void OnRemoveButtonClick (wxCommandEvent& WXUNUSED (event));
  void OnResetButtonClick (wxCommandEvent& WXUNUSED (event));
  void OnSplitterMoved (wxSplitterEvent& event);
  void OnTabResize (wxSizeEvent &event);

  void SetCardCount (unsigned int uiCount);
  void SetCardText (long lCardRef = -1);

  void UpdateFilterList ();

  enum
    {
      ID_ADD_BUTTON = wxID_HIGHEST + 1,
      ID_KEEP_BUTTON,
      ID_REMOVE_BUTTON,
      ID_RESET_BUTTON,
      ID_FIND_TEXTCTRL,
      ID_VAMPIRE_GRID,
      ID_SPLITTER_WINDOW,
      ID_INVENTORY_BUTTON
    };

  DECLARE_EVENT_TABLE()
};



class BrowserLibraryFilterGrid : public wxGrid
{
public:
  BrowserLibraryFilterGrid (BrowserLibraryModel *pModel, BrowserLibraryTab *pParent);
  ~BrowserLibraryFilterGrid ();

private:
  wxMenu             m_oFilterPopupMenu;

  BrowserLibraryModel *m_pModel;
  BrowserLibraryTab *  m_pParent;

  void OnFilterPopupHide (wxCommandEvent& WXUNUSED (event));
  void OnFilterPopupMoveLeft (wxCommandEvent& WXUNUSED (event));
  void OnFilterPopupMoveRight (wxCommandEvent& WXUNUSED (event));
  void OnFilterPopupRemove (wxCommandEvent& WXUNUSED (event));
  void OnFilterRightClick (wxGridEvent &event);

  enum
    {
      ID_POPUP_REMOVE = wxID_HIGHEST + 1,
      ID_POPUP_HIDE,
      ID_POPUP_MOVELEFT,
      ID_POPUP_MOVERIGHT
    };

  DECLARE_EVENT_TABLE()
};



class BrowserLibraryCardGrid : public wxGrid
{
public:
  BrowserLibraryCardGrid (BrowserLibraryModel *pModel, BrowserLibraryTab *pTab, wxWindow *pPanel);
  ~BrowserLibraryCardGrid ();

  wxArrayInt MyGetSelectedRows ();

private:
  bool                 m_bReverseSortOrder;

  wxArrayInt           m_oSelection;
  wxMenu               m_oCardPopupMenu;

  BrowserLibraryModel *m_pModel;
  wxWindow *           m_pParent;
  BrowserLibraryTab *  m_pTab;

  unsigned int         m_uiSortColumn;

  void OnCardActivated (wxGridEvent &event);
  void OnCardRightClick (wxGridEvent &event);
  void OnCardSelect (wxGridEvent &event);
  void OnColumnClick (wxGridEvent &event);
  void OnInventoryChange (wxGridEvent& event);
  void OnPopupAddAllToDeck (wxCommandEvent& WXUNUSED (event));
  void OnPopupAddToDeck (wxCommandEvent& WXUNUSED (event));
  void OnPopupCopyAll (wxCommandEvent& WXUNUSED (event));
  void OnPopupRemove (wxCommandEvent& WXUNUSED (event));

  enum
    {
      ID_POPUP_ADD = wxID_HIGHEST + 1,
      ID_POPUP_ADD_ALL,
      ID_POPUP_REMOVE,
      ID_POPUP_COPY_ALL
    };

  DECLARE_EVENT_TABLE()
};

#endif
