#pragma once

struct Vector2 {
  public:
    Vector2(double x, double y) : x(x), y(y) { }
    //Vector2(const Vector2 &original) {
    //  x = original.x;
    //  y = original.y;
    //}
    double x, y;
};