/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
*
*  Copyright (C) 2010 Graham Smith
*  graham.r.smith@gmail.com
*
*  contributors:
*
*  Official project page: http://code.google.com/p/ardb/
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "preferencedialog.h"
#include <wx/confbase.h>
#include <wx/fileconf.h>

///////////////////////////////////////////////////////////////////////////

PrefDialog::PrefDialog() : wxDialog(0, -1, wxT("Preferences"), wxDefaultPosition, wxSize( 245,142 ),  wxDEFAULT_DIALOG_STYLE)
{
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get();

    bool fUseProxy = FALSE;
    bool fUpdateCards = FALSE;

    if (pConfig) {
        pConfig->Read(wxT("UpdateCards"), &fUpdateCards, FALSE);
	pConfig->Read(wxT("UseProxy"), &fUseProxy, FALSE);
    }

    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer( wxVERTICAL );

#ifdef __WXMSW__
    m_cbUseProxy = new wxCheckBox(this, wxID_ANY,
                                        wxT("Use IE's proxy settings"),
                                        wxDefaultPosition, wxDefaultSize,
                                        0);
    m_cbUseProxy->SetValue(fUseProxy);
    bSizer4->Add(m_cbUseProxy, 0, wxALL, 5);
#else
    m_cbUseProxy = NULL;
#endif

    m_cbDownloadCards = new wxCheckBox(this, wxID_ANY,
                                       wxT("Automatic Card Text Download"),
                                       wxDefaultPosition, wxDefaultSize,
                                       0);

    m_cbDownloadCards->SetValue(fUpdateCards);
    bSizer4->Add( m_cbDownloadCards, 0, wxALL, 5 );
    bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );

    m_sdbSizer2 = new wxStdDialogButtonSizer();
    m_pbtnOK = new wxButton( this, wxID_OK );
    m_sdbSizer2->AddButton(m_pbtnOK);
    m_sdbSizer2Cancel = new wxButton( this, wxID_CANCEL );
    m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
    m_sdbSizer2->Realize();
    bSizer4->Add( m_sdbSizer2, 1, wxEXPAND, 5 );

    this->SetSizer( bSizer4 );
    this->Layout();

    m_pbtnOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                      wxCommandEventHandler(PrefDialog::SaveSettings ),
                      NULL, this);

}

PrefDialog::~PrefDialog()
{
    m_pbtnOK->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                         wxCommandEventHandler( PrefDialog::SaveSettings ),
                         NULL, this);
}

void PrefDialog::SaveSettings(wxCommandEvent& event)
{
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get();

    if (pConfig) {

	if (m_cbUseProxy != NULL) {
	    pConfig->Write(wxT("UseProxy"), m_cbUseProxy->GetValue());
	}

        pConfig->Write(wxT("UpdateCards"), m_cbDownloadCards->GetValue());
    }

    event.Skip();
}
