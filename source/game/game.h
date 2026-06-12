#pragma once
#include "../scene.h"
#include "../resources.h"
#include "../utils.h"
#include "../define.h"
#include "area.h"

// hotbar constant
#define HOTBAR_SIZE 4

// global namespace for the game
namespace Game
{
  // absolutely all areas here
  extern vec<uptr<Area>> areas;
  extern Area* curArea;

  // player's inventory
  extern umap<uchar, uchar> inventory;
  extern uchar hotbar[HOTBAR_SIZE];
  extern uchar selectedSlot;

  // create a new world
  void CreateWorld(int seed);
}