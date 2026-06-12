#include "../scene.h"
#include "../resources.h"
#include "game.h"
#include "info.h"
#include "../utils.h"
#include <iterator>

// #define toupper(str) \
//   std::transform(str.begin(), str.end(), str.begin(), \
//     [](unsigned char c) { return std::toupper(c); });

// declare scene
DeclareScene(Crafting);
SC_DEFAULT_INIT;

// pointer
static int pointer = 0;
static int scroll = 0;
static bool parsed = false;
umap<string, umap<uchar, uchar>> recipes;

// initialize init
static void Parse(void)
{
  // current recipe and recipe list
  auto rl = Res::Get<Res::UMF>("crafts");

  // loop through all the recipes
  for (auto& pair : rl)
  {
    // split
    auto parts = Utils::Split(pair.first, '/');

    // count
    if (parts.second == "$")
      recipes[parts.first][0] = pair.second.asNum;
    else
      recipes[parts.first][Info::Item::ids[parts.second]] = pair.second.asNum;
  }
}

// check for enough items
static bool Check(string craft)
{
  // get the items
  auto& items = recipes[craft];

  // for each item
  for (auto& pair : items)
  {
    // if not zero
    if (pair.first == 0)
      continue;

    // get item
    auto item = Game::GetItem(pair.first);
    if (item == nullptr)
      return false;

    // if not enough
    if (item->second < pair.second)
      return false;
  }

  // enough
  return true;
}

// craft
static void Craft(string craft)
{
  // get the items
  auto& items = recipes[craft];

  // for each item
  for (auto& pair : items)
  {
    // get item to the inventory
    if (pair.first == 0) {
      for (int i = 0; i < pair.second; i++)
        Game::AddItem(Info::Item::ids[craft]);
      continue;
    }

    // remove items
    for (int i = 0; i < pair.second; i++)
      Game::RemoveItem(pair.first);
  }
}

// update function
static void Update(void)
{
  // parse the recipes
  if (!parsed)
    Parse(), parsed = true;

  // check for escape
  if (cl::JustPressed<cl::KEY_ESC>())
    Scene::Switch("Game");
  if (cl::JustPressed<'C'>())
    Scene::Switch("Inventory");

  // up and down
  if (cl::JustPressed<cl::KEY_UP>())
    pointer--;
  else if (cl::JustPressed<cl::KEY_DOWN>())
    pointer++;

  // if pointer is out of bounds
  if (pointer < 0)
    pointer = recipes.size() - 1;
  if (pointer >= recipes.size())
    pointer = 0;

  // craft
  if (cl::JustPressed<cl::KEY_ENTER>() && Check(std::next(recipes.begin(), pointer)->first))
    Craft(std::next(recipes.begin(), pointer)->first);

  // background
  cl::Fill(' ', 3, 3, cl::Width() - 6, cl::Height() - 6, 0x00, 0x00);

  // starting point
  int hbX = 4;
  int hbY = 6, hbY2 = 11;
  int maxw = cl::Width() - hbX - 11;
  int maxh = cl::Height() - hbY2 - 4;

  // scrolling
  while (pointer >= maxh + scroll)
    scroll++;
  while (pointer < scroll)
    scroll--;

  // show current recipe
  auto recipe = std::next(recipes.begin(), pointer)->second;
  int i = 0;
  for (auto& pair : recipe) {
    if (pair.first == 0)
      continue;
    string itemName = Info::items[pair.first].title.substr(0, maxw + 2);
    int itemCount = pair.second;
    cl::Write("\f7" + itemName, hbX, hbY + i);
    cl::Write("\f7" + (" <" + std::to_string(itemCount)), hbX + maxw + 2, hbY + i);
    i++;
  }

  // render the recipe names
  cl::Write("\ffCRAFTING", hbX, hbY - 2);
  int index = 0;
  for (auto& pair : recipes) {
    string itemName = Info::items[Info::Item::ids[pair.first]].title.substr(0, maxw);
    int itemCount = pair.second[0];
    bool craftable = Check(pair.first);
    cl::Write((craftable ? "\ff" : "\f7") + itemName, hbX + 2, hbY2 + index);
    cl::Write((craftable ? "\ff" : "\f7") + (" <" + std::to_string(itemCount)), hbX + maxw + 2, hbY2 + index);
    index++;
  }

  // pointer
  cl::Put('>', hbX, hbY2 + pointer - scroll, 0x0a, 0x00);
  for (int i = 0; i < maxw + 7; i++)
    cl::AttrAt(hbX + i, hbY2 + pointer - scroll) = 0x0a;
}