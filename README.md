# Hand OCR Paint

## Description
Hand OCR Paint is meant to be useful for testing an OCR Neural Network for handwritten characters.

![](https://github.com/SimoSbara/HandOCRPaint/blob/main/example_ocr.gif)

## Testing
[Sample Networks](https://github.com/SimoSbara/HandOCRPaint/tree/main/nets) that I trained:
* [Alex Net](https://medium.com/@siddheshb008/alexnet-architecture-explained-b6240c528bd5);
* [Mobile Net V3](https://towardsdatascience.com/everything-you-need-to-know-about-mobilenetv3-and-its-comparison-with-previous-versions-a5d5e5a6eeaa);
* [KAN Net](https://github.com/Blealtan/efficient-kan) equivalent to MLP (trained on MNIST);
* [Convolutional KAN Net](https://github.com/SimoSbara/kan-lenet), LeNet but with KAN as Classifier (trained on MNIST).

## Parameters
If you are testing Sample Networks:
* Use [-1, 1] normal range and image inversion if you are using KAN Net or Convolutional KAN Net;
* Use [0, 1] normal range and no image inversion for the others;

Change these parameters according to your training data if you are testing your own network.

## Libraries used:
* [CxImage](https://www.codeproject.com/Articles/1300/CxImage) for image encoding/decoding;
* [ONNX](https://github.com/onnx/onnx) neural network engine with DirectML.

## Requirements for Devs
* Visual Studio 2022 with MFC

