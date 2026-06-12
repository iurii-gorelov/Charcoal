#pragma once
#include "../scene.h"
#include "../resources.h"
#include "../utils.h"
#include "../define.h"
#include "area.h"

// global namespace for the game
namespace Game
{
  // absolutely all areas here
  extern vec<uptr<Area>> areas;
  extern Area* curArea;

  // create a new world
  void CreateWorld(int seed);
}