#pragma once
#include "../define.h"
#include "../utils.h"
#include "../resources.h"
#include "info.h"
#include <sstream>

class Area;
struct Light;

class Entity
{
    protected:
    Area* area;

    public:

        v2s   pos;
    uchar id;
    short health;

        virtual ~Entity() {}
    Entity(void) = default;
    Entity(Area* area, v2s pos, uchar id) : area(area), pos(pos), id(id) {
      Info::Entity& info = Info::entities[id];
      health = info.hp;
    }

        void DefaultRender(int sx, int sy) {
      Info::Entity& info = Info::entities[id];
      cl::Put(info.character, sx, sy, info.fgcolor);
    }

        virtual void Render(int sx, int sy)
      { DefaultRender(sx, sy); }

        void TryMove(int dirX, int dirY);
    void ApplyDamage(int damage);
    virtual void Behave(void) = 0;
    virtual void Death(void);
};

class Drop : public Entity {
  
    private:
    vec<uchar> drops;

    public:

        Drop(Area* area, v2s pos, string drop) : Entity(area, pos, Info::Entity::ids["drop"]) {
      std::stringstream ss(drop);
      while (ss.good()) {
        string item;
        ss >> item;
        drops.push_back(Info::Item::ids[item]);
      }
    }

        void Behave(void) override;
    void Render(int sx, int sy) override;
};

class Player : public Entity
{
    private:

        ut::Timer moveTimer;
    ut::Timer useTimer;
    Light* light;
    int maxHealth = 10;

    public:

        Player(Area* area, v2s pos);

        void Behave(void) override;
    void Death(void) override;

        void UseItem(v2s dir);
    void SelectItem(int slot);
};

class Enemy : public Entity
{
    private:
    ut::Timer moveTimer;
    ut::Timer attackTimer;

    public:

        Enemy(Area* area, v2s pos, string name);

        void Behave(void) override;
    void Death(void) override;
};

class Projectile : public Entity
{
    private:
    Entity* owner;
    int power;
    v2s direction;
    Light* light;
    bool players;

    public:
    
        Projectile(Area* area, v2s pos, v2s dir, Entity* own, Info::Item& wand);

        void Behave(void) override;
    void Death(void) override;
};