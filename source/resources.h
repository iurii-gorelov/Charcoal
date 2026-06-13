#pragma once
#include "define.h"
#include "utils.h"

class Res {
 public:
  struct UMFValue {
    string asStr;
    double asNum;
  };

  using UMF = umap<string, UMFValue>;
  using Buf = vec<uchar>;

 private:
  static umap<string, uptr<Res>> resources;

  void* contents;
  string ext;

  Res(string ext) : ext(ext) {}

  static UMF* ParseUMF(vec<uchar>& block) {
    string data(rcast<char*>(block.data()), block.size());

    int l = data.length();
    auto* map = new UMF();

    for (int i = 0; i < l; i++) {
      if (data[i] == '\n' || data[i] == '\r' || data[i] == ' ')
        continue;

      else if (data[i] == '#') {
        while (data[i] != '\n' && data[i] != '\r' && i < l) i++;
        continue;
      }

      int j = i;
      while (data[i] != ':') i++;

      string key = data.substr(j, i - j);

      i++;
      while (data[i] == ' ') i++;
      j = i;

      if (data[i] == '"') {
        i++;
        while (data[i] != '"') {
          if (data[i] == '\\') i++;
          i++;
        }
        (*map)[key].asStr = data.substr(j + 1, i - j - 1);
      }

      else if (data[i] == '-' || data[i] >= '0' && data[i] <= '9') {
        i++;
        while (data[i] >= '0' && data[i] <= '9' || data[i] == '.') i++;
        (*map)[key].asStr = data.substr(j, i - j);
        (*map)[key].asNum = std::stof((*map)[key].asStr);
      }
    }

    return map;
  }

 public:
  ~Res() {
    if (ext == "umf")
      delete rcast<UMF*>(contents);
    else
      delete rcast<Buf*>(contents);
  }

  static void Load(string name, string path) {
    Res* resource = new Res(Utils::GetExt(path));
    vec<uchar> data = Utils::ReadFile(path);

    resources[name] = uptr<Res>(resource);

    if (resource->ext == "umf")
      resource->contents = ParseUMF(data);
    else {
      vec<uchar>* buffer = new vec<uchar>(data);
      resource->contents = buffer;
    }
  }

  static void LoadList(string name) {
    vec<uchar> data = Utils::ReadFile(name);
    auto* map = ParseUMF(data);

    for (auto& pair : *map) Load(pair.first, pair.second.asStr);

    delete map;
  }

  template <typename T>
  static T Get(string name) {
    return *rcast<T*>(resources[name]->contents);
  }
};