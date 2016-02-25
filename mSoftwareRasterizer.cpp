#include "stdafx.h"

mSoftRasterizer::mSoftRasterizer(HWND hWnd, int width, int height)
	: m_hWnd(hWnd), m_BackBuffer(NULL)
{
	m_BackSize.x = width;
	m_BackSize.y = height;

	HDC hMainDc = GetDC(hWnd);
	m_hBackDc = CreateCompatibleDC(hMainDc);
	ReleaseDC(hWnd, hMainDc);

	// ����BackBuffer
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), m_BackSize.x, -m_BackSize.y, 1, 32, BI_RGB, 
		0, 0, 0, 0, 0 }  };
	m_hBackBmp = CreateDIBSection(m_hBackDc, &bi, DIB_RGB_COLORS, (void **)&m_BackBuffer, NULL, 0);
	SelectObject(m_hBackDc, m_hBackBmp);
}

mSoftRasterizer::~mSoftRasterizer()
{
	DeleteObject(m_hBackBmp);
	DeleteDC(m_hBackDc);
	m_BackBuffer = NULL;
}

void mSoftRasterizer::Init()
{
}

void mSoftRasterizer::Present()
{
	HDC hdc = GetDC(m_hWnd);
	BitBlt(hdc, 0, 0, m_BackSize.x, m_BackSize.y, m_hBackDc, 0, 0, SRCCOPY);
	ReleaseDC(m_hWnd, hdc);
}

void mSoftRasterizer::DrawPoint(int x, int y, int Size, DWORD color)
{
	if(x >= m_BackSize.x || x < 0 ||
		y >= m_BackSize.y || y < 0)
	{
		return;
	}

	// ��ɫ���ĵ�
	*(m_BackBuffer + y*m_BackSize.x + x) = color;
}

// Bresenham's line algorithm
void mSoftRasterizer::DrawLine(int x1, int y1, int x2, int y2, DWORD color) 
{
	// ���� c Ϊ��ɫֵ
	int dx = abs(x2 - x1),
		dy = abs(y2 - y1),
		yy = 0;
 
	if (dx < dy) {
		yy = 1;
		Swap<int>(x1, y1);
		Swap<int>(x2, y2);
		Swap<int>(dx, dy);
	}
 
	int ix = (x2 - x1) > 0 ? 1 : -1,
		iy = (y2 - y1) > 0 ? 1 : -1,
		cx = x1, cy = y1, n2dy = dy * 2,
		n2dydx = (dy - dx) * 2, d = dy * 2 - dx;
 
	// ���ֱ���� x ��ļнǴ��� 45 ��
	if (yy) 
	{ 
		while (cx != x2) 
		{
			if (d < 0) 
			{
				d += n2dy;
			} 
			else 
			{
				cy += iy;
				d += n2dydx;
			}

			// ��Խ���ˣ��������õ�
			DrawPoint(cy, cx, 1, color);
			cx += ix;
		}
	} 
	// ���ֱ���� x ��ļн�С�� 45 ��
	else 
	{ 
		while (cx != x2) 
		{
			if (d < 0) 
			{
				d += n2dy;
			}
			else 
			{
				cy += iy;
				d += n2dydx;
			}

			// ��Խ���ˣ��������õ�
			DrawPoint(cx, cy, 1, color);
			cx += ix;
		}
	}
}

void mSoftRasterizer::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, DWORD color)
{
	if(x1 >= m_BackSize.x || x1 < 0 || y1 >= m_BackSize.y || y1 < 0 || 
		x2 >= m_BackSize.x || x2 < 0 || y2 >= m_BackSize.y || y2 < 0 || 
		x3 >= m_BackSize.x || x3 < 0 || y3 >= m_BackSize.y || y3 < 0)
	{
		return;
	}

	DWORD *ptr = (DWORD *)m_BackBuffer;
	
    // 28.4 fixed-point coordinates
    const int Y1 = (int)(16.0f * y1);
    const int Y2 = (int)(16.0f * y2);
    const int Y3 = (int)(16.0f * y3);

    const int X1 = (int)(16.0f * x1);
    const int X2 = (int)(16.0f * x2);
    const int X3 = (int)(16.0f * x3);

    // Deltas
    const int DX12 = X1 - X2;
    const int DX23 = X2 - X3;
    const int DX31 = X3 - X1;

    const int DY12 = Y1 - Y2;
    const int DY23 = Y2 - Y3;
    const int DY31 = Y3 - Y1;

    // Fixed-point deltas
    const int FDX12 = DX12 << 4;
    const int FDX23 = DX23 << 4;
    const int FDX31 = DX31 << 4;

    const int FDY12 = DY12 << 4;
    const int FDY23 = DY23 << 4;
    const int FDY31 = DY31 << 4;

    // Bounding rectangle
    int minx = (min(X1, min(X2, X3)) + 0xF) >> 4;
    int maxx = (max(X1, max(X2, X3)) + 0xF) >> 4;
    int miny = (min(Y1, min(Y2, Y3)) + 0xF) >> 4;
    int maxy = (max(Y1, max(Y2, Y3)) + 0xF) >> 4;

    ptr += miny*m_BackSize.x;

    // Half-edge constants
    int C1 = DY12 * X1 - DX12 * Y1;
    int C2 = DY23 * X2 - DX23 * Y2;
    int C3 = DY31 * X3 - DX31 * Y3;

    // Correct for fill convention
    if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
    if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
    if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

    int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
    int CY2 = C2 + DX23 * (miny << 4) - DY23 * (minx << 4);
    int CY3 = C3 + DX31 * (miny << 4) - DY31 * (minx << 4);

    for(int y = miny; y < maxy; y++)
    {
        int CX1 = CY1;
        int CX2 = CY2;
        int CX3 = CY3;

        for(int x = minx; x < maxx; x++)
        {
            if(CX1 > 0 && CX2 > 0 && CX3 > 0)
            {
                ptr[x] = color;
            }

            CX1 -= FDY12;
            CX2 -= FDY23;
            CX3 -= FDY31;
        }

        CY1 += FDX12;
        CY2 += FDX23;
        CY3 += FDX31;

        ptr += m_BackSize.x;
    }
}

