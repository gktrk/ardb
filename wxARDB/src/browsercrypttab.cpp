/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
*
*  Copyright (C) 2002 Francois Gombault
*  gombault.francois@wanadoo.fr
*
*  Copyright (C) 2007 Graham Smith
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


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class BrowserFrame;

#include "browsercrypttab.h" 
#include "deckmodel.h"
#include "inventorymodel.h"
#include "main.h" 
#include "speling.h"

#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/dir.h>

#include "imagedialog.h"




BEGIN_EVENT_TABLE (BrowserCryptTab, wxPanel)
// the top buttons
EVT_BUTTON (ID_ADD_BUTTON, BrowserCryptTab::OnAddButtonClick)
EVT_BUTTON (ID_REMOVE_BUTTON, BrowserCryptTab::OnRemoveButtonClick)
EVT_BUTTON (ID_KEEP_BUTTON, BrowserCryptTab::OnKeepButtonClick)
EVT_BUTTON (ID_RESET_BUTTON, BrowserCryptTab::OnResetButtonClick)
EVT_BUTTON (ID_INVENTORY_BUTTON, BrowserCryptTab::OnInventoryButtonClick)
// the find text field
EVT_TEXT (ID_FIND_TEXTCTRL, BrowserCryptTab::OnFindTextChange)

EVT_SPLITTER_SASH_POS_CHANGED(ID_SPLITTER_WINDOW, BrowserCryptTab::OnSplitterMoved)
//  EVT_SIZE (BrowserCryptTab::OnTabResize)

END_EVENT_TABLE ()


BrowserCryptTab::BrowserCryptTab(BrowserCryptModel *pModel, BrowserCryptController *pController, wxNotebook *pParent, unsigned int uiNumber) :
wxPanel (pParent, -1),
// Initialisation of member objects and variables
m_bDisplayInventory (FALSE),
m_lSplitterPosition (430),
m_oArrayOfNames (),
m_oPopupMenu (),
m_oTabSize (),
m_pAddButton (NULL),
m_pController (pController),
m_pFilterGrid (NULL),
m_pInventoryButton (NULL),
m_pKeepButton (NULL),
m_pModel (pModel),
m_pRemoveButton (NULL),
m_pResetButton (NULL),
m_pParent (pParent),
m_pPapaSizer (NULL),
m_pSplitterWindow (NULL),
m_pFindText (NULL),
m_pCardCountLabel (NULL),
m_pCardGrid (NULL),
m_pCardText (NULL),
m_uiCapacityTotal (0),
m_uiFilterListHeight (40),
m_uiNumber (uiNumber),
m_uiSortColumn (0)
{
	Init ();
}


BrowserCryptTab::~BrowserCryptTab()
{
	if (m_pModel) delete m_pModel;
	m_pModel = NULL;

	wxString sConfEntry = wxT ("SplitterPositionC");
	wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
	if (pConfig)
	{
		pConfig->Write (sConfEntry, m_lSplitterPosition);
		pConfig->Flush (TRUE);
	}
}

void BrowserCryptTab::SetFocus()
{
	if (m_pFindText != NULL)
	{
		m_pFindText->SetFocus();
	}
}

