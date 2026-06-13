#pragma once
#include "define.h"

#define SC_DEFAULT_INIT static void Init(void) {}
#define SC_DEFAULT_UPDATE static void Update(void) {}

#define DeclareScene(name) \
  static void Init(); \
  static void Update(); \
  static Scene scene(#name, Init, Update);


class Scene
{
    private:
    
        using SceneMap = umap<string, Scene*>;
    singleton(SceneMap, Scenes);
    static Scene* sceneptr;

        callback init;
    callback update;

    public:

        Scene(string name, callback init, callback update) {
      Scenes()[name] = this;
      this->init = init;
      this->update = update;
    }

        static void InitAll() {
      for (auto& scene : Scenes())
        scene.second->init();
    }

        static void UpdateCurrent() {
      sceneptr->update();
    }

        static void Switch(string name) {
      sceneptr = Scenes()[name];
    }
};