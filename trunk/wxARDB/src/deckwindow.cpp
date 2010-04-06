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

#include "amountdialog.h"
#include "deckwindow.h"
#include "deckmodel.h"
#include "drawsimulator.h"
#include "interfacedata.h"
#include "sllogindialog.h"

#include <wx/confbase.h>
#include <wx/fileconf.h>

BEGIN_EVENT_TABLE (DeckWindow, wxFrame)
    EVT_MENU (ID_FILE_NEW, DeckWindow::OnFileNew)
    EVT_MENU (ID_FILE_OPEN, DeckWindow::OnFileOpen)
    EVT_MENU (ID_FILE_SAVE_DB, DeckWindow::OnFileSaveDB)
    EVT_MENU (ID_FILE_SAVE_XML, DeckWindow::OnFileSaveXML)
    EVT_MENU (ID_FILE_EXPORT_HTML, DeckWindow::OnFileExportHTML)
    EVT_MENU (ID_FILE_EXPORT_BBCODE, DeckWindow::OnFileExportPhpBB)
    EVT_MENU (ID_FILE_EXPORT_TEXT, DeckWindow::OnFileExportText)
    EVT_MENU (ID_FILE_EXPORT_JOL, DeckWindow::OnFileExportJOL)
    EVT_MENU (ID_FILE_EXPORT_LACKEY, DeckWindow::OnFileExportLackey)
    EVT_MENU (ID_FILE_EXPORT_SL, DeckWindow::OnFileExportSecretLibrary)
    EVT_MENU (ID_FILE_IMPORT_ELD, DeckWindow::OnFileImportELD)
    EVT_MENU (ID_FILE_CLOSE, DeckWindow::OnFileClose)
    EVT_MENU (ID_TOOLS_DRAWSIM, DeckWindow::OnToolsDrawSim)
    EVT_MENU (ID_TOOLS_MERGE, DeckWindow::OnToolsMergeXML)
    EVT_MENU (ID_TOOLS_RESIZE, DeckWindow::OnToolsResize)
    EVT_MENU (ID_TOOLS_DRAWPER, DeckWindow::OnDrawPercentage)
    EVT_CLOSE (DeckWindow::OnClose)
    //  EVT_SIZE (DeckWindow::OnResize)
END_EVENT_TABLE ()


