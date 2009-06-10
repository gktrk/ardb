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

#include "amountdialog.h"
#include "decklibrarytab.h"
#include "deckmodel.h"
#include "speling.h"

BEGIN_EVENT_TABLE (DeckLibraryTab, wxPanel)
  EVT_BUTTON (ID_ADD_BUTTON, DeckLibraryTab::OnAddButtonClick)
  EVT_TEXT (ID_CARD_INPUT, DeckLibraryTab::OnCardInputChanged)
  EVT_TEXT_ENTER (ID_CARD_INPUT, DeckLibraryTab::OnCardInputEnter)
  EVT_LISTBOX (ID_CARD_PICKER, DeckLibraryTab::OnCardPickerSelection)
  EVT_LISTBOX_DCLICK (ID_CARD_PICKER, DeckLibraryTab::OnCardPickerDoubleClick)
  EVT_TREE_KEY_DOWN (ID_TREE, DeckLibraryTab::OnTreeKeyDown)
  EVT_TREE_ITEM_ACTIVATED(ID_TREE, DeckLibraryTab::OnTreeItemActivate)
  EVT_TREE_ITEM_RIGHT_CLICK (ID_TREE, DeckLibraryTab::OnTreeRightClicked)
  EVT_TREE_SEL_CHANGED(ID_TREE, DeckLibraryTab::OnTreeSelect)
  EVT_MENU (ID_POPUP_5_LESS, DeckLibraryTab::OnPopupFiveLess)
  EVT_MENU (ID_POPUP_5_MORE, DeckLibraryTab::OnPopupFiveMore)
  EVT_MENU (ID_POPUP_LESS, DeckLibraryTab::OnPopupLess)
  EVT_MENU (ID_POPUP_MORE, DeckLibraryTab::OnPopupMore)
  EVT_MENU (ID_POPUP_REMOVE_ALL, DeckLibraryTab::OnPopupRemoveAll)
  END_EVENT_TABLE ()

  const wxString DeckLibraryTab::s_sCardNamesQuery = 
wxT ("SELECT DISTINCT card_name "
	 "FROM library_view "
	 "ORDER BY dumbitdown(card_name) ASC");

