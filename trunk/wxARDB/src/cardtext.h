/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
 *
 *  Copyright (C) 2002 Francois Gombault
 *  gombault.francois@wanadoo.fr
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

#ifndef _cardtext_h
#define _cardtext_h

#include <wx/wx.h>

class CardText: public wxTextCtrl
{
 public:
  CardText (wxWindow* parent, wxWindowID id) :  wxTextCtrl (parent, id, wxT (""), wxDefaultPosition, wxDefaultSize, wxTE_RICH | wxTE_READONLY | wxTE_MULTILINE) { }
  ~CardText () { }
  
  void DisplayCryptText(long lCardRef, wxArrayString *cardImageNames);
  void DisplayLibraryText(long lCardRef, wxArrayString *cardImageNames);

 private:
	 wxString CardNameToFileName(wxString name);
};

#endif