void mSoftRasterizer::DrawTriangleV2(int x1, int y1, int x2, int y2, int x3, int y3, DWORD color)
{
	if(x1 >= m_BackSize.x || x1 < 0 || y1 >= m_BackSize.y || y1 < 0 || 
		x2 >= m_BackSize.x || x2 < 0 || y2 >= m_BackSize.y || y2 < 0 || 
		x3 >= m_BackSize.x || x3 < 0 || y3 >= m_BackSize.y || y3 < 0)
	{
		return;
	}

	DWORD *ptr = (DWORD *)m_BackBuffer;
	
    // 28.4 fixed-point coordinates
    const int Y1 = (int)(16.0f * y1);
    const int Y2 = (int)(16.0f * y2);
    const int Y3 = (int)(16.0f * y3);

    const int X1 = (int)(16.0f * x1);
    const int X2 = (int)(16.0f * x2);
    const int X3 = (int)(16.0f * x3);

    // Deltas
    const int DX12 = X1 - X2;
    const int DX23 = X2 - X3;
    const int DX31 = X3 - X1;

    const int DY12 = Y1 - Y2;
    const int DY23 = Y2 - Y3;
    const int DY31 = Y3 - Y1;

    // Fixed-point deltas
    const int FDX12 = DX12 << 4;
    const int FDX23 = DX23 << 4;
    const int FDX31 = DX31 << 4;

    const int FDY12 = DY12 << 4;
    const int FDY23 = DY23 << 4;
    const int FDY31 = DY31 << 4;

    // Bounding rectangle
    int minx = (min(X1, min(X2, X3)) + 0xF) >> 4;
    int maxx = (max(X1, max(X2, X3)) + 0xF) >> 4;
    int miny = (min(Y1, min(Y2, Y3)) + 0xF) >> 4;
    int maxy = (max(Y1, max(Y2, Y3)) + 0xF) >> 4;

    // Block size, standard 8x8 (must be power of two)
    const int q = 8;

    // Start in corner of 8x8 block
    minx &= ~(q - 1);
    miny &= ~(q - 1);

    ptr += miny*m_BackSize.x;

    // Half-edge constants
    int C1 = DY12 * X1 - DX12 * Y1;
    int C2 = DY23 * X2 - DX23 * Y2;
    int C3 = DY31 * X3 - DX31 * Y3;

    // Correct for fill convention
    if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
    if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
    if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;

    // Loop through blocks
    for(int y = miny; y < maxy; y += q)
    {
        for(int x = minx; x < maxx; x += q)
        {
            // Corners of block
            int x0 = x << 4;
            int x1 = (x + q - 1) << 4;
            int y0 = y << 4;
            int y1 = (y + q - 1) << 4;

            // Evaluate half-space functions
            bool a00 = C1 + DX12 * y0 - DY12 * x0 > 0;
            bool a10 = C1 + DX12 * y0 - DY12 * x1 > 0;
            bool a01 = C1 + DX12 * y1 - DY12 * x0 > 0;
            bool a11 = C1 + DX12 * y1 - DY12 * x1 > 0;
            int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);

            bool b00 = C2 + DX23 * y0 - DY23 * x0 > 0;
            bool b10 = C2 + DX23 * y0 - DY23 * x1 > 0;
            bool b01 = C2 + DX23 * y1 - DY23 * x0 > 0;
            bool b11 = C2 + DX23 * y1 - DY23 * x1 > 0;
            int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);

            bool c00 = C3 + DX31 * y0 - DY31 * x0 > 0;
            bool c10 = C3 + DX31 * y0 - DY31 * x1 > 0;
            bool c01 = C3 + DX31 * y1 - DY31 * x0 > 0;
            bool c11 = C3 + DX31 * y1 - DY31 * x1 > 0;
            int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

            // Skip block when outside an edge
            if(a == 0x0 || b == 0x0 || c == 0x0) continue;

            DWORD *buffer = ptr;

            // Accept whole block when totally covered
            if(a == 0xF && b == 0xF && c == 0xF)
            {
                for(int iy = 0; iy < q; iy++)
                {
                    for(int ix = x; ix < x + q; ix++)
                    {
                        buffer[ix] = 0x00007F00; // Green
                    }

                   buffer += m_BackSize.x;
                }
            }
            else // Partially covered block
            {
                int CY1 = C1 + DX12 * y0 - DY12 * x0;
                int CY2 = C2 + DX23 * y0 - DY23 * x0;
                int CY3 = C3 + DX31 * y0 - DY31 * x0;

                for(int iy = y; iy < y + q; iy++)
                {
                    int CX1 = CY1;
                    int CX2 = CY2;
                    int CX3 = CY3;

                    for(int ix = x; ix < x + q; ix++)
                    {
                        if(CX1 > 0 && CX2 > 0 && CX3 > 0)
                        {
                            buffer[ix] = 0x0000007F; // Blue
                        }

                        CX1 -= FDY12;
                        CX2 -= FDY23;
                        CX3 -= FDY31;
                    }

                    CY1 += FDX12;
                    CY2 += FDX23;
                    CY3 += FDX31;

                    buffer += m_BackSize.x;
                }
            }
        }

       ptr += q * m_BackSize.x;
    }
}