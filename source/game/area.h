#pragma once
#include "../define.h"
#include "../utils.h"
#include "info.h"
#include "entity.h"
#include "../scene.h"
#include <algorithm>

#define DAY_LENGTH ((6 * 60 * 60))
#define TIME_DAWN  ((10 * 60))
#define RANDOM_PING_COUNT 1

struct Block {
  uchar  id;
  uchar  hp;
  ushort ex;
};

struct Light
  { v2s pos; float scalar; };

class Area
{
    public:

        v2s camera = {0, 0};
    ulong time = 0;
    float lightScalar = 1;
    float lightCubed = 1;
    
        v2s size;
    int floorColor;
    Block outRenderBlock;
    v2s spawnPoint;

        ut::Random rand;

        vec<Block> blocks;
    vec<uptr<Entity>> entities;
    vec<uptr<Light>> lights;
    Player* player;

    public:

        void GenerateIsland(void);

        v2s ToWorld(v2s pos) { return pos + camera - v2s(cl::Width() / 2, cl::Height() / 2); }
    v2s ToScreen(v2s pos) { return pos - camera + v2s(cl::Width() / 2, cl::Height() / 2); }

        float TimeScalar(void) { return time % DAY_LENGTH / DAY_LENGTH; }

        bool Inside(v2s pos) { return pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y; }

        void BlockPut(int id, int x, int y) {
      if (!Inside(v2s(x, y)))
        return;
      auto& info = Info::blocks[id];
      auto& block = blocks[x + y * size.x];
      if (info.light > 0)
        LightCreate(v2s(x, y), info.light),
        block.ex = 'l';
      block.id = id;
      block.hp = Info::blocks[id].hp;
    }

        void BlockPut(string name, int x, int y)
      { BlockPut(Info::Block::ids[name], x, y); }

        void BlockRender(Block& block, v2s spos, v2s wpos)
    {  
            Info::Block& info = Info::blocks[block.id];

            if (info.name == "water") {
        int fg = info.fg, bg = info.bg;
        if (ut::ticks / 4 % 8 == (wpos.x + wpos.y + 9999) % 8)
          std::swap(fg, bg);
        cl::Put(info.character, spos.x, spos.y, fg, bg);
      }

            else if (info.name == "fireplace")
        cl::Put(info.character, spos.x, spos.y,
          ut::ticks / 6 % 2 ? 0xc : 0x4, 0xff);

            else
        cl::Put(info.character, spos.x, spos.y,
          info.fg < 16 ? info.fg : 0xff,
          info.bg < 16 ? info.bg : 0xff);
    }

        void BlockDamage(v2s pos, int damage)
    {  
            Block& block = blocks[pos.x + pos.y * size.x];
      Info::Block& info = Info::blocks[block.id];

            if (damage <= block.hp)
        block.hp -= damage;
      else
        block.hp = 0;

            if (block.hp == 0) {
        if (info.drop != "")
          EntityAdd(new Drop(this, pos, info.drop));
        if (info.light > 0)
          LightRemove(pos);
        block.id = 0;
      }
    }

        Light* LightCreate(v2s pos, float scalar) {
      Light* light = new Light();
      light->pos = pos;
      light->scalar = scalar;
      lights.push_back(uptr<Light>(light));
      return light;
    }

        void LightRemove(v2s pos) {
      for (int i = 0; i < lights.size(); i++)
        if (lights[i]->pos == pos)
          lights.erase(lights.begin() + i);
    }

        void LightRemove(Light* light) {
      for (int i = 0; i < lights.size(); i++)
        if (lights[i].get() == light)
          lights.erase(lights.begin() + i);
    }

        float LightLevel(v2s pos) {
      float result = 0;
      for (auto& light : lights) {
        float sqdist = (v2s(pos.x, pos.y) - light->pos).LengthSq();
        int lightsq = light->scalar * lightCubed;
        
                if (sqdist > lightsq)
          continue;

                result += 1 - sqdist / lightsq;

                if (result > 0.7)
          return result;
      }
      return result;
    }

        void EntityAdd(Entity* entity) {
      entities.push_back(uptr<Entity>(entity));
    }

        void EntityRemove(Entity* entity) {
      for (int i = 0; i < entities.size(); i++) {
        if (entities[i].get() == entity) {
          entities.erase(entities.begin() + i);
          break;
        }
      }
    }

