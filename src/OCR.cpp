#include "pch.h"
#include "OCR.h"

OCR::OCR()
{
	width = 0;
	height = 0;
	channels = 0;
	numOutputs = 0;

	session = NULL;
	loaded = false;
}

OCR::~OCR()
{
	StopModel();
}

bool OCR::LoadModel(CString netPath, CString labelPath)
{
	if (netPath.IsEmpty() || labelPath.IsEmpty())
		return false;

	return StartModel(netPath, labelPath);
}

bool OCR::Predict(BYTE* buffer, float** scores, int& best)
{
	if (!loaded || !session)
		return false;

	int imageSize = width * height * channels;
	int inputNetSizeInBytes = imageSize * sizeof(float);

	float* inputNet = new float[imageSize];
	INT64 inputShape[4];

	inputShape[0] = 1;
	inputShape[1] = height;
	inputShape[2] = width;
	inputShape[3] = channels;

	for (int i = 0; i < imageSize; i++)
	{
		inputNet[i] = buffer[i] / 255.0; //lento
	}

	Ort::Value output_tensor{nullptr};
	Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

	Ort::Value input_tensor = Ort::Value::CreateTensor(memory_info, inputNet, inputNetSizeInBytes, inputShape, 4);

	char* inputNames = inputName.GetBuffer();
	char* outputNames = outputName.GetBuffer();

	session->Run(Ort::RunOptions{nullptr}, &inputNames, & input_tensor, 1, &outputNames, & output_tensor, 1);

	float* probs = output_tensor.GetTensorMutableData<float>();

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

	delete inputNet;

	return true;
}

bool OCR::StartModel(CString netPath, CString labelPath)
{
	if (session)
		StopModel();

	Ort::SessionOptions sessionOptions;

	//OrtStatus* onnxStatus = OrtSessionOptionsAppendExecutionProvider_DML(sessionOptions, 0);
	//
	//if (onnxStatus)
	//{
	//	const OrtApi* g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);

	//	CString msg(g_ort->GetErrorMessage(onnxStatus));

	//	MessageBox(NULL, msg, _T("Error DirectML"), MB_OK);
	//}

	sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

	session = new Ort::Session(Ort::Env(), netPath.GetBuffer(), sessionOptions);

	if (session)
	{
		size_t inputCount, outputCount;

		Ort::AllocatorWithDefaultOptions allocator;

		inputCount = session->GetInputCount();
		outputCount = session->GetOutputCount();

		if (inputCount != 1 && outputCount != 1)
			return false;

		inputName = CStringA(session->GetInputNameAllocated(0, allocator).get());
		auto inputShape = session->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();

		if (inputShape.size() != 4)
			return false;

		width = inputShape[2];
		height = inputShape[1];
		channels = inputShape[3];

		outputName = CStringA(session->GetOutputNameAllocated(0, allocator).get());
		auto outputShape = session->GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();

		int len = outputShape.size();

		if (len == 0)
			return false;

		numOutputs = outputShape[len - 1];

		if (!ReadLabels(labelPath))
		{
			StopModel();
			return false;
		}

		loaded = true;
	}

	return true;
}

bool OCR::StopModel()
{
	if (session)
		delete session;

	return true;
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