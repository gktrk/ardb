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

#ifndef _inventorymodel_h_
#define _inventorymodel_h_

#include "types.h"

#include <wx/string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

class InventoryModel
{
public:
    // Singleton access
    static InventoryModel *Instance ();
    static void DeleteInstance ();

    // File operations
    bool ExportToCSV ();
    bool ExportToHTML ();
    bool ExportToText ();
    bool ExportToXML ();
    bool ExportWithXSL (wxString &sFileName, wxString *pXSL);
    bool ImportFromCSV ();
    bool ImportFromXML ();
    bool ImportFromXML (wxString &sFileName);

    void SetHaveCrypt (long lRef, long lAmount);
    void SetHWSCryptName (wxString sName, wxString sSet, bool bAdvanced,
                          long lHave, long lWant, long lSpare);
    void SetSpareCrypt (long lRef, long lAmount);
    void SetWantCrypt (long lRef, long lAmount);
    void SetHaveLibrary (long lRef, long lAmount);
    void SetHWSLibraryName (wxString sName, wxString sSet,
                            long lHave, long lWant, long lSpare);
    void SetSpareLibrary (long lRef, long lAmount);
    void SetWantLibrary (long lRef, long lAmount);

protected:
    // Constructor & destructor
    InventoryModel ();
    ~InventoryModel ();

private:
    // Singleton pointer
    static InventoryModel   *spInstance;

    RecordSet               m_oCryptList;
    RecordSet               m_oLibraryList;

    wxString                m_sName;

    long GetHaveCrypt (long lRef);
    long GetSpareCrypt (long lRef);
    long GetWantCrypt (long lRef);
    long GetHaveLibrary (long lRef);
    long GetSpareLibrary (long lRef);
    long GetWantLibrary (long lRef);

    //// XML stuff that needs wrapping
    xmlNodePtr my_xmlNewChild (xmlNodePtr parent,
                               xmlNsPtr ns,
                               wxString sName,
                               wxString sContent);
    xmlAttrPtr my_xmlNewProp (xmlNodePtr node,
                              wxString sName,
                              wxString sValue);
};


#endif
