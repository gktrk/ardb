/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2008 Graham Smith
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

#include "cardviewer.h"
#include "importxml.h"

#include <wx/file.h>
#include <wx/dir.h>

#define CONV_BUFFER_SIZE 1024

///////////////////////////////////////////////////////////////////////////

CardViewer::CardViewer( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
    wxBoxSizer* bSizer9;
    bSizer9 = new wxBoxSizer( wxVERTICAL );

    m_imagePanel = new ImagePanel( this);

    bSizer9->Add( m_imagePanel, 1, wxALIGN_CENTER|wxEXPAND|wxSHAPED|wxTOP, 5 );

    wxBoxSizer* bSizer11;
    bSizer11 = new wxBoxSizer( wxHORIZONTAL );

    bSizer11->Add( 0, 0, 1, wxEXPAND, 5 );

    m_nextPrevImage = new wxSpinButton( this, ID_SPIN_BUTTON, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS|wxSP_HORIZONTAL|wxSP_WRAP );
    bSizer11->Add( m_nextPrevImage, 0, wxALIGN_CENTER|wxALL, 5 );
    m_nextPrevImage->SetRange(0,0);

    bSizer11->Add( 0, 0, 1, wxEXPAND, 5 );

    bSizer9->Add( bSizer11, 0, wxEXPAND, 5 );

    this->SetSizer( bSizer9 );
    this->Layout();
}

CardViewer::~CardViewer()
{
}

void CardViewer::SetImage(wxArrayString *cardNames)
{
    cardImages = *cardNames;

    m_nextPrevImage->SetRange(0,cardImages.Count()-1);
    m_nextPrevImage->SetValue(cardImages.Count()-1);
    DisplayImage((cardImages.Count()-1));
}

void CardViewer::ChangeImage(wxSpinEvent& WXUNUSED (event))
{
    int i = m_nextPrevImage->GetValue();
    DisplayImage(i);
}


void CardViewer::DisplayImage(int i)
{
    bool imageFound = false;
    wxString cardName = cardImages.Item(i);

#ifndef __WXMSW__
    int k;
    char buffer[CONV_BUFFER_SIZE];
#endif

    for (int j=i; j>=0; j--) {
        wxString filename = wxString::Format(wxT("%s.jpg"),cardImages.Item(j).c_str());

#ifndef __WXMSW__
        const wxWX2MBbuf tmp_buf = filename.mb_str(wxConvISO8859_1);
        const char* tmp_str = tmp_buf;

        for(k = 0; k < strlen(tmp_str); k++) {
            if(tmp_str[k] < 0) {
                buffer[k] = Unicode2Ascii((unsigned char)tmp_str[k]);
            } else {
                buffer[k] = tmp_str[k];
            }
        }
        buffer[k] = '\0';

        filename = wxString(buffer, *wxConvCurrent);
#endif
        //if (wxFile::Exists(filename)) {
            m_imagePanel->SetImage(filename);

            imageFound = true;
            break;
	    //}
    }


    if (!imageFound) {
        //No set image found, find a standard image
        wxString altName = cardName.AfterFirst(wxT('/'));
        wxString filename = wxString::Format(wxT("%s/%s.jpg"),CARD_IMAGE_DIR, altName.c_str());

#ifndef __WXMSW__
        const wxWX2MBbuf tmp_buf2 = filename.mb_str(wxConvISO8859_1);
        const char* tmp_str2 = tmp_buf2;

        for(k = 0; k < strlen(tmp_str2); k++) {
            if(tmp_str2[k] < 0) {
                buffer[k] = Unicode2Ascii((unsigned char)tmp_str2[k]);
            } else {
                buffer[k] = tmp_str2[k];
            }
        }
        buffer[k] = '\0';

        filename = wxString(buffer, *wxConvCurrent);
#endif
        if (wxFile::Exists(filename)) {
            m_nextPrevImage->SetRange(0,0);
            m_imagePanel->SetImage(filename);

        } else { // No image at all clear the viewer
            m_imagePanel->Clear();
        }
    }
}

void CardViewer::Clear()
{
    m_imagePanel->Clear();
}



// --------------------------------------------------------
// Event table

BEGIN_EVENT_TABLE(CardViewer, wxPanel)
    EVT_SPIN(ID_SPIN_BUTTON, CardViewer::ChangeImage)
END_EVENT_TABLE()

//EVT_SPIN
