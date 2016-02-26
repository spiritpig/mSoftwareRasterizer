#include "stdafx.h"
#include "mMatrix4x4.h"


mMatrix4x4::mMatrix4x4(void)
{
	memset(m_Val, 0, sizeof(m_Val));
}


mMatrix4x4::~mMatrix4x4(void)
{
}

mMatrix4x4 mMatrix4x4::operator*(const mMatrix4x4 &mat)
{
	mMatrix4x4 res;
	// 第一行
	res.m_Val[0][0] = this->m_Val[0][0]*mat.m_Val[0][0] + this->m_Val[0][1]*mat.m_Val[1][0] + 
					this->m_Val[0][2]*mat.m_Val[2][0] + this->m_Val[0][3]*mat.m_Val[3][0];

	res.m_Val[0][1] = this->m_Val[0][0]*mat.m_Val[0][1] + this->m_Val[0][1]*mat.m_Val[1][1] + 
				this->m_Val[0][2]*mat.m_Val[2][1] + this->m_Val[0][3]*mat.m_Val[3][1];

	res.m_Val[0][2] = this->m_Val[0][0]*mat.m_Val[0][2] + this->m_Val[0][1]*mat.m_Val[1][2] + 
				this->m_Val[0][2]*mat.m_Val[2][2] + this->m_Val[0][3]*mat.m_Val[3][2];

	res.m_Val[0][3] = this->m_Val[0][0]*mat.m_Val[0][3] + this->m_Val[0][1]*mat.m_Val[1][3] + 
				this->m_Val[0][2]*mat.m_Val[2][3] + this->m_Val[0][3]*mat.m_Val[3][3];


	// 第二行
	res.m_Val[1][0] = this->m_Val[1][0]*mat.m_Val[0][0] + this->m_Val[1][1]*mat.m_Val[1][0] + 
				this->m_Val[1][2]*mat.m_Val[2][0] + this->m_Val[1][3]*mat.m_Val[3][0];

	res.m_Val[1][1] = this->m_Val[1][0]*mat.m_Val[0][1] + this->m_Val[1][1]*mat.m_Val[1][1] + 
				this->m_Val[1][2]*mat.m_Val[2][1] + this->m_Val[1][3]*mat.m_Val[3][1];

	res.m_Val[1][2] = this->m_Val[1][0]*mat.m_Val[0][2] + this->m_Val[1][1]*mat.m_Val[1][2] + 
				this->m_Val[1][2]*mat.m_Val[2][2] + this->m_Val[1][3]*mat.m_Val[3][2];

	res.m_Val[1][3] = this->m_Val[1][0]*mat.m_Val[0][3] + this->m_Val[1][1]*mat.m_Val[1][3] + 
				this->m_Val[1][2]*mat.m_Val[2][3] + this->m_Val[1][3]*mat.m_Val[3][3];

	// 第三行
	res.m_Val[2][0] = this->m_Val[2][0]*mat.m_Val[0][0] + this->m_Val[2][1]*mat.m_Val[1][0] + 
				this->m_Val[2][2]*mat.m_Val[2][0] + this->m_Val[2][3]*mat.m_Val[3][0];

	res.m_Val[2][1] = this->m_Val[2][0]*mat.m_Val[0][1] + this->m_Val[2][1]*mat.m_Val[1][1] + 
				this->m_Val[2][2]*mat.m_Val[2][1] + this->m_Val[2][3]*mat.m_Val[3][1];

	res.m_Val[2][2] = this->m_Val[2][0]*mat.m_Val[0][2] + this->m_Val[2][1]*mat.m_Val[1][2] + 
				this->m_Val[2][2]*mat.m_Val[2][2] + this->m_Val[2][3]*mat.m_Val[3][2];

	res.m_Val[2][3] = this->m_Val[2][0]*mat.m_Val[0][3] + this->m_Val[2][1]*mat.m_Val[1][3] + 
				this->m_Val[2][2]*mat.m_Val[2][3] + this->m_Val[2][3]*mat.m_Val[3][3];

	// 第四行
	res.m_Val[3][0] = this->m_Val[3][0]*mat.m_Val[0][0] + this->m_Val[3][1]*mat.m_Val[1][0] + 
				this->m_Val[3][2]*mat.m_Val[2][0] + this->m_Val[3][3]*mat.m_Val[3][0];

	res.m_Val[3][1] = this->m_Val[3][0]*mat.m_Val[0][1] + this->m_Val[3][1]*mat.m_Val[1][1] + 
				this->m_Val[3][2]*mat.m_Val[2][1] + this->m_Val[3][3]*mat.m_Val[3][1];

	res.m_Val[3][2] = this->m_Val[3][0]*mat.m_Val[0][2] + this->m_Val[3][1]*mat.m_Val[1][2] + 
				this->m_Val[3][2]*mat.m_Val[2][2] + this->m_Val[3][3]*mat.m_Val[3][2];

	res.m_Val[3][3] = this->m_Val[3][0]*mat.m_Val[0][3] + this->m_Val[3][1]*mat.m_Val[1][3] + 
				this->m_Val[3][2]*mat.m_Val[2][3] + this->m_Val[3][3]*mat.m_Val[3][3];

	return res;
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

void mMatrix4x4::MatRotateX(mMatrix4x4 &mat, float val)
{
	mat.m_Val[1][1] = cos(val);
	mat.m_Val[1][2] = sin(val);
	mat.m_Val[2][1] = -sin(val);
	mat.m_Val[2][2] = cos(val);
}

void mMatrix4x4::MatRotateY(mMatrix4x4 &mat, float val)
{
	mat.m_Val[0][0] = cos(val);
	mat.m_Val[0][2] = -sin(val);
	mat.m_Val[2][0] = sin(val);
	mat.m_Val[2][2] = cos(val);
}

void mMatrix4x4::MatRotateZ(mMatrix4x4 &mat, float val)
{
	mat.m_Val[0][0] = cos(val);
	mat.m_Val[0][1] = sin(val);
	mat.m_Val[1][0] = -sin(val);
	mat.m_Val[1][1] = cos(val);
}

void mMatrix4x4::MatLookAt(mMatrix4x4 &mat, Vertex3 &eye, Vertex3 &lookat, Vertex3 &up)
{
	Vertex3 eyeline = lookat - eye;
	eyeline.Normalized();

	Vertex3 right;
	Vertex3::Vec3Cross(right, up, eyeline);
	right.Normalized();

	Vertex3 rUp;		// 再求一次UP，保证UP的正确性
	Vertex3::Vec3Cross(rUp, eyeline, right);
	rUp.Normalized();

	mat.m_Val[0][0] = right.x;
	mat.m_Val[1][0] = right.y;
	mat.m_Val[2][0] = right.z;

	mat.m_Val[0][1] = rUp.x;
	mat.m_Val[1][1] = rUp.y;
	mat.m_Val[2][1] = rUp.z;

	mat.m_Val[0][2] = eyeline.x;
	mat.m_Val[1][2] = eyeline.y;
	mat.m_Val[2][2] = eyeline.z;

	mat.m_Val[3][0] = -Vertex3::Vec3Dot(eye, right);
	mat.m_Val[3][1] = -Vertex3::Vec3Dot(eye, rUp);
	mat.m_Val[3][2] = -Vertex3::Vec3Dot(eye, eyeline);
}

void mMatrix4x4::MatPerspective(mMatrix4x4 &mat, float fov, float aspect, float zn, float zf)
{
	mat.m_Val[1][1] = tan((90 - fov*0.5f)*DEG_TO_RAD);
	mat.m_Val[0][0] = mat.m_Val[1][1]/aspect;
	mat.m_Val[2][2] = zf/(zf-zn);
	mat.m_Val[3][2] = -zn*zf/(zf-zn);

	mat.m_Val[2][3] = 1;
	mat.m_Val[3][3] = 0;
}

Vertex4 mMatrix4x4::MatMultiply(Vertex4 &v, mMatrix4x4 &mat)
{
	Vertex4 res;
		// 第一行
	res.x = v.x*mat.m_Val[0][0] + v.y*mat.m_Val[1][0] + v.z*mat.m_Val[2][0] + v.w*mat.m_Val[3][0];
	res.y = v.x*mat.m_Val[0][1] + v.y*mat.m_Val[1][1] + v.z*mat.m_Val[2][1] + v.w*mat.m_Val[3][1];
	res.z = v.x*mat.m_Val[0][2] + v.y*mat.m_Val[1][2] + v.z*mat.m_Val[2][2] + v.w*mat.m_Val[3][2];
	res.w = v.x*mat.m_Val[0][3] + v.y*mat.m_Val[1][3] + v.z*mat.m_Val[2][3] + v.w*mat.m_Val[3][3];

	return res;
}