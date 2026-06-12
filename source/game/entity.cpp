#include "area.h"
#include "entity.h"

// entity tries to move (recommended to use separate axis)
void Entity::TryMove(int dirX, int dirY)
{
  // check bounds
  if (pos.x + dirX < 0 || pos.x + dirX >= area->size.x || pos.y + dirY < 0 || pos.y + dirY >= area->size.y)
    return;
  
  // check collision
  Block& block = area->blocks[pos.x + dirX + (pos.y + dirY) * area->size.x];
  if (Info::blocks[block.id].entCol)
    return;
  
  // move
  pos.x += dirX;
  pos.y += dirY;
}

// apply damage to the entity
void Entity::ApplyDamage(int damage) {
  health -= damage;
  if (health <= 0)
    Death();
}

// player constructor
Player::Player(Area* area, v2s pos) : Entity(area, pos, Info::Entity::ids["player"]) {
  moveTimer = ut::Timer(4);
  light = area->LightCreate(pos, 10);
}

// player movement
void Player::Behave(void)
{
  // movement axis
  char dirY = cl::Pressed(cl::KEY_DOWN) - cl::Pressed(cl::KEY_UP);
  char dirX = cl::Pressed(cl::KEY_RIGHT) - cl::Pressed(cl::KEY_LEFT);

  // move by timer
  if (moveTimer.Check()) {
    TryMove(dirX, 0);
    TryMove(0, dirY);
    moveTimer.Reset();
  }

  // follow by the camera and light
  area->camera = pos;
  light->pos = pos;
}

// on death
void Player::Death(void)
{
  // ...
}