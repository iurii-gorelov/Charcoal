#pragma once
#include "../define.h"
#include "../utils.h"
#include "../resources.h"
#include "info.h"
#include <sstream>

// forward declaration
class Area;
struct Light;

// all entities
class Entity
{
  // private
  protected:
    Area* area;

  // very public
  public:

    // properties
    v2s   pos;
    uchar id;
    short health;

    // constructor
    virtual ~Entity() {}
    Entity(void) = default;
    Entity(Area* area, v2s pos, uchar id) : area(area), pos(pos), id(id) {
      Info::Entity& info = Info::entities[id];
      health = info.hp;
    }

    // default render for entity
    void DefaultRender(int sx, int sy) {
      Info::Entity& info = Info::entities[id];
      cl::Put(info.character, sx, sy, info.fgcolor);
    }

    // default render function
    virtual void Render(int sx, int sy)
      { DefaultRender(sx, sy); }

    // try to move
    void TryMove(int dirX, int dirY);
    void ApplyDamage(int damage);
    virtual void Behave(void) = 0;
    virtual void Death(void);
};

// drop class
class Drop : public Entity {
  
  // properties
  private:
    vec<uchar> drops;

  // public
  public:

    // constructor
    Drop(Area* area, v2s pos, string drop) : Entity(area, pos, Info::Entity::ids["drop"]) {
      std::stringstream ss(drop);
      while (ss.good()) {
        string item;
        ss >> item;
        drops.push_back(Info::Item::ids[item]);
      }
    }

    // override the behaviour
    void Behave(void) override;
    void Render(int sx, int sy) override;
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
    int maxHealth = 10;

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

// enemy class
class Enemy : public Entity
{
  // data
  private:
    ut::Timer moveTimer;
    ut::Timer attackTimer;

  // public
  public:

    // constructor
    Enemy(Area* area, v2s pos, string name);

    // override the behaviour
    void Behave(void) override;
    void Death(void) override;
};

// projectile class
class Projectile : public Entity
{
  // data
  private:
    Entity* owner;
    int power;
    v2s direction;
    Light* light;
    bool players;

  // public
  public:
    
    // constructor
    Projectile(Area* area, v2s pos, v2s dir, Entity* own, Info::Item& wand);

    // override the behaviour
    void Behave(void) override;
    void Death(void) override;
};