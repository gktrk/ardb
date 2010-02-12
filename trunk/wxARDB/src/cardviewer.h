///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 20 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CARDVIEWER__
#define __CARDVIEWER__

#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/spinbutt.h>
#include <wx/event.h>

#include "imagePanel.h"

#define CARD_IMAGE_DIR wxT("cardimages")

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CardViewer
///////////////////////////////////////////////////////////////////////////////
class CardViewer : public wxPanel
{
private:

    void ChangeImage(wxSpinEvent& WXUNUSED (event));
    void DisplayImage(int i);

    enum {
        ID_SPIN_BUTTON = wxID_HIGHEST + 1,
    };

protected:
    ImagePanel* m_imagePanel;

    wxSpinButton* m_nextPrevImage;

    wxArrayString cardImages;

    DECLARE_EVENT_TABLE()


public:
    CardViewer( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 227,281 ), long style = wxTAB_TRAVERSAL );
    ~CardViewer();

    void SetImage(wxArrayString *cardNames);
    void Clear();

};

#endif //__noname__
