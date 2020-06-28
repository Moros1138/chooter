#include <stdlib.h>
#include <string.h>

#include "chooter.h"

enum { PLAYER, ENEMY, P_BULLET, E_BULLET, LEN };

static int enemy_spawn_rate, arena_reset_timer;
static Thing *player;
static SDL_Texture *textures[LEN];

static bool reset_arena(Arena *A);

static void fire_bullet(Arena *A) {
    Thing *bullet = calloc(1, sizeof(Thing));

    A->bullet_tail->next = bullet;
    A->bullet_tail = bullet;

    bullet->r.x = player->r.x;
    bullet->r.y = player->r.y;
    bullet->delta.x = PLAYER_BULLET_SPD;
    bullet->hp = 1;
    bullet->tex = textures[P_BULLET];
    bullet->side = player->side;
    set_thing_size(bullet);
    bullet->r.y += (player->r.h * 0.5) - (bullet->r.h * 0.5);
    bullet->side = SD_PLAYER;
    player->reload = 8; // The player can fire once every 8 frames.
}

static void fire_enemy_bullet(Arena *A, Thing *T) {
    Thing *bullet = calloc(1, sizeof(Thing));

    A->bullet_tail->next = bullet;
    A->bullet_tail = bullet;

    bullet->r.x = T->r.x;
    bullet->r.y = T->r.y;
    bullet->hp = 1;
    bullet->tex = textures[E_BULLET];
    bullet->side = T->side;
    set_thing_size(bullet);

    bullet->r.x += (T->r.w * 0.5) - (bullet->r.w * 0.5);
    bullet->r.y += (T->r.h * 0.5) - (bullet->r.h * 0.5);

    find_slope(player->r.x + player->r.w * 0.5, player->r.y + player->r.h * 0.5,
               T->r.x, T->r.y, &bullet->delta.x, &bullet->delta.y);

    bullet->delta.x *= ENEMY_BULLET_SPD;
    bullet->delta.y *= ENEMY_BULLET_SPD;

    // Make sure the bullet will only hit the player,
    // skipping the issuing enemy as well as any others it may happen to collide
    // with.
    bullet->side = SD_ENEMY;

    // The enemy may fire again anytime within the next two seconds.
    T->reload = randombytes_uniform(FPS) * 2;
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
    if (S->key & GK_FIRE && player->reload < 1) {
        fire_bullet(A);
    }
}

static void bound_player(void) {
    if (player->r.x < 0) {
        player->r.x = 0;
    } else if (player->r.x > WIN_WIDTH * 0.5) {
        player->r.x = WIN_WIDTH * 0.5;
    }

    if (player->r.y < 0) {
        player->r.y = 0;
    } else if (player->r.y > WIN_HEIGHT - player->r.h) {
        player->r.y = WIN_HEIGHT - player->r.h;
    }
}

static void perform_bullets(Arena *A) {
    for (Thing *prev = &A->bullet_head, *b = A->bullet_head.next; b;
         prev = b, b = b->next) {
        b->r.x += b->delta.x;
        b->r.y += b->delta.y;

        if (aircraft_got_shot(A, b) || b->r.x < -b->r.w || b->r.x > WIN_WIDTH ||
            b->r.y < -b->r.h || b->r.y > WIN_HEIGHT) {
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
    // ac = aircraft, I'm lazy - shoot me
    for (Thing *prev = &A->aircraft_head, *ac = A->aircraft_head.next; ac;
         prev = ac, ac = ac->next) {
        ac->r.x += ac->delta.x;
        ac->r.y += ac->delta.y;

        if (ac != player && ac->r.x < -ac->r.w) {
            ac->hp = 0;
        }

        if (ac->hp == 0) {
            if (ac == player) {
                player = NULL;
            }

            if (ac == A->aircraft_tail) {
                A->aircraft_tail = prev;
            }

            prev->next = ac->next;
            free(ac);
            ac = prev;
        }
    }
}

static void perform_enemies(Arena *A) {
    for (Thing *enemy = A->aircraft_head.next; enemy; enemy = enemy->next) {
        if (enemy != player && player && --enemy->reload < 1) {
            fire_enemy_bullet(A, enemy);
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
    enemy->delta.x = -randint(2, 5);
    enemy->side = SD_ENEMY;
    enemy->hp = 1;
    // Set the enemy's reload to 1 - 3 seconds, in order to give the player a
    // chance to destroy it.
    enemy->reload = (1 + randombytes_uniform(3)) * FPS;
    // A new enemy is spawned every 500-1500ms.
    enemy_spawn_rate = 30 + randombytes_uniform(FPS);
}

static void logic(State *S, Arena *A) {
    if (player) {
        perform_player(S, A);
    }

    perform_enemies(A);
    perform_aircrafts(A);
    perform_bullets(A);

    if (--enemy_spawn_rate < 1) {
        spawn_enemies(A);
    }

    if (player) {
        bound_player();
    }

    if (player == NULL && --arena_reset_timer < 1) {
        reset_arena(A);
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

static bool init_player(Arena *A) {
    if ((player = calloc(1, sizeof(Thing))) == NULL) {
        return false;
    }

    A->aircraft_tail->next = player;
    A->aircraft_tail = player;

    player->r.x = 100;
    player->r.y = 100;
    player->tex = textures[PLAYER];
    set_thing_size(player);
    player->side = SD_PLAYER;
    player->hp = 1;

    return true;
}

static bool reset_arena(Arena *A) {
    while (A->aircraft_head.next) {
        Thing *tmp = A->aircraft_head.next;
        A->aircraft_head.next = tmp->next;
        free(tmp);
    }

    while (A->bullet_head.next) {
        Thing *tmp = A->bullet_head.next;
        A->bullet_head.next = tmp->next;
        free(tmp);
    }

    memset(A, 0, sizeof(Arena));
    A->aircraft_tail = &A->aircraft_head;
    A->bullet_tail = &A->bullet_head;

    if (!init_player(A)) {
        return false;
    }

    // Actually unnecessary (global variables are initialized to 0 by default),
    // but I like it, it's consistent.
    enemy_spawn_rate = 0;

    arena_reset_timer = FPS * 2;

    return true;
}

bool arena_init(State *S, Arena *A) {
    S->dg.logic = logic;
    S->dg.render = render;

    memset(A, 0, sizeof(Arena));
    A->aircraft_tail = &A->aircraft_head;
    A->bullet_tail = &A->bullet_head;

    // Precache the textures, to avoid loading them each time.
    textures[PLAYER] = load_texture(S, "res/img/player.png");
    textures[ENEMY] = load_texture(S, "res/img/enemy.png");
    textures[P_BULLET] = load_texture(S, "res/img/player_bullet.png");
    textures[E_BULLET] = load_texture(S, "res/img/enemy_bullet.png");

    return reset_arena(A);
}
