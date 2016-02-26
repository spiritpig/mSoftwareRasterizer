#include "stdafx.h"

#ifndef _MSOFT_RER_
#define _MSOFT_RER_

struct CubeVertex
{
	Vertex4 pos;
	float u, v;
	DWORD color;
};

class mSoftRasterizer
{
public:
	mSoftRasterizer(HWND hWnd, int width, int height);
	~mSoftRasterizer();

	void Init();
	void Update();
	void Present();

	void MoveCam(float x, float y, float z);

	void DrawPoint(int x, int y, int Size, DWORD color);
	void DrawLine2D(int x1, int y1, int x2, int y2, DWORD color);
	void DrawTriangle2D(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3, DWORD color);
	void DrawTriangle2DV2(CubeVertex &v1, CubeVertex &v2, CubeVertex &v3, DWORD color);

	void ReadSkullModel();
	void ClearQuick();

private:
	HWND m_hWnd;
	unsigned int *m_BackBuffer, *m_MainTexture;
	HDC m_hBackDc;
	HBITMAP m_hBackBmp;
	POINT m_BackSize;

	mMatrix4x4 m_Transform;
	mMatrix4x4 m_World;
	mMatrix4x4 m_Proj;

	mCamera m_Camera;

	std::vector<CubeVertex> m_PreVertexList;
	std::vector<int> m_IndexList;
};

#endif // _MSOFT_RER_
