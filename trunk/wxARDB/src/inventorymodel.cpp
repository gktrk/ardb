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

#include "inventorymodel.h"
#include "interfacedata.h"
#include "database.h"
#include "updater.h"

#include "importxml.h"

#ifdef __WXMAC__
// required for some reason by libxslt on macOS X
#include <libxml/xmlexports.h>
#include <libxml/xmlversion.h>
#endif

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include <wx/confbase.h>
#include <wx/fileconf.h>

#define IGNORE_AMOUNT -6885

InventoryModel *InventoryModel::spInstance = NULL;


InventoryModel::InventoryModel () :
  m_oCryptList (),
  m_oLibraryList (),
  m_sName (wxT ("inventory"))
{

}


InventoryModel::~InventoryModel ()
{

}


void 
InventoryModel::DeleteInstance ()
{
  if (spInstance != NULL)
    {
      delete spInstance;
      spInstance = NULL;
    }
}


bool
InventoryModel::ExportToCSV ()
{
  Database *pDatabase = Database::Instance ();
  wxString sXSL, sConfEntry = wxT("InventoryCSVTemplate");
  wxString sFile = m_sName;
  
  if (pDatabase == NULL) return false;

  sXSL << pDatabase->GetDatabaseDirectory () 
       << wxFileName::GetPathSeparator ()
       << wxT("inv2csv.xsl");

  wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
  if (pConfig)
    {
      if (!pConfig->Read (sConfEntry, &sXSL))
	{
	  pConfig->Write (sConfEntry, sXSL);
	  pConfig->Flush (TRUE);
	}
    }

  sFile.Append (wxT (".csv"));

  wxFileDialog oFileDialog (g_pMainWindow, wxT ("Export inventory..."),
			    wxT (""), sFile, wxT ("*.csv"),
			    wxSAVE | wxOVERWRITE_PROMPT);
  if (oFileDialog.ShowModal () != wxID_OK)
    {
      return true;
    }

  sFile = oFileDialog.GetDirectory () << wxFileName::GetPathSeparator ()
				      << oFileDialog.GetFilename ();

  return ExportWithXSL (sFile, &sXSL);
}


bool
InventoryModel::ExportToHTML ()
{
  Database *pDatabase = Database::Instance ();
  wxString sXSL, sConfEntry = wxT("InventoryHTMLTemplate");
  wxString sFile = m_sName;
  
  if (pDatabase == NULL) return false;

  sXSL << pDatabase->GetDatabaseDirectory () 
       << wxFileName::GetPathSeparator ()
       << wxT("inv2html.xsl");

  wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
  if (pConfig)
    {
      if (!pConfig->Read (sConfEntry, &sXSL))
	{
	  pConfig->Write (sConfEntry, sXSL);
	  pConfig->Flush (TRUE);
	}
    }

  sFile.Append (wxT (".html"));

  wxFileDialog oFileDialog (g_pMainWindow, wxT ("Export inventory..."),
			    wxT (""), sFile, wxT ("*.html"),
			    wxSAVE | wxOVERWRITE_PROMPT);
  if (oFileDialog.ShowModal () != wxID_OK)
    {
      return true;
    }

  sFile = oFileDialog.GetDirectory () << wxFileName::GetPathSeparator ()
				      << oFileDialog.GetFilename ();

  return ExportWithXSL (sFile, &sXSL);
}


bool
InventoryModel::ExportToText ()
{
  Database *pDatabase = Database::Instance ();
  wxString sXSL, sConfEntry = wxT("InventoryTextTemplate");
  wxString sFile = m_sName;
  
  if (pDatabase == NULL) return false;

  sXSL << pDatabase->GetDatabaseDirectory () 
       << wxFileName::GetPathSeparator ()
       << wxT("inv2text.xsl");

  wxFileConfig *pConfig = (wxFileConfig *) wxFileConfig::Get ();
  if (pConfig)
    {
      if (!pConfig->Read (sConfEntry, &sXSL))
	{
	  pConfig->Write (sConfEntry, sXSL);
	  pConfig->Flush (TRUE);
	}
    }

  sFile.Append (wxT (".txt"));

  wxFileDialog oFileDialog (g_pMainWindow, wxT ("Export inventory..."),
			    wxT (""), sFile, wxT ("*.txt"),
			    wxSAVE | wxOVERWRITE_PROMPT);
  if (oFileDialog.ShowModal () != wxID_OK)
    {
      return true;
    }
  
  sFile = oFileDialog.GetDirectory () << wxFileName::GetPathSeparator ()
				      << oFileDialog.GetFilename ();
  
  return ExportWithXSL (sFile, &sXSL);
}


