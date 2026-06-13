#include "game.h"

#include <algorithm>

#include "info.h"

DeclareScene(Game);

uptr<Area> Game::curArea = nullptr;

vec<Info::Block> Info::blocks;
vec<Info::Entity> Info::entities;
vec<Info::Item> Info::items;
umap<string, uchar> Info::Block::ids;
umap<string, uchar> Info::Entity::ids;
umap<string, uchar> Info::Item::ids;

vec<pair<uchar, uchar>> Game::inventory;
uchar Game::selectedSlot;
bool Game::gameOver = false;

static void RenderUI(void);

static void Init(void) {
  Info::Block::Parse(Res::Get<Res::UMF>("blocks"));
  Info::Entity::Parse(Res::Get<Res::UMF>("entities"));
  Info::Item::Parse(Res::Get<Res::UMF>("items"));
}

static void Update(void) {
  if (cl::JustPressed<cl::KEY_ESC>()) Scene::Switch("Menu");

  Game::curArea->Update();
  Game::curArea->Render();
  RenderUI();

  cl::CursorMode(cl::CM_HIDDEN);
}

void Game::CreateWorld(int seed) {
  curArea.reset(new Area());
  curArea->rand.seed = seed;
  curArea->GenerateIsland();
  AddItem(1);
  curArea->player->SelectItem(0);
}

void Game::DeleteWorld(void) {
  curArea = nullptr;
  inventory.clear();
  selectedSlot = 0;
}

static void RenderUI(void) {
  for (int i = Game::curArea->player->health, j = 0; i > 0; i -= 2, j++) {
    if (i >= 2)
      cl::Put('`', 1 + j, 1, 0x0c);
    else
      cl::Put('{', 1 + j, 1, 0x0c);
  }

  string itemString[4];
  for (int i = 0; i < 4; i++) {
    if (i >= Game::inventory.size()) {
      itemString[i] = "EMPTY";
      continue;
    }
    auto& item = Game::inventory[i];
    string itemName = Info::items[Game::inventory[i].first].title;
    int itemCount = Game::inventory[i].second;
    itemString[i] = itemName + " <" + std::to_string(itemCount);
  }

  for (int i = 0; i < itemString[Game::selectedSlot].length(); i++)
    cl::Put(itemString[Game::selectedSlot][i],
            cl::Width() - itemString[Game::selectedSlot].length() - 1 + i, 1,
            0x0f);

  if (cl::Pressed('X')) {
    int maxLen =
        std::max_element(itemString, itemString + 4, [](string a, string b) {
          return a.length() < b.length();
        })->length();

    int jy = cl::Height() - 1 - 6;
    cl::Fill(' ', 1, jy, maxLen + 4, 6, 0x0f, 0x00);

    for (int i = 0; i < 4; i++)
      cl::Write((itemString[i] == "EMPTY" ? "\f8" : "\ff") + itemString[i], 4,
                jy + i + 1);

    cl::Put('>', 2, jy + Game::selectedSlot + 1, 0x0a, 0x00);
    for (int i = 1; i < maxLen + 4; i++)
      cl::AttrAt(i, jy + Game::selectedSlot + 1) = 0x0a;
  }
}

pair<uchar, uchar>* Game::GetItem(uchar id) {
  for (auto& pair : Game::inventory)
    if (pair.first == id) return &pair;
  return nullptr;
}

void Game::AddItem(uchar id) {
  auto res = Game::GetItem(id);
  if (res == nullptr)
    Game::inventory.push_back(std::make_pair(id, 1));
  else
    res->second++;
}

void Game::RemoveItem(uchar id) {
  auto res = std::find_if(Game::inventory.begin(), Game::inventory.end(),
                          [id](auto& pair) { return pair.first == id; });
  res->second--;
  if (res->second <= 0) {
    Game::inventory.erase(res);
    Game::curArea->player->SelectItem(0);
  }
}