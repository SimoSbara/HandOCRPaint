
// HandOCRVeryfier.h: file di intestazione principale per l'applicazione PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "includere 'pch.h' prima di includere questo file per PCH"
#endif

#include "../resource.h"		// simboli principali
#include "OCR/OCR.h"

// CHandOCRVeryfierApp:
// Per l'implementazione di questa classe, vedere HandOCRVeryfier.cpp
//

class CHandOCRVeryfierApp : public CWinApp
{
public:
	CHandOCRVeryfierApp();

	OCR* ocr;

// Override
public:
	virtual BOOL InitInstance();

// Implementazione

	DECLARE_MESSAGE_MAP()
};

extern CHandOCRVeryfierApp theApp;
