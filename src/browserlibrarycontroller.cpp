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

#include "browserlibrarycontroller.h"


BrowserLibraryController::BrowserLibraryController (BrowserLibraryModel *pModel) :
    m_eMode (LIBRARY_FILTER_KEEP),
    m_pModel (pModel),
    m_sFilterSummary (wxT ("")),
    m_sWhereClause (wxT ("WHERE "))
{

}


void
BrowserLibraryController::RemoveCard (long lCard, wxString &sName, bool bUpdateView)
{
    wxString sQuery;

    m_sFilterSummary.Printf (wxT ("Remove %s"), sName.c_str ());

    sQuery.Printf (wxT ("DELETE FROM library_selection WHERE browser_num=%d AND card_name IN (SELECT card_ref FROM library_view WHERE name_ref IN (SELECT name_ref FROM library_view WHERE card_ref = %ld))"), m_pModel->GetIDNumber (), lCard);
    m_pModel->QueryCardData (sQuery, m_sFilterSummary, bUpdateView);
}


LibraryFilterMode
BrowserLibraryController::GetFilterMode ()
{
    return m_eMode;
}


void
BrowserLibraryController::SetFilterMode (LibraryFilterMode eMode)
{
    m_eMode = eMode;
}


// This function will collect input from the library filter dialog
// and modify the model accordingly
void
BrowserLibraryController::UpdateModel (BrowserLibraryFilter *pFilter)
{
    wxString sQuery, sSummary;

    DeleteWhereClause ();
    DeleteFilterSummary ();

    ProcessTypes (pFilter);
    ProcessClans (pFilter);
    ProcessDisciplines (pFilter);
    ProcessTitles (pFilter);
    ProcessTexts (pFilter);
    ProcessOthers (pFilter);
    ProcessEditions (pFilter);
    ProcessRarity (pFilter);

    switch (m_eMode) {
    case LIBRARY_FILTER_ADD:
        sQuery.Printf (m_pModel->m_sAddQuery, m_sWhereClause.c_str ());
        sSummary.Printf (wxT ("Add "));
        break;
    case LIBRARY_FILTER_REMOVE:
        sQuery.Printf (m_pModel->m_sRemoveQuery, m_sWhereClause.c_str ());
        sSummary.Printf (wxT ("Remove "));
        break;
    case LIBRARY_FILTER_KEEP:
        sQuery.Printf (m_pModel->m_sKeepQuery, m_sWhereClause.c_str ());
        sSummary.Printf (wxT ("Select "));
        break;
    }
    sSummary.Append (m_sFilterSummary);
    m_pModel->QueryCardData (sQuery, sSummary);
}


void
BrowserLibraryController::AddToFilterSummary (const wxString &sExpression)
{
    if (!m_sFilterSummary.IsEmpty ()) {
        m_sFilterSummary.Append (wxT ("and "));
    }

    m_sFilterSummary.Append (sExpression.c_str ());
    m_sFilterSummary.Append (wxT (" "));
}


void
BrowserLibraryController::AddToWhereClause (const wxString &sExpression)
{
    if (m_sWhereClause.IsEmpty ()) {
        m_sWhereClause.Append (wxT ("WHERE "));
    } else {
        m_sWhereClause.Append (wxT ("AND "));
    }

    m_sWhereClause.Append (wxT ("("));
    m_sWhereClause.Append (sExpression.c_str ());
    m_sWhereClause.Append (wxT (") "));
}


void
BrowserLibraryController::DeleteFilterSummary ()
{
    m_sFilterSummary.Clear ();
}


void
BrowserLibraryController::DeleteWhereClause ()
{
    m_sWhereClause.Clear ();
}


