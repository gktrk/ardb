#include <wx/string.h>

#ifndef _importxml_h_
#define _importxml_h_

char Unicode2Ascii(int code);
wxString ReadXmlFile(wxString filename, bool twoByte);

#endif
