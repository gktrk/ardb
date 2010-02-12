/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
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

#ifndef _imagepanel_h
#define _imagepanel_h


#include "wx/wxprec.h"

#include <wx/confbase.h>
#include <wx/config.h>
#include <wx/dcmemory.h>

#include <wx/file.h>
#include <wx/fs_zip.h>
#include <wx/filesys.h>
#include <wx/wfstream.h>


#include "imagePanel.h"

#include "imagedialog.h"

// --------------------------------------------------------
// Constructors

/** \param parent - The parent window, and should be null for
 *         the main program's frame.
 *  \param image - an initial image to load into the panel
 */
ImagePanel::ImagePanel(wxWindow *parent) :
    wxPanel(parent)
{
    m_pParent = parent;
}

// --------------------------------------------------------
// Distructors

/// Deletes the stored image
ImagePanel::~ImagePanel()
{
    if (image.IsOk()) {
        image.Destroy();
    }
}

//mod to take a bitmap

/** Set a new active image to be shown in the panel
 *
 *  The old image if it exists is deleted.
 */
void ImagePanel::SetImage(wxImage &newImage)
{
    if (image.IsOk()) {
        image.Destroy();
    }

    image = newImage;
    Refresh();
}

/**
* Clears the image data
*
*/
void ImagePanel::Clear()
{
    m_fileName = wxT("");
    image.Destroy();
    Refresh();
}

void ImagePanel::OnSize(wxSizeEvent &event)
{
    if (!image.IsOk()) { // || event.GetEventObject() != imagePanel)
        event.Skip();
        return;
    }

    Refresh();
}

void ImagePanel::Click(wxMouseEvent &event)
{
    ImageDialog dialog;

    if (image.IsOk()) {
        dialog.SetImage(image);
        dialog.CentreOnParent();
        dialog.ShowModal();
    }
}

/// Draw the image in the panel if it exists
void ImagePanel::OnPaint(wxPaintEvent &event)
{
    if (!image.IsOk()) { // || event.GetEventObject() != imagePanel)
        event.Skip();
        return;
    }

    // imagePanel != NULL or we wouldn't get this event
    wxPaintDC dc(this);

    wxMemoryDC memDC;
    wxSize size = GetClientSize();

    int newWidth = size.GetWidth();
    int newHeight = size.GetHeight();

    int orgWidth = image.GetWidth();
    int orgHeight = image.GetHeight();

    int thumbWidth = 0;
    int thumbHeight = 0;

    if (orgWidth > orgHeight) {
        thumbWidth = newWidth;
        thumbHeight = (int) (orgHeight * (float)newHeight / (float)orgWidth);
    }

    if (orgWidth < orgHeight) {
        thumbWidth = (int) (orgWidth * (float)newWidth / (float)orgHeight);
        thumbHeight = newHeight;
    }

    if (orgWidth == orgHeight) {
        thumbWidth = newWidth;
        thumbHeight = newHeight;
    }

    int x;

    x = (newWidth - thumbWidth)/2;

    wxBitmap bitmap;
    bitmap = wxBitmap(image.Scale(thumbWidth,thumbHeight));

    if (bitmap.GetPalette()) {
        memDC.SetPalette(*bitmap.GetPalette());
        dc.SetPalette(*bitmap.GetPalette());
    }

    memDC.SelectObject(bitmap);

    dc.Blit(x, 1, bitmap.GetWidth(), bitmap.GetHeight(), &memDC, 0, 0, wxCOPY, FALSE);
    memDC.SelectObject(wxNullBitmap);
}

// --------------------------------------------------------
// Event table

BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
    // Paint event for the panel
    EVT_PAINT(ImagePanel::OnPaint)
    EVT_SIZE(ImagePanel::OnSize)
    EVT_LEFT_UP(ImagePanel::Click)
END_EVENT_TABLE()

#endif
