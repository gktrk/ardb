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

#ifndef _imagedialoh_h
#define _imagedialoh_h

#include <wx/wx.h>
#include <wx/spinctrl.h>

class ImageDialog : public wxDialog
{
public:
	ImageDialog();
	~ImageDialog() { };
	void SetImage(wxString fileName);

private:
	void OnPaint(wxPaintEvent &event);
	wxImage image;

	DECLARE_EVENT_TABLE()
};



#endif
