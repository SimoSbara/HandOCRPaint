#pragma once
#include "afxdialogex.h"


// Finestra di dialogo CSetupDlg

class CSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetupDlg)

public:
	CSetupDlg(CWnd* pParent = nullptr);   // costruttore standard
	virtual ~CSetupDlg();

// Dati della finestra di dialogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETUP_DLG };
#endif


protected:

	CString netPath, labelsPath;

	double minNorm, maxNorm;
	int invertImg;
	int channelFirst;

	CComboBox aiEngineCombo;

	CString SelectFolder(CString title = _T(""));
	CString SelectFile(CString title = _T(""));

	void LoadParams();
	void SaveParams();

	virtual void DoDataExchange(CDataExchange* pDX);    // supporto DDX/DDV
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowseLabels();
	afx_msg void OnBnClickedBrowseNet();
	afx_msg void OnEnChangeEditNet();
	afx_msg void OnEnChangeEditLabels();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