void
BrowserLibraryController::ProcessClans (BrowserLibraryFilter *pFilter)
{
    wxString sExpression, sTempString, sOperator, sSummary;
    InterfaceData *pUIData = InterfaceData::Instance ();

    if (pFilter->m_pClanLess->IsChecked ()) {
        sExpression.Printf (wxT ("clan = ''"));
        AddToWhereClause (sExpression);
        sSummary.Printf (wxT ("Clanless"));
        AddToFilterSummary (sSummary);
    } else {
        for (unsigned int i = 0; i < pUIData->GetClans ()->GetCount (); i++) {
            if (pFilter->m_pClanList->IsSelected (i)) {
                sTempString.Printf (wxT ("%s%s(clan LIKE '%s' or clan LIKE '%%/%s' or clan LIKE '%s/%%')"),
                                    sExpression.c_str (), sOperator.c_str (),
                                    pUIData->GetClans ()->Item (i).c_str (),
                                    pUIData->GetClans ()->Item (i).c_str (),
                                    pUIData->GetClans ()->Item (i).c_str ());
                sExpression = sTempString;
                sTempString.Printf (wxT ("%s%s "), sSummary.c_str (), pUIData->GetClans ()->Item (i).c_str ());
                sSummary = sTempString;
                sOperator.Printf (wxT (" OR "));
            }
        }
        if (!sExpression.IsEmpty ()) {
            AddToWhereClause (sExpression);
            AddToFilterSummary (sSummary);
        }
    }
}


void
BrowserLibraryController::ProcessDisciplines (BrowserLibraryFilter *pFilter)
{
    InterfaceData *pUIData = InterfaceData::Instance ();
    wxString sExpression, sTempString, sOperator, sSummary;

    if (pFilter->m_pDisciplineLess->IsChecked ()) {
        sExpression.Printf (wxT ("discipline = ''"));
        AddToWhereClause (sExpression);
        sSummary.Printf (wxT ("Disciplineless"));
        AddToFilterSummary (sSummary);
    } else {
        for (unsigned int i = 0; i < pUIData->GetDisciplines ()->GetCount (); i++) {
            if (pFilter->m_pDisciplineList->IsSelected (i)) {
                sTempString.Printf (wxT ("%s%s(discipline LIKE '%%%s%%')"),
                                    sExpression.c_str (), sOperator.c_str (), pUIData->GetDisciplines ()->Item (i)[0].c_str ());
                sExpression = sTempString;
                sTempString.Printf (wxT ("%s%s "), sSummary.c_str (), pUIData->GetDisciplines ()->Item (i)[0].c_str ());
                sSummary = sTempString;
                sOperator.Printf (wxT (" OR "));
            }
        }
        if (!sExpression.IsEmpty ()) {
            AddToWhereClause (sExpression);
            AddToFilterSummary (sSummary);
        }

        if (pFilter->m_pMultiDiscipline->IsChecked ()) {
            sExpression.Printf (wxT ("(discipline LIKE '%%/%%') OR (discipline LIKE '%%&%%')"));
            AddToWhereClause (sExpression);
            sSummary.Printf (wxT ("multi-discipline"));
            AddToFilterSummary (sSummary);
        }
    }
}


void
BrowserLibraryController::ProcessEditions (BrowserLibraryFilter *pFilter)
{
    wxString sExpression, sTempString, sOperator, sSummary;
    InterfaceData *pUIData = InterfaceData::Instance ();

    for (unsigned int i = 0; i < pUIData->GetEditions ()->GetCount (); i++) {
        if (pFilter->m_pEditionList->IsSelected (i)) {
            sTempString.Printf (wxT ("%s%s(set_name IN ("
                                     "SELECT set_name FROM cards_sets "
                                     "WHERE full_name = '%s' "
                                     "   OR full_name = 'Proxy %s'))"),
                                sExpression.c_str (),
                                sOperator.c_str (),
                                pUIData->GetEditions ()->Item (i).c_str (),
                                pUIData->GetEditions ()->Item (i).c_str ());
            sExpression = sTempString;
            sTempString.Printf (wxT ("%s%s "), sSummary.c_str (),
                                pUIData->GetEditions ()->Item (i).c_str ());
            sSummary = sTempString;
            sOperator.Printf (wxT (" OR "));
        }
    }
    if (!sExpression.IsEmpty ()) {
        AddToWhereClause (sExpression);
        AddToFilterSummary (sSummary);
    }
}


