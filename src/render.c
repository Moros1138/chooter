#include "chooter.h"

void render_clear(State *S) {
  SDL_SetRenderDrawColor(S->ren, 32, 32, 32, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(S->ren);
}

void render_display(State *S) { SDL_RenderPresent(S->ren); }

SDL_Texture *load_texture(State *S, const char *path) {
  SDL_assert(path);
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                 "\nloading image \"%s\"\n", path);

  SDL_Texture *t = IMG_LoadTexture(S->ren, path);

  if (t == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "could not load image \"%s\": %s\n",
                 path, IMG_GetError());
  }

  return t;
}

void render_texture(State *S, SDL_Texture *texture, float x, float y) {
  SDL_FRect dst;

  dst.x = x;
  dst.y = y;

  int w, h;

  SDL_QueryTexture(texture, NULL, NULL, &w, &h);

  dst.w = w;
  dst.h = h;

  SDL_RenderCopyF(S->ren, texture, NULL, &dst);
}