bool
InventoryModel::ExportToXML ()
{
  wxString sFile = m_sName;
  
  sFile.Append (wxT (".xml"));
  
  wxFileDialog oFileDialog (g_pMainWindow, wxT ("Save inventory..."),
			    wxT (""), sFile, wxT ("*.xml"), 
			    wxSAVE | wxOVERWRITE_PROMPT);
  if (oFileDialog.ShowModal () != wxID_OK)
    {
      return true;
    }
  
  sFile = oFileDialog.GetDirectory () << wxFileName::GetPathSeparator ()
				      << oFileDialog.GetFilename ();
  
  return ExportWithXSL (sFile, NULL);
}


bool
InventoryModel::ExportWithXSL (wxString &sFileName, wxString *pXSL)
{
#ifdef LIBXML_TREE_ENABLED

  bool bReturnValue;
  xmlDocPtr doc, res;                   // document pointer
  xmlNodePtr nRoot = NULL, node = NULL; // node pointers
  xmlNodePtr nLibrary, nCrypt;          // more node pointers
  xmlDtdPtr dtd = NULL;                 // DTD pointer
  xmlNodePtr nStylesheet = NULL;        // xsl stylesheet node pointer
  xsltStylesheetPtr cur = NULL;         // xsl stylesheet
  wxString sCount;
#ifdef __WXMSW__
  wxString sTemp;
#endif

  Database *pDatabase = Database::Instance ();
  pDatabase->Query (wxT ("SELECT "
                         "  card_ref, "
			 "  number_owned, "
			 "  number_to_trade, "
			 "  number_wanted, "
			 "  card_name, "
			 "  set_name, "
			 "  rarity, "
			 "  advanced "
			 "FROM inventory_view_crypt "
			 "WHERE number_owned OR number_to_trade OR number_wanted "
			 "ORDER BY card_name "), &m_oCryptList);
  pDatabase->Query (wxT ("SELECT "
                         "  card_ref, "
			 "  number_owned, "
			 "  number_to_trade, "
			 "  number_wanted, "
			 "  card_name, "
			 "  set_name, "
			 "  rarity "
			 "FROM inventory_view_library "
			 "WHERE number_owned OR number_to_trade OR number_wanted "
			 "ORDER BY card_name "), &m_oLibraryList);


  LIBXML_TEST_VERSION;
 
  // Creates a new document, a node and set it as a root node
  doc = xmlNewDoc (BAD_CAST "1.0");  
  nRoot = xmlNewNode (NULL, BAD_CAST "inventory");  
  my_xmlNewProp (nRoot, wxT ("formatVersion"), wxT ("-TODO-1.0"));
  my_xmlNewProp (nRoot, wxT ("databaseVersion"), wxT ("-TODO-20040101"));
  my_xmlNewProp (nRoot, wxT ("generator"), wxT ("Anarch Revolt Deck Builder"));
  xmlDocSetRootElement (doc, nRoot);
  
  // Creates a DTD declaration.
  dtd = xmlCreateIntSubset (doc, BAD_CAST "inventory", 
 			    NULL, BAD_CAST "AnarchRevoltInv.dtd");

  // Creates a default stylesheet declaration
  nStylesheet = xmlNewPI (BAD_CAST "xml-stylesheet", BAD_CAST "type=\"text/xsl\" href=\"inv2html.xsl\"");
  xmlAddPrevSibling (nRoot, nStylesheet);

  // Add the inventory's information nodes
  my_xmlNewChild (nRoot, NULL, wxT ("date"), wxNow ());
  //  my_xmlNewChild (nRoot, NULL, wxT ("author"), m_sAuthor);

  // TODO: //author/@contact

  // Add the crypt node
  nCrypt = my_xmlNewChild (nRoot, NULL, wxT ("crypt"), wxT (""));
  sCount = wxT ("0");
  my_xmlNewProp (nCrypt, wxT("size"), sCount);
  
  // Add the crypt cards
  for (unsigned int i = 0; i < m_oCryptList.GetCount (); i++)
    {
      /*
	Reminder of the query string used to fill m_oCryptList
                       card_ref, 
                       number_owned, 
                       number_to_trade, 
                       number_wanted, 
                       card_name, 
                       set_name, 
                       rarity, 
                       advanced 
      */      

      // The card node contains mandatory props
      node = my_xmlNewChild (nCrypt, NULL, wxT ("vampire"), wxT (""));
      my_xmlNewProp (node, wxT ("databaseID"), 
      		 m_oCryptList.Item (i).Item (0));
      my_xmlNewProp (node, wxT ("have"), 
      		 m_oCryptList.Item (i).Item (1));
      my_xmlNewProp (node, wxT ("spare"), 
      		 m_oCryptList.Item (i).Item (2));
      my_xmlNewProp (node, wxT ("need"), 
      		 m_oCryptList.Item (i).Item (3));

      // These are phony children nodes to help interoperabilty
      // and allow xsl to generate pretty things
      my_xmlNewChild (node, NULL, wxT ("name"),
		  m_oCryptList.Item (i).Item (4));
      my_xmlNewChild (node, NULL, wxT ("set"),
		  m_oCryptList.Item (i).Item (5));
      my_xmlNewChild (node, NULL, wxT ("rarity"),
		  m_oCryptList.Item (i).Item (6));
      my_xmlNewChild (node, NULL, wxT ("adv"),
		  m_oCryptList.Item (i).Item (7));
    }
  
  // Add the library node
  nLibrary = my_xmlNewChild (nRoot, NULL, wxT ("library"), wxT (""));
  sCount = wxT ("0");
  my_xmlNewProp (nLibrary, wxT ("size"), sCount);
  
  // Add the library cards
  for (unsigned int i = 0; i < m_oLibraryList.GetCount (); i++)
    {
      /*
	Reminder of the query string used to fill m_oLibraryList
                       card_ref, 
                       number_owned, 
                       number_to_trade, 
                       number_wanted, 
                       card_name, 
                       set_name, 
                       rarity, 
      */      

      // The card node contains mandatory props
      node = my_xmlNewChild (nLibrary, NULL, wxT ("card"), wxT (""));
      my_xmlNewProp (node, wxT ("databaseID"), 
      		 m_oLibraryList.Item (i).Item (0));
      my_xmlNewProp (node, wxT ("have"), 
      		 m_oLibraryList.Item (i).Item (1));
      my_xmlNewProp (node, wxT ("spare"), 
      		 m_oLibraryList.Item (i).Item (2));
      my_xmlNewProp (node, wxT ("need"), 
      		 m_oLibraryList.Item (i).Item (3));

      // These are phony children nodes to help interoperabilty
      // and allow xsl to generate pretty things
      my_xmlNewChild (node, NULL, wxT ("name"),
		  m_oLibraryList.Item (i).Item (4));
      my_xmlNewChild (node, NULL, wxT ("set"),
		  m_oLibraryList.Item (i).Item (5));
      my_xmlNewChild (node, NULL, wxT ("rarity"),
		  m_oLibraryList.Item (i).Item (6));
    }

  
  // No XSL means we save in XML format
  if (pXSL == NULL) 
    {
      // Write the xml tree to a file
      bReturnValue = xmlSaveFormatFileEnc (sFileName.mb_str (wxConvLibc),
					   doc, "UTF-8", 1) >= 0;
    }
  else
    {
      xmlChar acXSLname[1024];
      memcpy (acXSLname, pXSL->mb_str (wxConvLibc), 
	      (pXSL->Length () + 1) * sizeof (wxChar));

      xmlSubstituteEntitiesDefault (1);
      xmlLoadExtDtdDefaultValue = 1;
      cur = xsltParseStylesheetFile (acXSLname);
      
      if (cur != NULL) 
	{
	  res = xsltApplyStylesheet (cur, doc, NULL);
	  if (res != NULL)
	    {
	      bReturnValue = xsltSaveResultToFilename (sFileName.mb_str (wxConvLibc), res, cur, 0) >= 0;
	      xmlFreeDoc (res);
	    }
	  else
	    {
	      bReturnValue = FALSE;
	    }
	  xsltFreeStylesheet (cur);
	}
      else
	{
	  bReturnValue = FALSE;
	  wxLogError (wxT ("Can't open XSL file %s"), pXSL->c_str ());
	}

      xsltCleanupGlobals ();
    }

  // Free the document
  xmlFreeDoc (doc);

  // Free the global variables that may have been allocated by the parser.
  xmlCleanupParser ();

  return bReturnValue;

#else
  wxLogError (wxT ("XML tree not supported."));
  return 0;
#endif
}


