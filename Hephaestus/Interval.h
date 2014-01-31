#pragma once

class Interval {
  public:
    Interval(float a, float b) {
      start = a;
      end = b;
    }

    bool Contains(float x) { return x >= start && x <= end; }

  private:
    float start, end;
};