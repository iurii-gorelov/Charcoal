#pragma once
#include "../define.h"
#include "../utils.h"
#include "object.h"

// constants
#define DAY_LENGTH ((10 * 60 * 60))

// class for some kind of location with objects etc
class Area
{
  // area properties
  public:

    // current state
    v2s camera = {0, 0};
    ulong time = 0;
    float lightScalar = 10000;
    
    // generation properties
    v2s size;
    int floorColor;
    int outsideRender;
    v2s spawnPoint;

    // its own random
    ut::Random rand;

    // blocks and entities
    vec<Block> blocks;
    vec<uptr<Entity>> entities;
    vec<std::pair<v2s, char>> lights;

  // methods
  public:

    // constants
    static const int OR_BLACK = 0;
    static const int OR_SEA = 1;

    // put a block
    void Place(int id, int x, int y)
    {
      // check bounds
      if (x < 0 || x >= size.x || y < 0 || y >= size.y)
        return;

      // set the block
      blocks[x + y * size.x].id = id;
    }

    // spawn an entity
    void Spawn(Entity* ent, int x, int y) {
      ent->pos = v2s(x, y);
      entities.push_back(uptr<Entity>(ent));
    }

    // block link
    Block& BlockAt(int x, int y) {
      return blocks[x + y * size.x];
    }

    // update current area
    void Update(void)
    {
      // update all the entities
      for (auto& entity : entities) {
        entity->Behave(this);
      }

      // increment the time
      time++;
    }

    // renderer the area
    void Render(void)
    {
      // fill with default color
      cl::Fill(' ', 0, 0, cl::Width(), cl::Height(), 0x0, floorColor);

      // render the blocks
      for (int i = 0; i < cl::Width() * cl::Height(); i++)
      {
        // get the coords
        int x = i % cl::Width() + camera.x - cl::Width() / 2;
        int y = i / cl::Width() + camera.y - cl::Height() / 2;

        // draw the block
        if (!(x < 0 || x >= size.x || y < 0 || y >= size.y))
          blocks[x + y * size.x].Render(i % cl::Width(), i / cl::Width());
        
        // outside render mode
        else if (outsideRender == OR_BLACK)
          cl::Put(' ', i % cl::Width(), i / cl::Width(), 0x0, 0x0);

        // outside render mode
        else if (outsideRender == OR_SEA)
          Block::RenderWater(i % cl::Width(), i / cl::Width(), x, y);
      }

      // render all the entities
      for (auto& entity : entities)
        entity->Render(
          entity->pos.x - camera.x + cl::Width() / 2,
          entity->pos.y - camera.y + cl::Height() / 2);

      // render light mask
      vec<float> lightMask(cl::Width() * cl::Height(), 0);

      // for each point on the screen
      for (int x = 0; x < cl::Width(); x++) {
        for (int y = 0; y < cl::Height(); y++)
        {
          // get the world coords
          int wx = x + camera.x - cl::Width() / 2;
          int wy = y + camera.y - cl::Height() / 2;
          int lmpos = x + y * cl::Width();

          // try find light
          for (auto& light : lights) {
            float sqdist = (v2s(wx, wy) - light.first).LengthSq();
            int lightsq = light.second * light.second * lightScalar;
            
            // break if too far
            if (sqdist > lightsq)
              continue;

            // add to the mask
            lightMask[lmpos] += 1 - sqdist / lightsq;
          }
        }
      }

      // write the light mask
      for (int x = 0; x < cl::Width(); x++) {
        for (int y = 0; y < cl::Height(); y++) {
          float lm = lightMask[x + y * cl::Width()];
          if (lm < 0.35)
            cl::Put('a', x, y, 0x0);
          if (lm == 0)
            cl::AttrAt(x, y) = 0;
          else if (lm < 0.7)
            cl::AttrAt(x, y) = (Utils::decay[(cl::AttrAt(x, y) & 0xf0) >> 4] << 4) | (Utils::decay[cl::AttrAt(x, y) & 0x0f]);
        }
      }
    }

    // all generators
    void GenerateIsland(void);
};