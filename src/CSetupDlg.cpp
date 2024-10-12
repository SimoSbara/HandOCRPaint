// CSetupDlg.cpp: file di implementazione
//

#include "pch.h"
#include "afxdialogex.h"
#include "CSetupDlg.h"
#include "HandOCRVeryfier.h"
#include "HandOCRVeryfierDlg.h"
#include "json.hpp"
#include "OCR/ONNX/OCROnnx.h"
#include "OCR/TFLite/OCRTFLite.h"
#include <fstream>

using json = nlohmann::json;

extern CHandOCRVeryfierApp theApp;

// Finestra di dialogo CSetupDlg

IMPLEMENT_DYNAMIC(CSetupDlg, CDialog)

CSetupDlg::CSetupDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SETUP_DLG, pParent)
{
	minNorm = 0;
	maxNorm = 1;
	invertImg = 0;
}

CSetupDlg::~CSetupDlg()
{
}

void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NET, netPath);
	DDX_Text(pDX, IDC_EDIT_LABELS, labelsPath);
	DDX_Text(pDX, IDC_EDIT_NORM_MIN, minNorm);
	DDX_Text(pDX, IDC_EDIT_NORM_MAX, maxNorm);
	DDX_Check(pDX, IDC_CHECK_INVERT_IMG, invertImg);
	DDX_Check(pDX, IDC_CHECK_CHANNEL_FIRST, channelFirst);
	DDX_Control(pDX, IDC_COMBO_AI, aiEngineCombo);
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialog)
	ON_BN_CLICKED(IDC_BROWSE_LABELS, &CSetupDlg::OnBnClickedBrowseLabels)
	ON_BN_CLICKED(IDC_BROWSE_NET, &CSetupDlg::OnBnClickedBrowseNet)
	ON_EN_CHANGE(IDC_EDIT_NET, &CSetupDlg::OnEnChangeEditNet)
	ON_EN_CHANGE(IDC_EDIT_LABELS, &CSetupDlg::OnEnChangeEditLabels)
	ON_BN_CLICKED(IDOK, &CSetupDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSetupDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// Gestori di messaggi di CSetupDlg

BOOL CSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	aiEngineCombo.SetCurSel(ONNX);

	LoadParams();

	return TRUE;  // restituisce TRUE a meno che non venga impostato lo stato attivo su un controllo.
}

void CSetupDlg::LoadParams()
{
	std::ifstream i("save.json");

	if (i.is_open())
	{
		try
		{
			json load;
			int engine;

			i >> load;

			std::string net = load["netPath"];
			std::string labels = load["labelsPath"];
			netPath = CString(net.c_str());
			labelsPath = CString(labels.c_str());

			minNorm = load.value("minNorm", minNorm);
			maxNorm = load.value("maxNorm", maxNorm);
			invertImg = load.value("invertImg", invertImg);
			channelFirst = load.value("channelFirst", channelFirst);
			engine = load.value("engine", aiEngineCombo.GetCurSel());

			aiEngineCombo.SetCurSel(engine);
		}
		catch (std::exception e)
		{
			//MessageBoxA(this->m_hWnd, e.what(), "", MB_OK);
		}

		UpdateData(0);
		UpdateData(1);
	}
}

void CSetupDlg::SaveParams()
{
	std::ofstream saveFile;
	json save;

	save["netPath"] = CStringA(netPath).GetBuffer();
	save["labelsPath"] = CStringA(labelsPath).GetBuffer();
	save["minNorm"] = minNorm;
	save["maxNorm"] = maxNorm;
	save["invertImg"] = invertImg;
	save["channelFirst"] = channelFirst;
	save["engine"] = aiEngineCombo.GetCurSel();

	saveFile.open("save.json", std::ios::out);

	if (saveFile.is_open())
		saveFile << save;

	saveFile.close();
}
CString CSetupDlg::SelectFile(CString title)
{
	TCHAR curDir[MAX_PATH];
	CString path;
	CFileDialog dlg(TRUE);

	GetCurrentDirectory(MAX_PATH, curDir);

	dlg.m_ofn.lpstrTitle = title;
	dlg.m_ofn.lpstrInitialDir = curDir;

	if (dlg.DoModal())
	{
		path = dlg.GetPathName();
	}

	return path;
}

CString CSetupDlg::SelectFolder(CString title)
{
	TCHAR curDir[MAX_PATH];
	CString path;
	CFolderPickerDialog dlg;

	GetCurrentDirectory(MAX_PATH, curDir);

	dlg.m_ofn.lpstrTitle = title;
	dlg.m_ofn.lpstrInitialDir = curDir;

	if (dlg.DoModal())
	{
		path = dlg.GetPathName();
	}

	return path;
}

void CSetupDlg::OnBnClickedBrowseLabels()
{
	UpdateData(1);
	labelsPath = SelectFile(_T("Select Labels file .txt"));
	UpdateData(0);
}

void CSetupDlg::OnBnClickedBrowseNet()
{
	UpdateData(1);
	netPath = SelectFile(_T("Select ONNX Network file .onnx"));
	UpdateData(0);
}


void CSetupDlg::OnEnChangeEditNet()
{
	UpdateData(1);
	UpdateData(0);
}


void CSetupDlg::OnEnChangeEditLabels()
{
	UpdateData(1);
	UpdateData(0);
}


void CSetupDlg::OnBnClickedOk()
{
	UpdateData(1);

	if (minNorm > maxNorm)
	{
		double tmp = minNorm;
		minNorm = maxNorm;
		maxNorm = tmp;
	}
	else if (minNorm == maxNorm)
	{
		minNorm = 0;
		maxNorm = 1;
	}

	UpdateData(0);
	UpdateData(1);

	SaveParams();

	if (theApp.ocr)
	{
		delete theApp.ocr;
		theApp.ocr = NULL;
	}

	switch (aiEngineCombo.GetCurSel())
	{
	case ONNX: theApp.ocr = new OCROnnx; break;
	case TFLITE: theApp.ocr = new OCRTFLite; break;
	}

	if(!theApp.ocr)
		MessageBox(_T("Error Loading Net"), _T("Error"), MB_OK);

	theApp.ocr->LoadParams(minNorm, maxNorm, invertImg, channelFirst);

	if (!theApp.ocr->LoadModel(netPath, labelsPath))
	{
		MessageBox(_T("Error Loading Net"), _T("Error"), MB_OK);
		return;
	}

	ShowWindow(0);

	CHandOCRVeryfierDlg dlg;
	dlg.DoModal();

	ShowWindow(1);
}


void CSetupDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}
