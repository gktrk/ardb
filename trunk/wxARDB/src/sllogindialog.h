///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __noname__
#define __noname__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SLLoginDialog
///////////////////////////////////////////////////////////////////////////////
class SLLoginDialog : public wxDialog
{
private:


protected:
    wxStaticText* m_staticText2;
    wxTextCtrl* m_usernameEditbox;
    wxStaticText* m_staticText3;
    wxTextCtrl* m_PasswordEditBox;
    wxCheckBox* m_SaveDetailscheckBox;
    wxStdDialogButtonSizer* m_sdbSizer2;
    wxButton* m_sdbSizer2OK;
    wxButton* m_sdbSizer2Cancel;

public:
    SLLoginDialog(bool saveDetails, wxString &username, wxString &password);
    ~SLLoginDialog();

    wxString Username() {
        return m_usernameEditbox->GetValue();
    }

    wxString Password() {
        return m_PasswordEditBox->GetValue();
    }

    bool SaveDetails() {
        return m_SaveDetailscheckBox->IsChecked();
    }

    enum {
        ID_OK_BUTTON = wxID_HIGHEST + 1,
        ID_CANCEL_BUTTON
    };
};

#endif //__noname__
