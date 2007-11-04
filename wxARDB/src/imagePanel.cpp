#include "wx/wxprec.h"

#include <wx/confbase.h>
#include <wx/config.h>
#include <wx/dcmemory.h>

#include "config.h"
#include "imagePanel.h"

// --------------------------------------------------------
// Constructors

/** \param parent - The parent window, and should be null for
 *         the main program's frame.
 *  \param image - an initial image to load into the panel
 */
ImagePanel::ImagePanel(wxWindow *parent) :
    wxPanel(parent)
{
}

// --------------------------------------------------------
// Distructors

/// Deletes the stored image
ImagePanel::~ImagePanel()
{
    if (image.IsOk())
	{
		image.Destroy();
	}
}

//mod to take a bitmap

/** Set a new active image to be shown in the panel
 *
 *  The old image if it exists is deleted.
 */
void ImagePanel::SetImage(wxString fileName)
{
	if (image.IsOk())
	{
		image.Destroy();
	}

	image.LoadFile(fileName);
    Refresh();
}

/**
* Clears the image data
*
*/
void ImagePanel::Clear()
{
	image.Destroy();
	Refresh();
}

void ImagePanel::OnSize(wxSizeEvent &event)
{
	if (!image.IsOk()) // || event.GetEventObject() != imagePanel)
	{
		event.Skip();
		return;
	}

	Refresh();
}

/// Draw the image in the panel if it exists
void ImagePanel::OnPaint(wxPaintEvent &event)
{
  if (!image.IsOk()) // || event.GetEventObject() != imagePanel)
  {
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

	int thumbWidth;
	int thumbHeight;

	if (orgWidth > orgHeight) 
	{
		thumbWidth = newWidth;
		thumbHeight = orgHeight * ((float)newHeight / (float)orgWidth);
	}

	if (orgWidth < orgHeight) 
	{
		thumbWidth = orgWidth * ((float)newWidth / (float)orgHeight);
		thumbHeight = newHeight;
	}

	if (orgWidth == orgHeight) 
	{
		thumbWidth = newWidth;
		thumbHeight = newHeight;
	}

	int x,y;

	x = (newWidth - thumbWidth)/2;

	wxBitmap bitmap;
	bitmap = wxBitmap(image.Scale(thumbWidth,thumbHeight));

    if (bitmap.GetPalette())
    {
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
END_EVENT_TABLE()
