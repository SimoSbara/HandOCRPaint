#include "pch.h"
#include "PaintCtrl.h"
#include "ximage.h"

PaintCtrl::PaintCtrl(HWND pParent, int width, int height, int wPaint, int hPaint)
{
	if (width < 0)
		width = 1;

	if (height < 0)
		height = 1;	
	
	if (wPaint < 0)
		wPaint = 1;

	if (hPaint < 0)
		hPaint = 1;

	this->width = width;
	this->height = height;
	this->wPaint = wPaint;
	this->hPaint = hPaint;

	if (pParent)
		SubclassWindow(pParent);

	GetClientRect(&rectClient);

	drawLUT = new bool* [width];

	for (int x = 0; x < width; x++)
	{
		drawLUT[x] = new bool[height];

		for (int y = 0; y < height; y++)
			drawLUT[x][y] = 0;
	}

	coeffX = (double)width / (double)rectClient.Width();
	coeffY = (double)height / (double)rectClient.Height();

	CDC* pDC = this->GetDC();

	shadowMem.CreateCompatibleDC(pDC);
	shadowBit.CreateCompatibleBitmap(pDC, rectClient.Width(), rectClient.Height());

	curBuffer = new BYTE[rectClient.Width() * rectClient.Height() * 4];

	UpdateDisplay();

	isPressing = false;
}

PaintCtrl::~PaintCtrl()
{
	//UnsubclassWindow();

	for (int x = 0; x < width; x++)
	{
		delete drawLUT[x];
	}

	delete drawLUT;

	if (curBuffer)
		delete curBuffer;
}

void PaintCtrl::SaveOnFile(CString fname)
{
	CImage img;

	img.Attach((HBITMAP)shadowBit.m_hObject);

	img.Save(fname);
}

void PaintCtrl::GetImage(BYTE** buffer, int w, int h, int c)
{	
	BYTE* realBuf = new BYTE[width * height * c];
	BYTE* finalBuf = new BYTE[w * h * c];
	BYTE* ptr = realBuf;
	
	CxImage image;

	drawing.Lock();

	if (c == 3)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				*ptr++ = !drawLUT[x][y] * 255;
				*ptr++ = !drawLUT[x][y] * 255;
				*ptr++ = !drawLUT[x][y] * 255;
			}
		}
	}
	else if(c == 1)
	{
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				*ptr++ = !drawLUT[x][y] * 255;
	}

	image.CreateFromArray(realBuf, width, height, c * 8, width * c, false);
	image.Resample(w, h);

	memcpy(finalBuf, image.GetBits(), w * h * c);

	delete realBuf;

	drawing.Unlock();

	*buffer = finalBuf;
}

void PaintCtrl::ClearImage()
{
	drawing.Lock();

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			drawLUT[x][y] = 0;

	drawing.Unlock();
}

BEGIN_MESSAGE_MAP(PaintCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void PaintCtrl::OnPaint()
{
	UpdateDisplay();

	//CWnd::OnPaint();
}

void PaintCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	isPressing = false;

	CWnd::OnLButtonUp(nFlags, point);
}

void PaintCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	isPressing = true;

	CWnd::OnLButtonDown(nFlags, point);
}

void PaintCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (
		(point.x < 0 || point.y < 0) ||
		(point.x >= rectClient.right || point.y >= rectClient.bottom)
		)
	{
		isPressing = false;
		return;
	}

	if (!isPressing)
		return;

	int x = coeffX * point.x;
	int y = coeffY * point.y;

	for (int i = max(0, x - wPaint / 2); i <= min(width - 1, x + wPaint / 2); i++)
		for (int j = max(0, y - hPaint / 2); j <= min(height - 1, y + hPaint / 2); j++)
			drawLUT[i][j] = 1;

	//CWnd::OnMouseMove(nFlags, point);

	UpdateDisplay();
}

void PaintCtrl::UpdateDisplay()
{
	drawing.Lock();

	CPaintDC dc(this);

	CBrush brush(RGB(255, 255, 255));
	POINT point;
	CDC* pDC = &dc;
	BYTE* ptr;

	pOldBmp = (CBitmap*)shadowMem.SelectObject(&shadowBit);

	shadowMem.FillRect(&rectClient, &brush);

	shadowBit.GetBitmapBits(rectClient.Width() * rectClient.Height() * 4, curBuffer);

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
		{
			if (drawLUT[x][y])
			{
				point.x = x / coeffX;
				point.y = y / coeffY;

				//shadowMem.FillSolidRect(point.x - widthPaint / 2, point.y - heightPaint / 2, widthPaint, heightPaint, RGB(0, 0, 0));
				//shadowMem.SetPixel(point, RGB(0, 0, 0));

				ptr = &curBuffer[point.y * rectClient.Width() * 4 + point.x * 4];

				*ptr++ = 0;
				*ptr++ = 0;
				*ptr++ = 0;
				*ptr++ = 0;
			}
		}

	shadowBit.SetBitmapBits(rectClient.Width() * rectClient.Height() * 4, curBuffer);

	pDC->BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &shadowMem, 0, 0, SRCCOPY);
	shadowMem.SelectObject(pOldBmp);

	Invalidate(FALSE);

	drawing.Unlock();
}