InventoryModel *
InventoryModel::Instance () 
{
  if (spInstance == NULL)
    {
      spInstance = new InventoryModel ();
    }
  return spInstance;
}
 

long
InventoryModel::GetHaveCrypt (long lRef)
{
  Database *pDatabase = Database::Instance ();
  wxString sQuery;
  RecordSet *pRecordSet;
  long lAmount;
  
  sQuery.Printf (wxT ("SELECT sum(number_owned) "
		      "  FROM inventory_view_crypt "
		      "  WHERE card_ref IN "
		      "  (SELECT record_num "
		      "     FROM cards_crypt "
		      "     WHERE card_name IN "
		      "     (SELECT card_name " 
		      "        FROM cards_crypt "
		      "        WHERE record_num=%d))"),
		 lRef);
  pDatabase->ToggleVerbose ();
  pRecordSet = pDatabase->Query (sQuery);
  pDatabase->ToggleVerbose ();

  if (pRecordSet && pRecordSet->Count ())
    {
      pRecordSet->Item (0).Item (0).ToLong (&lAmount);
      return lAmount;
    }
  return 0;
}

bool
InventoryModel::ImportFromCSV ()
{
  Database *pDatabase = Database::Instance ();
  int iNumFields;
  unsigned int uiIndex = 0;
  long lHave, lWant;
  wxArrayString oArray;
  wxString sFile, sQuery;

  if (!pDatabase) return FALSE;

  wxFileDialog oFileDialog (g_pMainWindow, wxT ("Import inventory..."), wxT (""), wxT (""), wxT ("*.csv"), wxOPEN);
  if (oFileDialog.ShowModal () != wxID_OK) return FALSE;

  sFile = oFileDialog.GetDirectory () 
    << wxFileName::GetPathSeparator () << oFileDialog.GetFilename ();
      
  wxFileInputStream oInput (sFile);
  Updater::decodeCSV (&oInput, ',', '"', -1, &iNumFields, &oArray, TRUE);

  // Sanity check
  if (iNumFields != 5)
    {
      wxLogError (wxT ("Wrong number of columns in CSV file (%d instead of 5)."), iNumFields);
      return FALSE;
    }
  
  // string cleanup
  for (unsigned int i = 0; i < oArray.GetCount (); i+=5) 
    {
      // Remove unnecessary quotes
      oArray.Item (i).Replace (wxT ("\""), wxT (""));
      // Replace ticks with apostrophes
      oArray.Item (i).Replace (wxT ("`"), wxT ("'"));
    }

  pDatabase->Query (wxT ("BEGIN TRANSACTION;"));

  // Import crypt
  while (uiIndex < oArray.GetCount () - 1)
    {
      wxString& sName = oArray.Item (uiIndex++);
      oArray.Item (uiIndex++).ToLong (&lHave);
      oArray.Item (uiIndex++).ToLong (&lWant);
      uiIndex++; // empty field of unknown use
      wxString& sCategory = oArray.Item (uiIndex++);
      
      // Dirty hack to skip to library import
      if (sCategory.Find (wxT ("Crypt")) >= 0) 
	{
	  // Check wether it's an advanced vampire or not
	  if (sName.Replace (wxT (" (ADV)"), wxT ("")))
	    {
	      // Try to add an advanced vampire
	      SetHWSCryptName (sName, wxEmptyString, TRUE,
			       lHave, lWant, IGNORE_AMOUNT);
	    }
	  else
	    {
	      // Try to add a regular vampire
	      SetHWSCryptName (sName, wxEmptyString, FALSE,
			       lHave, lWant, IGNORE_AMOUNT);
	    }
	}
      else
	{
	  // It's a library card
	  SetHWSLibraryName (sName, wxEmptyString,
			 lHave, lWant, IGNORE_AMOUNT);
	}
    }

  pDatabase->Query (wxT ("COMMIT TRANSACTION;"));
  return TRUE;
}


