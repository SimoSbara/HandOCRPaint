
// HandOCRVeryfierDlg.h: file di intestazione
//

#pragma once
#include "PaintCtrl.h"
#include "OCR/OCR.h"


// Finestra di dialogo CHandOCRVeryfierDlg
class CHandOCRVeryfierDlg : public CDialogEx
{
// Costruzione
public:
	CHandOCRVeryfierDlg(CWnd* pParent = nullptr);	// costruttore standard
	~CHandOCRVeryfierDlg();
// Dati della finestra di dialogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HANDOCRVERYFIER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// supporto DDX/DDV



// Implementazione
protected:
	HICON m_hIcon;

	PaintCtrl* paintCtrl;

	CComboBox comboSize;

	CString SelectFile(CString title = _T(""));

	// Funzioni generate per la mappa dei messaggi
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDetect();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboSize();
};
