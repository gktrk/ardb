/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2009 Graham Smith
 *  graham.r.smith@gmail.com
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

#include "sllogindialog.h"

///////////////////////////////////////////////////////////////////////////

SLLoginDialog::SLLoginDialog(bool saveDetails, wxString &username, wxString &password) :
    wxDialog( 0, -1, wxT("Login"), wxDefaultPosition, wxSize( 224,165 ) )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer* fgSizer3;
    fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
    fgSizer3->SetFlexibleDirection( wxBOTH );
    fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Username"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText2->Wrap( -1 );
    fgSizer3->Add( m_staticText2, 0, wxALL, 5 );

    m_usernameEditbox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer3->Add( m_usernameEditbox, 0, wxALL, 5 );
    m_usernameEditbox->SetValue(username);

    m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText3->Wrap( -1 );
    fgSizer3->Add( m_staticText3, 0, wxALL, 5 );

    m_PasswordEditBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    fgSizer3->Add( m_PasswordEditBox, 0, wxALL, 5 );
    m_PasswordEditBox->SetValue(password);

    m_SaveDetailscheckBox = new wxCheckBox( this, wxID_ANY, wxT("Save Details"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SaveDetailscheckBox->SetValue(saveDetails);

    fgSizer3->Add( m_SaveDetailscheckBox, 0, wxALL, 5 );

    bSizer1->Add( fgSizer3, 1, wxEXPAND, 5 );

    m_sdbSizer2 = new wxStdDialogButtonSizer();
    m_sdbSizer2OK = new wxButton( this, wxID_OK );
    m_sdbSizer2->AddButton( m_sdbSizer2OK );
    m_sdbSizer2Cancel = new wxButton( this, wxID_CANCEL );
    m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
    m_sdbSizer2->Realize();
    bSizer1->Add( m_sdbSizer2, 1, wxEXPAND, 2 );

    m_sdbSizer2OK->SetDefault();

    this->SetSizer( bSizer1 );
    this->Layout();
}

SLLoginDialog::~SLLoginDialog()
{
}
