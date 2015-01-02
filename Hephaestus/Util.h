#pragma once

#include <functional>
#include "Vector2.h"
#include "Rectangle.h"
#include "DirectedSegment.h"

#define M_PI 3.14159265358979323846f

class Util {
	public:   
    static Vector2f MakeUnitVector(float angle);

    static int Sign(int x);
    static size_t HashStart();

    template<typename T> static void Hash(size_t &hash, const T &t) {
      for (size_t i = 0; i < sizeof(t); ++i) {
        hash = (hash * 16777619) ^ ((unsigned char *)&t)[i];
      }
    }

    //static void Hash(size_t &h, size_t x);
    //static void Hash(size_t &h, const Vector2f &v);
    
    template<> static void Hash<std::string>(size_t &h, const std::string &s) {
      std::hash<std::string> hasher;
      Hash(h, hasher(s));
    }

    // Returns the angle between origin and origin + 2*pi.
    static float Angle(float angle, float origin = 0);
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

    // Returns the clockwise angle from angle1 to angle2. Assumes input angles
    // are between 0 and 2*pi.
    static float AngleCW(float angle1, float angle2);
    static float AngleCCW(float angle1, float angle2);

    // Returns the shortest angle between angle1 and angle2.
    static float AngleBetween(float angle1, float angle2);

    // Returns true if testAngle is in the sector from angle1 counterclockwise
    // to angle2.
    static bool IsBetweenAngles(float testAngle, float angle1, float angle2, bool flipRotation = false);
    static bool IsBetweenAnglesClosed(float testAngle, float angle1, float angle2, bool flipRotation = false);
    static bool IsBetweenAnglesOpen(float testAngle, float angle1, float angle2);

    // Returns the angle in radians between the given vector and (1, 0).
    static float FindAngle(const Vector2f &vector);

    // Returns the angle in radians from vector1 counterclockwise to vector2.
		static float FindAngle(const Vector2f &vector1, const Vector2f &vector2);
    static float FindAngleDegrees(const Vector2f &vector1, const Vector2f &vector2);

    // Returns the shortest angle between vector1 and vector2.
    static float FindShortestAngle(const Vector2f &vector1, const Vector2f &vector2);
		static Vector2f Perpendicular(const Vector2f &vector);
		static void Normalize(Vector2f &vector);
    static Vector2f Normalized(const Vector2f &vector);
		static void Scale(Vector2f &vector, float length);
    static Vector2f Scaled(const Vector2f &vector, float length);
		static float Square(float x);
		template <typename T> static int Sign(T val);
		static int Laterality(const Vector2f &vector1,
							  const Vector2f &vector2);
		static void Rotate(Vector2f &vector, float angle);
    static Vector2f Rotated(const Vector2f &vector, float angle);
		static Vector2i GetVector2i(const Vector2f &vector) {
			return Vector2i((int) vector.x, (int) vector.y);
		}
		static Vector2f ToVector2f(const Vector2i &vector) {
			return Vector2f((float) vector.x, (float) vector.y);
		}
		static Vector2f ToVector2f(const sf::Vector2u &vector) {
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

    // Weight 0 -> angle1, weight 1 -> angle2
    // Interpolates through shortest angle
    static float InterpolateAngles(float angle1, float angle2, float weight);

    // Interpolates through counterclockwise angle
    static float InterpolateAnglesCcw(float angle1, float angle2, float weight);
};