#include "scene.h"
#include "resources.h"

// short scene declaration
DeclareScene(Menu,);

// gets the settings
static void Init(void)
{
  // get the settings
  auto settings = Res::Get<Res::UMF>("settings");
  
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
    
}

// update function works sequentially
static void Update(void)
{
}