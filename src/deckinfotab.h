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

#ifndef _deckinfotab_h
#define _deckinfotab_h

#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/wx.h>

class DeckModel;
class DeckWindow;

class DeckInfoTab : public wxPanel
{
  friend class DeckController;
  friend class DeckWindow;

private:
  DeckInfoTab (DeckModel *pModel, wxNotebook *pParent);
  ~DeckInfoTab ();

public:

private:
  bool            m_bNumbFields;
  wxStaticText   *m_pLabelTime;
  DeckModel      *m_pModel;
  wxNotebook     *m_pParent;
  wxTextCtrl     *m_pTextAuthor;
  wxTextCtrl     *m_pTextComments;
  wxTextCtrl     *m_pTextName;
  
  void OnTextAuthorChanged (wxCommandEvent& WXUNUSED (event));
  void OnTextCommentsChanged (wxCommandEvent& WXUNUSED (event));
  void OnTextNameChanged (wxCommandEvent& WXUNUSED (event));

  void Update ();

  enum
    {
      ID_TEXT_NAME = wxID_HIGHEST + 1,
      ID_TEXT_AUTHOR,
      ID_TEXT_COMMENTS
    };
  
  DECLARE_EVENT_TABLE()
};

#include "deckmodel.h"

#endif
