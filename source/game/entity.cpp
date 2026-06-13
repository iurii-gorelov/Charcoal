#include "area.h"
#include "entity.h"
#include "game.h"
#include <cmath>

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
  
  // target position
  v2s tarPos = pos + v2s(dirX, dirY);

  // check collision
  Block& block = area->blocks[tarPos.x + (tarPos.y) * area->size.x];
  if (Info::blocks[block.id].entCol)
    return;

  // check all the entities
  for (auto& entity : area->entities)
    if (Info::entities[entity->id].collidable && entity->pos == tarPos)
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
  moveTimer = ut::Timer(8);
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

  // change hotbar and update use timer
  if (cl::Pressed('X')) {
    if (cl::JustPressed<cl::KEY_DOWN>())
      SelectItem((Game::selectedSlot + 1 + HOTBAR_SIZE) % HOTBAR_SIZE);
    else if (cl::JustPressed<cl::KEY_UP>())
      SelectItem((Game::selectedSlot + HOTBAR_SIZE - 1) % HOTBAR_SIZE);
  }
  
  // use item
  else if (cl::Pressed('Z')) {
    if (dirX != 0 || dirY != 0)
      UseItem(v2s(dirX, dirY));
  }

  // move by timer
  else if (moveTimer.Check(false)) {
    TryMove(dirX, 0);
    TryMove(0, dirY);
    moveTimer.Reset();
  }

  // go to inventory
  if (cl::JustPressed<'C'>())
    Scene::Switch("Inventory");

  // follow by the camera and light
  area->camera = pos;
  light->pos = pos;
}

// select an item
void Player::SelectItem(int slot) {
  Game::selectedSlot = slot;
  if (Game::selectedSlot >= Game::inventory.size())
    Game::selectedSlot = 0;
  Info::Item& item = Info::items[Game::inventory[Game::selectedSlot].first];
  useTimer.max = item.useTimer;
  useTimer.Reset();
}

// on death
void Player::Death(void) {
  Game::gameOver = true;
  Scene::Switch("Menu");
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
  uchar itemId = Game::inventory[Game::selectedSlot].first;
  Info::Item& item = Info::items[itemId];
  uchar& itemCount = Game::inventory[Game::selectedSlot].second;

  // check if the item is available
  if (itemCount <= 0)
    return;

  // item is wand
  if (item.type == 1)
    area->EntityAdd(new Projectile(area, pos + dir, dir, this, item));
  
  // item is block
  if (item.type == 2) {
    v2s targetPos = pos + dir;
    Block& slot = area->blocks[targetPos.x + targetPos.y * area->size.x];
    if (Info::blocks[slot.id].entCol || Info::blocks[slot.id].projCol)
      return;
    area->BlockPut(Info::Block::ids[Info::items[itemId].name], targetPos.x, targetPos.y);
    Game::RemoveItem(itemId);
  }

  // item is food
  if (item.type == 3) {
    area->player->health += item.food;
    if (area->player->health > area->player->maxHealth)
      area->player->health = area->player->maxHealth;
    Game::RemoveItem(itemId);
  }
}

// projectile death
void Projectile::Death(void) {
  area->LightRemove(light);
  area->EntityRemove(this);
}

// behaviour
void Projectile::Behave(void)
{
  // check if
  if (area->Inside(pos) && Info::blocks[area->blocks[pos.x + pos.y * area->size.x].id].projCol) {
    if (players)
      area->BlockDamage(pos, health);
    else if (area->blocks[pos.x + pos.y * area->size.x].hp <= health * 2)
      area->BlockDamage(pos, health);
    Death();
    return;
  }

  // remove if out of power
  else if (power <= 0) {
    area->LightRemove(light);
    Death();
    return;
  }

  // check all the entities
  for (auto& entity : area->entities) {
    if (entity->pos == pos && Info::entities[entity->id].collidable && entity.get() != owner) {
      entity->ApplyDamage(health);
      Death();
      return;
    }
  }
  
  // only odd ticks
  if (ut::ticks % 2)
    return;
    
  // move
  pos = pos + direction;
  light->pos = pos;
  light->scalar = std::min(power / 3, 10);
  power--;
}

// drop behaviour
void Drop::Behave(void) {
  if (pos == area->player->pos) {
    for (int i = 0; i < drops.size(); i++)
      Game::AddItem(drops[i]);
    area->EntityRemove(this);
  }
}

// drop render
void Drop::Render(int sx, int sy) {
  Info::Entity& info = Info::entities[id];
  cl::Put(info.character, sx, sy, info.fgcolor + (ut::ticks / 4 % 2 ? 0 : 8));
}

// projectile constructor
Projectile::Projectile(Area* area, v2s pos, v2s dir, Entity* own, Info::Item& wand)  :
  Entity(area, pos, Info::Entity::ids["projectile"]),
  owner(own),
  power(wand.wandPower),
  direction(dir) {
  health = wand.wandDamage;
  players = wand.wandProjs;
  light = area->LightCreate(pos, 0);
}

// enemy constructor
Enemy::Enemy(Area* area, v2s pos, string name)  :
  Entity(area, pos, Info::Entity::ids[name]),
  moveTimer(8),
  attackTimer(name == "priest" ? 64 : 16) {
  Info::Entity& info = Info::entities[id];
  health = info.hp;
  moveTimer = ut::Timer(info.speed);
}

// behaviour
void Enemy::Behave(void)
{
  // remove if too far
  
  int dist = 64 * 64;
  if (area->time < DAY_LENGTH / 2)
    dist = 32 * 32;
  if ((pos - area->player->pos).LengthSq() > dist) {
    area->EntityRemove(this);
    return;
  }

  // info
  Info::Entity& info = Info::entities[id];
  v2s dir = (area->player->pos - pos).Normalize();

  // try to move toward player
  if (moveTimer.Check())
  {
    // move
    if (info.name == "demon" || info.name == "priest") {
      TryMove(dir.x, 0);
      TryMove(0, dir.y);
      if (Info::blocks[area->blocks[pos.x + pos.y * area->size.x].id].name == "peaks")
        ApplyDamage(2);
      moveTimer.Reset();
    }

    // passive
    if (info.name == "dog" || info.name == "sheep") {
      if (area->rand.Chance(0.04)) {
        dir = area->rand.SqrPos(1);
        TryMove(dir.x, 0);
        TryMove(0, dir.y);
        moveTimer.Reset();
      }
    }
  }

  // try to attack
  if (attackTimer.Check())
  {
    // melee attack
    if (info.name == "demon") {
      if (area->player->pos == pos + dir) {
        area->player->ApplyDamage(info.damage);
        attackTimer.Reset();
      }
    }

    // ranged attack
    if (info.name == "priest") {
      area->EntityAdd(new Projectile(area, pos, dir, this, Info::items[Info::Item::ids["priestwand"]]));
      attackTimer.Reset();
    }
  }
}

// death
void Enemy::Death(void) {
  if (Info::entities[id].drop != "")
    area->EntityAdd(new Drop(area, pos, Info::entities[id].drop));
  area->EntityRemove(this);
}