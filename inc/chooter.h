#ifndef CHOOTER_H_
#define CHOOTER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <sodium.h>
#include <stdbool.h>

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

#define FPS 60

#define PLAYER_SPD 4
#define PLAYER_BULLET_SPD 16
#define ENEMY_BULLET_SPD 8

typedef struct State State;
typedef struct Arena Arena;

typedef void (*delegate_fn)(State *, Arena *);

enum GameKey {
    GK_UP = 1 << 0,
    GK_DOWN = 1 << 1,
    GK_RIGHT = 1 << 2,
    GK_LEFT = 1 << 3,
    GK_FIRE = 1 << 4,
};

enum Side { SD_PLAYER, SD_ENEMY };

typedef struct {
    delegate_fn logic;
    delegate_fn render;
} Delegate;

/**
 * This structure represents an entity in the game.
 * Entities are objects like the player, the enemies, the bullets, etc.
 */
typedef struct Thing {
    int reload;
    unsigned int hp;
    enum Side side;
    SDL_FPoint delta;
    SDL_FRect r;
    SDL_Texture *tex;
    struct Thing *next;
} Thing;

/**
 * This structure holds information about the aircrafts and their bullets
 * in the arena current arena they are playing in.
 */
struct Arena {
    Thing aircraft_head;
    Thing bullet_head;
    Thing *aircraft_tail;
    Thing *bullet_tail;
};

/**
 * This structure contains the state of the game.
 * Being the main structure of the program, it is passed around between most of
 * the functions.
 */
struct State {
    bool running;
    enum GameKey key;
    Delegate dg;
    SDL_Renderer *ren;
    SDL_Window *win;
};

/**
 * Initialization functions
 */
bool init_sdl(State *S);
void deinit_sdl(State *S);

/**
 * Arena functions
 */
bool arena_init(State *S, Arena *A);

/**
 * Input functions
 */
void handle_input(State *S);

/**
 * Rendering functions
 */
void render_clear(State *S);
void render_display(State *S);
SDL_Texture *load_texture(State *S, const char *path);
void render_texture(State *S, SDL_Texture *texture, float x, float y);

/**
 * Utility functions
 */
void set_thing_size(Thing *T);
const char *get_res_path(void);
bool are_colliding(Thing *a, Thing *b);
void find_slope(float x1, float y1, float x2, float y2, float *dx, float *dy);

// returns a random integer N where a <= N <= b
static inline int randint(const uint32_t a, const uint32_t b) {
    return a + randombytes_uniform(b - a + 1);
}

#endif // CHOOTER_H_
