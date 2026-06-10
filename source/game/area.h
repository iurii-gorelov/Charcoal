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
    float lightScalar = 1;
    
    // generation properties
    v2s size;
    int floorColor;
    int outsideRender;

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

    // update current area
    void Update(void)
    {
      // increment the time
      time++;
    }

    // renderer the area
    void Render(void)
    {
      // fill with default color
      cl::Fill(' ', 0, 0, cl::Width(), cl::Height(), 0x0, floorColor);

      // render everything
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

      // render light mask
      vec<char> lightMask(cl::Width() * cl::Height(), 0);

      // for each point on the screen
      for (int x = 0; x < cl::Width(); x++) {
        for (int y = 0; y < cl::Height(); y++)
        {
          // get the world coords
          int wx = x + camera.x - cl::Width() / 2;
          int wy = y + camera.y - cl::Height() / 2;

          // try find light
          for (auto& light : lights) {
            float sqdist = (v2s(wx, wy) - light.first).LengthSq();
            int lightsq = light.second * light.second * lightScalar;
            if (sqdist < lightsq) {
              lightMask[x + y * cl::Width()] = 1;
              if (sqdist < lightsq * 4 / 5) {
                lightMask[x + y * cl::Width()] = 2;
                if (sqdist < lightsq * 1 / 2) {
                  lightMask[x + y * cl::Width()] = 3;
                  if (sqdist < lightsq * 1 / 4) {
                    lightMask[x + y * cl::Width()] = 4;
                    break;
                  }
                }
              }
            }
          }
        }
      }

      // write the light mask
      for (int x = 0; x < cl::Width(); x++) {
        for (int y = 0; y < cl::Height(); y++) {
          if (lightMask[x + y * cl::Width()] == 0)
            cl::AttrAt(x, y) = 0;
          else if (lightMask[x + y * cl::Width()] == 1)
            cl::Put('g', x, y, 0x0);
          else if (lightMask[x + y * cl::Width()] == 2)
            cl::Put('a', x, y, 0x0);
          else if (lightMask[x + y * cl::Width()] == 3)
            cl::AttrAt(x, y) = (Utils::decay[(cl::AttrAt(x, y) & 0xf0) >> 4] << 4) | (Utils::decay[cl::AttrAt(x, y) & 0x0f]);
        }
      }
    }

    // script for generating an island
    void GenerateIsland(void)
    {
      // base props
      rand.seed = 123456;
      floorColor = 0xa;
      outsideRender = OR_SEA;

      // set the size
      size.x = size.y = 256;
      blocks.resize(size.Area());

      // generate the grass
      for (int i = 0; i < size.Area(); i++)
        if (rand.Chance(0.1))
          Place(Block::GRASS, i % size.x, i / size.x);
        else if (rand.Chance(0.02))
          Place(Block::FLOWER, i % size.x, i / size.x);

      // generate the trees
      for (int i = 0; i < size.Area(); i++)
      {
        // chance of tree generation
        if (!rand.Chance(0.002))
          continue;

        // random amount of patches
        int clusterCount = rand.Next(1, 16);
        
        // each patch is a circle
        for (int j = 0; j < clusterCount; j++)
        {
          // random size and position
          float radius = rand.Next(1, 10) / 2.0;
          v2s center =
            v2s(i % size.x, i / size.x) +
              rand.SqrPos(10);

          for (int dx = -radius; dx <= radius; dx++)
            for (int dy = -radius; dy <= radius; dy++)
              if (dx * dx + dy * dy <= radius * radius && rand.Chance(0.8))
                Place(Block::TREE, center.x + dx, center.y + dy);
        }
      }

      // generate the rock clusters
      for (int i = 0; i < size.Area(); i++)
      {  
        // chance
        if (!rand.Chance(0.0005))
          continue;

        // random amount of patches
        int rockCount = rand.Next(8, 16);

        // put the rocks
        for (int j = 0; j < rockCount; j++) {
          v2s pos =
            v2s(i % size.x, i / size.x) +
            rand.SqrPos(4);
          Place(Block::ROCK, pos.x, pos.y);
        }
      }

      // generate the water around
      for (int x = 0; x < size.x; x++) {
        for (int y = 0; y < size.y; y++) {
          
          if (x < 1 || x >= size.x - 1 || y < 1 || y >= size.y - 1)
            if (rand.Chance(0.65))
              Place(Block::WATER, x, y);
          if (x < 2 || x >= size.x - 2 || y < 2 || y >= size.y - 2)
            if (rand.Chance(0.3))
              Place(Block::WATER, x, y);
          if (x < 3 || x >= size.x - 3 || y < 3 || y >= size.y - 3)
            if (rand.Chance(0.05))
              Place(Block::WATER, x, y);
        }
      }

      // Generate test lights
      for (int i = 0; i < 256; i++)
        lights.push_back(std::make_pair(rand.SqrPos(size.x / 2) + v2s(size.x / 2, size.y / 2), rand.Next(5, 20)));
    }
};