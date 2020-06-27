#include "chooter.h"

// TODO: refactor me?
static void handle_key(State *S, SDL_Event *e) {
  if (e->type == SDL_KEYDOWN) {
    if (e->key.keysym.scancode == SDL_SCANCODE_W) {
      S->key |= GK_UP;
    }
    if (e->key.keysym.scancode == SDL_SCANCODE_S) {
      S->key |= GK_DOWN;
    }
    if (e->key.keysym.scancode == SDL_SCANCODE_A) {
      S->key |= GK_LEFT;
    }
    if (e->key.keysym.scancode == SDL_SCANCODE_D) {
      S->key |= GK_RIGHT;
    }
    if (e->key.keysym.scancode == SDL_SCANCODE_SPACE) {
      S->key |= GK_FIRE;
    }
  } else {
    if (e->key.keysym.scancode == SDL_SCANCODE_W) {
      S->key &= ~GK_UP;
    }
    if (e->key.keysym.scancode == SDL_SCANCODE_S) {
      S->key &= ~GK_DOWN;
    }
    if (e->key.keysym.scancode == SDL_SCANCODE_A) {
      S->key &= ~GK_LEFT;
    }
    if (e->key.keysym.scancode == SDL_SCANCODE_D) {
      S->key &= ~GK_RIGHT;
    }
    if (e->key.keysym.scancode == SDL_SCANCODE_SPACE) {
      S->key &= ~GK_FIRE;
    }
  }
}

void handle_input(State *S) {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      S->running = false;
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      if (!e.key.repeat) {
        handle_key(S, &e);
      }
      break;
    default:
      break;
    }
  }
}
