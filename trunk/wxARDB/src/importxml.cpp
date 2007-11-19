
#include "importxml.h"
#include <wx/file.h>

#define BUFFER_SIZE 256

char MakeAscii(char c);

wxString ReadXmlFile(wxString filename)
{
	size_t bytesRead = 0;
	char buffer[BUFFER_SIZE];
	wxFile file(filename);
	wxString xmlString;

	if (file.IsOpened())
	{
		while(!file.Eof())
		{
			bytesRead = file.Read(buffer,BUFFER_SIZE);

			if (bytesRead != wxInvalidOffset)
			{
				for(size_t i=0;i<bytesRead;i++)
				{
					xmlString += MakeAscii(buffer[i]);
				}
			}
		}
	}

	file.Close();

	return xmlString;
}

char MakeAscii(char c)
{
	char result = c;

	switch (c)
	{
	case 'à':
	case 'á':
	case 'â':
	case 'ã':
	case 'ä':
	case 'å':
		result = 'a';
		break;

	case 'ç':
		result = 'c';
		break;

	case 'é':
	case 'è':
	case 'ê':
	case 'ë':
		result = 'e';
		break;

	case 'ì':
	case 'í':
	case 'î':
	case 'ï':
		result = 'i';
		break;

	case 'ñ':
		result = 'n';
		break;

	case 'ò':
	case 'ó':
	case 'ô':
	case 'õ':
	case 'ö':
		result = 'o';
		break;

	case 'ù':
	case 'ú':
	case 'û':
	case 'ü':
		result = 'u';
		break;

	case 'ý':
	case 'ÿ':
		result = 'y';
		break;

	default:
		break;
	}

	return result;
}