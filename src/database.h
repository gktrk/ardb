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


#ifndef _database_h_
#define _database_h_

#include <sqlite.h>
#include <wx/dynarray.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/utils.h>

#include "types.h"

class Database 
{
 public:
  // Singleton access
  static Database *Instance ();
  static void DeleteInstance ();

  // database creation & update
  int CreateBlankDatabase ();
  int CreateCoreTables ();
  int CreateDeckTables ();
  int CreateInventoryTables ();
  int CreateViews ();
  int DeleteCoreTables ();
  int DeleteDeckTables ();
  int DeleteInventoryTables ();
  wxString GetDatabaseDirectory () { return m_sDatabaseDirectory; }
  bool IsVerbose () { return m_bVerbose; }

  RecordSet *Query (const wxString &sQuery, RecordSet *pDest = NULL, bool bSilent = FALSE);

  void ToggleVerbose () { m_bVerbose = !m_bVerbose; }

 protected:
  // Constructor & destructor
  Database ();
  ~Database ();

 private:
  // Singleton pointer
  static Database *spInstance;

  bool      m_bReady;
  bool      m_bMustUpdate;
  bool      m_bVerbose;
  RecordSet m_oResultSet;
  sqlite   *m_pCardsDB;
  wxString  m_sDatabaseDirectory;
  wxString  m_sDatabaseFileName;
  

  static int Callback (void *pUserData, int argc, char **argv, char **azColName);
  

};

#endif