DeckWindow::DeckWindow (DeckModel *pModel, const wxPoint& pos, const wxSize& size) :
    wxFrame (0, -1, wxT ("Anarch Revolt - Deck Builder"), pos, size),
    m_pCryptTab (NULL),
    m_pInfoTab (NULL),
    m_pLibraryTab (NULL),
    m_oMenu (),
    m_pModel (pModel),
    m_pNotebook (NULL)
{
    wxBoxSizer *pPapaSizer;

    SetSizeHints (640, 480);

    pPapaSizer = new wxBoxSizer (wxVERTICAL);

    m_pNotebook = new wxNotebook (this, -1);
    pPapaSizer->Add (m_pNotebook, 3, wxEXPAND);

    SetSizer (pPapaSizer);
    pPapaSizer->Layout ();

    //  menu setup goes here
    //  wxMenu *pHelpMenu = new wxMenu;
    wxMenu *pFileMenu = new wxMenu;
    pFileMenu->Append (ID_FILE_NEW, wxT ("New deck\tCtrl+N"));
    pFileMenu->Append (ID_FILE_OPEN, wxT ("Open deck (xml)\tCtrl+O"));
    pFileMenu->Append (ID_FILE_SAVE_XML, wxT ("Save deck (xml)\tCtrl+S"));
    pFileMenu -> AppendSeparator () ;
    pFileMenu->Append (ID_FILE_EXPORT_HTML, wxT ("Export deck to HTML\tCtrl+H"));
    pFileMenu->Append (ID_FILE_EXPORT_BBCODE, wxT ("Export deck to phpBB\tCtrl+P"));
    pFileMenu->Append (ID_FILE_EXPORT_TEXT, wxT ("Export deck to text\tCtrl+T"));
    pFileMenu->Append (ID_FILE_EXPORT_JOL, wxT ("Export deck to JOL\tCtrl+J"));
    pFileMenu->Append (ID_FILE_EXPORT_LACKEY, wxT ("Export deck to Lackey CCG\tCtrl+K"));
    pFileMenu->Append (ID_FILE_EXPORT_SL, wxT ("Export deck to Secret Library\tCtrl+L"));
    pFileMenu -> AppendSeparator () ;
    pFileMenu->Append (ID_FILE_IMPORT_ELD, wxT ("Import ELD deck\tCtrl+E"));
    pFileMenu -> AppendSeparator () ;
    //  pFileMenu->Append (ID_FILE_SAVE_DB, wxT ("Save to database"));
    pFileMenu->Append (ID_FILE_CLOSE, wxT ("Close\tCtrl+Q"));

    wxMenu *pToolsMenu = new wxMenu;
    pToolsMenu->Append (ID_TOOLS_DRAWSIM, wxT ("Draw Simulator\tCtrl+Shift+D"));
    pToolsMenu->Append (ID_TOOLS_MERGE, wxT ("Merge deck\tCtrl+Shift+M"));
    pToolsMenu->Append (ID_TOOLS_RESIZE, wxT ("Resize library\tCtrl+Shift+R"));
    pToolsMenu->AppendCheckItem (ID_TOOLS_DRAWPER, wxT ("Show/Hide Percentage\tCtrl+Shift+P"));


//   pFileMenu -> Append ( wxH_FILEE_XIT, wxT ("E&xit"), wxT ("") );
//   pHelpMenu -> Append ( wxH_HELPMANUAL, wxT ("&Manual"), wxT ("") );
//   pHelpMenu -> Append ( wxH_HELPABOUT, wxT ("&About"), wxT ("") );
    wxMenuBar *menuBar = new wxMenuBar;

    menuBar->Append (pFileMenu, wxT ("&File") );
    menuBar->Append (pToolsMenu, wxT ("&Tools") );
    //  menuBar->Append (pHelpMenu, wxT ("&Help") );

    SetMenuBar (menuBar);

    // Add the various tabs
    m_pInfoTab = new DeckInfoTab (m_pModel, m_pNotebook);
    m_pCryptTab = new DeckCryptTab (m_pModel, m_pNotebook);
    m_pLibraryTab = new DeckLibraryTab (m_pModel, m_pNotebook);

    SetIcon (*g_pIcon);

    int iHeight, iWidth;
    wxString sDeckHeightEntry = wxT ("DeckWindowHeight"),
             sDeckWidthEntry = wxT ("DeckWindowWidth");
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
    if (pConfig) {
        if (!pConfig->Read (sDeckHeightEntry, &iHeight, 600)) {
            pConfig->Write (sDeckHeightEntry, iHeight);
            pConfig->Flush (TRUE);
        }
        if (!pConfig->Read (sDeckWidthEntry, &iWidth, 800)) {
            pConfig->Write (sDeckWidthEntry, iWidth);
            pConfig->Flush (TRUE);
        }
        SetSize (iWidth, iHeight);
    }

    Show ();
}


DeckWindow::~DeckWindow ()
{
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
    if (pConfig) {
        pConfig->Write (wxT ("DeckWindowHeight"), GetSize ().GetHeight ());
        pConfig->Write (wxT ("DeckWindowWidth"), GetSize ().GetWidth ());
        pConfig->Flush (TRUE);
    }

    DeckModel::DeleteInstance ();
}

void
DeckWindow::OnClose(wxCloseEvent& event)
{
    m_pModel->ShouldSaveWarning();
    Destroy();
}

void
DeckWindow::OnFileClose (wxCommandEvent& WXUNUSED (event))
{
    m_pModel->ShouldSaveWarning();
    Close();
}

void
DeckWindow::OnFileExportHTML (wxCommandEvent& WXUNUSED (event))
{
    if (!m_pModel->ExportToHTML ()) {
        wxLogError (wxT ("An error occured while saving"));
    }
}

void
DeckWindow::OnFileExportJOL (wxCommandEvent& WXUNUSED (event))
{
    if (!m_pModel->ExportToJOL ()) {
        wxLogError (wxT ("An error occured while saving"));
    }
}

void
DeckWindow::OnFileExportPhpBB (wxCommandEvent& WXUNUSED (event))
{
    if (!m_pModel->ExportToPhpBB ()) {
        wxLogError (wxT ("An error occured while saving"));
    }
}

