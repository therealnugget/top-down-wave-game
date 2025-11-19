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
	template<typename T>
	static inline Vector2<T> Sign(Vector2<T> v) {
		return Vector2<T>(Sign(v.x), Sign(v.y));
	}
	template<typename T>
	static inline Vector2<T> SignOrZero(Vector2<T> v) {
		return Vector2<T>(SignOrZero(v.x), SignOrZero(v.y));
	}
	template<typename T>
	static inline Vector2<T> abs(Vector2<T> orig) {
		return orig * Sign(orig);
	}
	template<typename T>
	static inline Vector2<T> Min(Vector2<T> a, Vector2<T> b) {
		return Vector2<T>(Min(a.x, b.x), Min(a.y, b.y));
	}
	template<typename T>
	static inline Vector2<T> Max(Vector2<T> a, Vector2<T> b) {
		return Vector2<T>(Max(a.x, b.x), Max(a.y, b.y));
	}
	static inline float sqr(float f) {
		return f * f;
	}
	template<typename T>
	static inline Vector2<T> Round(Vector2<T> v) {
		return Vector2<T>(roundf(v.x), roundf(v.y));
	}
	static inline float Cross(Vector2<float> a, Vector2<float> b) {
		return a.x * b.y - a.y * b.x;
	}
};