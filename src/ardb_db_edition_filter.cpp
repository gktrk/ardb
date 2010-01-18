/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2007 Sylvain Gogel
 *  meshee.knight@gmail.com
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

/**
 * this lib encapsulate all Database manipulation done by the "File/VTES Sets..."
 * mainly used in editionsdialog.cpp.
 */
 
#include "ardb_db_edition_filter.h"

//
// Retrieve all Used Sets for VTES Sets Filter
//
wxArrayString
ardb_db_ef_get_inuse_editions() {
	wxArrayString m_oUseEditionsArray;
	
	Database *pDatabase = Database::Instance ();
  	//if (!pDatabase) return;
  	RecordSet *pResult;

  	// Get the editions in use
  	pResult = pDatabase->Query (wxT ("SELECT DISTINCT full_name FROM cards_sets WHERE record_num IN (SELECT DISTINCT release_set FROM cards_library) AND full_name NOT LIKE 'Proxy%' ORDER BY release_date DESC"), NULL);
  	if (pResult) {
      	for (unsigned int i = 0; i < pResult->GetCount (); i++) {
	  		m_oUseEditionsArray.Add (pResult->Item (i).Item (0));
		}
    }
	
	return m_oUseEditionsArray;
}

//
// Retrieve all Ignored Sets for VTES Sets Filter
//
wxArrayString
ardb_db_ef_get_ignored_editions() {
	wxArrayString m_oIgnoredEditions;
	
	Database *pDatabase = Database::Instance ();
  	//if (!pDatabase) return;
  	RecordSet *pResult;

	pResult = pDatabase->Query (wxT ("SELECT DISTINCT full_name FROM cards_sets WHERE record_num IN (SELECT DISTINCT release_set FROM cards_library_ignored) AND full_name NOT LIKE 'Proxy%' ORDER BY release_date DESC"), NULL);
  	if (pResult) {
		for (unsigned int i = 0; i < pResult->GetCount (); i++) {
	  		m_oIgnoredEditions.Add (pResult->Item (i).Item (0));
		}
    }
	
	return m_oIgnoredEditions;
}

//
// Move named editions from InUse to Ignored tables
//
void
ardb_db_ef_move(const wxArrayString& inUseEditions, const wxArrayString& ignoreEditions) {
	
	Database *pDatabase = Database::Instance ();
	if (!pDatabase) return;
  	wxString sQuery;
  	
  	//  pDatabase->ToggleVerbose ();
  	pDatabase->Query (wxT ("BEGIN TRANSACTION;"));
  	
  	// In use --> Ignored
  	for (int i = 0; i < ignoreEditions.GetCount(); i++) {
		// Move crypt cards
      	sQuery.Printf (wxT ("INSERT OR REPLACE INTO cards_crypt_ignored "
			  "SELECT * FROM cards_crypt "
			  "WHERE release_set IN "
			  "(SELECT record_num FROM cards_sets"
			  " WHERE full_name = '%s' OR full_name = 'Proxy %s')"),
		      ignoreEditions.Item(i).c_str(),
		      ignoreEditions.Item(i).c_str());		     
      	pDatabase->Query (sQuery);
      
      	sQuery.Printf (wxT ("DELETE FROM cards_crypt "
			  "WHERE release_set IN "
			  "(SELECT record_num FROM cards_sets"
			  " WHERE full_name = '%s' OR full_name = 'Proxy %s')"),
		      ignoreEditions.Item(i).c_str(),
		      ignoreEditions.Item(i).c_str());		      
      	pDatabase->Query (sQuery);
      
      	// Move library cards
      	sQuery.Printf (wxT ("INSERT OR REPLACE INTO cards_library_ignored "
			  "SELECT * FROM cards_library "
			  "WHERE release_set IN "
			  "(SELECT record_num FROM cards_sets"
			  " WHERE full_name = '%s' OR full_name = 'Proxy %s')"),
		      ignoreEditions.Item(i).c_str(),
		      ignoreEditions.Item(i).c_str());
		pDatabase->Query (sQuery);
      
      	sQuery.Printf (wxT ("DELETE FROM cards_library "
			  "WHERE release_set IN "
			  "(SELECT record_num FROM cards_sets"
			  " WHERE full_name = '%s' OR full_name = 'Proxy %s')"),
		      ignoreEditions.Item(i).c_str(),
		      ignoreEditions.Item(i).c_str());
		      
      	pDatabase->Query (sQuery);
	}
    
	// Ignored --> In use
  	for (int i = 0; i < inUseEditions.GetCount(); i++) {
		// Move crypt cards
      	sQuery.Printf (wxT ("INSERT OR REPLACE INTO cards_crypt "
			  "SELECT * FROM cards_crypt_ignored "
			  "WHERE release_set IN "
			  "(SELECT record_num FROM cards_sets"
			  " WHERE full_name = '%s' OR full_name = 'Proxy %s')"),
		      inUseEditions.Item(i).c_str(),
		      inUseEditions.Item(i).c_str());
      	pDatabase->Query (sQuery);
      
      	sQuery.Printf (wxT ("DELETE FROM cards_crypt_ignored "
			  "WHERE release_set IN "
			  "(SELECT record_num FROM cards_sets"
			  " WHERE full_name = '%s' OR full_name = 'Proxy %s')"),
		      inUseEditions.Item(i).c_str(),
		      inUseEditions.Item(i).c_str());
      	pDatabase->Query (sQuery);
      
      	// Move library cards
      	sQuery.Printf (wxT ("INSERT OR REPLACE INTO cards_library "
			  "SELECT * FROM cards_library_ignored "
			  "WHERE release_set IN "
			  "(SELECT record_num FROM cards_sets"
			  " WHERE full_name = '%s' OR full_name = 'Proxy %s')"),
		      inUseEditions.Item(i).c_str(),
		      inUseEditions.Item(i).c_str());
      	pDatabase->Query (sQuery);
      	
      	sQuery.Printf (wxT ("DELETE FROM cards_library_ignored "
			  "WHERE release_set IN "
			  "(SELECT record_num FROM cards_sets"
			  " WHERE full_name = '%s' OR full_name = 'Proxy %s')"),
		      inUseEditions.Item(i).c_str (),
		      inUseEditions.Item(i).c_str ());
		      
		pDatabase->Query (sQuery);
    }
    
  	pDatabase->Query (wxT ("COMMIT TRANSACTION;"));
  	pDatabase->Query (wxT ("VACUUM"));
  	//  pDatabase->ToggleVerbose ();
}

//
// Conveniant method to make all editions in use
//
void
ardb_db_ef_reset() {
	wxArrayString inUse; //will get all Editions
	wxArrayString ignore; //will stay empty
	
	//Get ignored editions
	wxArrayString currentlyIgnored = ardb_db_ef_get_ignored_editions();
	wxArrayString currentlyInUse = ardb_db_ef_get_inuse_editions();	
	
	//put them in inUseEditions
	for (int i = 0; i < currentlyIgnored.GetCount(); i++)  {
		inUse.Add(currentlyIgnored.Item(i));
	}
	
	//MoveEditions
	ardb_db_ef_move(inUse, ignore);
}
