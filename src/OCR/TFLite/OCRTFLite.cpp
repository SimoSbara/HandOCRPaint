#include "pch.h"
#include "OCRTFLite.h"
#include <memory>
#include <string>

OCRTFLite::OCRTFLite()
{
	width = 0;
	height = 0;
	channels = 0;
	numOutputs = 0;

	model = NULL;
	interpreter = NULL;
	inputTensor = NULL;

	loaded = false;

	engine = TFLITE;
}

OCRTFLite::~OCRTFLite()
{
	StopModel();
}

bool OCRTFLite::LoadModel(CString netPath, CString labelPath)
{
	if (netPath.IsEmpty() || labelPath.IsEmpty())
		return false;

	return StartModel(netPath, labelPath);
}

bool OCRTFLite::Predict(BYTE* buffer, float** scores, int& best)
{
	if (!loaded || outputTensors.size() < 1)
		return false;

	int imageSize = width * height * channels;
	int inputNetSizeInBytes = imageSize * sizeof(float);

	std::unique_ptr<float> inpNet(new float[imageSize]);
	float* inputNet = inpNet.get();

	double range = maxNorm - minNorm;

	if (invert)
		for (int i = 0; i < imageSize; i++)
			inputNet[i] = 255.0f - buffer[i];
	else
		for (int i = 0; i < imageSize; i++)
			inputNet[i] = buffer[i];

	for (int i = 0; i < imageSize; i++)
		inputNet[i] = (inputNet[i] * grayScaleNorm) * range + minNorm;

	float* dst = inputTensor->data.f;
	memcpy(dst, inputNet, inputNetSizeInBytes);

	if (TfLiteInterpreterInvoke(interpreter) != kTfLiteOk)
		return false;

	float* probs = outputTensors[0]->data.f;

	int bestInd = 0;
	float bestScore = probs[0];

	for (int i = 1; i < numOutputs; i++)
	{
		if (bestScore < probs[i])
		{
			bestInd = i;
			bestScore = probs[i];
		}
	}

	best = bestInd;
	*scores = probs;

	OutputDebugStringA("outputs\n");
	OutputDebugStringA((std::to_string(probs[0]) + "\n").c_str());

	return true;
}

bool OCRTFLite::StartModel(CString netPath, CString labelPath)
{
	if (!ReadLabels(labelPath))
		return false;

	model = TfLiteModelCreateFromFile(CStringA(netPath).GetBuffer());
	if (model == nullptr)
	{
		MessageBoxA(NULL, "Error Loading", "Error OCR", MB_OK);
		return false;
	}

	TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
	TfLiteInterpreterOptionsSetNumThreads(options, 1);

	interpreter = TfLiteInterpreterCreate(model, options);
	if (interpreter == nullptr)
	{
		return false;
	}

	if (TfLiteInterpreterAllocateTensors(interpreter) != kTfLiteOk)
	{
		return false;
	}

	inputTensor = TfLiteInterpreterGetInputTensor(interpreter, 0);
	if (inputTensor->type != kTfLiteFloat32)
	{
		return false;
	}

	if (TfLiteInterpreterGetInputTensorCount(interpreter) != 1)
	{
		return false;
	}

	if (TfLiteInterpreterGetOutputTensorCount(interpreter) != 1)
	{
		return false;
	}

	if (flatten)
	{
		int size = sqrt(inputTensor->dims->data[1]); //sperando sia così
		width = size;
		height = size;
		channels = 1;
	}
	else
	{
		if (channelFirst)
		{
			width = inputTensor->dims->data[3];
			height = inputTensor->dims->data[2];
			channels = inputTensor->dims->data[1];
		}
		else
		{
			width = inputTensor->dims->data[2];
			height = inputTensor->dims->data[1];
			channels = inputTensor->dims->data[3];
		}
	}

	numOutputs = labels.size();

	outputTensors.push_back((TfLiteTensor*)TfLiteInterpreterGetOutputTensor(interpreter, 0));

	loaded = true;

	return true;
}

bool OCRTFLite::StopModel()
{
	if (model != NULL)
		TfLiteModelDelete(model);

	model = NULL;
	outputTensors.clear();

	return true;
}