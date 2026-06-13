#include "../scene.h"
#include "game.h"
#include "info.h"

DeclareScene(Inventory);
SC_DEFAULT_INIT;

static int pointer = 0;
static int scroll = 0;

static void Update(void) {
  if (cl::JustPressed<cl::KEY_ESC>())
    Scene::Switch("Game"), Game::curArea->player->SelectItem(0);
  if (cl::JustPressed<'C'>()) Scene::Switch("Crafting");

  if (cl::JustPressed<cl::KEY_UP>())
    pointer--;
  else if (cl::JustPressed<cl::KEY_DOWN>())
    pointer++;

  if (pointer < 0) pointer = Game::inventory.size() - 1;
  if (pointer >= Game::inventory.size()) pointer = 0;

  if (cl::JustPressed<'1'>())
    std::swap(Game::inventory[pointer], Game::inventory[0]);
  else if (cl::JustPressed<'2'>())
    std::swap(Game::inventory[pointer], Game::inventory[1]);
  else if (cl::JustPressed<'3'>())
    std::swap(Game::inventory[pointer], Game::inventory[2]);
  else if (cl::JustPressed<'4'>())
    std::swap(Game::inventory[pointer], Game::inventory[3]);

  cl::Fill(' ', 3, 3, cl::Width() - 6, cl::Height() - 6, 0x00, 0x00);

  int hbX = 3 + 1;
  int hbY = 3 + 3;
  int maxw = cl::Width() - hbX - 11;
  int maxh = cl::Height() - hbY - 4;

  while (pointer >= maxh + scroll) scroll++;
  while (pointer < scroll) scroll--;

  cl::Write("\ffINVENTORY", hbX, hbY - 2);
  for (int i = 0; i < maxh; i++) {
    if (i + scroll >= Game::inventory.size()) {
      cl::Write("\f8EMPTY", hbX + 2, hbY + i);
      continue;
    }
    auto& item = Game::inventory[i + scroll];
    string itemName =
        Info::items[Game::inventory[i + scroll].first].title.substr(0, maxw);
    int itemCount = Game::inventory[i + scroll].second;
    cl::Write((i < 4 ? "\ff" : "\f7") + itemName, hbX + 2, hbY + i);
    cl::Write((i < 4 ? "\ff" : "\f7") + (" <" + std::to_string(itemCount)),
              hbX + maxw + 2, hbY + i);
  }

  cl::Put('>', hbX, hbY + pointer - scroll, 0x0a, 0x00);
  for (int i = 0; i < maxw + 7; i++)
    cl::AttrAt(hbX + i, hbY + pointer - scroll) = 0x0a;
}