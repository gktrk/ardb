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

#ifndef _browsercryptcontroller_h
#define _browsercryptcontroller_h

#include "interfacedata.h"


typedef enum {
    CRYPT_FILTER_ADD,
    CRYPT_FILTER_REMOVE,
    CRYPT_FILTER_KEEP
} CryptFilterMode;


class BrowserCryptFilter;
class BrowserCryptModel;

class BrowserCryptController
{
public:

    BrowserCryptController (BrowserCryptModel *pModel);

    void RemoveCard (long lCard, wxString &sName, bool bUpdateView = TRUE);
    CryptFilterMode GetFilterMode ();
    void SetFilterMode (CryptFilterMode eMode);
    void UpdateModel (BrowserCryptFilter *pFilter);

private:
    CryptFilterMode    m_eMode;
    BrowserCryptModel *m_pModel;
    wxString           m_sFilterSummary;
    wxString           m_sWhereClause;

    void AddToWhereClause (const wxString &sExpression);
    void AddToFilterSummary (const wxString &sExpression);
    void DeleteFilterSummary ();
    void DeleteWhereClause ();
    void ProcessCapacities (BrowserCryptFilter *pFilter);
    void ProcessClans (BrowserCryptFilter *pFilter);
    void ProcessDisciplines (BrowserCryptFilter *pFilter);
    void ProcessEditions (BrowserCryptFilter *pFilter);
    void ProcessFeather (BrowserCryptFilter *pFilter);
    void ProcessGroups (BrowserCryptFilter *pFilter);
    void ProcessRarity (BrowserCryptFilter *pFilter);
    void ProcessSects (BrowserCryptFilter *pFilter);
    void ProcessSpecials (BrowserCryptFilter *pFilter);
    void ProcessTitles (BrowserCryptFilter *pFilter);

};

#include "browsercryptmodel.h"

#endif
