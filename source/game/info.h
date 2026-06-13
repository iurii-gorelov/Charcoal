#pragma once
#include "../define.h"
#include "../resources.h"

namespace Info
{
    struct Block;
  struct Entity;
  struct Item;

    extern vec<Block>  blocks;
  extern vec<Entity> entities;
  extern vec<Item>   items;

    struct Block
  {
        static umap<string, uchar> ids;

        string name;
    string drop = "";
    uchar hp;
    uchar light = 0;

        bool entCol;
    bool projCol;
    
        char character;
    char fg;
    char bg;

        static void Parse(Res::UMF umf)
    {
            blocks.resize(256);
      ids.reserve(256);

            for (auto& field : umf)
      {
                std::pair<string, string> idTitle =
          Utils::Split(field.first, '/');

                uchar id = std::stoi(idTitle.first);
        string sval = field.second.asStr;
        double dval = field.second.asNum;

                if (idTitle.second == "name")
          ids[sval] = id,
          blocks[id].name = sval;

                else if (idTitle.second == "charinfo")
          blocks[id].character = sval[0],
          blocks[id].fg = Utils::Chtoi(sval[1]),
          blocks[id].bg = Utils::Chtoi(sval[2]);

                else if (idTitle.second == "collision")
          blocks[id].entCol = sval[0] == '1',
          blocks[id].projCol = sval[1] == '1';

                else if (idTitle.second == "health")
          blocks[id].hp = dval;

                else if (idTitle.second == "drop")
          blocks[id].drop = sval;

                else if (idTitle.second == "light")
          blocks[id].light = dval;
      }
    }
  };

    struct Entity
  {
        static umap<string, uchar> ids;

        string name;
    uchar hp;
    string drop = "";

        bool collidable;
    
        char character;
    char fgcolor;

        int damage;
    int speed;

        static void Parse(Res::UMF umf)
    {
            entities.resize(256);
      ids.reserve(256);

            for (auto& field : umf)
      {
                std::pair<string, string> idTitle =
          Utils::Split(field.first, '/');

                uchar id = std::stoi(idTitle.first);
        string sval = field.second.asStr;
        double dval = field.second.asNum;

                if (idTitle.second == "name")
          ids[sval] = id,
          entities[id].name = sval;

                else if (idTitle.second == "charinfo")
          entities[id].character = sval[0],
          entities[id].fgcolor = Utils::Chtoi(sval[1]);

                else if (idTitle.second == "collision")
          entities[id].collidable = sval[0] == '1';

                else if (idTitle.second == "health")
          entities[id].hp = dval;

                else if (idTitle.second == "damage")
          entities[id].damage = dval;
        else if (idTitle.second == "speed")
          entities[id].speed = dval;
        else if (idTitle.second == "drop")
          entities[id].drop = sval;

      }
    }
  };

    struct Item
  {
        static umap<string, uchar> ids;
    
        string title;
    string name;
    uchar type;
    int useTimer = 1;

        double wandDamage;
    double wandPower;
    double wandProjs;

        int food;

        static void Parse(Res::UMF umf)
    {
            items.resize(256);
      ids.reserve(256);

            for (auto& field : umf)
      {
                std::pair<string, string> idTitle =
          Utils::Split(field.first, '/');

                uchar id = std::stoi(idTitle.first);
        string sval = field.second.asStr;
        double dval = field.second.asNum;

                if (idTitle.second == "name")
          ids[sval] = id,
          items[id].name = sval;

                else if (idTitle.second == "title")
          items[id].title = sval;
        else if (idTitle.second == "type")
          items[id].type = dval;
        else if (idTitle.second == "usetimer")
          items[id].useTimer = dval;

                else if (idTitle.second == "wand/damage")
          items[id].wandDamage = dval;
        else if (idTitle.second == "wand/power")
          items[id].wandPower = dval;
        else if (idTitle.second == "wand/projs")
          items[id].wandProjs = dval;

                else if (idTitle.second == "food")
          items[id].food = dval;
      }
    }
  };
}