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

#include "browsercryptcontroller.h"

#include <wx/confbase.h>
#include <wx/fileconf.h>

BrowserCryptController::BrowserCryptController (BrowserCryptModel *pModel) :
    m_eMode (CRYPT_FILTER_KEEP),
    m_pModel (pModel),
    m_sFilterSummary (wxT ("")),
    m_sWhereClause (wxT ("WHERE "))
{

}


void
BrowserCryptController::RemoveCard (long lCard, wxString &sName, bool bUpdateView)
{
    wxString sQuery;

    m_sFilterSummary.Printf (wxT ("Remove %s"), sName.c_str ());

    sQuery.Printf (wxT ("DELETE FROM crypt_selection WHERE browser_num=%d AND card_name IN (SELECT card_ref FROM crypt_view WHERE (name_ref IN (SELECT name_ref FROM crypt_view WHERE card_ref = %ld)) AND (advanced IN (SELECT advanced FROM crypt_view WHERE card_ref = %ld )))"), m_pModel->GetIDNumber (), lCard, lCard);
    m_pModel->QueryCardData (sQuery, m_sFilterSummary, bUpdateView);
}


CryptFilterMode
BrowserCryptController::GetFilterMode ()
{
    return m_eMode;
}


void
BrowserCryptController::SetFilterMode (CryptFilterMode eMode)
{
    m_eMode = eMode;
}


// This function will collect input from the crypt filter dialog
// and modify the model accordingly
void
BrowserCryptController::UpdateModel (BrowserCryptFilter *pFilter)
{
    wxString sQuery, sSummary;

    DeleteWhereClause();
    DeleteFilterSummary();

    ProcessClans(pFilter);
    ProcessDisciplines(pFilter);
    ProcessTitles(pFilter);
    ProcessCapacities(pFilter);
    ProcessSpecials(pFilter);
    ProcessSects(pFilter);
    ProcessGroups(pFilter);
    ProcessFeather(pFilter);
    ProcessEditions(pFilter);
    ProcessRarity(pFilter);

    switch (m_eMode) {
    case CRYPT_FILTER_ADD:
        sQuery.Printf(m_pModel->m_sAddQuery, m_sWhereClause.c_str());
        sSummary.Printf(wxT ("Add "));
        break;
    case CRYPT_FILTER_REMOVE:
        sQuery.Printf(m_pModel->m_sRemoveQuery, m_sWhereClause.c_str());
        sSummary.Printf(wxT ("Remove "));
        break;
    case CRYPT_FILTER_KEEP:
        sQuery.Printf(m_pModel->m_sKeepQuery, m_sWhereClause.c_str());
        sSummary.Printf(wxT ("Select "));
        break;
    }
    sSummary.Append(m_sFilterSummary);
    m_pModel->QueryCardData(sQuery, sSummary);
}


void
BrowserCryptController::AddToFilterSummary (const wxString &sExpression)
{
    if (!m_sFilterSummary.IsEmpty ()) {
        m_sFilterSummary.Append (wxT ("and "));
    }

    m_sFilterSummary.Append (sExpression.c_str ());
    m_sFilterSummary.Append (wxT (" "));
}


void
BrowserCryptController::AddToWhereClause (const wxString &sExpression)
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
BrowserCryptController::DeleteFilterSummary ()
{
    m_sFilterSummary.Clear ();
}


void
BrowserCryptController::DeleteWhereClause ()
{
    m_sWhereClause.Clear ();
}


void
BrowserCryptController::ProcessCapacities (BrowserCryptFilter *pFilter)
{
    wxString sExpression;

    if (pFilter->m_pCapEqualCheckbox->IsChecked ()) {
        sExpression.Printf (wxT ("capacity = %d"), pFilter->m_pCapEqualSlider->GetValue ());
        AddToWhereClause (sExpression);
        AddToFilterSummary (sExpression);
    }
    if (pFilter->m_pCapGreaterCheckbox->IsChecked ()) {
        sExpression.Printf (wxT ("capacity >= %d"), pFilter->m_pCapGreaterSlider->GetValue ());
        AddToWhereClause (sExpression);
        AddToFilterSummary (sExpression);
    }
    if (pFilter->m_pCapLesserCheckbox->IsChecked ()) {
        sExpression.Printf (wxT ("capacity <= %d"), pFilter->m_pCapLesserSlider->GetValue ());
        AddToWhereClause (sExpression);
        AddToFilterSummary (sExpression);
    }
}


