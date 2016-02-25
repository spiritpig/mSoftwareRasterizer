#include "stdafx.h"
#include "Vertex.h"

Vertex3::Vertex3(void)
	: x(0), y(0), z(0)
{
}

Vertex3::Vertex3(float _x, float _y, float _z)
	: x(_x), y(_y), z(_z)
{
}

Vertex3::~Vertex3(void)
{
}

float Vertex3::Length() const
{
	return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
}

void Vertex3::Normalized()
{
	float length = this->Length();
	if(length > 0)
	{
		this->x /= length;
		this->y /= length;
		this->z /= length;
	}
	else
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
}

Vertex3 Vertex3::operator-(const Vertex3 &ver) const
{
	return Vertex3(this->x - ver.x, this->y - ver.y, this->z - ver.z);
}

Vertex3 Vertex3::operator+(const Vertex3 &ver) const
{
	return Vertex3(this->x + ver.x, this->y + ver.y, this->z + ver.z);
}

Vertex3& Vertex3::operator+=(const Vertex3 &ver)
{
	this->x += ver.x;
	this->y += ver.y;
	this->z += ver.z;

	return *this;
}

void Vertex3::Vec3Cross(Vertex3 &v1, const Vertex3 &v2, const Vertex3 &v3)
{
	v1.x = v2.y*v3.z - v2.z*v3.y;
	v1.y = v2.z*v3.x - v2.x*v3.z;
	v1.z = v2.x*v3.y - v2.y*v3.x;
}

float Vertex3::Vec3Dot(const Vertex3 &v1, const Vertex3 &v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}



///////////////////////////////////////////////////////
//	Vertex4
///////////////////////////////////////////////////////
Vertex4::Vertex4(void)
	: x(0), y(0), z(0), w(0)
{
}

Vertex4::Vertex4(float _x, float _y, float _z, float _w)
	: x(_x), y(_y), z(_z), w(_w)
{
}

Vertex4::~Vertex4(void)
{
}
