#pragma once
#include "define.h"
#include <cmath>

struct v2s
{
    short x, y;

    v2s(void) {}
  v2s(short x, short y) : x(x), y(y) {}

    int Area(void) { return x * y; }
  double Length(void) { return std::sqrt(x * x + y * y); }
  double LengthSq(void) { return x * x + y * y; }
  v2s Normalize(void) {
    return v2s(
      x == 0 ? 0 : x > 0 ? 1 : -1,
      y == 0 ? 0 : y > 0 ? 1 : -1);
  }

    v2s operator+(v2s other)
    { return v2s(x + other.x, y + other.y); }
  v2s operator-(v2s other) 
    { return v2s(x - other.x, y - other.y); }
  v2s operator*(int scalar)
    { return v2s(x * scalar, y * scalar); }

    bool operator==(v2s other)
    { return x == other.x && y == other.y; }
};

namespace Utils
{
    extern int ticks;
  extern int decay[16];

    void Quit(void);

    vec<uchar> ReadFile(string name);

    string GetExt(string path);

    int Chtoi(char c);

    std::pair<string, string> Split(string str, char delim);

    class Random {
    public:

            int seed;
      Random() : seed(0) {}
      Random(int seed) : seed(seed) {}

            int Next()
        { return ((seed = (seed * 214013 + 2531011)) & 0xfffff) >> 4; }

            int Next(int max)
        { return Next() % max; }

            int Next(int min, int max)
        { return min + Next() % (max - min); }

            bool Chance(float chance)
        { return Next() / scast<float>(0xffff) < chance; }

            v2s SqrPos(int radius)
        { return v2s(Next(-radius, radius + 1), Next(-radius, radius + 1)); }
  };

    struct Timer 
  {
        int max, cur;
    Timer() : max(1), cur(1) {}
    Timer(int max) : cur(max), max(max) {}

        void Reset(void) { cur = max; }

        bool Check(bool update = true) {
      if (cur == 0)
        return true;
      else
        cur--;
      return false;
    }
  };
}

namespace ut = Utils;