bool
InventoryModel::ImportFromXML ()
{
  wxString sFile;

  wxFileDialog oFileDialog (g_pMainWindow, wxT ("Open inventory..."), wxT (""), wxT (""), wxT ("*.xml"), wxOPEN);
  if (oFileDialog.ShowModal () == wxID_OK)
    {

      sFile = oFileDialog.GetDirectory () 
	<< wxFileName::GetPathSeparator () << oFileDialog.GetFilename ();
      
      if (!ImportFromXML (sFile))
	{
	  wxLogError (wxT ("An error occured while opening %s"), sFile.c_str ());
	  return FALSE;
	}
    }
  else 
    return FALSE;
  return TRUE;
}


bool
InventoryModel::ImportFromXML (wxString &sFileName)
{
	Database *pDatabase = Database::Instance ();

	bool bMorphed = FALSE;
	xmlChar *pString, *pStringName, *pStringSet, *pStringAdvanced, *pDatabaseID = 0, *pHave = 0, *pSpare = 0, *pWant = 0;
	xmlDocPtr doc; // the document tree
	xmlXPathContextPtr xpathCtx; 
	xmlXPathObjectPtr xpathObj; 
	xmlNodeSetPtr nodes;
	xmlNodePtr cur, tmpnode;
	int size;
	wxString xmlStringDoc;

	xmlInitParser ();
	LIBXML_TEST_VERSION;

	xmlStringDoc = ReadXmlFile(sFileName);
	doc = xmlParseDoc((xmlChar *)xmlStringDoc.c_str());

	if (doc == NULL) return 0;

	// Create xpath evaluation context
	xpathCtx = xmlXPathNewContext (doc);
	if (xpathCtx == NULL) 
	{
		xmlFreeDoc(doc); 
		return 0;
	}

	// TODO: check the validity using the DTD

	pDatabase->Query (wxT ("BEGIN TRANSACTION;"));

	// Get the inventory's crypt
	xpathObj = xmlXPathEvalExpression (BAD_CAST "/inventory/crypt/vampire/@*", xpathCtx);
	if (xpathObj == NULL)
	{
		xmlXPathFreeContext(xpathCtx); 
		xmlFreeDoc(doc); 
		return 0;
	}
	nodes = xpathObj->nodesetval;
	size = (nodes) ? nodes->nodeNr : 0;
	for(int i = 0; i < size; i++) 
	{
		if (nodes->nodeTab[i])
		{
			cur = nodes->nodeTab[i];
			pString = xmlXPathCastNodeToString (cur);
			if (!strcmp ((char *) cur->name, "databaseID"))
				pDatabaseID = pString;
			else if (!strcmp ((char *) cur->name, "have"))
				pHave = pString;
			else if (!strcmp ((char *) cur->name, "need"))
				pWant = pString;
			else if (!strcmp ((char *) cur->name, "spare"))
				pSpare = pString;

			if (pDatabaseID && pHave && pWant && pSpare)
			{
				long lID, lHave, lWant, lSpare;
				wxString sID ((const char *) pDatabaseID, wxConvLibc), 
					sHave ((const char *) pHave, wxConvLibc),
					sWant ((const char *) pWant, wxConvLibc),
					sSpare ((const char *) pSpare, wxConvLibc);

				sID.ToLong (&lID);
				sHave.ToLong (&lHave);
				sWant.ToLong (&lWant);
				sSpare.ToLong (&lSpare);


				wxString sQuery;
				wxArrayString *pResult;
				RecordSet oRecordSet;

				// Get the name we have for this card in the DB
				sQuery.Printf (wxT ("SELECT card_name FROM crypt_view WHERE card_ref = %d;"), lID);
				pDatabase->Query (sQuery, &oRecordSet);
				pResult = NULL;
				if (oRecordSet.GetCount () > 0)
					pResult = &(oRecordSet.Item (0));
				// 	      else
				// 		wxLogMessage (wxT ("card_ref not found : %d"), lID);

				// Get the name we have for this card in the XML
				tmpnode = cur->parent->children;
				while (tmpnode && strcmp ((char *) tmpnode->name, "name")) 
				{
					tmpnode = tmpnode->next;
				}
				if (tmpnode)
				{
					pStringName = xmlXPathCastNodeToString (tmpnode);
					if (pStringName)
					{
						wxString sStringName ((const char *) pStringName, wxConvUTF8);
						// Compare the card names
						// ARDB Should be Set-ignorant for now
						// if (pResult && !sStringName.Cmp (pResult->Item (0)))
						if (0)
						{
							// OK these are the same names
							SetHaveCrypt (lID, lHave);
							SetWantCrypt (lID, lWant);
							SetSpareCrypt (lID, lSpare);
						}
						else
						{
							// Name mismatch -> DB morphed, we'll trust the XML info
							bMorphed = TRUE;

							// Get the card set
							tmpnode = cur->parent->children;
							while (tmpnode && strcmp ((char *) tmpnode->name, "set")) 
							{
								tmpnode = tmpnode->next;
							}
							pStringSet = NULL;
							if (tmpnode)
							{
								pStringSet = xmlXPathCastNodeToString (tmpnode);
							}

							// Get the advanced flag
							tmpnode = cur->parent->children;
							while (tmpnode && strcmp ((char *) tmpnode->name, "adv")) 
							{
								tmpnode = tmpnode->next;
							}
							pStringAdvanced = NULL;
							if (tmpnode)
							{
								pStringAdvanced = xmlXPathCastNodeToString (tmpnode);
							}

							if (pStringSet && pStringAdvanced)
							{
								wxString sStringSet ((const char *) pStringSet, wxConvUTF8);
								wxString sStringAdv ((const char *) pStringAdvanced, wxConvUTF8);

								/*
								// ARDB Should be Set-ignorant for now
								SetHWSCryptName (sStringName, sStringSet, sStringAdv.Length () > 0,
								lHave, lWant, lSpare);
								*/
								SetHWSCryptName (sStringName, wxEmptyString, sStringAdv.Length () > 0,
									lHave, lWant, lSpare);

								free (pStringSet);
								free (pStringAdvanced);
								pStringSet = 0;
								pStringAdvanced = 0;
							}
							else
							{
								SetHWSCryptName (sStringName, wxEmptyString, FALSE,
									lHave, lWant, lSpare);
							}

						}
						free (pStringName);
						pStringName = 0;
					}
					else
					{
						wxLogMessage (wxT ("DB/XML mismatch for ID %d, ignoring that card"), lID);
					}
				}

				free (pDatabaseID);
				free (pHave);
				free (pWant);
				free (pSpare);
				pDatabaseID = 0;
				pHave = 0;
				pWant = 0;
				pSpare = 0;
			}
		}
	}
	free (xpathObj);

	// Get the inventory's library
	xpathObj = xmlXPathEvalExpression (BAD_CAST "/inventory/library/card/@*", xpathCtx);
	if (xpathObj == NULL)
	{
		xmlXPathFreeContext(xpathCtx); 
		xmlFreeDoc(doc); 
		return 0;
	}
	nodes = xpathObj->nodesetval;
	size = (nodes) ? nodes->nodeNr : 0;
	for(int i = 0; i < size; i++) 
	{
		if (nodes->nodeTab[i])
		{
			cur = nodes->nodeTab[i];
			pString = xmlXPathCastNodeToString (cur);
			if (!strcmp ((char *) cur->name, "databaseID"))
				pDatabaseID = pString;
			else if (!strcmp ((char *) cur->name, "have"))
				pHave = pString;
			else if (!strcmp ((char *) cur->name, "need"))
				pWant = pString;
			else if (!strcmp ((char *) cur->name, "spare"))
				pSpare = pString;

			if (pDatabaseID && pHave && pWant && pSpare)
			{
				long lID, lHave, lWant, lSpare;
				wxString sID ((const char *) pDatabaseID, wxConvLibc), 
					sHave ((const char *) pHave, wxConvLibc),
					sWant ((const char *) pWant, wxConvLibc),
					sSpare ((const char *) pSpare, wxConvLibc);

				sID.ToLong (&lID);
				sHave.ToLong (&lHave);
				sWant.ToLong (&lWant);
				sSpare.ToLong (&lSpare);


				wxString sQuery;
				wxArrayString *pResult;
				RecordSet oRecordSet;

				// Get the name we have for this card in the DB
				sQuery.Printf (wxT ("SELECT card_name FROM library_view WHERE card_ref = %d;"), lID);
				pDatabase->Query (sQuery, &oRecordSet);
				pResult = NULL;
				if (oRecordSet.GetCount () > 0)
					pResult = &(oRecordSet.Item (0));
				// 	      else
				// 		wxLogMessage (wxT ("card_ref not found : %d"), lID);

				// Get the name we have for this card in the XML
				tmpnode = cur->parent->children;
				while (tmpnode && strcmp ((char *) tmpnode->name, "name")) 
				{
					tmpnode = tmpnode->next;
				}
				if (tmpnode)
				{
					pStringName = xmlXPathCastNodeToString (tmpnode);
					if (pStringName)
					{
						wxString sStringName ((const char *) pStringName, wxConvUTF8);
						// Compare the card names
						// ARDB Should be Set-ignorant for now
						// if (pResult && !sStringName.Cmp (pResult->Item (0)))
						if (0)
						{
							// OK these are the same names
							SetHaveLibrary (lID, lHave);
							SetWantLibrary (lID, lWant);
							SetSpareLibrary (lID, lSpare);
						}
						else
						{
							// Name mismatch -> DB morphed, we'll trust the XML info
							bMorphed = TRUE;

							// Get the card set
							tmpnode = cur->parent->children;
							while (tmpnode && strcmp ((char *) tmpnode->name, "set")) 
							{
								tmpnode = tmpnode->next;
							}
							pStringSet = NULL;
							if (tmpnode)
							{
								pStringSet = xmlXPathCastNodeToString (tmpnode);
							}

							if (pStringSet)
							{
								wxString sStringSet ((const char *) pStringSet, wxConvUTF8);

								/*
								// ARDB Should be Set-ignorant for now				  
								SetHWSLibraryName (sStringName, sStringSet,
								lHave, lWant, lSpare);
								*/
								SetHWSLibraryName (sStringName, wxEmptyString,
									lHave, lWant, lSpare);

								free (pStringSet);
								pStringSet = 0;
							}
							else
							{
								SetHWSLibraryName (sStringName, wxEmptyString,
									lHave, lWant, lSpare);
							}

						}
						free (pStringName);
						pStringName = 0;
					}
					else
					{
						wxLogMessage (wxT ("DB/XML mismatch for ID %d, ignoring that card"), lID);
					}
				}

				free (pDatabaseID);
				free (pHave);
				free (pWant);
				free (pSpare);
				pDatabaseID = 0;
				pHave = 0;
				pWant = 0;
				pSpare = 0;
			}
		}
	}
	free (xpathObj);


	// free
	xmlXPathFreeContext(xpathCtx); 
	xmlFreeDoc(doc);

	// Cleanup function for the XML library.
	xmlCleanupParser();

	pDatabase->Query (wxT ("COMMIT TRANSACTION;"));

	if (bMorphed)
	{
		ExportWithXSL (sFileName, NULL);
	}

  return 1;
}


