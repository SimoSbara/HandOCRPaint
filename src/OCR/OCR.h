#pragma once

#include <vector>

#ifdef _DEBUG
#pragma comment(lib, "cximagecrtdu.lib")
#else
#pragma comment(lib, "cximagecrtu.lib")
#endif

constexpr float grayScaleNorm = 1.0f / 255.0f;

enum AIEngines
{
	ONNX = 0,
	TFLITE,
	NUM_ENGINES
};

class OCR
{
public:
	OCR();
	virtual ~OCR() = 0;

	virtual bool LoadModel(CString netPath, CString labelPath) = 0;
	virtual bool Predict(BYTE* buffer, float** scores, int &best) = 0;

	void LoadParams(double min, double max, bool invert, bool channelFirst)
	{
		this->minNorm = min;
		this->maxNorm = max;
		this->invert = invert;
		this->channelFirst = channelFirst;
	}
	
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

	inline int GetEngine()
	{
		return this->engine;
	}

	std::vector<CString> labels;

protected:
	virtual bool StartModel(CString netPath, CString labelPath) = 0;
	virtual bool StopModel() = 0;
	bool ReadLabels(CString path);

protected:
	int width;
	int height;
	int channels;

	double minNorm;
	double maxNorm;
	bool invert;
	bool channelFirst;

	int numOutputs;

	bool loaded;
	bool flatten;

	int engine;
};