void
BrowserLibraryController::ProcessOthers (BrowserLibraryFilter *pFilter)
{
    InterfaceData *pUIData = InterfaceData::Instance ();
    wxString sExpression, sSummary, sTemp;

    for (unsigned int i = 0; i < pUIData->GetLibraryReqs ()->GetCount (); i++) {
        if (pFilter->m_apLibraryReqs[i]->IsChecked ()) {
            sExpression.Printf (wxT ("requires LIKE '%s' OR "
                                     "discipline LIKE '%s' OR "
                                     "clan LIKE '%s' "),
                                pUIData->GetLibraryReqs ()->Item (i)[1].c_str (),
                                pUIData->GetLibraryReqs ()->Item (i)[1].c_str (),
                                pUIData->GetLibraryReqs ()->Item (i)[1].c_str ());
            AddToWhereClause (sExpression);
            sSummary.Printf (wxT ("%s"),
                             pUIData->GetLibraryReqs ()->Item (i)[0].c_str ());
            AddToFilterSummary (sSummary);
        }
    }

    for (unsigned int i = 0; i < pUIData->GetLibraryTexts ()->GetCount (); i++) {
        if (pFilter->m_apLibraryTexts[i]->IsChecked ()) {
            sExpression.Printf (wxT ("card_text LIKE '%s'"),
                                pUIData->GetLibraryTexts ()->Item (i)[1].c_str ());
            AddToWhereClause (sExpression);
            sSummary.Printf (wxT ("%s"),
                             pUIData->GetLibraryTexts ()->Item (i)[0].c_str ());
            AddToFilterSummary (sSummary);
        }
    }

    if (pFilter->m_pCostBlood->IsChecked ()) {
        sExpression.Printf (wxT ("cost LIKE '%%Blood%%'"));
        AddToWhereClause (sExpression);
        sSummary.Printf (wxT ("cost blood"));
        AddToFilterSummary (sSummary);
    }
    if (pFilter->m_pCostPool->IsChecked ()) {
        sExpression.Printf (wxT ("cost LIKE '%%Pool%%'"));
        AddToWhereClause (sExpression);
        sSummary.Printf (wxT ("cost pool"));
        AddToFilterSummary (sSummary);
    }
}


void
BrowserLibraryController::ProcessRarity (BrowserLibraryFilter *pFilter)
{
    wxString sExpression, sTempString, sOperator, sSummary;
    InterfaceData *pUIData = InterfaceData::Instance ();

    for (unsigned int i = 0; i < pUIData->GetRarities ()->GetCount (); i++) {
        if (pFilter->m_pRarityList->IsSelected (i)) {
            sTempString.Printf (wxT ("%s%s (rarity_type LIKE '%s%%' OR "
                                     "rarity_type LIKE '%%/%s%%')"),
                                sExpression.c_str (),
                                sOperator.c_str (),
                                pUIData->GetRarities ()->Item (i)[1].c_str (),
                                pUIData->GetRarities ()->Item (i)[1].c_str ());
            sExpression = sTempString;
            sTempString.Printf (wxT ("%s%s "), sSummary.c_str (),
                                pUIData->GetRarities ()->Item (i)[0].c_str ());
            sSummary = sTempString;
            sOperator.Printf (wxT (" OR "));
        }
    }
    if (!sExpression.IsEmpty ()) {
        AddToWhereClause (sExpression);
        AddToFilterSummary (sSummary);
    }
}


