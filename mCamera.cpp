#include "stdafx.h"


mCamera::mCamera(void)
{
	mMatrix4x4::MatIdentify(m_View);
}


mCamera::~mCamera(void)
{
}

void mCamera::Set(const Vertex3 &eye, const Vertex3 &lookAt, const Vertex3 &up)
{
	m_Eye = eye;
	m_LookAt = lookAt;
	
	Vertex3 eyeline = m_LookAt - m_Eye;
	eyeline.Normalized();

	Vertex3::Vec3Cross(m_Right, up, eyeline);
	m_Right.Normalized();

	Vertex3::Vec3Cross(m_Up, eyeline, m_Right);
	m_Up.Normalized();

	m_View.m_Val[0][0] = m_Right.x;
	m_View.m_Val[1][0] = m_Right.y;
	m_View.m_Val[2][0] = m_Right.z;

	m_View.m_Val[0][1] = m_Up.x;
	m_View.m_Val[1][1] = m_Up.y;
	m_View.m_Val[2][1] = m_Up.z;

	m_View.m_Val[0][2] = eyeline.x;
	m_View.m_Val[1][2] = eyeline.y;
	m_View.m_Val[2][2] = eyeline.z;

	m_View.m_Val[3][0] = -Vertex3::Vec3Dot(m_Eye, m_Right);
	m_View.m_Val[3][1] = -Vertex3::Vec3Dot(m_Eye, m_Up);
	m_View.m_Val[3][2] = -Vertex3::Vec3Dot(m_Eye, eyeline);
}

void mCamera::Move(const Vertex3 &delta)
{
	m_Eye += delta;
	m_LookAt += delta;

	Vertex3 eyeline = m_LookAt - m_Eye;
	eyeline.Normalized();

	m_View.m_Val[3][0] = -Vertex3::Vec3Dot(m_Eye, m_Right);
	m_View.m_Val[3][1] = -Vertex3::Vec3Dot(m_Eye, m_Up);
	m_View.m_Val[3][2] = -Vertex3::Vec3Dot(m_Eye, eyeline);
}