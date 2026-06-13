#pragma once
#include "../define.h"
#include "../resources.h"
#include "../scene.h"
#include "../utils.h"
#include "area.h"

#define HOTBAR_SIZE 4

namespace Game {
extern uptr<Area> curArea;

extern vec<std::pair<uchar, uchar>> inventory;
extern uchar selectedSlot;
extern bool gameOver;

void CreateWorld(int seed);
void DeleteWorld(void);

void AddItem(uchar id);
void RemoveItem(uchar id);
pair<uchar, uchar>* GetItem(uchar id);
}  // namespace Game