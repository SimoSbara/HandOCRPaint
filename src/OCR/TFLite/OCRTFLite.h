#pragma once

#include "../OCR.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/c/c_api.h"

#include <vector>

#pragma comment(lib, "tensorflowlite_c.lib")

class OCRTFLite : public OCR
{
public:
	OCRTFLite();
	~OCRTFLite();

	virtual bool LoadModel(CString netPath, CString labelPath);
	virtual bool Predict(BYTE* buffer, float** scores, int &best);

protected:
	virtual bool StartModel(CString netPath, CString labelPath);
	virtual bool StopModel();

protected:

	TfLiteModel* model;
	TfLiteInterpreter* interpreter;
	TfLiteTensor* inputTensor;

	std::vector<TfLiteTensor*> outputTensors;
};

