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

#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/image.h>
#include <wx/panel.h>
#include <wx/dialog.h>

/** The main program frame
 * 
 *  This shows the main window for the program which includes the menu, a
 *  status bar and a panel in which to show the relevant image.
 */

class ImagePanel : public wxPanel 
{
public:
    ImagePanel(wxWindow *parent);
    ~ImagePanel();

    void SetImage(wxString fileName);
	void Clear();

protected:
    void OnPaint(wxPaintEvent &event);
	void OnSize(wxSizeEvent &event);
	void Click(wxMouseEvent &event);

private:
    wxImage image;
	wxString m_fileName;
	wxWindow *m_pParent;

protected:
    DECLARE_EVENT_TABLE()
};

#endif

