#pragma once
#include "../define.h"
#include "../utils.h"
#include "../resources.h"
#include "item.h"
#include "info.h"

// forward declaration
class Area;
struct Light;

// all entities
class Entity
{ 
  // very public
  public:

    // properties
    Area* area;
    v2s   pos;
    uchar id;
    uchar health;

    // constructor
    Entity(void) = default;
    Entity(Area* area, v2s pos, uchar id) : area(area), pos(pos), id(id) {
      Info::Entity& info = Info::entities[id];
      health = info.hp;
    }

    // default render function
    virtual void Render(int sx, int sy) {
      Info::Entity& info = Info::entities[id];
      cl::Put(info.character, sx, sy, info.fgcolor);
    }

    // try to move
    void TryMove(int dirX, int dirY);
    void ApplyDamage(int damage);
    virtual void Behave(void) = 0;
    virtual void Death(void) = 0;
};

// player object
class Player : public Entity
{
  // private stuff
  private:

    // timers
    ut::Timer moveTimer;
    Light* light;

  // public stuff
  public:

    // constructor
    Player(Area* area, v2s pos);

    // override the behaviour
    void Behave(void) override;
    void Death(void) override;
};

class Projectile : public Entity
{
  // data
  private:
    Entity* owner;
    int power;

  // public
  public:
    
    // Projectile(Area* area, v2s pos, Entity* owner, Info::Item& wand);

  
};