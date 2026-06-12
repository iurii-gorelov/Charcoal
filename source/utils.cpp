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
  file.read(rcast<char*>(buffer.data()), buffer.size());
  file.close();
  return buffer;
}

// get an extension of a file
string Utils::GetExt(string path) {
  int dot = path.find_last_of('.');
  return path.substr(dot + 1);
}

// char hex to int (only lowercase)
int Utils::Chtoi(char c) {
  return c - (c >= '0' && c <= '9' ? '0' : 'W');
}

// color decay table
int Utils::decay[16] {
  0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7
};

// tiiiiiiicks
int Utils::ticks = 0;

std::pair<string, string> Utils::Split(string str, char delim)
{
  int pos = str.find(delim);
  if (pos == string::npos)
    return std::make_pair(str, "");
  return std::make_pair(str.substr(0, pos), str.substr(pos + 1));
}