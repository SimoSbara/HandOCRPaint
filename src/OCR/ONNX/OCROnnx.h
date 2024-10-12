#pragma once

#include "../OCR.h"

#include "onnxruntime_cxx_api.h"
#include "dml_provider_factory.h"
#include "cpu_provider_factory.h"

#include <vector>

#pragma comment(lib, "onnxruntime.lib")

class OCROnnx : public OCR
{
public:
	OCROnnx();
	~OCROnnx();

	virtual bool LoadModel(CString netPath, CString labelPath);
	virtual bool Predict(BYTE* buffer, float** scores, int &best);

protected:
	virtual bool StartModel(CString netPath, CString labelPath);
	virtual bool StopModel();

protected:

	CStringA inputName;
	CStringA outputName;

	const OrtDmlApi* dmlApi;
	
	Ort::Session* session;
};

