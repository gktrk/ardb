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

#include "deckinfotab.h"

BEGIN_EVENT_TABLE (DeckInfoTab, wxPanel)
  EVT_TEXT (ID_TEXT_NAME, DeckInfoTab::OnTextNameChanged)
  EVT_TEXT (ID_TEXT_AUTHOR, DeckInfoTab::OnTextAuthorChanged)
  EVT_TEXT (ID_TEXT_COMMENTS, DeckInfoTab::OnTextCommentsChanged)
END_EVENT_TABLE ()


DeckInfoTab::DeckInfoTab (DeckModel *pModel, wxNotebook *pParent) :
  wxPanel (pParent, -1),
  m_bNumbFields (TRUE),
  m_pLabelTime (NULL),
  m_pModel (pModel),
  m_pParent (pParent),
  m_pTextAuthor (NULL),
  m_pTextComments (NULL),
  m_pTextName (NULL)
{
  wxBoxSizer *pPapaSizer;
  wxStaticText *pNameLabel, *pAuthorLabel, *pCommentsLabel;

  m_pParent->AddPage (this, wxT ("Deck Information"), FALSE, -1);

  pPapaSizer = new wxBoxSizer (wxVERTICAL); 
  SetSizer (pPapaSizer);
  
  pNameLabel = new wxStaticText (this, -1, wxT ("Deck name :"));
  pPapaSizer->Add (pNameLabel, 0, wxALL, 5); 

  m_pTextName = new wxTextCtrl (this, ID_TEXT_NAME);
  pPapaSizer->Add (m_pTextName, 0, wxEXPAND | wxALL, 5);

  pAuthorLabel = new wxStaticText (this, -1, wxT ("Author :"));
  pPapaSizer->Add (pAuthorLabel, 0, wxALL, 5); 

  m_pTextAuthor = new wxTextCtrl (this, ID_TEXT_AUTHOR);
  pPapaSizer->Add (m_pTextAuthor, 0, wxEXPAND | wxALL, 5);

  pCommentsLabel = new wxStaticText (this, -1, wxT ("Comments :"));
  pPapaSizer->Add (pCommentsLabel, 0, wxALL, 5); 

  m_pTextComments = new wxTextCtrl (this, ID_TEXT_COMMENTS, wxT (""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  pPapaSizer->Add (m_pTextComments, 1, wxEXPAND | wxALL, 5);

  pPapaSizer->Layout ();
  pPapaSizer->Fit (m_pParent);

  m_bNumbFields = FALSE;
}


DeckInfoTab::~DeckInfoTab ()
{

}


void
DeckInfoTab::OnTextAuthorChanged (wxCommandEvent& WXUNUSED (event))
{
  if (m_bNumbFields) return;

  wxString s = m_pTextAuthor->GetValue ();
  m_pModel->SetAuthor (s);
}


void
DeckInfoTab::OnTextCommentsChanged (wxCommandEvent& WXUNUSED (event))
{
  if (m_bNumbFields) return;

  wxString s = m_pTextComments->GetValue ();
  m_pModel->SetDescription (s);
}


void
DeckInfoTab::OnTextNameChanged (wxCommandEvent& WXUNUSED (event))
{
  if (m_bNumbFields) return;

  wxString s = m_pTextName->GetValue ();
  m_pModel->SetName (s);
}


void
DeckInfoTab::Update ()
{
  m_pTextName->SetValue (m_pModel->GetName ());
  m_pTextAuthor->SetValue (m_pModel->GetAuthor ());
  m_pTextComments->SetValue (m_pModel->GetDescription ());
}
