#pragma once

#include <functional>
#include "Vector2.h"
#include "Rectangle.h"
#include "DirectedSegment.h"

#define M_PI 3.14159265358979323846f

class Util {
	public:

    // Returns the angle between origin and origin + 2*pi.
    static float Angle(float angle, float origin);
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
    static float FindAngle(const Vector2f &vector);
		static float FindAngle(const Vector2f &vector1, const Vector2f &vector2);
    static float FindAngleDegrees(const Vector2f &vector1, const Vector2f &vector2);
		static Vector2f Perpendicular(const Vector2f &vector);
		static void Normalize(Vector2f &vector);
    static Vector2f Normalized(const Vector2f &vector);
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
		static Vector2f GetVector2f(const sf::Vector2u &vector) {
			return Vector2f((float) vector.x, (float) vector.y);
		}
    static void Limit(Vector2f &v, float length);

    template<class T> static T Constrain(T x, T min, T max) {
      if (x < min) return min;
      if (x > max) return max;
      return x;
    }

    template<class T> static T Constrain2(T v, T min, T max) {
      T result(v);
      result.x = Constrain(result.x, min.x, max.x);
      result.y = Constrain(result.y, min.y, max.y);
      return result;
    }

    const static Vector2i kLeft, kUp, kRight, kDown;
    static Rect BoundingRectangle(const DirectedSegment &segment);
    static float InterpolateAngles(float angle1, float angle2, float weight);
};