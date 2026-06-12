#pragma once
#include "define.h"
#include "utils.h"

// class that manages resources
class Res
{
  // type names
  public:

    // umf value (replace struct for union)
    struct UMFValue {
      string asStr;
      double asNum;
    };

    using UMF = umap<string, UMFValue>;
    using Buf = vec<uchar>;

  // private properties
  private:

    // all the resources
    static umap<string, uptr<Res>> resources;
  
    // contents of the resource
    void* contents;
    string ext;

    // constructor
    Res(string ext) : ext(ext) {}

    // unsigned map format parser
    static UMF* ParseUMF(vec<uchar>& block)
    {
      // as string
      string data(rcast<char*>(block.data()), block.size());

      // prepare for parsing
      int l = data.length();
      auto* map = new UMF();

      // loop through all the characters
      for (int i = 0; i < l; i++)
      {
        // is it a useless character?
        if (data[i] == '\n' || data[i] == '\r' || data[i] == ' ')
          continue;

        // if it is a comment, skip it
        else if (data[i] == '#') {
          while (data[i] != '\n' && data[i] != '\r' && i < l)
            i++;
          continue;
        }
        
        // get key until the colon
        int j = i;
        while (data[i] != ':')
          i++;

        // set key
        string key = data.substr(j, i - j);

        // skip colon and spaces after it
        i++;
        while (data[i] == ' ')
          i++;
        j = i;

        // get string value
        if (data[i] == '"') {
          i++;
          while (data[i] != '"') {
            if (data[i] == '\\')
              i++;
            i++;
          }
          (*map)[key].asStr = data.substr(j + 1, i - j - 1);
        }

        // get integer value
        else if (data[i] == '-' || data[i] >= '0' && data[i] <= '9') {
          i++;
          while (data[i] >= '0' && data[i] <= '9' || data[i] == '.')
            i++;
          (*map)[key].asStr = data.substr(j, i - j);
          (*map)[key].asNum = std::stof((*map)[key].asStr);
        }
      }

      // return the map
      return map;
    }

  // public functions
  public:
    
    // destruct
    ~Res() {
      if (ext == "umf")
        delete rcast<UMF*>(contents);
      else
        delete rcast<Buf*>(contents);
    }

    // load a single resource
    static void Load(string name, string path)
    {
      // create resource and read the file
      Res* resource = new Res(Utils::GetExt(path));
      vec<uchar> data = Utils::ReadFile(path);

      // register the resource
      resources[name] = uptr<Res>(resource);

      // parse the file
      if (resource->ext == "umf")
        resource->contents = ParseUMF(data);
      else {
        vec<uchar>* buffer = new vec<uchar>(data);
        resource->contents = buffer;
      }
    }

    // load by the map file
    static void LoadList(string name)
    {
      // read the file
      vec<uchar> data = Utils::ReadFile(name);
      auto* map = ParseUMF(data);

      // load all the resources
      for (auto& pair : *map)
        Load(pair.first, pair.second.asStr);

      // map
      delete map;
    }

    // get a resource
    template <typename T>
    static T Get(string name) {
      return *rcast<T*>(resources[name]->contents);
    }
};