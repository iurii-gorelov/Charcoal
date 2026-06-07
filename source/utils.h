#pragma once
#include "define.h"

// namespace for utility functions
namespace Utils
{
  // read a file into a uchar array
  vec<uchar> ReadFile(string name);

  // get an extension of a file
  string GetExt(string path);
}