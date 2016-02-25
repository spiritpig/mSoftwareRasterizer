#include "stdafx.h"
#include "mMatrix4x4.h"


mMatrix4x4::mMatrix4x4(void)
{
}


mMatrix4x4::~mMatrix4x4(void)
{
}

void mMatrix4x4::MatIdentify(mMatrix4x4 &mat)
{
	mat.m_Val[0][0] = 1; mat.m_Val[0][1] = 0; mat.m_Val[0][2] = 0; mat.m_Val[0][3] = 0;
	mat.m_Val[1][0] = 0; mat.m_Val[1][1] = 1; mat.m_Val[1][2] = 0; mat.m_Val[1][3] = 0;
	mat.m_Val[2][0] = 0; mat.m_Val[2][1] = 0; mat.m_Val[2][2] = 1; mat.m_Val[2][3] = 0;
	mat.m_Val[3][0] = 0; mat.m_Val[3][1] = 0; mat.m_Val[3][2] = 0; mat.m_Val[3][3] = 1;
}

void mMatrix4x4::MatTranslate(mMatrix4x4 &mat, Vertex4 &val)
{
	mat.m_Val[3][0] += val.x; 
	mat.m_Val[3][1] += val.y; 
	mat.m_Val[3][2] += val.z; 
	mat.m_Val[3][3] = 1;
}

void mMatrix4x4::MatScale(mMatrix4x4 &mat, float x, float y, float z)
{
	mat.m_Val[0][0] *= x; 
	mat.m_Val[1][1] *= y; 
	mat.m_Val[2][2] *= z; 
}

void mMatrix4x4::MatLookAt(mMatrix4x4 &mat, Vertex3 &eye, Vertex3 &lookat, Vertex3 &up )
{
	Vertex3 eyeline = lookat - eye;
}