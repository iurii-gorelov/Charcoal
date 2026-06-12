#include "area.h"
#include "entity.h"

// script for generating an island
void Area::GenerateIsland(void)
{
  // base props
  floorColor = 0xa;
  outRenderBlock.id = Info::Block::ids["water"];

  // set the size
  size.x = size.y = 256;
  blocks.resize(size.Area());

  // generate the grass
  for (int i = 0; i < size.Area(); i++)
    if (rand.Chance(0.1))
      BlockPut("grass", i % size.x, i / size.x);
    else if (rand.Chance(0.02))
      BlockPut("flower", i % size.x, i / size.x);

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

      // circular form
      for (int dx = -radius; dx <= radius; dx++)
        for (int dy = -radius; dy <= radius; dy++)
          if (dx * dx + dy * dy <= radius * radius && rand.Chance(0.8))
            BlockPut(rand.Chance(0.1) ? "redtree" : "tree", center.x + dx, center.y + dy);
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
      BlockPut("rock", pos.x, pos.y);
    }
  }

  // generate waters
  for (int i = 0; i < size.Area(); i++)
  {
    // chance of lake generation
    if (!rand.Chance(0.001))
      continue;

    // random amount of patches
    int clusterCount = rand.Next(4, 8);
    
    // each patch is a circle
    for (int j = 0; j < clusterCount; j++)
    {
      // random size and position
      float radius = rand.Next(1, 3) * 2 + 0.5;
      v2s center =
        v2s(i % size.x, i / size.x) +
          rand.SqrPos(4);

      // circular form
      for (int dx = -radius; dx <= radius; dx++)
        for (int dy = -radius; dy <= radius; dy++)
          if (dx * dx + dy * dy <= radius * radius)
            BlockPut("water", center.x + dx, center.y + dy);
    }
  }

  // generate the water around
  for (int x = 0; x < size.x; x++) {
    for (int y = 0; y < size.y; y++) {
      if (x < 1 || x >= size.x - 1 || y < 1 || y >= size.y - 1)
        if (rand.Chance(0.65))
          BlockPut("water", x, y);
      if (x < 2 || x >= size.x - 2 || y < 2 || y >= size.y - 2)
        if (rand.Chance(0.3))
          BlockPut("water", x, y);
      if (x < 3 || x >= size.x - 3 || y < 3 || y >= size.y - 3)
        if (rand.Chance(0.05))
          BlockPut("water", x, y);
    }
  }

  // clear space for the player
  for (int wx = size.x / 2 - 4; wx < size.x / 2 + 4; wx++) {
    for (int wy = size.y / 2 - 4; wy < size.y / 2 + 4; wy++) {
      Block& block = blocks[wx + wy * size.x];
      Info::Block& info = Info::blocks[block.id];
      if (info.entCol)
        block.id = 0;
    }
  }

  // create the player
  EntityAdd(player = new Player(this, v2s(size.x / 2, size.y / 2)));
}