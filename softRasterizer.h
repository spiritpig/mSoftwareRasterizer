#include <Windows.h>

#pragma once
class softRasterizer
{
public:
	softRasterizer(void);
	~softRasterizer(void);

private:
	HDC m_hBackDc;
	HBITMAP m_hBackBmp;

	HDC m_hFrontDc;
	HBITMAP m_hFrontBmp;
};

