template<typename T>
inline void Swap(T &a, T &b)
{
	T temp = a;
	a = b;
	b = temp;
}