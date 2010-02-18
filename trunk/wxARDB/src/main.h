/*	Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *	Copyright (C) 2002 Francois Gombault
 *	gombault.francois@wanadoo.fr
 *
 *	Official project page: https://savannah.nongnu.org/projects/anarchdb/
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


#ifndef _main_h
#define _main_h

#include "browsercryptmodel.h"
#include "browserlibrarymodel.h"
#include "deckmodel.h"
#include "updater.h"
#include "DownloadFile.h"
#include "wx/statusbr.h"
//
#include <wx/ptr_scpd.h>



//This adds the smart pointer for the ZIP extraction

#include <wx/apptrait.h>

class MyApp: public wxApp
{
public:
    MyApp () {

    }

    class MyAppTraits : public wxGUIAppTraits
    {
        virtual GSocketGUIFunctionsTable* GetSocketGUIFunctionsTable() {
            return NULL;
        };
    };

    virtual wxAppTraits *CreateTraits() {
        return new MyAppTraits;
    }

    virtual bool OnInit();
    virtual int OnExit ();


    DECLARE_NO_COPY_CLASS(MyApp)
};



class BrowserFrame: public wxFrame
{
public:
    BrowserFrame (const wxString& title, const wxPoint& pos, const wxSize& size);
    ~BrowserFrame ();

    void OnClose (wxCloseEvent& WXUNUSED(event));
    void OnBrowserCloseTab (wxCommandEvent& WXUNUSED (event));
    void OnBrowserNewCrypt (wxCommandEvent& WXUNUSED (event));
    void OnBrowserNewLibrary (wxCommandEvent& WXUNUSED (event));
    void OnFileExit (wxCommandEvent& WXUNUSED (event));
    void OnFileDeckBuilder (wxCommandEvent& WXUNUSED (event));
    void OnFileEditions (wxCommandEvent& WXUNUSED (event));
    void OnFilePreferences (wxCommandEvent& WXUNUSED (event));
    void OnFileUpdateDatabase (wxCommandEvent& WXUNUSED (event));

    void OnFileImageDownload (wxCommandEvent& event);
    void OnFileImageDownloadEvent (wxDownloadEvent& event);

    void OnHelpManual (wxCommandEvent& WXUNUSED (event));
    void OnHelpAbout (wxCommandEvent& WXUNUSED (event));
    void OnInventoryExportCSV (wxCommandEvent& WXUNUSED (event));
    void OnInventoryExportHTML (wxCommandEvent& WXUNUSED (event));
    void OnInventoryImport (wxCommandEvent& WXUNUSED (event));
    void OnInventoryOpen (wxCommandEvent& WXUNUSED (event));
    void OnInventorySave (wxCommandEvent& WXUNUSED (event));

    void TabChanged(wxNotebookEvent &event);


private:

    void EnableDownLoadMenu(bool fEnabled);

    BrowserCryptModel   *m_pBrowserCryptModel;
    BrowserLibraryModel *m_pBrowserLibraryModel;

    wxGauge     *m_pGauge;
    wxStatusBar *m_pStatbar;
    wxBoxSizer	*m_pPapaSizer;
    wxNotebook	*m_pNotebook;
    unsigned int	m_uiCryptBrowserCount;
    unsigned int	m_uiLibraryBrowserCount;
    bool ImagesSetExists(wxString set);

    enum {
        ID_BROWSER_NEW_CRYPT = wxID_HIGHEST + 1,
        ID_BROWSER_NEW_LIBRARY,
        ID_BROWSER_CLOSE_TAB,
        ID_FILE_EXIT,
        ID_FILE_DECKBUILDER,
        ID_FILE_EDITIONS,
        ID_FILE_PREFERENCES,
        ID_FILE_UPDATEDB,
        ID_FILE_IMAGE_DOWNLOAD,
        ID_HELP_MANUAL,
        ID_HELP_ABOUT,
        ID_INV_OPEN,
        ID_INV_SAVE,
        ID_INV_IMPORT,
        ID_INV_EXPORT_CSV,
        ID_INV_EXPORT_HTML,
        ID_BROWSER_NOTEBOOK,
        ID_EVENT_DOWNLOAD
    };

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(BrowserFrame)
};

#endif
