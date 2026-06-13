#include <conlib.h>

#include "resources.h"
#include "scene.h"

umap<string, uptr<Res>> Res::resources;
Scene* Scene::sceneptr;

const int FPS = 60;

int main() {
  cl::Initialize();
  Res::LoadList("resources/list.res");
  Scene::InitAll();
  Scene::Switch("Menu");

  while (true) {
    Scene::UpdateCurrent();

    cl::ApplyBuffer();
    cl::Sleep(1000 / FPS);
    ut::ticks++;
  }
}

void Utils::Quit(void) {
  cl::Destroy();
  exit(0);
}