#include <iterator>

#include "../resources.h"
#include "../scene.h"
#include "../utils.h"
#include "game.h"
#include "info.h"

DeclareScene(Crafting);
SC_DEFAULT_INIT;

static int pointer = 0;
static int scroll = 0;
static bool parsed = false;
umap<string, umap<uchar, uchar>> recipes;

static void Parse(void) {
  auto rl = Res::Get<Res::UMF>("crafts");

  for (auto& pair : rl) {
    auto parts = Utils::Split(pair.first, '/');

    if (parts.second == "$")
      recipes[parts.first][0] = pair.second.asNum;
    else
      recipes[parts.first][Info::Item::ids[parts.second]] = pair.second.asNum;
  }
}

static bool Check(string craft) {
  auto& items = recipes[craft];

  for (auto& pair : items) {
    if (pair.first == 0) continue;

    auto item = Game::GetItem(pair.first);
    if (item == nullptr) return false;

    if (item->second < pair.second) return false;
  }

  return true;
}

static void Craft(string craft) {
  auto& items = recipes[craft];

  for (auto& pair : items) {
    if (pair.first == 0) {
      for (int i = 0; i < pair.second; i++)
        Game::AddItem(Info::Item::ids[craft]);
      continue;
    }

    for (int i = 0; i < pair.second; i++) Game::RemoveItem(pair.first);
  }
}

static void Update(void) {
  if (!parsed) Parse(), parsed = true;

  if (cl::JustPressed<cl::KEY_ESC>())
    Scene::Switch("Game"), Game::curArea->player->SelectItem(0);
  if (cl::JustPressed<'C'>()) Scene::Switch("Inventory");

  if (cl::JustPressed<cl::KEY_UP>())
    pointer--;
  else if (cl::JustPressed<cl::KEY_DOWN>())
    pointer++;

  if (pointer < 0) pointer = recipes.size() - 1;
  if (pointer >= recipes.size()) pointer = 0;

  if (cl::JustPressed<cl::KEY_ENTER>() &&
      Check(std::next(recipes.begin(), pointer)->first))
    Craft(std::next(recipes.begin(), pointer)->first);

  cl::Fill(' ', 3, 3, cl::Width() - 6, cl::Height() - 6, 0x00, 0x00);

  int hbX = 4;
  int hbY = 6, hbY2 = 11;
  int maxw = cl::Width() - hbX - 11;
  int maxh = cl::Height() - hbY2 - 5;

  while (pointer >= maxh + scroll) scroll++;
  while (pointer < scroll) scroll--;

  auto recipe = std::next(recipes.begin(), pointer)->second;
  int i = 0;
  for (auto& pair : recipe) {
    if (pair.first == 0) continue;
    string itemName = Info::items[pair.first].title.substr(0, maxw + 2);
    int itemCount = pair.second;
    cl::Write("\f7" + itemName, hbX, hbY + i);
    cl::Write("\f7" + (" <" + std::to_string(itemCount)), hbX + maxw + 2,
              hbY + i);
    i++;
  }

  cl::Write("\ffCRAFTING", hbX, hbY - 2);
  for (int index = 0; index < maxh; index++) {
    int iw = index + scroll;
    auto pair = std::next(recipes.begin(), iw);
    string itemName =
        Info::items[Info::Item::ids[pair->first]].title.substr(0, maxw);
    int itemCount = pair->second[0];
    bool craftable = Check(pair->first);
    cl::Write((craftable ? "\ff" : "\f8") + itemName, hbX + 2, hbY2 + index);
    cl::Write((craftable ? "\ff" : "\f8") + (" <" + std::to_string(itemCount)),
              hbX + maxw + 2, hbY2 + index);
  }

  cl::Put('>', hbX, hbY2 + pointer - scroll, 0x0a, 0x00);
  for (int i = 0; i < maxw + 7; i++)
    cl::AttrAt(hbX + i, hbY2 + pointer - scroll) = 0x0a;
}