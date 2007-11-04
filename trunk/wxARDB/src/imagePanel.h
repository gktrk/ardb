#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/image.h>
#include <wx/panel.h>

/** The main program frame
 * 
 *  This shows the main window for the program which includes the menu, a
 *  status bar and a panel in which to show the relevant image.
 */

class ImagePanel : public wxPanel 
{
public:
    /** \name Constructors and Destructor
     *@{
     */
    ImagePanel(wxWindow *parent); ///< Default constructor

    ~ImagePanel(); ///< Destructor

    //@}

    void SetImage(wxString fileName); ///< Set the current active image
	void Clear(); ///< Set the current active image

protected:
    void OnPaint(wxPaintEvent &event); ///< Handle paint events for the panel
	void OnSize(wxSizeEvent &event);

private:
    wxImage image;

protected:
    DECLARE_EVENT_TABLE()
};

#endif

