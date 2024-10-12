#include "pch.h"
#include "OCR.h"
#include <memory>

OCR::OCR()
{
	width = 0;
	height = 0;
	channels = 0;
	numOutputs = 0;

	loaded = false;
}

OCR::~OCR()
{

}

bool OCR::ReadLabels(CString path)
{
	FILE* fp = fopen(CStringA(path), "rb");

	char buffer[1000];
	CString label;

	labels.clear();

	if (fp)
	{
		while (fgets(buffer, 1000, fp))
		{
			label = CString(buffer);
			label.Remove(_T('\n'));

			labels.push_back(label);
		}

		fclose(fp);
	}
	else
		return false;

	return true;
}