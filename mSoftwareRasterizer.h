#include "stdafx.h"

#ifndef _MSOFT_RER_
#define _MSOFT_RER_

class mSoftRasterizer
{
public:
	mSoftRasterizer(HWND hWnd, int width, int height);
	~mSoftRasterizer();

	void Init();
	void Present();
	void DrawPoint(int x, int y, int Size, DWORD color);
	void DrawLine(int x1, int y1, int x2, int y2, DWORD color);
	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, DWORD color);
	void DrawTriangleV2(int x1, int y1, int x2, int y2, int x3, int y3, DWORD color);

private:
	HWND m_hWnd;
	DWORD *m_BackBuffer;
	HDC m_hBackDc;
	HBITMAP m_hBackBmp;
	POINT m_BackSize;
};

#endif // _MSOFT_RER_
