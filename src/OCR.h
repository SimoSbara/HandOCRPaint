#pragma once

#include "onnxruntime_cxx_api.h"
#include "dml_provider_factory.h"
#include "cpu_provider_factory.h"

#include <vector>

#pragma comment(lib, "onnxruntime.lib")

#ifdef _DEBUG
#pragma comment(lib, "cximagecrtdu.lib")
#else
#pragma comment(lib, "cximagecrtu.lib")
#endif

class OCR
{
public:
	OCR();
	~OCR();

	bool LoadModel(CString netPath, CString labelPath);
	bool Predict(BYTE* buffer, float** scores, int &best);
	
	inline int GetWidth()
	{
		return this->width;
	}

	inline int GetHeight()
	{
		return this->height;
	}

	inline int GetChannels()
	{
		return this->channels;
	}

	std::vector<CString> labels;

private:
	bool StartModel(CString netPath, CString labelPath);
	bool StopModel();
	bool ReadLabels(CString path);

private:
	int width;
	int height;
	int channels;

	int numOutputs;

	bool loaded;

	CStringA inputName;
	CStringA outputName;
	
	Ort::Session* session;
};