void
BrowserCryptController::ProcessClans (BrowserCryptFilter *pFilter)
{
    wxString sExpression, sTempString, sOperator, sSummary;
    InterfaceData *pUIData = InterfaceData::Instance ();

    for (unsigned int i = 0; i < pUIData->GetClans ()->GetCount (); i++) {
        if (pFilter->m_pClanList->IsSelected (i)) {
            sTempString.Printf (wxT ("%s%s(card_type = '%s')"),
                                sExpression.c_str (), sOperator.c_str (),
                                pUIData->GetClans ()->Item (i).c_str ());
            sExpression = sTempString;
            sTempString.Printf (wxT ("%s%s "), sSummary.c_str (),
                                pUIData->GetClans ()->Item (i).c_str ());
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
BrowserCryptController::ProcessDisciplines (BrowserCryptFilter *pFilter)
{
    InterfaceData *pUIData = InterfaceData::Instance ();
    wxString sExpression, sSummary;

    for (unsigned int i = 0; i < pUIData->GetDisciplines ()->GetCount (); i++) {
        switch (pFilter->m_oDisciplinesCombos[i]->GetSelection ()) {
        case 1:
            // Only inferior
            sExpression.Printf (wxT ("(inferior LIKE '%s') AND (superior NOT LIKE '%s')"),
                                pUIData->GetDisciplines ()->Item (i)[1].c_str(),
                                pUIData->GetDisciplines ()->Item (i)[2].c_str());
            AddToWhereClause (sExpression);
            sSummary.Printf (wxT ("inferior %s"), pUIData->GetDisciplines ()->Item (i)[0].c_str());
            AddToFilterSummary (sSummary);
            break;
        case 2:
            // Inferior or superior
            sExpression.Printf (wxT ("(inferior LIKE '%s') OR (superior LIKE '%s')"),
                                pUIData->GetDisciplines ()->Item (i)[1].c_str(),
                                pUIData->GetDisciplines ()->Item (i)[2].c_str());
            AddToWhereClause (sExpression);
            sSummary.Printf (wxT ("%s"), pUIData->GetDisciplines ()->Item (i)[0].c_str());
            AddToFilterSummary (sSummary);
            break;
        case 3:
            // Only superior
            sExpression.Printf (wxT ("superior LIKE '%s'"),
                                pUIData->GetDisciplines ()->Item (i)[2].c_str());
            AddToWhereClause (sExpression);
            sSummary.Printf (wxT ("superior %s"), pUIData->GetDisciplines ()->Item (i)[0].c_str());
            AddToFilterSummary (sSummary);
            break;
        case 4:
            // None
            sExpression.Printf (wxT ("disciplines NOT LIKE '%s'"),
                                pUIData->GetDisciplines ()->Item (i)[1].c_str());
            AddToWhereClause (sExpression);
            sSummary.Printf (wxT ("%s-less"), pUIData->GetDisciplines ()->Item (i)[0].c_str());
            AddToFilterSummary (sSummary);
            break;
        default:
            break;
        }
    }
}


void
BrowserCryptController::ProcessEditions (BrowserCryptFilter *pFilter)
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
BrowserCryptController::ProcessFeather (BrowserCryptFilter *pFilter)
{
    InterfaceData *pUIData = InterfaceData::Instance ();
    Database *pDatabase = Database::Instance ();
    RecordSet oDisciplinesEtc;
    wxString sQuery, sExpression, sSummary, sTmp;
    int iCardCount = 40;
    wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
    if (pConfig) {
        wxString sCardsLikeMaxCount
        = wxT ("FilterCardsLikeMaxCount");
        if (!pConfig->Read (sCardsLikeMaxCount, &iCardCount)) {
            pConfig->Write (sCardsLikeMaxCount, iCardCount);
            pConfig->Flush (TRUE);
        }
    }

    if (pFilter->m_lFeatherRef != -1 &&
            pFilter->m_pCardPicker->GetSelection () > 0) {

        // SELECT <all disciplines>, groupnumber from cards_crypt WHERE record_num= ref
        sQuery = wxT ("SELECT ");
        for (unsigned int c=0; c<pUIData->GetDisciplines ()->GetCount (); c++) {
            sQuery.Append (pUIData->GetDisciplines ()->Item (c)[0].Lower ());
            sQuery.Append (wxT (", "));
        }
        sTmp.Printf (wxT ("groupnumber from cards_crypt WHERE record_num='%ld'"),
                     pFilter->m_lFeatherRef);
        sQuery.Append (sTmp);
        pDatabase->Query (sQuery, &oDisciplinesEtc);

        if (oDisciplinesEtc.GetCount ()) {
            sExpression.Printf (wxT ("card_ref IN ("
                                     "SELECT DISTINCT record_num "
                                     "FROM cards_crypt "
                                     "WHERE (%d OR (abs (groupnumber - %s) <= 1))"
                                     "ORDER BY ("
                                     "abs (capacity - %d) / 2 +"),
                                !pFilter->m_pFeatherGrpCheckbox->IsChecked (),
                                oDisciplinesEtc.Item (0).Item (pUIData->GetDisciplines ()->GetCount ()).c_str (),
                                pFilter->m_pFeatherCapSlider->GetValue ());

            for (unsigned int c=0; c<pUIData->GetDisciplines ()->GetCount (); c++) {
                sExpression.Append (wxT ("abs ("));
                sExpression.Append (pUIData->GetDisciplines ()->Item (c)[0].Lower ());
                sExpression.Append (wxT (" - "));
                sExpression.Append (oDisciplinesEtc.Item (0).Item (c));
                sExpression.Append (wxT (") "));
                if (c < pUIData->GetDisciplines ()->GetCount () - 1)
                    sExpression.Append (wxT ("+"));
                else
                    sExpression.Append (wxT (") "));
            }
            sTmp.Printf (wxT ("ASC, capacity ASC LIMIT %d)"),
                         iCardCount);
            sExpression.Append (sTmp);

            AddToWhereClause (sExpression);

        }
        sSummary.Printf (wxT ("like %s\ncapacity ~%d"),
                         pFilter->m_pCardPicker->GetStringSelection ().c_str (),
                         pFilter->m_pFeatherCapSlider->GetValue ());
        AddToFilterSummary (sSummary);
    }
}


void
BrowserCryptController::ProcessGroups (BrowserCryptFilter *pFilter)
{
    wxString sExpression, sTempString, sOperator, sSummary (wxT ("group "));

    for (unsigned int i = 0; i <= g_uiGroupCount; i++) {
        if (pFilter->m_pGroupList->IsSelected (i)) {
            sTempString.Printf (wxT ("%s%s(groupnumber = %d)"),
                                sExpression.c_str (), sOperator.c_str (), i);
            sExpression = sTempString;
            sTempString.Printf (wxT ("%s%d "), sSummary.c_str (), i);
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
BrowserCryptController::ProcessRarity (BrowserCryptFilter *pFilter)
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
BrowserCryptController::ProcessSects (BrowserCryptFilter *pFilter)
{
    InterfaceData *pUIData = InterfaceData::Instance ();
    wxString sExpression, sTempString, sOperator, sSummary;

    for (unsigned int i = 0; i < pUIData->GetSects ()->GetCount (); i++) {
        if (pFilter->m_pSectList->IsSelected (i)) {
            sTempString.Printf (wxT ("%s%s(card_text LIKE '%s' or card_text LIKE 'Advanced, %s')"),
                                sExpression.c_str (), sOperator.c_str (),
                                pUIData->GetSects ()->Item (i)[1].c_str (),
                                pUIData->GetSects ()->Item (i)[1].c_str ());
            sExpression = sTempString;
            sTempString.Printf (wxT ("%s%s "), sSummary.c_str (),
                                pUIData->GetSects ()->Item(i)[0].c_str ());
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
BrowserCryptController::ProcessSpecials (BrowserCryptFilter *pFilter)
{
    InterfaceData *pUIData = InterfaceData::Instance();
    wxString sExpression, sSummary, sTemp;

    for (unsigned int i = 0; i < pUIData->GetSpecials()->GetCount(); i++) {
        if (pFilter->m_apSpecials[i]->IsChecked()) {
            if (pFilter->m_apSpecials[i]->Is3State()) {
                if (pFilter->m_apSpecials[i]->Get3StateValue() == wxCHK_CHECKED) {
                    sExpression.Printf (wxT ("card_text LIKE '%s'"), pUIData->GetSpecials()->Item(i)[1].c_str());
                    AddToWhereClause(sExpression);
                    sSummary.Printf(wxT ("%s"), pUIData->GetSpecials()->Item(i)[0].c_str());
                    AddToFilterSummary(sSummary);
                } else if (pFilter->m_apSpecials[i]->Get3StateValue() == wxCHK_UNDETERMINED) {
                    sExpression.Printf (wxT("card_text NOT LIKE '%s'"), pUIData->GetSpecials()->Item(i)[1].c_str());
                    AddToWhereClause(sExpression);
                    sSummary.Printf(wxT ("not %s"), pUIData->GetSpecials()->Item(i)[0].c_str());
                    AddToFilterSummary(sSummary);
                }
            } else {
                sExpression.Printf (wxT ("card_text LIKE '%s'"), pUIData->GetSpecials()->Item(i)[1].c_str());
                AddToWhereClause(sExpression);
                sSummary.Printf(wxT ("%s"), pUIData->GetSpecials()->Item(i)[0].c_str());
                AddToFilterSummary(sSummary);
            }
        }
    }

    if (!pFilter->m_pOtherText->GetValue().IsEmpty()) {
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
BrowserCryptController::ProcessTitles (BrowserCryptFilter *pFilter)
{
    wxString sExpression, sTempString, sOperator, sSummary;
    InterfaceData *pUIData = InterfaceData::Instance ();

    for (unsigned int i = 0; i < pUIData->GetTitles ()->GetCount (); i++) {
        if (pFilter->m_pTitleList->IsSelected (i)) {
            sTempString.Printf (wxT ("%s%s(title LIKE '%s')"),
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