void
BrowserLibraryController::ProcessTexts (BrowserLibraryFilter *pFilter)
{
    wxString sExpression, sSummary, sTemp;

    if (!pFilter->m_pOtherText->GetValue ().IsEmpty ()) {
        sTemp = pFilter->m_pOtherText->GetValue ();
        sTemp.Replace (wxT ("'"), wxT ("''"));
        sExpression.Printf (wxT ("card_text LIKE '%%%s%%'"), sTemp.c_str ());
        AddToWhereClause (sExpression);
        sSummary.Printf (wxT ("text contains \"%s\""), pFilter->m_pOtherText->GetValue ().c_str ());
        AddToFilterSummary (sSummary);
    }
    if (!pFilter->m_pCardNameText->GetValue ().IsEmpty ()) {
        sTemp = pFilter->m_pCardNameText->GetValue ();
        sTemp.Replace (wxT ("'"), wxT ("''"));
        sExpression.Printf (wxT ("card_name LIKE '%%%s%%'"), sTemp.c_str ());
        AddToWhereClause (sExpression);
        sSummary.Printf (wxT ("name contains \"%s\""), pFilter->m_pCardNameText->GetValue ().c_str ());
        AddToFilterSummary (sSummary);
    }
}


void
BrowserLibraryController::ProcessTitles (BrowserLibraryFilter *pFilter)
{
    wxString sExpression, sTempString, sOperator, sSummary;
    InterfaceData *pUIData = InterfaceData::Instance ();

    for (unsigned int i = 0; i < pUIData->GetTitles ()->GetCount () - 2; i++) {
        if (pFilter->m_pTitleList->IsSelected (i)) {
            sTempString.Printf (wxT ("%s%s(requires LIKE '%%%s%%')"),
                                sExpression.c_str (),
                                sOperator.c_str (),
                                pUIData->GetTitles ()->Item (i)[1].c_str ());
            sExpression = sTempString;
            sTempString.Printf (wxT ("%s%s "),
                                sSummary.c_str (),
                                pUIData->GetTitles ()->Item (i)[0].c_str ());
            sSummary = sTempString;
            sOperator.Printf (wxT (" OR "));
        }
    }
    if (!sExpression.IsEmpty ()) {
        AddToWhereClause (sExpression);
        AddToFilterSummary (sSummary);
    }
}


void
BrowserLibraryController::ProcessTypes (BrowserLibraryFilter *pFilter)
{
    wxString sExpression, sSummary;
    InterfaceData *pUIData = InterfaceData::Instance ();

    for (unsigned int i = 0; i < pUIData->GetTypes ()->GetCount (); i++) {
        if (pFilter->m_apTypes[i]->IsChecked ()) {
            sExpression.Printf (wxT ("(card_type = '%s') OR (card_type LIKE '%s/%%') OR (card_type LIKE '%%/%s') OR (card_type LIKE '%s / %%') OR (card_type LIKE '%% / %s')"),
                                pUIData->GetTypes ()->Item (i).c_str (),
                                pUIData->GetTypes ()->Item (i).c_str (),
                                pUIData->GetTypes ()->Item (i).c_str (),
                                pUIData->GetTypes ()->Item (i).c_str (),
                                pUIData->GetTypes ()->Item (i).c_str ());
            AddToWhereClause (sExpression);
            sSummary.Printf (wxT ("%s"), pUIData->GetTypes ()->Item (i).c_str ());
            AddToFilterSummary (sSummary);
        }
    }

    if (pFilter->m_pCombo->IsChecked ()) {
        sExpression.Printf (wxT ("card_type LIKE '%%/%%'"));
        AddToWhereClause (sExpression);
        sSummary.Printf (wxT ("Combo"));
        AddToFilterSummary (sSummary);
    }
    if (pFilter->m_pReflex->IsChecked ()) {
        sExpression.Printf (wxT ("card_text LIKE '%%[REFLEX]%%'"));
        AddToWhereClause (sExpression);
        sSummary.Printf (wxT ("Reflex"));
        AddToFilterSummary (sSummary);
    }
}
