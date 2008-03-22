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

#include <wx/file.h>
#include <wx/dir.h>

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

	wxString cardName = cardImages.Item((cardImages.Count()-1));
	
	m_nextPrevImage->SetRange(0,cardImages.Count()-1);
	m_nextPrevImage->SetValue(cardImages.Count()-1);
	DisplayImage(cardName);
}

void CardViewer::ChangeImage(wxSpinEvent& WXUNUSED (event))
{
	int i = m_nextPrevImage->GetValue();
	wxString cardName = cardImages.Item(i);
	DisplayImage(cardName);
}


void CardViewer::DisplayImage(wxString cardName)
{
	wxString filename = wxString::Format(wxT("%s/%s.jpg"),CARD_IMAGE_DIR,cardName.c_str());

	if (wxFile::Exists(filename))
	{
		m_imagePanel->SetImage(filename);				
	}
	else
	{
		//No set image found, find a standard image
		wxString altName = cardName.AfterFirst(wxT('/'));
		filename = wxString::Format(wxT("%s/%s.jpg"),CARD_IMAGE_DIR, altName.c_str());

		if (wxFile::Exists(filename))
		{
			m_nextPrevImage->SetRange(0,0);
			m_imagePanel->SetImage(filename);
		}
		else // No image at all clear the viewer
		{
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
