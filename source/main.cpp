#include <windows.h>
#include <iostream>
#include <conlib.h>

namespace CL = ConLib;

int main() {
  CL::Initialize();
  CL::Resize(32, 16);
  CL::NoResize();
  CL::NoSelect();
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      CL::Put(i * 2, j, 'A' + i + j, i, j);
      CL::Put(i * 2 + 1, j, 'A' + i + j, i, j);
    }
  }
  CL::ApplyBuffer();
  std::cin.get();
  CL::Destroy();
}