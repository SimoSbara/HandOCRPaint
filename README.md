# Hand OCR Paint

## Description
Hand OCR Paint is meant to be useful for testing an OCR Neural Network for handwritten characters.

## Testing
[Sample Networks](https://github.com/SimoSbara/HandOCRPaint/tree/main/nets) that I trained:
* [Alex Net](https://medium.com/@siddheshb008/alexnet-architecture-explained-b6240c528bd5);
* [Mobile Net V3](https://towardsdatascience.com/everything-you-need-to-know-about-mobilenetv3-and-its-comparison-with-previous-versions-a5d5e5a6eeaa);
* [Custom OCR KAN Net](https://github.com/Blealtan/efficient-kan) that is equivalent to MLP (trained on MNIST).

![](https://github.com/SimoSbara/HandOCRPaint/blob/main/example_ocr.gif)

## Libraries used:
* [CxImage](https://www.codeproject.com/Articles/1300/CxImage) for image encoding/decoding;
* [ONNX](https://github.com/onnx/onnx) neural network engine with DirectML.

## Requirements for Devs
* Visual Studio 2022 with MFC

