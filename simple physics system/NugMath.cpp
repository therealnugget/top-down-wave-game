#include "physics.hpp"
#include "math.hpp"
Vector2<float> Math::GetTurbulentPos(Vector2<float> pt, Vector2<float> origin, float rotation, float period, float amplitude) {
	Vector2<float> returnVal = origin.To(pt).Rotated(rotation);
	return returnVal + Vector2<float>(amplitude * sinf(returnVal.y * period), amplitude * sinf(returnVal.x * period)) + origin;
}
float Math::Cross(Vector2<float> a, Vector2<float> b) {
	return a.x * b.y - a.y * b.x;
}