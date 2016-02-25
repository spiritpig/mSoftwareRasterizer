#include "stdafx.h"

// ×óÊÖ×ø±êÏµ
#pragma once
class mMatrix4x4
{
public:
	mMatrix4x4(void);
	~mMatrix4x4(void);

	static void MatIdentify(mMatrix4x4 &mat);
	static void MatTranslate(mMatrix4x4 &mat, Vertex4 &val);
	static void MatScale(mMatrix4x4 &mat, float x, float y, float z);
	static void MatLookAt(mMatrix4x4 &mat, Vertex3 &eye, Vertex3 &lookat, Vertex3 &up );

private:
	float m_Val[4][4];
};

