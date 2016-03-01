#ifndef _BASEMATH_
#define _BASEMATH_

template<typename T>
inline void Swap(T &a, T &b)
{
	T temp = a;
	a = b;
	b = temp;
}

inline bool FloatCmp(float a, float b)
{
	float val = a-b;
	if(val < 0.00001f && val > -0.00001f)
	{
		return true;
	}

	return false;
}

#endif // _BASEMATH_