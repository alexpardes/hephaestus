#ifndef UTIL_
#define UTIL_

#include "stdafx.h"

#define M_PI 3.14159265358979323846f

using sf::Vector2f;
using sf::Vector2i;

class Util {
	public:
		static float Degrees(float radians);
		static float Radians(float degrees);
		static float Distance(const Vector2f &point1,
							  const Vector2f &point2);
		static float Distance2(const Vector2f &point1,
							   const Vector2f &point2);
		static float Length(const Vector2f &vector);
		static float Length2(const Vector2f &vector);
		static float Cross(const Vector2f &vector1, const Vector2f &vector2);
		static float Dot(const Vector2f &vector1, const Vector2f &vector2);
		static float FindAngle(const Vector2f &vector1, const Vector2f &vector2);
		static Vector2f Perpendicular(const Vector2f &vector);
		static void Normalize(Vector2f &vector);
		static void Scale(Vector2f &vector, float length);
		static float Square(float x);
		template <typename T> static int Sign(T val);
		static int Laterality(const Vector2f &vector1,
							  const Vector2f &vector2);
		static void Rotate(Vector2f &vector, float angle);
		static Vector2i GetVector2i(const Vector2f &vector) {
			return Vector2i((int) vector.x, (int) vector.y);
		}
		static Vector2f GetVector2f(const Vector2i &vector) {
			return Vector2f((float) vector.x, (float) vector.y);
		}
};

#endif