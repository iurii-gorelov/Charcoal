#include "area.h"

// script for generating an island
void Area::GenerateIsland(void)
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
    else if (rand.Chance(0.02))
      Place(Block::FIREPLACE, i % size.x, i / size.x);

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

  // test light
  lights.push_back(std::make_pair(camera, 10));

  // find place to spawn the player
  for (int i = 0; i < size.Area(); i += size.x / 16)
    if (blocks[i].id == Block::AIR && rand.Chance(0.001))
      spawnPoint = v2s(i % size.x, i / size.x);

  // create the player
  Spawn(new Player(), spawnPoint.x, spawnPoint.y);
}