// all default important includes
#include <memory>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <variant>

// main library
#include <conlib.h>

// define the basic types
using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
using ilong = long long;

// alias for string
using string = std::string;

// alias for unique_ptr
template <typename T> using
  uptr = std::unique_ptr<T>;

// alias for callbacks
using callback = void(*)(void);

// alias for containers
template <typename T> using
  vec = std::vector<T>;
template <typename I, typename T> using
  map = std::map<I, T>;
template <typename I, typename T> using
  umap = std::unordered_map<I, T>;

// no more long casts
#define rcast reinterpret_cast
#define scast static_cast
#define dcast dynamic_cast

// singleton shortcut
#define singleton(type, name) \
  static type& name() \
    { static type name##_; return name##_; }