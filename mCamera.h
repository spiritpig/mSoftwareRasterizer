#include "stdafx.h"

#pragma once
class mCamera
{
public:
	mCamera(void);
	~mCamera(void);

	const mMatrix4x4& GetView() const
	{
		return m_View;
	}
	const Vertex3& GetEye() const
	{
		return m_Eye;
	}
	void Set(const Vertex3 &eye, const Vertex3 &lookAt, const Vertex3 &up);
	void Move(const Vertex3 &delta);

private:
	mMatrix4x4 m_View;
	Vertex3 m_Eye, m_LookAt, m_Right, m_Up;
};

