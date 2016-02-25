#pragma once
class Vertex3
{
public:
	Vertex3(void);
	Vertex3(float _x, float _y, float _z);
	~Vertex3(void);

	float Length() const;
	void Normalized();
	Vertex3 operator-(const Vertex3 &ver) const;
	Vertex3 operator+(const Vertex3 &ver) const;
	Vertex3& operator+=(const Vertex3 &ver);

	static void Vec3Cross(Vertex3 &v1, const Vertex3 &v2, const Vertex3 &v3);
	static float Vec3Dot(const Vertex3 &v1, const Vertex3 &v2);

	float x, y, z;
};

class Vertex4
{
public:
	Vertex4(void);
	Vertex4(float _x, float _y, float _z, float _w);
	~Vertex4(void);

	float x, y, z, w;
};

