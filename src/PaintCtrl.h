#pragma once
#include <afxwin.h>
#include <vector>

class PaintCtrl : public CWnd
{
public:
	PaintCtrl(HWND pParent = NULL, int width = 320, int height = 240, int wPaint = 3, int hPaint = 3);
	~PaintCtrl();

	void SaveOnFile(CString fname);
	void GetImage(BYTE** buffer, int w, int h, int c);
	void ClearImage();

	void SetPaintSize(int w, int h)
	{
		wPaint = w;
		hPaint = h;
	}

private:
	CRect rectClient;
	CSemaphore drawing;

	bool isPressing;

	bool** drawLUT;

	int width;
	int height;

	double coeffX;
	double coeffY;

	int wPaint;
	int hPaint;

	CDC shadowMem;
	CBitmap shadowBit, *pOldBmp;

	BYTE* curBuffer;

protected:
	PaintCtrl() {}
	void UpdateDisplay();

	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

