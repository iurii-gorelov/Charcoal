#include "scene.h"
#include "resources.h"
#define MENU_START_GAME 0
#define MENU_LOAD_GAME 1
#define MENU_FULLSCREEN 2
#define MENU_QUIT 3

// short scene declaration
DeclareScene(Menu);

// global settings
static Res::UMF settings;

// function to toggle fullscreen
static void ToggleFullscreen(void);

// gets the settings
static void Init(void)
{
  // get the settings
  settings = Res::Get<Res::UMF>("settings");
  
  // minor settings 
  cl::NoResize();
  cl::NoScrollBar();
  cl::NoSelect();

  // set the font
  cl::InstallFont(
    ("resources/" + settings["font-name"].asStr + ".ttf")
      .c_str());
  cl::SetupFont(
    settings["font-name"].asStr.c_str(),
    settings["font-size-win"].asNum);
  
  // set the window size
  cl::Resize(
    settings["win-width"].asNum,
    settings["win-height"].asNum);

  // set the title
  cl::Title("Charcoal");

  // minor settings
  cl::CursorMode(cl::CM_HIDDEN);
}

// update function works sequentially
static void Update(void)
{
  // ticker
  static int tick = 0;
  static char cursor = 0;

  // check for press
  if (cl::JustPressed<cl::KEY_DOWN>())
    cursor = (cursor + 1) % 4; 
  else if (cl::JustPressed<cl::KEY_UP>())
    cursor = (cursor - 1 + 4) % 4;
  
  // the actor key
  if (cl::JustPressed<' '>()) {
    if (cursor == MENU_START_GAME)
      Scene::Switch("Game");
    else if (cursor == MENU_LOAD_GAME)
      Scene::Switch("LoadGame");
    else if (cursor == MENU_FULLSCREEN)
      ToggleFullscreen();
    else if (cursor == MENU_QUIT)
      Utils::Quit();
  }

  // clear the screen
  cl::Fill(' ', 0, 0, cl::Width(), cl::Height(), 0x0F, 0x00);

  // background
  for (int y = (tick / 10) % 4 - 3; y < cl::Height(); y += 4) {
    for (int x = (tick / 10) % 9 - cl::Height() - 8; x < cl::Width(); x += 9) {
      vec<string> colors = {"\f8", "\b8\f0", "\f7", "\bf\f0"};
      for (int i = 0; i < 4; i++)
        cl::Write(colors[i] + "CHARCOAL ", x + y + i, y + i);
    }
  }

  // clear place for the menu
  int bx = cl::Width() / 2 - 7, by = cl::Height() / 2 - 3;
  cl::Fill(' ', bx, by, 14, 6, 0x0F, 0x00);
  
  // write the text
  cl::Write("\f7NEW GAME",     bx + 3, by + MENU_START_GAME + 1);
  cl::Write("\f7LOAD GAME",    bx + 3, by + MENU_LOAD_GAME + 1);
  if (cl::FullScreen()) 
    cl::Write("\f7FULLSCREEN", bx + 3, by + MENU_FULLSCREEN + 1);
  else 
    cl::Write("\f7WINDOWED",   bx + 3, by + MENU_FULLSCREEN + 1);
  cl::Write("\f7QUIT",         bx + 3, by + MENU_QUIT + 1);

  // write the pointer
  cl::Put('X', bx + 1, by + 1 + cursor, 0x0A, 0x00);
  for (int i = 0; i < 14; i++)
    cl::AttrAt(bx + i, by + 1 + cursor) = 0x0A;

  // buggy cursor
  cl::CursorMode(cl::CM_HIDDEN);

  // increment the tick
  tick++;
}

// toggle fullscreen
static void ToggleFullscreen(void)
{
  // in case of windowed mode
  if (cl::FullScreen()) 
    cl::SetupFont(
      settings["font-name"].asStr.c_str(),
      settings["font-size-win"].asNum);

  // fullscreen
  else {
    cl::SetupFont(
      settings["font-name"].asStr.c_str(),
      settings["font-size-fs"].asNum);
  }
  
  // toggle the real value
  cl::FullScreen(!cl::FullScreen());

  // set the window size
  if (!cl::FullScreen())
    cl::Resize(
      settings["win-width"].asNum,
      settings["win-height"].asNum);
}