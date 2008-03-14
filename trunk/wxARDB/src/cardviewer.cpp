///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 20 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

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
	wxString filename = wxString::Format(wxT("%s/%s.jpg"),CARD_IMAGE_DIR,cardName.c_str());

	m_nextPrevImage->SetRange(0,cardImages.Count()-1);
	m_nextPrevImage->SetValue(cardImages.Count()-1);

	if (wxFile::Exists(filename))
	{
		m_imagePanel->SetImage(filename);				
	}
	else
	{
		m_imagePanel->Clear();
	}
}

void CardViewer::Clear()
{
	m_imagePanel->Clear();
}

void CardViewer::ChangeImage(wxSpinEvent& WXUNUSED (event))
{
	int i = m_nextPrevImage->GetValue();
	wxString cardName = cardImages.Item(i);

	wxString filename = wxString::Format(wxT("%s/%s.jpg"),CARD_IMAGE_DIR,cardName.c_str());

	if (wxFile::Exists(filename))
	{
		m_imagePanel->SetImage(filename);
	}
	else
	{
		m_imagePanel->Clear();
	}
}


// --------------------------------------------------------
// Event table

BEGIN_EVENT_TABLE(CardViewer, wxPanel)
	EVT_SPIN(ID_SPIN_BUTTON, CardViewer::ChangeImage)
END_EVENT_TABLE()

//EVT_SPIN