
#include "importxml.h"
#include <wx/file.h>

#define BUFFER_SIZE 1024


wxString ReadXmlFile(wxString filename, bool twoByte)
{
	size_t bytesRead = 0;
	unsigned char buffer[BUFFER_SIZE];
	wxFile file(filename);
	wxString xmlString;
	unsigned char utf8[2];
	int utf8index = 0;

	if (file.IsOpened())
	{
		while(!file.Eof())
		{
			bytesRead = file.Read(buffer,BUFFER_SIZE);

			if (bytesRead != wxInvalidOffset)
			{
				for(size_t i=0;i<bytesRead;i++)
				{
					if (buffer[i] >= 128) // 2 byte unicode char
					{
						if (twoByte)
						{
							utf8[utf8index++] = buffer[i];

							if (utf8index == 2)
							{
								//Got both utf8 bytes
								//calc unicode value
								int unicode = (utf8[0]-192)*64 + (utf8[1]-128);
								utf8index = 0;

								//lookup unicode
								xmlString += Unicode2Ascii(unicode);
							}
						}
						else
						{
							xmlString += Unicode2Ascii(buffer[i]);
						}
					}
					else
					{					
						//unsigned char conC = MakeAscii(buffer[i]);
						xmlString += buffer[i];
					}
				}
			}
		}
	}

	file.Close();

	return xmlString;
}

char Unicode2Ascii(int code)
{
	char result = '\0';

	switch (code)
	{
	case 225:
	case 224:
	case 226:
	case 228:
	case 227:
	case 229:
		result = 'a';
		break;

	case 231:
		result = 'c';
		break;

	case 233:
	case 232:
	case 234:
	case 235:
		result = 'e';
		break;

	case 237:
	case 236:
	case 238:
	case 239:

		result = 'i';
		break;

	case 241:
		result = 'n';
		break;

	case 243:
	case 242:
	case 244:
	case 246:
	case 245:
		result = 'o';
		break;

	case 250:
	case 249:
	case 251:
	case 252:	
		result = 'u';
		break;

	//case 'ý':
	case 255:
		result = 'y';
		break;

	default:
		break;
	}

	return result;
}