xmlNodePtr
InventoryModel::my_xmlNewChild (xmlNodePtr parent, 
			   xmlNsPtr ns, 
			   wxString sName, 
			   wxString sContent)
{
#ifdef LIBXML_TREE_ENABLED
  return xmlNewTextChild (parent, ns, 
			  BAD_CAST (const char *) sName.mb_str (wxConvUTF8), 
			  BAD_CAST (const char *) sContent.mb_str (wxConvUTF8));
#endif
}


xmlAttrPtr
InventoryModel::my_xmlNewProp (xmlNodePtr node, 
			  wxString sName, 
			  wxString sValue)
{
#ifdef LIBXML_TREE_ENABLED
  return xmlNewProp (node, 
		     BAD_CAST (const char *) sName.mb_str (wxConvUTF8), 
		     BAD_CAST (const char *) sValue.mb_str (wxConvUTF8));
#endif
}


void 
InventoryModel::SetHaveCrypt (long lRef, long lAmount)
{
  Database *pDatabase = Database::Instance ();
  wxString sQuery;
  
  //  wxLogMessage (wxT ("avant:%d"), GetHaveCrypt (lRef));

  sQuery.Printf (wxT ("UPDATE inventory_crypt "
		      "  SET number_owned=%d "
		      "  WHERE card_ref=%d"),
		 lAmount,
		 lRef);
  pDatabase->Query (sQuery);
}


