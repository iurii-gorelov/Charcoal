#pragma once
#include "../define.h"
#include "../utils.h"
#include "../resources.h"
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
    virtual ~Entity() {}
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
    virtual void Death(void);
};

// player object
class Player : public Entity
{
  // private stuff
  private:

    // timers
    ut::Timer moveTimer;
    ut::Timer useTimer;
    Light* light;

  // public stuff
  public:

    // constructor
    Player(Area* area, v2s pos);

    // override the behaviour
    void Behave(void) override;
    void Death(void) override;

    // it's own behavior
    void UseItem(v2s dir);
    void SelectItem(int slot);
};

// projectile class
class Projectile : public Entity
{
  // data
  private:
    Entity* owner;
    int power;
    v2s direction;

  // public
  public:
    
    // constructor
    Projectile(Area* area, v2s pos, v2s dir, Entity* own, Info::Item& wand)  :
      Entity(area, pos, Info::Entity::ids["projectile"]),
      owner(own),
      power(wand.wandPower),
      direction(dir) {
      health = wand.wandDamage;
    }

    // override the behaviour
    void Behave(void) override;
};