#include <conlib.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
using ilong = long long;

using string = std::string;

template <typename T>
using uptr = std::unique_ptr<T>;

using callback = void (*)(void);

template <typename T>
using vec = std::vector<T>;
template <typename I, typename T>
using map = std::map<I, T>;
template <typename I, typename T>
using umap = std::unordered_map<I, T>;
template <typename I, typename T>
using pair = std::pair<I, T>;

#define rcast reinterpret_cast
#define scast static_cast
#define dcast dynamic_cast

#define singleton(type, name) \
  static type& name() {       \
    static type name##_;      \
    return name##_;           \
  }

#define titledbg(title) \
  cl::Title(title);     \
  cl::Sleep(1000);