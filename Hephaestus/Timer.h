#pragma once
#include <SFML/System/Clock.hpp>

class Timer {
  public:
    static void Start() { Reset1(); main.restart();}
    static void Stop() { mainTime = main.getElapsedTime().asSeconds() * 1000000; }
    static void Start1() { clock1.restart(); }
    static void Stop1() {
      float time = clock1.getElapsedTime().asSeconds() * 1000000;
      time1 += time;
      if (time > worst1)
        worst1 = time;

      ++count1;
    }
    static void Reset1() { time1 = 0; worst1 = 0; count1 = 0; }
    static float Total() { return mainTime; }
    static float Time1() { return time1; }
    static float Average1() { return time1 / count1; }
    static float Worst1() { return worst1; }
    static float Fraction1() { return 100 * time1 / mainTime; }

  private:
    static sf::Clock main, clock1;
    static float mainTime, time1, worst1;
    static int count1;
};