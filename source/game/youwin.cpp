#include "../scene.h"

DeclareScene(YouWin);
SC_DEFAULT_INIT;

static void Update(void) {
  if (cl::JustPressed<cl::KEY_ESC>()) Scene::Switch("Game");
  cl::Fill(' ', 0, 0, cl::Width(), cl::Height(), 0x0F, 0x00);
  cl::Write("\f7YOU WIN", 1, 1);
}