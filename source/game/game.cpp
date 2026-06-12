#include "game.h"
#include "info.h"

// declare the scene
DeclareScene(Game);

// initialize statics
vec<uptr<Area>> Game::areas;
Area* Game::curArea = nullptr;

// initialize infoes
vec<Info::Block>  Info::blocks;
vec<Info::Entity> Info::entities;
vec<Info::Item>   Info::items;
umap<string, uchar> Info::Block::ids;
umap<string, uchar> Info::Entity::ids;
umap<string, uchar> Info::Item::ids;

// player's inventory
umap<uchar, uchar> Game::inventory;
uchar Game::hotbar[HOTBAR_SIZE];
uchar Game::selectedSlot;

// initialize the scene
static void Init(void)
{
  // parse the object info
  Info::Block::Parse(Res::Get<Res::UMF>("blocks"));
  Info::Entity::Parse(Res::Get<Res::UMF>("entities"));
  Info::Item::Parse(Res::Get<Res::UMF>("items"));
}

// update the scene
static void Update(void)
{
  // quit to the menu
  if (cl::JustPressed<cl::KEY_ESC>())
    Scene::Switch("Menu");

  // current area
  Game::curArea->Update();
  Game::curArea->Render();

  // buggy cursor
  cl::CursorMode(cl::CM_HIDDEN);
}

// temp
void Game::CreateWorld(int seed) {
  Game::areas.push_back(std::make_unique<Area>());
  Area* area = Game::areas.back().get();
  area->rand.seed = seed;
  area->GenerateIsland();
  Game::curArea = area;
  hotbar[0] = 1;
  inventory[1] = 1;
  area->player->SelectItem(0);
}