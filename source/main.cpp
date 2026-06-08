#include <conlib.h>
#include "scene.h"
#include "resources.h"
#include "global.h"

// definitions
umap<string, uptr<Res>> Res::resources;
Scene* Scene::sceneptr;

// constants
const int FPS = 60;

// main function
int main()
{
  // initialize the console resources and scenes
  cl::Initialize();
  Res::LoadList("resources/list.res");
  Scene::InitAll();
  Scene::Switch("Menu");

  // main loop
  while (true)
  {
    // call current update function
    Scene::UpdateCurrent();

    // apply the buffer and delay
    cl::ApplyBuffer();
    cl::Sleep(1000 / FPS);
  }
}

// quit the game
void Utils::Quit(void) {
  cl::Destroy();
  exit(0);
}