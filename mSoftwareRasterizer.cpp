#include "stdafx.h"

CubeVertex cubeVert[8] = 
{
	{ Vertex4(-1,  1, -1, 1), 0, 0, 0xffff0000 },
	{ Vertex4( 1,  1, -1, 1), 1, 0, 0xff00ff00 },
	{ Vertex4(-1, -1, -1, 1), 0, 1, 0xff0000ff },
	{ Vertex4( 1, -1, -1, 1), 1, 1, 0xfff0f000 },
	{ Vertex4(-1,  1,  1, 1), 0, 0, 0xff00f0f0 },
	{ Vertex4( 1,  1,  1, 1), 1, 0, 0xfff000f0 },
	{ Vertex4(-1, -1,  1, 1), 0, 1, 0xfff0f0f0 },
	{ Vertex4( 1, -1,  1, 1), 1, 1, 0xff111111 }
};

#define TEX_WIDTH	512
#define TEX_HEIGHT	512

mSoftRasterizer::mSoftRasterizer(HWND hWnd, int width, int height)
	: m_hWnd(hWnd), m_BackBuffer(NULL)
{
	m_BackSize.x = width;
	m_BackSize.y = height;

	HDC hMainDc = GetDC(hWnd);
	m_hBackDc = CreateCompatibleDC(hMainDc);
	ReleaseDC(hWnd, hMainDc);

	// 创建BackBuffer
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), m_BackSize.x, -m_BackSize.y, 1, 32, BI_RGB, 
		0, 0, 0, 0, 0 }  };
	m_hBackBmp = CreateDIBSection(m_hBackDc, &bi, DIB_RGB_COLORS, (void **)&m_BackBuffer, NULL, 0);
	SelectObject(m_hBackDc, m_hBackBmp);

	m_PreVertexList.clear();
	m_PreVertexList.push_back(cubeVert[0]);
	m_PreVertexList.push_back(cubeVert[1]);
	m_PreVertexList.push_back(cubeVert[2]);
	m_PreVertexList.push_back(cubeVert[3]);
	m_PreVertexList.push_back(cubeVert[4]);
	m_PreVertexList.push_back(cubeVert[5]);
	m_PreVertexList.push_back(cubeVert[6]);
	m_PreVertexList.push_back(cubeVert[7]);

	// 初始化全局纹理
	m_MainTexture = new unsigned int[TEX_WIDTH*TEX_HEIGHT];
	for(int row=0; row<64; ++row)
	{
		for(int col=0; col<64; ++col)
		{
			for(int i=0; i<8; ++i)
			{
				if(col%2)
				{
					memset(m_MainTexture + (i + 8*row)*TEX_WIDTH + col*8, 0xff, sizeof(DWORD)*8);
				}
				else
				{
					memset(m_MainTexture + (i + 8*row)*TEX_WIDTH + col*8, 0x90, sizeof(DWORD)*8);
				}
			}
		}
	}
}

mSoftRasterizer::~mSoftRasterizer()
{
	SAFE_DELARR(m_MainTexture);
	DeleteObject(m_hBackBmp);
	DeleteDC(m_hBackDc);
	m_BackBuffer = NULL;
}

void mSoftRasterizer::Init()
{
	mMatrix4x4::MatIdentify(m_World);
	m_Camera.Set(Vertex3(0, 0, -5.0f), Vertex3(0, 0, 5.0f), Vertex3(0, 1.0f, 0));
	mMatrix4x4::MatPerspective(m_Proj, 90.0f, (float)m_BackSize.x/m_BackSize.y, 1.0f, 2000.0f);
}

