#pragma once
#include "../define.h"
#include "../utils.h"
#include "info.h"
#include "entity.h"

// constants
#define DAY_LENGTH ((2 * 60 * 60))
#define TIME_DAWN  ((10 * 60))

// block object
struct Block {
  uchar  id;
  uchar  hp;
  ushort ex;
};

// light object
struct Light
  { v2s pos; float scalar; };

// class for some kind of location with objects etc
class Area
{
  // area properties
  public:

    // current state
    v2s camera = {0, 0};
    ulong time = 3600;
    float lightScalar = 1;
    float lightCubed = 1;
    
    // generation properties
    v2s size;
    int floorColor;
    Block outRenderBlock;
    v2s spawnPoint;

    // its own random
    ut::Random rand;

    // blocks and entities
    vec<Block> blocks;
    vec<uptr<Entity>> entities;
    vec<uptr<Light>> lights;
    Player* player;

  // methods
  public:

    // all generators
    void GenerateIsland(void);

    // camera transform
    v2s ToWorld(v2s pos) { return pos + camera - v2s(cl::Width() / 2, cl::Height() / 2); }
    v2s ToScreen(v2s pos) { return pos - camera + v2s(cl::Width() / 2, cl::Height() / 2); }

    // get time scalar
    float TimeScalar(void) { return time % DAY_LENGTH / DAY_LENGTH; }

    // check if inside the area
    bool Inside(v2s pos) { return pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y; }

    // put a block
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

    // by name
    void BlockPut(string name, int x, int y)
      { BlockPut(Info::Block::ids[name], x, y); }

    // render the block
    void BlockRender(Block& block, v2s spos, v2s wpos)
    {  
      // get the info
      Info::Block& info = Info::blocks[block.id];

      // render the water
      if (info.name == "water") {
        int fg = info.fg, bg = info.bg;
        if (ut::ticks / 4 % 8 == (wpos.x + wpos.y + 9999) % 8)
          std::swap(fg, bg);
        cl::Put(info.character, spos.x, spos.y, fg, bg);
      }

      // render the fireplace
      else if (info.name == "fireplace")
        cl::Put(info.character, spos.x, spos.y,
          ut::ticks / 6 % 2 ? 0xc : 0x4, 0xff);

      // render usual block
      else
        cl::Put(info.character, spos.x, spos.y,
          info.fg < 16 ? info.fg : 0xff,
          info.bg < 16 ? info.bg : 0xff);
    }

    // damage block
    void BlockDamage(v2s pos, int damage)
    {  
      // get the block
      Block& block = blocks[pos.x + pos.y * size.x];
      Info::Block& info = Info::blocks[block.id];

      // damage the block
      if (damage <= block.hp)
        block.hp -= damage;
      else
        block.hp = 0;

      // if the block is destroyed
      if (block.hp == 0) {
        if (info.drop != "")
          EntityAdd(new Drop(this, pos, info.drop));
        if (info.light > 0)
          LightRemove(pos);
        block.id = 0;
      }
    }

    // create a light
    Light* LightCreate(v2s pos, float scalar) {
      Light* light = new Light();
      light->pos = pos;
      light->scalar = scalar;
      lights.push_back(uptr<Light>(light));
      return light;
    }

    // remove a light
    void LightRemove(v2s pos) {
      for (int i = 0; i < lights.size(); i++)
        if (lights[i]->pos == pos)
          lights.erase(lights.begin() + i);
    }

    // remove a light by addr
    void LightRemove(Light* light) {
      for (int i = 0; i < lights.size(); i++)
        if (lights[i].get() == light)
          lights.erase(lights.begin() + i);
    }

    // get light level at pos
    float LightLevel(v2s pos) {
      float result = 0;
      for (auto& light : lights) {
        float sqdist = (v2s(pos.x, pos.y) - light->pos).LengthSq();
        int lightsq = light->scalar * lightCubed;
        
        // break if too far
        if (sqdist > lightsq)
          continue;

        // add to the mask
        result += 1 - sqdist / lightsq;

        // optimization
        if (result > 0.7)
          return result;
      }
      return result;
    }

    // spawn an entity
    void EntityAdd(Entity* entity) {
      entities.push_back(uptr<Entity>(entity));
    }

    // remove an entity
    void EntityRemove(Entity* entity) {
      for (int i = 0; i < entities.size(); i++) {
        if (entities[i].get() == entity) {
          entities.erase(entities.begin() + i);
          break;
        }
      }
    }

    // update current area
    void Update(void)
    {
      // update all the entities
      for (int i = 0; i < entities.size(); i++)
        entities[i]->Behave();

      // dawn and sunset times
      uint daytime = time % DAY_LENGTH;
      if (daytime < TIME_DAWN)
        lightScalar = 2 + 5.0 * daytime / TIME_DAWN;
      else if (daytime >= DAY_LENGTH / 2 && daytime < DAY_LENGTH / 2 + TIME_DAWN)
        lightScalar = 2 + 5 * (1 - scast<float>(daytime - DAY_LENGTH / 2) / TIME_DAWN);

      // increment the time
      time++;
    }

    // renderer the area
    void Render(void)
    {
      // fill with default color
      cl::Fill(' ', 0, 0, cl::Width(), cl::Height(), 0x0, floorColor);

      // render the blocks
      for (int sx = 0; sx < cl::Width(); sx++) {
        for (int sy = 0; sy < cl::Height(); sy++)
        {
          // get the world coords
          v2s wpos = ToWorld(v2s(sx, sy));

          // draw the block
          if (!(wpos.x < 0 || wpos.x >= size.x || wpos.y < 0 || wpos.y >= size.y))
            BlockRender(blocks[wpos.x + wpos.y * size.x], v2s(sx, sy), wpos);
          
          // outside render mode
          else
            BlockRender(outRenderBlock, v2s(sx, sy), wpos);
        }
      }

      // render all the entities
      for (auto& entity : entities)
        entity->Render(
          entity->pos.x - camera.x + cl::Width() / 2,
          entity->pos.y - camera.y + cl::Height() / 2);

      // render light mask
      vec<float> lightMask(cl::Width() * cl::Height(), 0);
      lightCubed = lightScalar * lightScalar * lightScalar;

      // for each point on the screen
      for (int x = 0; x < cl::Width(); x++)
        for (int y = 0; y < cl::Height(); y++)
          lightMask[x + y * cl::Width()] = LightLevel(v2s(
            x + camera.x - cl::Width() / 2,
            y + camera.y - cl::Height() / 2));

      // write the light mask
      for (int x = 0; x < cl::Width(); x++) {
        for (int y = 0; y < cl::Height(); y++) {
          float lm = lightMask[x + y * cl::Width()];
          
          // pitch black
          if (lm == 0)
            cl::AttrAt(x, y) = 0;
          
          // pre pitch black state
          else if (lm < 0.35)
            cl::Put('a', x, y, 0x0);
          
          // color decay (works as for self and as for the pre pitch black state)
          if (lm < 0.7)
            cl::AttrAt(x, y) =
              (Utils::decay[(cl::AttrAt(x, y) & 0xf0) >> 4] << 4) |
              (Utils::decay[cl::AttrAt(x, y) & 0x0f]);
        }
      }
    }
};