DeckLibraryTab::DeckLibraryTab (DeckModel *pModel, wxNotebook *pParent) :
  wxPanel (pParent, -1),
  m_bFuzzySelect (TRUE),
  m_bNoEvents (FALSE),
  m_iCycleCounter (0),
  m_iCycleLowerValue (-1),
  m_iCycleUpperValue (-1),
  m_lAmount (1),
  m_lSelectedCard (-1),
  m_lSelectedCardAmount (1),
  m_oArrayOfNames (),
  m_oCardList (),
  m_oNameList (),
  m_oPopupMenu (),
  m_oRootId (),
  m_pAddButton (NULL),
  m_pCardInput (NULL),
  m_pCardPicker (NULL),
  m_pCardText (NULL),
  m_pModel (pModel),
  m_pParent (pParent),
  m_pSetPicker (NULL),
  m_pStatsText (NULL),
  m_pTree (NULL),
  m_sSelectedCardName ()
{
  wxBoxSizer *pPapaSizer, *pPickerSizer, *pFindSizer, *pSubPickerSizer;
  wxNotebook *pNotebook;
  wxPanel *pCardTextPanel, *pDeckInfoPanel;
  wxStaticText *pFindLabel;
  m_bShowPercentage = FALSE;

  m_pParent->AddPage (this, wxT ("Library"), FALSE, -1);

  pPapaSizer = new wxBoxSizer (wxHORIZONTAL); 
  SetSizer (pPapaSizer);

  pPickerSizer = new wxBoxSizer (wxVERTICAL);
  pSubPickerSizer = new wxBoxSizer (wxHORIZONTAL);
  pFindSizer = new wxBoxSizer (wxHORIZONTAL);

  pFindLabel = new wxStaticText (this, -1, wxT ("Find :"));
  pFindSizer->Add (pFindLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
  m_pCardInput = new wxTextCtrl (this, ID_CARD_INPUT, wxT (""), 
				 wxDefaultPosition, wxDefaultSize,
				 wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
  m_pCardInput->SetToolTip (wxT ("Type in the name of a card, ENTER adds to deck."));
  pFindSizer->Add (m_pCardInput, 1);
  pPickerSizer->Add (pFindSizer, 0, wxEXPAND);

  m_pCardPicker = new wxListBox (this, ID_CARD_PICKER, wxDefaultPosition, 
				 wxDefaultSize, 0, NULL, 
				 wxLB_SINGLE | wxLB_ALWAYS_SB);
  m_pCardPicker->SetSize (-1, m_pCardPicker->GetSize ().GetHeight () * 2);
  FillCardPicker ();
  pPickerSizer->Add (m_pCardPicker, 0, wxEXPAND);
  
  pPapaSizer-> Add (pPickerSizer, 1, wxEXPAND | wxALL, 5);

  m_pSetPicker = new wxComboBox (this, ID_SET_PICKER, wxT (""), wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY);
  pSubPickerSizer->Add (m_pSetPicker, 1);

  m_pAddButton = new wxButton (this, ID_ADD_BUTTON, wxT ("Add to deck"));
  pSubPickerSizer->Add (m_pAddButton, 0);
  
  pPickerSizer->Add (pSubPickerSizer, 0, wxEXPAND);

  m_pTree = new MyTreeCtrl (this, ID_TREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxSUNKEN_BORDER);
  m_pTree->SetToolTip (wxT ("Double click to set amount\nRight click for menu"));

  m_oRootId = m_pTree->AddRoot (wxT (""));
  pPapaSizer->Add (m_pTree, 1, wxEXPAND | wxALL, 5);
  
  pNotebook = new wxNotebook (this, -1);
  pDeckInfoPanel = new wxPanel (pNotebook, -1);
  wxBoxSizer *pDeckInfoSizer = new wxBoxSizer (wxVERTICAL);
  pDeckInfoPanel->SetSizer (pDeckInfoSizer);
  m_pStatsText = new wxTextCtrl (pDeckInfoPanel, -1, wxT (""), wxDefaultPosition, wxDefaultSize, wxTE_RICH | wxTE_READONLY | wxTE_MULTILINE);
  pDeckInfoSizer->Add (m_pStatsText, 1, wxEXPAND);
  pNotebook->AddPage (pDeckInfoPanel, wxT ("Deck Overview"));

  pCardTextPanel = new wxPanel (pNotebook, -1);
  wxBoxSizer *pCardTextSizer = new wxBoxSizer (wxVERTICAL);
  pCardTextPanel->SetSizer (pCardTextSizer);
  m_pCardText = new CardText (pCardTextPanel, -1);
  pCardTextSizer->Add (m_pCardText, 1, wxEXPAND);
  pNotebook->AddPage (pCardTextPanel, wxT ("Card Text"));


  pPickerSizer->Add (pNotebook, 1, wxEXPAND | wxTOP, 10);

  // Card tree popup menu
  m_oPopupMenu.Append (ID_POPUP_MORE, wxT ("More\tF7"));
  m_oPopupMenu.Append (ID_POPUP_LESS, wxT ("Less\tF6"));
  m_oPopupMenu.Append (ID_POPUP_5_MORE, wxT ("5 More\tF8"));
  m_oPopupMenu.Append (ID_POPUP_5_LESS, wxT ("5 Less\tF5"));
  m_oPopupMenu.Append (ID_POPUP_REMOVE_ALL, wxT ("Remove all\tdel"));

  pPapaSizer->Layout ();
  pPapaSizer->Fit (m_pParent);

}


DeckLibraryTab::~DeckLibraryTab ()
{

}


wxTreeItemId
DeckLibraryTab::AddOrUpdateTreeItem (wxTreeItemId oParent, wxString& sLabel, long lNumber, float fHandPre, long lCardRef, bool bExpand)
{
	bool bSearch = TRUE, bFound = FALSE;
	wxString sLabelCount = wxT (""), sLabelWhat;
	wxString sDrawPre = wxT(" ");
	wxTreeItemId oSearchId, oMatchId;
	wxTreeItemIdValue cookie;
	long lCount;

	if (!oParent.IsOk ())
	{
		wxLogError (wxT ("AddOrUpdateTreeItem : Invalid parent item"));
		return m_oRootId;
	}

	if (fHandPre != -1.0)
	{
		if (m_bShowPercentage)
		{
			sDrawPre.Printf(wxT("(%.2f) "),fHandPre);
		}
	}

	if (m_pTree->ItemHasChildren (oParent))
	{
		oSearchId = m_pTree->GetFirstChild (oParent, cookie);

		// Here we search if an item of the same label is already in the tree
		while (bSearch)
		{
			// We ignore the counter prefix
			sLabelWhat = m_pTree->GetItemText (oSearchId).AfterFirst (' ');
			// And we compare the labels
			bSearch = sLabelWhat.Cmp (sLabel.c_str ()) != 0;
			if (!bSearch)
			{
				oMatchId = oSearchId;
				bFound = TRUE;
			}
			oSearchId = m_pTree->GetNextChild (oParent, cookie);
			if (!oSearchId.IsOk ())
			{
				bSearch = FALSE;
			}
		}
		if (bFound && oMatchId.IsOk ())
		{
			// increment the counter
			sLabelCount = m_pTree->GetItemText (oMatchId).BeforeFirst (' ');
			sLabelCount.ToLong (&lCount);
			lCount += lNumber;
			
			sLabelCount.Printf (wxT ("%ld"), lCount);
			sLabelCount << sDrawPre;
	
			// Change the text
			m_pTree->SetItemText (oMatchId, sLabelCount + sLabel);
		}
		else
		{ 
			sLabelCount.Empty ();
			sLabelCount << lNumber << sDrawPre;

			oMatchId = m_pTree->AppendItem (oParent, sLabelCount << sLabel);
		}
	}
	else
	{
		sLabelCount.Empty ();
		sLabelCount << lNumber << sDrawPre;

		oMatchId = m_pTree->AppendItem (oParent, sLabelCount << sLabel);
	}

	if (bExpand)
	{
		m_pTree->Expand (oParent);
		if (oMatchId.IsOk ())
		{
			m_pTree->EnsureVisible (oMatchId);
		}
	}
	else
	{
		m_pTree->EnsureVisible (oParent);
	}

	if (lCardRef >= 0 && oMatchId.IsOk ())
	{
		// Associated data is the card reference id
		m_pTree->SetItemData (oMatchId, new MyTreeItemData (lCardRef));
		if (lCardRef == m_lSelectedCard)
		{
			if (m_bFuzzySelect)
			{
				oMatchId = m_pTree->GetItemParent (oMatchId);
			}
			m_pTree->SelectItem (oMatchId);
		}
	}
	m_pTree->SortChildren (oParent);

	return oMatchId;
}


void
DeckLibraryTab::AddToDeck ()
{
  wxArrayString *pMyRecord, *pModelsRecord;
  long lCardRef;

  pMyRecord = (wxArrayString *) m_pSetPicker->GetClientData (m_pSetPicker->GetSelection ());
  if (pMyRecord != NULL)
	{
	  if (pMyRecord->Item (2).ToLong (&lCardRef))
	{
	  pModelsRecord = m_pModel->AddToLibrary (lCardRef, m_lAmount, FALSE);
	  if (pModelsRecord != NULL)
		{
		  pModelsRecord->Item (4).ToLong(&m_lSelectedCard);
		  m_sSelectedCardName = pModelsRecord->Item (1);
		  pModelsRecord->Item (0).ToLong(&m_lSelectedCardAmount);
		  Update ();
		}
	}
	}
}


void
DeckLibraryTab::AddToTree (wxString sType, wxString sName, wxString sSet, 
			   long lNumber, long lCardRef, float fHandPre)
{
  wxTreeItemId oItemId;

  // Add the category
  oItemId = AddOrUpdateTreeItem (m_oRootId, sType, lNumber);
  m_pTree->SetItemBold (oItemId, TRUE);
  // Add the card name
  oItemId = AddOrUpdateTreeItem (oItemId, sName, lNumber, fHandPre);
  // Add the card set
  oItemId = AddOrUpdateTreeItem (oItemId, sSet, lNumber, -1.0, lCardRef, FALSE);
}


wxArrayLong *
DeckLibraryTab::DeleteBranch (wxTreeItemId oItemId, bool bFirstCall)
{
  wxArrayLong *pRefArray = new wxArrayLong (), *pOtherArray;
  long lCardRef = (long) &oItemId;
  wxTreeItemIdValue cookie;
  MyTreeItemData *pData;

  m_bNoEvents = TRUE;

  if (!oItemId.IsOk ())
	{
	  wxLogError (wxT ("DeleteBranch : Bad Item ID"));
	  return NULL;
	}

  // For foolproofness, we don't allow deleting the whole tree this way
  //if (oItemId == m_oRootId)
  //{
//	  Update();
//	  return NULL;
  //}

  pData = (MyTreeItemData *) m_pTree->GetItemData (oItemId);

  if (pData != NULL)
	{
	  lCardRef = pData->GetValue ();
	  if (lCardRef >= 0)
	{
	  // Remember the card to remove, actual deleting will be later
	  pRefArray->Add (lCardRef);
	}
	}
	  
  // Recursively process the children
  wxTreeItemId oChildId = m_pTree->GetFirstChild (oItemId, cookie);
  while (oChildId.IsOk ())
	{
	  pOtherArray = DeleteBranch (oChildId, FALSE);
	  if (pOtherArray != NULL)
	{
	  WX_APPEND_ARRAY (*pRefArray, *pOtherArray);
	  delete pOtherArray;
	}
	  oChildId = m_pTree->GetNextChild (oItemId, cookie);
	}

  // after all the recursive processing has taken place, 
  // update the view
  if (bFirstCall)
	{
	  // Actually delete the cards
	  for (unsigned int i = 0; i < pRefArray->Count (); i++)
	{
	  m_pModel->DelFromLibrary (pRefArray->Item (i), -1, FALSE);
	}

	  m_bNoEvents = FALSE;
	  Update ();
	  delete pRefArray;
	  return NULL;
	}
  return pRefArray;
}


void
DeckLibraryTab::FillCardPicker ()
{
  Database *pDatabase = Database::Instance ();

  pDatabase->Query (s_sCardNamesQuery, &m_oNameList);

  m_pCardPicker->Freeze ();
  for (unsigned int i = 0; i < m_oNameList.GetCount (); i++)
	{
	  m_pCardPicker->Append (m_oNameList.Item (i).Item (0).c_str ());

	  // associated data is a pointer to the corresponding Record
	  m_pCardPicker->SetClientData (i, &m_oNameList.Item (i));

	  // Fill the other name array for "speling"
	  m_oArrayOfNames.Add (m_oNameList.Item (i).Item (0));
	}

  m_pCardPicker->Thaw ();
}


void
DeckLibraryTab::LessOfAnItem (wxTreeItemId oItemId, long lCount)
{
  MoreOfAnItem (oItemId, -lCount);
}


void
DeckLibraryTab::MoreOfAnItem (wxTreeItemId oItemId, long lCount)
{
  wxTreeItemId oChild;
  wxTreeItemIdValue cookie;
  long lCardRef;

  if (!oItemId.IsOk ()) return;

  MyTreeItemData * pData = (MyTreeItemData *) m_pTree->GetItemData (oItemId);
  if (pData != NULL)
	{
	  // we know what to modify
	  lCardRef = pData->GetValue ();
	  if (lCardRef >= 0)
	{
	  if (lCount > 0)
		{
		  m_pModel->AddToLibrary (lCardRef, lCount, TRUE);
		}
	  else
		{
		  m_pModel->DelFromLibrary (lCardRef, -lCount, TRUE);
		}
	}
	}
  else
	{
	  // We don't know what to modify, so we'll try with a child
	  oChild = m_pTree->GetFirstChild (oItemId, cookie);
	  if (oChild.IsOk ())
	{
	  MoreOfAnItem (oChild, lCount);
	}
	}
}


void
DeckLibraryTab::OnAddButtonClick (wxCommandEvent& WXUNUSED (event))
{
  if (m_bNoEvents) return;
  AddToDeck ();
}


void
DeckLibraryTab::OnCardInputChanged (wxCommandEvent& WXUNUSED (event))
{
  bool bSearchLower = TRUE, bSearchUpper = TRUE;
  int iIndex = 0, iStringLength;
  wxString CardName;

  if (m_bNoEvents || !m_pCardInput || !m_pCardPicker) return;

  m_lAmount = 1;
  CardName = m_pCardInput->GetValue ();

  iStringLength = CardName.Len ();

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
	  bSearchLower = CardName.CmpNoCase (m_pCardPicker->GetString (iIndex).Left (iStringLength).c_str ()) != 0;
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
	  bSearchUpper = CardName.CmpNoCase (m_pCardPicker->GetString (iIndex).Left (iStringLength).c_str ()) == 0;
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
	  wxString sVal (CardName);
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
DeckLibraryTab::OnCardInputEnter (wxCommandEvent& WXUNUSED (event))
{
  if (m_bNoEvents || !m_pCardInput) return;
  m_pCardInput->SetSelection (0, -1);
  AddToDeck ();
}


void
DeckLibraryTab::OnCardPickerDoubleClick (wxCommandEvent& WXUNUSED (event))
{
  if (m_bNoEvents) return;
  AddToDeck ();
}


void
DeckLibraryTab::OnCardPickerSelection (wxCommandEvent& WXUNUSED (event))
{
  Database *pDatabase = Database::Instance ();
  long lCardRef;
  wxString sQuery, sTemp;

  if (m_bNoEvents || !pDatabase || !m_pCardPicker || !m_pSetPicker) return;

  sTemp = m_pCardPicker->GetStringSelection ();
  sTemp.Replace (wxT ("'"), wxT ("''"));

  sQuery.Printf (wxT ("SELECT library_view_with_proxy.card_name, "
			  "		  library_view_with_proxy.set_name, "
			  "		  library_view_with_proxy.card_ref, "
			  "		  library_view_with_proxy.card_type, "
			  "		  cards_sets.full_name, "
			  "		  cards_sets.release_date "
			  "FROM library_view_with_proxy, cards_sets "
			  "WHERE library_view_with_proxy.set_ref = cards_sets.record_num "
			  "		 AND card_name = '%s' "
			  "ORDER BY cards_sets.release_date DESC ;"),
		 sTemp.c_str ());

  pDatabase->Query (sQuery.c_str (), &m_oCardList);

  m_pSetPicker->Freeze ();
  
  m_pSetPicker->Clear ();
  m_oCardList.Item (0).Item (2).ToLong (&lCardRef);
  m_pCardText->DisplayLibraryText(lCardRef,NULL);

  for (unsigned int i = 0; i < m_oCardList.GetCount (); i++)
	{
	  // associated data is a pointer to the record
	  m_pSetPicker->Append (m_oCardList.Item (i).Item (4),
				(void *) &m_oCardList.Item (i));
	  m_pSetPicker->SetSelection (0);
	}

  m_pSetPicker->Thaw ();
}


void
DeckLibraryTab::OnPopupFiveLess (wxCommandEvent& WXUNUSED (event))
{
  wxTreeItemId oSelectedId;

  if (m_bNoEvents || !m_pTree) return;
  oSelectedId = m_pTree->GetSelection ();
  LessOfAnItem (oSelectedId, 5);
}


void
DeckLibraryTab::OnPopupFiveMore (wxCommandEvent& WXUNUSED (event))
{
  wxTreeItemId oSelectedId;

  if (m_bNoEvents || !m_pTree) return;
  oSelectedId = m_pTree->GetSelection ();
  MoreOfAnItem (oSelectedId, 5);
}


void
DeckLibraryTab::OnPopupLess (wxCommandEvent& WXUNUSED (event))
{
  wxTreeItemId oSelectedId;

  if (m_bNoEvents || !m_pTree) return;
  oSelectedId = m_pTree->GetSelection ();
  LessOfAnItem (oSelectedId, 1);
}


void
DeckLibraryTab::OnPopupMore (wxCommandEvent& WXUNUSED (event))
{
  wxTreeItemId oSelectedId;

  if (m_bNoEvents || !m_pTree) return;
  oSelectedId = m_pTree->GetSelection ();
  MoreOfAnItem (oSelectedId, 1);
}


void
DeckLibraryTab::OnPopupRemoveAll (wxCommandEvent& WXUNUSED (event))
{
  wxTreeItemId oSelectedId;

  if (m_bNoEvents || !m_pTree)
  {
	  return;
  }

  oSelectedId = m_pTree->GetSelection();
  DeleteBranch(oSelectedId);
}


void
DeckLibraryTab::OnTreeItemActivate (wxTreeEvent & WXUNUSED (event))
{
#if 0
  wxArrayInt oArrayCount;
  wxArrayString oArrayName;

  oArrayName.Add (m_sSelectedCardName);
  oArrayCount.Add (m_lSelectedCardAmount);

  AmountDialog * pDialog = new AmountDialog (&oArrayName, &oArrayCount);
  
  if (pDialog->ShowModal ())
	{
	  m_pModel->SetLibraryRefAmount (m_lSelectedCard, oArrayCount.Item (0));
	  m_lSelectedCardAmount = oArrayCount.Item (0);
	}
  
  delete pDialog;
#endif
}


void
DeckLibraryTab::OnTreeKeyDown (wxTreeEvent &event)
{
  wxKeyEvent oKeyEvent = event.GetKeyEvent ();

  if (m_bNoEvents || !m_pTree) return;
  switch (oKeyEvent.GetKeyCode ())
	{
	  // Keys to delete cards
	case WXK_BACK: // backspace
	case WXK_DELETE: // delete
	  DeleteBranch (m_pTree->GetSelection ());
	  break;
	case WXK_F5:
	  LessOfAnItem (m_pTree->GetSelection (), 5);
	  break;
	case WXK_F6:
	  LessOfAnItem (m_pTree->GetSelection (), 1);
	  break;
	case WXK_F7:
	  MoreOfAnItem (m_pTree->GetSelection (), 1);
	  break;
	case WXK_F8:
	  MoreOfAnItem (m_pTree->GetSelection (), 5);
	  break;
	default:
	  //	   wxString sMessage = "Pressed key #";
	  //	   sMessage << oKeyEvent.GetKeyCode ();
	  //	   wxLogMessage (sMessage);
	  //	   event.Skip ();
	  break;
	}
}


void
DeckLibraryTab::OnTreeRightClicked (wxTreeEvent &event)
{
  if (m_bNoEvents || !m_pTree) return;
  m_pTree->SelectItem (event.GetItem ());
  m_pTree->PopupMenu (&m_oPopupMenu, event.GetPoint ());
}


void
DeckLibraryTab::OnTreeSelect (wxTreeEvent &event)
{
  wxTreeItemIdValue	 cookie;
  long lCount;
  MyTreeItemData *pData = NULL;
  wxString sName;
  wxTreeItemId oItem, oParentItem;
  
  if (m_bNoEvents || !m_pTree || !m_pCardInput) return;

  oItem = event.GetItem ();
  if (!oItem.IsOk ()) return;

  pData = (MyTreeItemData *) m_pTree->GetItemData (oItem);
  m_bFuzzySelect = FALSE;
  if (pData == NULL) 
	{
	  oParentItem = oItem;
	  oItem = m_pTree->GetFirstChild (oItem, cookie);
	  if (oItem.IsOk ())
	{
	  pData = (MyTreeItemData *) m_pTree->GetItemData (oItem);
	}
	  m_bFuzzySelect = TRUE;
	} 
  
  if (m_bFuzzySelect)
	{
	  sName = m_pTree->GetItemText (oParentItem).AfterFirst (' ');
	  m_pTree->GetItemText (oParentItem).BeforeFirst (' ').ToLong (&lCount);
	}
  else
	{
	  sName = m_pTree->GetItemText (oItem).AfterFirst (' ');
	  m_pTree->GetItemText (oItem).BeforeFirst (' ').ToLong (&lCount);
	}

  if (pData != NULL) 
	{
	  m_lSelectedCard =	 pData->GetValue ();
	  m_sSelectedCardName = sName;
	  m_lSelectedCardAmount = lCount;

	  m_pCardInput->Clear ();
	  m_pCardInput->AppendText (sName);
	}
}


void
DeckLibraryTab::Update ()
{
  UpdateCardList();
  UpdateTotalCardCount();
  UpdateStats();
}


void
DeckLibraryTab::UpdateTotalCardCount ()
{
  wxString sCardCount;

  switch (m_pModel->GetLibraryCount())
	{
	case 0:
	  sCardCount = wxT ("Empty library");
	  break;
	case 1:
	  sCardCount = wxT ("1 card");
	  break;
	default:
	  sCardCount.Printf (wxT ("%d cards"), m_pModel->GetLibraryCount ());
	}

  m_pTree->SetItemText (m_oRootId, sCardCount);
}


void
DeckLibraryTab::UpdateCardList ()
{
  RecordSet *pCardList = m_pModel->GetLibraryList ();
  long lNumber, lCardRef;

  if (m_bNoEvents) return;
  m_bNoEvents = TRUE;

  m_pTree->Freeze ();
  m_pTree->DeleteChildren (m_oRootId);

  for (unsigned int i = 0; i < pCardList->GetCount(); i++)
  {
	  /* remember the view query is :
	 "SELECT number_used, "
	 "		 card_name, "
	 "		 set_name, "
	 "		 card_type, "
	 "		 card_ref "
	  */
	  if (pCardList->Item(i).Item(0).ToLong(&lNumber) && pCardList->Item(i).Item(4).ToLong(&lCardRef))
	  {
		  AddToTree ( pCardList->Item(i).Item(3),
					  pCardList->Item(i).Item(1),
					  pCardList->Item(i).Item(2), 
					  lNumber, lCardRef,HandPercentage(m_pModel->GetLibraryCount(),lNumber));
	  }	
  }	
  
  //  m_pTree->SortChildren (m_oRootId);
  m_pTree->Thaw ();

  m_bNoEvents = FALSE;
}

// Calculates the percentage chance of having card in hand
float
DeckLibraryTab::HandPercentage(long deckCount, long lNumber)
{
	double p = 0;
	double fracs[7];
	int n;
	int i;
	int s = deckCount;
	int c = lNumber;

	n = s-c;

	for(i=0;i<7;i++)
	{
		fracs[i] = (double)(n-i)/(s-i);
	}

	p = 1-(fracs[0] * fracs[1] * fracs[2] * fracs[3] * fracs[4] * fracs[5] * fracs[6]);
	
	return (p*100.0);
}

void
DeckLibraryTab::UpdateStats ()
{
  RecordSet *pRecordSet;
  wxFont oFont = m_pStatsText->GetDefaultStyle ().GetFont ();;
  wxString sSummary;

  m_pStatsText->Freeze ();
  m_pStatsText->Clear ();
  
  oFont.SetWeight (wxBOLD);
  oFont.SetUnderlined (TRUE);
  m_pStatsText->SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));
  m_pStatsText->WriteText (wxT ("Deck overview\n\n"));
  oFont.SetWeight (wxNORMAL);
  oFont.SetUnderlined (FALSE);
  m_pStatsText->SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));

  sSummary.Clear ();
  sSummary << wxT ("Total blood cost : ") << m_pModel->GetLibStatsBloodCost ();
  m_pStatsText->WriteText (sSummary);
  m_pStatsText->WriteText (wxT ("\n"));
  
  sSummary.Clear ();
  sSummary << wxT ("Total pool cost : ") << m_pModel->GetLibStatsPoolCost ();
  m_pStatsText->WriteText (sSummary);
  m_pStatsText->WriteText (wxT ("\n"));
  m_pStatsText->WriteText (wxT ("\n"));
  

  pRecordSet = m_pModel->GetLibStatsDiscisplines ();
  if (pRecordSet && pRecordSet->GetCount () > 0) 
	{
	  oFont.SetWeight (wxBOLD);
	  m_pStatsText->SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));
	  m_pStatsText->WriteText (wxT ("Disciplines\n\n"));
	  oFont.SetWeight (wxNORMAL);
	  m_pStatsText->SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));
	  
	  sSummary.Clear ();
	  for (unsigned int i = 0; i < pRecordSet->GetCount (); i++)
	{
	  if (!pRecordSet->Item (i).Item (1).IsEmpty ())
		{
		  sSummary << pRecordSet->Item (i).Item (0);
		  if (!pRecordSet->Item (i).Item (0).Cmp (wxT ("1")))
		{
		  sSummary << wxT (" card requires ");
		}
		  else
		{
		  sSummary << wxT (" cards require ");
		}
		  sSummary << pRecordSet->Item (i).Item (1) << wxT ("\n");
		}
	}
	  m_pStatsText->WriteText (sSummary);
	  m_pStatsText->WriteText (wxT ("\n"));
	}

  pRecordSet = m_pModel->GetLibStatsClans ();
  if (pRecordSet && pRecordSet->GetCount () > 0) 
	{
	  oFont.SetWeight (wxBOLD);
	  m_pStatsText->SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));
	  m_pStatsText->WriteText (wxT ("Clans\n\n"));
	  oFont.SetWeight (wxNORMAL);
	  m_pStatsText->SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));

	  sSummary.Clear ();
	  for (unsigned int i = 0; i < pRecordSet->GetCount (); i++)
	{
	  if (!pRecordSet->Item (i).Item (1).IsEmpty ())
		{
		  sSummary << pRecordSet->Item (i).Item (0);
		  if (!pRecordSet->Item (i).Item (0).Cmp (wxT ("1")))
		{
		  sSummary << wxT (" card requires ");
		}
		  else
		{
		  sSummary << wxT (" cards require ");
		}
		  sSummary << pRecordSet->Item (i).Item (1) << wxT ("\n");
		}
	}
	  m_pStatsText->WriteText (sSummary);
	  m_pStatsText->WriteText (wxT ("\n"));
	}

  pRecordSet = m_pModel->GetLibStatsRequirements ();
  if (pRecordSet && pRecordSet->GetCount () > 0) 
	{
	  oFont.SetWeight (wxBOLD);
	  m_pStatsText->SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));
	  m_pStatsText->WriteText (wxT ("Miscellaneous\n\n"));
	  oFont.SetWeight (wxNORMAL);
	  m_pStatsText->SetDefaultStyle (wxTextAttr (wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour (wxSYS_COLOUR_WINDOW), oFont));

	  sSummary.Clear ();
	  for (unsigned int i = 0; i < pRecordSet->GetCount (); i++)
	{
	  if (!pRecordSet->Item (i).Item (1).IsEmpty ())
		{
		  sSummary << pRecordSet->Item (i).Item (0);
		  if (!pRecordSet->Item (i).Item (0).Cmp (wxT ("1")))
		{
		  sSummary << wxT (" card requires ");
		}
		  else
		{
		  sSummary << wxT (" cards require ");
		}
		  sSummary << pRecordSet->Item (i).Item (1) << wxT ("\n");
		}
	}
	  m_pStatsText->WriteText (sSummary);
	  m_pStatsText->WriteText (wxT ("\n"));
	}

  m_pStatsText->Thaw ();
}

void
DeckLibraryTab::ShowPercentage()
{
	if (m_bShowPercentage)
	{
		m_bShowPercentage = FALSE;
	}
	else
	{
		m_bShowPercentage = TRUE;
	}

	UpdateCardList();
}



// ////////////////////////////////////////////////////////////////
// 
// Class MyTreeCtrl
//

#if USE_GENERIC_TREECTRL
IMPLEMENT_DYNAMIC_CLASS(MyTreeCtrl, wxGenericTreeCtrl)
#else
IMPLEMENT_DYNAMIC_CLASS(MyTreeCtrl, wxTreeCtrl)
#endif

int 
MyTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
  wxString sLabel1, sLabel2;

  sLabel1 = GetItemText (item1).AfterFirst (' ');
  sLabel2 = GetItemText (item2).AfterFirst (' ');

  return sLabel1.Cmp (sLabel2.c_str ());
}


