#include "stdafx.h"

CubeVertex cubeVert[8] = 
{
	{ Vertex4(-1,  1, -1, 1), 0, 0, 0x00ff0000 },
	{ Vertex4( 1,  1, -1, 1), 1, 0, 0x00ff0000 },
	{ Vertex4(-1, -1, -1, 1), 0, 1, 0x0000ff00 },
	{ Vertex4( 1, -1, -1, 1), 1, 1, 0x0000ff00 },
	{ Vertex4(-1,  1,  1, 1), 0, 0, 0x000000ff },
	{ Vertex4( 1,  1,  1, 1), 1, 0, 0x000000ff },
	{ Vertex4(-1, -1,  1, 1), 0, 1, 0x0000f0f0 },
	{ Vertex4( 1, -1,  1, 1), 1, 1, 0x0000f0f0 }
};

#define TEX_WIDTH	2048
#define TEX_HEIGHT	2048

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
	// 棋盘的格子数（正方形棋盘，行列的格子数一致）
	int blockNum = 16;
	// 单个格子的尺寸
	int blockWidth = TEX_WIDTH/blockNum;
	for(int row=0; row<blockNum; ++row)
	{
		for(int col=0; col<blockNum; ++col)
		{
			for(int i=0; i<blockWidth; ++i)
			{
				bool isOddRow = row%2, isOddCol = col%2;
				char oddColor, evenColor;
				if(isOddRow)
				{
					oddColor = (char)0xff;
					evenColor = (char)0x50;
				}
				else
				{
					oddColor = (char)0x50;
					evenColor = (char)0xff;
				}

				if(col%2)
				{
					memset(m_MainTexture + (i + blockWidth*row)*TEX_WIDTH + col*blockWidth, oddColor, sizeof(DWORD)*blockWidth);
				}
				else
				{
					memset(m_MainTexture + (i + blockWidth*row)*TEX_WIDTH + col*blockWidth, evenColor, sizeof(DWORD)*blockWidth);
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
	rot += 0.3f;
	mMatrix4x4::MatRotateX(rotate, DEG_TO_RAD*rot);
	//m_World = rotate;
	m_Transform = m_World*m_Camera.GetView()*m_Proj;

	static std::vector<CubeVertex> resVertexList;
	resVertexList.clear();

	m_IndexList.clear();
	m_IndexList.resize(36);
	m_IndexList[0] = 0;
	m_IndexList[1] = 2;
	m_IndexList[2] = 1;
	m_IndexList[3] = 3;
	m_IndexList[4] = 1;
	m_IndexList[5] = 2;

	m_IndexList[6] = 1;
	m_IndexList[7] = 3;
	m_IndexList[8] = 5;
	m_IndexList[9] = 7;
	m_IndexList[10] = 5;
	m_IndexList[11] = 3;

	m_IndexList[12] = 4;
	m_IndexList[13] = 5;
	m_IndexList[14] = 6;
	m_IndexList[15] = 7;
	m_IndexList[16] = 6;
	m_IndexList[17] = 5;

	m_IndexList[18] = 0;
	m_IndexList[19] = 4;
	m_IndexList[20] = 2;
	m_IndexList[21] = 6;
	m_IndexList[22] = 2;
	m_IndexList[23] = 4;

	m_IndexList[24] = 4;
	m_IndexList[25] = 0;
	m_IndexList[26] = 5;
	m_IndexList[27] = 1;
	m_IndexList[28] = 5;
	m_IndexList[29] = 0;

	m_IndexList[30] = 6;
	m_IndexList[31] = 7;
	m_IndexList[32] = 2;
	m_IndexList[33] = 3;
	m_IndexList[34] = 2;
	m_IndexList[35] = 7;


	CubeVertex tempVert;
	// 顶点变换
	for(size_t i=0; i<m_IndexList.size(); i+=6)
	{
		// 非背面，不裁剪
		if(!_BackFaceCull(m_PreVertexList[m_IndexList[i]], m_PreVertexList[m_IndexList[i+1]], m_PreVertexList[m_IndexList[i+2]]))
		{
			tempVert.pos = mMatrix4x4::MatMultiply(m_PreVertexList[m_IndexList[i]].pos, m_Transform);
			float rhw = 1/tempVert.pos.w;
			tempVert.pos.x = (tempVert.pos.x*rhw + 1.0f) * m_BackSize.x * 0.5f;
			tempVert.pos.y = (-tempVert.pos.y*rhw + 1.0f) * m_BackSize.y * 0.5f;
			tempVert.pos.z = tempVert.pos.z*rhw;
			tempVert.color = 0xffffffff / (i+1);
			tempVert.u = 0.0f;
			tempVert.v = 0.0f;

			resVertexList.push_back(tempVert);

			// 第二个顶点
			tempVert.pos = mMatrix4x4::MatMultiply(m_PreVertexList[m_IndexList[i+1]].pos, m_Transform);
			rhw = 1/tempVert.pos.w;
			tempVert.pos.x = (tempVert.pos.x*rhw + 1.0f) * m_BackSize.x * 0.5f;
			tempVert.pos.y = (-tempVert.pos.y*rhw + 1.0f) * m_BackSize.y * 0.5f;
			tempVert.pos.z = tempVert.pos.z*rhw;
			tempVert.color = 0xffffffff / (i+1);
			tempVert.u = 0.0f;
			tempVert.v = 1.0f;

			resVertexList.push_back(tempVert);

			// 第三个顶点
			tempVert.pos = mMatrix4x4::MatMultiply(m_PreVertexList[m_IndexList[i+2]].pos, m_Transform);
			rhw = 1/tempVert.pos.w;
			tempVert.pos.x = (tempVert.pos.x*rhw + 1.0f) * m_BackSize.x * 0.5f;
			tempVert.pos.y = (-tempVert.pos.y*rhw + 1.0f) * m_BackSize.y * 0.5f;
			tempVert.pos.z = tempVert.pos.z*rhw;
			tempVert.color = 0xffffffff / (i+1);
			tempVert.u = 1.0f;
			tempVert.v = 0.0f;

			resVertexList.push_back(tempVert);
		}

		// 非背面，不裁剪
		if(!_BackFaceCull(m_PreVertexList[m_IndexList[i+3]], m_PreVertexList[m_IndexList[i+4]], m_PreVertexList[m_IndexList[i+5]]))
		{
			tempVert.pos = mMatrix4x4::MatMultiply(m_PreVertexList[m_IndexList[i+3]].pos, m_Transform);
			float rhw = 1/tempVert.pos.w;
			tempVert.pos.x = (tempVert.pos.x*rhw + 1.0f) * m_BackSize.x * 0.5f;
			tempVert.pos.y = (-tempVert.pos.y*rhw + 1.0f) * m_BackSize.y * 0.5f;
			tempVert.pos.z = tempVert.pos.z*rhw;
			tempVert.color = 0xffffffff / (i+1);
			tempVert.u = 1.0f;
			tempVert.v = 1.0f;

			resVertexList.push_back(tempVert);

			// 第二个顶点
			tempVert.pos = mMatrix4x4::MatMultiply(m_PreVertexList[m_IndexList[i+4]].pos, m_Transform);
			rhw = 1/tempVert.pos.w;
			tempVert.pos.x = (tempVert.pos.x*rhw + 1.0f) * m_BackSize.x * 0.5f;
			tempVert.pos.y = (-tempVert.pos.y*rhw + 1.0f) * m_BackSize.y * 0.5f;
			tempVert.pos.z = tempVert.pos.z*rhw;
			tempVert.color = 0xffffffff / (i+1);
			tempVert.u = 1.0f;
			tempVert.v = 0.0f;

			resVertexList.push_back(tempVert);

			// 第三个顶点
			tempVert.pos = mMatrix4x4::MatMultiply(m_PreVertexList[m_IndexList[i+5]].pos, m_Transform);
			rhw = 1/tempVert.pos.w;
			tempVert.pos.x = (tempVert.pos.x*rhw + 1.0f) * m_BackSize.x * 0.5f;
			tempVert.pos.y = (-tempVert.pos.y*rhw + 1.0f) * m_BackSize.y * 0.5f;
			tempVert.pos.z = tempVert.pos.z*rhw;
			tempVert.color = 0xffffffff / (i+1);
			tempVert.u = 0.0f;
			tempVert.v = 1.0f;

			resVertexList.push_back(tempVert);
		}
	}

	for(size_t i=0; i<resVertexList.size(); i+=3)
	{
		DrawTriangle_ScanLine(resVertexList[i], resVertexList[i+1], resVertexList[i+2], resVertexList[i].color);
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

void mSoftRasterizer::DrawTriangle_Tiled(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3, DWORD color)
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

void mSoftRasterizer::DrawTriangle_TiledV2(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3, DWORD color)
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

	Vertex2 vert21(v1.pos.x, v1.pos.y);
	Vertex2 vert22(v2.pos.x, v2.pos.y);
	Vertex2 vert23(v3.pos.x, v3.pos.y);
  	Vertex2 edge1 = vert22 - vert21;
	Vertex2 edge2 = vert23 - vert21;  
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
                for(int iy = y; iy < y + q; iy++)
                {
                    for(int ix = x; ix < x + q; ix++)
                    {
						// 计算重心坐标，利用面积比计算
						float temp;
						Vertex2 ep1 = Vertex2((float)ix, (float)iy) - vert21;

						temp = Vertex2::Vec2Cross(edge1, edge2);
						float areaTri = abs(temp);
						temp = Vertex2::Vec2Cross(edge1, ep1);
						float areaC = abs(temp);
						temp = Vertex2::Vec2Cross(edge2, ep1);
						float areaB = abs(temp);
						float c = areaC/areaTri;
						float b = areaB/areaTri;
						float a = 1 - c - b;

						int u,v;
						double zr = a*(1/v1.pos.w) + b*(1/v2.pos.w) + c*(1/v3.pos.w);
						u = (int)(((a*(v1.u/v1.pos.w) + b*(v2.u/v2.pos.w) + c*(v3.u/v3.pos.w)) / zr) * TEX_WIDTH);
						v = (int)(((a*(v1.v/v1.pos.w) + b*(v2.v/v2.pos.w) + c*(v3.v/v3.pos.w)) / zr) * TEX_HEIGHT);

						int index = (int)(u + v*TEX_WIDTH);
						if(index < 0)
						{
							index = 0;
						}
						else
						if(index >= TEX_WIDTH*TEX_HEIGHT)
						{
							index = TEX_WIDTH*TEX_HEIGHT-1;
						}
						buffer[ix] = m_MainTexture[index];
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
							// 计算重心坐标，利用面积比计算
							float temp;
							Vertex2 ep1 = Vertex2((float)ix, (float)iy) - vert21;

							temp = Vertex2::Vec2Cross(edge1, edge2);
							float areaTri = abs(temp);
							temp = Vertex2::Vec2Cross(edge1, ep1);
							float areaC = abs(temp);
							temp = Vertex2::Vec2Cross(edge2, ep1);
							float areaB = abs(temp);
							float c = areaC/areaTri;
							float b = areaB/areaTri;
							float a = 1 - c - b;

							int u,v;
							double zr = a*(1/v1.pos.w) + b*(1/v2.pos.w) + c*(1/v3.pos.w);
							u = (int)(((a*(v1.u/v1.pos.w) + b*(v2.u/v2.pos.w) + c*(v3.u/v3.pos.w)) / zr) * TEX_WIDTH);
							v = (int)(((a*(v1.v/v1.pos.w) + b*(v2.v/v2.pos.w) + c*(v3.v/v3.pos.w)) / zr) * TEX_HEIGHT);

							int index = (int)(u + v*TEX_WIDTH);
							if(index < 0)
							{
								index = 0;
							}
							else
							if(index >= TEX_WIDTH*TEX_HEIGHT)
							{
								index = TEX_WIDTH*TEX_HEIGHT-1;
							}
							buffer[ix] = m_MainTexture[index];
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

void mSoftRasterizer::DrawTriangle_ScanLine(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3, DWORD color)
{
	// 确保顶点按照Y的顺序排序
	_SortThree(v1, v2, v3);
	if(FloatCmp(v1.pos.y, v2.pos.y))
	{
		_DrawTriangle_Top(v1, v2, v3, color);
	}
	else
	if(FloatCmp(v2.pos.y, v3.pos.y))
	{
		_DrawTriangle_Bottom(v1, v2, v3, color);
	}
	else
	{
		// 为了防止vert23,vert21构成的线段斜率为0，使用变化量求截取线段的其中一个X值
		// vert22.x 则为另一X值
		CubeVertex v4;
		float dy21 = v2.pos.y - v1.pos.y, dy31 = v3.pos.y - v1.pos.y;
		v4.pos.x = v1.pos.x + dy21*(v3.pos.x - v1.pos.x)/dy31;
		v4.pos.y = v2.pos.y;
		v4.pos.z = 1/v1.pos.z + dy21*(1/v3.pos.z - 1/v1.pos.z)/dy31;
		float invz1 = 1/v1.pos.z, invz3 = 1/v3.pos.z;
		v4.u = v1.u*invz1 + dy21*(v3.u*invz3 - v1.u*invz1)/dy31;
		v4.v = v1.v*invz1 + dy21*(v3.v*invz3 - v1.v*invz1)/dy31;
		v4.u /= v4.pos.z;
		v4.v /= v4.pos.z;
		v4.pos.z = 1/v4.pos.z;

		_DrawTriangle_Bottom(v1, v4, v2, color);
		_DrawTriangle_Top(v4, v2, v3, color);
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

// 绘制平底三角形 + 透视纹理映射
void mSoftRasterizer::_DrawTriangle_Bottom(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3, DWORD color)
{
	// 若三角形退化为直线，则直接绘制直线
	if(FloatCmp(v1.pos.y, v2.pos.y))
	{
		DrawLine2D((int)v1.pos.x, (int)v1.pos.y, (int)v3.pos.x, (int)v3.pos.y, color);
		return;
	}

	// 保证v2的X小于v3的X
	if(v3.pos.x < v2.pos.x)
	{
		Swap<CubeVertex>(v2, v3);
	}

	// 计算x的插值系数。
	float dxLeft, dxRight, dy21 = v2.pos.y - v1.pos.y, dy31 = v3.pos.y - v1.pos.y;
	dxLeft = (v2.pos.x - v1.pos.x)/dy21;
	dxRight = (v3.pos.x - v1.pos.x)/dy31;

	int iy = (int)ceil(v1.pos.y);
	float diyy = iy - v1.pos.y,
		xStart = v1.pos.x + dxLeft*diyy,
		xEnd = v1.pos.x + dxRight*diyy;

	// 计算1/z, u/z, t/z的插值系数。
	// 计算1/z
	float invZ1, invZ2, invZ3;
	if(v1.pos.z == 0)
	{
		invZ1 = 0;
	}
	else
	{
		invZ1 = 1/v1.pos.z;
	}
	if(v2.pos.z == 0)
	{
		invZ2 = 0;
	}
	else
	{
		invZ2 = 1/v2.pos.z;
	}
	if(v3.pos.z == 0)
	{
		invZ3 = 0;
	}
	else
	{
		invZ3 = 1/v3.pos.z;
	}

	float invZStart, invZEnd, dIzLeft, dIzRight, 
	uStart, uEnd, duLeft, duRight,
	vStart, vEnd, dvLeft, dvRight;

	// 1/z的系数
	dIzLeft = (invZ2 - invZ1)/dy21;
	dIzRight = (invZ3 - invZ1)/dy31;
	invZStart = invZ1 + dIzLeft*diyy;
	invZEnd = invZ1 + dIzRight*diyy;

	// u/z
	float uDivZ1 = v1.u * invZ1, 
		uDivZ2 = v2.u * invZ2, 
		uDivZ3 = v3.u * invZ3;
	duLeft = (uDivZ2 - uDivZ1)/dy21;
	duRight = (uDivZ3 - uDivZ1)/dy31;
	uStart = uDivZ1 + duLeft*diyy;
	uEnd = uDivZ1 + duRight*diyy;

	// v/z
	float vDivZ1 = v1.v * invZ1, 
		vDivZ2 = v2.v * invZ2, 
		vDivZ3 = v3.v * invZ3;
	dvLeft = (vDivZ2 - vDivZ1)/dy21;
	dvRight = (vDivZ3 - vDivZ1)/dy31;
	vStart = vDivZ1 + dvLeft*diyy;
	vEnd = vDivZ1 + dvRight*diyy;

	for(int curY=iy; curY<v3.pos.y; ++curY )
	{
		DrawLine2D((int)xStart, curY, (int)xEnd, curY, color);

		xStart += dxLeft;
		xEnd += dxRight;

		// 处理纹理坐标的插值
		float dx = xEnd - xStart;
		float invZStep = (invZEnd - invZStart)/dx, 
			uStep = (uEnd - uStart)/dx, 
			vStep = (vEnd - vStart)/dx;
		float curU = uStart, curV = vStart,
			curInvZ = invZStart;
		int x, xEndInt = (int)xEnd, u, v;
		for(x=(int)xStart; x<xEndInt; ++x )
		{
			float uF = _AdjustUV(curU / curInvZ);
			float vF = _AdjustUV(curV / curInvZ);
			u = (int)(uF * TEX_WIDTH);
			v = (int)(vF * TEX_HEIGHT);
			m_BackBuffer[curY*m_BackSize.x + x] = m_MainTexture[v*TEX_WIDTH + u];

			curU += uStep;
			curV += vStep;
			curInvZ += invZStep;
		}

		invZStart += dIzLeft;
		invZEnd += dIzRight;
		uStart += duLeft;
		uEnd += duRight;
		vStart += dvLeft;
		vEnd += dvRight;
	}
}

// 绘制平顶三角形
void mSoftRasterizer::_DrawTriangle_Top(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3, DWORD color)
{
	// 若三角形退化为直线，则直接绘制直线
	if(FloatCmp(v2.pos.y, v3.pos.y))
	{
		DrawLine2D((int)v1.pos.x, (int)v1.pos.y, (int)v2.pos.x, (int)v2.pos.y, color);
		return;
	}

	// 保证v1的X小于v2的X
	if(v2.pos.x < v1.pos.x)
	{
		Swap<CubeVertex>(v2, v1);
	}

	float dy31 = v3.pos.y - v1.pos.y,
		dy32 = v3.pos.y - v2.pos.y,
		dxLeft, dxRight;
	dxLeft = (v3.pos.x - v1.pos.x)/dy31;
	dxRight = (v3.pos.x - v2.pos.x)/dy32;

	int iy = (int)ceil(v1.pos.y);
	float diyy1 = iy - v1.pos.y, diyy2 = iy - v2.pos.y, 
		xStart = v1.pos.x + dxLeft*diyy1,
		xEnd = v2.pos.x + dxRight*diyy2;

		// 计算1/z, u/z, t/z的插值系数。
	// 计算1/z
	float invZ1, invZ2, invZ3;
	if(v1.pos.z == 0)
	{
		invZ1 = 0;
	}
	else
	{
		invZ1 = 1/v1.pos.z;
	}
	if(v2.pos.z == 0)
	{
		invZ2 = 0;
	}
	else
	{
		invZ2 = 1/v2.pos.z;
	}
	if(v3.pos.z == 0)
	{
		invZ3 = 0;
	}
	else
	{
		invZ3 = 1/v3.pos.z;
	}

	float invZStart, invZEnd, dIzLeft, dIzRight, 
	uStart, uEnd, duLeft, duRight,
	vStart, vEnd, dvLeft, dvRight;

	// 1/z的系数
	dIzLeft = (invZ3 - invZ1)/dy31;
	dIzRight = (invZ3 - invZ2)/dy32;
	invZStart = invZ1 + dIzLeft*diyy1;
	invZEnd = invZ2 + dIzRight*diyy2;

	// u/z
	float uDivZ1 = v1.u * invZ1, 
		uDivZ2 = v2.u * invZ2, 
		uDivZ3 = v3.u * invZ3;
	duLeft = (uDivZ3 - uDivZ1)/dy31;
	duRight = (uDivZ3 - uDivZ2)/dy32;
	uStart = uDivZ1 + duLeft*diyy1;
	uEnd = uDivZ2 + duRight*diyy2;

	// v/z
	float vDivZ1 = v1.v * invZ1, 
		vDivZ2 = v2.v * invZ2, 
		vDivZ3 = v3.v * invZ3;
	dvLeft = (vDivZ3 - vDivZ1)/dy31;
	dvRight = (vDivZ3 - vDivZ2)/dy32;
	vStart = vDivZ1 + dvLeft*diyy1;
	vEnd = vDivZ2 + dvRight*diyy2;

	for(int curY=iy; curY<v3.pos.y; ++curY )
	{
		DrawLine2D((int)xStart, curY, (int)xEnd, curY, color);

		xStart += dxLeft;
		xEnd += dxRight;

		// 处理纹理坐标的插值
		float dx = xEnd - xStart;
		float invZStep = (invZEnd - invZStart )/dx, 
			uStep = (uEnd - uStart)/dx, 
			vStep = (vEnd - vStart)/dx;
		float curU = uStart, curV = vStart,
			curInvZ = invZStart;
		int x = (int)xStart, xEndInt = (int)xEnd, u, v;
		for(; x<xEndInt; ++x )
		{
			float uF = _AdjustUV(curU / curInvZ);
			float vF = _AdjustUV(curV / curInvZ);
			u = (int)(uF * TEX_WIDTH);
			v = (int)(vF * TEX_HEIGHT);
			m_BackBuffer[curY*m_BackSize.x + x] = m_MainTexture[v*TEX_WIDTH + u];

			curU += uStep;
			curV += vStep;
			curInvZ += invZStep;
		}

		invZStart += dIzLeft;
		invZEnd += dIzRight;
		uStart += duLeft;
		uEnd += duRight;
		vStart += dvLeft;
		vEnd += dvRight;
	}
}

void mSoftRasterizer::_SortThree(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3)
{
	if(v1.pos.y > v2.pos.y)
	{
		Swap<CubeVertex>(v1, v2);
	}
	if(v2.pos.y > v3.pos.y)
	{
		Swap<CubeVertex>(v2, v3);
	}
	if(v1.pos.y > v2.pos.y)
	{
		Swap<CubeVertex>(v1, v2);
	}
}

// true 为背面, 需裁剪
// false 为正面, 不裁剪
bool mSoftRasterizer::_BackFaceCull(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3)
{
	Vertex3 v12 = v2.pos - v1.pos;
	Vertex3 v13 = v3.pos - v1.pos;
	Vertex3 N;
	Vertex3::Vec3Cross(N, v13, v12);

	Vertex3 view = m_Camera.GetEye() - v1.pos;
	if(Vertex3::Vec3Dot(view, N) < 0.0f)
	{
		return true;
	}

	return false;
}

float mSoftRasterizer::_AdjustUV(float val)
{
	float mod = fmodf(val, 1.0f);
	if(val < 0)
	{
		mod = 1.0f + mod;
	}

	return mod;
}