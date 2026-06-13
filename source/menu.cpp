#include "game/game.h"
#include "resources.h"
#include "scene.h"

#define MENU_START_GAME 0
#define MENU_LOAD_GAME 1
#define MENU_FULLSCREEN 2
#define MENU_QUIT 3

DeclareScene(Menu);

static Res::UMF settings;

static void ToggleFullscreen(void);

static void Init(void) {
  settings = Res::Get<Res::UMF>("settings");

  cl::NoResize();
  cl::NoScrollBar();
  cl::NoSelect();

  cl::InstallFont(
      ("resources/" + settings["font-name"].asStr + ".ttf").c_str());
  cl::SetupFont(settings["font-name"].asStr.c_str(),
                settings["font-size-win"].asNum);

  cl::Resize(settings["win-width"].asNum, settings["win-height"].asNum);

  cl::Title("Charcoal");

  cl::CursorMode(cl::CM_HIDDEN);
}

static void Update(void) {
  static char cursor = 0;

  if (Game::gameOver) {
    Game::DeleteWorld();
    Game::gameOver = false;
  }

  if (cl::JustPressed<cl::KEY_DOWN>())
    cursor = (cursor + 1) % 4;
  else if (cl::JustPressed<cl::KEY_UP>())
    cursor = (cursor - 1 + 4) % 4;

  if (cl::JustPressed<cl::KEY_ENTER>()) {
    if (cursor == MENU_START_GAME)
      Scene::Switch("Seed");
    else if (cursor == MENU_LOAD_GAME && Game::curArea != nullptr)
      Scene::Switch("Game");
    else if (cursor == MENU_FULLSCREEN)
      ToggleFullscreen();
    else if (cursor == MENU_QUIT)
      Utils::Quit();
  }

  cl::Fill(' ', 0, 0, cl::Width(), cl::Height(), 0x0F, 0x00);

  for (int y = (ut::ticks / 10) % 4 - 3; y < cl::Height(); y += 4) {
    for (int x = (ut::ticks / 10) % 9 - cl::Height() - 8; x < cl::Width();
         x += 9) {
      vec<string> colors = {"\f8", "\b8\f0", "\f7", "\bf\f0"};
      for (int i = 0; i < 4; i++)
        cl::Write(colors[i] + "CHARCOAL ", x + y + i, y + i);
    }
  }

  int bx = cl::Width() / 2 - 7, by = cl::Height() / 2 - 3;
  cl::Fill(' ', bx, by, 14, 6, 0x0F, 0x00);

  cl::Write("\f7NEW GAME", bx + 3, by + MENU_START_GAME + 1);
  cl::Write(string(Game::curArea ? "\f7" : "\f8") + "CONTINUE", bx + 3,
            by + MENU_LOAD_GAME + 1);
  if (cl::FullScreen())
    cl::Write("\f7FULLSCREEN", bx + 3, by + MENU_FULLSCREEN + 1);
  else
    cl::Write("\f7WINDOWED", bx + 3, by + MENU_FULLSCREEN + 1);
  cl::Write("\f7QUIT", bx + 3, by + MENU_QUIT + 1);

  cl::Put('>', bx + 1, by + 1 + cursor, 0x0A, 0x00);
  for (int i = 0; i < 14; i++) cl::AttrAt(bx + i, by + 1 + cursor) = 0x0A;

  cl::CursorMode(cl::CM_HIDDEN);
}

static void ToggleFullscreen(void) {
  if (cl::FullScreen())
    cl::SetupFont(settings["font-name"].asStr.c_str(),
                  settings["font-size-win"].asNum);

  else {
    cl::SetupFont(settings["font-name"].asStr.c_str(),
                  settings["font-size-fs"].asNum);
  }

  cl::FullScreen(!cl::FullScreen());

  if (!cl::FullScreen())
    cl::Resize(settings["win-width"].asNum, settings["win-height"].asNum);
}