        void Update(void)
    {
            for (int i = 0; i < entities.size(); i++)
        entities[i]->Behave();

            uint daytime = time % DAY_LENGTH;
      if (daytime < TIME_DAWN)
        lightScalar = 2 + 5.0 * daytime / TIME_DAWN;
      else if (daytime >= DAY_LENGTH / 2 && daytime < DAY_LENGTH / 2 + TIME_DAWN)
        lightScalar = 2 + 5 * (1 - scast<float>(daytime - DAY_LENGTH / 2) / TIME_DAWN);

            for (int i = 0; i < RANDOM_PING_COUNT; i++)
        RandomPing();

            time++;

            if (time == DAY_LENGTH)
        Scene::Switch("YouWin");
    }

        void Render(void)
    {
            cl::Fill(' ', 0, 0, cl::Width(), cl::Height(), 0x0, floorColor);

            for (int sx = 0; sx < cl::Width(); sx++) {
        for (int sy = 0; sy < cl::Height(); sy++)
        {
                    v2s wpos = ToWorld(v2s(sx, sy));

                    if (!(wpos.x < 0 || wpos.x >= size.x || wpos.y < 0 || wpos.y >= size.y))
            BlockRender(blocks[wpos.x + wpos.y * size.x], v2s(sx, sy), wpos);
          
                    else
            BlockRender(outRenderBlock, v2s(sx, sy), wpos);
        }
      }

            std::sort(entities.begin(), entities.end(),
        [](auto& a, auto& b) { return a->id < b->id; });
      for (auto& entity : entities)
        entity->Render(
          entity->pos.x - camera.x + cl::Width() / 2,
          entity->pos.y - camera.y + cl::Height() / 2);

            vec<float> lightMask(cl::Width() * cl::Height(), 0);
      lightCubed = lightScalar * lightScalar * lightScalar;

            for (int x = 0; x < cl::Width(); x++)
        for (int y = 0; y < cl::Height(); y++)
          lightMask[x + y * cl::Width()] = LightLevel(v2s(
            x + camera.x - cl::Width() / 2,
            y + camera.y - cl::Height() / 2));

            for (int x = 0; x < cl::Width(); x++) {
        for (int y = 0; y < cl::Height(); y++) {
          float lm = lightMask[x + y * cl::Width()];
          
                    if (lm == 0)
            cl::AttrAt(x, y) = 0;
          
                    else if (lm < 0.35)
            cl::Put('a', x, y, 0x0);
          
                    if (lm < 0.7)
            cl::AttrAt(x, y) =
              (Utils::decay[(cl::AttrAt(x, y) & 0xf0) >> 4] << 4) |
              (Utils::decay[cl::AttrAt(x, y) & 0x0f]);
        }
      }
    }

        void RandomPing(void) 
    {
            int x = rand.Next(size.x);
      int y = rand.Next(size.y);

            if (Inside(v2s(x, y)) && (Info::blocks[blocks[x + y * size.x].id].entCol || Info::blocks[blocks[x + y * size.x].id].projCol)) {
        auto& block = blocks[x + y * size.x];
        auto& info = Info::blocks[block.id];
        if (info.name == "wheatseeds")
          block.id = Info::Block::ids["wheat"];
        else if (info.name == "sapling")
          block.id = rand.Chance(0.1) ? Info::Block::ids["redtree"] : Info::Block::ids["tree"];
        return;
      }

            else if (entities.size() < 128)
      {
                if (rand.Chance(0.05)) {
          if (time > DAY_LENGTH / 2 && LightLevel(v2s(x, y)) == 0)
            EntityAdd(new Enemy(this, v2s(x, y), "demon"));
          return;
        }

                if (time < DAY_LENGTH / 2 && rand.Chance(0.1)) {
          EntityAdd(new Enemy(this, v2s(x, y), "dog"));
          return;
        }

                if (time < DAY_LENGTH / 2 && rand.Chance(0.1)) {
          EntityAdd(new Enemy(this, v2s(x, y), "sheep"));
          return;
        }

                if (rand.Chance(0.02)) {
          if (time > DAY_LENGTH / 2 && LightLevel(v2s(x, y)) == 0)
            EntityAdd(new Enemy(this, v2s(x, y), "priest"));
          return;
        }
      }
    }
};