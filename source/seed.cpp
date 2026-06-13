#include "resources.h"
#include "scene.h"
#include <string>
#include <sstream>
#include "game/game.h"

DeclareScene(Seed);

static int seed;
static std::string inputBuffer;
static bool waitForInput = true;

static void Init(void) {
  inputBuffer.clear();
  waitForInput = true;
}

static void Update(void) {
  if (waitForInput) {
    if (cl::JustPressed<cl::KEY_ENTER>()) {
      std::stringstream ss(inputBuffer);
      ss >> seed;
      waitForInput = false;
    }
    else if (cl::JustPressed<cl::KEY_BACKSPACE>()) {
      if (!inputBuffer.empty()) inputBuffer.pop_back();
    }
    else if (cl::JustPressed<'0'>()) inputBuffer += '0';
    else if (cl::JustPressed<'1'>()) inputBuffer += '1';
    else if (cl::JustPressed<'2'>()) inputBuffer += '2';
    else if (cl::JustPressed<'3'>()) inputBuffer += '3';
    else if (cl::JustPressed<'4'>()) inputBuffer += '4';
    else if (cl::JustPressed<'5'>()) inputBuffer += '5';
    else if (cl::JustPressed<'6'>()) inputBuffer += '6';
    else if (cl::JustPressed<'7'>()) inputBuffer += '7';
    else if (cl::JustPressed<'8'>()) inputBuffer += '8';
    else if (cl::JustPressed<'9'>()) inputBuffer += '9';
    else if (cl::JustPressed<cl::KEY_ESC>()) {
      Scene::Switch("Menu");
      return;
    }
    cl::Fill(' ', 0, 0, cl::Width(), cl::Height(), 0x0F, 0x00);
        for (int y = (ut::ticks / 10) % 4 - 3; y < cl::Height(); y += 4) {
      for (int x = (ut::ticks / 10) % 9 - cl::Height() - 8; x < cl::Width(); x += 9) {
        vec<string> colors = {"\f8", "\b8\f0", "\f7", "\bf\f0"};
        for (int i = 0; i < 4; i++)
          cl::Write(colors[i] + "CHARCOAL ", x + y + i, y + i);
      }
    }

        int bx = cl::Width() / 2 - 15, by = cl::Height() / 2 - 2;
    cl::Fill(' ', bx, by, 30, 5, 0x0F, 0x00);
    cl::Write("\f7ENTER SEED", bx + 2, by + 1);
    std::string display = inputBuffer;
    cl::Write(display, bx + 2, by + 2);
  }
  else {
    if (Game::curArea != nullptr)
      Game::DeleteWorld();
    Game::CreateWorld(seed);
    Scene::Switch("Game");
  }
}