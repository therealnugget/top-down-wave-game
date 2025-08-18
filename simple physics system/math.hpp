#pragma once
//static
class Math {
public:
	static inline float Sign(float num) {
		return (num >= .0f) * 2.f - 1.f;
	}
	static inline float SignOrZero(float num) {
		return Sign(num) - (num == .0f);
	}
	//special case where i don't use camel case because it's such a commonly used function
	static inline float abs(float orig) {
		return orig * Sign(orig);
	}
	template<typename T>
	static inline T Min(T a, T b) {
		return a < b ? a : b;
	}
	template<typename T>
	static inline T Max(T a, T b) {
		return a > b ? a : b;
	}
	static inline float Min(float a, float b) {
		return a < b ? a : b;
	}
	static inline float Max(float a, float b) {
		return a > b ? a : b;
	}
	template<typename T2>
	static inline Vector2<T2> Sign(Vector2<T2> v) {
		return Vector2<T2>(Sign(v.x), Sign(v.y));
	}
	template<typename T2>
	static inline Vector2<T2> SignOrZero(Vector2<T2> v) {
		return Vector2<T2>(SignOrZero(v.x), SignOrZero(v.y));
	}
	template<typename T2>
	static inline Vector2<T2> abs(Vector2<T2> orig) {
		return orig * Sign(orig);
	}
	template<typename T2>
	static inline Vector2<T2> Min(Vector2<T2> a, Vector2<T2> b) {
		return Vector2<T2>(Min(a.x, b.x), Min(a.y, b.y));
	}
	template<typename T2>
	static inline Vector2<T2> Max(Vector2<T2> a, Vector2<T2> b) {
		return Vector2<T2>(Max(a.x, b.x), Max(a.y, b.y));
	}
	static inline float sqr(float f) {
		return f * f;
	}
	template<typename T2>
	static inline Vector2<T2> Round(Vector2<T2> v) {
		return Vector2<T2>(roundf(v.x), roundf(v.y));
	}
	static inline float Cross(Vector2<float> a, Vector2<float> b) {
		return a.x * b.y - a.y * b.x;
	}
};