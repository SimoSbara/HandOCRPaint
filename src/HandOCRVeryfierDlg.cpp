
// HandOCRVeryfierDlg.cpp: file di implementazione
//

#include "pch.h"
#include "../framework.h"
#include "HandOCRVeryfier.h"
#include "HandOCRVeryfierDlg.h"
#include "afxdialogex.h"

#include <chrono>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

extern CHandOCRVeryfierApp theApp;

// Finestra di dialogo CHandOCRVeryfierDlg

CHandOCRVeryfierDlg::CHandOCRVeryfierDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HANDOCRVERYFIER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	paintCtrl = NULL;
}

CHandOCRVeryfierDlg::~CHandOCRVeryfierDlg()
{
	if (paintCtrl)
		delete paintCtrl;
}

void CHandOCRVeryfierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_SIZE, comboSize);
}

BEGIN_MESSAGE_MAP(CHandOCRVeryfierDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CHandOCRVeryfierDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDDETECT, &CHandOCRVeryfierDlg::OnBnClickedDetect)
	ON_BN_CLICKED(IDCLEAR, &CHandOCRVeryfierDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_SIZE, &CHandOCRVeryfierDlg::OnCbnSelchangeComboSize)
END_MESSAGE_MAP()


// Gestori di messaggi di CHandOCRVeryfierDlg

BOOL CHandOCRVeryfierDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (int i = 0; i < 20; i++)
		comboSize.AddString(std::to_wstring(i + 1).c_str());

	comboSize.SetCurSel(2);

	paintCtrl = new PaintCtrl(GetDlgItem(IDC_PAINT_CTRL)->GetSafeHwnd());

	return TRUE;  // restituisce TRUE a meno che non venga impostato lo stato attivo su un controllo.
}

void CHandOCRVeryfierDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// Se si aggiunge alla finestra di dialogo un pulsante di riduzione a icona, per trascinare l'icona sarà necessario
//  il codice sottostante.  Per le applicazioni MFC che utilizzano il modello documento/visualizzazione,
//  questa operazione viene eseguita automaticamente dal framework.

void CHandOCRVeryfierDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // contesto di dispositivo per il disegno

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrare l'icona nel rettangolo client.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Disegnare l'icona
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Il sistema chiama questa funzione per ottenere la visualizzazione del cursore durante il trascinamento
//  della finestra ridotta a icona.
HCURSOR CHandOCRVeryfierDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


CString CHandOCRVeryfierDlg::SelectFile(CString title)
{
	TCHAR curDir[MAX_PATH];
	CString path;
	CFileDialog dlg(FALSE);

	GetCurrentDirectory(MAX_PATH, curDir);

	dlg.m_ofn.lpstrTitle = title;
	dlg.m_ofn.lpstrInitialDir = curDir;

	if (dlg.DoModal())
	{
		path = dlg.GetPathName();
	}

	return path;
}

void CHandOCRVeryfierDlg::OnBnClickedOk()
{
	CString fname = SelectFile(_T("Select save path"));

	paintCtrl->SaveOnFile(fname);
}


void CHandOCRVeryfierDlg::OnBnClickedDetect()
{
	if (!theApp.ocr)
		return;

	BYTE* img = NULL;
	paintCtrl->GetImage(&img, theApp.ocr->GetWidth(), theApp.ocr->GetHeight(), theApp.ocr->GetChannels());

	float* scores;
	int best = 0;
	bool success;

	auto start = Time::now();
	success = theApp.ocr->Predict(img, &scores, best);
	auto end = Time::now();
	
	fsec fs = end - start;
	ms d = std::chrono::duration_cast<ms>(fs);

	if (success)
	{
		CString text;
		float score = scores[best] * 100.0f;

		text.Format(_T("%s %.2f"), theApp.ocr->labels[best], score);
		text += _T("% (");
		text += std::to_wstring(d.count()).c_str();
		text += "ms)";

		SetWindowText(text);
	}

	delete img;
}


void CHandOCRVeryfierDlg::OnBnClickedCancel()
{
	paintCtrl->ClearImage();
}


void CHandOCRVeryfierDlg::OnCbnSelchangeComboSize()
{
	int pSize = comboSize.GetCurSel() + 1;
	paintCtrl->SetPaintSize(pSize, pSize);
}
