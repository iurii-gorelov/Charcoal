#include "area.h"
#include "object.h"

// player movement appreciated
void Player::Behave(Area* area)
{
  // try move
  if (moveTimer.Check())
  {
    // set direction
    char dirX = cl::Pressed(cl::KEY_RIGHT) - cl::Pressed(cl::KEY_LEFT);
    char dirY = cl::Pressed(cl::KEY_DOWN) - cl::Pressed(cl::KEY_UP);

    // reset the timer
    if (dirX != 0 || dirY != 0)
      moveTimer.Reset();

    // move
    pos.x += dirX;
    pos.y += dirY;
  }

  // make followed by the camera
  area->camera = pos;
}