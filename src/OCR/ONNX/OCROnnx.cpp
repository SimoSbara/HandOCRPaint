#include "pch.h"
#include "OCROnnx.h"
#include <memory>

OCROnnx::OCROnnx()
{
	width = 0;
	height = 0;
	channels = 0;
	numOutputs = 0;

	session = NULL;
	loaded = false;

	Ort::GetApi().GetExecutionProviderApi("DML", ORT_API_VERSION, reinterpret_cast<const void**>(&dmlApi));

	engine = ONNX;
}

OCROnnx::~OCROnnx()
{
	StopModel();
}

bool OCROnnx::LoadModel(CString netPath, CString labelPath)
{
	if (netPath.IsEmpty() || labelPath.IsEmpty())
		return false;

	return StartModel(netPath, labelPath);
}

bool OCROnnx::Predict(BYTE* buffer, float** scores, int& best)
{
	if (!loaded || !session)
		return false;

	int imageSize = width * height * channels;
	int inputNetSizeInBytes = imageSize * sizeof(float);

	std::unique_ptr<float> inpNet(new float[imageSize]);
	float* inputNet = inpNet.get();

	double range = maxNorm - minNorm;

	int inputS = (flatten) ? 2 : 4;

	std::vector<INT64> inputShape(inputS);

	if (flatten)
	{
		inputShape[0] = 1;
		inputShape[1] = width * height;
	}
	else
	{	
		if (channelFirst)
		{
			inputShape[0] = 1;
			inputShape[1] = channels;
			inputShape[2] = height;
			inputShape[3] = width;
		}
		else
		{
			inputShape[0] = 1;
			inputShape[1] = height;
			inputShape[2] = width;
			inputShape[3] = channels;
		}
	}

	if (invert)
		for (int i = 0; i < imageSize; i++)
			inputNet[i] = 255.0f - buffer[i];
	else
		for (int i = 0; i < imageSize; i++)
			inputNet[i] = buffer[i];

	for (int i = 0; i < imageSize; i++)
		inputNet[i] = (inputNet[i] * grayScaleNorm) * range + minNorm;

	Ort::Value output_tensor{nullptr};
	Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

	Ort::Value input_tensor = Ort::Value::CreateTensor(memory_info, inputNet, inputNetSizeInBytes, inputShape.data(), (flatten) ? 2 : 4);

	char* inputNames = inputName.GetBuffer();
	char* outputNames = outputName.GetBuffer();

	session->Run(Ort::RunOptions{nullptr}, &inputNames, & input_tensor, 1, &outputNames, & output_tensor, 1);

	float* probs = output_tensor.GetTensorMutableData<float>();

	int bestInd = 0;
	float bestScore = probs[0];

	OutputDebugStringA("outputs\n");
	OutputDebugStringA((std::to_string(probs[0]) + "\n").c_str());

	for (int i = 1; i < numOutputs; i++)
	{
		if (bestScore < probs[i])
		{
			bestInd = i;
			bestScore = probs[i];
		}

		OutputDebugStringA((std::to_string(probs[i]) + "\n").c_str());
	}

	best = bestInd;
	*scores = probs;

	return true;
}

bool OCROnnx::StartModel(CString netPath, CString labelPath)
{
	if (session)
		StopModel();

	try
	{

		Ort::SessionOptions sessionOptions;

		//OrtStatus* onnxStatus = OrtSessionOptionsAppendExecutionProvider_DML(sessionOptions, 0);
		//
		//if (onnxStatus)
		//{
		//	const OrtApi* g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);

		//	CString msg(g_ort->GetErrorMessage(onnxStatus));

		//	MessageBox(NULL, msg, _T("Error DirectML"), MB_OK);
		//}

		sessionOptions.DisableMemPattern();
		sessionOptions.SetExecutionMode(ExecutionMode::ORT_SEQUENTIAL);

		OrtStatus* status = dmlApi->SessionOptionsAppendExecutionProvider_DML(sessionOptions, 0);

		if (status)
			return false;



		//sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

		session = new Ort::Session(Ort::Env(), netPath.GetBuffer(), sessionOptions);

		if (session)
		{
			size_t inputCount, outputCount;

			Ort::AllocatorWithDefaultOptions allocator;

			flatten = false;

			inputCount = session->GetInputCount();
			outputCount = session->GetOutputCount();

			if (inputCount != 1 && outputCount != 1)
				return false;

			inputName = CStringA(session->GetInputNameAllocated(0, allocator).get());
			auto inputShape = session->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();

			if (inputShape.size() == 2)
				flatten = true;
			else if (inputShape.size() != 4)
				return false;

			if (flatten)
			{
				int size = sqrt(inputShape[1]); //sperando sia così
				width = size;
				height = size;
				channels = 1;
			}
			else
			{
				if (channelFirst)
				{
					width = inputShape[3];
					height = inputShape[2];
					channels = inputShape[1];
				}
				else
				{
					width = inputShape[2];
					height = inputShape[1];
					channels = inputShape[3];
				}
			}

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
	}
	catch (Ort::Exception ex)
	{
		MessageBoxA(NULL, ex.what(), "Error OCR", MB_OK);
		return false;
	}

	return true;
}

bool OCROnnx::StopModel()
{
	if (session)
		delete session;

	return true;
}