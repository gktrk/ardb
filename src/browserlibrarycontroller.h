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

#ifndef _browserlibrarycontroller_h
#define _browserlibrarycontroller_h

#include "interfacedata.h"


typedef enum
  {
    LIBRARY_FILTER_ADD,
    LIBRARY_FILTER_REMOVE,
    LIBRARY_FILTER_KEEP
  } LibraryFilterMode;


class BrowserLibraryFilter;
class BrowserLibraryModel;

class BrowserLibraryController 
{
 public:

  BrowserLibraryController (BrowserLibraryModel *pModel);

  void RemoveCard (long lCard, wxString &sName, bool bUpdateView = TRUE);
  LibraryFilterMode GetFilterMode ();
  void SetFilterMode (LibraryFilterMode eMode);
  void UpdateModel (BrowserLibraryFilter *pFilter);

 private:
  LibraryFilterMode    m_eMode;

  BrowserLibraryModel *m_pModel;

  wxString             m_sFilterSummary;
  wxString             m_sWhereClause;

  void AddToFilterSummary (const wxString &sExpression);
  void AddToWhereClause (const wxString &sExpression);
  void DeleteFilterSummary ();
  void DeleteWhereClause ();
  void ProcessClans (BrowserLibraryFilter *pFilter);
  void ProcessDisciplines (BrowserLibraryFilter *pFilter);
  void ProcessEditions (BrowserLibraryFilter *pFilter);
  void ProcessOthers (BrowserLibraryFilter *pFilter);
  void ProcessRarity (BrowserLibraryFilter *pFilter);
  void ProcessTexts (BrowserLibraryFilter *pFilter);
  void ProcessTitles (BrowserLibraryFilter *pFilter);
  void ProcessTypes (BrowserLibraryFilter *pFilter);

};

#include "browserlibrarymodel.h"

#endif
