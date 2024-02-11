// CSetupDlg.cpp: file di implementazione
//

#include "pch.h"
#include "afxdialogex.h"
#include "CSetupDlg.h"
#include "HandOCRVeryfier.h"
#include "HandOCRVeryfierDlg.h"

extern CHandOCRVeryfierApp theApp;

// Finestra di dialogo CSetupDlg

IMPLEMENT_DYNAMIC(CSetupDlg, CDialog)

CSetupDlg::CSetupDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SETUP_DLG, pParent)
{

}

CSetupDlg::~CSetupDlg()
{
}

void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NET, netPath);
	DDX_Text(pDX, IDC_EDIT_LABELS, labelsPath);
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



	return TRUE;  // restituisce TRUE a meno che non venga impostato lo stato attivo su un controllo.
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
	theApp.ocr.LoadModel(netPath, labelsPath);

	CHandOCRVeryfierDlg dlg;
	dlg.DoModal();

	CDialog::OnOK();
}


void CSetupDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}