void
InventoryModel::SetHWSCryptName (wxString sName, wxString sSet, bool bAdvanced, 
				 long lHave, long lWant, long lSpare)
{
  Database *pDatabase = Database::Instance ();
  wxString sQuery;
  long lCardRef = -1;

  sName.Replace (wxT ("'"), wxT ("''"));

  // Check wether it's an advanced vampire or not
  if (bAdvanced)
    {
      // Try to find an advanced vampire
      sQuery.Printf (wxT ("SELECT card_ref FROM crypt_view WHERE dumbitdown(card_name) LIKE dumbitdown('%s') AND advanced = 'Advanced' "), sName.c_str ());
    }
  else
    {
      // Try to find a regular vampire
      sQuery.Printf (wxT ("SELECT card_ref FROM crypt_view WHERE dumbitdown(card_name) LIKE dumbitdown('%s') "), sName.c_str ());
    }
  
  if (sSet.Length ())
    {
      sQuery << wxT ("AND set_name = '") << sSet << wxT ("' ");
    } 
  
  sQuery <<  wxT ("ORDER BY card_ref ASC LIMIT 1");

  RecordSet * pResult = pDatabase->Query (sQuery);
     
  if (pResult && pResult->GetCount () && 
      pResult->Item (0).GetCount ())
    {
      // Add whatever we've found
      pResult->Item (0).Item (0).ToLong (&lCardRef);
      if (lHave != IGNORE_AMOUNT) SetHaveCrypt (lCardRef, lHave);
      if (lWant != IGNORE_AMOUNT) SetWantCrypt (lCardRef, lWant);
      if (lSpare != IGNORE_AMOUNT) SetSpareCrypt (lCardRef, lSpare);
    }
  else
    {
      // Or display an error
      sName.Replace (wxT ("''"), wxT ("'"));
      wxLogError (wxT ("Couldn't find vampire %s"), sName.c_str ());
    }
}


