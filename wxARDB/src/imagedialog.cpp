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

#include "imagedialog.h"


ImageDialog::ImageDialog() : wxDialog (0, -1, wxT ("Card Image"), wxDefaultPosition, wxDefaultSize)
{
}

void ImageDialog::SetImage(wxImage &newImage)
{
     image = newImage;

     int width = image.GetWidth();
     int height = image.GetHeight();

     SetSize(width,height+10);  //+10 is a fudge factor to take into account dialog title on Windows
     //This will need to be changed for different platforms.
}

void ImageDialog::OnPaint(wxPaintEvent &event)
{
    if (!image.IsOk()) { // || event.GetEventObject() != imagePanel)
        event.Skip();
        return;
    }

    // imagePanel != NULL or we wouldn't get this event
    wxPaintDC dc(this);

    wxMemoryDC memDC;
    wxSize size = GetClientSize();

    wxBitmap bitmap;
    bitmap = wxBitmap(image);

    if (bitmap.GetPalette()) {
        memDC.SetPalette(*bitmap.GetPalette());
        dc.SetPalette(*bitmap.GetPalette());
    }

    memDC.SelectObject(bitmap);

    dc.Blit(0, 0, bitmap.GetWidth(), bitmap.GetHeight(), &memDC, 0, 0, wxCOPY, FALSE);
    memDC.SelectObject(wxNullBitmap);
}

// --------------------------------------------------------
// Event table

BEGIN_EVENT_TABLE(ImageDialog, wxDialog)
    // Paint event for the panel
    EVT_PAINT(ImageDialog::OnPaint)
END_EVENT_TABLE()
