#include <string.h>

#include "chooter.h"

void set_thing_size(Thing *T) {
  int w = 0, h = 0;

  SDL_QueryTexture(T->tex, NULL, NULL, &w, &h);

  T->r.w = w;
  T->r.h = h;
}

const char *get_res_path(void) {
  // This will hold the base resource path: chooter/res/.
  // It has static lifetime so only one call to SDL_GetBasePath() is needed.
  static char *base_res;

  if (base_res == NULL) {
    char *base_path = SDL_GetBasePath();

    if (base_path == NULL) {
      fprintf(stderr, "could not get resource path: %s\n", SDL_GetError());
      return "";
    }

    base_res = base_path;
    SDL_free(base_path);

    // Replace the last "bin" with "res" to get the the resource path.
    char *s = strstr(base_res, "bin");
    const size_t pos = s - base_res;

    base_res[pos] = 'r';
    base_res[pos + 1] = 'e';
    base_res[pos + 2] = 's';
  }

  return base_res;
}
