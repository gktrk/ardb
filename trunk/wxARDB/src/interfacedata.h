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

#ifndef _interfacedata_h
#define _interfacedata_h

#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/string.h>

#include "database.h"

extern wxIcon *g_pIcon;
extern wxBitmap *g_pSplashBitmap;

extern wxFrame *g_pMainWindow;

extern unsigned int g_uiGroupCount;

class InterfaceData
{
public:
    // Singleton access
    static InterfaceData *Instance ();
    static void DeleteInstance ();

    wxArrayString *GetClans ()       {
        return &m_oClanArray;
    }
    RecordSet     *GetDisciplines () {
        return &m_oDisciplineRecord;
    }
    wxArrayString *GetEditions ()    {
        return &m_oEditionArray;
    }
    wxString      *GetImageDir ()    {
        return &m_sImagedir;
    }
    RecordSet     *GetLibraryReqs () {
        return &m_oLibraryReqRecord;
    }
    RecordSet     *GetLibraryTexts () {
        return &m_oLibraryTextRecord;
    }
    RecordSet     *GetRarities ()    {
        return &m_oRarityRecord;
    }
    RecordSet     *GetSects ()       {
        return &m_oSectRecord;
    }
    RecordSet     *GetSpecials ()    {
        return &m_oSpecialRecord;
    }
    RecordSet     *GetTitles ()      {
        return &m_oTitleRecord;
    }
    wxArrayString *GetTypes ()       {
        return &m_oTypeArray;
    }
    wxStaticBitmap *MakeStaticBitmap (wxWindow* pParent, wxString& sName);

protected:
    InterfaceData ();
    ~InterfaceData () { }

private:
    // Singleton pointer
    static InterfaceData *spInstance;

    bool      m_bReady;

    wxArrayString   m_oClanArray;
    wxArrayString   m_oEditionArray;
    wxArrayString   m_oSectArray;
    wxArrayString   m_oTypeArray;
    wxString        m_sImagedir;
    RecordSet       m_oDisciplineRecord;
    RecordSet       m_oLibraryReqRecord;
    RecordSet       m_oLibraryTextRecord;
    RecordSet       m_oRarityRecord;
    RecordSet       m_oSectRecord;
    RecordSet       m_oSpecialRecord;
    RecordSet       m_oTitleRecord;

};

#endif
