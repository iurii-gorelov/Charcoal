#include <windows.h>
#include <iostream>
#include <conlib.h>


int main() {
  ConLib::Initialize();
  ConLib::Resize(32, 16);
  ConLib::NoResize();
  ConLib::NoSelect();
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      ConLib::Put(i * 2, j, 'A' + i + j, i, j);
      ConLib::Put(i * 2 + 1, j, 'A' + i + j, i, j);
    }
  }
  ConLib::ApplyBuffer();
  std::cin.get();
  ConLib::Destroy();
}