void
InventoryModel::SetSpareCrypt (long lRef, long lAmount)
{
  Database *pDatabase = Database::Instance ();
  wxString sQuery;
  
  sQuery.Printf (wxT ("UPDATE inventory_crypt "
		      "  SET number_to_trade=%d "
		      "  WHERE card_ref=%d"),
		 lAmount,
		 lRef);
  pDatabase->Query (sQuery);

}


void
InventoryModel::SetWantCrypt (long lRef, long lAmount)
{
  Database *pDatabase = Database::Instance ();
  wxString sQuery;
  
  sQuery.Printf (wxT ("UPDATE inventory_crypt "
		      "  SET number_wanted=%d "
		      "  WHERE card_ref=%d"),
		 lAmount,
		 lRef);
  pDatabase->Query (sQuery);

}


void 
InventoryModel::SetHaveLibrary (long lRef, long lAmount)
{
  Database *pDatabase = Database::Instance ();
  wxString sQuery;
  
  sQuery.Printf (wxT ("UPDATE inventory_library "
		      "  SET number_owned=%d "
		      "  WHERE card_ref=%d"),
		 lAmount,
		 lRef);
  pDatabase->Query (sQuery);
}


void
InventoryModel::SetHWSLibraryName (wxString sName, wxString sSet, 
				   long lHave, long lWant, long lSpare)
{
  Database *pDatabase = Database::Instance ();
  wxString sQuery;
  long lCardRef = -1;

  sName.Replace (wxT ("'"), wxT ("''"));

  // Try to find a card
  sQuery.Printf (wxT ("SELECT card_ref FROM library_view WHERE dumbitdown(card_name) LIKE dumbitdown('%s') "), sName.c_str ());
  
  if (sSet.Length ())
    {
      sQuery << wxT ("AND set_name = '") << sSet << wxT ("' ");
    } 
  
  sQuery <<  wxT ("ORDER BY card_ref ASC LIMIT 1");
  
  RecordSet * pResult = pDatabase->Query (sQuery);
     
  if (pResult && pResult->GetCount () && 
      pResult->Item (0).GetCount ())
    {
      // Add whatever we've found
      pResult->Item (0).Item (0).ToLong (&lCardRef);
      if (lHave != IGNORE_AMOUNT) SetHaveLibrary (lCardRef, lHave);
      if (lWant != IGNORE_AMOUNT) SetWantLibrary (lCardRef, lWant);
      if (lSpare != IGNORE_AMOUNT) SetSpareLibrary (lCardRef, lSpare);
    }
  else
    {
      // Or display an error
      sName.Replace (wxT ("''"), wxT ("'"));
      wxLogError (wxT ("Couldn't find library card %s"), sName.c_str ());
    }
}


void
InventoryModel::SetSpareLibrary (long lRef, long lAmount)
{
  Database *pDatabase = Database::Instance ();
  wxString sQuery;
  
  sQuery.Printf (wxT ("UPDATE inventory_library "
		      "  SET number_to_trade=%d "
		      "  WHERE card_ref=%d"),
		 lAmount,
		 lRef);
  pDatabase->Query (sQuery);

}


void
InventoryModel::SetWantLibrary (long lRef, long lAmount)
{
  Database *pDatabase = Database::Instance ();
  wxString sQuery;
  
  sQuery.Printf (wxT ("UPDATE inventory_library "
		      "  SET number_wanted=%d "
		      "  WHERE card_ref=%d"),
		 lAmount,
		 lRef);
  pDatabase->Query (sQuery);

}

