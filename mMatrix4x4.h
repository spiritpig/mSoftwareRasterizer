#include "stdafx.h"

// ×óÊÖ×ø±êÏµ
#pragma once
class mMatrix4x4
{
public:
	mMatrix4x4(void);
	~mMatrix4x4(void);
	mMatrix4x4 operator*(const mMatrix4x4 &mat);

	static void MatIdentify(mMatrix4x4 &mat);
	static void MatTranslate(mMatrix4x4 &mat, Vertex4 &val);
	static void MatScale(mMatrix4x4 &mat, float x, float y, float z);
	static void MatRotateX(mMatrix4x4 &mat, float val);
	static void MatRotateY(mMatrix4x4 &mat, float val);
	static void MatRotateZ(mMatrix4x4 &mat, float val);
	static void MatLookAt(mMatrix4x4 &mat, Vertex3 &eye, Vertex3 &lookat, Vertex3 &up );
	static void MatPerspective(mMatrix4x4 &mat, float fov, float aspect, float zn, float zf);
	static Vertex4 MatMultiply(Vertex4 &v, mMatrix4x4 &mat);

	float m_Val[4][4];
};