void
BrowserCryptTab::Init ()
{
	wxBoxSizer *pFilterSizer;
	wxString sTabName;

	if (m_uiNumber)
	{
		sTabName.Printf (wxT ("Crypt Browser (%d)"), m_uiNumber + 1);
	}
	else
	{
		sTabName.Printf (wxT ("Crypt Browser"));
	}

	m_pParent->AddPage (this, sTabName, TRUE);

	m_pPapaSizer = new wxBoxSizer (wxVERTICAL);
	SetSizer (m_pPapaSizer);

	// Place the find textbox and various filter buttons
	pFilterSizer = new wxBoxSizer (wxHORIZONTAL);

	wxStaticText *pFindLabel = new wxStaticText (this, -1, wxT ("Find :"));
	pFilterSizer->Add (pFindLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
	m_pFindText = new wxTextCtrl (this, ID_FIND_TEXTCTRL, wxT (""), wxDefaultPosition, wxSize (150, -1));
	pFilterSizer->Add (m_pFindText, 0);

	pFilterSizer->Add (10, 0, 1);

	m_pKeepButton = new wxButton (this, ID_KEEP_BUTTON, wxT ("Select..."));
	pFilterSizer->Add (m_pKeepButton, 0, wxRIGHT, 5);
	m_pAddButton = new wxButton (this, ID_ADD_BUTTON, wxT ("Add..."));
	pFilterSizer->Add (m_pAddButton, 0, wxRIGHT, 5);
	m_pRemoveButton = new wxButton (this, ID_REMOVE_BUTTON, wxT ("Remove..."));
	pFilterSizer->Add (m_pRemoveButton, 0, wxRIGHT, 5);
	m_pResetButton = new wxButton (this, ID_RESET_BUTTON, wxT ("Reset"));
	pFilterSizer->Add (m_pResetButton, 0, wxRIGHT, 5);

	m_pPapaSizer->Add (pFilterSizer, 0, wxEXPAND | wxALL, 5);

	// The filter list
	m_pFilterGrid = new BrowserCryptFilterGrid (m_pModel, this);
	m_pFilterGrid->SetToolTip (wxT ("Right click for menu"));
	m_pFilterGrid->SetSize (100, m_uiFilterListHeight);
	m_pFilterGrid->SetColLabelSize (0);
	m_pFilterGrid->SetRowLabelSize (0);
	m_pFilterGrid->EnableGridLines (TRUE);
	m_pFilterGrid->SetGridLineColour (this->GetBackgroundColour ());
	m_pFilterGrid->SetMargins (0, 0);
	m_pFilterGrid->CreateGrid (1, 1);
	m_pFilterGrid->SetRowSize (0, m_uiFilterListHeight - 1);

	m_pPapaSizer->Add (m_pFilterGrid, 0, wxEXPAND);

	// Place the vampire count label and inventory button
	wxBoxSizer *pInvCountSizer = new wxBoxSizer (wxHORIZONTAL);
	m_pInventoryButton = new wxButton (this, ID_INVENTORY_BUTTON, wxT ("Inventory"));
	pInvCountSizer->Add (m_pInventoryButton, 0, wxALL, 5);
	m_pCardCountLabel = new wxStaticText (this, -1, wxT ("two\nlines"), wxDefaultPosition, wxDefaultSize);
	pInvCountSizer->Add (m_pCardCountLabel, 1, wxEXPAND);
	m_pPapaSizer->Add (pInvCountSizer, 0, wxEXPAND);

	// Insert the splitter window
	m_pSplitterWindow = new wxSplitterWindow (this, ID_SPLITTER_WINDOW, wxDefaultPosition, wxDefaultSize,
		wxSP_3D | wxSP_LIVE_UPDATE);

	m_pSplitterWindow->SetMinimumPaneSize (20);

	// Create the vampire list
	m_pCardGrid = new BrowserCryptCardGrid (m_pModel, this, m_pSplitterWindow);
	m_pCardGrid->CreateGrid (0, 12);
	m_pCardGrid->SetSelectionMode (wxGrid::wxGridSelectRows);
	m_pCardGrid->SetGridLineColour (this->GetBackgroundColour ());
	m_pCardGrid->SetToolTip (wxT ("Double click adds to deck,\nRight click for menu"));
	m_pCardGrid->SetRowLabelSize (0);
	m_pCardGrid->SetColLabelSize (20);
	m_pCardGrid->SetColMinimalAcceptableWidth (0);
	m_pCardGrid->SetColLabelValue (0, wxT ("Have"));
	m_pCardGrid->SetColLabelValue (1, wxT ("Want"));
	m_pCardGrid->SetColLabelValue (2, wxT ("Spare"));
	m_pCardGrid->SetColLabelValue (3, wxT ("Name"));
	m_pCardGrid->SetColLabelValue (4, wxT ("Adv"));
	m_pCardGrid->SetColLabelValue (5, wxT ("Cap"));
	m_pCardGrid->SetColLabelValue (6, wxT ("Disciplines"));
	m_pCardGrid->SetColLabelValue (7, wxT ("Clan"));
	m_pCardGrid->SetColLabelValue (8, wxT ("Title"));
	m_pCardGrid->SetColLabelValue (9, wxT ("Grp"));
	m_pCardGrid->SetColLabelValue (10, wxT ("Text"));
	m_pCardGrid->SetColLabelValue (11, wxT ("Id"));

	wxPanel *pCardTextPanel = new wxPanel(m_pSplitterWindow,-1);
	wxBoxSizer *pCardTextSizer = new wxBoxSizer (wxHORIZONTAL);

	m_pCardText = new CardText (pCardTextPanel, -1);
	
	m_pCardViewer = NULL;

	if (wxDir::Exists(CARD_IMAGE_DIR))
	{
		m_pCardViewer = new CardViewer(pCardTextPanel,wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	}

	pCardTextSizer->Add(m_pCardText, 3, wxEXPAND);

	if (m_pCardViewer != NULL)
	{
		pCardTextSizer->Add(m_pCardViewer, 1, wxEXPAND|wxSHAPED);
	}

	pCardTextPanel->SetSizer(pCardTextSizer);


	// Create vampire text field
	m_pCardGrid->Show(TRUE);
	pCardTextPanel->Show(TRUE);

	wxString sConfEntry = wxT ("SplitterPositionC");
	wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
	if (pConfig)
	{
		if (!pConfig->Read (sConfEntry, &m_lSplitterPosition))
		{
			pConfig->Write (sConfEntry, m_lSplitterPosition);
			pConfig->Flush (TRUE);
		}
	}

	m_pPapaSizer->Add (m_pSplitterWindow, 1, wxEXPAND|wxFIXED_MINSIZE);
	m_pPapaSizer->Layout();

#ifdef __WXMSW__
	if (!m_uiNumber)
	{
		//Required for Windows.  Without controls, are not displayed.
		m_pPapaSizer->Fit(m_pParent);
	}
#endif

	// Put both in the splitter window
	if (m_pSplitterWindow)
	{
		m_pSplitterWindow->SetSashGravity(0.5);
	}

	m_pSplitterWindow->SplitHorizontally (m_pCardGrid, pCardTextPanel, m_lSplitterPosition);

	m_oTabSize = GetSize ();
}


void
BrowserCryptTab::UpdateView ()
{
	if (!m_pCardGrid || !m_pModel) return;

	// update display of inventory columns
	wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
	if (pConfig)
	{
		wxString sDisplayInventory 
			= wxT ("DisplayInventory");
		if (!pConfig->Read (sDisplayInventory, &m_bDisplayInventory))
		{
			pConfig->Write (sDisplayInventory, m_bDisplayInventory);
			pConfig->Flush (TRUE);
		}
	}

	UpdateFilterList ();

	FillCardList ();

	if (m_pCardGrid->GetNumberRows () > 0) 
	{
		for (unsigned int iCol = 0; iCol < 3; iCol++)
		{
			if (m_bDisplayInventory)
				m_pCardGrid->AutoSizeColumn (iCol);
			else
				m_pCardGrid->SetColSize (iCol, 0);
		}

		// Size last column (Id) to 0
		m_pCardGrid->SetColSize (m_pCardGrid->GetNumberCols () - 1, 0);

		for (int iCol = 3; iCol < m_pCardGrid->GetNumberCols () - 1; iCol++)
		{
			m_pCardGrid->AutoSizeColumn (iCol);
			if (m_pCardGrid->GetColSize (iCol) < 30) 
			{
				m_pCardGrid->SetColSize (iCol, 30);
			}
		}
	}

	SetCardCount (m_pModel->GetCardCount ());
	SetCardText ();

	SetCursor (*wxSTANDARD_CURSOR);
	m_pCardGrid->ForceRefresh ();
	m_pCardGrid->Scroll (0,0);
}


void
BrowserCryptTab::UpdateFilterList ()
{
	unsigned int i;
	wxListItem oItem;

	if (!m_pFilterGrid || !m_pModel) return;

	if (m_pModel->GetFilterCount () > 0) 
	{
		m_pFilterGrid->Freeze ();

		m_pFilterGrid->DeleteCols (0, m_pFilterGrid->GetNumberCols (), FALSE);
		m_pFilterGrid->AppendCols (m_pModel->GetFilterCount (), FALSE);
		for (i = 0; i < m_pModel->GetFilterCount (); i++)
		{
			m_pFilterGrid->SetCellValue (0, i, m_pModel->GetFilterSummary (i));
			m_pFilterGrid->SetReadOnly (0, i);
			m_pFilterGrid->AutoSizeColumn (i);
			if (m_pModel->IsFilterVisible (i))
			{
				m_pFilterGrid->SetCellBackgroundColour(0, i, wxColour (100, 180, 100));
			}
			else
			{
				m_pFilterGrid->SetCellBackgroundColour(0, i, wxColour (180, 100, 100));
			}
		}
		//if (m_pPapaSizer) m_pPapaSizer->Layout ();

		m_pFilterGrid->MakeCellVisible (0, i - 1);
		m_pFilterGrid->AutoSizeRow (0);
		m_pFilterGrid->SetClientSize (100, m_pFilterGrid->GetRowSize (0));

		if (m_pPapaSizer->GetItem (m_pFilterGrid) == NULL)
		{
			m_pPapaSizer->Insert (1, m_pFilterGrid, 0, wxEXPAND);
		}
		m_pFilterGrid->Show ();
		m_pFilterGrid->Thaw ();
	}
	else
	{
		if (m_pPapaSizer->GetItem (m_pFilterGrid) != NULL)
		{
			m_pPapaSizer->Detach (m_pFilterGrid);
		}
		m_pFilterGrid->Hide ();
	}
	m_pPapaSizer->Layout ();
}


void
BrowserCryptTab::FillCardList () 
{
	long lCapacity, lGroup;
	wxString sCopy;

	//  wxStopWatch sw;

	if (!m_pCardGrid || !m_pModel || !m_pModel->GetCardList ()) return;

	m_oArrayOfNames.Clear ();
	m_uiCapacityTotal = 0;
	for (unsigned int i = 0; i < 50; i++)
	{
		m_acGroupCounts[i] = 0;
	}

	m_pCardGrid->Freeze ();
	m_pCardGrid->BeginBatch ();

	if (m_pCardGrid->GetNumberRows() > 0)
	{
		m_pCardGrid->DeleteRows (0, m_pCardGrid->GetNumberRows ());
	}

	m_pCardGrid->AppendRows (m_pModel->GetCardCount (), FALSE);

	for (unsigned int iLine = 0; iLine < m_pModel->GetCardCount (); iLine++)
	{
		m_oArrayOfNames.Add (m_pModel->GetCardList ()->Item (iLine).Item (13));
		m_pCardGrid->AutoSizeRow (iLine);

		for (int iCol = 0; iCol < m_pCardGrid->GetNumberCols (); iCol++)
		{
			sCopy = m_pModel->GetCardList ()->Item (iLine).Item (iCol);
			sCopy.Replace (wxT ("\r"), wxT (" "));
			sCopy.Replace (wxT ("\n"), wxT (" "));

			m_pCardGrid->SetCellValue (iLine, iCol, sCopy);

		}
		// Add this vampire's capacity to the total
		if (m_pModel->GetCardList()->Item (iLine).Item (5).ToLong (&lCapacity))
		{
			m_uiCapacityTotal += lCapacity;
		}
		// Remember the vampire's group
		if (m_pModel->GetCardList()->Item (iLine).Item (9).ToLong (&lGroup))
		{
			m_acGroupCounts[lGroup] = m_acGroupCounts[lGroup] + 1;
		}
	}

	//  Fit ();
	m_pCardGrid->MakeCellVisible (0, 0);
	m_pCardGrid->EndBatch ();
	m_pCardGrid->Thaw ();

	//  wxLogMessage (_T("items inserted in %ldms\n"), sw.Time());
}


void
BrowserCryptTab::SetCardText (long lCardRef)
{
	if (!m_pCardText) return;

	if (lCardRef == -1)
	{
		m_pCardText->Clear();

		if (m_pCardViewer != NULL)
		{
			m_pCardViewer->Clear();
		}
	}
	else 
	{
		wxArrayString cardNames;
		m_pCardText->DisplayCryptText(lCardRef,&cardNames);

		if (m_pCardViewer != NULL)
		{
			if (cardNames.Count() > 0)
			{
				m_pCardViewer->SetImage(&cardNames);
			}
			else
			{
				m_pCardViewer->Clear();
			}
		}
	}
}


void
BrowserCryptTab::SetCardCount (unsigned int uiCount)
{
	InterfaceData *pUIData = InterfaceData::Instance ();
	wxString sLabel, sBestGroups;
	float fAverageCap;
	unsigned int i, lBestGroup = 1;

	if (uiCount > 0 && m_uiCapacityTotal > 0) 
	{
		//Fix group count
		for (i = 1; i <= g_uiGroupCount; i++)
		{
			if (m_acGroupCounts[i] + m_acGroupCounts [i - 1] >= m_acGroupCounts[lBestGroup] + m_acGroupCounts[lBestGroup - 1])
			{
				lBestGroup = i;
			}
		}
		
		if (m_acGroupCounts[lBestGroup] + m_acGroupCounts [lBestGroup - 1] > 0)
		{
			sBestGroups << wxT(" (") << m_acGroupCounts [lBestGroup-1] + m_acGroupCounts[lBestGroup] << wxT (" in groups ") << lBestGroup - 1 << wxT ("-") << lBestGroup << wxT (")");
		}

		fAverageCap = m_uiCapacityTotal;
		fAverageCap /= uiCount; 
		sLabel.Printf (wxT ("Crypt count: %d %s    Average capacity: %.2f"), uiCount, sBestGroups.c_str (), fAverageCap);

		// Compute best disciplines
		Database *pDatabase = Database::Instance ();
		int iMaxCards = 100;
		wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
		if (pConfig)
		{
			wxString sPrimeDisciplinesMaxCards = wxT ("PrimeDisciplinesMaxCards");
			if (!pConfig->Read (sPrimeDisciplinesMaxCards, &iMaxCards))
			{
				pConfig->Write (sPrimeDisciplinesMaxCards, iMaxCards);
				pConfig->Flush (TRUE);
			}
		}

		if (pDatabase && 
			m_pModel->GetCardCount () < (unsigned int) iMaxCards) 
		{
			HappyList oHappyList;
			HappyBucket *pBucket, *pLastBucket;
			wxString sQuery, sLowerCaseDiscName;
			long lCount;
			RecordSet *pRecordSet;

			for (unsigned int i = 0; i < pUIData->GetDisciplines ()->GetCount (); i++)
			{
				sLowerCaseDiscName = pUIData->GetDisciplines ()->Item (i)[0];
				sLowerCaseDiscName = sLowerCaseDiscName.MakeLower ();

				sQuery.Printf (wxT ("SELECT '%s ', sum(cards_crypt.%s) FROM cards_crypt WHERE cards_crypt.record_num IN (SELECT card_name FROM crypt_selection WHERE browser_num='%d');"), pUIData->GetDisciplines ()->Item (i)[0].c_str (), sLowerCaseDiscName.c_str (), m_pModel->GetIDNumber ());

				pRecordSet = pDatabase->Query (sQuery);
				if (pRecordSet && pRecordSet->Count ()) 
				{
					if (pRecordSet->Item (0).Item (1).ToLong (&lCount) && lCount > 0)
					{
						oHappyList.Append (new HappyBucket (pRecordSet->Item (0).Item (0), lCount));
					}
				}
			}
			// Sort and get the top ranked disciplines
			oHappyList.Sort (HappyBucket::Compare);

			// The last discipline might be a tie, if so concatenate the names
			// And remove the extra disciplines, if any
			unsigned int uiIndex = 4;
			if (oHappyList.GetCount () > uiIndex)
			{
				pLastBucket = oHappyList.Item (uiIndex - 1)->GetData ();
				while (uiIndex < oHappyList.GetCount () && oHappyList.Item (uiIndex))
				{
					pBucket = oHappyList.Item (uiIndex)->GetData ();
					if (pBucket->m_uiDiscTotal == pLastBucket->m_uiDiscTotal) 
					{
						pLastBucket->m_sName << wxT ("/ ") << pBucket->m_sName;
					}
					oHappyList.DeleteNode (oHappyList.Item (uiIndex));
				}
			}

			sLabel.Append (wxT ("\nPrime disciplines: "));

			for (unsigned int i = 0; i < oHappyList.GetCount (); i++)
			{
				pBucket = oHappyList.Item (i)->GetData ();
				// We're not going to display too many extra disciplines here,
				// hence the limit on string length
				if (pBucket->m_sName.Length () < 20)
					sLabel.Append (pBucket->m_sName + wxT (" "));
			}
		}
	}
	else
	{
		sLabel.Printf (wxT ("Crypt count: %d"), uiCount);
	}

	m_pCardCountLabel->SetLabel (sLabel);
}


void
BrowserCryptTab::OnAddButtonClick (wxCommandEvent& WXUNUSED (event))
{
	BrowserCryptFilter oDialog (m_pController);

	m_pController->SetFilterMode (CRYPT_FILTER_ADD);
	oDialog.CentreOnParent ();
	oDialog.SetDialogTitle (CRYPT_FILTER_ADD);
	oDialog.Reset ();
	oDialog.ShowModal ();
}


void
BrowserCryptTab::OnRemoveButtonClick (wxCommandEvent& WXUNUSED (event))
{
	BrowserCryptFilter oDialog (m_pController);

	m_pController->SetFilterMode (CRYPT_FILTER_REMOVE);
	oDialog.CentreOnParent ();
	oDialog.SetDialogTitle (CRYPT_FILTER_REMOVE);
	oDialog.Reset ();
	oDialog.ShowModal ();
}


void
BrowserCryptTab::OnInventoryButtonClick (wxCommandEvent& WXUNUSED (event))
{
	// Toggle mode
	m_bDisplayInventory = !m_bDisplayInventory;

	m_pCardGrid->DisableCellEditControl ();

	//save display preference
	wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
	if (pConfig)
	{
		wxString sDisplayInventory = wxT ("DisplayInventory");
		pConfig->Write (sDisplayInventory, m_bDisplayInventory);
		pConfig->Flush (TRUE);
	}

	// update display
	for (unsigned int iCol = 0; iCol < 3; iCol++)
	{
		if (m_bDisplayInventory)
			m_pCardGrid->AutoSizeColumn (iCol);
		else
			m_pCardGrid->SetColSize (iCol, 0);
	}
	m_pCardGrid->ForceRefresh ();

}


void
BrowserCryptTab::OnKeepButtonClick (wxCommandEvent& WXUNUSED (event))
{
	BrowserCryptFilter oDialog (m_pController);

	m_pController->SetFilterMode (CRYPT_FILTER_KEEP);
	oDialog.CentreOnParent ();
	oDialog.SetDialogTitle (CRYPT_FILTER_KEEP);
	oDialog.Reset ();
	oDialog.ShowModal ();
}


void
BrowserCryptTab::OnResetButtonClick (wxCommandEvent& WXUNUSED (event))
{
	SetCursor (*wxHOURGLASS_CURSOR);

	m_pModel->Reset ();
}


void
BrowserCryptTab::OnSplitterMoved (wxSplitterEvent& event)
{
	if (m_pCardGrid->MyGetSelectedRows ().Count ())
	{
		m_pCardGrid->MakeCellVisible (m_pCardGrid->MyGetSelectedRows ().Item (0), 0);
	}

	if (m_pSplitterWindow &&
		m_pSplitterWindow->GetSashPosition () > 0)
		m_lSplitterPosition = m_pSplitterWindow->GetSashPosition ();

	event.Skip ();
}


void
BrowserCryptTab::OnTabResize (wxSizeEvent &event)
{
	/*
	int iDeltaHeight, iSashPosition;

	if (m_pSplitterWindow != NULL)
	{
	iDeltaHeight = event.GetSize ().GetHeight () - m_oTabSize.GetHeight ();
	iSashPosition = m_pSplitterWindow->GetSashPosition ();
	m_pSplitterWindow->SetSashPosition (iSashPosition + iDeltaHeight);
	m_oTabSize = event.GetSize ();
	}
	*/

	event.Skip ();
}


void
BrowserCryptTab::OnFindTextChange (wxCommandEvent& WXUNUSED (event))
{
	bool bSearch = TRUE;
	int iCard = 0;
	long lCardRef;
	wxArrayString *pRecord, *pMatch = 0;
	int iStringLength = m_pFindText->GetValue ().Len ();

	m_pCardGrid->Freeze ();
	while (bSearch && (unsigned int) iCard < m_pModel->GetCardCount ())
	{
		// Here we search vampire names
		pRecord = &(m_pModel->GetCardList ()->Item (iCard));
		bSearch = m_pFindText->GetValue ().CmpNoCase (pRecord->Item (13).Left (iStringLength).c_str ()) != 0;
		if (!bSearch)
		{
			pMatch = pRecord;
		}
		else 
		{
			iCard++;
		}
	}

	// Not found, use our phat speling powaz
	if (bSearch)
	{
		wxString sVal (m_pFindText->GetValue ());
		if ((iCard = check_speling (sVal, m_oArrayOfNames)) >= 0)
		{
			bSearch = FALSE;
		}
	}

	if (bSearch == FALSE)
	{
		// Unselect current vampire(s)
		m_pCardGrid->ClearSelection ();

		m_pCardGrid->MakeCellVisible (m_pCardGrid->GetNumberRows () - 1, 0);
		m_pCardGrid->MakeCellVisible (iCard, 0);
		m_pCardGrid->SelectRow (iCard);
		m_pCardGrid->GetCellValue (iCard, m_pCardGrid->GetNumberCols () -1).ToLong (&lCardRef);
		SetCardText (lCardRef);
	}

	m_pCardGrid->Thaw ();
}







/* ****************************************************************
*
* class BrowserCryptFilterGrid
*
****************************************************************** */


BEGIN_EVENT_TABLE (BrowserCryptFilterGrid, wxGrid)
EVT_GRID_CELL_RIGHT_CLICK (BrowserCryptFilterGrid::OnFilterRightClick)
EVT_MENU (ID_POPUP_REMOVE, BrowserCryptFilterGrid::OnFilterPopupRemove)
EVT_MENU (ID_POPUP_HIDE, BrowserCryptFilterGrid::OnFilterPopupHide)
EVT_MENU (ID_POPUP_MOVELEFT, BrowserCryptFilterGrid::OnFilterPopupMoveLeft)
EVT_MENU (ID_POPUP_MOVERIGHT, BrowserCryptFilterGrid::OnFilterPopupMoveRight)
END_EVENT_TABLE ()


BrowserCryptFilterGrid::BrowserCryptFilterGrid (BrowserCryptModel *pModel, BrowserCryptTab *pParent) :
wxGrid (pParent, -1),
m_oFilterPopupMenu (),
m_pModel (pModel),
m_pParent (pParent)
{
	// Filter List Popup Menu
	m_oFilterPopupMenu.Append (ID_POPUP_REMOVE, wxT ("Remove"));
	m_oFilterPopupMenu.Append (ID_POPUP_HIDE, wxT ("Switch on/off"));
	m_oFilterPopupMenu.Append (ID_POPUP_MOVELEFT, wxT ("Move left"));
	m_oFilterPopupMenu.Append (ID_POPUP_MOVERIGHT, wxT ("Move right"));
}


BrowserCryptFilterGrid::~BrowserCryptFilterGrid ()
{

}


void
BrowserCryptFilterGrid::OnFilterPopupHide (wxCommandEvent& WXUNUSED (event))
{
	bool bState;
	long lFilter;

	// repaint where the popup was
	Update ();

	m_pParent->SetCursor (*wxHOURGLASS_CURSOR);

	for (lFilter = GetNumberCols () - 1; lFilter >= 0; lFilter--)
	{
		if (IsInSelection (0, lFilter))
		{
			bState = m_pModel->IsFilterVisible (lFilter);
			m_pModel->SetFilterVisible (lFilter, !bState, FALSE);
		}
	}
	m_pParent->UpdateView ();
}


void
BrowserCryptFilterGrid::OnFilterPopupMoveLeft (wxCommandEvent& WXUNUSED (event))
{
	long lFilter;

	// repaint where the popup was
	Update ();

	m_pParent->SetCursor (*wxHOURGLASS_CURSOR);

	for (lFilter = 0; lFilter < GetNumberCols (); lFilter++)
	{
		if (IsInSelection (0, lFilter))
		{
			m_pModel->LowerFilter (lFilter, FALSE);
		}
	}
	m_pParent->UpdateView ();

}


void
BrowserCryptFilterGrid::OnFilterPopupMoveRight (wxCommandEvent& WXUNUSED (event))
{
	long lFilter;

	// repaint where the popup was
	Update ();

	m_pParent->SetCursor (*wxHOURGLASS_CURSOR);

	for (lFilter = GetNumberCols () - 1; lFilter >= 0; lFilter--)
	{
		if (IsInSelection (0, lFilter))
		{
			m_pModel->RaiseFilter (lFilter, FALSE);
		}
	}
	m_pParent->UpdateView ();

}


void
BrowserCryptFilterGrid::OnFilterPopupRemove (wxCommandEvent& WXUNUSED (event))
{
	long lFilter;

	// repaint where the popup was
	Update ();

	m_pParent->SetCursor (*wxHOURGLASS_CURSOR);

	for (lFilter = GetNumberCols () - 1; lFilter >= 0; lFilter--)
	{
		if (IsInSelection (0, lFilter))
		{
			m_pModel->DeleteFilter (lFilter, FALSE);
		}
	}
	m_pParent->UpdateView ();
}


void
BrowserCryptFilterGrid::OnFilterRightClick (wxGridEvent &event)
{
	long lClickedFilter;

	lClickedFilter = event.GetCol ();
	if (!IsInSelection (0, lClickedFilter))
	{
		// unselect the other filter(s)
		ClearSelection ();
		// and select this one
		SelectBlock (0, lClickedFilter, 0, lClickedFilter);
	}
	PopupMenu (&m_oFilterPopupMenu, event.GetPosition ());
}


/* ****************************************************************
*
* class BrowserCryptCardGrid
*
****************************************************************** */

BEGIN_EVENT_TABLE (BrowserCryptCardGrid, wxGrid)
EVT_GRID_CELL_CHANGE(BrowserCryptCardGrid::OnInventoryChange)
EVT_GRID_CELL_RIGHT_CLICK (BrowserCryptCardGrid::OnCardRightClick)
EVT_GRID_LABEL_LEFT_CLICK (BrowserCryptCardGrid::OnColumnClick)
EVT_GRID_CELL_LEFT_DCLICK (BrowserCryptCardGrid::OnCardActivated)
EVT_GRID_SELECT_CELL(BrowserCryptCardGrid::OnCardSelect)
EVT_MENU (ID_POPUP_ADD, BrowserCryptCardGrid::OnPopupAddToDeck)
EVT_MENU (ID_POPUP_ADD_ALL, BrowserCryptCardGrid::OnPopupAddAllToDeck)
EVT_MENU (ID_POPUP_REMOVE, BrowserCryptCardGrid::OnPopupRemove)
EVT_MENU (ID_POPUP_COPY_ALL, BrowserCryptCardGrid::OnPopupCopyAll)
END_EVENT_TABLE ()


BrowserCryptCardGrid::BrowserCryptCardGrid (BrowserCryptModel *pModel, BrowserCryptTab *pTab, wxWindow *pParent) :
wxGrid (pParent, -1),
m_bReverseSortOrder (FALSE),
m_oSelection (),
m_oCardPopupMenu (),
m_pModel (pModel),
m_pParent (pParent),
m_pTab (pTab),
m_uiSortColumn (3)
{
	// Card List Popup Menu
	m_oCardPopupMenu.Append (ID_POPUP_ADD, wxT ("Add selected to deck"));
	m_oCardPopupMenu.Append (ID_POPUP_ADD_ALL, wxT ("Add all to deck"));
	m_oCardPopupMenu.Append (ID_POPUP_REMOVE, wxT ("Remove selected"));
	//  m_oCardPopupMenu.AppendSeparator ();
	//   m_oCardPopupMenu.Append (ID_POPUP_COPY_ALL, "Copy all list to clipboard");

}


BrowserCryptCardGrid::~BrowserCryptCardGrid ()
{

}


wxArrayInt
BrowserCryptCardGrid::MyGetSelectedRows ()
{
	wxGridCellCoordsArray upperlefts=GetSelectionBlockTopLeft ();
	wxGridCellCoordsArray lowerrights=GetSelectionBlockBottomRight ();

	m_oSelection.Clear ();
	for(size_t i = 0; i < upperlefts.Count (); ++i)
	{
		for (int j = upperlefts.Item (i).GetRow ();
			j <= lowerrights.Item (i).GetRow (); ++j)
		{
			m_oSelection.Add (j);
		}
	} 
	return m_oSelection;
}


void
BrowserCryptCardGrid::OnCardActivated (wxGridEvent &event)
{
	// Add to deck unless we're in the inventory columns
	if (event.GetCol () >= 3)
		OnPopupAddToDeck (event);
}


void
BrowserCryptCardGrid::OnCardRightClick (wxGridEvent &event)
{
	PopupMenu (&m_oCardPopupMenu, event.GetPosition ());

	event.Skip ();
}


void
BrowserCryptCardGrid::OnCardSelect (wxGridEvent& event)
{
	long lCardRef;

	GetCellValue (event.GetRow (), GetNumberCols () -1).ToLong (&lCardRef);
	m_pTab->SetCardText (lCardRef);

	//  wxLogMessage (wxT ("%d,%d"), event.GetRow (), event.GetCol ());
	if (event.GetCol () < 3)
	{
		EnableEditing (TRUE);
	}
	else
	{
		EnableEditing (FALSE);
	}
	event.Skip ();
	MakeCellVisible (event.GetRow (), 0);
}


void
BrowserCryptCardGrid::OnColumnClick (wxGridEvent &event)
{
	unsigned int uiColumn = event.GetCol ();
	
	if (event.ControlDown())
	{
		//Multi sort
	}

	if (uiColumn == m_uiSortColumn)
	{
		m_bReverseSortOrder = !m_bReverseSortOrder;
	}
	else
	{
		m_bReverseSortOrder = FALSE;
		m_uiSortColumn = uiColumn;
	}

	uiColumn++;
	
	if (m_bReverseSortOrder) 
	{
		uiColumn = -uiColumn;
	}

	m_pModel->SetSortColumn(uiColumn);

	m_pModel->ExecFilterList (TRUE);
	m_pTab->FillCardList ();
}


void
BrowserCryptCardGrid::OnInventoryChange (wxGridEvent& event)
{
	InventoryModel *pInventoryModel = InventoryModel::Instance ();

	unsigned int uiCol = event.GetCol (),
		uiRow = event.GetRow ();
	long lHave, lWant, lSpare;
	wxString sName, sAdv;

	GetCellValue (uiRow, 0).ToLong (&lHave);
	GetCellValue (uiRow, 1).ToLong (&lWant);
	GetCellValue (uiRow, 2).ToLong (&lSpare);
	sName = GetCellValue (uiRow, 3);
	sAdv = GetCellValue (uiRow, 4);

	if (uiCol > 2)
	{
		wxLogError (wxT ("You are not supposed to edit this column."));
		return;
	}

	pInventoryModel->SetHWSCryptName (sName, wxEmptyString, sAdv.Length () > 0, 
		lHave, lWant, lSpare);
}


void
BrowserCryptCardGrid::OnPopupAddAllToDeck (wxCommandEvent& WXUNUSED (event))
{
	DeckModel *pDeck = DeckModel::Instance ();
	long lCardRef = 0;

	if (m_pModel->GetCardCount () > 20) 
	{
		wxMessageDialog oWarningDialog (NULL, wxT ("That's a lot of vampires, are you sure ?"), wxT ("Really ?"), wxYES | wxNO | wxICON_QUESTION);

		if (oWarningDialog.ShowModal () != wxID_YES)
		{
			return;
		}
	}

	for (unsigned int iCard = 0; iCard < m_pModel->GetCardCount (); iCard++)
	{
		m_pModel->GetCardList ()->Item (iCard).Item (11).ToLong (&lCardRef);
		pDeck->AddToCrypt (lCardRef, 1, FALSE);
	}
	// force refresh with a dummy operation
	pDeck->AddToCrypt (lCardRef, 0, TRUE);
}


void
BrowserCryptCardGrid::OnPopupAddToDeck (wxCommandEvent& WXUNUSED (event))
{
	DeckModel *pDeck = DeckModel::Instance ();
	long lCardRef = 0;

	if (MyGetSelectedRows ().Count () > 20) 
	{
		wxMessageDialog oWarningDialog (NULL, wxT ("That's a lot of vampires, are you sure ?"), wxT ("Really ?"), wxYES | wxNO | wxICON_QUESTION);

		if (oWarningDialog.ShowModal () != wxID_YES)
		{
			return;
		}
	}

	for (unsigned int i = 0; i < MyGetSelectedRows ().Count (); i++)
	{
		GetCellValue (MyGetSelectedRows ().Item (i), GetNumberCols () -1).ToLong (&lCardRef);
		pDeck->AddToCrypt (lCardRef, 1, FALSE);
	}
	// force refresh with a dummy operation
	pDeck->AddToCrypt (lCardRef, 0, TRUE);
}


void
BrowserCryptCardGrid::OnPopupCopyAll (wxCommandEvent& WXUNUSED (event))
{
	unsigned int iCard;
	wxString sTempList, sCardList;
	wxTextDataObject *pDataObject;

	// pre-allocate the memory
	//  sCardList.Alloc (100 * m_pModel->GetCardCount ());
	//  sTempList.Alloc (100 * m_pModel->GetCardCount ());

	// Write the text to the clipboard
	if (wxTheClipboard->Open ())
	{
		wxTheClipboard->Clear ();

		for (iCard = 0; iCard < m_pModel->GetCardCount (); iCard++)
		{
			// 	  sTempList.Printf ("%s%s\n", sCardList.c_str (), m_pModel->GetCardList ()->Item (iCard).Item (0).c_str ());
			// 	  sCardList = sTempList;
			sCardList += m_pModel->GetCardList ()->Item (iCard).Item (0);
			sCardList.Append (wxT ("\n"));
		}
		// This data object is held by the clipboard, 
		// so we don't want to delete it here
		pDataObject = new wxTextDataObject(sCardList);
		//     pDataObject->SetText (sCardList);
		wxTheClipboard->SetData (pDataObject);
		wxTheClipboard->Close ();
	}
	else
	{
		wxLogError (wxT ("Unable to access the clipboard."));
		// that data object becomes useless
		//delete pDataObject;
	}
}


void 
BrowserCryptCardGrid::OnPopupRemove (wxCommandEvent& WXUNUSED (event))
{
	long lCardRef;
	wxArrayLong oIndexArray;
	wxArrayString oNameArray;

	SetCursor (*wxHOURGLASS_CURSOR);

	for (unsigned int i = 0; i < MyGetSelectedRows ().Count (); i++)
	{
		GetCellValue (MyGetSelectedRows ().Item (i), GetNumberCols () -1).ToLong (&lCardRef);
		oIndexArray.Add (lCardRef);
		oNameArray.Add (GetCellValue (MyGetSelectedRows ().Item (i), 3));
	}

	for (unsigned int i = 0; i < oIndexArray.Count (); i++)
	{
		m_pTab->m_pController->RemoveCard (oIndexArray.Item (i), oNameArray.Item (i), FALSE);
	}

	m_pTab->UpdateView ();
}


