/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2002 Francois Gombault
 *  gombault.francois@wanadoo.fr
 *
 *  contributors:
 *    meshee.knight@gmail.com
 *  
 *  Official project page: http://code.google.com/p/ardb/
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

#ifndef _editionsdialog_h
#define _editionsdialog_h


#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/listctrl.h>

#include "database.h"
#include "ardb_db_edition_filter.h"


class EditionsDialog : public wxDialog
{
 public:
  // Singleton access
  static EditionsDialog *Instance ();
  static void DeleteInstance ();

 protected:
  // Constructor & destructor
  EditionsDialog ();
  ~EditionsDialog ();

 private:
  // Singleton pointer
  static EditionsDialog   *spInstance;

  wxArrayString     m_oIgnoreEditionsArray;
  wxArrayString     m_oUseEditionsArray;
  wxListView       *m_pIgnoreEditionsList;
  wxListView       *m_pUseEditionsList;
  


  // callbacks
  void OnCancel (wxCommandEvent& WXUNUSED (event));
  void OnIgnoreButtonClick (wxCommandEvent& WXUNUSED (event));
  void OnOK (wxCommandEvent& WXUNUSED (event));
  void OnUseButtonClick (wxCommandEvent& WXUNUSED (event));

  enum
    {
      ID_USE_BUTTON = wxID_HIGHEST + 1,
      ID_IGNORE_BUTTON
    };
  
  DECLARE_EVENT_TABLE()
};

#endif
