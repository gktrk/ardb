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

#ifndef _updater_h
#define _updater_h

/*
** This updater requires the CSV files found on White-Wolf's website
** http://www.white-wolf.com/VTES/downloads/vtescsv.zip
**
*/

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/scrolwin.h>
#include <wx/protocol/http.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/mstream.h>

#include "database.h"

// This class handles the automatic database creation and update.

// Buffer read size:
// this is almost twice as much as needed, the longest line is around 640.
#define BUFFER_SIZE 1000



class Updater : public wxDialog
{
 public:
  // Singleton access
  static Updater *Instance ();
  static void DeleteInstance ();


  int DoUpdate ();
  static void decodeCSV (wxInputStream *file, char sep, char quote, int maxrecords, int *numfields, wxArrayString *pResult, bool bSkipFirstLine);

  protected:
  // Constructor & destructor
  Updater ();
  ~Updater ();

 private:
  // Singleton pointer
  static Updater   *spInstance;

  bool              m_bUpdating;
  wxArrayString     m_oDisciplinesArray;
  wxButton         *m_pOKButton;
  wxScrolledWindow *m_pScrolledWindow;
  wxSizer          *m_pScrolledSizer;
  wxTextCtrl       *m_pStatusLabel;
  wxString          m_sZipFile;


  int FetchCSVFiles ();
  int LoadDisciplinesFromCSV ();
  int LoadTableFromCSV (wxString sTable, wxString sCSVFile, int iNulls);
  void Log (wxString sText);
  int UpdateDatabaseFromCSV ();
  static wxChar MakeAscii(wxChar c);

};

#endif
