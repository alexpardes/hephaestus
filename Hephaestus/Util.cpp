#include "stdafx.h"
#include "Util.h"

float Util::Degrees(float radians) {
	return radians * 180.f / M_PI;
}

float Util::Radians(float degrees) {
	return degrees * M_PI / 180.f;
}

float Util::Distance2(const Vector2f &point1, const Vector2f &point2) {
	return Length2(point1 - point2);
}

float Util::Distance(const Vector2f &point1, const Vector2f &point2) {
	return Length(point1 - point2);
}

//	Returns the square of the length of the vector.
float Util::Length2(const Vector2f &vector) {
	return vector.x * vector.x + vector.y * vector.y;
}

float Util::Length(const Vector2f &vector) {
	return std::sqrtf(vector.x * vector.x + vector.y * vector.y);
}

float Util::Cross(const Vector2f &vector1, const Vector2f &vector2) {
	return vector1.x * vector2.y - vector1.y * vector2.x;
}

float Util::Dot(const Vector2f &vector1, const Vector2f &vector2) {
	return vector1.x * vector2.x + vector1.y * vector2.y;
}


//	Returns the angle in radians from vector1 counterclockwise to vector2.
float Util::FindAngle(const Vector2f &vector1, const Vector2f &vector2) {
	float angle = std::acosf(Dot(vector1, vector2) / Length(vector1) / Length(vector2));
	if (Cross(vector1, vector2) < 0) angle = 2*M_PI - angle;
	return angle;
}


// Returns the vector rotated by 90 degrees CCW.
Vector2f Util::Perpendicular(const Vector2f &vector) {
	return Vector2f(-vector.y, vector.x);
}

void Util::Normalize(Vector2f &vector) {
	float length = Length(vector);
	if (length != 0)
			vector /= Length(vector);
}

void Util::Scale(Vector2f &vector, float length) {
	Normalize(vector);
	vector *= length;
}

template <typename T> int Util::Sign(T val) {
    return (T(0) < val) - (val < T(0));
}

int Util::Laterality(const Vector2f &vector1,
					 const Vector2f &vector2) {
	return Sign(Cross(vector1, vector2));
}

//	Rotates the vector counter clockwise by angle in radians.
void Util::Rotate(Vector2f &vector, float angle) {
	float cos = std::cosf(angle);
	float sin = std::sinf(angle);
	float x = cos*vector.x - sin*vector.y;
	float y = sin*vector.x + cos*vector.y;
	vector.x = x;
	vector.y = y;
}

float Util::Square(float x) {
	return x * x;
}