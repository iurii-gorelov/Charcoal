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
  extern uptr<Area> curArea;

  // player's inventory
  extern vec<std::pair<uchar, uchar>> inventory;
  extern uchar selectedSlot;

  // create a new world
  void CreateWorld(int seed);
  void AddItem(uchar id);
  void RemoveItem(uchar id);
}