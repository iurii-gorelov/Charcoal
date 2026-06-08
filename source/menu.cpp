#include "scene.h"
#include "resources.h"
#include "global.h"

// short scene declaration
DeclareScene(Menu,);

// global settings
static Res::UMF settings;

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
    ("resources/" + settings["font.name"].asStr + ".ttf")
      .c_str());
  cl::SetupFont(
    settings["font.name"].asStr.c_str(),
    settings["font.size"].asNum);
  
  // check if it is fullscreen
  if (settings["win.mode"].asNum == 1)
    cl::FullScreen(true);
  
  // get the window size
  else
    cl::Resize(
      settings["win.width"].asNum,
      settings["win.height"].asNum);

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
  if (cl::PressedJ<cl::KEY_DOWN>())
    cursor = (cursor + 1) % 3; 
  else if (cl::PressedJ<cl::KEY_UP>())
    cursor = (cursor - 1 + 3) % 3;
  
  // the actor key
  if (cl::PressedJ<' '>()) {
    if (cursor == 0)
      Scene::Switch("NewGame");
    else if (cursor == 1) {
      cl::FullScreen(!cl::FullScreen());
      if (!cl::FullScreen())
        cl::Resize(
          settings["win.width"].asNum,
          settings["win.height"].asNum);
    }
    else if (cursor == 2)
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
  cl::Fill(' ', bx, by, 14, 5, 0x0F, 0x00);
  
  // write the text
  cl::Write("\f7NEW GAME",   bx + 3, by + 1);
  cl::Write("\f7FULLSCREEN", bx + 3, by + 2);
  cl::Write("\f7QUIT",       bx + 3, by + 3);

  // write the pointer
  cl::Put('X', bx + 1, by + 1 + cursor, 0x0A, 0x00);
  for (int i = 0; i < 14; i++)
    cl::AttrAt(bx + i, by + 1 + cursor) = 0x0A;

  // increment the tick
  tick++;
}