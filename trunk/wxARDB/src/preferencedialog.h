/*  Anarch Revolt Deck Builder - a VTES inventory manager / deck builder
*
*  Copyright (C) 2010 Graham Smith
*  graham.r.smith@gmail.com
*
*  contributors:
*
*  Official project page: http://code.google.com/p/ardb/
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef __prefdialog__
#define __prefdialog__

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class prefDialog
///////////////////////////////////////////////////////////////////////////////
class PrefDialog : public wxDialog
{
private:

protected:
#if 0 //Not supported in v3.0.0    	
    wxCheckBox* m_cbDownloadImages;
#endif
    wxCheckBox* m_cbDownloadCards;

    wxStdDialogButtonSizer* m_sdbSizer2;
    wxButton* m_pbtnOK;
    wxButton* m_sdbSizer2Cancel;

    void SaveSettings( wxCommandEvent& event );

public:
    PrefDialog();
    ~PrefDialog();



};

#endif //__prefdialog__
