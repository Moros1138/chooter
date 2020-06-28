#include <math.h>
#include <string.h>

#include "chooter.h"

#ifdef max
#undef max
#endif

#define max(a, b) (((a) > (b)) ? (a) : (b))

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

bool are_colliding(Thing *a, Thing *b) {
  SDL_Rect a_r = {a->r.x, a->r.y, a->r.w, a->r.h},
           b_r = {b->r.x, b->r.y, b->r.w, b->r.h};

  return SDL_HasIntersection(&a_r, &b_r);
}

void find_slope(const SDL_FPoint src, const SDL_FPoint dst, SDL_FPoint *out) {
  float steps = max(fabsf(src.x - dst.x), fabsf(src.y - dst.y));

  if (steps == 0) {
    out->x = 0;
    out->y = 0;
  } else {
    out->x = src.x - dst.x / steps;
    out->y = src.y - dst.y / steps;
  }
}
