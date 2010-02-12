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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _deckwindow_h
#define _deckwindow_h

#include <wx/wx.h>
#include <wx/notebook.h>

#include "deckcrypttab.h"
#include "deckinfotab.h"
#include "decklibrarytab.h"

class DeckWindow : public wxFrame
{
    friend class DeckModel;

public:
    void UpdateView ();

private:
    DeckWindow (DeckModel *pModel, const wxPoint& pos, const wxSize& size);
    ~DeckWindow ();

private:
    DeckCryptTab	  *m_pCryptTab;
    DeckInfoTab	  *m_pInfoTab;
    DeckLibraryTab  *m_pLibraryTab;
    wxMenu	   m_oMenu;
    DeckModel	  *m_pModel;
    wxNotebook	  *m_pNotebook;

    void OnFileClose (wxCommandEvent& event);
    void OnFileExportHTML (wxCommandEvent& event);
    void OnFileExportJOL (wxCommandEvent& event);
    void OnFileExportPhpBB (wxCommandEvent& event);
    void OnFileExportText (wxCommandEvent& event);
    void OnFileImportELD (wxCommandEvent& event);
    void OnFileExportSecretLibrary(wxCommandEvent& event);
    void OnFileNew (wxCommandEvent& event);
    void OnFileOpen (wxCommandEvent& event);
    void OnFileSaveDB (wxCommandEvent& event);
    void OnFileSaveXML (wxCommandEvent& event);
    void OnToolsDrawSim (wxCommandEvent& event);
    void OnToolsMergeXML (wxCommandEvent& event);
    void OnToolsResize (wxCommandEvent& event);
    void OnDrawPercentage(wxCommandEvent& event);
    void OnClose (wxCloseEvent& WXUNUSED(event));

    enum {
        // menu IDs
        ID_FILE_NEW = wxID_HIGHEST + 1,
        ID_FILE_OPEN,
        ID_FILE_SAVE_DB,
        ID_FILE_SAVE_XML,
        ID_FILE_EXPORT_HTML,
        ID_FILE_EXPORT_BBCODE,
        ID_FILE_EXPORT_TEXT,
        ID_FILE_EXPORT_JOL,
        ID_FILE_IMPORT_ELD,
        ID_FILE_CLOSE,
        ID_TOOLS_DRAWSIM,
        ID_TOOLS_RESIZE,
        ID_TOOLS_MERGE,
        ID_TOOLS_DRAWPER,
        ID_FILE_EXPORT_SL
    };

    DECLARE_EVENT_TABLE()
};

#endif