void
DeckWindow::OnFileExportText (wxCommandEvent& WXUNUSED (event))
{
    if (!m_pModel->ExportToText ()) {
        wxLogError (wxT ("An error occured while saving"));
    }
}

void
DeckWindow::OnFileExportLackey(wxCommandEvent& WXUNUSED (event))
{
    if (!m_pModel->ExportToLackey ()) {
        wxLogError (wxT ("An error occured while saving"));
    }
}

void
DeckWindow::OnFileExportSecretLibrary(wxCommandEvent& WXUNUSED (event))
{
    wxString username = wxT("");
    wxString password = wxT("");
    wxString sSaveDetails = wxT("SaveSLDetails");
    wxString sSlUserName = wxT("SlUserName");
    wxString sSlPassword = wxT("SlPassword");
    bool saveDetails;

    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get();

    if (pConfig) {
        if (pConfig->Read(sSaveDetails, &saveDetails, FALSE)) {
            if (saveDetails) {
                pConfig->Read(sSlUserName, &username, wxT(""));
                pConfig->Read(sSlPassword, &password, wxT(""));
            }
        }
    }

    //Prompt for Username and Password
    SLLoginDialog *pDialog = new SLLoginDialog(saveDetails,username,password);

    if (pDialog->ShowModal() == wxID_OK) {
        username = pDialog->Username();
        password = pDialog->Password();
        saveDetails = pDialog->SaveDetails();

        if (saveDetails) {
            pConfig->Write(sSaveDetails, &saveDetails);
            pConfig->Write(sSlUserName, username);
            pConfig->Write(sSlPassword, password);
            pConfig->Flush(TRUE);
        }

        if (!m_pModel->ExportToSecretLibrary(username,password)) {
            wxLogError (wxT ("An error occured while uploading"));
        }
    }

    delete pDialog;

}

void
DeckWindow::OnFileImportELD (wxCommandEvent& WXUNUSED (event))
{
    if (!m_pModel->ImportFromELD ()) {
        wxLogError (wxT ("An error occured while importing"));
    }
}


void
DeckWindow::OnFileNew (wxCommandEvent& WXUNUSED (event))
{
    m_pModel->Clear ();
}


void
DeckWindow::OnFileOpen (wxCommandEvent& WXUNUSED (event))
{
    m_pModel->ImportFromXML ();
}


void
DeckWindow::OnFileSaveDB (wxCommandEvent& WXUNUSED (event))
{
    wxLogMessage (wxT ("Doesn't work yet"));
}


void
DeckWindow::OnFileSaveXML (wxCommandEvent& WXUNUSED (event))
{
    if (!m_pModel->ExportToXML ()) {
        wxLogError (wxT ("An error occured while saving"));
    }
}


void
DeckWindow::OnToolsDrawSim (wxCommandEvent& WXUNUSED (event))
{
    DrawSimulator * pDrawSim = DrawSimulator::Instance ();
    pDrawSim->Show ();
}


void
DeckWindow::OnToolsMergeXML (wxCommandEvent& WXUNUSED (event))
{
    m_pModel->MergeFromXML ();
}


void
DeckWindow::OnToolsResize (wxCommandEvent& WXUNUSED (event))
{

    unsigned int uiOldCount = m_pModel->GetLibraryCount ();
    wxArrayInt oArrayCount;
    wxArrayString oArrayText;
    wxString sText = wxT ("Current library size: ");
    sText << uiOldCount << wxT (" cards.\n Please input new size: ");

    oArrayText.Add (sText);
    oArrayCount.Add (uiOldCount);

    AmountDialog * pDialog = new AmountDialog (&oArrayText, &oArrayCount);

    if (pDialog->ShowModal ()) {
        m_pModel->ResizeLibrary (oArrayCount.Item (0));
    }

    delete pDialog;

}

void
DeckWindow::OnDrawPercentage(wxCommandEvent& WXUNUSED (event))
{
    m_pLibraryTab->ShowPercentage();
}


void
DeckWindow::UpdateView ()
{
    m_pCryptTab->Update ();
    m_pLibraryTab->Update ();
    m_pInfoTab->Update ();
}
