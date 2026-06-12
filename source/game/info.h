#pragma once
#include "../define.h"
#include "../resources.h"

// namespace for all the info classes
namespace Info
{
  // forward declarations
  struct Block;
  struct Entity;
  struct Item;

  // containers
  extern vec<Block>  blocks;
  extern vec<Entity> entities;
  extern vec<Item>   items;

  // class for block infos
  struct Block
  {
    // id map
    static umap<string, uchar> ids;

    // the properties
    string name;
    uchar hp;

    // collision
    bool entCol;
    bool projCol;
    
    // char info
    char character;
    char fg;
    char bg;

    // the parser
    static void Parse(Res::UMF umf)
    {
      // resize both
      blocks.resize(256);
      ids.reserve(256);

      // parse the fields
      for (auto& field : umf)
      {
        // separate id and title
        std::pair<string, string> idTitle =
          Utils::Split(field.first, '/');

        // get the id
        uchar id = std::stoi(idTitle.first);
        string sval = field.second.asStr;
        double dval = field.second.asNum;

        // title
        if (idTitle.second == "name")
          ids[sval] = id,
          blocks[id].name = sval;

        // character info
        else if (idTitle.second == "charinfo")
          blocks[id].character = sval[0],
          blocks[id].fg = Utils::Chtoi(sval[1]),
          blocks[id].bg = Utils::Chtoi(sval[2]);

        // collision data
        else if (idTitle.second == "collision")
          blocks[id].entCol = sval[0] == '1',
          blocks[id].projCol = sval[1] == '1';

        // health
        else if (idTitle.second == "health")
          blocks[id].hp = dval;
      }
    }
  };

  // class for entity infos
  struct Entity
  {
    // id map
    static umap<string, uchar> ids;

    // the properties
    string name;
    uchar hp;

    // collision
    bool collidable;
    
    // char info
    char character;
    char fgcolor;

    // the parser
    static void Parse(Res::UMF umf)
    {
      // resize both
      entities.resize(256);
      ids.reserve(256);

      // parse the fields
      for (auto& field : umf)
      {
        // separate id and title
        std::pair<string, string> idTitle =
          Utils::Split(field.first, '/');

        // get the id
        uchar id = std::stoi(idTitle.first);
        string sval = field.second.asStr;
        double dval = field.second.asNum;

        // title
        if (idTitle.second == "name")
          ids[sval] = id,
          entities[id].name = sval;

        // character info
        else if (idTitle.second == "charinfo")
          entities[id].character = sval[0],
          entities[id].fgcolor = Utils::Chtoi(sval[1]);

        // collision data
        else if (idTitle.second == "collision")
          entities[id].collidable = sval[0] == '1';

        // health
        else if (idTitle.second == "health")
          entities[id].hp = dval;
      }
    }
  };

  // class for item infos
  struct Item
  {
    // id map
    static umap<string, uchar> ids;
    
    // the properties
    string title;
    string name;
    uchar type;
    int useTimer;

    // wand properties
    double wandDamage;
    double wandPower;
    double wandProjs;

    // the parser
    static void Parse(Res::UMF umf)
    {
      // resize both
      items.resize(256);
      ids.reserve(256);

      // parse the fields
      for (auto& field : umf)
      {
        // separate id and title
        std::pair<string, string> idTitle =
          Utils::Split(field.first, '/');

        // get the id
        uchar id = std::stoi(idTitle.first);
        string sval = field.second.asStr;
        double dval = field.second.asNum;

        // title
        if (idTitle.second == "name")
          ids[sval] = id,
          items[id].name = sval;

        // title and type
        else if (idTitle.second == "title")
          items[id].title = sval;
        else if (idTitle.second == "type")
          items[id].type = dval;
        else if (idTitle.second == "usetimer")
          items[id].useTimer = dval;

        // wand stuff
        else if (idTitle.second == "wand/damage")
          items[id].wandDamage = dval;
        else if (idTitle.second == "wand/power")
          items[id].wandPower = dval;
        else if (idTitle.second == "wand/projs")
          items[id].wandProjs = dval;
      }
    }
  };
}