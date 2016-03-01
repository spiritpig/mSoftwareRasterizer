#pragma once

class Vertex2
{
public:
	Vertex2(void);
	Vertex2(float _x, float _y);
	~Vertex2(void);

	float Length() const;
	void Normalized();
	Vertex2 operator-(const Vertex2 &ver) const;
	Vertex2 operator+(const Vertex2 &ver) const;
	Vertex2& operator+=(const Vertex2 &ver);

	static float Vec2Cross(const Vertex2 &v2, const Vertex2 &v3);
	static float Vec2Dot(const Vertex2 &v1, const Vertex2 &v2);

	float x, y;
};

class Vertex4;
class Vertex3
{
public:
	Vertex3(void);
	Vertex3(float _x, float _y, float _z);
	Vertex3(Vertex4& v4);
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

	Vertex4 operator-(const Vertex4 &ver) const;

	float x, y, z, w;
};