/**********************************************************************
*  File: DownloadEvent.cpp
*
*  Purpose: Implementation of the download notification event for the
*  wxDownloadFile class.
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

#include "DownloadEvent.h"


IMPLEMENT_DYNAMIC_CLASS(wxDownloadEvent, wxNotifyEvent)

wxDownloadEvent::wxDownloadEvent(wxEventType commandType, int id)
: wxNotifyEvent(commandType, id)
, m_downloadStatus(DOWNLOAD_NONE)
, m_nFileSize(0)
, m_nBytesDownloaded(0)
{
}

wxDownloadEvent::wxDownloadEvent(const wxDownloadEvent& event)
: wxNotifyEvent(event)
{
	m_downloadStatus =event.m_downloadStatus;
	m_nFileSize =event.m_nFileSize;
	m_nBytesDownloaded =event.m_nBytesDownloaded;
	m_strURL =event.m_strURL;
	m_strFILE =event.m_strFILE;
}

wxDownloadEvent::~wxDownloadEvent(void)
{
}

int wxDownloadEvent::GetDownLoadStatus(void)
{
	return (int)m_downloadStatus;
}

void wxDownloadEvent::SetDownLoadStatus(int status)
{
	m_downloadStatus = (DownloadSTATUS)status;
}

wxString wxDownloadEvent::GetDownLoadURL(void)
{
	return m_strURL;
}

void wxDownloadEvent::SetDownLoadURL(wxString strURL)
{
	m_strURL = strURL;
}

wxString wxDownloadEvent::GetDownLoadedFile(void)
{
	return m_strFILE;
}

void wxDownloadEvent::SetDownLoadedFile(wxString strFILE)
{
	m_strFILE = strFILE;
}

wxInt64 wxDownloadEvent::GetDownLoadedBytesCount(void)
{
	return m_nBytesDownloaded;
}

void wxDownloadEvent::SetDownLoadedBytesCount(wxInt64 nBytes)
{
	m_nBytesDownloaded = nBytes;
}

wxInt64 wxDownloadEvent::GetFileSize(void)
{
	return m_nFileSize;
}

void wxDownloadEvent::SetFileSize(wxInt64 nSize)
{
	m_nFileSize = nSize;
}
