#include "area.h"
#include "entity.h"
#include "game.h"

// remove entity default
void Entity::Death(void) {
  area->EntityRemove(this);
}

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

  // decrement timers
  moveTimer.Check();
  useTimer.Check();

  // use item
  if (cl::Pressed('Z')) {
    if (dirX != 0 || dirY != 0)
      UseItem(v2s(dirX, dirY));
  }

  // change hotbar and update use timer
  else if (cl::Pressed(cl::KEY_TAB))
    SelectItem((Game::selectedSlot + 1) % HOTBAR_SIZE);
  

  // move by timer
  else if (moveTimer.Check(false)) {
    TryMove(dirX, 0);
    TryMove(0, dirY);
    moveTimer.Reset();
  }

  // follow by the camera and light
  area->camera = pos;
  light->pos = pos;
}

// select an item
void Player::SelectItem(int slot) {
  Game::selectedSlot = slot;
  Info::Item& item = Info::items[Game::hotbar[slot]];
  useTimer.max = item.useTimer;
  useTimer.Reset();
}

// on death
void Player::Death(void)
{
  // ...
}

// use item
void Player::UseItem(v2s dir)
{
  // use timer
  if (!useTimer.Check(false))
    return;

  // use item
  useTimer.Reset();

  // get the item id
  uchar itemId = Game::hotbar[Game::selectedSlot];
  Info::Item& item = Info::items[itemId];
  uchar& itemCount = Game::inventory[itemId];

  // check if the item is available
  if (itemCount <= 0)
    return;

  // item is wand
  if (item.type == 1)
    area->EntityAdd(new Projectile(area, pos, dir, this, item));
}

// behaviour
void Projectile::Behave(void)
{
  // move
  pos = pos + direction;
  power--;

  // check if
  if (area->Inside(pos) && Info::blocks[area->blocks[pos.x + pos.y * area->size.x].id].projCol) {
    area->blocks[pos.x + pos.y * area->size.x].id = 0;
    area->EntityRemove(this);
  }

  // remove if out of power
  else if (power <= 0)
    area->EntityRemove(this);
}