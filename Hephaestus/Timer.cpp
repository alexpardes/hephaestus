#include "stdafx.h"
#include "Timer.h"

sf::Clock Timer::main, Timer::clock1;
float Timer::mainTime = 0;
float Timer::time1 = 0;
float Timer::worst1 = 0;
int Timer::count1 = 0;