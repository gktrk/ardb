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


#include "browsercryptfilter.h"
#include "speling.h"

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!


BEGIN_EVENT_TABLE(BrowserCryptFilter, wxDialog)
EVT_TEXT (ID_CARD_INPUT, BrowserCryptFilter::OnCardInputChanged)
EVT_LISTBOX (ID_CARD_PICKER, BrowserCryptFilter::OnCardPickerSelection)
EVT_BUTTON (ID_OK_BUTTON, BrowserCryptFilter::OnOKButtonClick)
EVT_BUTTON (ID_CANCEL_BUTTON, BrowserCryptFilter::OnCancelButtonClick)
EVT_BUTTON (ID_CLEAR_BUTTON, BrowserCryptFilter::OnClearButtonClick)
EVT_COMMAND_SCROLL (ID_CAP_LESS_SCALE, BrowserCryptFilter::OnCapLesserScroll)
EVT_COMMAND_SCROLL (ID_CAP_EQ_SCALE, BrowserCryptFilter::OnCapEqualScroll)
EVT_COMMAND_SCROLL (ID_CAP_MORE_SCALE, BrowserCryptFilter::OnCapGreaterScroll)
END_EVENT_TABLE()


BrowserCryptFilter::BrowserCryptFilter(BrowserCryptController *pController) :
wxDialog (0, -1, wxT ("Crypt Filter"), wxDefaultPosition, wxDefaultSize),
// Initialisation of member objects and variables
m_bNoEvents (FALSE),
m_iCycleCounter (-1),
m_iCycleLowerValue (-1),
m_iCycleUpperValue (-1),
m_lFeatherRef (-1),
m_oArrayOfNames (),
m_oCardList (),
m_oDisciplinesCombos (),
m_oNameList (),
m_pAnarchCheckbox (NULL),
m_pBlackHandCheckbox (NULL),
m_pCancelButton (NULL),
m_pCapEqualCheckbox (NULL),
m_pCapGreaterCheckbox (NULL),
m_pCapLesserCheckbox (NULL),
m_pCapEqualSlider (NULL),
m_pCapGreaterSlider (NULL),
m_pCapLesserSlider (NULL),
m_pCardInput (NULL),
m_pCardPicker (NULL),
m_pCardText (NULL),
m_pClanList (NULL),
m_pClearButton (NULL),
m_pController (pController),
m_pEditionList (NULL),
m_pFeatherCapSlider (NULL),
m_pFeatherGrpCheckbox (NULL),
m_pGroupList (NULL),
m_pOKButton (NULL),
m_pOtherText (NULL),
m_pRarityList (NULL),
m_pSectList (NULL),
m_pSeraphCheckbox (NULL),
m_pTitleList (NULL),
m_pCardNameText (NULL)
{
	wxAcceleratorEntry entries[3];
	entries[0].Set(wxACCEL_NORMAL, WXK_RETURN, ID_OK_BUTTON);
	entries[1].Set(wxACCEL_NORMAL, WXK_ESCAPE, ID_CANCEL_BUTTON);
	entries[2].Set(wxACCEL_NORMAL, WXK_DELETE, ID_CLEAR_BUTTON);
	wxAcceleratorTable accel(3, entries);
	this->SetAcceleratorTable(accel);

	InterfaceData *pUIData = InterfaceData::Instance ();

	wxBoxSizer *pPapaSizer = new wxBoxSizer (wxVERTICAL);
	SetAutoLayout (TRUE);
	SetSizer (pPapaSizer);

	// create the notebook
	wxNotebook *pNotebook = new wxNotebook (this , -1);

	// create the criterion panel
	wxFlexGridSizer *pPrimoSizer = new wxFlexGridSizer (2, 5, 5);
	wxPanel *pPrimoPanel = new wxPanel (pNotebook, -1);

	pNotebook->AddPage (pPrimoPanel, wxT ("Primo"));

	// The Disciplines ComboBoxes
	wxStaticBox *pDiscStaticBox = new wxStaticBox (pPrimoPanel, -1, wxT ("By discipline"));
	wxStaticBoxSizer *pDisciplineBox = new wxStaticBoxSizer (pDiscStaticBox, wxHORIZONTAL);
	wxBoxSizer *pDisCol1Sizer = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *pDisCol2Sizer = new wxBoxSizer (wxVERTICAL);
	pDisciplineBox->Add (pDisCol1Sizer, 0, wxRIGHT, 5);
	pDisciplineBox->Add (pDisCol2Sizer, 0);

	{
		unsigned int i = 0;
		for (; i < pUIData->GetDisciplines ()->GetCount () / 2; i++)
		{
			BuildDiscipline (i, pDisCol1Sizer, pPrimoPanel);
		}
		for (; i < pUIData->GetDisciplines ()->GetCount (); i++)
		{
			BuildDiscipline (i, pDisCol2Sizer, pPrimoPanel);
		}
	}
	pPrimoSizer->Add (pDisciplineBox, 1, wxEXPAND | wxALL, 5);

	// The vertical sizer on the right side
	wxBoxSizer *pUpperRightSizer = new wxBoxSizer (wxVERTICAL);
	pPrimoSizer->Add (pUpperRightSizer, 1, wxEXPAND | wxALL, 5);

	// Titles
	wxStaticBox *pTitleStaticBox = new wxStaticBox (pPrimoPanel, -1, wxT ("By title"));
	wxStaticBoxSizer *pTitleBox = new wxStaticBoxSizer (pTitleStaticBox, wxVERTICAL);
	m_pTitleList = new wxListView (pPrimoPanel, -1, wxDefaultPosition, wxSize (200, 75), wxLC_REPORT | wxLC_NO_HEADER);
	m_pTitleList->InsertColumn (0, wxEmptyString);
	for (unsigned int i = 0; i < pUIData->GetTitles ()->GetCount (); i++)
	{
		m_pTitleList->InsertItem (i, pUIData->GetTitles ()->Item (i)[0]);
	}
	// m_pTitleList->SetScrollPos (wxVERTICAL, -1);
	pTitleBox->Add (m_pTitleList, 1, wxEXPAND | wxTOP, 5);
	pUpperRightSizer->Add (pTitleBox, 0, wxEXPAND);

	// Clans
	wxStaticBox *pClanStaticBox = new wxStaticBox (pPrimoPanel, -1, wxT ("By clan"));
	wxStaticBoxSizer *pClanBox = new wxStaticBoxSizer (pClanStaticBox, wxVERTICAL);
	m_pClanList = new wxListView (pPrimoPanel, -1, wxDefaultPosition, wxSize (200, 75), wxLC_REPORT | wxLC_NO_HEADER);
	m_pClanList->InsertColumn (0, wxEmptyString);
	for (unsigned int i = 0; i < pUIData->GetClans ()->GetCount (); i++)
	{
		m_pClanList->InsertItem (i, pUIData->GetClans ()->Item (i));
	}
	pClanBox->Add (m_pClanList, 1, wxEXPAND | wxTOP, 5);
	pUpperRightSizer->Add (pClanBox, 1, wxEXPAND);

	// Special abilities & card text
	wxStaticBox *pSpecialStaticBox = new wxStaticBox (pPrimoPanel, -1, wxT ("By card text"));
	wxStaticBoxSizer *pSpecialBox = new wxStaticBoxSizer (pSpecialStaticBox, wxHORIZONTAL);
	wxGridSizer *pSpecialSizer = new wxGridSizer (4);
	wxBoxSizer *pSpecialSizerV = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *pSpecialSizerOther = new wxBoxSizer (wxHORIZONTAL);
	pSpecialBox->Add (pSpecialSizerV, 1, wxEXPAND | wxTOP, 5);
	pSpecialSizerV->Add (pSpecialSizer, 1, wxEXPAND);
	pSpecialSizerV->Add (pSpecialSizerOther, 0, wxEXPAND);
	for (unsigned int i = 0; i < pUIData->GetSpecials ()->GetCount (); i++)
	{
		BuildSpecial (i, pSpecialSizer, pPrimoPanel);
	}

	wxStaticText *pLabel = new wxStaticText (pPrimoPanel, -1, wxT ("Card name :"));
	pSpecialSizerOther->Add (pLabel, 0, wxALIGN_CENTER);
	m_pCardNameText = new wxTextCtrl (pPrimoPanel, -1);
	pSpecialSizerOther->Add (m_pCardNameText, 1, wxALL, 5);

	pLabel = new wxStaticText (pPrimoPanel, -1, wxT ("Other text :"));
	pSpecialSizerOther->Add (pLabel, 0, wxALIGN_CENTER | wxLEFT, 10);
	m_pOtherText = new wxTextCtrl (pPrimoPanel, -1);
	pSpecialSizerOther->Add (m_pOtherText, 1, wxALL, 5);

	pPrimoSizer->Add (pSpecialBox, 0, wxEXPAND | wxALL, 5);

	// the Sects and group sizer
	wxFlexGridSizer *pGroupSectSizer = new wxFlexGridSizer (2);
	wxBoxSizer *pLowerRightSizer = new wxBoxSizer (wxVERTICAL);
	pPrimoSizer->Add (pLowerRightSizer, 1, wxEXPAND | wxALL, 5);
	// Sects
	wxStaticBox *pSectStaticBox = new wxStaticBox (pPrimoPanel, -1, wxT ("By sect"));
	wxStaticBoxSizer *pSectBox = new wxStaticBoxSizer (pSectStaticBox, wxHORIZONTAL);
	wxBoxSizer *pSectSizer = new wxBoxSizer (wxVERTICAL);
	pSectBox->Add (pSectSizer, 1, wxEXPAND);
	m_pSectList = new wxListView (pPrimoPanel, -1, wxDefaultPosition, wxSize (120, 60), wxLC_REPORT | wxLC_NO_HEADER);
	m_pSectList->InsertColumn (0, wxEmptyString);
	for (unsigned int i = 0; i < pUIData->GetSects ()->Count (); i++)
	{
		m_pSectList->InsertItem (i, pUIData->GetSects ()->Item (i)[0]);
	}
	pSectSizer->Add (m_pSectList, 1, wxEXPAND | wxTOP, 5);
	pGroupSectSizer->Add (pSectBox, 1, wxEXPAND);

	// Groups
	wxStaticBox *pGroupStaticBox = new wxStaticBox (pPrimoPanel, -1, wxT ("By group"));
	wxStaticBoxSizer *pGroupBox = new wxStaticBoxSizer (pGroupStaticBox, wxHORIZONTAL);
	wxBoxSizer *pGroupSizer = new wxBoxSizer (wxVERTICAL);
	pGroupBox->Add (pGroupSizer, 1, wxEXPAND);
#ifdef __WXMAC__
	m_pGroupList = new wxListView (pPrimoPanel, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_NO_HEADER);
#else
	m_pGroupList = new wxListView (pPrimoPanel, -1, wxDefaultPosition, wxSize (120, 60), wxLC_REPORT | wxLC_NO_HEADER);
#endif
	wxString sGroupString;
	m_pGroupList->InsertColumn (0, wxEmptyString);
	for (unsigned int i = 0; i <= g_uiGroupCount; i++)
	{
		sGroupString.Printf (wxT ("%d"), i);
		m_pGroupList->InsertItem (i, sGroupString);
	}
	pGroupSizer->Add (m_pGroupList, 1, wxEXPAND | wxTOP, 5);
	pGroupSectSizer->Add (pGroupBox, 1, wxEXPAND);
	pUpperRightSizer->Add (pGroupSectSizer, 0, wxEXPAND, 5);


	// Capacity
	wxStaticBox *pCapStaticBox = new wxStaticBox (pPrimoPanel, -1, wxT ("By capacity"));
	wxStaticBoxSizer *pCapBox = new wxStaticBoxSizer (pCapStaticBox, wxVERTICAL);
	wxFlexGridSizer *pCapSizer = new wxFlexGridSizer (2);
	pCapBox->Add (pCapSizer, 1, wxEXPAND | wxTOP, 5);

	m_pCapLesserCheckbox = new wxCheckBox (pPrimoPanel, ID_CAP_LESS_CHECKBOX, wxT ("Capacity <="));
	pCapSizer->Add (m_pCapLesserCheckbox, 0, wxALIGN_CENTER_VERTICAL);
	m_pCapLesserSlider = new wxSlider (pPrimoPanel, ID_CAP_LESS_SCALE, 8, 1, 11, wxDefaultPosition, wxSize (100, -1), wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
	pCapSizer->Add (m_pCapLesserSlider, 1, wxEXPAND);
	m_pCapEqualCheckbox = new wxCheckBox (pPrimoPanel, ID_CAP_EQ_CHECKBOX, wxT ("Capacity ="));
	pCapSizer->Add (m_pCapEqualCheckbox, 0, wxALIGN_CENTER_VERTICAL);
	m_pCapEqualSlider = new wxSlider (pPrimoPanel, ID_CAP_EQ_SCALE, 6, 1, 11, wxDefaultPosition, wxSize (100, -1), wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
	pCapSizer->Add (m_pCapEqualSlider, 1, wxEXPAND);
	m_pCapGreaterCheckbox = new wxCheckBox (pPrimoPanel, ID_CAP_MORE_CHECKBOX, wxT ("Capacity >="));
	pCapSizer->Add (m_pCapGreaterCheckbox, 0, wxALIGN_CENTER_VERTICAL);
	m_pCapGreaterSlider = new wxSlider (pPrimoPanel, ID_CAP_MORE_SCALE, 4, 1, 11, wxDefaultPosition, wxSize (100, -1), wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
	pCapSizer->Add (m_pCapGreaterSlider, 1, wxEXPAND);

	pLowerRightSizer->Add (pCapBox, 1, wxEXPAND);

	pPrimoPanel->SetAutoLayout (TRUE);
	pPrimoPanel->SetSizer (pPrimoSizer);


	// The Secundo panel
	wxPanel *pSecundoPanel = new wxPanel (pNotebook, -1);
	pNotebook->AddPage (pSecundoPanel, wxT ("Secundo"));

	wxBoxSizer *pSecundoSizer = new wxBoxSizer (wxHORIZONTAL);

	// Vampires like...
	wxStaticBox *pFeatherStaticBox = new wxStaticBox (pSecundoPanel, -1, wxT ("Vampires like..."));
	wxStaticBoxSizer *pFeatherBox = new wxStaticBoxSizer (pFeatherStaticBox, wxHORIZONTAL);
	wxBoxSizer *pFeatherSizer = new wxBoxSizer (wxVERTICAL);
	pFeatherBox->Add (pFeatherSizer, 1, wxEXPAND);
	m_pCardText = new CardText (pSecundoPanel, -1);
	wxStaticText *pFindLabel = new wxStaticText (pSecundoPanel, -1, wxT ("Find :"));
	wxBoxSizer *pFindSizer = new wxBoxSizer (wxHORIZONTAL);
	pFindSizer->Add (pFindLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
	m_pCardInput = new wxTextCtrl (pSecundoPanel, ID_CARD_INPUT, wxT (""), 
		wxDefaultPosition, wxDefaultSize,
		wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
	m_pCardInput->SetToolTip (wxT ("Type in the name of a vampire"));
	pFindSizer->Add (m_pCardInput, 1);


	m_pCardPicker = new wxListBox (pSecundoPanel, ID_CARD_PICKER, wxDefaultPosition, 
		wxDefaultSize, 0, NULL, 
		wxLB_SINGLE | wxLB_ALWAYS_SB);
	FillCardPicker ();
	pFeatherSizer->Add (pFindSizer, 0, wxEXPAND | wxTOP, 5);
	pFeatherSizer->Add (m_pCardPicker, 1, wxEXPAND);

	pFeatherSizer->Add (m_pCardText, 1, wxEXPAND | wxTOP | wxBOTTOM, 5);

	wxStaticText *pFeatherCapLabel = new wxStaticText (pSecundoPanel, -1, wxT ("Prefered capacity :"));
	pFeatherSizer->Add (pFeatherCapLabel, 0, wxEXPAND);
	m_pFeatherCapSlider = new wxSlider (pSecundoPanel, -1, 5, 1, 11, wxDefaultPosition, wxSize (100, -1), wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
	pFeatherSizer->Add (m_pFeatherCapSlider, 0, wxEXPAND);
	m_pFeatherGrpCheckbox = new wxCheckBox (pSecundoPanel, -1, wxT ("Only from compatible groups"));
	m_pFeatherGrpCheckbox->SetValue (TRUE);
	pFeatherSizer->Add (m_pFeatherGrpCheckbox, 0, wxEXPAND);

	pSecundoSizer->Add (pFeatherBox, 1, wxEXPAND | wxALL, 5);

	// Editions
	wxBoxSizer *pSecundoRightSizer = new wxBoxSizer (wxVERTICAL);
	pSecundoSizer->Add (pSecundoRightSizer, 1, wxEXPAND | wxALL, 5);

	wxStaticBox *pEditionStaticBox = new wxStaticBox (pSecundoPanel, -1, wxT ("By set"));
	wxStaticBoxSizer *pEditionBox = new wxStaticBoxSizer (pEditionStaticBox, wxHORIZONTAL);
	wxBoxSizer *pEditionSizer = new wxBoxSizer (wxVERTICAL);
	pEditionBox->Add (pEditionSizer, 1, wxEXPAND);
	m_pEditionList = new wxListView (pSecundoPanel, -1, wxDefaultPosition, wxSize (250, 75), wxLC_REPORT | wxLC_NO_HEADER);
	m_pEditionList->InsertColumn (0, wxEmptyString);
	for (unsigned int i = 0; i < pUIData->GetEditions ()->GetCount (); i++)
	{
		m_pEditionList->InsertItem (i, pUIData->GetEditions ()->Item (i));
	}
	pEditionSizer->Add (m_pEditionList, 1, wxEXPAND | wxTOP, 5);
	pSecundoRightSizer->Add (pEditionBox, 1, wxEXPAND);

	// Rarity
	wxStaticBox *pRarityStaticBox = new wxStaticBox (pSecundoPanel, -1, wxT ("By rarity"));
	wxStaticBoxSizer *pRarityBox = new wxStaticBoxSizer (pRarityStaticBox, wxHORIZONTAL);
	wxBoxSizer *pRaritySizer = new wxBoxSizer (wxVERTICAL);
	pRarityBox->Add (pRaritySizer, 1, wxEXPAND);
	m_pRarityList = new wxListView (pSecundoPanel, -1, wxDefaultPosition, wxSize (250, 75), wxLC_REPORT | wxLC_NO_HEADER);
	m_pRarityList->InsertColumn (0, wxEmptyString);
	for (unsigned int i = 0; i < pUIData->GetRarities ()->GetCount (); i++)
	{
		m_pRarityList->InsertItem (i, pUIData->GetRarities ()->Item (i)[0]);
	}
	pRaritySizer->Add (m_pRarityList, 1, wxEXPAND | wxTOP, 5);
	pSecundoRightSizer->Add (pRarityBox, 1, wxEXPAND);

	pSecundoPanel->SetAutoLayout (TRUE);
	pSecundoPanel->SetSizer (pSecundoSizer);


	// OK, Clear and Cancel buttons
	wxBoxSizer *pOKCancelSizer = new wxBoxSizer (wxHORIZONTAL);
	m_pOKButton = new wxButton (this, ID_OK_BUTTON, wxT ("OK"));
	pOKCancelSizer->Add (m_pOKButton, 0, wxRIGHT | wxALIGN_BOTTOM, 5);
	m_pClearButton = new wxButton (this, ID_CLEAR_BUTTON, wxT ("Clear All"));
	pOKCancelSizer->Add (m_pClearButton, 0, wxRIGHT | wxALIGN_BOTTOM, 5);
	m_pCancelButton = new wxButton (this, ID_CANCEL_BUTTON, wxT ("Cancel"));
	pOKCancelSizer->Add (m_pCancelButton, 0, wxRIGHT | wxALIGN_BOTTOM, 5);

	SetIcon (*g_pIcon);  

	pPapaSizer->Add (pNotebook, 1, wxEXPAND | wxLEFT, 5);
	pPapaSizer->Add (pOKCancelSizer, 0, wxALIGN_RIGHT | wxALL, 5);
	pPapaSizer->Layout ();
	pPapaSizer->Fit (this);

	m_pTitleList->SetColumnWidth (0, m_pTitleList->GetClientSize ().GetWidth () - 20);
	m_pClanList->SetColumnWidth (0, m_pClanList->GetClientSize ().GetWidth () - 20);
	m_pSectList->SetColumnWidth (0, m_pSectList->GetClientSize ().GetWidth () - 20);
	m_pGroupList->SetColumnWidth (0, m_pGroupList->GetClientSize ().GetWidth () - 20);
	m_pEditionList->SetColumnWidth (0, m_pEditionList->GetClientSize ().GetWidth () - 20);
	m_pRarityList->SetColumnWidth (0, m_pRarityList->GetClientSize ().GetWidth () - 20);
}


void
BrowserCryptFilter::FillCardPicker ()
{
	Database *pDatabase = Database::Instance ();
	wxString sFullCardName, sQuery;

	sQuery = wxT ("SELECT DISTINCT card_name, advanced "
		"FROM crypt_view "
		"ORDER BY dumbitdown(card_name) ASC, advanced");

	pDatabase->Query (sQuery, &m_oNameList);

	m_pCardPicker->Freeze ();
	for (unsigned int i = 0; i < m_oNameList.GetCount (); i++)
	{
		// Is it an advanced vampire ?
		if (!m_oNameList.Item (i).Item (1).IsEmpty ())
		{
			m_pCardPicker->Append (m_oNameList.Item (i).Item (0).Append (wxT (" [Advanced]")).c_str ());
		}
		else // regular vampire
		{
			m_pCardPicker->Append (m_oNameList.Item (i).Item (0).c_str ());
		}


		// associated data is a pointer to the corresponding Record
		m_pCardPicker->SetClientData (i, &m_oNameList.Item (i));

		// Fill the other name array for "speling"
		m_oArrayOfNames.Add (m_oNameList.Item (i).Item (0));
	}

	m_pCardPicker->Thaw ();
}


void
BrowserCryptFilter::Reset () 
{
	InterfaceData *pUIData = InterfaceData::Instance ();

	for (unsigned int i = 0; i < pUIData->GetDisciplines ()->GetCount (); i++)
	{
		m_oDisciplinesCombos[i]->SetSelection (0);
	}
	for (unsigned int i = 0; i < pUIData->GetSpecials ()->GetCount (); i++)
	{
		m_apSpecials[i]->SetValue (FALSE);
	}
	m_pOtherText->Clear ();
	m_pCardNameText->Clear ();
	for (int i = 0; i < m_pTitleList->GetItemCount (); i++)
	{
		m_pTitleList->Select (i, FALSE);
	}
	for (int i = 0; i < m_pClanList->GetItemCount (); i++)
	{
		m_pClanList->Select (i, FALSE);
	}
	m_pCapEqualSlider->SetValue (6);
	m_pCapGreaterSlider->SetValue (4);
	m_pCapLesserSlider->SetValue (8);
	m_pCapEqualCheckbox->SetValue (FALSE);
	m_pCapGreaterCheckbox->SetValue (FALSE);
	m_pCapLesserCheckbox->SetValue (FALSE);  
	for (int i = 0; i < m_pSectList->GetItemCount (); i++)
	{
		m_pSectList->Select (i, FALSE);
	}
	for (int i = 0; i < m_pGroupList->GetItemCount (); i++)
	{
		m_pGroupList->Select (i, FALSE);
	}
	for (int i = 0; i < m_pEditionList->GetItemCount (); i++)
	{
		m_pEditionList->Select (i, FALSE);
	}
	for (int i = 0; i < m_pRarityList->GetItemCount (); i++)
	{
		m_pRarityList->Select (i, FALSE);
	}
	m_pCardInput->Clear ();
	m_pCardText->Clear ();
	m_pCardPicker->SetSelection (m_pCardPicker->GetSelection (), FALSE);
	m_lFeatherRef = -1;
}


void
BrowserCryptFilter::SetDialogTitle (int iMode)
{
	// Set the dialog's title
	switch (iMode)
	{
	case CRYPT_FILTER_ADD:
		SetTitle (wxT ("Add vampires..."));
		break;
	case CRYPT_FILTER_REMOVE:
		SetTitle (wxT ("Remove vampires..."));
		break;
	case CRYPT_FILTER_KEEP:
		SetTitle (wxT ("Select vampires..."));
		break;
	}
}


wxSizer *
BrowserCryptFilter::BuildDiscipline (unsigned int uiDisciplineNumber, wxSizer *pContainer, wxWindow *pWindow) 
{
	InterfaceData *pUIData = InterfaceData::Instance ();
	wxBoxSizer *pSizer;
	wxComboBox *pCombo;
	wxStaticText *pLabel;

	pSizer = new wxBoxSizer (wxHORIZONTAL);
	pLabel = new wxStaticText (pWindow, -1, pUIData->GetDisciplines ()->Item (uiDisciplineNumber)[0]);
	pCombo = new wxComboBox (pWindow, -1, wxT (""), wxDefaultPosition, wxSize (120, -1), 0, 0, wxCB_READONLY);

	pCombo->Append (wxT (""));
	pCombo->Append (wxT ("Inferior"));
	pCombo->Append (wxT ("At least Inferior"));
	pCombo->Append (wxT ("Superior"));
	pCombo->Append (wxT ("None"));
	//  pCombo->SetSelection (-1); // I don't think it's of any use.

	wxString sIconName;
	sIconName.Printf (wxT ("Discipline %s"),
		pUIData->GetDisciplines ()->Item (uiDisciplineNumber)[2].c_str ());
	sIconName.Replace (wxT ("%"), wxT (""));

	wxStaticBitmap *pIcon = pUIData->MakeStaticBitmap (pWindow, sIconName);
	if (pIcon) pSizer->Add (pIcon, 0, wxRIGHT, 5);

	pSizer->Add (pLabel, 1, wxALIGN_CENTER | wxRIGHT, 5);
	pSizer->Add (pCombo, 0);
	pContainer->Add (pSizer, 0, wxEXPAND);

	m_oDisciplinesCombos.Add (pCombo);

	return pSizer;
}


void
BrowserCryptFilter::BuildSpecial (unsigned int uiSpecialNumber, wxSizer *pContainer, wxWindow *pWindow) 
{
	InterfaceData *pUIData = InterfaceData::Instance ();
	wxCheckBox *pCheckBox;

	pCheckBox = new wxCheckBox(pWindow, -1, pUIData->GetSpecials()->Item(uiSpecialNumber)[0],wxDefaultPosition,wxDefaultSize,wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);

	int iSpace=0;
#ifdef __WXMSW__
	iSpace=3;
#endif
#ifdef __WXMAC__
	iSpace=11;
#endif
	
	pContainer->Add (pCheckBox, 0, wxALL, iSpace);
	m_apSpecials[uiSpecialNumber] = pCheckBox;
}


void 
BrowserCryptFilter::OnCancelButtonClick (wxCommandEvent& WXUNUSED (event))
{
	EndModal (0);
}


void
BrowserCryptFilter::OnCapEqualScroll (wxScrollEvent& WXUNUSED (event))
{
	m_pCapEqualCheckbox->SetValue (TRUE);
}


void
BrowserCryptFilter::OnCapGreaterScroll (wxScrollEvent& WXUNUSED (event))
{
	m_pCapGreaterCheckbox->SetValue (TRUE);
}


void
BrowserCryptFilter::OnCapLesserScroll (wxScrollEvent& WXUNUSED (event))
{
	m_pCapLesserCheckbox->SetValue (TRUE);
}


void 
BrowserCryptFilter::OnClearButtonClick (wxCommandEvent& WXUNUSED (event))
{
	Reset ();
}


void 
BrowserCryptFilter::OnOKButtonClick (wxCommandEvent& WXUNUSED (event))
{
	m_pController->UpdateModel(this);
	EndModal(1);
}


void
BrowserCryptFilter::OnCardInputChanged (wxCommandEvent& WXUNUSED (event))
{
	bool bSearchLower = TRUE, bSearchUpper = TRUE;
	int iIndex = 0, iStringLength;

	if (m_bNoEvents) return;

	iStringLength = m_pCardInput->GetValue ().Len ();

	if (iStringLength <= 0) 
	{
		return;
	}

	m_iCycleLowerValue = -1;
	m_iCycleUpperValue = -1;

	// search for the first card that meets the description
	while (bSearchLower)
	{
		// Here we search card names
		bSearchLower = m_pCardInput->GetValue ().CmpNoCase (m_pCardPicker->GetString (iIndex).Left (iStringLength).c_str ()) != 0;
		if (!bSearchLower)
		{
			m_iCycleLowerValue = iIndex;
		}
		iIndex++;
		if (iIndex >= m_pCardPicker->GetCount ())
		{
			bSearchLower = FALSE;
		}
	}
	if (m_iCycleLowerValue >= 0)
	{
		// search for the last card that meets the description
		while (bSearchUpper)
		{
			// Here we search card names
			bSearchUpper = m_pCardInput->GetValue ().CmpNoCase (m_pCardPicker->GetString (iIndex).Left (iStringLength).c_str ()) == 0;
			if (!bSearchUpper)
			{
				m_iCycleUpperValue = iIndex;
			}
			iIndex++;
			if (iIndex >= m_pCardPicker->GetCount ())
			{
				bSearchUpper = FALSE;
				m_iCycleUpperValue = iIndex;
			}
		}
	}

	// If we have found something
	if (m_iCycleLowerValue != -1)
	{
		m_iCycleCounter = 0;

		m_pCardPicker->SetSelection (m_iCycleLowerValue);
		m_pCardPicker->SetFirstItem (m_iCycleLowerValue);
	}
	else 
		// Use our phat speling powaz
	{
		int iRet;
		wxString sVal (m_pCardInput->GetValue ());
		if ((iRet = check_speling (sVal, m_oArrayOfNames)) >= 0)
		{
			m_iCycleCounter = 0;
			m_iCycleLowerValue = iRet;
			m_iCycleUpperValue = iRet;
			m_pCardPicker->SetSelection (iRet);
			m_pCardPicker->SetFirstItem (iRet);
		}
	}

	// Force a call here since the selection event doesn't seem to be triggered
	wxCommandEvent oEvt;
	OnCardPickerSelection (oEvt);

}


void
BrowserCryptFilter::OnCardPickerSelection (wxCommandEvent& WXUNUSED (event))
{
	Database *pDatabase = Database::Instance ();
	wxString sQuery, sInfo, sTemp, sAdvanced = wxT ("");

	if (m_bNoEvents) return;

	sTemp = m_pCardPicker->GetStringSelection ();
	if (sTemp.Length () > 0)
	{
		sTemp.Replace (wxT ("'"), wxT ("''"));

		if (sTemp.Replace (wxT (" [Advanced]"), wxT ("")))
		{
			sAdvanced = wxT ("Advanced");
		}

		sQuery.Printf (wxT ("SELECT card_ref "
			"FROM crypt_view "
			"WHERE card_name = '%s' "
			"      AND advanced = '%s' ;"),
			sTemp.c_str (),
			sAdvanced.c_str ());

		pDatabase->Query (sQuery.c_str (), &m_oCardList);

		if (m_oCardList.GetCount () != 0)
		{
			m_oCardList.Item (0).Item (0).ToLong (&m_lFeatherRef);
			m_pCardText->DisplayCryptText (m_lFeatherRef,NULL);
		}
		else
		{
			wxLogError (wxT ("Can't find reference in database."));
		}
	}
}


