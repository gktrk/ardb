/**********************************************************************
*  File: DownloadFile.h
*
*  Purpose: Class for downloading a file from a given URL and send back
*  event notification to the owner, when the download fails or complete.
*
*  Authors: Priyank Bolia <priyank.bolia@gmail.com>
*
*  Created: 24 December, 2005
*
*  Modified: 01 May, 2007
*
*  Copyright (C) 2005-2007 by Priyank Bolia. All rights reserved.
*
*  Licence: wxWindows licence
*
*  http://www.priyank.co.in
*********************************************************************/

#pragma once
#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/url.h>
#include <wx/file.h>
#include "DownloadEvent.h"

class wxDownloadFile : public wxThread
{
private:
    wxWindow *m_pParent;
    wxString m_strURL;
    wxArrayString m_strFiles;  //List of file names to download
    bool m_bIsDownload;
    wxInt64 m_nFileSize;
    bool m_bNotifyDownloading;
    wxInt64 m_nNotifyBytes;
    wxInt32 m_nCurrentFile;
    wxString m_strDstDir;

public:
    /**
     * Constructor
     * \param *pParent parent window pointer
     * \param strURL URL from where to download the file
     * \param strFiles list of files to download from strURL
     * \param bNotify Enable or disable the downloading in progress notification.
     * \param nBytes After how many number of bytes downloaded, to send the notification.
     * \return
     */
    wxDownloadFile(wxWindow *pParent, wxString strURL,
                   wxArrayString &strFiles,
                   wxString strDstDir,
                   bool bNotify = false, wxInt64 nBytes = 1000);
    /**
     * Virtual destructor
     * \param void
     * \return
     */
    virtual ~wxDownloadFile(void);

public:
    /// thread execution starts here
    virtual void *Entry();

    /// called when the thread exits - whether it terminates normally or is
    /// stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

public:
    /**
     * Function to cancel the download, by the user/program.
     * \param void
     */
    void CancelDownload(void);

    /**
     * Returns the size of the downloading file; may be wrong also, it depends on the server.
     * \param void
     * \return Size of the file downloading.
     */
    wxInt64 GetFileSize(void);

    /**
     * Sends an DOWNLOAD_INPROGRESS event after the specified number of bytes downloaded.
     * \param bool Enable or disable the notification.
     * \param wxInt64 After how many bytes downloaded, the notification should be sent.
     */
    void SetDownloadingNotification(bool bEnable = true, wxInt64 nBytes = 1000);
};