void mSoftRasterizer::Update()
{
	static float rot = 0.0f;

	mMatrix4x4 rotate;
	mMatrix4x4::MatIdentify(rotate);
	rot += 0.05f;
	mMatrix4x4::MatRotateX(rotate, DEG_TO_RAD*rot);
	m_World = rotate;
	m_Transform = m_World*m_Camera.GetView()*m_Proj;

	static std::vector<CubeVertex> resVertexList;
	resVertexList.clear();
	resVertexList.resize(36);

	m_IndexList.clear();
	m_IndexList.resize(36);
	m_IndexList[0] = 0;
	m_IndexList[1] = 2;
	m_IndexList[2] = 1;
	m_IndexList[3] = 2;
	m_IndexList[4] = 3;
	m_IndexList[5] = 1;

	m_IndexList[6] = 1;
	m_IndexList[7] = 3;
	m_IndexList[8] = 5;
	m_IndexList[9] = 3;
	m_IndexList[10] = 7;
	m_IndexList[11] = 5;

	m_IndexList[12] = 4;
	m_IndexList[13] = 5;
	m_IndexList[14] = 6;
	m_IndexList[15] = 6;
	m_IndexList[16] = 5;
	m_IndexList[17] = 7;

	m_IndexList[18] = 0;
	m_IndexList[19] = 4;
	m_IndexList[20] = 2;
	m_IndexList[21] = 2;
	m_IndexList[22] = 4;
	m_IndexList[23] = 6;

	m_IndexList[24] = 4;
	m_IndexList[25] = 0;
	m_IndexList[26] = 5;
	m_IndexList[27] = 0;
	m_IndexList[28] = 1;
	m_IndexList[29] = 5;

	m_IndexList[30] = 6;
	m_IndexList[31] = 7;
	m_IndexList[32] = 2;
	m_IndexList[33] = 2;
	m_IndexList[34] = 7;
	m_IndexList[35] = 3;


	// 顶点变换
	for(size_t i=0; i<m_IndexList.size(); ++i)
	{
		resVertexList[i].pos = mMatrix4x4::MatMultiply(m_PreVertexList[m_IndexList[i]].pos, m_Transform);

		float rhw = 1/resVertexList[i].pos.w;
		resVertexList[i].pos.x = (resVertexList[i].pos.x*rhw + 1.0f) * m_BackSize.x * 0.5f;
		resVertexList[i].pos.y = (-resVertexList[i].pos.y*rhw + 1.0f) * m_BackSize.y * 0.5f;
		resVertexList[i].pos.z = resVertexList[i].pos.z*rhw;

		resVertexList[i].color = m_PreVertexList[m_IndexList[i]].color;

		resVertexList[i].u = m_PreVertexList[m_IndexList[i]].u;
		resVertexList[i].v = m_PreVertexList[m_IndexList[i]].v;
	}

	for(size_t i=0; i<resVertexList.size(); i+=3)
	{
		DrawTriangle2DV2(resVertexList[i], resVertexList[i+1], resVertexList[i+2], resVertexList[i].color);
	}
}

void mSoftRasterizer::Present()
{
	HDC hdc = GetDC(m_hWnd);
	BitBlt(hdc, 0, 0, m_BackSize.x, m_BackSize.y, m_hBackDc, 0, 0, SRCCOPY);
	ReleaseDC(m_hWnd, hdc);
}

void mSoftRasterizer::MoveCam(float x, float y, float z)
{
	static Vertex3 v;

	v.x = x;
	v.y = y;
	v.z = z;

	m_Camera.Move(v);
}

void mSoftRasterizer::DrawPoint(int x, int y, int Size, DWORD color)
{
	if(x >= m_BackSize.x || x < 0 ||
		y >= m_BackSize.y || y < 0)
	{
		return;
	}

	// 着色中心点
	*(m_BackBuffer + y*m_BackSize.x + x) = color;
}

// Bresenham's line algorithm
void mSoftRasterizer::DrawLine2D(int x1, int y1, int x2, int y2, DWORD color) 
{
	// 参数 c 为颜色值
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
 
	// 如果直线与 x 轴的夹角大于 45 度
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

			// 若越界了，则跳过该点
			DrawPoint(cy, cx, 1, color);
			cx += ix;
		}
	} 
	// 如果直线与 x 轴的夹角小于 45 度
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

			// 若越界了，则跳过该点
			DrawPoint(cx, cy, 1, color);
			cx += ix;
		}
	}
}

