#pragma once
#include "../define.h"
#include "../utils.h"
#include "../resources.h"

// all objects are this wtf
class Object {
  public:

    // info class
    struct Info {
      uchar ciChar;
      uchar cifg;
      uchar cibg;
    };

    // id constants
    static const uchar AIR = 0;
    static const uchar WATER = 1;
    static const uchar GRASS = 2;
    static const uchar TREE = 3;
    static const uchar ROCK = 4;
    static const uchar FLOWER = 5;
    static const uchar FIREPLACE = 6;
    static const uchar PLAYER = 7;

    // info map
    static vec<Info> info;

    // very basic properties
    uchar id;
    uchar health;

    // parse object info
    static void ParseInfo(Res::UMF umf)
    {
      // resize the info
      info.resize(256);

      // loop through all the fields
      for (auto& field : umf)
      {
        // separate id and property
        int slasha = field.first.find('/');
        int id = std::stoi(field.first.substr(0, slasha));
        string property = field.first.substr(slasha + 1);
        string sval = field.second.asStr;
        double dval = field.second.asNum;

        // set the char information
        if (property == "charinfo")
          info[id].ciChar = sval[0],
          info[id].cifg = Utils::Chtoi(sval[1]),
          info[id].cibg = Utils::Chtoi(sval[2]);
      }
    }
};

// the block class
class Block : public Object {
  public:

    // render the block
    void Render(int x, int y) {
      Object::Info& info = Object::info[id];
      cl::Put(info.ciChar, x, y, info.cifg < 16 ? info.cifg : 0xff, info.cibg < 16 ? info.cibg : 0xff);
    }

    // special renderer for water
    static void RenderWater(int x, int y, int rx, int ry) {
      int phase = ut::ticks / 4 % 8;
      int fg = Object::info[Object::WATER].cifg;
      int bg = Object::info[Object::WATER].cibg;
      if (phase == (rx + ry + 9999) % 8)
        std::swap(fg, bg);
      cl::Put(Object::info[Object::WATER].ciChar, x, y, fg, bg);
    }
};

// forward declaration
class Area;

// class for entities
class Entity : public Object
{
  // almost fully public
  public:

    // render the entity
    void Render(int x, int y) {
      Object::Info& info = Object::info[id];
      cl::Put(info.ciChar, x, y, info.cifg < 16 ? info.cifg : 0xff, info.cibg < 16 ? info.cibg : 0xff);
    }

    // constructor
    Entity(uchar idt) { id = idt; }

    // has coords
    v2s pos;

    // behaviour
    virtual void Behave(Area* area) { }
};

// player object
class Player : public Entity
{
  // private stuff
  private:

    // timers
    ut::Timer moveTimer;
    std::pair<v2s, char>* light;

  // public stuff
  public:

    // constructor
    Player(void) : Entity(Object::PLAYER), moveTimer(4) {}

    // override the behaviour
    void Behave(Area* area) override;
};