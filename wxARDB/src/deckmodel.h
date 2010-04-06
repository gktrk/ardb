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

#ifndef _deckmodel_h
#define _deckmodel_h

#include <wx/string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "database.h"

class DeckWindow;

class HappyBucket
{
public:
    HappyBucket () { }
    HappyBucket (wxString &sName, unsigned int uiTotal, unsigned int uiCount = 0) : m_sName (sName), m_uiDiscTotal (uiTotal), m_uiCardCount (uiCount) { }

    static int Compare (const HappyBucket **arg1, const HappyBucket **arg2) {
        return (*arg2)->m_uiDiscTotal - (*arg1)->m_uiDiscTotal;
    }

    wxString m_sName;
    unsigned int m_uiDiscTotal;
    unsigned int m_uiCardCount;
};

WX_DECLARE_LIST(HappyBucket, HappyList);

class DeckModel
{
public:
    static const wxString s_sDeckInfoQuery;
    static const wxString s_sLibraryViewQuery;
    static const wxString s_sCryptViewQuery;

    // Singleton access
    static DeckModel *Instance ();
    static void DeleteInstance ();

    DeckModel ();
    ~DeckModel ();

    void ComputeCryptHappiness ();

    // File operations
    bool ExportToELD ();
    bool ExportToHTML ();
    bool ExportToJOL ();
    bool ExportToLackey();
    bool ExportToPhpBB ();
    bool ExportToText ();
    bool ExportToXML ();
    bool ExportToSecretLibrary(wxString &sUsername, wxString &sPassword);
    bool ExportWithXSL (wxString &sFileName, wxString *pXSL);
    bool XmlToXslt(wxString &sResult, wxString *pXSL);
    bool ImportFromELD ();
    bool ImportFromXML ();
    bool ImportFromXML (wxString &sFileName, bool bImportAll = TRUE);
    bool MergeFromXML ();

    // Database operations
    bool OpenFromDatabase (wxString &sDeckName);
    bool SaveToDatabase (wxString &sDeckName);

    // Accessors
    wxString& GetAuthor ();
    double GetCryptAvg () {
        return m_lCryptAvg;
    }
    unsigned int GetCryptCount () {
        return m_lCryptCount;
    }
    RecordSet *GetCryptList ();
    long GetCryptMax () {
        return m_lCryptMax;
    }
    long GetCryptMin () {
        return m_lCryptMin;
    }
    wxString& GetDescription ();
    HappyList *GetHappyList () {
        return &m_oHappyList;
    }
    unsigned int GetLibraryCount () {
        return m_lLibraryCount;
    }
    RecordSet *GetLibraryList ();
    RecordSet *GetLibStatsClans () {
        return &m_oLibStatsClans;
    }
    RecordSet *GetLibStatsDiscisplines () {
        return &m_oLibStatsDisciplines;
    }
    RecordSet *GetLibStatsRequirements () {
        return &m_oLibStatsRequirements;
    }
    long GetLibStatsBloodCost () {
        return m_lTotalBloodCost;
    }
    long GetLibStatsPoolCost () {
        return m_lTotalPoolCost;
    }
    wxString& GetName ();
    void SetAuthor (wxString &sAuthor);
    void SetDescription (wxString &sDescription);
    void SetHappyDisciplineCount (unsigned int uiCount) {
        m_uiHappyDisciplineCount = uiCount;
    }
    void SetHappyLibrarySize (unsigned int uiSize) {
        m_uiHappyLibrarySize = uiSize;
    }
    void SetHappyMasterPercentage (unsigned int uiPercent) {
        m_uiHappyMasterPercentage = uiPercent;
    }
    void SetName (wxString &sName);

    void Clear (); // clears the current deck

    // Crypt manipulations
    wxArrayString * AddToCrypt (wxString sName, wxString sAdvanced, wxString sSet, unsigned int uiCount = 1, bool bRefreshUI = TRUE);
    wxArrayString * AddToCrypt (long lCardRef, unsigned int uiCount = 1, bool bRefreshUI = TRUE);
    void DelFromCrypt (long lCardRef, int iCount = 1, bool bRefreshUI = TRUE);
    void SetCryptRefAmount (long lCardRef, unsigned int uiCount, bool bRefreshUI = TRUE);

    // Library manipultations
    wxArrayString * AddToLibrary (wxString sName, wxString sSet, unsigned int uiCount = 1, bool bRefreshUI = TRUE);
    wxArrayString * AddToLibrary (long lCardRef, unsigned int uiCount = 1, bool bRefreshUI = TRUE);
    void DelFromLibrary (long lCardRef, int iCount = 1, bool bRefreshUI = TRUE);
    void ResizeLibrary (unsigned int uiCount);
    void SetLibraryRefAmount (long lCardRef, unsigned int uiCount, bool bRefreshUI = TRUE);

    void ShouldSaveWarning();

private:

    bool                    m_bReady;
    bool                    m_bSaved;

    // Singleton pointer
    static DeckModel       *spInstance;

    // The associated View
    DeckWindow             *m_pView;

    // Crypt stats
    double                  m_lCryptAvg;
    long                    m_lCryptMax;
    long                    m_lCryptMin;

    // number of cards
    long                    m_lCryptCount;
    long                    m_lLibraryCount;

    // Library and crypt lists
    RecordSet               m_oCryptList;
    HappyList               m_oHappyList;
    RecordSet               m_oLibraryList;
    // Stats
    RecordSet               m_oLibStatsClans;
    RecordSet               m_oLibStatsDisciplines;
    RecordSet               m_oLibStatsRequirements;
    long                    m_lTotalBloodCost;
    long                    m_lTotalPoolCost;

    // Misc. info
    wxString                m_sAuthor;
    wxString                m_sDescription;
    wxString                m_sName;
    //  wxString                m_sLastModified;

    // Happy Families parameters
    unsigned int            m_uiHappyDisciplineCount;
    unsigned int            m_uiHappyLibrarySize;
    unsigned int            m_uiHappyMasterPercentage;

    void ComputeLibraryStats ();
    void RefreshModel (bool bRefreshUI = FALSE);

    wxString StripInvalidFilename(wxString name);

    // XML stuff that needs wrapping
    xmlNodePtr my_xmlNewChild (xmlNodePtr parent,
                               xmlNsPtr ns,
                               wxString sName,
                               wxString sContent);
    xmlAttrPtr my_xmlNewProp (xmlNodePtr node,
                              wxString sName,
                              wxString sValue);

};

#include "deckwindow.h"

#endif