void mSoftRasterizer::DrawTriangle2D(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3, DWORD color)
{
	if(v1.pos.x >= m_BackSize.x || v1.pos.x < 0 || v1.pos.y >= m_BackSize.y || v1.pos.y < 0 || 
		v2.pos.x >= m_BackSize.x || v2.pos.x < 0 || v2.pos.y >= m_BackSize.y || v2.pos.y < 0 || 
		v3.pos.x >= m_BackSize.x || v3.pos.x < 0 || v3.pos.y >= m_BackSize.y || v3.pos.y < 0)
	{
		return;
	}

	DWORD *ptr = (DWORD *)m_BackBuffer;
	
    // 28.4 fixed-point coordinates
	const int Y1 = (int)(16.0f * v1.pos.y);
    const int Y2 = (int)(16.0f * v2.pos.y);
    const int Y3 = (int)(16.0f * v3.pos.y);

    const int X1 = (int)(16.0f * v1.pos.x);
    const int X2 = (int)(16.0f * v2.pos.x);
    const int X3 = (int)(16.0f * v3.pos.x);

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

void mSoftRasterizer::DrawTriangle2DV2(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3, DWORD color)
{
	if(v1.pos.x >= m_BackSize.x || v1.pos.x < 0 || v1.pos.y >= m_BackSize.y || v1.pos.y < 0 || 
		v2.pos.x >= m_BackSize.x || v2.pos.x < 0 || v2.pos.y >= m_BackSize.y || v2.pos.y < 0 || 
		v3.pos.x >= m_BackSize.x || v3.pos.x < 0 || v3.pos.y >= m_BackSize.y || v3.pos.y < 0)
	{
		return;
	}

	DWORD *ptr = (DWORD *)m_BackBuffer;
	
    // 28.4 fixed-point coordinates
    const int Y1 = (int)(16.0f * v1.pos.y);
    const int Y2 = (int)(16.0f * v2.pos.y);
    const int Y3 = (int)(16.0f * v3.pos.y);

    const int X1 = (int)(16.0f * v1.pos.x);
    const int X2 = (int)(16.0f * v2.pos.x);
    const int X3 = (int)(16.0f * v3.pos.x);

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
						float c = ((v1.pos.y-v2.pos.y)*(ix)+(v2.pos.x-v1.pos.x)*(y+iy)+v1.pos.x*v2.pos.y-v2.pos.x*v1.pos.y)/
								((v1.pos.y-v2.pos.y)*v3.pos.x+(v2.pos.x-v1.pos.x)*v3.pos.y+v1.pos.x*v2.pos.y-v2.pos.x*v1.pos.y);
						float b = ((v1.pos.y-v3.pos.y)*(ix)+(v3.pos.x-v1.pos.x)*(y+iy)+v1.pos.x*v3.pos.y-v3.pos.x*v1.pos.y)/
								((v1.pos.y-v3.pos.y)*v2.pos.x+(v3.pos.x-v1.pos.x)*v2.pos.y+v1.pos.x*v3.pos.y-v3.pos.x*v1.pos.y);
						float a = 1-b-c;

						float u,v;
						u = a*v1.u + b*v2.u + c*v3.u;
						v = a*v1.v + b*v2.v + c*v3.v;
						buffer[ix] = m_MainTexture[(int)(u*TEX_WIDTH + v*TEX_HEIGHT*TEX_WIDTH)];
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
                           	float c = ((v1.pos.y-v2.pos.y)*(ix)+(v2.pos.x-v1.pos.x)*(iy)+v1.pos.x*v2.pos.y-v2.pos.x*v1.pos.y)/
									((v1.pos.y-v2.pos.y)*v3.pos.x+(v2.pos.x-v1.pos.x)*v3.pos.y+v1.pos.x*v2.pos.y-v2.pos.x*v1.pos.y);
							float b = ((v1.pos.y-v3.pos.y)*(ix)+(v3.pos.x-v1.pos.x)*(iy)+v1.pos.x*v3.pos.y-v3.pos.x*v1.pos.y)/
									((v1.pos.y-v3.pos.y)*v2.pos.x+(v3.pos.x-v1.pos.x)*v2.pos.y+v1.pos.x*v3.pos.y-v3.pos.x*v1.pos.y);
							float a = 1-b-c;

							float u,v;
							u = a*v1.u + b*v2.u + c*v3.u;
							v = a*v1.v + b*v2.v + c*v3.v;
							buffer[ix] = m_MainTexture[(int)(u*TEX_WIDTH + v*TEX_HEIGHT*TEX_WIDTH)];
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

void mSoftRasterizer::ReadSkullModel()
{
	std::ifstream skullFile("skull.txt");

	int VertexCount;
	skullFile>>VertexCount;

	skullFile.close();
}

void mSoftRasterizer::ClearQuick()
{
	memset(m_BackBuffer, 0, sizeof(DWORD)*m_BackSize.x*m_BackSize.y);
}

