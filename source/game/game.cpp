#include "game.h"

// declare the scene
DeclareScene(Game);

// initialize statics
vec<Object::Info> Object::info;
vec<uptr<Area>> Game::areas;
Area* Game::curArea = nullptr;

// initialize the scene
static void Init(void)
{
  // parse the object info
  Object::ParseInfo(Res::Get<Res::UMF>("objects"));

  // generate a test world
  Game::CreateWorld();
}

// update the scene
static void Update(void)
{
  // move the camera
  // if (cl::Pressed(cl::KEY_UP) && ut::ticks % 2)
  //   Game::curArea->camera.y--;
  // else if (cl::Pressed(cl::KEY_DOWN) && ut::ticks % 2)
  //   Game::curArea->camera.y++;
  // if (cl::Pressed(cl::KEY_LEFT) && ut::ticks % 2)
  //   Game::curArea->camera.x--;
  // else if (cl::Pressed(cl::KEY_RIGHT) && ut::ticks % 2)
  //   Game::curArea->camera.x++;

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
void Game::CreateWorld(void) {
  Game::areas.push_back(std::make_unique<Area>());
  Area* area = Game::areas.back().get();
  area->GenerateIsland();
  Game::curArea = area;
}