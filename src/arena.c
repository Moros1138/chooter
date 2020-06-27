#include <stdlib.h>
#include <string.h>

#include "chooter.h"

enum { PLAYER, ENEMY, P_BULLET, E_BULLET, LEN };

static int enemy_spawn_rate;
static Thing *player;
static SDL_Texture *textures[LEN];

static void fire_bullet(Arena *A) {
  Thing *bullet = calloc(1, sizeof(Thing));

  A->bullet_tail->next = bullet;
  A->bullet_tail = bullet;

  bullet->r.x = player->r.x;
  bullet->r.y = player->r.y;
  bullet->delta.x = PLAYER_BULLET_SPD;
  bullet->hp = 1;
  bullet->tex = textures[P_BULLET];
  set_thing_size(bullet);
  bullet->side = SD_PLAYER;

  bullet->r.y += player->r.h * 0.5 - bullet->r.h * 0.5;
  player->reload = 8; // The player can fire once every 8 frames.
}

static bool aircraft_got_shot(Arena *A, Thing *bullet) {
  for (Thing *enemy = A->aircraft_head.next; enemy; enemy = enemy->next) {
    if (enemy->side != bullet->side && are_colliding(bullet, enemy)) {
      bullet->hp = 0;
      enemy->hp = 0;

      return true;
    }
  }
  return false;
}

static void perform_player(State *S, Arena *A) {
  player->delta.x = 0;
  player->delta.y = 0;

  if (player->reload > 0) {
    --player->reload;
  }

  if (S->key & GK_UP) {
    player->delta.y = -PLAYER_SPD;
  }
  if (S->key & GK_DOWN) {
    player->delta.y = PLAYER_SPD;
  }
  if (S->key & GK_RIGHT) {
    player->delta.x = PLAYER_SPD;
  }
  if (S->key & GK_LEFT) {
    player->delta.x = -PLAYER_SPD;
  }
  if (S->key & GK_FIRE && player->reload == 0) {
    fire_bullet(A);
  }
}

static void perform_bullets(Arena *A) {
  for (Thing *prev = &A->bullet_head, *b = A->bullet_head.next; b;
       prev = b, b = b->next) {
    b->r.x += b->delta.x;
    b->r.y += b->delta.y;

    if (b->r.x > WIN_WIDTH || aircraft_got_shot(A, b)) {
      // Is it the last bullet in the list?
      if (b == A->bullet_tail) {
        A->bullet_tail = prev;
      }

      // Remove the bullet from the list.
      prev->next = b->next;
      free(b);
      b = prev;
    }
  }
}

static void perform_aircrafts(Arena *A) {
  for (Thing *prev = &A->aircraft_head, *e = A->aircraft_head.next; e;
       prev = e, e = e->next) {
    e->r.x += e->delta.x;
    e->r.y += e->delta.y;

    if (e != player && (e->r.x < -e->r.w || e->hp == 0)) {
      if (e == A->aircraft_tail) {
        A->aircraft_tail = prev;
      }

      prev->next = e->next;
      free(e);
      e = prev;
    }
  }
}

static void spawn_enemies(Arena *A) {
  Thing *enemy = calloc(1, sizeof(Thing));

  A->aircraft_tail->next = enemy;
  A->aircraft_tail = enemy;

  enemy->r.x = WIN_WIDTH;
  enemy->r.y = randombytes_uniform(WIN_HEIGHT);
  enemy->tex = textures[ENEMY];
  set_thing_size(enemy);
  enemy->side = SD_ENEMY;
  enemy->hp = 1;
  enemy->delta.x = -randint(2, 5);
  // A new enemy is spawned every 500-1500ms.
  enemy_spawn_rate = randint(30, 89);
}

static void logic(State *S, Arena *A) {
  perform_player(S, A);
  perform_aircrafts(A);
  perform_bullets(A);

  if (--enemy_spawn_rate < 1) {
    spawn_enemies(A);
  }
}

static void render(State *S, Arena *A) {
  for (Thing *aircraft = A->aircraft_head.next; aircraft;
       aircraft = aircraft->next) {
    render_texture(S, aircraft->tex, aircraft->r.x, aircraft->r.y);
  }
  for (Thing *b = A->bullet_head.next; b; b = b->next) {
    render_texture(S, b->tex, b->r.x, b->r.y);
  }
}

static bool init_player(State *S, Arena *A) {
  if ((player = calloc(1, sizeof(Thing))) == NULL) {
    return false;
  }

  A->aircraft_tail->next = player;
  A->aircraft_tail = player;

  player->r.x = 100;
  player->r.y = 100;
  player->tex = load_texture(S, "res/img/player.png");
  set_thing_size(player);
  player->side = SD_PLAYER;

  return true;
}

bool arena_init(State *S, Arena *A) {
  S->dg.logic = logic;
  S->dg.render = render;

  memset(A, 0, sizeof(Arena));
  A->aircraft_tail = &A->aircraft_head;
  A->bullet_tail = &A->bullet_head;

  if (!init_player(S, A)) {
    return false;
  }

  // Precache the textures, to avoid loading them each time.
  // textures[PLAYER] = load_texture(S, "res/img/player.png");
  textures[ENEMY] = load_texture(S, "res/img/enemy.png");
  textures[P_BULLET] = load_texture(S, "res/img/player_bullet.png");

  // Actually unnecessary (global variables are initialized to 0 by default),
  // but I like it, it's consistent.
  enemy_spawn_rate = 0;

  return true;
}
