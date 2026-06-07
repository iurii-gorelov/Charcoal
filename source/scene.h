#pragma once
#include "define.h"

// scene flags
#define SC_NO_INIT static void Init(void) {}
#define SC_NO_UPDATE static void Update(void) {}

// shortcut for the scene class
#define DeclareScene(name, defaults) \
  static void Init(); \
  static void Update(); \
  static Scene scene(#name, Init, Update); \
  defaults;


// class that defines a scene
class Scene
{
  // private properties
  private:
    
    // collection of all the scenes and current scene
    using SceneMap = umap<string, Scene*>;
    singleton(SceneMap, Scenes);
    static Scene* sceneptr;

    // callback functions
    callback init;
    callback update;

  // public functions
  public:

    // constructor
    Scene(string name, callback init, callback update) {
      Scenes()[name] = this;
      this->init = init;
      this->update = update;
    }

    // init all the scenes
    static void InitAll() {
      for (auto& scene : Scenes())
        scene.second->init();
    }

    // call update on current scene
    static void UpdateCurrent() {
      sceneptr->update();
    }

    // switch the scene
    static void Switch(string name) {
      sceneptr = Scenes()[name];
    }
};