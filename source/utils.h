#pragma once
#include "define.h"
#include <cmath>

// short 2d vector
struct v2s
{
  // properties
  short x, y;

  // constructors
  v2s(void) {}
  v2s(short x, short y) : x(x), y(y) {}

  // functions
  int Area(void) { return x * y; }
  double Length(void) { return std::sqrt(x * x + y * y); }
  double LengthSq(void) { return x * x + y * y; }
  v2s Normalize(void) {
    return v2s(
      x == 0 ? 0 : x > 0 ? 1 : -1,
      y == 0 ? 0 : y > 0 ? 1 : -1);
  }

  // operators
  v2s operator+(v2s other)
    { return v2s(x + other.x, y + other.y); }
  v2s operator-(v2s other) 
    { return v2s(x - other.x, y - other.y); }
  v2s operator*(int scalar)
    { return v2s(x * scalar, y * scalar); }

  // comparison
  bool operator==(v2s other)
    { return x == other.x && y == other.y; }
};

// namespace for utility functions
namespace Utils
{
  // ticks
  extern int ticks;
  extern int decay[16];

  // released in main.cpp
  void Quit(void);

  // read a file into a uchar array
  vec<uchar> ReadFile(string name);

  // get an extension of a file
  string GetExt(string path);

  // char hex to int
  int Chtoi(char c);

  // splitter
  std::pair<string, string> Split(string str, char delim);

  // random class
  class Random {
    public:

      // property and constructors
      int seed;
      Random() : seed(0) {}
      Random(int seed) : seed(seed) {}

      // get a random number
      int Next()
        { return ((seed = (seed * 214013 + 2531011)) & 0xfffff) >> 4; }

      // get a random number from 0 to max (exclusive)
      int Next(int max)
        { return Next() % max; }

      // get a random number from min to max (exclusive)
      int Next(int min, int max)
        { return min + Next() % (max - min); }

      // chance of success
      bool Chance(float chance)
        { return Next() / scast<float>(0xffff) < chance; }

      // random vector
      v2s SqrPos(int radius)
        { return v2s(Next(-radius, radius), Next(-radius, radius)); }
  };

  // timer class
  struct Timer 
  {
    // properties and constructor
    int max, cur;
    Timer() : max(1), cur(1) {}
    Timer(int max) : cur(max), max(max) {}

    // reset the timer
    void Reset(void) { cur = max; }

    // check the timer
    bool Check(bool update = true) {
      if (cur == 0)
        return true;
      else
        cur--;
      return false;
    }
  };
}

// alias
namespace ut = Utils;