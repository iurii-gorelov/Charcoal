#include "utils.h"
#include <fstream>
#include <sstream>

// read whole file
std::vector<uchar> Utils::ReadFile(string name)
{
  // open file
  std::ifstream file(name,
    std::ios::in | std::ios::binary | std::ios::ate);
  
  // create buffer
  vec<uchar> buffer;
  buffer.resize(file.tellg());

  // read the file and return
  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
  file.close();
  return buffer;
}

// get an extension of a file
string Utils::GetExt(string path) {
  int dot = path.find_last_of('.');
  return path.substr(dot + 1);
}